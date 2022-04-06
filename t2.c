/**
 * UNB - Brasilia - Brasil
 * FSO - 2021/2 - Trabalho 2

 * Andre Larrosa Chimpliganond - 19/0010321
 * Gustavo Tomas de Paula - 19/0014148
 * Rafael Henrique Nogalha de Lima - 19/0036966
 * Tong Zhou - 19/0038764
**/

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

// DEFINES
#define N 5                 // Numero de filhos
#define UPPER 180           // Tempo maximo
#define LOWER 60            // Tempo minimo
#define TIME 50000000000    // Tempo de execucao do busy waiting

// Tempos dos processos filhos
typedef struct {
    time_t tempo_inicio;
    time_t tempo_final;
    time_t tempo_randomico;
    int pid_filho;
} filho_tempo;

// Struct dos filhos
filho_tempo ft[N];

// Ordem crescente
int cmpfunc(const void *a, const void *b)
{
    filho_tempo *ftA = (filho_tempo *)a;
    filho_tempo *ftB = (filho_tempo *)b;
    return (ftA->tempo_randomico - ftB->tempo_randomico);
}

int pid_pai;
int filho_i;
int contador;

// Libera o filho quando o busy waiting acabar
void termina_execucao(int i)
{
    for (long k = 0; k < TIME; k++);
    printf("Pid do filho: %d\n", getpid());
    exit(i);
}

// Funcao que contam o tempo entre os processos
void funcao_para_acordar(int signal)
{
    int tempo_entre_os_filhos = 0;
    int filho_anterior;
    int filho_depois;
    
    // Enquanto o tempo entre os processos filhos for igual a zero, continua
    while (tempo_entre_os_filhos == 0)
    {
        filho_depois = filho_i;
        
        // SIGCONT 19 - Continua a execucao do processo depois do SIGSTOP
        kill(ft[filho_depois++].pid_filho, SIGCONT);
        
        // Se a posicao do filho atual for a ultima, da o break
        if (filho_i == N)
        {
            break;
        }
        filho_anterior = filho_i - 1;
        tempo_entre_os_filhos = ft[filho_i].tempo_randomico - ft[filho_anterior].tempo_randomico;
        filho_i++;
    } 

    alarm(tempo_entre_os_filhos);
}

int main() 
{
    pid_pai = getpid();
    int pid = 1, idfila;
    srand(time(NULL));

    for (int i = 0; i < N && pid != 0; i++)
    {
        // Gera um tempo aleatorio
        int tempo = (rand() % (UPPER - LOWER + 1)) + LOWER;
        if ((pid = fork()) < 0)
            printf("ERRO: falha na criacao do filho\n");

        // Marca o tempo de inicio
        ft[i].tempo_inicio = time(0); 

        // Se for filho, vai terminar a execucao
        if (pid == 0)
        {
            termina_execucao(i); 
            printf("isso aqui nao eh pra aparecer :(\n");
        }
        
        // Se for o pai, espera os filhos
        kill(pid, SIGTSTP);

        printf("I: %d - PID: %d - TEMPO_I: %d\n", i, pid, tempo);
        
        ft[i].pid_filho = pid;
        ft[i].tempo_randomico = tempo;
    }

    // Ordena o vetor de filhos de acordo com o valor dos tempos
    qsort(ft, N, sizeof(filho_tempo), cmpfunc);

    // SIGALRM 14 alarm clock
    signal(SIGALRM, funcao_para_acordar);

    // Marca um alarme em segundos com o tempo do processo de menor tempo
    alarm(ft[0].tempo_randomico);

    // Variavel para o status do filho 
    int status_filho;

    // Espera todos os filhos acabarem
    contador = 0;
    while (contador < N)
    {
        // Monitora o status do processo filho
        if(waitpid(ft[contador].pid_filho, &status_filho, 0) < 0)
        {
            printf("ERRO: falha na espera do filho\n");
            exit(1);
        } 
        else 
        {
            ft[contador].tempo_final = time(0);
            time_t turnaround_time = ft[contador].tempo_final - ft[contador].tempo_inicio;
           
            // Retorna verdadeiro se o filho terminou corretamente
            if (WIFEXITED(status_filho))
            {
                printf("Tempo de turnaround do filho %d foi %ld\n", ft[contador].pid_filho , turnaround_time);
            }
            else
            {
                printf("Filho %d nao terminou corretamente\n", ft[contador].pid_filho);
                exit(1);
            }
        }
        contador++;
    }

    return 0;
}

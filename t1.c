/**
 * UNB - Brasilia - Brasil
 * FSO - 2021/2 - Trabalho 1

 * Andre Larrosa Chimpliganond - 19/0010321
 * Gustavo Tomas de Paula - 19/0014148
 * Rafael Henrique Nogalha de Lima - 19/0036966
 * Tong Zhou - 19/0038764
**/

// bibliotecas
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include <string.h>

#define N 10 // numero de filhos

// estrutura mensagem
int pid_msg;
struct mensagem
{
    long pid_msg;
    char msg[30];
};

// globais
int idsem, idfila_pai, idfila_filhos;
struct mensagem mensagem_env, mensagem_rec;

// operacao do semaforo
struct sembuf operacao[2];

// semaforo up
int p_sem()
{
    operacao[0].sem_num = 0;
    operacao[0].sem_op  = 0;
    operacao[0].sem_flg = 0;
    operacao[1].sem_num = 0;
    operacao[1].sem_op  = 1;
    operacao[1].sem_flg = 0;
    if (semop(idsem, operacao, 2) < 0)
        printf("erro no p = %d\n", errno);
}

// semaforo down
int v_sem()
{
    operacao[0].sem_num = 0;
    operacao[0].sem_op = -1;
    operacao[0].sem_flg = 0;
    if (semop(idsem, operacao, 1) < 0)
        printf("erro no p = %d\n", errno);
}

// programa principal
int main()
{
    int i, pid = 1, estado;

    // cria semaforo
    if ((idsem = semget(0x1234, 1, IPC_CREAT | 0x1ff)) < 0)
    {
        perror("main: semget");
        exit(1);
    }

    // cria fila de mensagem para avisar processo pai
    if ((idfila_pai = msgget(0x1223, IPC_CREAT|0x1B6)) < 0)
    {
        printf("erro na criacao da fila\n");
        exit(1);
    }

    // cria fila de mensagem para avisar processos filhos
    if ((idfila_filhos = msgget(0x1224, IPC_CREAT|0x1B6)) < 0)
    {
        printf("erro na criacao da fila\n");
        exit(1);
    }

    // criacao dos processos filhos
    for (i = 0; i < N && pid != 0; i++)
        if ((pid = fork()) < 0)
            printf("ERRO: falha na criacao do filho\n");

    // filho
    if (pid == 0)
    {
        p_sem();    // lock
            int filho_pid = getpid();
            printf("sou o processo filho, pid = %d\n", filho_pid);
            // filho avisa o pai
            mensagem_env.pid_msg = filho_pid;
            strcpy(mensagem_env.msg, "filhos terminaram");
            msgsnd(idfila_pai, &mensagem_env, sizeof(mensagem_env) - sizeof(long), 0);
        v_sem();    // unlock

        // filhos esperam a liberacao do pai
        msgrcv(idfila_filhos, &mensagem_rec, sizeof(mensagem_rec) - sizeof(long), 0, 0);

        // filhos terminam a execucao
        exit(0);
    }
    // pai espera os filhos executarem o print
    // printf("pai esperando o print dos filhos\n");   // descomentar para debug
    for (int i = 0; i < N; i++)
        msgrcv(idfila_pai, &mensagem_rec, sizeof(mensagem_rec) - sizeof(long), 0, 0);
    
    // pai envia mensagem de liberacao para cada filho
    mensagem_env.pid_msg = getpid();
    strcpy(mensagem_env.msg, "pai liberou");
    for (i = 0; i < N; i++)
        msgsnd(idfila_filhos, &mensagem_env, sizeof(mensagem_env) - sizeof(long), 0);

    // pai espera por todos os processos filhos terminarem, entao encerra
    while (wait(&estado) > 0);
    // printf("filhos terminaram\n"); // descomentar para debug

    // deletar semaforo
    if (semctl(idsem, 0, IPC_RMID) < 0)
        printf("ERRO ao deletar o semaforo\n");

    // deletar filas
    struct msqid_ds *buf;
    
    if (msgctl(idfila_filhos, IPC_RMID, buf) < 0) 
        printf("ERRO ao deletar a fila dos filhos\n");

    if (msgctl(idfila_pai, IPC_RMID, buf) < 0)
        printf("ERRO ao deletar a fila do pai\n");

    return 0;
}
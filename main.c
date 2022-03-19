#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/sem.h>
// #include "semaforo.h"

#define N 2    // numero de filhos
#define P 1     // numero de pai

void filho_func();
void pai_func();

struct sembuf operacao[2];

int idsem_pai;
int idsem_filho;
int idsem_lock;

int cont = 0;

union semun {
    int val;
    struct semid_ds *buf;
    ushort * array;
} arg;

union semun arg; // argumento para semctl

// semaforo
void p_sem(int idsem)
{
    int r;
    printf("down 1\n");
    operacao[0].sem_num = 0;
    printf("down 2\n");
    operacao[0].sem_op = 0;
    printf("down 3\n");
    operacao[0].sem_flg = 0;
    printf("down 4\n");
    operacao[1].sem_num = 0;
    printf("down 5\n");
    operacao[1].sem_op = 1;
    printf("down 6\n");
    operacao[1].sem_flg = 0;
    printf("down 7\n");
    r = semop(idsem, operacao, 2);
    printf("SEM OP: %d\n", r);
    if (r < 0)
        printf("erro no p = %d\n", errno);
    else
        printf("down 8\n");
}

void v_sem(int idsem)
{
    operacao[0].sem_num = 0;
    operacao[0].sem_op = -1;
    operacao[0].sem_flg = 0;
    if (semop(idsem, operacao, 1) < 0)
        printf("erro no p = %d\n", errno);
}

void filho_func()
{
    printf("sou o processo filho, pid = %d\n", getpid());
    // inicio regiao critica
    // sem_wait(&lock);
    p_sem(idsem_lock);
        cont++;
        printf("cont: %d\n", cont);
        if (cont == N)          // se todos os filhos 
            // sem_post(&pai);     // avisa o pai
            v_sem(idsem_pai);
    // sem_post(&lock);
    v_sem(idsem_lock);
    // fim regiao critica
    printf("eu sou um filho e estou pronto para matar\n");
    // sem_wait(&filho);   // espera os outros filhos
    p_sem(idsem_filho);
}

void pai_func() 
{    
    printf("sou o processo pai, pid = %d\n", getpid());
    int i, pid;
    printf("PAI FUNC 1\n");
    p_sem(idsem_pai); // espera os filhos
    printf("PAI FUNC 2\n");

    for (i = 0; i < N; i++)
        // sem_post(&filho);
        v_sem(idsem_filho);
    printf("eu sou o pai e terminei de contar meus filhos\n");
    return;
}

int main()
{
    int estado, idshm;
   
    // idsem_pai = create_sem(0);      // cria semaforo do pai
    // idsem_filho = create_sem(1);    // cria semaforo do filho
    // idsem_lock = create_sem(1);     // cria um lock
    
    if ((idsem_pai = semget(0x1111, 1, IPC_CREAT|0x1ff)) < 0)
    {
        printf("erro na criacao da semaforo 1\n");
        exit(1);
    }

    if ((idsem_filho = semget(0x1111, 1, IPC_CREAT|0x1ff)) < 0)
    {
        printf("erro na criacao da semaforo 2\n");
        exit(1);
    }

    if ((idsem_lock = semget(0x1111, 1, IPC_CREAT|0x1ff)) < 0)
    {
        printf("erro na criacao de semaforo 3\n");
        exit(1);
    }

    /* cria memoria compartilhada */
    if ((idshm = shmget(0x555, sizeof(int), IPC_CREAT|0x1ff)) < 0)
    {
        printf("erro na criacao da memoria compartilhada\n");
        exit(1);
    }

    int pid = getpid();
    printf("ppid = %d\n", pid);

    // cria 10 filhos geneticamente identicos
    for (int i = 0; i < N && pid != 0; i++)
    {
        // ocorreu uma catastrofe do maior escalao
        if ((pid = fork()) < 0)
        {
            printf("si-fuw-deu kkkk\n");
            exit(1); 
        }
    }

    // se for um filho recem formado, imprime o pid e avisa o pai
    if (pid == 0)  
        filho_func();
    else
        pai_func();
    
    // doce liberdade
    printf("%d acabou!\n", getpid());

    int ret;
    
    ret = semctl(idsem_pai, 0, IPC_RMID, arg);
    if (ret == 0)
        printf("semaforo removido\n");
    ret = shmctl(idshm, IPC_RMID, (struct shmid_ds *) NULL);
    if (ret == 0)
        printf("memoria removida\n");
    exit (0);
    
    return 0;
}

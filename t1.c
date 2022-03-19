#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h>

struct sembuf operacao[2];
int idsem;

void p_sem(i)
int i;
{
    printf("PSEM %d\n", i);
    operacao[0].sem_num = i;
    operacao[0].sem_op = 0;
    operacao[0].sem_flg = 0;
    operacao[1].sem_num = i;
    operacao[1].sem_op = 1;
    operacao[1].sem_flg = 0;
    if (semop(idsem, operacao, 2) < 0)
        printf("erro no p=%d\n", errno);
}
void v_sem(i)
int i;
{
    printf("VSEM\n");
    operacao[0].sem_num = i;
    operacao[0].sem_op = -1;
    operacao[0].sem_flg = 0;
    if (semop(idsem, operacao, 1) < 0)
        printf("erro no v=%d\n", errno);
}

union semun {
    int val;
    struct semid_ds *buf;
    ushort * array;
} arg;

union semun arg; // argumento para semctl

struct shmid_ds shmid_ds;

int main()
{
    int pid, idshm,  estado, i, ret;
    struct shmid_ds buf;
    int *pshm;
    printf("1\n");
    /* cria semaforo*/
    if ((idsem = semget(0x1111, 2, IPC_CREAT|0x1ff)) < 0)
    {
        printf("erro na criacao da fila\n");
        exit(1);
    }
    printf("2\n");
    /* cria memoria*/
    if ((idshm = shmget(0x555, sizeof(int), IPC_CREAT|0x1ff)) < 0)
    {
        printf("erro na criacao da fila\n");
        exit(1);
    }
    printf("3\n");
    p_sem(0);
    p_sem(1);
    printf("3.5\n");
    /* cria processo filho */
    pid = fork();
    printf("4\n");
    if (pid == 0)
    {
        /* codigo do filho */
        pshm = (int *) shmat(idshm, (char *)0, 0);
        if (pshm == (int *)-1) 
        {
            printf("erro no attach\n");
            exit(1);
        }
        for (i=0; i<20;i++)
        {
            *pshm=i;
            printf("numero escrito = %d\n", *pshm); 
            v_sem(0);
            p_sem(1);
        }
        exit(0);
    }

    /* codigo do pai */
    pshm = (int *) shmat(idshm, (char *)0, 0);
    if (pshm == (int *)-1) 
    {
        printf("erro no attach\n");
        exit(1);
    }
     
    for (i=0; i<20; i++)
    {
        p_sem(0);
        printf("numero lido = %d\n", *pshm);
        v_sem(1);
        sleep(1);
    }

    wait(&estado);
    ret = semctl(idsem, 0, IPC_RMID, arg);
    if (ret == 0)
        printf("semaforo removido\n");
    ret = shmctl(idshm, IPC_RMID, (struct shmid_ds *) NULL);
    if (ret == 0)
        printf("memoria removida\n");
    exit (0);
}
     
#include <stdio.h>
#include <errno.h>
#include <sys/sem.h>
#include <stdlib.h>
#include <sys/ipc.h>

void p_sem(struct sembuf operacao[], int idsem)
{
    operacao[0].sem_num = 0;
    operacao[0].sem_op = 0;
    operacao[0].sem_flg = 0;
    operacao[1].sem_num = 0;
    operacao[1].sem_op = 1;
    operacao[1].sem_flg = 0;
    if (semop(idsem, operacao, 2) < 0)
        printf("erro no p = %d\n", errno);
}

void v_sem(struct sembuf operacao[], int idsem)
{
    operacao[0].sem_num = 0;
    operacao[0].sem_op = -1;
    operacao[0].sem_flg = 0;
    if (semop(idsem, operacao, 1) < 0)
        printf("erro no p = %d\n", errno);
}

int create_sem(int tam)
{
    int id;
    if ((id = semget(0x1223, tam, IPC_CREAT|0x1ff)) < 0)
    {
        printf("erro na criacao da fila\n");
        exit(1);
    }
    return id;
}

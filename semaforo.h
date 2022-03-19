#ifndef _SEMAFORO_H
#define _SEMAFORO_H

void p_sem(struct sembuf operacao[], int idsem);
void v_sem(struct sembuf operacao[], int idsem);
int create_sem(int tam);

#endif // _SEMAFORO_H
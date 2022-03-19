# fso-2-21
Repositório dos trabalhos de fundamentos de sistemas operacionais - FSO 2021/2

## Trabalho 1 - Criação de Processos e Barreira de Sincronização

- Os alunos devem confeccionar um programa com 11 processos (1 processo pai e 10 processos filhos) da seguinte maneira:

- Processo pai: cria 10 processos filhos e espera o aviso de todos os filhos. Quando receber todos os avisos dos filhos, os filhos são notificados e podem terminar a execução. O processo pai só termina a execução quando tiver certeza de que todos os filhos já terminaram. 

- Processos filhos (10 processos): Imprime “sou o processo filho, pid = <pid>” e avisa o processo pai que imprimiu. Quando receber a notificação do processo pai, termina a execução. 

__Observação: devem ser usadas chamadas de sistema Unix que foram discutidas nas aulas.__

__Observação: deve ser entregue o programa fonte em C no formato .txt__



## Trabalho 2 - Execução Postergada de Processos

Os alunos devem implementar um mecanismo de execução postergada com 1 processo pai e 5 processos filhos, da seguinte maneira:

- O processo pai:

  - Para i de 1 a 5

    Begin

        Obtem um número randômico entre 60 e 180 (tempo_i)

        Cria o processo filho (filho_i) e imediatamente pára a execução do filho_i;

        Imprime i, pid e tempo_i

    End

- Para todo processo filho_i

      depois de decorrido o tempo_i, libera o filho para execução

- Para todo processo filho_i

    Begin

        espera o término do filho

        imprime o tempo de turnaround (makespan) do filho

    End

- O processo pai termina a execução somente quando tem certeza que todos os filhos terminaram. 

Cada processo filho:

   - Vai executar o seguinte código:

   

   long int i;

   `for(i=0; i<50000000000; i++);/* busy waiting com 100% de CPU – demora cerca de 80s */`

   

   - Ao término do “for”, o processo filho imprime o seu pid e termina a execução.



__Observação: devem ser usadas chamadas de sistema Unix que foram discutidas nas aulas.__

__Observação: deve ser entregue o programa fonte em C no formato .txt__


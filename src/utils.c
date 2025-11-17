#include <stdio.h>
#include <stdlib.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/sem.h>
#include "utils.h"

int get_shell_semaphore() {
    int sem_id = semget(SHELL_SEM_KEY, 1, IPC_CREAT | 0x1FF);
    if (sem_id < 0) {
        printf("\n\033[1;31mERRO!!!\033[0m Não foi possível criar o semáforo!\n");
        exit(1);
    }
    return sem_id;
}

void down_sem(int sem_id) {
    struct sembuf sem_op[1];
    sem_op[0].sem_num = 0;
    sem_op[0].sem_op = -1;
    sem_op[0].sem_flg = 0;
    if (semop(sem_id, sem_op, 1) < 0) {
        printf("\n\033[1;31mERRO!!!\033[0m Falha em uma operação do semáforo!\n");
        exit(1);
    }
}

void up_sem(int sem_id) {
    struct sembuf sem_op[1];
    sem_op[0].sem_num = 0;
    sem_op[0].sem_op = 1;
    sem_op[0].sem_flg = 0;
    if (semop(sem_id, sem_op, 1) < 0) {
        printf("\n\033[1;31mERRO!!!\033[0m Falha em uma operação do semáforo!\n");
        exit(1);
    }
}
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "scheduler.h"

static void kill_if_alive(struct process_t *p) {
    if (!p) return;

    int status;
    pid_t res = waitpid(p->pid, &status, WNOHANG);

    if (res == 0) {
        kill(p->pid, SIGKILL);
        waitpid(p->pid, &status, 0);
    }

    if (!p->finished) {
        clock_gettime(CLOCK_MONOTONIC, &p->finish_time);
        p->finished = 1;
    }
}

void exit_scheduler(void) {

    printf("\n========== ENCERRANDO ESCALONADOR ==========\n");

    /* 1 — Finalizar processo atual */
    if (current_process) {
        printf("Finalizando processo atual: %s (PID=%d)\n",
               current_process->program_name,
               current_process->pid);

        kill_if_alive(current_process);
    }

    /* 2 — Relatório final de turnaround */
    printf("\n========== RELATÓRIO FINAL ==========\n");

    if (current_process && current_process->finished) printf("Processo atual finalizado: %s — Turnaround: %.6f s\n",current_process->program_name,compute_turnaround(current_process));
   
    /* 3 — Finalizar processos das filas */
    for (int i = 0; i < num_of_queues; i++) {
        struct process_t *p = process_queues[i];

        while (p != NULL) {
            struct process_t *next = p->next;

            printf("Encerrando processo: %s (PID=%d)\n",
                   p->program_name, p->pid);

            kill_if_alive(p);

            free(p);
            p = next;
        }

        process_queues[i] = NULL;
    }

    /* 4 — Remover fila de mensagens */
    if (msqid >= 0) {
        printf("Removendo fila de mensagens...\n");
        msgctl(msqid, IPC_RMID, NULL);
    }

    /* 5 — Remover semáforo */
    if (semid >= 0) {
        printf("Removendo semáforo...\n");
        semctl(semid, 0, IPC_RMID);
    }
    printf("Encerramento concluído.\n");
    printf("=======================================\n\n");
}
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include "utils.h"

int get_scheduler_msg_queue();

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

void exit_scheduler(
    struct process_t *current_process, struct process_t *process_queues[], int num_of_queues
) {
    printf("\n========== ENCERRANDO ESCALONADOR ==========\n");
    /* 1 — Relatório final de turnaround */
    printf("\n========== RELATÓRIO FINAL ==========\n");
    
    /* 2 — Finalizar processo atual */
    if (current_process) {
        printf("Processo atual: %s (PID=%d)\n",
            current_process->program_name,
            current_process->pid
        );

        if (current_process->finished) {
            printf("  STATUS: FINALIZADO. Turnaround: %.6f s\n", 
                   current_process->turnaround);
        } 
        else{
            printf("  STATUS: PENDENTE. Encerrando o processo ativo...\n");
            kill_if_alive(current_process);
        }
    }
       
    /* 3 — Finalizar processos das filas */
    printf("\n========== PROCESSOS NAS FILAS ==========\n");
    
        for (int i = 0; i < num_of_queues; i++) {
            struct process_t *p = process_queues[i];
            
            while (p != NULL) {
                struct process_t *next = p->next;

                if (p != current_process){
                    printf("Fila %d | Proc: %s (PID %d) | ", HIGHEST_PRIORITY - i, p->program_name, p->pid);
        
                    if (p->finished) {
                        printf("STATUS: FINALIZADO. Turnaround: %.6f s\n", p->turnaround);
                    }
                    else {
                        printf("STATUS: PENDENTE. Matando...\n");
                        kill_if_alive(p);
                    }
                }

                free(p);
                p = next;
            }
            process_queues[i] = NULL;
        }

    printf("\n=======================================\n\n");

    /* 4 — Remover fila de mensagens */
    int msq_id = get_scheduler_msg_queue();
    if (msq_id >= 0) {
        printf("Removendo fila de mensagens...\n");
        msgctl(msq_id, IPC_RMID, NULL);
    }
    
    /* 5 — Remover semáforo */
    int semid = get_shell_semaphore();
    if (semid >= 0) {
        printf("Removendo semáforo...\n");
        semctl(semid, 0, IPC_RMID);
    }

    printf("\nEncerramento concluído.\n");
    printf("\n=======================================\n\n");
}
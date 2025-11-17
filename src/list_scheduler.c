#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include "utils.h"

static const char *state_of(struct process_t *current_process, struct process_t *p) {
    if (p == NULL) return "DESCONHECIDO";

    if (p->finished)
        return "FINALIZADO";

    if (current_process && current_process->pid == p->pid)
        return "EXECUTANDO";

    return "PRONTO";
}

void list_scheduler(struct process_t *current_process, struct process_t *process_queues[], int num_of_queues) {
    printf("\n========== LISTA DE PROCESSOS ==========\n");

    if (current_process) {
        printf("\nProcesso atual:\n");
        printf("  Programa: %s\n", current_process->program_name);
        printf("  PID: %d\n", current_process->pid);
        printf("  Prioridade: %d\n", current_process->priority);
        printf("  Estado: %s\n", state_of(current_process, current_process));

        if (current_process->finished) {
            printf("  Turnaround: %.6f s\n", current_process->turnaround);
        }
    } else {
        printf("\nNenhum processo está executando no momento.\n");
    }

    printf("\nFilas (%d no total):\n\n", num_of_queues);

    for (int i = 0; i < num_of_queues; i++) {
        printf("Fila de Prioridade %d:\n", HIGHEST_PRIORITY - i);

        struct process_t *p = process_queues[i];

        if (!p) {
            printf("  [Vazia]\n\n");
            continue;
        }

        while (p != NULL) {
            printf("  %s (PID = %d, prioridade = %d) — %s",
               p->program_name,
               p->pid,
               p->priority,
               state_of(current_process, p)
            );

            if (p->finished) {
                printf(" — Turnaround: %.6f s", p->turnaround);
            }

            printf("\n");
            p = p->next;
        }
        printf("\n");
    }

    printf("\n=========================================\n");
}
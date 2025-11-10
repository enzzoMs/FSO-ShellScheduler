/* ============================================================
 * TRABALHO PRATICO - FSO - 2025/2
 * ALUNOS:
 *    Enzzo Morais de Olinda - 211042720
 *    Gabriel Barros Ferreira - 190125616
 *    Layr de Andrade Neto - 222001331
 * COMPILADOR: gcc 13.3.0
 * SISTEMA OPERACIONAL: Ubuntu 24.04.1 LTS
 * ============================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "utils.h"

#define HIGHEST_PRIORITY 1

struct process_t {
  char program_name[100];
  int priority;
  struct process_t *next;
  struct process_t *previous;
};

#define SCHED_MSQ_KEY 0x4272
#define SCHED_MSG_SIZE 100

// Requisição de escalonamento. O mtext deve ser uma string no formato:
// "<nome_do_programa> <prioridade>"
struct scheduler_req_t {
  long mtype;
  char mtext[SCHED_MSG_SIZE];
};

// Até 3 filas de processos são possíveis. Cada elemento do array representa a cabeça de uma dessas filas.
// process_queues[0] -> Processos de prioridade 1
// process_queues[1] -> Processos de prioridade 0
// process_queues[2] -> Processos de prioridade -1
struct process_t *process_queues[3];

struct process_t *current_process = NULL;
int num_of_queues;

int receive_scheduling_req(int msq_id, struct scheduler_req_t *msg);
void enqueue_process(struct scheduler_req_t *msg);
int get_scheduler_msg_queue();

int main(int argc, char *argv[]) {
  int shell_sem_id = get_shell_semaphore();

  if (strcmp(argv[1], "2") == 0) {
    num_of_queues = 2;
  } else if (strcmp(argv[1], "3") == 0) {
    num_of_queues = 3;
  } else {
    printf(
      "\033[1;31mERRO!!!\033[0m Argumento inválido para 'user_scheduler'. Só é possível criar 2 ou 3 filas.\n"
    );
    up_sem(shell_sem_id);
    exit(1);
  }

  printf("\nProcesso 'user_scheduler' inicializado com %d filas.\n", num_of_queues);
  up_sem(shell_sem_id);

  int msq_id = get_scheduler_msg_queue();

  struct scheduler_req_t msg;
  while (1) {
    if (receive_scheduling_req(msq_id, &msg)) {
      enqueue_process(&msg);
    }
  }
  return 0;
}

// Recebe uma requisição de escalonamento. Retorna 0 caso não tenha recebido
// nenhuma mensagem, retorna 1 caso contrário.
int receive_scheduling_req(int msq_id, struct scheduler_req_t *msg) {
  int res = msgrcv(msq_id, msg, SCHED_MSG_SIZE, 1, 0);
  if (res == -1) {
    printf("\nError: Unable to receive scheduling request!\n");
    return 0;
  }
  return 1;
}

// Cria e enfilera um processo na sua fila de prioridade.
void enqueue_process(struct scheduler_req_t *msg) {
  char *program_name = strtok(msg->mtext, " ");
  int priority = strtol(strtok(NULL, " "), NULL, 10);

  if (priority > HIGHEST_PRIORITY || priority <= HIGHEST_PRIORITY - num_of_queues) {
    printf("Error: Invalid priority for program %s!", program_name);
    return;
  }

  struct process_t *new_process = malloc(sizeof(struct process_t));
  strcpy(new_process->program_name, program_name);
  new_process->priority = priority;
  new_process->next = NULL;
  new_process->previous = NULL;

  int queue_index = HIGHEST_PRIORITY - priority;

  if (process_queues[queue_index] == NULL) {
    process_queues[queue_index] = new_process;
  } else {
    struct process_t *temp = process_queues[queue_index];
    while (temp->next != NULL) {
      temp = temp->next;
    }
    temp->next = new_process;
    new_process->previous = temp;
  }
}

// Cria e retorna o identificador da fila de mensagens do escalonador.
int get_scheduler_msg_queue() {
  int msq_id = msgget(SCHED_MSQ_KEY, IPC_CREAT | 0x1FF);
  if (msq_id < 0) {
    printf("\nError: Unable to create message queue!\n");
    exit(1);
  }
  return msq_id;
}
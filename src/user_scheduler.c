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
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <sys/wait.h>
#include "utils.h"

#define QUANTUM_SECONDS 4

// Até 3 filas de processos são possíveis. Cada elemento do array representa a cabeça de uma dessas filas.
// process_queues[0] -> Processos de prioridade 1
// process_queues[1] -> Processos de prioridade 0
// process_queues[2] -> Processos de prioridade -1
struct process_t *process_queues[3] = { NULL, NULL, NULL };
struct process_t *current_process = NULL;
int num_of_queues;

int receive_scheduling_req(int msq_id, struct scheduler_req_t *msg);
void enqueue_process(struct scheduler_req_t *msg);
int get_scheduler_msg_queue();
void execute_scheduling();
void execute_round_robin();
void handle_process_termination();

void list_scheduler(struct process_t *current_process, struct process_t *process_queues[], int num_of_queues);
void exit_scheduler(struct process_t *current_process, struct process_t *process_queues[], int num_of_queues); // TODO: Arrumar isso

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
    up_sem(shell_sem_id); // Libera o shell_sched
    exit(1);
  }

  signal(SIGALRM, execute_scheduling);
  signal(SIGCHLD, handle_process_termination);
  alarm(QUANTUM_SECONDS); // O escalonador será acordado em QUANTUM_SECONDS segundos

  printf("\nProcesso 'user_scheduler' inicializado com %d filas.\n", num_of_queues);
  up_sem(shell_sem_id); // Libera o shell_sched

  int msq_id = get_scheduler_msg_queue();

  struct scheduler_req_t msg;
  while (1) {
    if (!receive_scheduling_req(msq_id, &msg)) continue;
    
    if (msg.mtype == 1) {
      enqueue_process(&msg);
      up_sem(shell_sem_id);
    } else if (msg.mtype == 2) {
      list_scheduler(current_process, process_queues, num_of_queues);
      up_sem(shell_sem_id);
    } else if (msg.mtype == 3) {
      exit_scheduler(current_process, process_queues, num_of_queues);
      exit(0);
    }
  }
  return 0;
}

// Recebe uma requisição de escalonamento. Retorna 0 caso não tenha recebido
// nenhuma mensagem, retorna 1 caso contrário.
int receive_scheduling_req(int msq_id, struct scheduler_req_t *msg) {
  int res = msgrcv(msq_id, msg, SCHED_MSG_SIZE, 0, 0);
  if (res == -1) {
    // O escalonador pode receber a interrupção SIGALRM enquanto ele estiver esperando
    // pela mensagem. Nesse caso, a fila de mensagens vai retornar um erro EINTR (de interrupção),
    // porém, como esse comportamento é esperado, nós ignoramos o erro nesses casos.
    if (errno != EINTR) {
      printf("\nError: Unable to receive scheduling request!\n");
    }
    return 0;
  }
  return 1;
}

// Cria e enfilera um processo na sua fila de prioridade.
void enqueue_process(struct scheduler_req_t *msg) {
  // Reseta o relógio para impedir que o escalonador seja interrompido
  // enquanto ele faz a atualização das filas
  int remaining_quantum = alarm(0);

  char *program_name = strtok(msg->mtext, " ");
  int priority = strtol(strtok(NULL, " "), NULL, 10);

  if (priority > HIGHEST_PRIORITY || priority <= HIGHEST_PRIORITY - num_of_queues) {
    printf("Erro: Prioridade inválida para o programa %s!", program_name);
    return;
  }

  int process_pid = fork();
  if (process_pid == 0) {
    raise(SIGSTOP);  // O novo processo começa parado
    execl(program_name, program_name, NULL);
    printf("\nErro: Não foi possível executar o programa '%s'!\n", program_name);
    exit(1);
  }

  struct process_t *new_process = malloc(sizeof(struct process_t));
  strcpy(new_process->program_name, program_name);
  clock_gettime(CLOCK_MONOTONIC, &new_process->arrival_time);
  new_process->priority = priority;
  new_process->pid = process_pid;
  new_process->next = NULL;
  new_process->finished = 0;
  new_process->turnaround = 0;

  int queue_index = HIGHEST_PRIORITY - priority;

  if (process_queues[queue_index] == NULL) {
    process_queues[queue_index] = new_process;
  } else {
    struct process_t *p = process_queues[queue_index];
    while (p->next != NULL) {
      p = p->next;
    }
    p->next = new_process;
  }

  if (current_process != NULL && current_process->priority < priority) {
    // Se o novo processo tiver uma prioridade maior, então realizamos
    // o escalonamento novamente para que ele seja escolhido
    execute_scheduling();
  } else {
    alarm(remaining_quantum);
  }
}

// Executa o escalonamento dos processos, escolhendo o processo de
// maior prioridade para ser executado.
void execute_scheduling() {
  alarm(0);
  if (current_process != NULL) {
    kill(current_process->pid, SIGSTOP);
    execute_round_robin();
  }

  // Escolhe o próximo processo a ser executado
  struct process_t *next_process = NULL;

  for (int i = 0; i < num_of_queues; i++) {
    struct process_t *p = process_queues[i];
    while (p != NULL) {
      if (p->finished == 0) {
        next_process = p;
        break;
      }
      p = p->next;
    }
    if (next_process != NULL) {
      break;
    }
  }

  current_process = next_process;
  if (next_process != NULL) {
    kill(next_process->pid, SIGCONT);
  }
  alarm(QUANTUM_SECONDS);
}

// Coloca o processo atual no final da sua fila de prioridade
void execute_round_robin() {
  int queue_index = HIGHEST_PRIORITY - current_process->priority;

  // Primeiro removemos o processo da fila
  struct process_t *prev = NULL;
  struct process_t *p = process_queues[queue_index];

  while (p != NULL) {
    if (p == current_process) {
      if (prev == NULL) {
        process_queues[queue_index] = current_process->next;
      } else {
        prev->next = current_process->next;
      }
      break;
    }
    prev = p;
    p = p->next;
  }

  // Agora inserimos o processo no fila da fila novamente
  if (process_queues[queue_index] == NULL) {
    process_queues[queue_index] = current_process;
  } else {
    struct process_t *p = process_queues[queue_index];
    while (p->next != NULL) {
      p = p->next;
    }
    p->next = current_process;
  }
  current_process->next = NULL;
}

// Atualiza nas filas de prioridade quais processos terminaram, realizando
// o cálculo do tempo de turnaround
void handle_process_termination() {
  // Reseta o relógio para impedir que o escalonador seja interrompido
  // enquanto ele faz a atualização das filas
  int remaining_quantum = alarm(0);

  int status;
  pid_t pid;

  int should_exec_scheduling = 0;

  while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
    if (current_process && current_process->pid == pid) {
      should_exec_scheduling = 1;
    }

    for (int i = 0; i < num_of_queues; i++) {
      struct process_t *p = process_queues[i];
      while (p != NULL) {
        if (p->pid == pid) {
          p->finished = 1;
          clock_gettime(CLOCK_MONOTONIC, &p->finish_time);

          double start = p->arrival_time.tv_sec + p->arrival_time.tv_nsec / 1e9;
          double end   = p->finish_time.tv_sec + p->finish_time.tv_nsec / 1e9;
          p->turnaround = end - start;

          break;
        }
        p = p->next;
      }
    }
  }

  if (should_exec_scheduling) {
    execute_scheduling();
  } else {
    alarm(remaining_quantum);
  }
}

// Cria e retorna o identificador da fila de mensagens do escalonador.
int get_scheduler_msg_queue() {
  int msq_id = msgget(SCHED_MSQ_KEY, IPC_CREAT | 0x1FF);
  if (msq_id < 0) {
    printf("\nErro: Não foi possível criar a fila de mensagens do escalonador!\n");
    exit(1);
  }
  return msq_id;
}
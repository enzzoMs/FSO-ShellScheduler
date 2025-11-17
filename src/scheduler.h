#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <time.h>


#define HIGHEST_PRIORITY 1
#define SCHED_MSQ_KEY 0x4272
#define SCHED_MSG_SIZE 100


struct process_t {
    char program_name[100];
    int pid;
    int priority;
    int finished;

    struct timespec arrival_time;
    struct timespec finish_time;

    struct process_t *next;
    struct process_t *previous;
};

// Requisição de escalonamento. O mtext deve ser uma string no formato:
// "<nome_do_programa> <prioridade>"
struct scheduler_req_t {
  long mtype;
  char mtext[SCHED_MSG_SIZE];
};


extern struct process_t *process_queues[3];
extern struct process_t *current_process;
extern int num_of_queues;
extern int msqid;
extern int semid;




void list_scheduler();
void exit_scheduler();

double compute_turnaround(struct process_t *p);

#endif
#ifndef UTILS_H
#define UTILS_H

/* PROCESS ==================================== */

#define HIGHEST_PRIORITY 1

struct process_t {
    char program_name[100];
    int pid;
    int priority;
    int finished;
    double turnaround;

    struct timespec arrival_time;
    struct timespec finish_time;

    struct process_t *next;
};

/* SCHEDULER MESSAGE QUEUE ===================  */

#define SCHED_MSQ_KEY 0x4272
#define SCHED_MSG_SIZE 100

// Requisição de escalonamento. O mtext deve ser uma string no formato:
// "<nome_do_programa> <prioridade>"
struct scheduler_req_t {
    long mtype;
    char mtext[SCHED_MSG_SIZE];
};

/* SEMAPHORE ================================== */

#define SHELL_SEM_KEY 0x2110

int get_shell_semaphore();

void down_sem(int sem_id);

void up_sem(int sem_id);

#endif
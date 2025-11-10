#ifndef UTILS_H
#define UTILS_H

int get_shell_semaphore();

void down_sem(int sem_id);

void up_sem(int sem_id);

#endif
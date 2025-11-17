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
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/sem.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include "./utils.h"

#define MAX_COMANDO 256

int main() {
    char comando[MAX_COMANDO];
    char *token, *n_queues, *command, *priority;

    int shell_sem_id = get_shell_semaphore();
    semctl(shell_sem_id, 0, SETVAL, 0);

    int user_scheduler_pid = -1;

    while (1) {
        printf("\n>shell_sched: ");
        
        if (fgets(comando, MAX_COMANDO, stdin) == NULL){
            printf("\n\033[1;31mERRO!!!\033[0m Falha na leitura do comando!\n");
            break;
        }

        comando[strcspn(comando, "\n")] = '\0';
        token = strtok(comando, " ");

        if (token == NULL){
            continue;
        }
        else if (strcmp(token, "user_scheduler") == 0){
            n_queues = strtok(NULL, " ");
            
            if (n_queues == NULL){
                printf("\033[1;31mERRO!!!\033[0m Foi recebido menos argumentos que o esperado!\n");
                printf("Utilização: user_scheduler <number_of_queues>\n");
                continue;
            }
            
            token = strtok(NULL, " ");
            if (token != NULL){
                printf("\033[1;31mERRO!!!\033[0m Foi passado mais argumentos que o esperado!\n");
                printf("Utilização: user_scheduler <number_of_queues>\n");
                continue;
            }

            int pid = fork();
            if (pid == 0) {
                user_scheduler_pid = pid;
                execl("./out/user_scheduler", "user_scheduler", n_queues, NULL);
                printf("\033[1;31mERRO!!!\033[0m Não foi possível criar o processo 'user_scheduler'!\n");
                exit(1);
            }

            down_sem(shell_sem_id); // Espera até que o user_scheduler seja inicializado
        }
        else if (strcmp(token, "execute_process") == 0){
            command = strtok(NULL, " ");
            priority = strtok(NULL, " ");
            
            if (command == NULL || priority == NULL){
                printf("\033[1;31mERRO!!!\033[0m Foi recebido menos argumentos que o esperado!\n");
                printf("Utilização: execute_process <command> <priority>\n");
                continue;
            }
            
            token = strtok(NULL, " ");
            if (token != NULL){
                printf("\033[1;31mERRO!!!\033[0m Foi passado mais argumentos que o esperado!\n");
                printf("Utilização: execute_process <command> <priority>\n");
                continue;
            }

            int msq_id = msgget(SCHED_MSQ_KEY, 0);
            struct scheduler_req_t msg;

            msg.mtype = 1;
            snprintf(msg.mtext, SCHED_MSG_SIZE, "%s %s", command, priority);
            msgsnd(msq_id, &msg, SCHED_MSG_SIZE, 0);

            down_sem(shell_sem_id); // Espera até que o comando termine
        }
        else if (strcmp(token, "list_scheduler") == 0){
            token = strtok(NULL, " ");
            if (token != NULL){
                printf("\033[1;31mERRO!!!\033[0m Foi passado mais argumentos que o esperado!\n");
                printf("Utilização: list_scheduler\n");
                continue;
            }

            int msq_id = msgget(SCHED_MSQ_KEY, 0);
            struct scheduler_req_t msg;

            msg.mtype = 2;
            msg.mtext[0] = '\0';
            msgsnd(msq_id, &msg, SCHED_MSG_SIZE, 0);

            down_sem(shell_sem_id); // Espera até que o comando termine
        }
        else if (strcmp(token, "exit_scheduler") == 0){
            token = strtok(NULL, " ");
            if (token != NULL){
                printf("\033[1;31mERRO!!!\033[0m Foi passado mais argumentos que o esperado!\n");
                printf("Utilização: exit_scheduler\n");
                continue;
            }
            
            int msq_id = msgget(SCHED_MSQ_KEY, 0);
            struct scheduler_req_t msg;

            msg.mtype = 3;
            msg.mtext[0] = '\0';
            msgsnd(msq_id, &msg, SCHED_MSG_SIZE, 0);

            // Espera o escalonador terminar
            if (user_scheduler_pid != -1) {
                int status;
                wait(&status);
            }
            break;
        }
        else {
            printf("\033[1;31mERRO!!!\033[0m Comando não reconhecido!\n"); 
        }
    }
    return 0;
}
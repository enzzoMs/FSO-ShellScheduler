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

#define MAX_COMANDO 256

void shell_sched() {
    char comando[MAX_COMANDO];
    char *token, *n_queues, *command, *priority;

    while(1) {
        printf(">shell_sched: ");
        
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
                printf("Utilização:\n\nuser_scheduler <number_of_queues>\n");
                continue;
            }
            
            token = strtok(NULL, " ");
            if (token != NULL){
                printf("\033[1;31mERRO!!!\033[0m Foi passado mais argumentos que o esperado!\n");
                printf("Utilização:\n\nuser_scheduler <number_of_queues>\n");
                continue;
            }
            
            printf("Numero de filas: %s\n", n_queues);

            //user_scheduler(n_queues);
        }
        else if (strcmp(token, "execute_process") == 0){
            command = strtok(NULL, " ");
            priority = strtok(NULL, " ");
            
            if (command == NULL || priority == NULL){
                printf("\033[1;31mERRO!!!\033[0m Foi recebido menos argumentos que o esperado!\n");
                printf("Utilização:\n\nexecute_process <command priority>\n");
                continue;
            }
            
            token = strtok(NULL, " ");
            if (token != NULL){
                printf("\033[1;31mERRO!!!\033[0m Foi passado mais argumentos que o esperado!\n");
                printf("Utilização:\n\nexecute_process <command priority>\n");
                continue;
            }

            //execute_process(command, priority);
        }
        else if (strcmp(token, "list_scheduler") == 0){
            token = strtok(NULL, " ");
            if (token != NULL){
                printf("\033[1;31mERRO!!!\033[0m Foi passado mais argumentos que o esperado!\n");
                printf("Utilização:\n\nlist_scheduler\n");
                continue;
            }
            
            //list_scheduler();
        }
        else if (strcmp(token, "exit_scheduler") == 0){
            token = strtok(NULL, " ");
            if (token != NULL){
                printf("\033[1;31mERRO!!!\033[0m Foi passado mais argumentos que o esperado!\n");
                printf("Utilização:\n\nexit_scheduler\n");
                continue;
            }
            
            //exit_scheduler();
            break;
        }
        else{
            printf("\033[1;31mERRO!!!\033[0m Comando não reconhecido!\n"); 
        }
    }
} 
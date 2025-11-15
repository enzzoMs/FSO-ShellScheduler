# FSO - ShellScheduler

Este projeto implementa um shell com suporte a escalonamento preemptivo com prioridades estáticas, desenvolvido como projeto prático para a disciplina de Fundamentos de Sistemas Operacionais (FSO), ofertada pela [Universidade de Brasília](https://www.unb.br/) durante o semestre 2025/2.

## Execução

### Requisitos

Para a execução do projeto, é necessário ter:

* GCC 13.0 ou superior instalado no sistema;
* Ambiente Linux ou MacOS compatível com System V IPC (filas de mensagens, sinais etc.);

### Como executar

Primeiramente faça o clone deste repositório para o seu ambiente local.

```
git clone https://github.com/enzzoMs/FSO-ShellScheduler.git
```

Abra o terminal ou prompt de comando e navegue até o diretório raiz do projeto:

```
cd ./FSO-ShellScheduler
```

Execute o script de build para compilar todos os binários:

```
./build.sh
```

Após a compilação, inicie o shell:

```
./out/shell_sched
```

## Comandos

Comandos disponíveis no shell_sched:

| Comando                                    | Descrição                                                 |
| ------------------------------------------ | --------------------------------------------------------- |
| `user_scheduler <number_of_queues>` | Cria o processo escalonador com 2 ou 3 filas round-robin.             |
| `execute_process <program> <priority>`       | Envia um novo processo para o escalonador com uma determinada prioridade.                |
| `list_scheduler`                           | Mostra o estado atual das filas e o processo em execução. |
| `exit_scheduler`                           | Finaliza o escalonador e exibe o tempo de turnaround de cada processo.     |

## Autores

* Gabriel Barros Ferreira — 190125616
* Enzzo Morais de Olinda — 211042720
* Layr de Andrade Neto — 222001331

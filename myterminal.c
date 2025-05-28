#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "lista.h"

#define MAX_ARGS 100
#define MAX_COMMANDS 5

void ejecutarComando(char *comando);
void ejecutarComandoPipas(char *comandos[], int numComandos);
char** dividirComando(char *comando, int *numArgs);

int main() {
    char entrada[200];
    char *comandos[MAX_COMMANDS];
    int numComandos;

    while (1) {
        printf("Terminal_Hacker_PRO> ");
        if (fgets(entrada, sizeof(entrada), stdin) == NULL) {
            perror("Error al leer entrada");
            continue;
        }
        entrada[strcspn(entrada, "\n")] = 0; 

        if (strcmp(entrada, "exit") == 0) {
            printf("Adios\n");
            break;
        }

        numComandos = 0;
        char *inicio = entrada;
        int enComilla = 0;
        char *comActual = entrada;
        
        for (char *p = entrada; *p; p++) {
            if (*p == '\'' || *p == '\"') {
                enComilla = !enComilla;
            } else if (*p == '|' && !enComilla) {
                *p = '\0';
                comandos[numComandos++] = comActual;
                comActual = p + 1;
                
                while (*(comActual) == ' ') {
                    comActual++;
                }
                
                if (numComandos >= MAX_COMMANDS) break;
            }
        }
        
        if (comActual[0] != '\0') {
            comandos[numComandos++] = comActual;
        }

        if (numComandos == 1) {
            ejecutarComando(comandos[0]);
        } else {
            ejecutarComandoPipas(comandos, numComandos);
        }
    }

    return 0;
}

char** dividirComando(char *comando, int *numArgs) {
    char **args = malloc(MAX_ARGS * sizeof(char*));
    *numArgs = 0;
    
    char *p = comando;
    int inArg = 0;
    char comilla = 0;
    char buffer[1024];
    int bufIdx = 0;
    
    while (*p) {
      
        if (comilla) {
            if (*p == comilla) {
                comilla = 0;
            } else {
               
                buffer[bufIdx++] = *p;
            }
        } 
      
        else {
            if (*p == ' ' || *p == '\t') {
                if (inArg) {
                    
                    buffer[bufIdx] = '\0';
                    args[(*numArgs)++] = strdup(buffer);
                    bufIdx = 0;
                    inArg = 0;
                }
            } else if (*p == '\'' || *p == '\"') {
                comilla = *p;
                if (!inArg) {
                    inArg = 1;
                }
            } else {
             
                if (!inArg) {
                    inArg = 1;
                }
                buffer[bufIdx++] = *p;
            }
        }
        p++;
    }
    
    if (inArg) {
        buffer[bufIdx] = '\0';
        args[(*numArgs)++] = strdup(buffer);
    }
    
    args[*numArgs] = NULL;
    return args;
}

void ejecutarComando(char *comando) {
    int numArgs = 0;
    char **args = dividirComando(comando, &numArgs);

    int pid = fork();
    if (pid < 0) {
        perror("Error al crear el proceso hijo");
    } else if (pid == 0) {
     
        if (execvp(args[0], args) == -1) {
            perror("Error al ejecutar el comando");
        }
        
        for (int i = 0; i < numArgs; i++) {
            free(args[i]);
        }
        free(args);
        exit(EXIT_FAILURE);
    } else {
       
        wait(NULL);
      
        for (int i = 0; i < numArgs; i++) {
            free(args[i]);
        }
        free(args);
    }
}

void ejecutarComandoPipas(char *comandos[], int numComandos) {
    int pipes[MAX_COMMANDS - 1][2];
    pid_t pids[MAX_COMMANDS];

    for (int i = 0; i < numComandos - 1; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("Error al crear la tubería");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < numComandos; i++) {
        pids[i] = fork();
        if (pids[i] < 0) {
            perror("Error al crear el proceso hijo");
            exit(EXIT_FAILURE);
        } else if (pids[i] == 0) {
          
            if (i > 0) {
                
                dup2(pipes[i - 1][0], STDIN_FILENO);
            }
            if (i < numComandos - 1) {
                
                dup2(pipes[i][1], STDOUT_FILENO);
            }

            for (int j = 0; j < numComandos - 1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            int numArgs = 0;
            char **args = dividirComando(comandos[i], &numArgs);
            
            if (execvp(args[0], args) == -1) {
                perror("Error al ejecutar el comando");
            }
            
            for (int j = 0; j < numArgs; j++) {
                free(args[j]);
            }
            free(args);
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < numComandos - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    for (int i = 0; i < numComandos; i++) {
        waitpid(pids[i], NULL, 0);
    }
}


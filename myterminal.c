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
#include "proceso.h"
#include "memoria.h"
#include "calendarizacionCPU.h"
#include "manual.h"

#define MAX_ARGS 100
#define MAX_COMMANDS 5
#define TAMANO_MEMORIA 1024 // Tamaño total de memoria simulada

// gcc myterminal.c memoria.c lista.c proceso.c calendarizacionCPU.c manual.c -o myterminal -lm

// Definición global de la lista de procesos
struct Lista listaProcesos;

void ejecutarComando(char *comando);
void ejecutarComandoPipas(char *comandos[], int numComandos);
char** dividirComando(char *comando, int *numArgs);

int main() {
    inicializarMemoria(TAMANO_MEMORIA);

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
        } else {
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

    // mkprocess <id> <burstTime> <bloques>
    if (numArgs > 0 && strcmp(args[0], "mkprocess") == 0) {
        if (numArgs != 4) {
            printf("mkprocess <id> <burstTime> <bloques>\n");
        } else {
            // Verifica si ya existe el proceso
            struct Nodo* temp = listaProcesos.cabeza;
            int existe = 0;
            while (temp) {
                struct Proceso* p = (struct Proceso*)temp->dato;
                if (strcmp(p->id, args[1]) == 0) {
                    existe = 1;
                    break;
                }
                temp = temp->siguiente;
            }
            if (existe) {
                printf("Error: Ya existe un proceso con el id %s\n", args[1]);
            } else {
                struct Proceso* proc = malloc(sizeof(struct Proceso));
                strncpy(proc->id, args[1], sizeof(proc->id)-1);
                proc->id[sizeof(proc->id)-1] = '\0';
                proc->burstTime = atoi(args[2]);
                proc->bloques = atoi(args[3]);
                proc->estado = NEW;
                proc->waitingTime = 0;
                proc->turnaroundTime = 0;
                agregarElemento(&listaProcesos, proc);
                printf("Proceso %s creado.\n", args[1]);
            }
        }
        for (int i = 0; i < numArgs; i++) free(args[i]);
        free(args);
        return;
    }

    // listprocess
    if (numArgs > 0 && strcmp(args[0], "listprocess") == 0) {
        printf("ID\tBurst\tBloques\tEstado\n");
        struct Nodo* temp = listaProcesos.cabeza;
        while (temp) {
            struct Proceso* p = (struct Proceso*)temp->dato;
            const char* estadoStr = (p->estado == NEW) ? "NEW" :
                                    (p->estado == READY) ? "READY" :
                                    (p->estado == TERMINATED) ? "TERMINATED" : "UNKNOWN";
            printf("%s\t%d\t%zu\t%s\n", p->id, p->burstTime, p->bloques, estadoStr);
            temp = temp->siguiente;
        }
        for (int i = 0; i < numArgs; i++) free(args[i]);
        free(args);
        return;
    }

    // my_kill <id>
    if (numArgs > 0 && strcmp(args[0], "my_kill") == 0) {
        if (numArgs != 2) {
            printf("Uso: my_kill <idproceso>\n");
        } else {
            eliminarProcesoPorId(&listaProcesos, args[1]);
        }
        for (int i = 0; i < numArgs; i++) free(args[i]);
        free(args);
        return;
    }

    // alloc <id> <estrategia>
    if (numArgs > 0 && strcmp(args[0], "alloc") == 0) {
        if (numArgs != 3) {
            printf("Uso: alloc <id> <estrategia>\n");
        } else {
            struct Nodo* temp = listaProcesos.cabeza;
            struct Proceso* proc = NULL;
            while (temp) {
                struct Proceso* p = (struct Proceso*)temp->dato;
                if (strcmp(p->id, args[1]) == 0) {
                    proc = p;
                    break;
                }
                temp = temp->siguiente;
            }
            if (!proc) {
                printf("No existe el proceso %s\n", args[1]);
            } else if (proc->estado != NEW) {
                printf("El proceso %s ya esta cargado en memoria\n", args[1]);
            } else {
                if (asignarBloque(proc->id, proc->bloques, args[2])) {
                    proc->estado = READY;
                    printf("Proceso %s cargado a memoria\n", proc->id);
                } else {
                    printf("No se pudo cargar el proceso %s a memoria\n", proc->id);
                }
            }
        }
        for (int i = 0; i < numArgs; i++) free(args[i]);
        free(args);
        return;
    }

    // mstatus
    if (numArgs > 0 && strcmp(args[0], "mstatus") == 0) {
        mostrarEstadoMemoria();
        for (int i = 0; i < numArgs; i++) free(args[i]);
        free(args);
        return;
    }

    // compact
    if (numArgs > 0 && strcmp(args[0], "compact") == 0) {
        compactarMemoria();
        printf("Memoria compactada.\n");
        for (int i = 0; i < numArgs; i++) free(args[i]);
        free(args);
        return;
    }

    // free <id>
    if (numArgs > 0 && strcmp(args[0], "free") == 0) {
        if (numArgs != 2) {
            printf("Uso: free <id>\n");
        } else {
            struct Nodo* temp = listaProcesos.cabeza;
            struct Proceso* proc = NULL;
            while (temp) {
                struct Proceso* p = (struct Proceso*)temp->dato;
                if (strcmp(p->id, args[1]) == 0) {
                    proc = p;
                    break;
                }
                temp = temp->siguiente;
            }
            if (!proc) {
                printf("No existe el proceso %s\n", args[1]);
            } else if (proc->estado != READY) {
                printf("El proceso %s no está en memoria\n", args[1]);
            } else {
                if (liberarBloque(proc->id)) {
                    proc->estado = TERMINATED;
                    printf("Proceso %s liberado de memoria\n", proc->id);
                }
            }
        }
        for (int i = 0; i < numArgs; i++) free(args[i]);
        free(args);
        return;
    }

    // fcfs
    if (numArgs > 0 && strcmp(args[0], "fcfs") == 0) {
        if (listaProcesos.cabeza == NULL) {
            printf("No hay procesos en memoria.\n");
        } else {
            calcularTiemposFCFS(&listaProcesos);
            mostrarResultados(&listaProcesos);
        }
        for (int i = 0; i < numArgs; i++) free(args[i]);
        free(args);
        return;
    }

    // sjf
    if (numArgs > 0 && strcmp(args[0], "sjf") == 0) {
        if (listaProcesos.cabeza == NULL) {
            printf("No hay procesos en memoria.\n");
        } else {
            calcularTiemposSJF(&listaProcesos);
            mostrarResultados(&listaProcesos);
        }
        for (int i = 0; i < numArgs; i++) free(args[i]);
        free(args);
        return;
    }

    // roundrobin [quantum]
    if (numArgs > 0 && strcmp(args[0], "roundrobin") == 0) {
        if (listaProcesos.cabeza == NULL) {
            printf("No hay procesos en memoria.\n");
        } else {
            int quantum = 10;
            if (numArgs == 2) {
                quantum = atoi(args[1]);
            } else {
                printf("¿Cuántos quantums quieres usar? [valor por defecto: 10]: ");
                char buffer[16];
                if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
                    int q = atoi(buffer);
                    if (q > 0) quantum = q;
                }
            }
            calcularTiemposRR(&listaProcesos, quantum);
            mostrarResultados(&listaProcesos);
        }
        for (int i = 0; i < numArgs; i++) free(args[i]);
        free(args);
        return;
    }
    if (numArgs > 0 && strcmp(args[0], "manual") == 0) {
    if (numArgs != 2) {
        printf("Uso: manual <comando>\n");
    } else {
        mostrar_manual(args[1]);
    }
    for (int i = 0; i < numArgs; i++) free(args[i]);
    free(args);
    return;
}

    // Comando externo (por defecto)
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
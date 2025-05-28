#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "proceso.h"
#include "memoria.h"

struct Proceso* crearProceso(const char* id, int burstTime, size_t bloques) {
    void* mem = reservarMemoria(bloques);
    if (!mem) {
        printf("Error: No hay suficiente memoria para el proceso %s\n", id);
        return NULL;
    }

    struct Proceso* proceso = (struct Proceso*)malloc(sizeof(struct Proceso));
    if (proceso == NULL) {
        printf("Error al asignar memoria para el proceso\n");
        liberarMemoria(mem);
        return NULL;
    }

    strncpy(proceso->id, id, sizeof(proceso->id) - 1);
    proceso->id[sizeof(proceso->id) - 1] = '\0';
    proceso->burstTime = burstTime;
    proceso->waitingTime = 0;
    proceso->turnaroundTime = 0;
    proceso->memoria = mem;

    return proceso;
}

void liberarProceso(struct Proceso* proceso) {
    if (proceso != NULL) {
        if (proceso->memoria) {
            liberarMemoria(proceso->memoria);
        }
        free(proceso);
    }
}


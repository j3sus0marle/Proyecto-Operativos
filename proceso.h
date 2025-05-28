#ifndef PROCESO_H
#define PROCESO_H
#include "lista.h"
#include <stddef.h>

struct Proceso {
    char id[32];
    int burstTime;
    int waitingTime;
    int turnaroundTime;
    int estado;        // 0 = listo, 1 = ejecutando, 2 = terminado, etc.
    size_t bloques;    // Cantidad de bloques ocupados
};

struct Proceso* crearProceso(const char* id, int burstTime, size_t bloques, const char* estrategia);
void liberarProceso(struct Proceso* proceso);
void mostrarProcesos(struct Lista* lista);
void eliminarProcesoPorId(struct Lista* lista, const char* id);

#endif
#ifndef PROCESO_H
#define PROCESO_H
#include "lista.h"
#include <stddef.h>

enum EstadoProceso { NEW, READY, TERMINATED };

struct Proceso {
    char id[32];
    int burstTime;
    int waitingTime;
    int turnaroundTime;
    enum EstadoProceso estado;
    size_t bloques;
};

struct Proceso* crearProceso(const char* id, int burstTime, size_t bloques, const char* estrategia);
void liberarProceso(struct Proceso* proceso);
void mostrarProcesos(struct Lista* lista);
void eliminarProcesoPorId(struct Lista* lista, const char* id);

#endif
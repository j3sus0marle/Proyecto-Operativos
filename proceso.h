#ifndef PROCESO_H
#define PROCESO_H

struct Proceso {
    char id[10];
    int burstTime;
    int waitingTime;
    int turnaroundTime;
    void* memoria; // Dirección de memoria reservada
};

struct Proceso* crearProceso(const char* id, int burstTime, size_t bloques);
void liberarProceso(struct Proceso* proceso);
void mostrarProcesos(struct Lista* lista);

#endif
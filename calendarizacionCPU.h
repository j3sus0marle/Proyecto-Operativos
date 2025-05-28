#ifndef CALENDARIZACION_CPU_H
#define CALENDARIZACION_CPU_H

#include "lista.h"

// FCFS
void calcularTiemposFCFS(struct Lista* lista);

// SJF
void calcularTiemposSJF(struct Lista* lista);

// Round Robin
void calcularTiemposRR(struct Lista* lista, int quantum);

// Mostrar resultados
void mostrarResultados(struct Lista* lista);

#endif
#ifndef CALENDARIZACION_CPU_H
#define CALENDARIZACION_CPU_H

#include "lista.h"

void calcularTiemposFCFS(struct Lista* lista);
void calcularTiemposSJF(struct Lista* lista);
void calcularTiemposRR(struct Lista* lista, int quantum);
void mostrarResultados(struct Lista* lista);

#endif
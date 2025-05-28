#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "proceso.h"
#include "memoria.h"
#include "lista.h"

struct Proceso* crearProceso(const char* id, int burstTime, size_t bloques, const char* estrategia) {
    if (!asignarBloque(id, bloques, estrategia)) {
        printf("Error: No hay suficiente memoria para el proceso %s\n", id);
        return NULL;
    }

    struct Proceso* proceso = (struct Proceso*)malloc(sizeof(struct Proceso));
    if (proceso == NULL) {
        printf("Error al asignar memoria para el proceso\n");
        liberarBloque(id);
        return NULL;
    }

    strncpy(proceso->id, id, sizeof(proceso->id) - 1);
    proceso->id[sizeof(proceso->id) - 1] = '\0';
    proceso->burstTime = burstTime;
    proceso->waitingTime = 0;
    proceso->turnaroundTime = 0;
    proceso->estado = 0; // Por ejemplo, 0 = listo
    proceso->bloques = bloques;

    return proceso;
}

void liberarProceso(struct Proceso* proceso) {
    if (proceso != NULL) {
        liberarBloque(proceso->id);
        free(proceso);
    }
}

void mostrarProcesos(struct Lista* lista) {
    if (lista == NULL || lista->cabeza == NULL) {
        printf("No hay procesos creados.\n");
        return;
    }
    printf("ID\tBurst\tWaiting\tTurnaround\tEstado\tBloques\n");
    struct Nodo* temp = lista->cabeza;
    while (temp != NULL) {
        struct Proceso* p = (struct Proceso*)temp->dato;
        // Puedes traducir el estado a texto si lo deseas
        const char* estadoStr = (p->estado == 0) ? "Listo" :
                                (p->estado == 1) ? "Ejecutando" :
                                (p->estado == 2) ? "Terminado" : "Desconocido";
        printf("%s\t%d\t%d\t%d\t\t%s\t%zu\n", p->id, p->burstTime, p->waitingTime, p->turnaroundTime, estadoStr, p->bloques);
        temp = temp->siguiente;
    }
}

void eliminarProcesoPorId(struct Lista* lista, const char* id) {
    if (!lista || !lista->cabeza) return;

    struct Nodo* actual = lista->cabeza;
    struct Nodo* anterior = NULL;

    while (actual) {
        struct Proceso* p = (struct Proceso*)actual->dato;
        if (strcmp(p->id, id) == 0) {
            if (anterior) {
                anterior->siguiente = actual->siguiente;
            } else {
                lista->cabeza = actual->siguiente;
            }
            liberarProceso(p); // Libera memoria y bloque
            free(actual);
            printf("Proceso %s eliminado correctamente.\n", id);
            return;
        }
        anterior = actual;
        actual = actual->siguiente;
    }
    printf("No se encontró el proceso con id %s.\n", id);
}
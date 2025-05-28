#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lista.h"
#include "proceso.h"
#include <fcntl.h>
#include <unistd.h>

void leerArchivoProcesos(const char* filename, struct Lista* lista);
void liberarProcesos(struct Lista* lista);
void calcularTiemposRR(struct Lista* lista, int quantum);
void mostrarResultados(struct Lista* lista);

int main() {
    struct Lista lista;
    inicializarLista(&lista);

    int quantum = 10; 
    leerArchivoProcesos("listaprocesos", &lista);
    calcularTiemposRR(&lista, quantum);
    mostrarResultados(&lista);

    liberarProcesos(&lista);
    liberarLista(&lista);
    return 0;
}

void leerArchivoProcesos(const char* filename, struct Lista* lista) {
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        printf("Error al abrir el archivo %s\n", filename);
        return;
    }

    char buffer[1];
    char line[50];
    int idx = 0;
    ssize_t bytesRead;
    while ((bytesRead = read(fd, buffer, 1)) > 0) {
        if (buffer[0] == '\n' || idx >= 49) {
            line[idx] = '\0';
            if (idx > 0) {
                char id[10];
                int burstTime;
                sscanf(line, "%[^,],%d", id, &burstTime);
                struct Proceso* proceso = crearProceso(id, burstTime);
                if (proceso == NULL) {
                    close(fd);
                    return;
                }
                agregarElemento(lista, (void*)proceso);
            }
            idx = 0;
        } else {
            line[idx++] = buffer[0];
        }
    }
    if (idx > 0) {
        line[idx] = '\0';
        char id[10];
        int burstTime;
        sscanf(line, "%[^,],%d", id, &burstTime);
        struct Proceso* proceso = crearProceso(id, burstTime);
        if (proceso != NULL) {
            agregarElemento(lista, (void*)proceso);
        }
    }

    close(fd);
}

void liberarProcesos(struct Lista* lista) {
    if (lista == NULL || lista->cabeza == NULL) return;

    struct Nodo* temp = lista->cabeza;
    while (temp != NULL) {
        struct Proceso* proceso = (struct Proceso*)temp->dato;
        liberarProceso(proceso);
        temp = temp->siguiente;
    }
}

void calcularTiemposRR(struct Lista* lista, int quantum) {
    if (lista == NULL || lista->cabeza == NULL) {
        printf("La lista está vacía o no inicializada\n");
        return;
    }

    int numProcesos = 0;
    struct Nodo* temp = lista->cabeza;
    while (temp != NULL) {
        numProcesos++;
        temp = temp->siguiente;
    }

    int* remainingTimes = (int*)malloc(numProcesos * sizeof(int));
    if (remainingTimes == NULL) {
        printf("Error al asignar memoria para los tiempos restantes\n");
        return;
    }

    temp = lista->cabeza;
    for (int i = 0; i < numProcesos; i++) {
        struct Proceso* proceso = (struct Proceso*)temp->dato;
        remainingTimes[i] = proceso->burstTime;
        temp = temp->siguiente;
    }

    int tiempoActual = 0;
    int hayProcesosPendientes = 1;

    while (hayProcesosPendientes) {
        hayProcesosPendientes = 0;
        temp = lista->cabeza;

        for (int i = 0; i < numProcesos; i++) {
            struct Proceso* proceso = (struct Proceso*)temp->dato;

            if (remainingTimes[i] > 0) {
                hayProcesosPendientes = 1;

                if (remainingTimes[i] > quantum) {
                    tiempoActual += quantum;
                    remainingTimes[i] -= quantum;
                } else {
                    tiempoActual += remainingTimes[i];
                    proceso->waitingTime = tiempoActual - proceso->burstTime;
                    proceso->turnaroundTime = tiempoActual;
                    remainingTimes[i] = 0;
                }
            }

            temp = temp->siguiente;
        }
    }

    free(remainingTimes);
}


void mostrarResultados(struct Lista* lista) {
    if (lista == NULL || lista->cabeza == NULL) {
        printf("La lista está vacía o no inicializada\n");
        return;
    }

    struct Nodo* temp = lista->cabeza;
    int totalWaitingTime = 0, totalTurnaroundTime = 0, count = 0;

    printf("Proceso\tBurst Time\tWaiting Time\tTurnaround Time\n");
    while (temp != NULL) {
        struct Proceso* proceso = (struct Proceso*)temp->dato;

        printf("%s\t%d\t\t%d\t\t%d\n", proceso->id, proceso->burstTime, proceso->waitingTime, proceso->turnaroundTime);

        totalWaitingTime += proceso->waitingTime;
        totalTurnaroundTime += proceso->turnaroundTime;
        count++;

        temp = temp->siguiente;
    }

    printf("\nWaiting Time promedio: %.2f\n", (float)totalWaitingTime / count);
    printf("Turnaround Time promedio: %.2f\n", (float)totalTurnaroundTime / count);
}
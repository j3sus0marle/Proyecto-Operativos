#include "calendarizacionCPU.h"
#include "proceso.h"
#include "memoria.h"
#include <stdio.h>
#include <stdlib.h>

// FCFS
void calcularTiemposFCFS(struct Lista* lista) {
    if (!lista || !lista->cabeza) return;
    struct Nodo* temp = lista->cabeza;
    int tiempoActual = 0;
    printf("\n[FCFS] Orden de entrada y salida al CPU:\n");
    while (temp) {
        struct Proceso* p = (struct Proceso*)temp->dato;
        if (p->estado == READY) {
            printf("Proceso %s entra al CPU en t=%d\n", p->id, tiempoActual);
            p->waitingTime = tiempoActual;
            p->turnaroundTime = p->waitingTime + p->burstTime;
            tiempoActual += p->burstTime;
            printf("Proceso %s sale del CPU en t=%d\n", p->id, tiempoActual);
            p->estado = TERMINATED;
            liberarBloque(p->id);
        }
        temp = temp->siguiente;
    }
}

// SJF
void calcularTiemposSJF(struct Lista* lista) {
    if (!lista || !lista->cabeza) return;
    // Contar procesos READY
    int n = 0;
    struct Nodo* temp = lista->cabeza;
    while (temp) {
        struct Proceso* p = (struct Proceso*)temp->dato;
        if (p->estado == READY) n++;
        temp = temp->siguiente;
    }
    if (n == 0) return;

    // Crear arreglo de punteros a procesos READY
    struct Proceso** arr = malloc(n * sizeof(struct Proceso*));
    temp = lista->cabeza;
    int idx = 0;
    while (temp) {
        struct Proceso* p = (struct Proceso*)temp->dato;
        if (p->estado == READY) arr[idx++] = p;
        temp = temp->siguiente;
    }
    // Ordenar por burstTime (simple bubble sort)
    for (int i = 0; i < n-1; i++) {
        for (int j = 0; j < n-i-1; j++) {
            if (arr[j]->burstTime > arr[j+1]->burstTime) {
                struct Proceso* tmp = arr[j];
                arr[j] = arr[j+1];
                arr[j+1] = tmp;
            }
        }
    }
    int tiempoActual = 0;
    printf("\n[SJF] Orden de entrada y salida al CPU:\n");
    for (int i = 0; i < n; i++) {
        printf("Proceso %s entra al CPU en t=%d\n", arr[i]->id, tiempoActual);
        arr[i]->waitingTime = tiempoActual;
        arr[i]->turnaroundTime = arr[i]->waitingTime + arr[i]->burstTime;
        tiempoActual += arr[i]->burstTime;
        printf("Proceso %s sale del CPU en t=%d\n", arr[i]->id, tiempoActual);
        arr[i]->estado = TERMINATED;
        liberarBloque(arr[i]->id);
    }
    free(arr);
}

// Round Robin
void calcularTiemposRR(struct Lista* lista, int quantum) {
    if (!lista || !lista->cabeza) return;
    // Contar procesos READY
    int n = 0;
    struct Nodo* temp = lista->cabeza;
    while (temp) {
        struct Proceso* p = (struct Proceso*)temp->dato;
        if (p->estado == READY) n++;
        temp = temp->siguiente;
    }
    if (n == 0) return;

    // Crear arreglo de punteros a procesos READY y sus tiempos restantes
    struct Proceso** arr = malloc(n * sizeof(struct Proceso*));
    int* remaining = malloc(n * sizeof(int));
    int* finished = calloc(n, sizeof(int));
    temp = lista->cabeza;
    int idx = 0;
    while (temp) {
        struct Proceso* p = (struct Proceso*)temp->dato;
        if (p->estado == READY) {
            arr[idx] = p;
            remaining[idx] = p->burstTime;
            finished[idx] = 0;
            idx++;
        }
        temp = temp->siguiente;
    }
    int tiempoActual = 0, done = 0;
    printf("\n[Round Robin] Orden de entrada y salida al CPU:\n");
    while (done < n) {
        for (int i = 0; i < n; i++) {
            if (finished[i]) continue;
            if (remaining[i] > 0) {
                printf("Proceso %s entra al CPU en t=%d\n", arr[i]->id, tiempoActual);
                int uso = (remaining[i] > quantum) ? quantum : remaining[i];
                tiempoActual += uso;
                remaining[i] -= uso;
                if (remaining[i] == 0) {
                    arr[i]->turnaroundTime = tiempoActual;
                    arr[i]->waitingTime = arr[i]->turnaroundTime - arr[i]->burstTime;
                    printf("Proceso %s sale del CPU en t=%d (terminado)\n", arr[i]->id, tiempoActual);
                    arr[i]->estado = TERMINATED;
                    liberarBloque(arr[i]->id);
                    finished[i] = 1;
                    done++;
                } else {
                    printf("Proceso %s sale del CPU en t=%d (restante: %d)\n", arr[i]->id, tiempoActual, remaining[i]);
                }
            }
        }
    }
    free(arr);
    free(remaining);
    free(finished);
}

void mostrarResultados(struct Lista* lista) {
    if (!lista || !lista->cabeza) return;
    struct Nodo* temp = lista->cabeza;
    int n = 0;
    double totalTurnaround = 0, totalWaiting = 0;
    printf("\nResultados:\nID\tBurst\tWaiting\tTurnaround\n");
    while (temp) {
        struct Proceso* p = (struct Proceso*)temp->dato;
        if (p->estado == TERMINATED) {
            printf("%s\t%d\t%d\t%d\n", p->id, p->burstTime, p->waitingTime, p->turnaroundTime);
            totalWaiting += p->waitingTime;
            totalTurnaround += p->turnaroundTime;
            n++;
        }
        temp = temp->siguiente;
    }
    if (n > 0) {
        printf("Promedio Waiting Time: %.2f\n", totalWaiting / n);
        printf("Promedio Turnaround Time: %.2f\n", totalTurnaround / n);
    } else {
        printf("No hay procesos terminados para mostrar resultados.\n");
    }
}
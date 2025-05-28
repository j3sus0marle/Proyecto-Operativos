#include "calendarizacionCPU.h"
#include "proceso.h"
#include <stdio.h>
#include <stdlib.h>

// FCFS
void calcularTiemposFCFS(struct Lista* lista) {
    if (lista == NULL || lista->cabeza == NULL) return;
    struct Nodo* temp = lista->cabeza;
    int tiempoActual = 0;
    printf("\n[FCFS] Orden de entrada y salida al CPU:\n");
    while (temp != NULL) {
        struct Proceso* proceso = (struct Proceso*)temp->dato;
        printf("Proceso %s entra al CPU en t=%d\n", proceso->id, tiempoActual);
        proceso->waitingTime = tiempoActual;
        proceso->turnaroundTime = proceso->waitingTime + proceso->burstTime;
        tiempoActual += proceso->burstTime;
        printf("Proceso %s sale del CPU en t=%d\n", proceso->id, tiempoActual);
        temp = temp->siguiente;
    }
}

// SJF
void calcularTiemposSJF(struct Lista* lista) {
    if (lista == NULL || lista->cabeza == NULL) return;
    int n = 0;
    struct Nodo* temp = lista->cabeza;
    while (temp) { n++; temp = temp->siguiente; }
    if (n == 0) return;

    struct Proceso** arr = malloc(n * sizeof(struct Proceso*));
    temp = lista->cabeza;
    for (int i = 0; i < n; i++) {
        arr[i] = (struct Proceso*)temp->dato;
        temp = temp->siguiente;
    }
    // Ordenar por burstTime (simple bubble sort)
    for (int i = 0; i < n-1; i++) {
        for (int j = 0; j < n-i-1; j++) {
            if (arr[j]->burstTime > arr[j+1]->burstTime) {
                struct Proceso* aux = arr[j];
                arr[j] = arr[j+1];
                arr[j+1] = aux;
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
    }
    free(arr);
}

// Round Robin
void calcularTiemposRR(struct Lista* lista, int quantum) {
    if (lista == NULL || lista->cabeza == NULL) return;
    int n = 0;
    struct Nodo* temp = lista->cabeza;
    while (temp) { n++; temp = temp->siguiente; }
    if (n == 0) return;

    struct Proceso** arr = malloc(n * sizeof(struct Proceso*));
    int* remaining = malloc(n * sizeof(int));
    int* finished = calloc(n, sizeof(int));
    temp = lista->cabeza;
    for (int i = 0; i < n; i++) {
        arr[i] = (struct Proceso*)temp->dato;
        remaining[i] = arr[i]->burstTime;
        arr[i]->waitingTime = 0;
        arr[i]->turnaroundTime = 0;
        finished[i] = 0;
        temp = temp->siguiente;
    }
    int tiempoActual = 0, done = 0;
    printf("\n[Round Robin] Orden de entrada y salida al CPU:\n");
    while (done < n) {
        for (int i = 0; i < n; i++) {
            if (remaining[i] > 0) {
                printf("Proceso %s entra al CPU en t=%d\n", arr[i]->id, tiempoActual);
                int tiempoUso = (remaining[i] > quantum) ? quantum : remaining[i];
                arr[i]->waitingTime += tiempoActual - arr[i]->turnaroundTime;
                tiempoActual += tiempoUso;
                remaining[i] -= tiempoUso;
                arr[i]->turnaroundTime = tiempoActual;
                printf("Proceso %s sale del CPU en t=%d\n", arr[i]->id, tiempoActual);
                if (remaining[i] == 0) done++;
            }
        }
    }
    for (int i = 0; i < n; i++) {
        arr[i]->turnaroundTime = arr[i]->turnaroundTime;
        arr[i]->waitingTime = arr[i]->turnaroundTime - arr[i]->burstTime;
    }
    free(arr);
    free(remaining);
    free(finished);
}

// Mostrar resultados
void mostrarResultados(struct Lista* lista) {
    if (lista == NULL || lista->cabeza == NULL) {
        printf("La lista está vacía o no inicializada\n");
        return;
    }
    struct Nodo* temp = lista->cabeza;
    int totalWaitingTime = 0, totalTurnaroundTime = 0, count = 0;
    printf("\nProceso\tBurst Time\tWaiting Time\tTurnaround Time\n");
    while (temp != NULL) {
        struct Proceso* proceso = (struct Proceso*)temp->dato;
        printf("%s\t%d\t\t%d\t\t%d\n", proceso->id, proceso->burstTime, proceso->waitingTime, proceso->turnaroundTime);
        totalWaitingTime += proceso->waitingTime;
        totalTurnaroundTime += proceso->turnaroundTime;
        count++;
        temp = temp->siguiente;
    }
    if (count > 0) {
        printf("\nWaiting Time promedio: %.2f\n", (float)totalWaitingTime / count);
        printf("Turnaround Time promedio: %.2f\n", (float)totalTurnaroundTime / count);
    }
}
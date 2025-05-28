#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lista.h"
#include "proceso.h"
#include <fcntl.h>
#include <unistd.h>

void leerArchivoProcesos(const char* filename, struct Lista* lista);
void liberarProcesos(struct Lista* lista);
void calcularTiempos(struct Lista* lista);
void mostrarResultados(struct Lista* lista);

int main() {
    struct Lista lista;
    inicializarLista(&lista);

    leerArchivoProcesos("listaprocesos", &lista);
    calcularTiempos(&lista);
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

void calcularTiempos(struct Lista* lista) {
    if (lista == NULL || lista->cabeza == NULL) {
        printf("La lista está vacía o no inicializada\n");
        return;
    }

    struct Nodo* temp = lista->cabeza;
    int tiempoActual = 0;

    while (temp != NULL) {
        struct Proceso* proceso = (struct Proceso*)temp->dato;

        proceso->waitingTime = tiempoActual;
        proceso->turnaroundTime = proceso->waitingTime + proceso->burstTime;

        tiempoActual += proceso->burstTime;
        temp = temp->siguiente;
    }
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

    printf("\nWatting Time promedio: %.2f\n", (float)totalWaitingTime / count);
    printf("Turnaround promedio: %.2f\n", (float)totalTurnaroundTime / count);
}
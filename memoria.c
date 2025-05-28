#include "memoria.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void* memoria_base = NULL;
static size_t memoria_total = 0;
static BloqueMemoria* lista_bloques = NULL;

void inicializarMemoria(size_t tamano) {
    if (memoria_base != NULL) {
        free(memoria_base);
        memoria_base = NULL;
        memoria_total = 0;
        // Liberar la lista de bloques
        BloqueMemoria* actual = lista_bloques;
        while (actual) {
            BloqueMemoria* siguiente = actual->siguiente;
            free(actual);
            actual = siguiente;
        }
        lista_bloques = NULL;
    }
    memoria_base = malloc(tamano);
    if (!memoria_base) {
        printf("Error al asignar memoria simulada\n");
        exit(1);
    }
    memoria_total = tamano;

    lista_bloques = (BloqueMemoria*)malloc(sizeof(BloqueMemoria));
    lista_bloques->direccion = memoria_base;
    lista_bloques->tamano = tamano;
    lista_bloques->libre = true;
    lista_bloques->siguiente = NULL;
}

void* reservarMemoria(size_t tamano) {
    BloqueMemoria* actual = lista_bloques;
    while (actual) {
        if (actual->libre && actual->tamano >= tamano) {
            if (actual->tamano > tamano) {
                // Dividir el bloque
                BloqueMemoria* nuevo = (BloqueMemoria*)malloc(sizeof(BloqueMemoria));
                nuevo->direccion = (char*)actual->direccion + tamano;
                nuevo->tamano = actual->tamano - tamano;
                nuevo->libre = true;
                nuevo->siguiente = actual->siguiente;

                actual->tamano = tamano;
                actual->siguiente = nuevo;
            }
            actual->libre = false;
            return actual->direccion;
        }
        actual = actual->siguiente;
    }
    return NULL; // No hay suficiente memoria
}

void liberarMemoria(void* ptr) {
    BloqueMemoria* actual = lista_bloques;
    while (actual) {
        if (actual->direccion == ptr) {
            actual->libre = true;
            // Fusionar con el siguiente si está libre
            while (actual->siguiente && actual->siguiente->libre) {
                BloqueMemoria* siguiente = actual->siguiente;
                actual->tamano += siguiente->tamano;
                actual->siguiente = siguiente->siguiente;
                free(siguiente);
            }
            break;
        }
        actual = actual->siguiente;
    }
}

void mostrarEstadoMemoria(void) {
    BloqueMemoria* actual = lista_bloques;
    int i = 0;
    printf("Estado de la memoria:\n");
    while (actual) {
        printf("Bloque %d: %p | Tamaño: %zu | %s\n", i, actual->direccion, actual->tamano, actual->libre ? "Libre" : "Ocupado");
        actual = actual->siguiente;
        i++;
    }
}

void liberarMemoriaTotal(void) {
    if (memoria_base) {
        free(memoria_base);
        memoria_base = NULL;
    }
    BloqueMemoria* actual = lista_bloques;
    while (actual) {
        BloqueMemoria* siguiente = actual->siguiente;
        free(actual);
        actual = siguiente;
    }
    lista_bloques = NULL;
    memoria_total = 0;
}
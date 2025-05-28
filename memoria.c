#include "memoria.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static BloqueMemoria* listaBloques = NULL;
static size_t memoriaTotal = 0;

void inicializarMemoria(size_t tamano) {
    liberarMemoriaTotal();
    listaBloques = (BloqueMemoria*)malloc(sizeof(BloqueMemoria));
    listaBloques->direccionBase = 0;
    listaBloques->tamano = tamano;
    listaBloques->estado = true;
    listaBloques->idProceso[0] = '\0';
    listaBloques->siguiente = NULL;
    memoriaTotal = tamano;
}

// Estrategias de asignación
static BloqueMemoria* buscarBloque(size_t tamano, const char* estrategia) {
    BloqueMemoria *actual = listaBloques, *mejor = NULL;
    if (strcmp(estrategia, "first") == 0) {
        while (actual) {
            if (actual->estado && actual->tamano >= tamano)
                return actual;
            actual = actual->siguiente;
        }
    } else if (strcmp(estrategia, "best") == 0) {
        size_t min = (size_t)-1;
        while (actual) {
            if (actual->estado && actual->tamano >= tamano && actual->tamano < min) {
                mejor = actual;
                min = actual->tamano;
            }
            actual = actual->siguiente;
        }
        return mejor;
    } else if (strcmp(estrategia, "worst") == 0) {
        size_t max = 0;
        while (actual) {
            if (actual->estado && actual->tamano >= tamano && actual->tamano > max) {
                mejor = actual;
                max = actual->tamano;
            }
            actual = actual->siguiente;
        }
        return mejor;
    }
    return NULL;
}

bool asignarBloque(const char* idProceso, size_t tamano, const char* estrategia) {
    if (!idProceso || !estrategia) return false;
    // Verifica que el proceso no esté ya en memoria
    BloqueMemoria* temp = listaBloques;
    while (temp) {
        if (!temp->estado && strcmp(temp->idProceso, idProceso) == 0) {
            printf("Error: El proceso %s ya tiene un bloque asignado.\n", idProceso);
            return false;
        }
        temp = temp->siguiente;
    }
    BloqueMemoria* bloque = buscarBloque(tamano, estrategia);

    if (!bloque) {
        printf("No hay bloque suficiente para el proceso %s\n", idProceso);
        return false;
    }
    
    if (bloque->tamano > tamano) {
        // Dividir el bloque
        BloqueMemoria* nuevo = (BloqueMemoria*)malloc(sizeof(BloqueMemoria));
        nuevo->direccionBase = bloque->direccionBase + tamano;
        nuevo->tamano = bloque->tamano - tamano;
        nuevo->estado = true;
        nuevo->idProceso[0] = '\0';
        nuevo->siguiente = bloque->siguiente;

        bloque->tamano = tamano;
        bloque->siguiente = nuevo;
    }
    bloque->estado = false;
    strncpy(bloque->idProceso, idProceso, sizeof(bloque->idProceso)-1);
    bloque->idProceso[sizeof(bloque->idProceso)-1] = '\0';
    return true;
}

bool liberarBloque(const char* idProceso) {
    BloqueMemoria* actual = listaBloques;
    while (actual) {
        if (!actual->estado && strcmp(actual->idProceso, idProceso) == 0) {
            actual->estado = true;
            actual->idProceso[0] = '\0';
            // Fusionar con siguiente si está estado
            while (actual->siguiente && actual->siguiente->estado) {
                BloqueMemoria* siguiente = actual->siguiente;
                actual->tamano += siguiente->tamano;
                actual->siguiente = siguiente->siguiente;
                free(siguiente);
            }
            return true;
        }
        actual = actual->siguiente;
    }
    printf("No se encontró bloque para el proceso %s\n", idProceso);
    return false;
}

void compactarMemoria(void) {
    BloqueMemoria* actual = listaBloques;
    while (actual && actual->siguiente) {
        if (actual->estado && actual->siguiente->estado) {
            BloqueMemoria* siguiente = actual->siguiente;
            actual->tamano += siguiente->tamano;
            actual->siguiente = siguiente->siguiente;
            free(siguiente);
        } else {
            actual = actual->siguiente;
        }
    }
}

void mostrarEstadoMemoria(void) {
    BloqueMemoria* actual = listaBloques;
    printf("Bloques de memoria:\n");
    printf("Base\tLimite\tTamaño\tEstado\t\tProceso\n");
    while (actual) {
        printf("%zu\t%zu\t%zu\t%s\t\t%s\n",
            actual->direccionBase,
            actual->direccionBase + actual->tamano - 1,
            actual->tamano,
            actual->estado ? "libre" : "Ocupado",
            actual->estado ? "-" : actual->idProceso
        );
        actual = actual->siguiente;
    }
}

void liberarMemoriaTotal(void) {
    BloqueMemoria* actual = listaBloques;
    while (actual) {
        BloqueMemoria* siguiente = actual->siguiente;
        free(actual);
        actual = siguiente;
    }
    listaBloques = NULL;
    memoriaTotal = 0;
}
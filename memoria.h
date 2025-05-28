#ifndef MEMORIA_H
#define MEMORIA_H

#include <stddef.h>
#include <stdbool.h>

typedef struct BloqueMemoria {
    size_t direccionBase;         // Dirección base simulada (ej: offset)
    size_t tamano;                // Tamaño del bloque
    bool estado;                   // Estado: true = libre, false = ocupado
    char idProceso[16];           // ID del proceso alojado ("" si libre)
    struct BloqueMemoria* siguiente;
} BloqueMemoria;

// Inicializa la memoria simulada con un solo bloque libre de tamaño total
void inicializarMemoria(size_t tamano);

// Solicita un bloque de memoria para un proceso (estrategia: "first", "best", "worst")
bool asignarBloque(const char* idProceso, size_t tamano, const char* estrategia);

// Libera el bloque ocupado por el proceso con ese id
bool liberarBloque(const char* idProceso);

// Compacta bloques libres contiguos
void compactarMemoria(void);

// Muestra el estado actual de la memoria
void mostrarEstadoMemoria(void);

// Libera toda la memoria simulada
void liberarMemoriaTotal(void);

#endif
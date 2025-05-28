#ifndef MEMORIA_H
#define MEMORIA_H

#include <stddef.h>
#include <stdbool.h>

// Estructura para representar un bloque de memoria
typedef struct BloqueMemoria {
    void* direccion;
    size_t tamano;
    bool libre;
    struct BloqueMemoria* siguiente;
} BloqueMemoria;

// Inicializa la memoria con un tamaño dado
void inicializar_memoria(size_t tamano);

// Reserva un bloque de memoria de tamaño solicitado
void* reservar_memoria(size_t tamano);

// Libera un bloque de memoria previamente reservado
void liberar_memoria(void* ptr);

// Muestra el estado actual de la memoria (para depuración)
void mostrar_estado_memoria(void);

// Libera toda la memoria simulada (para finalizar el programa)
void liberar_memoria_total(void);

#endif // MEMORIA_H
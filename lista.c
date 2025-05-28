#include <stdio.h>
#include <stdlib.h>
#include "lista.h"

void inicializarLista(struct Lista* lista) {
    lista->cabeza = NULL;
}


void agregarElemento(struct Lista* lista, void* dato) {
    struct Nodo* nuevoNodo = (struct Nodo*)malloc(sizeof(struct Nodo));
    if (nuevoNodo == NULL) {
        printf("Error al asignar memoria\n");
        return;
    }

    nuevoNodo->dato = dato;
    nuevoNodo->siguiente = NULL;

    if (lista->cabeza == NULL) {
        lista->cabeza = nuevoNodo;
    } else {
        struct Nodo* temp = lista->cabeza;
        while (temp->siguiente != NULL) {
            temp = temp->siguiente;
        }
        temp->siguiente = nuevoNodo;
    }
}


void liberarLista(struct Lista* lista) {
    if (lista == NULL) return;

    struct Nodo* temp = lista->cabeza;
    while (temp != NULL) {
        struct Nodo* siguiente = temp->siguiente;
        free(temp);
        temp = siguiente;
    }
    lista->cabeza = NULL;
}
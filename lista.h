#ifndef LISTA_H
#define LISTA_H


struct Nodo {
    void* dato; 
    struct Nodo* siguiente;
};

struct Lista {
    struct Nodo* cabeza;
};


void inicializarLista(struct Lista* lista);
void agregarElemento(struct Lista* lista, void* dato); 
void mostrarLista(struct Lista* lista);
void eliminarElemento(struct Lista* lista, void* dato); 
void liberarLista(struct Lista* lista);

#endif 
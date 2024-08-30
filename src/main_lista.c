#include <stdlib.h>
#include <stdio.h>

#include "mylloc.h"

struct no{
    struct no* p;
    int v;
} typedef No;


struct lista{
    No* primeiro;
} typedef Lista;

No* criarNo(int v){
    No* ret = mylloc(sizeof(No));
    ret->p = NULL;
    ret->v = v;

    return ret;
}


void addToLista(Lista* lista, int v){
    No* pivo = lista->primeiro;
    if (pivo == NULL){
        lista->primeiro = criarNo(v);
        return;
    }

    while (pivo->p != NULL){
        pivo = pivo->p;
    };

    pivo->p = criarNo(v);
}

void removeLista(Lista* lista, int v){
    No* pivo = lista->primeiro;
    
    if (lista->primeiro->v == v){
        
        lista->primeiro = lista->primeiro->p;
        myFree(pivo);
    }
    
    No* pivo_direita = pivo->p;

    while (pivo_direita != NULL){
        if (pivo_direita->v == v){
            pivo->p = pivo_direita->p;
            myFree(pivo_direita);
            return;
        };

        pivo = pivo_direita;
        pivo_direita = pivo_direita->p;
    }

    return;
}

void print_lista(Lista* lista){
    No* pivo = lista->primeiro;

    printf("[");
    while (pivo->p != NULL){
        printf("%i, ", pivo->v);

        pivo = pivo->p;
    }

    printf("%i]\n", pivo->v);
}

int main2(){
    createList(malloc(PAGE_SIZE), PAGE_SIZE, 0);


    Lista lista;
    lista.primeiro = NULL;
    
    for (int i = 0; i < 25; i++){
        addToLista(&lista, i);
    }

    removeLista(&lista, 4);


    print_lista(&lista);
 
    return 1;
}
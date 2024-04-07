#include "mylloc.h"

#include <stdlib.h>
#include <stdio.h>

void printAddresList(No* posicaoInicial){
    No* guiaAtual = posicaoInicial;

    while (guiaAtual != NULL){
        printf("-------------------------\n");
        printf("Endereço do NO:      %p\n", guiaAtual);
        printf("inicio do Datablock: %p\n", guiaAtual->block->start);
        printf("fim do data block:   %p\n", guiaAtual->block->end);
        printf("memoria free:        %i\n", guiaAtual->block->free);
        printf("Tamanho da memoria:  %li\n", guiaAtual->block->end - guiaAtual->block->start);
        printf("proximo No:          %p\n", guiaAtual->proximo);
        printf("No anterior:         %p\n", guiaAtual->anterior);
        guiaAtual = guiaAtual->proximo;
    };

};

int main(){

    initMemory(malloc(MEMORY_BLOCK_SIZE));

    void* caraDoFinal = mylloc(500);
    No* noPai = (void*) Bloco_Pai - sizeof(No) - 1;


    printf("\n###### INICIO LOOP ######\n");


    void* caraDoMeio2 = mylloc(300);
    void* caraDoMeio = mylloc(350);
    void* caraDoinicio = mylloc(400);

    //myfree(caraDoinicio);
    //myfree(caraDoMeio);
    //myfree(caraDoFinal);
    
    myfree(caraDoFinal);

    mylloc(500);

    printAddresList(noPai);

    myfree(caraDoinicio);
    myfree(caraDoMeio2);
    myfree(caraDoMeio);

    return 0;
}


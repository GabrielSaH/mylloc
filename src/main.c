#include <stdlib.h>
#include <stdio.h>

#include "mylloc.h"

void printNo(DataBlock* no){

        if (no == NULL_MYLLOC){
            return;
        }

        printf("-------------------------\n");
        printf("Endereço do NO:      %p\n", no);
        printf("inicio do Datablock: %p\n", no->inicioData);
        printf("fim do data block:   %p\n", no->inicioData + no->size);
        printf("Tamanho da memoria:  %i\n", no->size);
        printf("proximo No:          %p\n", no->proximo);
        printf("No anterior:         %p\n", no->anterior);
        printf("Proximo No do tipo:  %p\n", no->proximoTipo);
        printf("Anterior No do tipo: %p\n", no->anteriorTipo);
        printf("Estado Do No:        %i\n", no->estado);
}

void printPonteirosFisico(DataBlock* inicio_memoria){
    DataBlock* atual = inicio_memoria;

    while (atual->anterior != NULL_MYLLOC){
        atual = atual->anterior;
    }

    printf("--------- FISICO ----------\n\n\n");
    while (atual != NULL_MYLLOC){
        printNo(atual);
        atual = atual->proximo;
    }    
};

void printLivres(Header* header){

    printf("----------- LIVRES -----------\n\n");
    DataBlock* atual = header->listaLivres;

    while (atual != NULL_MYLLOC){
        printNo(atual);
        atual = atual->proximoTipo;
    }
}

void printOcupados(Header* header){
    printf("--------- OCUPADOS ----------\n\n");
    DataBlock* atual = header->listaOcupados;
    
    while (atual != NULL_MYLLOC){
        printNo(atual);
        atual = atual->proximoTipo;
    }

}

void printVazios(Header* header){
    printf("------------ VAZIOS -----------\n\n");
    DataBlock* atual = header->listaVazios;

    while (atual != NULL_MYLLOC){
        printNo(atual);
        atual = atual->proximoTipo;
    }
}

void printPonteirosTipo(Header* header){
    printf("--------- TIPO ----------\n\n\n");
    
    printLivres(header);
    printOcupados(header);
    printVazios(header);

}



int main(){
    void* memoria_bruta = malloc(PAGE_SIZE);

    int offset = 0;

    void* primeirofisico = memoria_bruta;
    if (offset)
        primeirofisico = memoria_bruta + offset + 2 * sizeof(DataBlock);

    void* ponteiro_header = memoria_bruta + offset + sizeof(DataBlock) * 10;

    createList(memoria_bruta, PAGE_SIZE, offset);


    // void* ponteiro_1 = mylloc(100); 
    // ENTRE ESSES DOIS PONTEIROS ERA PARA FICAR A LISTA
    // void* ponteiro_2 = mylloc(200);  
    // void* ponteiro_3 = mylloc(400); 
    // void* ponteiro_4 = mylloc(600);

    // AQUI FICA O RESTO DO BLOCO


    // myFree(ponteiro_1);
    // myFree(ponteiro_2);
    // myFree(ponteiro_3);
    // myFree(ponteiro_4);

    // void* memoria_inicial = memoria_bruta + 504;

    // void* ponteiros[10] = {};
    // int tamanhos[10] = {};
    // int quantidade = 10;

    // for (int i = 0; i < 10; i++){
    //     ponteiros[i] = memoria_inicial + 100 * i;
    //     tamanhos[i] = 100;
        
    //     printf("%p -> %p\n", ponteiros[i], ponteiros[i] + tamanhos[i]);
    // };


    // bloqueia_pontos(ponteiros, tamanhos, quantidade);

    printOcupados(Primeiro_Header);
    printLivres(Primeiro_Header);
    printVazios(Primeiro_Header);


    printf("\n\n");

    printf("\n\ninicio do bloco de memoria:         %p\n", memoria_bruta);
    if (offset) printf("Inicio da lista:                %p\n", memoria_bruta + offset);
    printf("Fim do bloco de memoria:            %p\n", memoria_bruta + PAGE_SIZE);
    printf("Tamanho do No:                      %li\n", sizeof(DataBlock));
    printf("tamanho Header:                     %li\n", sizeof(Header));

    return 0;
};


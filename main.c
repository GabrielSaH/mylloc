#include "headers.h"
#include <stdlib.h>

#define MEMORY_BLOCK_SIZE 4096
#define _STRINGIFY(s) #s
#define STRINGIFY(s) _STRINGIFY(s)

void* virtual_malloc(){
    return malloc(MEMORY_BLOCK_SIZE);
};

DataBlock* initMemory(void* inicioMemoria){
    // Alocando um bloco de memoria, o tamanho escolhido para o tamanho do bloco foi 2 ** 12    

    DataBlock* bloco;
    
    // Os dados do bloco de memoria sao salvos no inicio do bloco
    bloco = inicioMemoria;
    
    bloco->end = (inicioMemoria + MEMORY_BLOCK_SIZE);
    
    // o start é o inicio utilizavel do bloco, recebe o inicio mais o tamanho do dataBlock
    // ja que o inicio real ta sendo utilizado pelos metadados do bloco
    
    bloco->start = inicioMemoria + sizeof(DataBlock);
    
    bloco->free = 1;
    
    return bloco;
};


int main(){

    DataBlock* teste = initMemory(virtual_malloc());

    printf("Inicio do DataBlock: %p\n", teste->start);
    printf("fim do DataBlock:    %p\n", teste->end);
    printf("local do DataBlock:  %p\n", teste);

    return 0;
}

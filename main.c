#include "headers.h"
#include <stdlib.h>

#define MEMORY_BLOCK_SIZE 4096   // 2 ** 12
#define _STRINGIFY(s) #s
#define STRINGIFY(s) _STRINGIFY(s)


/* ----------SIMULACAO---------- */
/* Bloco de memoria principal usado para simular o gerenciamento*/
DataBlock* Bloco_Pai = NULL;

/* funcao para alocar memoria com o malloc, usada apenas para simular o ambiente */
void* virtual_malloc(){
    return malloc(MEMORY_BLOCK_SIZE);
};

/* ------------NO GUIA------------ */

/* Cria um No na lista de fragmentos de memoria, o No é salvo no inicio do bloco antes dos metadados
do mesmo e sera chamado ao longo do codigo como guia */
void createList(void* inicioMemoria, void* blocoADR){
    No* no = NULL;
    no = inicioMemoria;
    no->proximo = NULL;
    no->anterior = NULL;
    no->block = blocoADR;

};

/* ------------MYLLOC------------ */

/* Por enquanto nao faz nada so serve de placeholder para chamar a função initMemory */
void* mylloc(){
    if (Bloco_Pai == NULL){
        initMemory(virtual_malloc());
    };

    return NULL;
}


/* Inicia o bloco pai, mapeando memoria para poder ser distruibuida com mylloc futuramente.
o bloco é estruturado como segue:
1-> No para a lista de fragmentos
2-> Metadados do bloco de memoria
3-> memoria livre para ser mapeada */
DataBlock* initMemory(void* inicioMemoria){
    // O local dos metadados é depois do guia, por isso "+ sizeof(No) + 1" 
    // (o +1 é para nao dividirem o mesmo endereço)
    Bloco_Pai = inicioMemoria + sizeof(No) + 1;
    
    Bloco_Pai->end = (inicioMemoria + MEMORY_BLOCK_SIZE);

    // O inicio da memoria utilizavel do bloco é depois dos metadados que fica depois do guia
    // (o +2 é porque tem que somar com o +1 de diferença do No) 
    Bloco_Pai->start = inicioMemoria + sizeof(DataBlock) + sizeof(No) + 2;
    
    // 1 == Liberado para uso || 0 == ocupado no momento || 69 == debug
    Bloco_Pai->free = 69;

    createList(inicioMemoria, Bloco_Pai);

    return Bloco_Pai;
};





int main(){

    mylloc();
    mylloc();

    printf("Inicio do DataBlock: %p\n", Bloco_Pai->start);
    printf("fim do DataBlock:    %p\n", Bloco_Pai->end);
    printf("local do DataBlock:  %p\n", Bloco_Pai);

    printf("-------------------------\n");

    No* teste = (void*) Bloco_Pai - sizeof(No) - 1;

    printf("Proximo No:          %p\n", teste->proximo);
    printf("Anterior No:         %p\n", teste->anterior);
    printf("bloco:               %p\n", teste->block);
    printf("endereço NO:         %p\n", teste);
    printf("bloco do No Free:    %i\n", teste->block->free);

    /* Se o codigo estiver funcionando corretamente as seguintes coisas devem ser verdade:
    1 -> fim do data block - endereço no = 0x1000 ou 4096
    2 -> Proximo No = NULL ou (nil)
    3 -> Anterior No = NULL ou (nil)
    4 -> bloco = local do data block
    5 -> bloco do No free = 69 (o numero ainda nao faz nada mas significa que é possivel recuperar
                                data só pelo endereço do bloco)
    */

    return 0;
}

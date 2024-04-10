#include "mylloc.h"

DataBlock* Bloco_Pai = NULL;


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


DataBlock* initMemory(void* inicioMemoria){
    // O local dos metadados é depois do guia, por isso "+ sizeof(No) + 1" 
    // (o +1 é para nao dividirem o mesmo endereço)
    Bloco_Pai = inicioMemoria + sizeof(No) + 1;
    Bloco_Pai->end = (inicioMemoria + MEMORY_BLOCK_SIZE);

    // O inicio da memoria utilizavel do bloco é depois dos metadados que fica depois do guia
    // (o +2 é porque tem que somar com o +1 de diferença do No) 
    Bloco_Pai->start = inicioMemoria + sizeof(DataBlock) + sizeof(No) + 2;
    
    // 1 == Liberado para uso || 0 == ocupado no momento || 69 == debug
    Bloco_Pai->free = 1;

    createList(inicioMemoria, Bloco_Pai);

    return Bloco_Pai;
};

No* buscaGuia(int tamanho){
    No* guia_atual;
    No* guia_ideal;
    guia_atual = (void *) Bloco_Pai - sizeof(No) - 1;
    guia_ideal = (void *) Bloco_Pai - sizeof(No) - 1;
    int atual_tamanho;
    int ideal_tamanho = 0;

    while (guia_atual != NULL){
        if (!(guia_atual->block->free)){
            guia_atual = guia_atual->proximo;
            continue;
        };

        atual_tamanho = (guia_atual->block->end - guia_atual->block->start) + sizeof(DataBlock) + sizeof(No) + 2;

        if ((atual_tamanho < ideal_tamanho || !ideal_tamanho) && atual_tamanho >= tamanho){
            guia_ideal = guia_atual;
            ideal_tamanho = atual_tamanho;
        };
        guia_atual = guia_atual->proximo;
    };

    if (guia_ideal->block->free) return guia_ideal;

    return NULL;
};

void* mymap(int tamanho, No* guia){
    void* inicioNovoBloco = guia->block->end - tamanho;
    void* fimNovoBloco = guia->block->end;
    
    DataBlock* bloco;
    bloco = inicioNovoBloco + sizeof(No) + 1;
    
    
    bloco->end = fimNovoBloco;
    bloco->free = 0;
    bloco->start = inicioNovoBloco + sizeof(DataBlock) + sizeof(No) + 2;    

    createList(inicioNovoBloco, bloco);

    No* lista = inicioNovoBloco;
    lista->anterior = guia;
    lista->proximo = guia->proximo;
    
    if (guia->proximo != NULL){
        guia->proximo->anterior = lista;
    }
    guia->proximo = lista;
    
    guia->block->end = inicioNovoBloco - 1;

    return bloco->start;

};

void* myPerfectmap(No* noPerfeito){
    noPerfeito->block->free = 0;
    
    return noPerfeito;
};

void* mylloc(int tamanho){
    tamanho = tamanho + sizeof(DataBlock) + sizeof(No) + 2; 
    if (Bloco_Pai == NULL){
        10 / 0;
    };

    No* guiaIdeal = buscaGuia(tamanho);
    if (guiaIdeal == NULL){
        return NULL;
    };

    if ( guiaIdeal->block->end - (void*) guiaIdeal == tamanho){
        return myPerfectmap(guiaIdeal);
    };

    return mymap(tamanho, guiaIdeal);
}

void myfree(void* inicioData){
    No* guiaFree = inicioData - sizeof(DataBlock) - sizeof(No) - 2;
    guiaFree->block->free = 1;

    if (guiaFree->anterior != NULL){
        if (guiaFree->anterior->block->free){ // Ou seja: o anterior esta livre
            guiaFree->anterior->proximo = guiaFree->proximo;
            guiaFree->anterior->block->end = guiaFree->block->end; 
            if (guiaFree->proximo != NULL){
                guiaFree->proximo->anterior = guiaFree->anterior;
            };
        }
    }
        /* Caso o anterior esteja ocupado ou nao exista não é necessario fazer nada. */

    if (guiaFree->proximo != NULL){
        if (guiaFree->proximo->block->free){  // Ou seja: o proximo esta livre
            /* Esse algoritmo é complexo e confuso, ele esta plenamente explicado no read me */
            
            guiaFree->proximo->anterior->proximo = guiaFree->proximo->proximo;
            guiaFree->proximo->anterior->block->end = guiaFree->proximo->block->end;
        }

        /* Caso o proximo esteja ocupado ou nao exista não é necessario fazer nada. */
    }    


}



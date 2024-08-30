#include "mylloc.h"

// Definição inicial, sera substituido logo que a função createList for executada.
Header* Primeiro_Header = NULL_MYLLOC;

void createList(void* inicioBloco, int size, int offSetLista){
    
    void* posicaoLista = inicioBloco + offSetLista;
    
    // Salvo no final do primeiro bloco-lista
    Primeiro_Header = posicaoLista + sizeof(DataBlock) * 10;


    int contagem = 0; // contagem de quantos blocos ja foram construidos e ocupados, para ser mais legivel começa com 0 e é incrementado a cada bloco

    // Criação do bloco-lista inicial
    DataBlock* lista = posicaoLista;
    lista->anterior = NULL_MYLLOC;
    lista->proximo = NULL_MYLLOC;
    lista->proximoTipo = NULL_MYLLOC;
    lista->anteriorTipo = NULL_MYLLOC;
    lista->inicioData = posicaoLista;
    lista->size = sizeof(DataBlock) * 10 + sizeof(Header); // O tamanho é de 10 NOs + o tamanho do header, ja que esse fica no final da lista
    lista->estado = 2; // Estado 2 = OCUPADO
    
    contagem++;

    // O bloco a direita é chamado assim por que ele é o bloco que fica logicamente a direita do bloco-lista, ele representa a memoria livre
    // com ecessão do bloco que fica entre o offset e a lista caso esse seja > 0
    DataBlock* blocoDireita = posicaoLista + sizeof(DataBlock);
    blocoDireita->anterior = lista;
    blocoDireita->proximo = NULL_MYLLOC;
    blocoDireita->inicioData = posicaoLista + lista->size; // A data inicia logo que o bloco-lista termina
    blocoDireita->size = size - lista->size;               // O tamanho é tamanho total - bloco-lista (ele é atualizado futuramente se offset > 0) 
    blocoDireita->proximoTipo = NULL_MYLLOC;
    blocoDireita->anteriorTipo = NULL_MYLLOC;
    blocoDireita->estado = 1;                              // Estado 1 = LIVRE

    lista->proximo = blocoDireita;
    
    contagem++;

    // Caso haja offset é necessario criar um bloco a esquerda da lista, esse bloco é do tamanho do offset e esta livre por definição.
    if (offSetLista){
        DataBlock* blocoEsquerda = posicaoLista + 2 * sizeof(DataBlock); // o NO ocupa a terceira posicação na lista, as outras duas ja estão ocupadas
        blocoEsquerda->anterior = NULL_MYLLOC;
        blocoEsquerda->proximo = lista;
        blocoEsquerda->inicioData = inicioBloco;
        blocoEsquerda->size = posicaoLista - inicioBloco;                // Modo alternativo de calcula o offset
        blocoEsquerda->proximoTipo = NULL_MYLLOC;
        blocoEsquerda->anteriorTipo = blocoDireita;
        blocoEsquerda->estado = 1;

        contagem++;

        lista->anterior = blocoEsquerda;
        
        blocoDireita->proximoTipo = blocoEsquerda;
        blocoDireita->size = (inicioBloco + PAGE_SIZE) - (posicaoLista + lista->size);  // Atualizando o tamanho do bloco a direita para considerar um bloco de offset
    };

    // Configurando o primeiro header
    Header* head = Primeiro_Header;
    head->listaOcupados = lista;
    head->listaLivres = blocoDireita;
    head->listaVazios = posicaoLista + contagem * sizeof(DataBlock); 

    DataBlock* anterior = NULL_MYLLOC;

    // cria todos os outros blocos como blocos vazios, isso é, ainda não ha dados neles.
    for (contagem; contagem < 10; contagem++){
        DataBlock* atual = posicaoLista + contagem * sizeof(DataBlock);
        atual->anterior = NULL_MYLLOC;
        atual->proximo = NULL_MYLLOC;
        atual->proximoTipo = NULL_MYLLOC;
        atual->inicioData = NULL_MYLLOC;
        atual->size = 0;
        atual->estado = 0;
        if (anterior != NULL_MYLLOC){
            anterior->proximoTipo = atual;
            atual->anteriorTipo = anterior;
        };

        anterior = atual; // é preciso manter um ponteiro para o bloco anterior para que mantenha-se um controle sobre suas posiçoes fisicas
    };


}

DataBlock* getUltimoNoVazio(){
    return Primeiro_Header->listaVazios;
}

int createNewList(){
    int size = sizeof(DataBlock) * 10;
    DataBlock* blocoIdeal = bestFit(size);
    if (blocoIdeal == NULL_MYLLOC){
        return NULL_MYLLOC;
    }

    DataBlock* blocoVazio = Primeiro_Header->listaVazios; 

    configuraBlocoVazio(blocoVazio, blocoIdeal, size);

    blocoIdeal->inicioData += size;
    blocoIdeal->size -= size;


    if (blocoIdeal->anterior != NULL_MYLLOC){
        blocoIdeal->anterior->proximo = blocoVazio;
    }

    blocoIdeal->anterior = blocoVazio;

    DataBlock* anterior = NULL_MYLLOC;
    void* posicaoLista = blocoVazio->inicioData;

    for (int contagem = 0; contagem < 10; contagem++){
        DataBlock* atual = posicaoLista + contagem * sizeof(DataBlock);
        atual->anterior = NULL_MYLLOC;
        atual->proximo = NULL_MYLLOC;
        atual->proximoTipo = NULL_MYLLOC;
        atual->inicioData = NULL_MYLLOC;
        atual->size = 0;
        atual->estado = 0;
        if (anterior != NULL_MYLLOC){
            anterior->proximoTipo = atual;
            atual->anteriorTipo = anterior;
        };

        anterior = atual;
    };

    Primeiro_Header->listaVazios = blocoVazio->inicioData;
    return 1;

}

DataBlock* bestFit(int tamanho){
    
    DataBlock* atual = Primeiro_Header->listaLivres;
    DataBlock* anterior = NULL_MYLLOC;

    DataBlock* ideal = NULL_MYLLOC;

    
    while(atual != NULL_MYLLOC){
        if (atual->size >= tamanho && (ideal == NULL_MYLLOC || atual->size < ideal->size)){
            ideal = atual;
        };

        anterior = atual;
        atual = atual->proximoTipo;
    };

    if (ideal->size == tamanho){
        perfectFit(ideal);
    }

    return ideal;
}

DataBlock* getNoVazio(){
    if (! Primeiro_Header->listaVazios->proximoTipo){
        if (!createNewList())
            return NULL_MYLLOC;
    };
    return Primeiro_Header->listaVazios;
}

void configuraBlocoVazio(DataBlock* blocoVazio, DataBlock* blocoIdeal, int size){
    
    blocoVazio->inicioData = blocoIdeal->inicioData;
    blocoVazio->size = size;

    blocoVazio->anterior = blocoIdeal->anterior;
    blocoVazio->proximo = blocoIdeal;
    
    blocoVazio->proximoTipo = Primeiro_Header->listaOcupados;
    blocoVazio->proximoTipo->anteriorTipo = blocoVazio;
    blocoVazio->estado = 2;
    Primeiro_Header->listaOcupados = blocoVazio;

    return;
};

void perfectFit(DataBlock* ideal){
    DataBlock* anterior = ideal->anteriorTipo;

    if (anterior == NULL_MYLLOC){
        Primeiro_Header->listaLivres = ideal->proximoTipo;
    }
    else{
        anterior->proximoTipo = ideal->proximoTipo;
        if (ideal->proximoTipo){
            ideal->proximoTipo->anteriorTipo = anterior;
        }
    }

    ideal->estado = 2;
    ideal->proximoTipo = Primeiro_Header->listaOcupados;
    Primeiro_Header->listaOcupados->anteriorTipo = ideal;
    Primeiro_Header->listaOcupados = ideal;
    ideal->anteriorTipo = NULL_MYLLOC;

    return;
}

void* mylloc(int size){
    if (size <= 0){
        return NULL_MYLLOC;
    }


    DataBlock* blocoIdeal = bestFit(size);
    if (blocoIdeal == NULL_MYLLOC){
        return NULL_MYLLOC;
    }

    if (blocoIdeal->size == size){
        return blocoIdeal->inicioData;
    }

    DataBlock* blocoVazio = getNoVazio();

    Primeiro_Header->listaVazios = Primeiro_Header->listaVazios->proximoTipo;
    Primeiro_Header->listaVazios->anteriorTipo = NULL_MYLLOC;

    configuraBlocoVazio(blocoVazio, blocoIdeal, size);

    blocoIdeal->inicioData += size;
    blocoIdeal->size -= size;
    
    if (blocoIdeal->anterior != NULL_MYLLOC){
        blocoIdeal->anterior->proximo = blocoVazio;
    }

    blocoIdeal->anterior = blocoVazio;


    return blocoVazio->inicioData;

}

// Acha um bloco da lista de ocupados com uma busca linear.
// Configura a lista de ocupados para não incluir mais o nó que sera liberado
// Retorna o No ou NULL_MYLLOC caso não o ache
void* getNo_from_inicioData(void* inicioData){

    DataBlock* atual = Primeiro_Header->listaOcupados;

    while (atual->proximoTipo != NULL_MYLLOC){
        if (atual->inicioData == inicioData){
            if (atual->anteriorTipo == NULL_MYLLOC){
                Primeiro_Header->listaOcupados = atual->proximoTipo;
                atual->proximoTipo->anteriorTipo = NULL_MYLLOC;
                return atual;
            };

            atual->anteriorTipo->proximoTipo = atual->proximoTipo;
            atual->proximoTipo->anteriorTipo = atual->anteriorTipo;
            return atual;
        }

        atual = atual->proximoTipo;        
    
    };

    return NULL_MYLLOC;

}

void reset_block(DataBlock* alvo){
            alvo->anterior = NULL_MYLLOC;
            alvo->proximo = NULL_MYLLOC;
            alvo->inicioData = NULL_MYLLOC;
            alvo->proximoTipo = Primeiro_Header->listaVazios;
            alvo->proximoTipo->anteriorTipo = alvo;
            alvo->anteriorTipo = NULL_MYLLOC;
            alvo->size = 0;
            alvo->estado = 0;
            Primeiro_Header->listaVazios = alvo;
}

void* myFree_laterais_livres(DataBlock* noAlvo){
    DataBlock* anterior = noAlvo->anterior;
    DataBlock* proximo = noAlvo->proximo;

    if (anterior->anteriorTipo != NULL_MYLLOC){
        anterior->anteriorTipo->proximoTipo = anterior->proximoTipo;
    }

    if (anterior->proximoTipo != NULL_MYLLOC){
        if (anterior->anteriorTipo == NULL_MYLLOC){
            Primeiro_Header->listaLivres = anterior->proximoTipo;
        }

            anterior->proximoTipo->anteriorTipo = anterior->anteriorTipo;
    }


    if (proximo->anteriorTipo != NULL_MYLLOC){
        proximo->anteriorTipo->proximoTipo = proximo->proximoTipo;
    }

    else if (! proximo->proximoTipo){
        Primeiro_Header->listaLivres = NULL_MYLLOC;
    }
    

    if (proximo->proximoTipo != NULL_MYLLOC){
        if (proximo->anteriorTipo == NULL_MYLLOC){
            Primeiro_Header->listaLivres = proximo->proximoTipo;
        }

            proximo->proximoTipo->anteriorTipo = proximo->anteriorTipo;
    }

    anterior->proximo = proximo->proximo;

    if (anterior->proximo)
        anterior->proximo->anterior = anterior;

    anterior->size += noAlvo->size + proximo->size;
    
    anterior->anteriorTipo = NULL_MYLLOC;
    anterior->proximoTipo = Primeiro_Header->listaLivres;
    if (anterior->proximoTipo)
        anterior->proximoTipo->anteriorTipo = anterior;
    Primeiro_Header->listaLivres = anterior; 

    reset_block(noAlvo);
    reset_block(proximo);

}

void* myFree(void* inicioData){
    DataBlock* noAlvo = getNo_from_inicioData(inicioData);
    if (noAlvo == NULL_MYLLOC){
        return NULL_MYLLOC;
    };

    DataBlock* anterior = noAlvo->anterior;
    DataBlock* proximo = noAlvo->proximo;

    int resetProximo = 0;

    if (anterior && proximo)
        if (anterior->estado == 1 && proximo->estado == 1){
            return myFree_laterais_livres(noAlvo);
        }

    if (anterior){
        if (anterior->estado == 1){
            anterior->proximo = noAlvo->proximo;
            anterior->size += noAlvo->size;
            proximo->anterior = anterior;

            reset_block(noAlvo);

            noAlvo = anterior;
        }
    }

    if (proximo){
        if (proximo->estado == 1){
            noAlvo->proximo = proximo->proximo;
            noAlvo->size += proximo->size;

            noAlvo->proximoTipo = proximo->proximoTipo;
            noAlvo->anteriorTipo = proximo->anteriorTipo;
            

            if (noAlvo->anteriorTipo){
                noAlvo->anteriorTipo->proximoTipo = noAlvo;
            }

            else{
                Primeiro_Header->listaLivres = noAlvo;
            }

            if (noAlvo->proximoTipo){
                noAlvo->proximoTipo->anteriorTipo = noAlvo;
            }


            resetProximo = 1;
        }
    }



    noAlvo->estado = 1;
    if ( !( (proximo && proximo->estado == 1) || (anterior && anterior->estado == 1) ) ){
        noAlvo->proximoTipo = Primeiro_Header->listaLivres;
        noAlvo->proximoTipo->anteriorTipo = noAlvo;
        noAlvo->anteriorTipo = NULL_MYLLOC;
        Primeiro_Header->listaLivres = noAlvo;

    }

    if (resetProximo){
        reset_block(proximo);
    }

    return noAlvo;

}


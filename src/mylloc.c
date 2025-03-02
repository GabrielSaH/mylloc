#include "mylloc.h"
#include <stdio.h>


// Definição inicial, sera substituido logo que a função createList for executada.
Header* Primeiro_Header = NULL_MYLLOC;

void createList(void* inicioBloco, int size, int offSetLista){
    
    void* posicaoLista = inicioBloco + offSetLista;
    
    // Salvo no final do primeiro bloco-lista
    Primeiro_Header = posicaoLista + sizeof(DataBlock) * BLOCK_NUMBER;


    int contagem = 0; // contagem de quantos blocos ja foram construidos e ocupados, para ser mais legivel começa com 0 e é incrementado a cada bloco

    // Criação do bloco-lista inicial
    DataBlock* lista = posicaoLista;
    lista->anterior = NULL_MYLLOC;
    lista->proximo = NULL_MYLLOC;
    lista->proximoTipo = NULL_MYLLOC;
    lista->anteriorTipo = NULL_MYLLOC;
    lista->inicioData = posicaoLista;
    lista->size = sizeof(DataBlock) * BLOCK_NUMBER + sizeof(Header); // O tamanho é de BLOCK_NUMBER NOs + o tamanho do header, ja que esse fica no final da lista
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
    head->finder = 1;

    DataBlock* anterior = NULL_MYLLOC;

    // cria todos os outros blocos como blocos vazios, isso é, ainda não ha dados neles.
    for (contagem; contagem < BLOCK_NUMBER; contagem++){
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


//  Cria um novo bloco-lista com novos BLOCK_NUMBER NOs para serem utilizados
//  Retorna 1 em caso de sucesso ou 0 caso não haja mais memoria disponivel
int createNewList(){
    //  Para criar um novo bloco-lista precisamos primeiro alocar a um NO que representara o novo bloco-lista no bloco-lista anterior
    //  Dessa forma o gerenciado sabera que essa parte esta alocada, como a criação desse novo bloco necessita de varias excessoes
    //  No codigo generico é mais pratico ter a sua função separada.

    int size = sizeof(DataBlock) * BLOCK_NUMBER;          // O tamanho do novo bloco é de BLOCK_NUMBER NOs, ja que ele não necessita de um novo header.
    DataBlock* blocoIdeal = searchBlock(size);  // selecionando o bloco livre
    if (blocoIdeal == NULL_MYLLOC){
        return NULL_MYLLOC;                     // Não ha mais espaço na memoria
    }
    if (blocoIdeal->size == size){
        perfectFit(blocoIdeal);                 // Configurações especiais para caso ambos os tamanhos sejam iguais
    }

    DataBlock* blocoVazio = Primeiro_Header->listaVazios;   // Seleciona manualmente o ultimo bloco livre do header

    configuraBlocoVazio(blocoVazio, blocoIdeal, size);




    // <---------------- configuração padrão de novo bloco-lista ---------------->

    DataBlock* anterior = NULL_MYLLOC;
    void* posicaoLista = blocoVazio->inicioData;

    for (int contagem = 0; contagem < BLOCK_NUMBER; contagem++){
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

    // <------------------------------------------------------------------------>

    Primeiro_Header->listaVazios = blocoVazio->inicioData;      // O header agora aponta para os novos blocos vazios da nova lista.
    return 1;

}


//  Cria um novo bloco-lista, essa função só é chamada caso acabe os blocos livre ENQUANTO o gerenciador esta bloqueando uma lista
//  de ponteiros, passando os dados dos pontos bloqueados ela criara uma nova lista de blocos vazios em um lugar seguro
//  Retorna nullo caso não haja espaço compativel
//  Após bloquear os ponteiros o espaço não utilizado na lista sera liberado.
// 
//  o ponteiro de ponteiro retPointer ira receber um ponteiro apontando para a antiga lista de blocos vazios
DataBlock* createNewList_Safeguard(int qnt, void* ponteiros[], int tamanhos[], DataBlock** retPointer){

    //  A primeira etapa é definir quantos blocos vazios serão necessarios, cada separação de bloco
    //  precisa de no maximo 2 blocos vazios, por isso 2 blocos vazios são separados por ponteiro
    //  (ou seja, por separação)

    int qntPnt = qnt;
    if (qnt < 5) qnt = 5;

    qnt = qnt * 2;
    int size = qnt * sizeof(DataBlock);
    
    //  o firstFit pode retornar um bloco que contenha os locais reservados
    //  por isso é necessario fazer uma separação 
    DataBlock* blocoIdeal = firstFit(size);

    void* divisor = blocoIdeal->inicioData; 
    
    int saltos = 0;

    if (! blocoIdeal){
        return NULL_MYLLOC;
    }

    int continua = 1;
    
    // procurando um espaço no bloco que tenha espaço suficiente e não contenha nenhum ponteiro que sera bloqueado
    while (blocoIdeal){
        for (int i = 0; i < qntPnt; i++){

            //  Caso haja algum ponteiro que comece depois do inicio do bloco e esteja incluido
            //  no bloco alvo
            if (divisor < ponteiros[i] && (divisor + size) > ponteiros[i]){
                divisor = ponteiros[i] + tamanhos[i];
                break;
            };


            //  caso haja algum ponteiro que começa antes do bloco alvo e esteja incluindo no
            //  bloco alvo
            if (divisor < ponteiros[i] + tamanhos[i] && divisor + size > ponteiros[i] + tamanhos[i]){
                divisor = ponteiros[i] + tamanhos[i];
                break;
            };

            //  só acionara a flag continua na ultima interação do for loop
            if (i == qntPnt - 1){
                continua = 0;
            };
        };

        //  Sai do while pois o local ideal ja foi encontrado
        if (!continua){
            break;
        }

        //  o local ideal ainda não foi encontrado, logo é necessario que um outro bloco seja selecionado
        //  caso fosse usado o first fit ele escolheria o mesmo bloco então temos que ignorar os blocos ja examinados
        if ( !(divisor + size <= blocoIdeal->inicioData + blocoIdeal->size) ){
            saltos++;
            blocoIdeal = firstFit_pulos(size, saltos);
            if (blocoIdeal){
                divisor = blocoIdeal->inicioData;
            };
        }

    };

    //  Após os while duas opções são possiveis, um bloco foi selecionado e nesse bloco a um ponteiro "divisor"
    //  que mostra que nenhum ponteiro bloqueado esta reservando aquele local

    if (!blocoIdeal){
        return NULL_MYLLOC;
    };

    
    //  Separa o bloco ideal no divisor, sendo assim, o bloco antes do divisor pode ser liberado para uso no futuro
    //  e o bloco após o divisor sera usado para alocar os blocos vazios
    if ( divisor != blocoIdeal->inicioData){
        blocoIdeal = separa_NO(blocoIdeal, divisor);
    };

    //  para poder liberar os blocos no futuro não podemos usar os blocos livres disponiveis anteriormente
    *retPointer = Primeiro_Header->listaVazios->proximoTipo;

    return createNewList_atBlock(blocoIdeal, qnt);

};

// Cria uma nova lista que comporta "tamanho" blocos no endereco "bloco", alem disso é MUITO importante sabe que essa função
// SUBSTIUI os blocos livre no header, dessa forma se ja ouver outros blocos livres eles serão perdidos
DataBlock* createNewList_atBlock(DataBlock* bloco, int tamanho){
    int size = tamanho * sizeof(DataBlock);
    
    DataBlock* blocoVazio = Primeiro_Header->listaVazios;
    Primeiro_Header->listaVazios = Primeiro_Header->listaVazios->proximoTipo;

    configuraBlocoVazio(blocoVazio, bloco, size);


    // <---------------- configuração padrão de novo bloco-lista ---------------->

    DataBlock* anterior = NULL_MYLLOC;
    void* posicaoLista = blocoVazio->inicioData;

    for (int contagem = 0; contagem < tamanho; contagem++){
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

    // <------------------------------------------------------------------------>

    Primeiro_Header->listaVazios = blocoVazio->inicioData;      // o header agora aponta para os novos blocos vazios da nova lista.

    return blocoVazio;
}

DataBlock* searchBlock(int size){
    switch (Primeiro_Header->finder){
        case 1:
            return( bestFit(size) );

        case 2:
            return( firstFit(size) );

        case 3:
            return( worstFit(size) );

    };
}

//  BestFit escolhe o bloco de memoria livre de menor tamanho mas que ainda é maior ou igual ao alvo
//  Retorna o NO ou NULL_MYLLOC caso não haja memoria o suficiente
DataBlock* bestFit(int tamanho){
    
    DataBlock* atual = Primeiro_Header->listaLivres;

    DataBlock* ideal = NULL_MYLLOC;

    //  Percorre linearmente a lista de blocos vazios, a maior desvantagem do bestFit é a complexidade de tempo N
    while(atual != NULL_MYLLOC){
        if (atual->size >= tamanho && (ideal == NULL_MYLLOC || atual->size < ideal->size)){
            ideal = atual;
        };


        atual = atual->proximoTipo;
    };

    return ideal;
};

//  FirstFit escolhe o primeiro bloco de memoria livre de tamanho maior do que o alvo
//  Retorna NULL_MYLLOC caso não haja memoria o suficiente
DataBlock* firstFit(int tamanho){
    DataBlock* atual = Primeiro_Header->listaLivres;

    while (atual){
        if (atual->size >= tamanho){
            return atual;
        };

        atual = atual->proximoTipo;
    }

    return NULL_MYLLOC;
}

//  realiza o firstfit POREM ignorando os "pulos" primeiros blocos
//  Pulando eles
//  Retorna NULL_MYLLOC caso não haja memoria o suficiente
DataBlock* firstFit_pulos(int tamanho, int pulos){
    DataBlock* atual = Primeiro_Header->listaLivres;
    int pulosDados = 0;

    while (atual){
        if (atual->size >= tamanho){
            if (pulosDados >= pulos){
                return atual;
            }
            
            pulosDados++;
        };

        atual = atual->proximoTipo;
    }

    return NULL_MYLLOC;

};

//  WorstFit percorre a lista e retorna o bloco livre de maior tamanho
//  Retorna NULL_MYLLOC caso não haja memoria o suficiente
DataBlock* worstFit(int tamanho){
    
    DataBlock* atual = Primeiro_Header->listaLivres;
    DataBlock* ideal = NULL_MYLLOC;

    //  Percorre linearmente a lista de blocos vazios
    while(atual != NULL_MYLLOC){
        if (atual->size >= tamanho && (ideal == NULL_MYLLOC || atual->size > ideal->size)){
            ideal = atual;
        };

        atual = atual->proximoTipo;
    };

    return ideal;
}
//  Retorna o primeiro NO da lista de NOs vazios do header
//  Caso só haja um NO vazio criara outro bloco-lista por meio da função createNewList() e então retornara o primeiro bloco vazio da nova lista
DataBlock* getNoVazio(){
    DataBlock* ret = Primeiro_Header->listaVazios;


    Primeiro_Header->listaVazios = Primeiro_Header->listaVazios->proximoTipo;   //  Atualiza o cabeça da lista de NOs vazios
    Primeiro_Header->listaVazios->anteriorTipo = NULL_MYLLOC;

    return ret;
}


//  Checa se ha mais de um bloco vazio, caso contrario cria uma nova lista com novos blocos vazios
//  Retorna 0 se não estiver espaço vazio para uma nova lista
int checaVazios(){
    if (! Primeiro_Header->listaVazios->proximoTipo){
        return createNewList();
    };

};

//  Prepara um bloco vazio para ser ocupado por um novo conjunto de dados, removendo ele da lista de vazios e o colocando na lista de ocupados
//  tambem prepara o bloco e o deixa ja configurado na lista de ocupado, não retorna nada.
void configuraBlocoVazio(DataBlock* blocoVazio, DataBlock* blocoIdeal, int size){
    
    blocoVazio->inicioData = blocoIdeal->inicioData;
    blocoVazio->size = size;

    blocoVazio->anterior = blocoIdeal->anterior;
    blocoVazio->proximo = blocoIdeal;
    
    blocoVazio->proximoTipo = Primeiro_Header->listaOcupados;
    blocoVazio->proximoTipo->anteriorTipo = blocoVazio;
    blocoVazio->estado = 2;
    Primeiro_Header->listaOcupados = blocoVazio;

    blocoIdeal->inicioData += size;
    blocoIdeal->size -= size;

    if (blocoIdeal->anterior != NULL_MYLLOC){
        blocoIdeal->anterior->proximo = blocoVazio;         // Mantendo a coerencia fisica dos blocos
    }

    blocoIdeal->anterior = blocoVazio;

    return;
};


//  Caso o tamanho do bloco livre seja exatamente igual ao tamanho alocado é possivel pular etapas, não sendo necessario preparar o bloco
//  Mas apenas movendo-o da lista de livres para a lista de ocupados, função responsavel por esse processo, transforma o bloco perfeito
//  de maneira que possa ser tratado como um caso generico pelo resto do progama
void perfectFit(DataBlock* ideal){
    DataBlock* anterior = ideal->anteriorTipo;

    retira_lista(ideal);

    ideal->estado = 2;
    ideal->proximoTipo = Primeiro_Header->listaOcupados;    //  O proximo bloco do tipo agora é o primeiro da lista de ocupados
    ideal->proximoTipo->anteriorTipo = ideal; 
    Primeiro_Header->listaOcupados = ideal;                 //  O bloco perfeito agora é a cabeça da lista de ocupados
    ideal->anteriorTipo = NULL_MYLLOC;                      //  Como cabeça ele não pode ter anteriores

    return;
}

//  Retorna um ponteiro para um endereço de memoria do tamanho do argumento.
//  Para liberar o endereço de memoria é necessario usar myFree(ponteiro)
void* mylloc(int size){
    if (size <= 0){             //  Não aceita 0 ou numeros negativos por motivos obvios...
        return NULL_MYLLOC;
    }

    DataBlock* blocoIdeal = searchBlock(size);
    
    if (blocoIdeal == NULL_MYLLOC){         //  Caso não haja memoria disponivel returna um ponteior NULL
        return NULL_MYLLOC;
    }

    if (blocoIdeal->size == size){          //  Caso o bloco seja do tamanho perfeito a função perfectFit ja o prepara completamente
        perfectFit(blocoIdeal);
        return blocoIdeal->inicioData;      //  Tornando um early return possivel
    }

    DataBlock* blocoVazio = getNoVazio();

    if (! checaVazios()){                   //  Checa para ver se tem mais de um bloco livre, caso cnotrario cria uma nova lista
        return 0;
    };

    configuraBlocoVazio(blocoVazio, blocoIdeal, size);

    return blocoVazio->inicioData;

}

// Acha um bloco da lista de ocupados apartir de um ponteiro com uma busca linear.
// Retorna o No ou NULL_MYLLOC caso não o ache
void* getNo_from_ocupados(void* inicioData){

    DataBlock* atual = Primeiro_Header->listaOcupados;

    //  Busca linear pela lista de ocupados para achar o NO que representa a data que o usuario quer liberar, dessa forma
    //  O usuario pode usar myFree diretamente no objeto que esta alocado na memoria sem problemas
    while (atual->proximoTipo != NULL_MYLLOC){
        if (atual->inicioData == inicioData){                           //  Caso ache o NO representante
            return atual;                   
        }

        atual = atual->proximoTipo;        
    
    };

    return NULL_MYLLOC;

}

//  Retira um bloco da sua lista de tipos
void retira_lista(DataBlock* bloco){
    if (bloco->anteriorTipo){
        bloco->anteriorTipo->proximoTipo = bloco->proximoTipo;
        if (bloco->proximoTipo){
            bloco->proximoTipo->anteriorTipo = bloco->anteriorTipo;
        };
        return;
    };

    switch (bloco->estado)
    {
        case 0:
            Primeiro_Header->listaVazios = Primeiro_Header->listaVazios->proximoTipo;
            Primeiro_Header->listaVazios->anteriorTipo = NULL_MYLLOC;
            break;

        case 1:
            Primeiro_Header->listaLivres = Primeiro_Header->listaLivres->proximoTipo;
            Primeiro_Header->listaLivres->anteriorTipo = NULL_MYLLOC;
            break;

        case 2:
            Primeiro_Header->listaOcupados = Primeiro_Header->listaOcupados->proximoTipo;
            Primeiro_Header->listaOcupados->anteriorTipo = NULL_MYLLOC;
            break;
    }
}

//  Reseta todos os dados de um NO e o coloca no top da lista de blocos vazios para poder representar um novo bloco de dados
void reset_block(DataBlock* alvo){
            alvo->anterior = NULL_MYLLOC;
            alvo->proximo = NULL_MYLLOC;
            alvo->inicioData = NULL_MYLLOC;
            alvo->proximoTipo = Primeiro_Header->listaVazios;
            alvo->proximoTipo->anteriorTipo = alvo;
            alvo->anteriorTipo = NULL_MYLLOC;
            alvo->size = 0;
            alvo->estado = 0;                                   //  Estado 0 = LIVRE
            Primeiro_Header->listaVazios = alvo;
}

//  Função excessao para tratar blocos de memoria cujo as duas laterais estão livres, nesse caso os 3 blocos devem ser fundidos em um unico bloco livre
//  O NO anterior recebera o tamanho dos dois proximos NOs que serão resetados e preparados para representar novos blocos de dados
void* myFree_laterais_livres(DataBlock* noAlvo){
    DataBlock* anterior = noAlvo->anterior;         //  BLOCO FISICAMENTE ANTERIOR
    DataBlock* proximo = noAlvo->proximo;           //  BLOCO FISICAMENTE POSTERIOR

    //  Esse é um algoritmo complexo e de dificil leitura, mantenha em mente que esta sendo tratado NOs que pertencem simuntaneamente a duas listas
    //  Que não possuem obrigatoriedade logica entre si, isso é, o bloco anterior FISICAMENTE não necessariamente (e provavelmente) é o anterior de TIPO
    //  Tendo casos onde o anterior FISICO é o proximo de TIPO (e vice versa)

    //  De maneira humana o algoritmo retira os 2 blocos adjacentes da lista de blocos livres e retira tambem o bloco do meio (o alvo que deve ser liberado)
    //  Da lista de ocupados. Em seguida o bloco anterior ao alvo FISICAMENTE é expandido para "Devorar" tanto o bloco alvo quanto o bloco seguinte
    //  (Lembrando que o bloco a direita FISICAMENTE tambem esta livre), o bloco anterior (ou a esquerda) é então recolocado no topo da lista de blocos
    //  Livres e o bloco do meio e o bloco da direita são resetados e colocados na lista de blocos livres  

    // <---------------- Retirando o bloco a ESQUERDA FISICAMENTE da lista de blocos livres ---------------->

    if (anterior->anteriorTipo != NULL_MYLLOC){                             //  Checando se o bloco anterior FISICAMENTE não é o primeiro bloco livre
        anterior->anteriorTipo->proximoTipo = anterior->proximoTipo;            //  Fazendo com que os vizinhos se esqueçam do bloco a esquerda
    }

    if (anterior->proximoTipo != NULL_MYLLOC){                              //  Checando se o bloco anterior FISICAMENTE não é o ultimo bloco livre
        if (anterior->anteriorTipo == NULL_MYLLOC){                             //  Checando novamente se o bloco a esquerda não é o primeiro da lista livre
            Primeiro_Header->listaLivres = anterior->proximoTipo;                   //  Caso seja atualiza o cabeça da lista livre para poder generalizar o codigo
        }

            anterior->proximoTipo->anteriorTipo = anterior->anteriorTipo;       //  Fazendo com que os vizinhos se esqueçam do bloco a esquerda
    }

//  <------------------------------------------------------------------------------------------------------->

//  Para fazer o mesmo com o bloco a esquerda um proceço muito similar é aplicado porem com algumas precauçoes extras

// <---------------- Retirando o bloco a DIREITA FISICAMENTE da lista de blocos livres ---------------->
    
    if (proximo->anteriorTipo != NULL_MYLLOC){                             //   Checando se o bloco seguinte FISICAMENTE não é o primeiro bloco livre
        proximo->anteriorTipo->proximoTipo = proximo->proximoTipo;              //  Fazendo com que os vizinhos se esqueçam do bloco a direita
    }

    else if (! proximo->proximoTipo){                                      //   Caso ele seja o primeiro é necessario retirar ele da primeira posição
        Primeiro_Header->listaLivres = NULL_MYLLOC;
    }
    

    if (proximo->proximoTipo != NULL_MYLLOC){                              //   Chacando se o bloco seguinte FISICAMENTE não é o ultimo bloco livre
        if (proximo->anteriorTipo == NULL_MYLLOC){                              //  Checando novamente se o bloco a direita não é o primeiro da lista livre
            Primeiro_Header->listaLivres = proximo->proximoTipo;                    //  Caso seja atualiza o cabeça da lista livre para poder generalizar o codigo
        }

            proximo->proximoTipo->anteriorTipo = proximo->anteriorTipo;         //  Fazendo com que os vizinhos se esqueçam do bloco a direita
    }

//  <--------------------------------------------------------------------------------------------------->

//  Nesse ponto ambos os blocos da direita e da esquerda devem estar fora da lista de livres embora ainda tenham um ponteiro para seus antigos
//  Vizinhos, só resta então atualizar os blocos FISICAMENTE e fazer com que o bloco a esquerda (anterior) assuma o tamanho dos 3 blocos
//  Nesse ponto o topo da lista de blocos livres é um bloco livre OU NULL caso os 2 blocos adjacentes fossem os unicos livres

    anterior->proximo = proximo->proximo;       //  O proximo do anterior agora é o proximo do bloco a direita, visto que o anterior "Engoliu" os 3 blocos

    if (anterior->proximo)                          //  Checando se o bloco anterior é agora o ultimo bloco FISICAMENTE para evitar erros de fragmentação
        anterior->proximo->anterior = anterior;         //  O proximo bloco agora reconhece o "Mega"-bloco como seu bloco anterior

    anterior->size += noAlvo->size + proximo->size; //  Atualizando o tamanho do bloco anterior para ser a soma dos 3 blocos
    
    anterior->anteriorTipo = NULL_MYLLOC;                   
    anterior->proximoTipo = Primeiro_Header->listaLivres;       // Colocando o "mega"-bloco no topo da lista de blocos livres
    if (anterior->proximoTipo)                              //  Caso o primeiro da lista livre não seja NULL
        anterior->proximoTipo->anteriorTipo = anterior;
    
    Primeiro_Header->listaLivres = anterior;            //  Atualiza o cabeça da lista livre 

    reset_block(noAlvo);            //  Reseta os dois blocos livres
    reset_block(proximo);
}

//  Libera a memoria argumento para poder ser alocado novamente no futuro 
void* myFree(void* inicioData){

    //  A função de liberação é a parte mais complexa do projeto, ao liberar um bloco de dados é necessario um processo chamado de desfragmentação
    //  Isso é, unir blocos de memorias livres que foram separados para alocação em blocos de memorias grandes novamente.
    //  De maneira simplificada, sempre que um bloco de memoria livre possui um vizinho tambem livre eles devem se juntar em um unico bloco livre

    //  a função possui 2 caminhos principais que podem ser tomados
    //  Caso os dois blocos de memoria laterais ao bloco alvo estejam livres é então usado a função myFree_laterais_livres
    //  Caso contrario um unico algoritmo lida com os outros 3 casos possiveis
    // {L O O} | {O O L} | {O O O}  onde O = ocupado e L = livre e o bloco do meio é sempre o que queremos liberar

    //  {L O O} -> executa a parte A e C
    //  {O O L} -> executa a parte B e C
    //  {O O O} -> executa a parte C
    //  {L O L} -> executa a exeção myFree_laterais_livres

    DataBlock* noAlvo = getNo_from_ocupados(inicioData);
    if (noAlvo == NULL_MYLLOC){     // Retorna NULL caso não seja encontrado um bloco que represente o ponteiro alvo
        return NULL_MYLLOC;
    };

    retira_lista(noAlvo);

    DataBlock* anterior = noAlvo->anterior;     //  BLOCO ANTERIOR FISICAMENTE
    DataBlock* proximo = noAlvo->proximo;       //  BLOCO POSTERIOR FISICAMENTE

    int resetProximo = 0;           //  Flag que sera usada para saber o rumo do algoritmo e se é necessario... resetar o proximo kk

    if (anterior && proximo)                                    //  Evitando erros de fragmentação de nucleo
        if (anterior->estado == 1 && proximo->estado == 1){
            return myFree_laterais_livres(noAlvo);              //  Os vizinhos estão livre logo um algoritmo diferente deve ser usado
        }

    //  <------------------------------------------------ PARTE A ------------------------------------------------->

    if (anterior){
        if (anterior->estado == 1){                 //  Caso o bloco anterior esteja livre então ele "Engole" o bloco alvo, tranformando-os em um unico bloco livre
            anterior->proximo = noAlvo->proximo;    //  Esse novo bloco tem como proximo FISICAMENTE o bloco a direita do bloco alvo 
            anterior->size += noAlvo->size;
            proximo->anterior = anterior;           //  O bloco a direita FISICAMENTE do bloco alvo agora entende o "super"-bloco como seu anterior FISICO

            reset_block(noAlvo);                    //  Reseta o NO que representa o noAlvo, esse NO agora esta no topo da lista de vazios

            noAlvo = anterior;                      //  a VARIAVEL noAlvo agora aponta para o "super"-bloco
                                                    //  Novamente, o NO que essa variavel representava agora esta na lista de VAZIOS
        }
    }


    //  <------------------------------------------------ PARTE B ------------------------------------------------>

    if (proximo){
        if (proximo->estado == 1){
            //  Caso o bloco posterior FISICAMENTE esteja livre o NO "noAlvo" engole ele, lembrando que o bloco "noAlvo" não pode estar representando
            //  O NO anterior visto que isso so aconteceria se o anterior e o proximo estivessem livres, o que causaria a função myFree_laterais_livres anteriormente
            
            noAlvo->proximo = proximo->proximo;     //  O "super"-bloco formado pelo "noAlvo" e o seu seguinte tem como proximo bloco o bloco que vinha depois do
                                                    //  bloco "proximo"   
            
            noAlvo->size += proximo->size;          //  O "super"-bloco tem como tamanho a soma do tamanho dos dois blocos

            noAlvo->proximoTipo = proximo->proximoTipo;     //  o "noAlvo" assume a posição do proximo na sua lista de blocos livres, roubando sua identidade
            noAlvo->anteriorTipo = proximo->anteriorTipo;
            

            if (noAlvo->anteriorTipo){                      //  Evitando erros de fragmentação 
                noAlvo->anteriorTipo->proximoTipo = noAlvo;     //  Atualizando os vizinhos do proximo para roubar seu lugar
            }

            else{                                           //  Caso ele nao possua anterior então ele era o cabeça da lista livre que agora é o "noAlvo"
                Primeiro_Header->listaLivres = noAlvo;
            }

            if (noAlvo->proximoTipo){                       //  Evitando erros de fragmentação denovo
                noAlvo->proximoTipo->anteriorTipo = noAlvo;     //  Atualizando o vizinho para receber o noAlvo como novo vizinho
            }


            resetProximo = 1;                               //  Agora que o noAlvo ficou com o proximo resetamos ele, ainda não podemos reseta-lo por causa da parte C
        }
    }

    //  <------------------------------------------------ PARTE C ------------------------------------------------>

    noAlvo->estado = 1;
    if ( !( (proximo && proximo->estado == 1) || (anterior && anterior->estado == 1) ) ){   //  Checando se o bloco alvo nao esta isolado, o if é feito dessa forma
                                                                                            //  Para evitar erros de fragmentação

        noAlvo->proximoTipo = Primeiro_Header->listaLivres;     //  Caso ele esteja isolado simplesmente coloca ele no topo da lista de blocos livres
        noAlvo->proximoTipo->anteriorTipo = noAlvo;
        noAlvo->anteriorTipo = NULL_MYLLOC;
        Primeiro_Header->listaLivres = noAlvo;

    }

    if (resetProximo){              //  Checa se o bloco "proximo" foi devorado no meio do processo para poder reseta-lo
        reset_block(proximo);
    }

    return noAlvo;

};

// Retorna o NO que representa o bloco de memoria que contem o ponteiro requisitado, retorna NULL caso não ache ou caso o ponteiro esteja dividido entre 2 blocos
DataBlock* getNo_from_livres(void* local, int size){
    DataBlock* pivo = Primeiro_Header->listaLivres;

    //  Busca linear entre os blocos livres
    while (pivo != NULL_MYLLOC){
        if ( (pivo->inicioData <= local) && (pivo->inicioData + pivo->size >= local + size) ){ // caso o ponteiro esteja entre um bloco de memoria
            return pivo;
        };

        pivo = pivo->proximoTipo;
    }

    return NULL_MYLLOC;
}

int cont = 0;
// torna um ponteiro um bloco proprio marcado como ocupado
// Retorna a quantidade de blocos vazios que utilizou ou -1 caso nao tenha achado o ponto alvo
int bloqueia_ponto_AUX(void* local, int size){
    
    if (local == NULL_MYLLOC || size <= 0){
        return -1;
    };
    
    // seleciona um bloco da lista de blocos livre
    DataBlock* alvo = getNo_from_livres(local, size);
    
    int qnt_blocosVazios_usados = 0;

    if (!alvo){
        return -1;
    };

    //  Caso o ponteiro seja exatamente o bloco livre inteiro
    if (alvo->inicioData == local && alvo->inicioData + alvo->size == local + size){
        perfectFit(alvo);
        return 0;
    };

    //  Caso o ponteiro esteja no inicio do bloco separa o bloco em dois
    if (alvo->inicioData == local){
        separa_NO(alvo, local + size);
        perfectFit(alvo);
        return 1;
    };

    DataBlock* meio = separa_NO(alvo, local);
    
    //  Caso o bloco separado seja exatamente do tamanho do ponteiro alvo quer dizer que eles são a mesma coisa
    if (meio->inicioData + meio->size == local + size){
        perfectFit(meio);
        return 1;
    }

    //  Caso contrario separa o bloco mais uma vez
    void* divisor = local + size;
    
    DataBlock* direita = separa_NO(meio, local + size);

    perfectFit(meio);

    return 2;

};



int bloqueia_ponto(void* local, int size){

    if (!checaVazios()) return 0;
    return bloqueia_ponto_AUX(local, size);

};


//  Separa um bloco de memoria na posição *separador*, retorna um ponteiro para o novo bloco.
//  A função não checa a quantidade de blocos vazios, lembre de chamar checaVazios antes
DataBlock* separa_NO(DataBlock* alvo, void* separador){
    DataBlock* novo = getNoVazio();
    int size = (alvo->inicioData + alvo->size) - separador;

    //  Configurando o novo bloco.
    novo->inicioData = separador;
    novo->size = size;
    novo->estado = alvo->estado;
    novo->anterior = alvo;
    novo->proximo = alvo->proximo;
    novo->anteriorTipo = alvo;
    novo->proximoTipo = alvo->proximoTipo;

    //  Caso haja um proximo bloco fisico, então o proximo bloco fisico entende o novo bloco como o novo anterior
    if (novo->proximo){
        novo->proximo->anterior = novo;
    };

    //  Caso haja um anterior bloco fisico, então o anterior bloco fisico entende o novo bloco como o novo proximo
    if (novo->proximoTipo){
        novo->proximoTipo->anteriorTipo = novo;
    }

    //  o bloco antigo agora entende o novo como seu proximo, mantendo assim a coerencia fisica
    alvo->proximo = novo;
    alvo->proximoTipo = novo;
    alvo->size -= size;

    return novo;

};


//  Bloquei uma lista de ponteiros, a lista de tamanhos deve estar em ordem com a lista de ponteiros
void bloqueia_pontos(void* ponteiros[], int tamanhos[], int quantidade){


    DataBlock* pivo = Primeiro_Header->listaVazios;
    DataBlock* retPointer = NULL_MYLLOC;
    DataBlock* bloco_lista = NULL_MYLLOC;
    int count = 0;
    int flag_novaLista = 0;
    int blocos_usados = 0;
    int flag_sucesso = 0;

    // contando quantos blocos vazios estão disponiveis
    while (pivo){
        count++;
        pivo = pivo->proximoTipo;
    };
    
    //  Cada bloqueio pode custar ate 2 blocos novos, alem disso o programa precisa ter pelo menos 2 blocos livres a todo momento para garantir o funcionamento.
    //  caso não haja blocos o suficiente uma nova lista é criada APENAS para bloquear os ponteiros

    if ( (quantidade * 2 + 2) > count){
        bloco_lista = createNewList_Safeguard(quantidade, ponteiros, tamanhos, &retPointer);
        flag_novaLista = 1;
    }
    
    // bloqueia todos os blocos usando a função bloqueia pontos aux, é necessario usar a função auxiliar pois no final do processo pode existir exatamente
    // 0 blocos livres, a função bloquei ponto criaria uma nova lista, aqui não é necessario pois o uso de blocos ja foi calculado pela função createNewList_safeguard
    for (int i = 0; i < quantidade; i++){
        flag_sucesso = bloqueia_ponto_AUX(ponteiros[i], tamanhos[i]);
        if (flag_sucesso == -1) return;
        blocos_usados += flag_sucesso;  //  é importante manter conta dos blocos usados para liberar o espaço extra na nova lista depois
    };

    //  calculando a diferença entre o espaço alocado para a lista e o usado, o espaço minimo alocado é de 10 blocos
    int qnt_total = quantidade * 2;
    if (quantidade < 5) qnt_total = 10;
    int blocos_vazios = qnt_total - blocos_usados;
    
    //  liberando o espaço alocado mas não utilizado, dessa forma o numero de blocos alocados é extamente o necessario
    if (flag_novaLista && retPointer){
        Primeiro_Header->listaVazios = retPointer;
        Primeiro_Header->listaVazios->anteriorTipo = NULL_MYLLOC;
        checaVazios();
        
        DataBlock* vaziosLivres = separa_NO(bloco_lista, bloco_lista->inicioData + (bloco_lista->size - blocos_vazios * sizeof(DataBlock)));
                
        myFree(vaziosLivres->inicioData);
        
    }

    return;
}

// troca o metodo de busca
void change_finder(char finder[]){
    if (finder == 'best'){
        Primeiro_Header->finder = 1;
        return;
    };

    if (finder == 'first'){
        Primeiro_Header->finder = 2;
        return;
    };

    if (finder == 'worst'){
        Primeiro_Header->finder = 3;
        return;
    };

}
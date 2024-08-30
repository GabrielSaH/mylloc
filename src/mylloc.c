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


//  Cria um novo bloco-lista com novos 10 NOs para serem utilizados
//  Retorna 1 em caso de sucesso ou 0 caso não haja mais memoria disponivel
int createNewList(){
    //  Para criar um novo bloco-lista precisamos primeiro alocar a um NO que representara o novo bloco-lista no bloco-lista anterior
    //  Dessa forma o gerenciado sabera que essa parte esta alocada, como a criação desse novo bloco necessita de varias excessoes
    //  No codigo generico é mais pratico ter a sua função separada.

    int size = sizeof(DataBlock) * 10;          // O tamanho do novo bloco é de 10 NOs, ja que ele não necessita de um novo header.
    DataBlock* blocoIdeal = bestFit(size);      // Pulando etapas e realizando a chamada do bestfit manualmente
    if (blocoIdeal == NULL_MYLLOC){
        return NULL_MYLLOC;                     // Não ha mais espaço na memoria
    }

    DataBlock* blocoVazio = Primeiro_Header->listaVazios;   // Seleciona manualmente o ultimo bloco livre do header

    configuraBlocoVazio(blocoVazio, blocoIdeal, size);

    blocoIdeal->inicioData += size;             // O inicio do bloco livre agora é o novo bloco-lista, por isso movemos o inicio do bloco livre
    blocoIdeal->size -= size;                   // atualizando o tamanho do bloco livre


    if (blocoIdeal->anterior != NULL_MYLLOC){
        blocoIdeal->anterior->proximo = blocoVazio;         // Mantendo a coerencia fisica dos blocos
    }

    blocoIdeal->anterior = blocoVazio;

    // <---------------- configuração padrão de novo bloco-lista ---------------->

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

    // <------------------------------------------------------------------------>

    Primeiro_Header->listaVazios = blocoVazio->inicioData;      // O header agora aponta para os novos blocos vazios da nova lista.
    return 1;

}

//  BestFit escolhe o bloco de memoria livre de menor tamanho mas que ainda é maior ou igual ao alvo
//  Retorna o NO ou NULL_MYLLOC caso não haja memoria o suficiente
DataBlock* bestFit(int tamanho){
    
    DataBlock* atual = Primeiro_Header->listaLivres;
    DataBlock* anterior = NULL_MYLLOC;

    DataBlock* ideal = NULL_MYLLOC;

    //  Percorre linearmente a lista de blocos vazios, a maior desvantagem do bestFit é a complexidade de tempo N
    while(atual != NULL_MYLLOC){
        if (atual->size >= tamanho && (ideal == NULL_MYLLOC || atual->size < ideal->size)){
            ideal = atual;
        };

        anterior = atual;
        atual = atual->proximoTipo;
    };

    // Caso o bloco seja do exato mesmo tamanho do alvo então um tratamento especial é necessario, chamda de perfectFit
    if (ideal->size == tamanho){
        perfectFit(ideal);
    }

    return ideal;
}

//  Retorna o primeiro NO da lista de NOs vazios do header
//  Caso só haja um NO vazio criara outro bloco-lista por meio da função createNewList() e então retornara o primeiro bloco vazio da nova lista
DataBlock* getNoVazio(){
    if (! Primeiro_Header->listaVazios->proximoTipo){
        if (!createNewList())
            return NULL_MYLLOC;
    };
    return Primeiro_Header->listaVazios;
}

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

    return;
};


//  Caso o tamanho do bloco livre seja exatamente igual ao tamanho alocado é possivel pular etapas, não sendo necessario preparar o bloco
//  Mas apenas movendo-o da lista de livres para a lista de ocupados, função responsavel por esse processo, transforma o bloco perfeito
//  de maneira que possa ser tratado como um caso generico pelo resto do progama
void perfectFit(DataBlock* ideal){
    DataBlock* anterior = ideal->anteriorTipo;

    if (anterior == NULL_MYLLOC){
        Primeiro_Header->listaLivres = ideal->proximoTipo;  //  Caso o bloco livre seja a cabeça da lista transforma o proximo na nova cabeça
    }
    else{
        anterior->proximoTipo = ideal->proximoTipo;         //  Caso contrario força a lista a pular o bloco livre, excluindo-o da lista
        if (ideal->proximoTipo){                            
            ideal->proximoTipo->anteriorTipo = anterior;    //  Os if impedem falha de segmentação por não existir blocos vizinhos
        }
    }

    ideal->estado = 2;
    ideal->proximoTipo = Primeiro_Header->listaOcupados;    //  O proximo bloco do tipo agora é o primeiro da lista de ocupados
    Primeiro_Header->listaOcupados->anteriorTipo = ideal; 
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


    DataBlock* blocoIdeal = bestFit(size);
    if (blocoIdeal == NULL_MYLLOC){         //  Caso não haja memoria disponivel returna um ponteior NULL
        return NULL_MYLLOC;
    }

    if (blocoIdeal->size == size){          //  Caso o bloco seja do tamanho perfeito a função perfectFit ja o preparou completamente
        return blocoIdeal->inicioData;      //  Tornando um early return possivel
    }

    DataBlock* blocoVazio = getNoVazio();

    Primeiro_Header->listaVazios = Primeiro_Header->listaVazios->proximoTipo;   //  Atualiza o cabeça da lista de NOs vazios
    Primeiro_Header->listaVazios->anteriorTipo = NULL_MYLLOC;

    configuraBlocoVazio(blocoVazio, blocoIdeal, size);

    blocoIdeal->inicioData += size;        //   Atualiza o inicio e o tamanho do bloco repartido
    blocoIdeal->size -= size;
    
    if (blocoIdeal->anterior != NULL_MYLLOC){
        blocoIdeal->anterior->proximo = blocoVazio;
    }

    blocoIdeal->anterior = blocoVazio;     //   o "blocoVazio" é o NO que representa a data recem configurada, como ele saiu do inicio do bloco ideal ele esta
                                           //   fisicamente adjacente ao bloco ideal, essa atualização é necessaria para manter a coerencia fisica da lista  


    return blocoVazio->inicioData;

}

// Acha um bloco da lista de ocupados apartir de um ponteiro com uma busca linear.
// Configura a lista de ocupados para não incluir mais o nó que sera liberado
// Retorna o No ou NULL_MYLLOC caso não o ache
void* getNo_from_inicioData(void* inicioData){

    DataBlock* atual = Primeiro_Header->listaOcupados;

    //  Busca linear pela lista de ocupados para achar o NO que representa a data que o usuario quer liberar, dessa forma
    //  O usuario pode usar myFree diretamente no objeto que esta alocado na memoria sem problemas
    while (atual->proximoTipo != NULL_MYLLOC){
        if (atual->inicioData == inicioData){                           //  Caso ache o NO representante
            if (atual->anteriorTipo == NULL_MYLLOC){                        //  Caso seja o primeiro da lista de ocupados reconfigura a lista para o cabeça
                Primeiro_Header->listaOcupados = atual->proximoTipo;        //  Ser o proximo da lista
                atual->proximoTipo->anteriorTipo = NULL_MYLLOC;
                return atual;                                               
            };

            atual->anteriorTipo->proximoTipo = atual->proximoTipo;          //  Caso contrario retira ele da lista de ocupados de maneira simples
            atual->proximoTipo->anteriorTipo = atual->anteriorTipo;
            return atual;                   
        }

        atual = atual->proximoTipo;        
    
    };

    return NULL_MYLLOC;

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
//  Nesse ponto o topo da lista de blocos livres é um bloco vazio OU NULL caso os 2 blocos adjacentes fossem os unicos livres

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

    DataBlock* noAlvo = getNo_from_inicioData(inicioData);
    if (noAlvo == NULL_MYLLOC){     // Retorna NULL caso não seja encontrado um bloco que represente o ponteiro alvo
        return NULL_MYLLOC;
    };

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


            resetProximo = 1;                               //  Agora que o noAlvo ficou com o proximo resetamos ele, ainda não podemos reseta-lo por causa da etapa C
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

}


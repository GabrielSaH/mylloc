#define NULL_MYLLOC (void *) 0
#define PAGE_SIZE 4096
#define BLOCK_NUMBER 10

// <---------------- Structs ---------------->

// Se refere a struct que representa um NO dentro da lista ligada, essa struct suporta ser ao mesmo tempo um No para duas listas distintas
typedef struct dataBlock{
   struct dataBlock* proximo;          // Guarda um ponteiro para o NO que representa o proximo bloco de um ponto de vista FISICO
   struct dataBlock* anterior;         // Guarda um ponteiro para o NO que representa o bloco anterior de um ponto de vista FISICO
   struct dataBlock* proximoTipo;      // Guarda o proximo bloco do seu tipo, isso é {Livre, Ocupado, Vazio}
   struct dataBlock* anteriorTipo;     // Guarda o bloco anterior do seu tipo, isso é {Livre, Ocupado, Vazio}
   void* inicioData;                   // Onde começa a memoria que sera disponibilizada ao usuario
   int size;                           // Tamanho que esse bloco representaa
   int estado;                         // Estados possiveis são { 0: VAZIO, 1: LIVRE, 2: OCUPADO }
} DataBlock;

// A struct que guarda o endereço dos primeiros indices, seu local é logo em seguida ao bloco que guarda a primeira lista
typedef struct header{
   struct dataBlock* listaOcupados;
   struct dataBlock* listaLivres;
   struct dataBlock* listaVazios;
   int finder;
} Header;

// <---------------- Globals ---------------->

// Variavel quer guarda o local do primeiro header, quando multiplos blocos de memoria estiverem disponiveis a ideia é
// ligar os headers para um gerenciamento mais homegenio
extern Header* Primeiro_Header;


// <---------------- Funçoes ---------------->


// Cria a primeira lista com 10 NOS vazios, importante lembrar que o primeiro NO da lista contem o endereço da lista, por isso ele nao pode ser liberado
// offSetLista é a distancia entre o inicio do bloco a ser gerenciado e a lista, pode ser 0
void createList(void* inicioBloco, int size, int offSetLista);

// Algoritmos do modo que o bloco de memoria livre sera escolhido

// BestFit escolhe o bloco de memoria livre de menor tamanho mas que ainda é maior ou igual ao alvo
//  Retorna o NO ou NULL_MYLLOC caso não haja memoria o suficiente
DataBlock* bestFit(int tamanho);

// Retorna o primeiro NO vazio do header EXCETO em casos onde só ha um NO vazio, nesse caso uma nova lista é alocada para o ultimo NO vazio e estes novos
// NOs vazios são colocado no topo da lista de NOs vazios do header
DataBlock* getNoVazio();

// Função a nivel usuario que permite a alocação de memoria baseade apenas no tamanho alvo, recomendado o uso da função base sizeof()
void* mylloc(int size);

// Recebe um ponteiro void* para um bloco de memoria e não retorna nada.
// O bloco enviado é dado como livre, ele não sera apagado mas sim liberado para que possa ser sobre-escrito, o usuario não tem controle sobre quando
// um bloco é sobre-escrito ou não
void* myFree(void*);

// Retorna o NO que representa o bloco de memoria que contem o ponteiro requisitado, retorna NULL caso não ache ou caso o ponteiro esteja dividido entre 2 blocos
DataBlock* acha_ponto(void* local, int size);

// Marca um ponteiro como um bloco proprio marcado como ocupado
int bloqueia_ponto(void* local, int size);

//  Separa um bloco de memoria na posição *separador*
//  A função não checa a quantidade de blocos vazios, use checaVazios para isso
//  Retorna um ponteiro para o novo bloco
DataBlock* separa_NO(DataBlock* alvo, void* separador);

//  Bloquei uma lista de ponteiros como ja utilizados, assim nenhuma outra alocação ira ser feita nesse espaço de memoria
//  a lista tamanhos deve estar em ordem para representa os ponteiros 
//   
//  EX: para bloquear os ponteiros 0x0000 e 0x10000 de tamanhos 200 e 300
//      ponteiros = [0x0000, 0x10000]
//      tamanhos =  [200, 300]
//      quantidade = 2
void bloqueia_pontos(void* ponteiros[], int tamanhos[], int quantidade);


//  Cria um novo bloco-lista, essa função só é chamada caso acabe os blocos livre ENQUANTO o gerenciador esta bloqueando uma lista
//  de ponteiros, passando os dados dos pontos bloqueados ela criara uma nova lista de blocos vazios em um lugar seguro
// 
//  Após bloquear os ponteiros o espaço não utilizado na lista sera liberado.
//  Retorna nullo caso não haja espaço compativel
//  o ponteiro de ponteiro retPointer ira receber um ponteiro apontando para a antiga lista de blocos vazios
DataBlock* createNewList_Safeguard(int qnt, void* ponteiros[], int tamanhos[], DataBlock** retPointer);

//  FirstFit escolhe o primeiro bloco de memoria livre de tamanho maior do que o alvo
//  Retorna NULL_MYLLOC caso não haja memoria o suficiente
DataBlock* firstFit(int tamanho);

//  realiza o firstfit POREM ignorando os "pulos" primeiros blocos
//  Pulando eles
//  Retorna NULL_MYLLOC caso não haja memoria o suficiente
//  
//  EX: firstFit_pulos(x, 2) retornaria o 2 bloco de tamanho maior ou igual a x, ignorando o primeiro
DataBlock* firstFit_pulos(int tamanho, int pulos);


// Cria uma nova lista que comporta "tamanho" blocos no endereco "bloco", alem disso é MUITO importante saber que essa função
// SUBSTIUI os blocos livre no header, dessa forma se ja ouver outros blocos livres eles serão perdidos
DataBlock* createNewList_atBlock(DataBlock* bloco, int tamanho);

// Muda o metodo de busca
// first = first fit
// best = Best fit
// worst = Worst fit
void change_finder(char finder[]);

// Retorna um bloco livre que comporte o tamanho escolhido usando o metodo de busca padrão atual
DataBlock* searchBlock(int size);

// Metodo de busca que retorna o maior bloco de tamanho maior que o alvo
DataBlock* worstFit(int tamanho);

// checa se a lista de blocos vazios possui pelo menos 2 blocos vazios disponiveis
// caso contrario criara uma nova lista que possui novos 10 blocos vazios
int checaVazios();

// Bloquei um unico ponto de tamanho "size" de ser utilizado pelo programa, para libera-lo novamente
// basta usar myfree no mesmo ponteiro
int bloqueia_ponto(void* local, int size);

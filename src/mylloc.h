#define NULL_MYLLOC (void *) 0
#define PAGE_SIZE 4096

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

DataBlock* separa_NO(DataBlock* alvo, void* separador);

void bloqueia_pontos(void* ponteiros[], int tamanhos[], int quantidade);

DataBlock* createNewList_Safeguard(int qnt, void* ponteiros[], int tamanhos[], DataBlock** retPointer);

DataBlock* firstFit(int tamanho);

DataBlock* firstFit_pulos(int tamanho, int pulos);

DataBlock* createNewList_atBlock(DataBlock* bloco, int tamanho);

void change_finder(char finder[]);

DataBlock* searchBlock(int size);


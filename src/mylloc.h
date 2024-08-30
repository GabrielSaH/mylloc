#define NULL_MYLLOC (void *) 0
#define PAGE_SIZE 4096



typedef struct dataBlock{
   struct dataBlock* proximo;
   struct dataBlock* anterior;
   struct dataBlock* proximoTipo;
   struct dataBlock* anteriorTipo;
   void* inicioData;
   int size;
   int estado;
} DataBlock;


typedef struct header{
   struct dataBlock* listaOcupados;
   struct dataBlock* listaLivres;
   struct dataBlock* listaVazios;
} Header;

extern Header* Primeiro_Header;

void createList(void* inicioBloco, int size, int offSetLista);

DataBlock* bestFit(int tamanho);

DataBlock* getNoVazio();

void* mylloc(int size);

void* myFree(void*);

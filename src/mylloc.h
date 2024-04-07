
#define MEMORY_BLOCK_SIZE 4096   // 2 ** 12
#define NULL (void *) 0


typedef struct dataBlock {
    void* start;
    void* end;
    int free;

} DataBlock;


typedef struct no{
    struct no* proximo;
    struct no* anterior;
    DataBlock* block;

} No;


extern DataBlock* Bloco_Pai;



void createList(void* inicioMemoria, void* blocoADR);
DataBlock* initMemory(void* inicioMemoria);
No* buscaGuia(int tamanho);
void* mymap(int tamanho, No* guia);
void* myPerfectmap(No* noPerfeito);
void* mylloc(int tamanho);
void myfree(void* inicioData);



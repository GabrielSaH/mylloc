
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


/* Cria e prepara a struct No para represetar o bloco de memoria, nao e colacado nenhum dado nela 
ainda*/
void createList(void* inicioMemoria, void* blocoADR);

/* Inicia o bloco pai, mapeando memoria para poder ser distruibuida com mylloc futuramente.
o bloco é estruturado como segue:
1-> No para a lista de fragmentos
2-> Metadados do bloco de memoria
3-> memoria livre para ser mapeada */
DataBlock* initMemory(void* inicioMemoria);

/* Busca a guia ideal para o tamanho de memoria, a guia ideal é aquela que possui o menor
valor mas que ainda sejaa maior que o tamanho alvo*/
No* buscaGuia(int tamanho);

/* separa o No que foi passado para que seja do tamanho que foi solicitado + os metadados do bloco
essa função retorna um bloco de memoria completamente pronto para uso*/
void* mymap(int tamanho, No* guia);

/* funcao para preparar um bloco de memoria caso o No seja do exato tamanho solicitado
essa função retorna um bloco de memoria completamente pronto para uso*/
void* myPerfectmap(No* noPerfeito);

/* aloca a quantidade de memoria solicitada, retorna um ponteiro void* de memoria.*/
void* mylloc(int tamanho);

/* Libera uma quantidade de memoria para poder ser utilizada novamente no futuro*/
void myfree(void* inicioData);



#include <stdio.h>


typedef struct dataBlock {
    void* start;
    void* end;
    int free;

} DataBlock;

DataBlock* initMemory();
/* initialize a memory block with malloc to simulate a enviroment in which memory is alredy allocated
*/

typedef struct no{
    struct no* proximo;
    struct no* anterior;
    DataBlock* block;

} No;

typedef struct doubleKeyList
{
    No* primeiro;
    No* ultimo;

} DoublekeyList;


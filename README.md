## Resumo do projeto
Bem-vindo ao projeto de Gerenciador de Memória para nosso Sistema Operacional Hipotético! Este é um projeto open-source voltado para o desenvolvimento de um gerenciador de memória simples porem robusto, sendo efetuado para a disciplina de Sistemas operacionais da faculdade Instituto Federal Catarinense campus Blumenau. O projeto sera desenvolvido em C tendo em mente um sistema operacional para os proccessadores da linha 80/86, apenas sera desenvolvido um GERENCIADOR de memoria e não um ALOCADOR de memoria.



## DEV LOG
Aqui, compartilharemos nossos pensamentos, ideias e progresso ao longo do desenvolvimento do algoritmo para o gerenciador de memória de nosso Sistema Operacional.

Este espaço servirá como um diário, onde documentaremos os desafios encontrados, as soluções propostas e as decisões tomadas durante o processo de desenvolvimento

### Dia 1 (03/03/24)
Primerio commit, inicio do desenvolvimento e primeira implementação funcional. pela necessidade dos outros grupos de usar um gerenciados de memoria o desenvolvimento e implementação foi adiantado,apos ler o relatorio da ultima implementação do gerenciamento de memoria cheguei a conclusão que a maneira mais efetiva de criar uma implementação rapida e facil era usar listas duplamente encadeadas que serviram como guia para qual parte da memoria esta disponivel. Ainda no artigo vi e entendi a necessidade de guardar metadados sobre os blocos de memoria. Por isso no primeiro estagio de desenvolvimento foram criadas as structs No, DataBlock e DoubleKeyList.

#### Structs
Algumas structs foram implementadas para o desenvolvimento dos gerenciamento de dados, sua implementação e sua utilidade esta detalhada a seguir.

##### No 
é o index da lista e se constitui de um ponteiro para o proximo No, um para o anterior e um ponteio para o bloco de memoria que ele esta representando, ao longo do codigo e desse guia chamarei um No que esta gerenciando um bloco como "guia". Dessa forma, a implementação esta como segue:
No* Proximo;    Proximo No atrelado a um bloco de memoria
No* anterior;     O No anterior tambem atrelado a um bloco de memoria
DataBlock* bloco;     O bloco de memoria que esse no representa.

Importante ressaltar que um No so pode existir se ele representa um bloco de memoria. dessa forma, cada no esta ligado a uma porção de memoria.

##### Data block
são os metadados do bloco de memoria, neles são guardados onde o bloco começa (ou seja, o inicio da memoria DISPONIVEL para alocação), se ele esta livre e onde ele termina. Dessa forma, a implementação esta como segue:  
void* start;     O inicio da parte LIVRE de memoria, isso é, a parte que sera disponibilizada para o usuario.  
void* end;     O fim da parte LIVRE da memoria, isso é, um ponteiro para o utlimo byte que o usuario podera utilizar  
int free;     Nao existe booleano em C nativo, logo, 1 = memoria esta livre | 0 = memoria esta ocupada  
  
Ponteiros nulos são ponteiros para partes brutas de memoria que ainda nao foram tipadas, ou seja, partes da memoria que ainda nao possuem um tipo especifico 
  
##### DoubleKeyList
no planejamento da primeira implementação achei que seria vantajoso guardas o primeiro e o ultimo no de uma lista de blocos, porem no decorrer do dia senti que não era necessario, por isso a struct ainda nao esta sendo usada.

#### Funçoes
Aqui sera tratado quais funçoes foram implementadas no primeiro dia, para que elas servem, e como os seus algoritmos funcionam.

##### void* initMemory(void* inicioMemoria);
junto com as struct foi implementado a primeira função para iniciar um bloco de 
emoria. vale ressaltar que para simular a ALOCAÇÃO de momeria estamos usando malloc, visto que o kernel de um sistema possui a sua disponibilidade qualque endereço, ou seja,
quando é requisitado em um programa C normal um endereço ainda nao alocado (em outras palavras, um endereço acima da break line ou fora da stack) tera um falha de segmentação, porem, um kernel ou um OS de um computador tem acesso a qualquer parte da memoria, desse modo é necessario que aloquemos memoria para simular um ambiente de manipulação livre de memoria, outra opçao seria alocar memoria usando blocos de memoria STATIC, porem essa opção foi descartata devido ao baixo desempenho.

a função esta organizando o primeiro bloco de memoria, chamado de bloco_pai da seguinte forma:
![Memoria_Mapa](https:  github.com/GabrielSaH/mylloc/assets/102604902/e12fc961-9075-4586-ab9a-236df4f875fa)

a primeira vez que o mylloc for chamado ele checara se a variavel global Bloco_pai esta pronta, caso ela nao esteja ele chamara a função initMemory, que configura o bloco pai como um bloco comum de memoria. IMPORTANTE manter em mente que o bloco pai em si é apenas um bloco de memoria assim como qualquer outro.

##### void createList(void* inicioMemoria, void* blocoADR)
é a função que configura o No que sera atrelado ao bloco de memoria, essa função nao define os vizinhos dos nos mas apenas configura o No para ser utilizado. A posição do No é bem simples, ele começa no primeiro byte da memoria bruta, assim como mostra a figura 1.

##### No* buscaGuia(size_t tamanho)
essa função é responsavel por achar o melhor bloco de memoria disponivel para o tamanho solicitado de memoria, nessa etapa do projeto estamos utilizando o metodo de BEST FIT, o BEST-FIT tem como caracteristicas melhor funcionamento quando a memoria disponivel são blocos grandes e as requisiçoes sao de blocos pequenos, oque nos acreditamos que seria o mais comum em um kernel, para mais informaçoes sobre os algoritmos é recomendado o livro [Operating Systems: Design and Implementation](https:  csc-knu.github.io/sys-prog/books/Andrew%20S.%20Tanenbaum%20-%20Operating%20Systems.%20Design%20and%20Implementation.pdf) ou para uma leitura rapida [esse artigo](https:  www.linkedin.com/advice/1/what-difference-between-best-fit-worst-fit-memory-ripyf)

a implementação é uma busca simples, o algoritmo inicia a busca tendo o bloco pai como ponto de inicio e como No ideal, ele então percorre a lista ate que encontre um valor 
nullo, caso algum dos valores que o guia_atual passar for menor que o bloco_ideal e esteja livre para alocação, esse agora passar a ser o No ideal, que é então retornado.

o motivo do argumento ser do tipo size_t é porque a solicitação de memoria em C segue por padrao uma chamada da função base sizeof [ ex: malloc(sizeof(struct bla)) ] essa função por sua vez retorn uma variavel do tipo size_t, que corresponde ao tamanho em bytes da struct.

##### mymap(size_t tamanho, No* guia)
essa função é a etapa final para alocação de memoria, ela é responsavel por separar um novo bloco de memoria e configurar ele para uso. A implementção é como segue:
primeiramente um esclaricimento, o argumento tamanho se refere ao tamanho do novo bloco de memoria, enquanto a variavel guia se refere ao No* do bloco de memoria ideal para a nova alocação.  
  
1 -> é separado duas variaveis, que é o inicio da memoria utilizavel pelo usuario e o fim da memoria utilizavel pelo usuario.  
2 -> é criada um novo bloco de metadados que é então alocado na posição inicial + o tamanho da struct No + 1 (isso porque a estrutura No deve ficar no inicio do bloco, o +1 é para que o No guia e os metadados nao colidam um com o outro).  
3 -> o bloco de metadados é configurado para uso  
4 -> é chamada a função que cria a lista.  
5 -> a lista é alocada no inicio do bloco de memoria total.  
6 -> é configurado os vizinhos do bloco dentro do No guia  
7 -> retorna um ponteiro PARA O INICIO DA MEMORIA UTILIZAVEL, OU SEJA, PARA DEPOIS DOS METADADOS  
  
  
##### void* mylloc(size_t tamanho)
essa função serve apenas como interface de usuario, ou seja, ela nao faz nada alem de chamar as outras funçoes antes mostradas. sua unica utilidade é desviar a alocação em 2 caminhos diferentes.  
1 -> o bloco pai ainda nao foi iniciado, ao qual mylloc solicita a inicialização atraves da função initMemory, e depois solicita a alocação de memoria  
2 -> o bloco pai ja foi iniciado, ao qual mylloc apenas solicita a alocação do novo bloco.  
essa função retorna um ponteiro nullo que ja foi abordado anteriormente, mas relembrando trata-se de ponteiro para um bloco de memoria bruto cujo ainda nao foi tipado.  
  
##### void myfree(void* inicioData)
essa é a função responsavel por desalocar memoria e permitir que ela seja redistribuida, importante ressaltar que a função nao apaga propriamente a memoria mas apenas permite que o usuario sobreescreva oque estava guardado nela. Normalmente é necessario fazer dois algoritmos, um que libera a memoria e outro que desfragmenta (a memoria fica dividida em blocos pequenos depois de multiplas alocaçoes, o processo de desfazer esses blocos pequenos e juntar eles em blocos maiores é o chamamos de desfragmentação) porem, conseguimos desenvolver um algoritmo que faz os dois ao mesmo tempo, essa é a parte mais complicada do codigo.
existe 5 possiveis casos para desalocação de memoria

1: a memoria a esquerda esta livre e a da direita não  
2: a memoria a esquerda esta livre e a da direita tambem esta livre  
3: a memoria a esquerda esta ocupada e a da direita esta ocupada  
4: a memoria a esquerda esta ocupada e a da direita esta livre  
  
para resolver esses 5 problemas em um so processo foi aplicado uma sequencia de dois algoritmos, a combinação de aplicar as vezes apenas um, os dois ou nenhum é o que torna possivel fazer todos os casos em um unico processo.  
  
Processos padroes:  
antes de iniciarmos os algortimos a variavel block free é alterada para 1, esse proccesso ocorre indeendente dos algortimos a seguir.  
   
O primeiro algoritmo é o algortimo A, ele segue na seguinte forma:  
1 -> Checa se o bloco anterior existe, se não existir nao realizar nada.  
2 -> Checa se o bloco anterior esta livre, se não estiver nao realiza nada.  
3 -> O bloco anterior tem como proximo o bloco que vem depois do bloco atual  
4 -> O Final do bloco anterior passa a ser o final do bloco atual  
  
ou seja, o algoritmo A checa se o bloco anteriro esta livre, caso esteja ele "engole" o bloco atual.  
  
  
O algoritmo B funciona como se segue:  
Para deixar o algoritmo mais facil de entender considere que queremos liberar o bloco B e que ele esta em um grupo de vizinhos q se segue assim:
A-B-C-D  
onde A,B,C estão livres e D esta ocupado.  
  
1 -> checa se o proximo bloco existe, se não estiver nao realiza nada  
2 -> checa se o proximo bloco esta livre, se não estiver nao realiza nada  
OBS: as alteraçoes desse algoritmo são feitas no bloco anterior ao proximo bloco, ou seja, caso nao tenha sido executado o algoritmo A as mudanças são feitas no bloco atual
caso tenha sido feito o algoritmo A as alteraçoes sao feitas no bloco anterior ao que se quer remover.  
3 -> configura o bloco anterior ao proximo bloco para que o seu 'proximo' seja o bloco que vem depois do proximo. [O bloco anterior ao bloco C tem como seu proximo o bloco D]  
4 -> configura o bloco anterior ao proximo bloco para que o seu fim seja no final do proximo bloco.  [O bloco anterior ao bloco C tem o seu fim marcado para o final do bloco C]  
  
Importante ressaltar que caso o algoritmo A tenha sido rodado o bloco C tem como seu anterior o bloco A, caso contrario o seu anterior é o bloco B mesmo.  
o algoritmo B é responsavel por forçar o bloco seguinte do que esta sendo liberado a ser engolido pelo bloco anterior a ele (caso o proximo bloco esteja livre).  

Com esses dois algoritmos resolvemos os 4 casos estipulados originalmente da seguinte forma. considere os blocos A-B-C-D onde queremos liberar o bloco B.  
Caso 1: é executado o algoritmo A, que faz com que o bloco a esquerda devore o bloco a ser liberado. Resultado -> o bloco B some, A-C-D  
Caso 2: é executado o algoritmo A, que deixa a memoria no estado A-C-D, depois é executado o algoritmo B, que faz com que o bloco C seja devorado pelo bloco que vem antes dele, nesse caso o bloco A, resultado final -> o bloco A engole tanto o bloco B quanto o bloco C, A-D    
caso 3: o bloco a ser liberado esta isolado, nesse caso nao ha nada mais que possamos fazer, nenhum algoritmo é executado e o bloco é configurado para bloco livre. Resultado -> A-B-C-D  
Caso 4: é executado o algoritmo B que faz com que o bloco anterior ao Proximo (o bloco anterior a C) devore o proximo (devore o bloco C), nesse caso, o bloco B. Resultado -> A-B-D  

essas foram todas as coisas desenvolvidas no primeiro dia de desenvolvimento.

#### Dia 2 (13/03/24)
Como o desenvolvimento ja tinha alcançado um ponto de uma primeira implementação funcional o segundo dia de desenvolvimento demorou bastante, focamos o tempo entre eles em teoria e planejamento. No segundo dia resolvemos o unico bug que tinha no codigo e implementamos um função para debug. O codigo ja esta basicamente pronto desde o primeiro dia de desenvolvimento por isso a ideia é ou seguir para implementação em um kernel ou começar a estudar como adicionar memoria virtual ao codigo. Talvez os dois.

##### Void* myPerfectmap(No* noPerfeito)
o bug que tinhamos é que quando tentavamos alocar um espaço de memoria exatamente igual a um bloco livre ele encadeava a lista em circulos.  
para resolver foi criada a função myPerfectmap, essa função apenas muda o bloco alvo para ocupado e retorna ele pronto. A função foi adicionada ao mylloc como opção caso o bloco seja do exato tamanho da alocação. 

##### Void printAddresList(No* posicaoInicial)
é um print para lista encadeada simples. funciona muito bem para debugg e é altamente recomendado implementa-la caso queira fazer o proprio alocado, ja que sem ela debuggar é bem dificil.

##### Void* mylloc(size_t tamanho)
foram feitas algumas mudanças no mylloc, principalmente refatorização. Adicionamente foi separa a chamada em mais possiveis casos incluindo agora o caso 3  
3 -> o bloco é do tamanho exato que o usuario requisitou, nesse caso retorna o resultado da funcao myPerfectmap.

#### Dia 3 (08/04/24)
Agora que o gerenciador de memoria esta basicamente pronto foi focado em preparar ele para implementação em um kernel, para isso foi necessario remover todas as bibliotecas dos arquivos mylloc.h e mylloc.c  
isso resultou em alguns problemas, primeiro, o NULL nao esta definido em C nativo, por isso foi necessario redefinir NULL no mylloc.h, alem disso foi necessario trocar todas as funcoes que possuiam o tipo size_t para int, ja que size_t não é definido em C nativo tambem.  
algumas funcoes foram alteradas, elas são

#### void* mylloc(int tamanho):
essa função foi levemente alterada para que ela nao chame mais initMemory internamente, dessa forma, agora é necessario iniciar a memoria por fora do progama.  

#### void* virtualMalloc(int tamanho):
essa função foi completamente removida ja que ela necessitava de malloc, que nao esta definido no C nativo.

#### void printAddresList(No* posicaoInicial):
essa função agora foi movida para o arquivo de teste main.c, isso porque printf nao esta definido em C nativo, dessa forma é possivel seguir usando essa função que é amplamente utilizada para debugging sem prejudicar a implementação no kernel.

#### main()
a função main foi agora movida para o seu arquivo proprio, tornando o mylloc.c e mylloc.h completamente importaveis para o kernel.

#### Mudanças na utilização:
devido a ter movido a main de lugar e visando a utilização em kernel agora é necessario que a função initMemory seja chamada antes de começar a chamar mylloc, ela pede como argumento um ponteiro inicial para qual possa ser atribuido o bloco pai, em um kernel de sistema esse ponteiro sera passado diretamente, dessa forma permitindo a alocação de memoria. Ja em um ambiente de simulação tem que ser passado um malloc como argumento, ja que não é possivel atribuir ponteiros ainda nao alocados fora do kernel do sistema.









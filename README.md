## Resumo do projeto
Bem-vindo ao projeto de Gerenciador de Memória para nosso Sistema Operacional Hipotético! Este é um projeto open-source voltado para o desenvolvimento de um gerenciador de memória simples porem robusto, sendo efetuado para a disciplina de Sistemas operacionais da faculdade Instituto Federal Catarinense campus Blumenau. O projeto sera desenvolvido em C tendo em mente um sistema operacional para os proccessadores da linha 80/86, apenas sera desenvolvido um GERENCIADOR de memoria e não um ALOCADOR de memoria.



## DEV LOG
Aqui, compartilharemos nossos pensamentos, ideias e progresso ao longo do desenvolvimento do algoritmo para o gerenciador de memória de nosso Sistema Operacional.

Este espaço servirá como um diário, onde documentaremos os desafios encontrados, as soluções propostas e as decisões tomadas durante o processo de desenvolvimento

### Dia 0 (03/03/24)
Primerio commit e inicio do desenvolvimento, pela necessidade dos outros grupos de usar um gerenciados de memoria o desenvolvimento e implementação foi adiantado,apos ler o relatorio da ultima implementação do gerenciamento de memoria cheguei a conclusão que a maneira mais efetiva de criar uma implementação rapida e facil era usar listas duplamente encadeadas que serviram como guia para qual parte da memoria esta disponivel. Ainda no artigo vi e entendi a necessidade de guardar metadados sobre os blocos de memoria. Por isso no primeiro estagio de desenvolvimento foram criadas as structs No, DataBlock e DoubleKeyList.
#### No 
é o index da lista e se constitui de um ponteiro para o proximo No, um para o anterior e um ponteio para o bloco de memoria que ele esta representando, ao longo do codigo e desse guia chamarei um No que esta gerenciando um bloco como "guia"

#### Data block
são os metadados do bloco de memoria, neles são guardados onde o bloco começa (ou seja, o inicio da memoria DISPONIVEL para alocação), se ele esta livre e onde ele termina.

#### DoubleKeyList
no planejamento da primeira implementação achei que seria vantajoso guardas o primeiro e o ultimo no de uma lista de blocos, porem no decorrer do dia senti que não era necessario, por isso a struct ainda nao esta sendo usada.

junto com as struct foi implementado a primeira função para iniciar um bloco de 
emoria. vale ressaltar que para simular a ALOCAÇÃO de momeria estamos usando malloc, visto que o projeto é sobre GERENCIAR a memoria.
a função esta organizando o primeiro bloco de memoria, chamado de bloco_pai da seguinte forma:
m![Memoria_Mapa](https://github.com/GabrielSaH/mylloc/assets/102604902/e12fc961-9075-4586-ab9a-236df4f875fa)


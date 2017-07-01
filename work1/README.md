# Trabalho 1 de MATA54, UFBA,  2017.1.

## Considerações gerais
O trabalho foi implementado em C++, versão 11. Para compilá-lo, basta dar o comando `make` na pasta raiz e o executável `main.out` será gerado.
O arquivo que implementa o método é o _src/file.cpp_. O arquivo _src/main.cpp_ faz o manuseio da entrada e saída e possui a definição da variável `TAMANHO_ARQUIVO`. O arquivo _include/file.hpp_ apresenta a estrutura utilizada para os registros e o cabeçalho das funções de _src/file.cpp_.

## Detalhes de implementação
O arquivo é estruturado como uma tabela _hash_, com colisões resolvidas por encadeamento. Diverge do método visto em sala de aula porque mantém uma lista encadeada para o rápido acesso a espaços vazios no arquivo.

### Registros
Os registros são armazenados em _structs_ chamadas `Record`, definidas no arquivo _include/file.hpp_. Possuem membros representando cada um dos atributos da especificação, além de uma variável do tipo `bool` chamada `good`, que representa se o registro está ou não ocupado, e duas variáveis inteiras, `next`, que armazena o índice do espaço no arquivo do próximo registro na lista encadeada ou -1 caso ele não exista, e `prev`, o análogo de `next` para o registro anterior na lista encadeada.

### Manuseio dos espaços livres
Os espaços livres no arquivo são gerenciados com uma lista duplamente encadeada para suas posições, com um ponteiro na memória principal apontando para o primeiro elemento da lista. Com o fim da execução do programa, esse ponteiro é salvo no cabeçalho do arquivo.
Quando o arquivo é criado, essa lista é inicializada com as posições em ordem decrescente, para minimizar divergências da implementação esperada. A inserção de um registro no arquivo gera a remoção da posição livre que ocupa a cabeça da lista e o avanço do ponteiro para a primeira posição livre, caso ainda haja espaço para registros no arquivo. A remoção de um registro faz com que sua posição seja reinserida na lista de espaços livres, na primeira posição.

### Arquivo
O programa inicialmente verifica se o arquivo de caminho `File::filename` (por padrão, _records.log_) existe. Caso não exista, é criado e preenchido com um cabeçalho contendo o ponteiro da primeira posição livre no arquivo, o tamanho do arquivo e registros vazios.

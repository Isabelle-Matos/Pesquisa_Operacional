# Trabalho de Programação Linear Inteira (PLI) - Pesquisa Operacional

## Descrição

Este repositório contém a implementação dos modelos de Programação Linear Inteira (PLI) para os problemas clássicos abordados na disciplina de Pesquisa Operacional, utilizando o **CPLEX (IBM)** integrado com a linguagem de programação **C++**.

Os problemas modelados e resolvidos neste trabalho incluem:

- **Problema de Fluxo de Custo Mínimo (PFCM)**
- **Problema do Transporte (PT)**
- **Problema da Designação (PD)**
- **Problema do Caminho Mínimo (PCM)**
- **Problema de Fluxo Máximo (PFM)**

Cada um desses problemas é implementado de forma separada em suas respectivas pastas. Para cada problema, foram usados os respectivos grafos fornecidos nas aulas e nos arquivos de entrada \`in.txt\`, conforme as especificações.

## Estrutura do Repositório

A estrutura de pastas do repositório é organizada da seguinte maneira:

\`\`\`
/Trabalho_PLI
    /PFCM
        entrada.txt
        Makefile
        main.cpp
    /PT
        entrada.txt
        Makefile
        main.cpp
    /PD
        entrada.txt
        Makefile
        main.cpp
    /PCM
        entrada.txt
        Makefile
        main.cpp
    /PFM
        entrada.txt
        Makefile
        main.cpp
\`\`\`
- **entrada.txt**: Arquivo de entrada contendo os dados necessários para a execução do modelo.
- **Makefile**: Arquivo para automatizar a compilação.
- **main.cpp**: Arquivo principal com o código C++.
- **output.txt**: Arquivo de saída com os resultados da execução do programa.
## Requisitos

Para rodar os modelos, é necessário que você tenha o seguinte ambiente configurado:

- **C++** (compilador compatível, como \`g++\` ou \`clang++\`)
- **ILOG CPLEX**: Software de otimização para resolver problemas de Programação Linear Inteira. Para mais informações sobre a instalação, consulte o [manual do CPLEX](https://www.ibm.com/products/ilog-cplex-optimization-studio).
- **Sistema Operacional**: Linux (disponibilizado no arquivo de instalação).

## Como Executar

1. **Instalar o CPLEX**
2. **Compilar os Códigos**: Navegue até a pasta do problema desejado e compile o código com o comando:

   \`\`\`
   make
   \`\`\`

3. **Executar o Modelo**: Após a compilação, execute o programa com o comando:

   \`\`\`
   ./problema.exe < entrada.txt
   \`\`\`

## Modelos de Problemas

### 1. Problema de Fluxo de Custo Mínimo (PFCM)

Este problema visa minimizar o custo total de fluxo entre os nós em um grafo, considerando as capacidades de cada aresta. A implementação assume que a capacidade mínima de fluxo de cada aresta é zero, simplificando o modelo.

### 2. Problema do Transporte (PT)

O objetivo do Problema do Transporte é determinar a quantidade de mercadoria a ser transportada entre as fontes e os destinos, de forma a minimizar o custo total, respeitando as capacidades de fornecimento e demanda.

### 3. Problema da Designação (PD)

Este problema envolve a alocação de tarefas a pessoas de maneira que minimize o custo total, com a restrição de que cada pessoa seja designada a uma única tarefa e vice-versa.

### 4. Problema do Caminho Mínimo (PCM)

Aqui, o objetivo é encontrar o caminho de custo mínimo entre dois nós em um grafo. As variáveis de decisão são binárias, representando a inclusão ou não de uma aresta no caminho.

### 5. Problema de Fluxo Máximo (PFM)

Neste problema, busca-se maximizar o fluxo entre dois nós em um grafo, considerando as capacidades das arestas. Assim como no PFCM, a capacidade mínima de cada aresta é zero.

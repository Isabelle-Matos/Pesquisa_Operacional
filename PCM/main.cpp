#include <bits/stdc++.h>
#include <ilcplex/ilocplex.h>

using namespace std;
ILOSTLBEGIN //MACRO - "using namespace" for ILOCPEX
#define CPLEX_TIME_LIM 3600 //3600 segundos

struct Aresta {
    int custo;
    bool conectado;
};

struct No {
    int id, valor;
};

int N, M, s, t; // Quantidade de vertices (N) e quantidade de arestas (M)
vector<Aresta> aresta; // vetor de arestas
vector<No> no; // vetor de nos

void cplex(vector<vector<Aresta>> &g) {
    //CPLEX
    IloEnv env; //Define o ambiente do CPLEX

    //Variáveis --------------------------------------------- 
    int i, j, k; //Auxiliares

    //---------- MODELAGEM ---------------
    //Definição - Variáveis de Decisão 2 dimensões (x_ij) binárias
    IloArray<IloNumVarArray> x(env);
    for (i = 0; i < N; i++) {
        x.add(IloNumVarArray(env));
        for (j = 0; j < N; j++) {
            if (g[i][j].conectado == 1) {
                x[i].add(IloIntVar(env, 0, 1));
            } else {
                x[i].add(IloIntVar(env, 0, 0));
            }
        }
    }

    //Definição do ambiente modelo ------------------------------------------
    IloModel model(env);

    //FUNÇÃO OBJETIVO: Minimizar o custo total do transporte ---------------------------------------------
    IloExpr sum(env); // Expression for Sum
    IloExpr sum2(env);

    sum.clear();
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            if (g[i][j].conectado == 1) {
                sum += (g[i][j].custo * x[i][j]); // Somatório do custo de i j * x_i x_j
            }
        }
    }
    model.add(IloMinimize(env, sum)); 

    // RESTRIÇÕES ---------------------------------------------    

    // R1 - Restrição de saída do nó s
    sum.clear();
    for (int j = 0; j < N; j++) {
        if (g[s][j].conectado) {
            sum += x[s][j];
        }
    }
    model.add(sum == 1);

    // R2 - Restrição de chegada no nó t
    sum.clear();
    for (int i = 0; i < N; i++) {
        if (g[i][t].conectado) {
            sum += x[i][t];
        }
    }
    model.add(sum == 1);

    // R3 - Restrição de balanceamento de fluxo para nós intermediários
    for (int k = 0; k < N; k++) {
        if (k != s && k != t) { 
            sum.clear();
            for (int i = 0; i < N; i++) { // oque chega em k
                if (g[i][k].conectado) {
                    sum += x[i][k];
                }
            }
            sum2.clear();
            for (int j = 0; j < N; j++) {
                if (g[k][j].conectado) { // o que sai de k
                    sum2 += x[k][j];
                }
            }
            model.add(sum == sum2); 
        }
    }

    //------ EXECUÇÃO do MODELO ----------
    time_t timer, timer2;
    IloNum value, objValue;
    double runTime;
    string status;

    //Informações ---------------------------------------------    
    printf("--------Informações da Execução:----------\n\n");
    cout << "Uso de memória após criação das variáveis:  " << env.getMemoryUsage() / (1024. * 1024.) << " MB" << endl;

    IloCplex cplex(model);
    cout << "Uso de memória após cplex(Model):  " << env.getMemoryUsage() / (1024. * 1024.) << " MB" << endl;

    //Configuração de parâmetros CPLEX
    cplex.setParam(IloCplex::TiLim, CPLEX_TIME_LIM);

    time(&timer);
    cplex.solve(); //COMANDO DE EXECUÇÃO
    time(&timer2);

    //Resultados
    bool sol = true;

    switch (cplex.getStatus()) {
        case IloAlgorithm::Optimal:
            status = "Ótimo";
            break;
        case IloAlgorithm::Feasible:
            status = "Viável";
            break;
        default:
            status = "Sem solução";
            sol = false;
    }

    cout << endl << endl;
    cout << "Status da FO: " << status << endl;

    if (sol) {
        objValue = cplex.getObjValue();
        runTime = difftime(timer2, timer);

        cout << "Variáveis de decisão: " << endl;
        for (i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                if (g[i][j].conectado == 1) {
                    value = IloRound(cplex.getValue(x[i][j]));
                    printf("x[%d][%d]: %.0lf\n", i, j, value);
                }
            }
        }
        printf("\n");

        cout << "Valor da Função Objetivo = " << objValue << endl;
        printf("..(%.6lf segundos).\n\n", runTime);

    } else {
        printf("Sem solução!\n");
    }

    //Liberar memória
    cplex.end();
    sum.end();

    cout << "Uso de memória antes de finalizar:  " << env.getMemoryUsage() / (1024. * 1024.) << " MB" << endl;
    env.end();
}

int main() {
    //Leitura dos dados:
    //A partir de um arquivo (entrada.txt)
    int i;
    cin >> N >> M;
    vector<vector<Aresta>> g(N, vector<Aresta>(N, {0, false}));  

    // Leitura do grafo
    for (i = 0; i < M; i++) {
        int u, v, c;
        cin >> u >> v >> c;
        g[u][v] = {c, true}; 
    }

    cin >> s >> t;

    cplex(g);

    return 0;
}

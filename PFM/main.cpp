#include <bits/stdc++.h>
#include <ilcplex/ilocplex.h>

using namespace std;
ILOSTLBEGIN //MACRO - "using namespace" for ILOCPEX
#define CPLEX_TIME_LIM 3600 //3600 segundos

struct Aresta {
    int capacidade;
    bool conectado;
};

int N, M, s, t; // Quantidade de vertices (N) e quantidade de arestas (M)
vector<Aresta> aresta; // vetor de arestas

void cplex(vector<vector<Aresta>> &g) {
    IloEnv env;

    int i, j, k;

    IloArray<IloNumVarArray> x(env);
    for (i = 0; i < N; i++) {
        x.add(IloNumVarArray(env));
        for (j = 0; j < N; j++) {
            if (g[i][j].conectado == 1) {
                x[i].add(IloIntVar(env, 0, g[i][j].capacidade));
            } else {
                x[i].add(IloIntVar(env, 0, 0));
            }
        }
    }

    IloModel model(env);
    IloExpr sum(env);
    IloExpr sum2(env);
    sum.clear();
    
    //FUNCAO OBJETIVO: Maximizar o fluxo ---------------------------------------------

    for (i = 0; i < N; i++) {
        if (g[s][i].conectado == 1) {
            sum += x[s][i];
        }
    }
    model.add(IloMaximize(env, sum));

    //RESTRICOES ---------------------------------------------    
    // R1 - Tudo que sai da origem == tudo que chega no destino
    sum2.clear();
    for (i = 0; i < N; i++) {
        if (g[i][t].conectado == 1) { // Fluxos chegando no destino t
            sum2 += x[i][t];
        }
    }
    model.add(sum == sum2);

    // R2 - Conservação de fluxo
    for (int k = 0; k < N; k++) {
        if (k != s && k != t) {
            sum.clear();
            for (int i = 0; i < N; i++) {
                if (g[i][k].conectado) {
                    sum += x[i][k];
                }
            }
            sum2.clear();
            for (int j = 0; j < N; j++) {
                if (g[k][j].conectado) {
                    sum2 += x[k][j];
                }
            }
            model.add(sum == sum2);
        }
    }

    // R3 - Capacidade máxima
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (g[i][j].conectado) {
                model.add(x[i][j] <= g[i][j].capacidade);
            }
        }
    }

    //------ EXECUCAO do MODELO ----------
    time_t timer, timer2;
    IloNum value, objValue;
    double runTime;
    string status;

    //Informacoes ---------------------------------------------	
    printf("--------Informacoes da Execucao:----------\n\n");
    cout << "Memory usage after variable creation:  " << env.getMemoryUsage() / (1024. * 1024.) << " MB" << endl;

    IloCplex cplex(model);
    cout << "Memory usage after cplex(Model):  " << env.getMemoryUsage() / (1024. * 1024.) << " MB" << endl;

    //Setting CPLEX Parameters
    cplex.setParam(IloCplex::TiLim, CPLEX_TIME_LIM);

    time(&timer);
    cplex.solve();//COMANDO DE EXECUCAO
    time(&timer2);

    //Results
    bool sol = true;

    switch(cplex.getStatus()){
        case IloAlgorithm::Optimal: 
            status = "Optimal";
            break;
        case IloAlgorithm::Feasible: 
            status = "Feasible";
            break;
        default: 
            status = "No Solution";
            sol = false;
    }

    cout << endl << endl;
    cout << "Status da FO: " << status << endl;

    if(sol){ 

        objValue = cplex.getObjValue();
        runTime = difftime(timer2, timer);
        
        cout << "Variaveis de decisao: " << endl;
        for( i = 0; i < N; i++ ){
            for(int j = 0; j < N; j++){
                if(g[i][j].conectado == 1){
                    value = IloRound(cplex.getValue(x[i][j]));
                    printf("x[%d][%d]: %.0lf\n", i, j, value);
                }
            }
        }
        printf("\n");
        
        cout << "Funcao Objetivo Valor = " << objValue << endl;
        printf("..(%.6lf seconds).\n\n", runTime);

    }else{
        printf("No Solution!\n");
    }

    //Free Memory
    cplex.end();
    sum.end();
    sum2.end();

    cout << "Memory usage before end:  " << env.getMemoryUsage() / (1024. * 1024.) << " MB" << endl;
    env.end();
}


int main() {
    int i;
    cin >> N >> M;
    vector<vector<Aresta>> g(N, vector<Aresta>(N, {0, false}));

    for (i = 0; i < M; i++) {
        int u, v, c;
        cin >> u >> v >> c;
        g[u][v] = {c, true};
    }

    cin >> s >> t;

    cplex(g);
    return 0;
}

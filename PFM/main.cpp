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
    int numberVar = 0;
    int numberRes = 0;

    IloArray<IloNumVarArray> x(env);
    for (i = 0; i < N; i++) {
        x.add(IloNumVarArray(env));
        for (j = 0; j < N; j++) {
            if (g[i][j].conectado == 1) {
                x[i].add(IloIntVar(env, 0, g[i][j].capacidade));
                numberVar++;
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

    // Execução do modelo
    IloCplex cplex(model);
    cplex.setParam(IloCplex::TiLim, CPLEX_TIME_LIM);

    time_t timer, timer2;
    time(&timer);
    cplex.solve();
    time(&timer2);

    bool sol = true;
    string status;

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

    cout << "Status da FO: " << status << endl;

    if (sol) {
        double objValue = cplex.getObjValue();
        double runTime = difftime(timer2, timer);

        cout << "Objective Function Value: " << objValue << endl;
        cout << "Variable Values:" << endl;

        for (i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                if (g[i][j].conectado == 1) {
                    double value = IloRound(cplex.getValue(x[i][j]));
                    printf("X[%d][%d]: %.6lf\n", i, j, value);
                }
            }
        }

        cout << "Execution Time: " << runTime << " seconds." << endl;
    } else {
        cout << "Sem solução!" << endl;
    }

    cplex.end();
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

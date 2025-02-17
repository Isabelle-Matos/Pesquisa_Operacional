#include <bits/stdc++.h>
#include <ilcplex/ilocplex.h>

using namespace std;
ILOSTLBEGIN
#define CPLEX_TIME_LIM 3600

int N; // Número de agentes/tarefas
vector<vector<int>> custos; // Matriz de custos

void cplex() {
    IloEnv env;
    try {
        IloModel model(env);

        // Variáveis de decisão binárias (x[i][j] = 1 se agente i designado à tarefa j)
        IloArray<IloNumVarArray> x(env);
        for (int i = 0; i < N; i++) {
            x.add(IloNumVarArray(env));
            for (int j = 0; j < N; j++) {
                x[i].add(IloIntVar(env, 0, 1)); // Variáveis binárias
            }
        }

        // Função Objetivo: Minimizar o custo total
        IloExpr fo(env);
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                fo += custos[i][j] * x[i][j];
            }
        }
        model.add(IloMinimize(env, fo));
        fo.end();

        // Restrições ---------------------------------------------------------
        // Cada agente designado a exatamente UMA tarefa
        for (int i = 0; i < N; i++) {
            IloExpr restricao(env);
            for (int j = 0; j < N; j++) {
                restricao += x[i][j];
            }
            model.add(restricao == 1);
            restricao.end();
        }

        // Cada tarefa designada a exatamente UM agente
        for (int j = 0; j < N; j++) {
            IloExpr restricao(env);
            for (int i = 0; i < N; i++) {
                restricao += x[i][j];
            }
            model.add(restricao == 1);
            restricao.end();
        }

        // Execução do modelo
        IloCplex cplex(model);
        cplex.setParam(IloCplex::TiLim, CPLEX_TIME_LIM);

        if (cplex.solve()) {
            cout << "Custo Total: " << cplex.getObjValue() << endl;
            // Exibir designações
            for (int i = 0; i < N; i++) {
                for (int j = 0; j < N; j++) {
                    if (cplex.getValue(x[i][j]) >= 0.99) { // Evitar erros de precisão
                        cout << "Agente " << i << " -> Tarefa " << j << endl;
                    }
                }
            }
        } else {
            cerr << "Nenhuma solução viável!" << endl;
        }

        env.end();
    } catch (IloException& e) {
        cerr << "Erro CPLEX: " << e << endl;
    }
}

int main() {
    cin >> N;
    custos.resize(N, vector<int>(N));

    // Ler matriz de custos
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            cin >> custos[i][j];
        }
    }

    cplex();
    return 0;
}
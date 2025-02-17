#include <bits/stdc++.h>
#include <ilcplex/ilocplex.h>

using namespace std;
ILOSTLBEGIN
#define CPLEX_TIME_LIM 3600

struct Aresta {
    int origem, destino, custo;
};

int N, M, s, t; // Nós, arestas, origem, destino
vector<Aresta> arestas;

void cplex() {
    IloEnv env;
    try {
        IloModel model(env);

        // Variáveis binárias: x[i] = 1 se a aresta i está no caminho
        IloNumVarArray x(env, M, 0, 1, ILOINT);

        // Função Objetivo: Minimizar custo total
        IloExpr fo(env);
        for (int i = 0; i < M; i++) {
            fo += arestas[i].custo * x[i];
        }
        model.add(IloMinimize(env, fo));
        fo.end();

        // Restrições de conservação de fluxo
        for (int u = 0; u < N; u++) {
            IloExpr entrada(env);
            IloExpr saida(env);
            
            for (int i = 0; i < M; i++) {
                if (arestas[i].destino == u) entrada += x[i];
                if (arestas[i].origem == u) saida += x[i];
            }

            if (u == s) {
                model.add(saida - entrada == 1); // Origem: sai 1 fluxo
            } else if (u == t) {
                model.add(entrada - saida == 1); // Destino: entra 1 fluxo
            } else {
                model.add(saida == entrada); // Nós intermediários
            }

            entrada.end();
            saida.end();
        }

        // Execução
        IloCplex cplex(model);
        cplex.setParam(IloCplex::TiLim, CPLEX_TIME_LIM);

        if (cplex.solve()) {
            cout << "Custo mínimo: " << cplex.getObjValue() << endl;
            cout << "Caminho:" << endl;
            for (int i = 0; i < M; i++) {
                if (cplex.getValue(x[i]) > 0.9) {
                    cout << arestas[i].origem << " -> " << arestas[i].destino << endl;
                }
            }
        } else {
            cerr << "Nenhum caminho encontrado!" << endl;
        }

        env.end();
    } catch (IloException& e) {
        cerr << "Erro CPLEX: " << e << endl;
    }
}

int main() {
    cin >> N >> M;
    cin >> s >> t;

    for (int i = 0; i < M; i++) {
        int u, v, c;
        cin >> u >> v >> c;
        arestas.push_back({u, v, c});
    }

    cplex();
    return 0;
}
#include <bits/stdc++.h>
#include <ilcplex/ilocplex.h>

using namespace std;
ILOSTLBEGIN
#define CPLEX_TIME_LIM 3600

struct Aresta {
    int origem, destino, capacidade;
};

int N, M, s, t; // Nós, arestas, origem, destino
vector<Aresta> arestas;

void cplex() {
    IloEnv env;
    try {
        IloModel model(env);

        // Variáveis de fluxo inteiras (0 <= f[i] <= capacidade)
        IloNumVarArray f(env, M);
        for (int i = 0; i < M; i++) {
            f[i] = IloIntVar(env, 0, arestas[i].capacidade);
        }

        // Função Objetivo: Maximizar o fluxo saindo da origem
        IloExpr fo(env);
        for (int i = 0; i < M; i++) {
            if (arestas[i].origem == s) fo += f[i];
        }
        model.add(IloMaximize(env, fo));
        fo.end();

        // Restrições de conservação de fluxo
        for (int u = 0; u < N; u++) {
            if (u == s || u == t) continue; // Ignora origem/destino

            IloExpr entrada(env);
            IloExpr saida(env);
            
            for (int i = 0; i < M; i++) {
                if (arestas[i].destino == u) entrada += f[i];
                if (arestas[i].origem == u) saida += f[i];
            }

            model.add(entrada == saida); // Entrada = Saída
            entrada.end();
            saida.end();
        }

        // Execução
        IloCplex cplex(model);
        cplex.setParam(IloCplex::TiLim, CPLEX_TIME_LIM);

        if (cplex.solve()) {
            cout << "Fluxo Máximo: " << cplex.getObjValue() << endl;
            cout << "Fluxo nas arestas:" << endl;
            for (int i = 0; i < M; i++) {
                cout << arestas[i].origem << " -> " << arestas[i].destino 
                     << ": " << cplex.getValue(f[i]) << "/" 
                     << arestas[i].capacidade << endl;
            }
        } else {
            cerr << "Nenhum fluxo viável!" << endl;
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
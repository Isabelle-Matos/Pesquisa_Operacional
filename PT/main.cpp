#include <bits/stdc++.h>
#include <ilcplex/ilocplex.h>

using namespace std;
ILOSTLBEGIN
#define CPLEX_TIME_LIM 3600

struct Aresta {
    int custo, capacidade;
    bool conectado;
};

int N, M; // N = origens + destinos, M = arestas (origem->destino)
vector<int> ofertas;
vector<int> demandas;
vector<vector<Aresta>> grafo;

void modelagemPT(IloEnv env, IloModel model, IloNumVarArray& x) {
    // Função Objetivo: Minimizar custo total
    IloExpr fo(env);
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (grafo[i][j].conectado) {
                fo += grafo[i][j].custo * x[i*N + j];
            }
        }
    }
    model.add(IloMinimize(env, fo));
    fo.end();

    // Restrições de Oferta
    for (int i = 0; i < ofertas.size(); i++) {
        IloExpr restricao(env);
        for (int j = 0; j < N; j++) {
            if (grafo[i][j].conectado) {
                restricao += x[i*N + j];
            }
        }
        model.add(restricao <= ofertas[i]);
        restricao.end();
    }

    // Restrições de Demanda
    for (int j = 0; j < demandas.size(); j++) {
        IloExpr restricao(env);
        for (int i = 0; i < N; i++) {
            if (grafo[i][j].conectado) {
                restricao += x[i*N + j];
            }
        }
        model.add(restricao >= demandas[j]);
        restricao.end();
    }
}

void executarCPLEX() {
    IloEnv env;
    try {
        IloModel model(env);
        IloNumVarArray x(env);
        
        // Criar variáveis inteiras
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                if (grafo[i][j].conectado) {
                    x.add(IloIntVar(env, 0, grafo[i][j].capacidade));
                } else {
                    x.add(IloIntVar(env, 0, 0)); // Variável fantasma
                }
            }
        }

        modelagemPT(env, model, x);

        IloCplex cplex(model);
        cplex.setParam(IloCplex::TiLim, CPLEX_TIME_LIM);
        
        if (cplex.solve()) {
            cout << "Solução ótima encontrada!" << endl;
            cout << "Custo total: " << cplex.getObjValue() << endl;
            
            // Exibir variáveis
            for (int i = 0; i < N; i++) {
                for (int j = 0; j < N; j++) {
                    if (grafo[i][j].conectado) {
                        cout << "x[" << i << "][" << j << "] = " 
                             << cplex.getValue(x[i*N + j]) << endl;
                    }
                }
            }
        } else {
            cerr << "Nenhuma solução encontrada!" << endl;
        }
    } catch (IloException& e) {
        cerr << "Erro CPLEX: " << e << endl;
    }
    env.end();
}

int main() {
    cin >> N >> M;
    grafo.resize(N, vector<Aresta>(N, {0, 0, false}));

    // Ler arestas (origem, destino, custo, capacidade)
    for (int i = 0; i < M; i++) {
        int origem, destino, custo, capacidade;
        cin >> origem >> destino >> custo >> capacidade;
        grafo[origem][destino] = {custo, capacidade, true};
    }

    // Ler ofertas e demandas
    ofertas.resize(N, 0);
    demandas.resize(N, 0);
    
    for (int i = 0; i < N; i++) {
        int valor;
        cin >> valor;
        if (valor > 0) ofertas[i] = valor;
        else if (valor < 0) demandas[i] = -valor;
    }

    int totalOferta = accumulate(ofertas.begin(), ofertas.end(), 0);
    int totalDemanda = accumulate(demandas.begin(), demandas.end(), 0);

    if (totalOferta != totalDemanda) {
        cerr << "ERRO: Oferta total (" << totalOferta 
            << ") ≠ Demanda total (" << totalDemanda << ")!" << endl;
        return 1;
    }

    executarCPLEX();
    return 0;
}
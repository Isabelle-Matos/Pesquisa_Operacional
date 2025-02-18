#include <bits/stdc++.h>
#include <ilcplex/ilocplex.h>

using namespace std;
ILOSTLBEGIN //MACRO - "using namespace" for ILOCPEX
#define CPLEX_TIME_LIM 3600 //3600 segundos

struct Aresta{
    int capacidade;
    bool conectado;
};

struct No{
    int id, valor;
};

int N, M; // Quantidade de vertices (N) e quantidade de arestas (M)
vector<Aresta> aresta; // vetor de arestas
vector<No> S, D; // S -> Vector de oferta, D -> Vector de demanda, T -> Vector de transbordo

void cplex(vector<vector<Aresta>> &g){
    //CPLEX
    IloEnv env; //Define o ambiente do CPLEX

    //Variaveis --------------------------------------------- 
    int i, j, k; //Auxiliares

    //---------- MODELAGEM ---------------
    //Definicao - Variaveis de Decisao 2 dimensoes (x_ij) não binárias (discretas)
    IloArray<IloNumVarArray> x(env);
    for( i = 0; i < N; i++ ){
        x.add(IloNumVarArray(env));
        for( j = 0; j < N; j++ ){
            if(g[i][j].conectado == 1){
                x[i].add(IloIntVar(env, 0, INT_MAX));
            }else{
                x[i].add(IloIntVar(env, 0, 0));
            }
        }
    }

    //Definicao do ambiente modelo ------------------------------------------
    IloModel model ( env );
    
    // //Definicao do ambiente expressoes, para os somatorios ---------------------------------
    IloExpr sum(env); /// Expression for Sum

    //FUNCAO OBJETIVO: Minimizar o custo total do transporte mínimo ---------------------------------------------
    sum.clear();
    for(i = 0; i< N; i++){
        for(j=0; j< N; j++){
            if(g[i][j].conectado == 1){
                sum += (g[i][j].capacidade * x[i][j]); // Somatório do custo de i j * x_i x_j
            }        
        }  
    }
    model.add(IloMinimize(env, sum)); //Minimizacao 

    //RESTRICOES ---------------------------------------------    
    
    // R1 - Restricao de oferta
    for (int i = 0; i < S.size(); i++) {
        sum.clear();    
        for (int j = 0; j < N; j++) {
            if (g[S[i].id][j].conectado == 1) {
                sum += x[S[i].id][j];
            }
        }
        model.add(sum <= S[i].valor);  
    }
 

    // R2 - Restricao de demanda
    for (int i = 0; i < D.size(); i++) {
    sum.clear();    
    for (int j = 0; j < N; j++) {
        if (g[j][D[i].id].conectado == 1) {
            sum += x[j][D[i].id];
        }
    }
        model.add(sum == D[i].valor);  
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

    cout << "Memory usage before end:  " << env.getMemoryUsage() / (1024. * 1024.) << " MB" << endl;
    env.end();
}

int main(){
    //Leitura dos dados:
    //A partir de um arquivo (entrada.txt)
    int i;
    cin >> N >> M;
    vector<vector<Aresta>> g(N, vector<Aresta>(N, {0, false}));  

    // leitura do grafo
    for(i=0; i< M; i++){
        int u, v, c;
        cin >> u >> v >> c;
        if(c == -1){ // se o custo for -1 significa que ele é inf
            c = INT_MAX;
        }
        g[u][v] = {c, true};
    }
    for(int i=0; i< N; i++){
        int v;
        cin >> v;
        if(v > 0){
            // cout << "Oferta ";
            S.push_back({i, v});  
            // cout << "ID " << S.back().id << " Valor " << S.back().valor << endl;
        }
        else if(v < 0){
            v *= -1;
            // cout << "Demanda ";
            D.push_back({i, v}); 
            // cout << "ID " << D.back().id << " Valor " << D.back().valor << endl;
        }
    }
    // for(auto x: g){
    //     for(auto y: x){
    //         cout << y.capacidade << ' ' << y.conectado << " | ";
    //     }
    //     cout << endl;
    // }

    cplex(g);

    return 0;
}

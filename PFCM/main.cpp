#include <bits/stdc++.h>
#include <ilcplex/ilocplex.h>

using namespace std;
ILOSTLBEGIN //MACRO - "using namespace" for ILOCPEX
#define CPLEX_TIME_LIM 3600 //3600 segundos

struct Aresta{
    int custo, fluxo_maximo;
    bool conectado;
};

struct No{
    int id, valor;
};

int N, M; // Quantidade de vertices (N) e quantidade de arestas (M)
vector<Aresta> aresta; // vetor de arestas
vector<No> no; // vetor de nos
vector<No> S, D, T; // S -> Vector de oferta, D -> Vector de demanda, T -> Vector de transbordo

void cplex(vector<vector<Aresta>> &g){
    //CPLEX
    IloEnv env; //Define o ambiente do CPLEX

    //Variaveis --------------------------------------------- 
    int i, j, k; //Auxiliares
    int numberVar = 0; //Total de Variaveis
    int numberRes = 0; //Total de Restricoes


    //---------- MODELAGEM ---------------
    //Definicao - Variaveis de Decisao 2 dimensoes (x_ij) não binárias (discretas)
    IloArray<IloNumVarArray> x(env);
    for( i = 0; i < N; i++ ){
        x.add(IloNumVarArray(env));
        for( j = 0; j < N; j++ ){
            if(g[i][j].conectado == 1){
                x[i].add(IloIntVar(env, 0, g[i][j].fluxo_maximo));
                numberVar++;
            }else{
                x[i].add(IloIntVar(env, 0, 0));
            }
        }
    }

    //Definicao do ambiente modelo ------------------------------------------
    IloModel model ( env );
    
    // //Definicao do ambiente expressoes, para os somatorios ---------------------------------
    IloExpr sum(env); /// Expression for Sum
    IloExpr sum2(env); /// Expression for Sum2

    //FUNCAO OBJETIVO: Minimizar o custo total do transporte ---------------------------------------------
    sum.clear();
    for(i = 0; i< N; i++){
        for(j=0; j< N; j++){
            if(g[i][j].conectado == 1){
                sum += (g[i][j].custo * x[i][j]); // Somatório do custo de i j * x_i x_j
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
                // cout << "ID " << S[i].id << "VALOR" << S[i].valor << endl;
            }
        }
        sum2.clear();
        for (int k = 0; k < N; k++) {
            if (g[k][S[i].id].conectado == 1) {
                sum2 += x[k][S[i].id];
            }
        }
        model.add(sum - sum2 <= S[i].valor);
        numberRes++;    
    }

    // R2 - Restricao de demanda
    for (int i = 0; i < D.size(); i++) {
    sum.clear();    
    for (int j = 0; j < N; j++) {
        if (g[D[i].id][j].conectado == 1) {
            sum += x[D[i].id][j];
            // cout << "ID " << D[i].id << "VALOR" << D[i].valor << endl;
        }
    }
    sum2.clear();
    for (int k = 0; k < N; k++) {
        if (g[k][D[i].id].conectado == 1) {
            sum2 += x[k][D[i].id];
            }
        }
        model.add(sum - sum2 <= D[i].valor);
        numberRes++;    
    }

    // R3 - Restricao de transbordo
    for (int i = 0; i < T.size(); i++) {
        sum.clear();    
        for (int j = 0; j < N; j++) {
            if (g[T[i].id][j].conectado == 1) {
                sum += x[T[i].id][j];
            }
        }
        sum2.clear();
        for (int k = 0; k < N; k++) {
            if (g[k][T[i].id].conectado == 1) {
                sum2 += x[k][T[i].id];
            }
        }

        model.add(sum - sum2 == 0);
        numberRes++;    
    }

    //R4 - Restricao de capacidade
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            if (g[i][j].conectado == 1) {
                model.add(0 <= x[i][j]); // Limita fluxo entre 0
                model.add(x[i][j] <= g[i][j].fluxo_maximo); // e a capacidade da aresta
                numberRes++;
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
    printf("#Var: %d\n", numberVar);
    printf("#Restricoes: %d\n", numberRes);
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

int main(){
  //Leitura dos dados:
  //A partir de um arquivo (entrada.txt)
  int i, s, d, tipo, no_id, oferta, demanda;
  cin >> N >> M;
  // no.resize(N);
    vector<vector<Aresta>> g(N, vector<Aresta>(N, {-1, -1, false}));  

    // leitura do grafo
    for(i=0; i< M; i++){
        int u, v, c, f_max;
        cin >> u >> v >> c >> f_max;
        if(f_max == -1){ // se o fluxo maximo for infinito (-1)
            f_max = INT_MAX;
        }
        g[u][v] = {c, f_max, true};
        

    }
    for(auto x: g){
        for(auto y: x){
            cout << y.custo << ' ' << y.fluxo_maximo << ' ' << y.conectado << " | ";
        }
        cout << endl;
    }
    
    for(int i=0; i< N; i++){
        int v;
        cin >> v;
        if(v > 0){
            cout << "Oferta ";
            S.push_back({i, v});  
            cout << "ID " << S.back().id << " Valor " << S.back().valor << endl;
        }
        else if(v < 0){
            cout << "Demanda ";
            D.push_back({i, v}); 
            cout << "ID " << D.back().id << " Valor " << D.back().valor << endl;
        }
        else{
            cout << "Transposto ";
            T.push_back({i, 0});  
            cout << "ID " << T.back().id << " Valor " << T.back().valor << endl;
        }
    }
    
   

    cplex(g);

    return 0;
}
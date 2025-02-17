#include <bits/stdc++.h>
#include <ilcplex/ilocplex.h>

using namespace std;
ILOSTLBEGIN //MACRO - "using namespace" for ILOCPEX
#define CPLEX_TIME_LIM 3600 //3600 segundos

struct Aresta{
    int fluxo_maximo, custo;
    bool conectado;
};

struct No{
    int oferta, demanda, transbordo;
};

int N, M; // Quantidade de vertices (N) e quantidade de arestas (M)
vector<Aresta> aresta; // vetor de arestas
vector<No> no; // vetor de nos
vector<int> S, D, T; // S -> Vector de oferta, D -> Vector de demanda, T -> Vector de transbordo


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
            x[i].add(IloIntVar(env, 0, INT_MAX));
            numberVar++;
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
            if(g[i][j].conectado == 1 && g[i][j].custo != INT_MAX){
                sum += (g[i][j].custo * x[i][j]); // Somatório do custo de i j * x_i x_j
            }        
        }  
    }
    model.add(IloMinimize(env, sum)); //Minimizacao 

    //RESTRICOES ---------------------------------------------    

    //RESTRICOES ---------------------------------------------    
    
    // R1 - Restricao de oferta
    for (int i = 0; i < S.size(); i++) {
    sum.clear();    
    for (int j = 0; j < N; j++) {
        if (g[S[i]][j].conectado == 1 && g[S[i]][j].custo != INT_MAX) {
                sum += x[S[i]][j];
            }
        }
        sum2.clear();
        for (int k = 0; k < N; k++) {
            if (g[k][S[i]].conectado == 1 && g[k][S[i]].custo != INT_MAX) {
                sum2 += x[k][S[i]];
            }
        }
            model.add(sum - sum2 <= no[S[i]].oferta);
            numberRes++;    
    }

    // R2 - Restricao de demanda
    for (int i = 0; i < D.size(); i++) {
    sum.clear();    
    for (int j = 0; j < N; j++) {
        if (g[D[i]][j].conectado == 1 && g[D[i]][j].custo != INT_MAX) {
            sum += x[D[i]][j];
        }
    }
    sum2.clear();
    for (int k = 0; k < N; k++) {
        if (g[k][D[i]].conectado == 1 && g[k][D[i]].custo != INT_MAX) {
            sum2 += x[k][D[i]];
            }
        }
        model.add(sum - sum2 <= no[D[i]].demanda);
        numberRes++;    
    }

    // R3 - Restricao de transbordo
    for (int i = 0; i < T.size(); i++) {
    sum.clear();    
    for (int j = 0; j < N; j++) {
        if (g[T[i]][j].conectado == 1 && g[T[i]][j].custo != INT_MAX) {
            sum += x[T[i]][j];
        }
    }
    sum2.clear();
    for (int k = 0; k < N; k++) {
        if (g[k][T[i]].conectado == 1 && g[k][T[i]].custo != INT_MAX) {
            sum2 += x[k][T[i]];
            }
        }
        model.add(sum - sum2 == 0);
        numberRes++;    
    }

    //R4 - Restricao de capacidade
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            if (g[i][j].conectado == 1 && g[i][j].custo != INT_MAX) {
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
        IloNum objValue = cplex.getObjValue();
        cout << "Funcao Objetivo Valor = " << objValue << " seconds.\n";
    
        // Exibe os valores ótimos das variáveis de decisão
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                IloNum value = IloRound(cplex.getValue(x[i][j]));
                printf("x[%d][%d]: %.0lf\n", i, j, value); // Imprime resultado
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
    no.resize(N);
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
    
    for(int i=0; i< N; i++){
        int v;
        cin >> v;
        if(v > 0){
            S.push_back(i);
            no[i].oferta = v;
        }
        else if(v < 0){
            D.push_back(i);
            no[i].demanda = v;
        }
        else{
            T.push_back(i);
        }
    }
   

    cplex(g);

    return 0;
}
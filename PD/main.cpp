#include <bits/stdc++.h>
#include <ilcplex/ilocplex.h>

using namespace std;
ILOSTLBEGIN //MACRO - "using namespace" for ILOCPEX
#define CPLEX_TIME_LIM 3600 //3600 segundos

int N, M; // Quantidade de vertices (N) e quantidade de arestas (M)
vector<vector<int>> custo;


void cplex(vector<vector<int>> &custo){
    //CPLEX
    IloEnv env; //Define o ambiente do CPLEX

    //Variaveis --------------------------------------------- 
    int i, j, k; //Auxiliares
    int numberVar = 0; //Total de Variaveis
    int numberRes = 0; //Total de Restricoes


    //---------- MODELAGEM ---------------
    //VARIAVEIS DE DECISAO (x_i) binaria
    IloArray<IloNumVarArray> x(env);
	for(i = 0; i < N; i++ ){
		x.add(IloNumVarArray(env));
		for( j = 0; j < N; j++ ){
            x[i].add(IloIntVar(env, 0, 1));
			numberVar++;
		}
	}

    //Definicao do ambiente modelo ------------------------------------------
    IloModel model ( env );
    
    // //Definicao do ambiente expressoes, para os somatorios ---------------------------------
    IloExpr sum(env); /// Expression for Sum

    //FUNCAO OBJETIVO: Determinar a designação de menor custo ---------------------------------------------
    sum.clear();
    for(i = 0; i< N; i++){
        for(j=0; j< N; j++){
            sum += (custo[i][j] * x[i][j]); // Somatório do custo de i j * x_i x_j
        }        
    }  
    model.add(IloMinimize(env, sum)); //Minimizacao 

    //RESTRICOES ---------------------------------------------    
    
    // R1 - Restricao de tarefa designada
    for (int i = 0; i < N; i++) {
        sum.clear();
        for (int j = 0; j < N; j++) {
            sum += x[i][j];
        }
        model.add(sum == 1);
        numberRes++;    
    }

    // R2 - Restricao de pessoa designada
    for (int j = 0; j < N; j++) {
        sum.clear();
        for (int i = 0; i < N; i++) {
            sum += x[i][j];
        }
        model.add(sum == 1);
        numberRes++;    
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
                value = IloRound(cplex.getValue(x[i][j]));
                printf("x[%d][%d]: %.0lf\n", i, j, value);
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
    custo.resize(N, vector<int> (N));

    // leitura do grafo
    for(i=0; i< M; i++){
        for(int j = 0; j< N; j++){
            cin >> custo[i][j];
        }
    }

    for(int i=0; i< N; i++){
        for(int j=0; j<N; j++){
            cout << custo[i][j] << " ";
        }
    }

   

    cplex(custo);

    return 0;
}
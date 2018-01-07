#include <chrono>
#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <cstdio>
#include <algorithm>
#include <time.h>
#include <fstream>

using namespace std;
using namespace std::chrono;

/*dado.h*/
struct Dados{
	int n; //numero de vertices
	int m; //numero de arestas 
};

/*lista.h*/
struct Lista{
    int v;  
    int grau;
    vector<int> adj;
	bool visitado = false;
};


/*Arquivo.h*/
int abreArquivo(FILE **, char*);
int carregaArquivo(FILE **, vector<Lista>&);
int carregaTamanho(FILE **, Dados*);

//Grafo.h
void contarGraus(vector<Lista>&);
void ordenarLista(vector<Lista>&);
bool compararPorGraus(const Lista&, const Lista&);

int verificarPropriedades(vector<Lista>&,vector<Lista>&,Dados*,Dados*);
int** gerarMatrizAdj(int,vector<Lista>&);


//Verificação de Componentes 
void DFS(int, vector<Lista>&);
int componentesConexos(vector<Lista>&);

//Liberar Memoria das Matrizes
void liberarMemoria(int**,int);

int main(int argc, char *argv[]){
    
	duration<double> time_span2;

	//arquivos de entrada dos grafos
    FILE *grafo1,*grafo2;
	
	//Dados do grafos I e II
    Dados dados1, dados2;
	
	//Matrizes de Adjacencias
	int **matrizAdj1, **matrizAdj2;
	
	//lendo arquivos dos grafos
	if ((grafo1 = fopen(argv[1],"r")) == NULL) {
	   cout << "Erro na abertura do arquivo\n"; return 0;
	}
	if ((grafo2 = fopen(argv[2],"r")) == NULL) {
	   cout << "Erro na abertura do arquivo\n"; return 0;
	}
    
	//iniciando contagem do tempo
	high_resolution_clock::time_point t1 = high_resolution_clock::now();
	
    // Grafo I	
	carregaTamanho(&grafo1, &dados1);
    vector<Lista> listaAdj1(dados1.m);
    carregaArquivo(&grafo1, listaAdj1);
	
	cout<<"Grafo I: "<<endl;
    cout<<"Quantidade de vertices:"<<dados1.m-1<<" Arestas:"<<dados1.n-1<<endl;

    contarGraus(listaAdj1);
	
	sort(listaAdj1.begin(),listaAdj1.end(),compararPorGraus);
	
	//Preenchendo Matriz de adjacencia 
	matrizAdj1 = gerarMatrizAdj(dados1.m-1,listaAdj1);

	// GRAFO II
	
	carregaTamanho(&grafo2, &dados2);
    vector<Lista> listaAdj2(dados2.m);
    carregaArquivo(&grafo2, listaAdj2);
	
	cout<<"Grafo II: "<<endl;
    cout<<"Quantidade de vertices:"<<dados2.m-1<<" Arestas:"<<dados2.n-1<<endl;
    
	//realizando a contagem dos graus por vertices do grafo 2
    contarGraus(listaAdj2);
	
	//ordenando a lista do grafo para comparacao do numero de grau por vertices
	sort(listaAdj2.begin(),listaAdj2.end(),compararPorGraus);
	
	//Preenchendo Matriz de adjacencia 
	matrizAdj2 = gerarMatrizAdj(dados2.m-1,listaAdj2);
	
	//verificando 4 propriedades
	
	if(verificarPropriedades(listaAdj1,listaAdj2,&dados1,&dados2)==1)
		cout<<"Os grafos sao isomorfos"<<endl;
	else
		cout<<"Os grafos nao sao isomorfos"<<endl;
	
	/*cout<<endl;
	for(int i=0;i<listaAdj1.size();i++){
		cout<<"Vertice: "<<listaAdj1[i].v <<"Posicao: "<<i<<endl;
		cout<<"Grau: "<<listaAdj1[i].grau <<endl;
	////	cout<<"Lista de adjacencias: ";
		cout<<"Qtd de vertices adjacentes: "<<listaAdj1[i].adj.size()<<endl;
		cout<<"Visitado: "<<listaAdj1[i].visitado<<endl;
	}
	*/
	
	high_resolution_clock::time_point t2 = high_resolution_clock::now();
    duration<double> time_span = duration_cast<duration<double> >(t2 - t1);
	
	cout<<"Tempo total: "<<time_span.count()<<endl;
	
	//liberar memoria
	liberarMemoria(matrizAdj1,dados1.m);
	liberarMemoria(matrizAdj2,dados2.m);
	
    return 0;
}
/*Arquivo.cpp*/

int carregaArquivo(FILE **entrada, vector<Lista> &listaAdj){
	
	int v1 = 0, v2 = 0;
	char e, enter;
	
	while(fscanf(*entrada, "%c %d %d %[\n]", &e,  &v1, &v2, &enter) != EOF){
		listaAdj[v1].v = v1;
		listaAdj[v1].adj.push_back(v2);
		listaAdj[v2].v = v2;
		listaAdj[v2].adj.push_back(v1);
	}
	listaAdj[0].adj.clear();
	fclose(*entrada);
}

int carregaTamanho(FILE **entrada, Dados* dados){
	char edge[8];
	fscanf(*entrada, "%[A-Z a-z] %d %d", edge, &dados->m, &dados->n);
	dados-> m++;
	dados-> n++;
}

/*lista.cpp*/
void contarGraus(vector<Lista>& listaAdj){
    for (int i=0;i<listaAdj.size();i++){	
        listaAdj[i].grau = listaAdj[i].adj.size();
    }

}
int** gerarMatrizAdj(int tamanho,vector<Lista>& listaAdj){
	 
	int aux = 1;
	int lin = tamanho + 1;
	int col = tamanho + 1;
	
	//aloca espaco para ponteiros para inteiros (representam as linhas)
	 int **matriz = (int**)malloc(lin * sizeof(int*));

    for(int i = 0; i < col; i++)
	  //aloca espaco para 5 ponteiros para inteiros (representam as colunas)
	  matriz[i] = (int*) malloc(col * sizeof(int));
	
	//inicializando a matriz com zeros
	for(int i=0; i < lin; i++){ 
       for(int j=0; j < col; j++){   
           matriz[i][j] = 0;
	   }
	}
	ofstream escreve;
    escreve.open("matriz.txt");
	
	while(tamanho!=aux){
		
		//Percorrendo todos os vertices que são adjacentes a v e verificando se ja foram visitados
	    for(int i=1;i<listaAdj[aux].adj.size();i++){ 
	         
		    int pos = listaAdj[aux].adj[i];
		    matriz[listaAdj[aux].v][pos] = 1 ;
			//escreve<<"Vertice: "<<listaAdj[aux].v<<" Posicao: "<<pos<<endl;
	    } 
		
		aux++;
	
	}
	
	for(int i=0; i < lin; i++){
		
		for(int j=0; j < col; j++){
			escreve<<matriz[i][j];
	    }
		escreve<<endl;
	}
       
	escreve.close();
	
	return matriz;
	
}

int verificarPropriedades(vector<Lista>& listaAdj1,vector<Lista>& listaAdj2,Dados* dados1,Dados* dados2){
	
	
	int status = 1; 
	
	//mesmo numero de vertices
	if(dados1->m-1 != dados2->m-1) 
		status = 0;
	//mesmo numero de arestas
	if(dados1->n-1 != dados2->n-1)
		status = 0;
	
	//numero de graus por vertices
	if(status==1){
	   
		for(int i=0; i< listaAdj1.size();i++){
		
			if(listaAdj1[i].grau != listaAdj2[i].grau){
                status = 0;
                break;				
			}
		}
	}
    //numero de componentes
	if(componentesConexos(listaAdj1)!=componentesConexos(listaAdj2))
		status = 0;
	
	return status;
}

int componentesConexos(vector<Lista>& listaAdj){
	
	int aux = 0;
	// Percorrendo todos os vertices presentes na lista
    for (int i=0; i<listaAdj.size(); i++){
		
		int aux = 0;
		// Se o vertice na posicao i ainda nao foi visitado, visitamos o vertice e tbm seu adjcentes 
		// ate que nao haja mais vertices adjacentes que ainda nao foram visitados
		// assim e somado um componente conexo 
		
        if (listaAdj[i].visitado == false){
           
            DFS(listaAdj[i].v, listaAdj);
 
           // cout << "\n";
			aux++;
        }
		
    }
	
	return aux;
}

void DFS(int v, vector<Lista>& listaAdj){
	
    //marcando vertice na posicao v como visitado
    listaAdj[v].visitado = true;
    //cout << v << " ";
    
	//Percorrendo todos os vertices que são adjacentes a v e verificando se ja foram visitados
	for(int i=0;i<listaAdj[v].adj.size();i++){ 
	
		int pos = listaAdj[v].adj[i];
		if(listaAdj[pos].visitado == false)
			
		    DFS(pos,listaAdj);
	}

}
void liberarMemoria(int **matriz, int tamanho){
  
  //libera memoria para a matriz
  for (int i = 0; i < tamanho; i++)
    //libera memoria para cada um dos n ponteiros que representam as linhas
    free(matriz[i]);
	
  //libera memoria da variavel que representa a matriz inteira (ponteiro para os 10 ponteiros)
  free(matriz);
}
bool compararPorGraus(const Lista &l1, const Lista &l2){
    return l1.grau < l2.grau;
}
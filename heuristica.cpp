// Autor: Erick Silva
// Carnet: 11-10969

#include <cstdio>
#include <iostream>
#include <utility>
#include <vector>
#include <queue>
#include <stack>
#include <string>
#include <string.h>
#include <fstream>
#include <algorithm>

#define mp make_pair
#define ff first
#define ss second

using namespace std;

typedef pair<int,int> pii;


// Usamos variables globales para el grafo por comodidad y rapidez.
vector< vector< int > > grafo;
int beneficio[105][105];
int costo[105][105];
int beneficioOriginal[105][105];

// Para el dijkstra
int padre[105];

// Declaramos las funciones.
vector<int> construirSolucion(int, vector<vector<int> >&);
void dfsUnirComponente(vector<int> &);
void dfsCompConexasR(int, int, vector<int>&, vector<vector<int> >&);
vector < vector <int> > obtenerComponentesConexasR(int);
pair< vector<int>, int > dijkstra(int,int,bool);
void mejorarSolucion(vector<int> &, int );


int main(int argc, char* argv[]){
	int nroVertices;
	int nroLados;
	int nroLadosR;

	string nombre_instancia;
	ifstream input;
	ofstream output;

	if (argc > 2){
		cout << "El programa espera el path al archivo de la instancia" << endl;
		exit(0);
	}
	if (argc == 1){
		cout << "Coloque el nombre o path a la instancia" << endl;
		cin >> nombre_instancia;
	}
	if (argc == 2){
		nombre_instancia = string(argv[1]);
	}

	input.open(nombre_instancia);

	// Lectura:

	// Variable auxiliar para deshacernos de parte del input innecesaria.
	string aux;
	
	// Reading the input, we use aux to ignore certain string tokens.
	// number of vertices : {int}
	input >> aux >> aux >> aux >> aux >> nroVertices;
	// number of required edges {int}
	input >> aux >> aux >> aux >> aux >> nroLadosR;

	// inicializamos los arreglos y el grafo.
	grafo.resize(nroVertices+1);
	memset(beneficio,-1,sizeof(beneficio));
	memset(costo,-1,sizeof(costo));

	int v0,v1,c,b;
	for (int i = 0; i < nroLadosR; i++){
		input >> v0 >> v1 >> c >> b;
		grafo[v0].push_back(v1);
		grafo[v1].push_back(v0);
		beneficioOriginal[v0][v1] = b;
		beneficioOriginal[v1][v0] = b;
		beneficio[v0][v1] = b;
		costo[v0][v1] = c;
		beneficio[v1][v0] = b;
		costo[v1][v0] = c;
	}

	// number of non required edges  {int} 
	input >> aux >> aux >> aux >> aux >> aux >> nroLados;
	
	for (int i = 0; i < nroLados; i++){
		input >> v0 >> v1 >> c >> b;
		grafo[v0].push_back(v1);
		grafo[v1].push_back(v0);
		beneficioOriginal[v0][v1] = b;
		beneficioOriginal[v1][v0] = b;	
		beneficio[v0][v1] = b;
		costo[v0][v1] = c;
		beneficio[v1][v0] = b;
		costo[v1][v0] = c;
	}

	// Cerramos el archivo de input.
	input.close();

	// Fin Lectura.

	nroLados = nroLados + nroLadosR;


	// Populamos el vector de componentes Conexas (que usan solo lados R)

	vector < vector <int> > compConexas = obtenerComponentesConexasR(nroVertices);

	// compConexas tiene ahora todos las componentes conexas

	// Con esto sacamos una solucion principal, no sera buena pero con un algoritmo
	// greedy solo podemos desear por una aproximacion rapida.
	vector<int> solucion = construirSolucion(nroVertices,compConexas);

	int valorSolucion = 0;


	for (unsigned int i = 0 ; i < solucion.size() - 1;i++){
		valorSolucion += beneficioOriginal[solucion[i]][solucion[i+1]];
		valorSolucion -= costo[solucion[i]][solucion[i+1]];
		beneficioOriginal[solucion[i]][solucion[i+1]] = 0;
		beneficioOriginal[solucion[i+1]][solucion[i]] = 0;
	}

	//mejorarSolucion(solucion,valorSolucion);
	// Puesto que mejorar la solucion depende de eliminar lados repetidos y
	// el problema principal del algoritmo es que repetimos lados que no deberiamos
	// este no mejora mucho la solucion.


	// Si la solucion greedy no lo logra. Vamos a 0.
	if (valorSolucion < 0){
		valorSolucion = 0;
		solucion.resize(1);
		solucion[0] = 1;
	}

	output.open(nombre_instancia + "_salida.txt");
	output << valorSolucion << endl;
	output << solucion[0];
	for (unsigned int i = 1 ; i < solucion.size(); i++){
		output << " " << solucion[i];
	}
	output <<endl;


	output.close();
}

vector<int> construirSolucion(int nroVertices, vector < vector <int> > &compConexas){
	vector<int> solucion;
	vector<int> componenteLista;
	int ultimo;
	int mejorGanancia = -100, componente = 0;
	bool cambio;

	componenteLista.resize(compConexas.size());
	componenteLista.assign(compConexas.size(),0);
	componenteLista[1] = 1;
	componenteLista[0] = 1;

	solucion.push_back(1);

	// Agregamos de manera 'aleatoria' algunos de los lados de la componente
	// conexa R. Esto puede ayudar o no, pero no hay forma de saber definitiva
	// sin busqueda exhaustiva.
	dfsUnirComponente(solucion);


	pair< vector<int>, int > respuestaDijkstra;


	// Creamos un vector de visitados para saber que falta.

	vector<int> visitado;
	visitado.resize(nroVertices+1);
	visitado.assign(nroVertices+1,0);


	vector<int> nuevoCamino;
	cambio = true;
	while (cambio){
		cambio = false;
		ultimo = solucion[solucion.size()-1];
		for (unsigned int i = 1; i < compConexas.size(); i++){
			if (compConexas[i].size() <= 1) continue;
			if (componenteLista[i] == 1) continue;
			// Se intenta el menor camino hacia todas las otras
			for (unsigned int j = 0; j < compConexas[i].size(); j++){
				if (visitado[compConexas[i][j]]) continue;
				respuestaDijkstra = dijkstra(ultimo,compConexas[i][j],true);
				if (respuestaDijkstra.ff.size() == 0) continue;
				else if (respuestaDijkstra.ss > mejorGanancia){
					mejorGanancia = respuestaDijkstra.ss;
					nuevoCamino = respuestaDijkstra.ff;
					cambio = true;
					componente = i;
				}
			}
		}
		// El mejor camino se une a la solucion
		if (nuevoCamino.size() != 0 and cambio){
			// Se cambian los beneficios de los lados usados
			for (unsigned int i = 0; i < nuevoCamino.size() - 1; i++){
				beneficio[nuevoCamino[i]][nuevoCamino[i+1]] = 0;
				beneficio[nuevoCamino[i+1]][nuevoCamino[i]] = 0;
			}
			for (int nodo: nuevoCamino){
				visitado[nodo] = 1;
			}
			componenteLista[componente] = 1;
			solucion.insert(solucion.end(),nuevoCamino.begin()+1,nuevoCamino.end());
			// Se unen lados de la componente R conexa.
			dfsUnirComponente(solucion);
		}
	}



	int cAdy,bAdy,nodoAux;

	// Completamos la solucion
	respuestaDijkstra = dijkstra(solucion[solucion.size()-1],1,true);
	nuevoCamino = respuestaDijkstra.ff;

	solucion.insert(solucion.end(),nuevoCamino.begin()+1,nuevoCamino.end());

	for (int nodo: solucion){
		visitado[nodo] = 1;
	}

	// Se agregan todos los lados de componentes R que falten.
	bool avance = true;
	while(avance){
		avance = false;
		for (unsigned int i = 1; i < compConexas.size(); i++){
			if (compConexas[i].size() <= 1) continue;
			for (int nodo: compConexas[i]){
				if (visitado[nodo] != 0) continue;
				for (int j = solucion.size() - 1; j >= 0; j--){
					nodoAux = solucion[j];

					bAdy = beneficio[nodoAux][nodo];
					cAdy = costo[nodoAux][nodo];
					if (cAdy == -1) continue;
					if (bAdy >= 2*cAdy){
						avance = true;
						beneficio[nodoAux][nodo] = 0;
						beneficio[nodo][nodoAux] = 0;
						visitado[nodo] = 1;
						solucion.insert(solucion.begin()+j+1,nodo);
						solucion.insert(solucion.begin()+j+2,nodoAux);
						break;
					}
				}
			}
		}
	}
	



	return solucion;

}

// Usa un dfs modificado para unir a toda la componente actual.
void dfsUnirComponente(vector<int> &v){
	int vis[105];
	memset(vis,-1,sizeof(vis));

	for (unsigned int i = 0; i < v.size(); i++){
		vis[v[i]] = 1;
	}
	int nodoFinal = v[v.size()-1];
	int nodoAdy, bAdy, cAdy;
	bool avance = true;
	while(avance){
		avance = false;
		for (unsigned int i = 0; i < grafo[nodoFinal].size();i++){
			nodoAdy = grafo[nodoFinal][i];
			bAdy = beneficio[nodoFinal][nodoAdy];
			cAdy = costo[nodoFinal][nodoAdy];
			if (bAdy >= cAdy and vis[nodoAdy] == -1){
				vis[nodoAdy] = 1;
				beneficio[nodoFinal][nodoAdy] = 0;
				beneficio[nodoAdy][nodoFinal] = 0;
				v.push_back(nodoAdy);
				nodoFinal = nodoAdy;
				avance = true;
				break;
			}
		}
	}

}

// Dfs que agrega las componentes conexas a su lugar.
void dfsCompConexasR(int vertice, int componente, vector<int> &visitado,
	                  	vector < vector <int> > &compConexas ){
	compConexas[componente].push_back(vertice);
	visitado[vertice] = 1;
	int nodo;
	for (unsigned int i = 0; i < grafo[vertice].size(); i++){
		nodo = grafo[vertice][i];
		if (beneficio[vertice][nodo] >= costo[vertice][nodo] and visitado[nodo] == -1){
			dfsCompConexasR(nodo,componente,visitado,compConexas);
		}
	}
}

// Funcion para obtener los componentes conexas de lados R.
vector< vector<int> > obtenerComponentesConexasR(int nroVertices){
	vector<int> visitado;
	vector < vector <int> > compConexas;

	visitado.resize(nroVertices+1);
	visitado.assign(nroVertices+1,-1);
	compConexas.resize(nroVertices+1);
	int componente = 0;
	for (int i = 1; i < nroVertices; i++){
		if (visitado[i] == -1){
			componente++;
			dfsCompConexasR(i,componente,visitado,compConexas);
		}
	}
	compConexas.resize(componente+1);

	return compConexas;
}

// Retorna el menor camino basado unicamente en costo, solo
// recorre una vez cada nodo para evitar ciclos negativos.
pair< vector<int>, int > dijkstra(int start, int end, bool useNegative){
	priority_queue< pii, vector<pii>, greater<pii> > cola;

	cola.push(mp(0,start));
	pii actual;
	int nodoActual;
	int costoActual;
	// Se resettea el arreglo de padres
	memset(padre,-1,sizeof(padre));
	// variables auxiliares para costo y beneficio.
	int c,b; 

	padre[start] = start;

	// Vector donde guardaremos el camino
	vector<int> camino;
	// nodo auxiliar
	int nodo,ady,ganancia;


	while (!cola.empty()){
		actual = cola.top();
		cola.pop();
		nodoActual = actual.ss;
		costoActual = actual.ff;

		// Si llegamos al final, reconstruimos el camino
		if (nodoActual == end){
			// usamos el nodo para iterar hasta que encontremos el final.
			ganancia = 0;
			nodo = nodoActual;
			while (padre[nodo] != nodo){
				camino.push_back(nodo);
				ganancia += beneficio[padre[nodo]][nodo] - costo[padre[nodo]][nodo];
				nodo = padre[nodo];
			}
			camino.push_back(nodo);
			reverse(camino.begin(), camino.end());
			return mp(camino,ganancia);
		}

		// Si no, seguimos iterando.
		for (unsigned int i = 0; i < grafo[nodoActual].size();i++){
			ady = grafo[nodoActual][i];
			c = costo[nodoActual][ady];
			b = beneficio[nodoActual][ady];
			if ((padre[ady] == -1) and (useNegative or c <= b)){
				padre[ady] = nodoActual;
				cola.push(mp(costoActual+c - b,ady));
			}
		}
	}

	return mp(camino,-1000000);
}
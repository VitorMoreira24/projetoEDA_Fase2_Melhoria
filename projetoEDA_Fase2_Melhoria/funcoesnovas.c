#define _CRT_SECURE_NO_WARNINGS
#include "antenas.h"

Antena* procurarAntena(Grafo* cidade, int coluna, int linha){
	
	if (cidade == NULL) {
		return -1;
	}
	else {
		for (Antena* antenaAtual = cidade->antenas; antenaAtual != NULL; antenaAtual->proxima) {
			if (antenaAtual->coluna == coluna && antenaAtual->linha) {
				return antenaAtual; // Retorna a antena se encontrar a posição exata
			}
		}
		return NULL; 
	}
}

Antena* popularAntena(char freq, int coluna, int linha) {
	Antena* novaAntena = malloc(sizeof(Antena));
	if (novaAntena == NULL) {
		return NULL; // Retorna NULL se não conseguir alocar memória
	}
	novaAntena->frequencia = freq;
	novaAntena->coluna = coluna;
	novaAntena->linha = linha;
	novaAntena->ligacoes = NULL; // Inicializa sem ligações
	novaAntena->visitada = false; // Inicializa como não visitada
	novaAntena->proxima = NULL; // Inicializa como o último elemento da lista
	return novaAntena;
}

int adicionarAntenaOrdenado(Grafo* cidade, char freq, int coluna, int linha) {

	if (cidade == NULL) {
		return -1;
	}

	//Inserir no INICIO SEM ANTENAS
	if (cidade->antenas == NULL) {
		cidade->antenas = popularAntena(freq, coluna, linha);
	}
	else {
		Antena* antenaAnterior = NULL;
		Antena* antenaPosterior = cidade->antenas;
		
		//Inserir no INICIO com ANTENAS
		if(antenaPosterior->linha > linha || (antenaPosterior->coluna >= coluna && antenaPosterior->linha == linha)){
			if (antenaPosterior->linha == linha && antenaPosterior->coluna == coluna) {
				return -2; // Antena já existe na posição
			}
			Antena* novaAntena = popularAntena(freq, coluna, linha);
			if (novaAntena == NULL) {
				return  -3; // Falha ao alocar memória
			}
			novaAntena->proxima = cidade->antenas; 
			cidade->antenas = novaAntena; 
			
		}
		else {
			while (antenaPosterior->linha < linha || (antenaPosterior->coluna <= coluna && antenaPosterior->linha == linha)) {
				if (antenaPosterior->proxima != NULL) {
					antenaAnterior = antenaPosterior;
					antenaPosterior = antenaPosterior->proxima;
				}
				else {
					break; // Chegou ao final da lista
				}
			}
			if (antenaAnterior->linha == linha && antenaAnterior->coluna == coluna) {
				return -2; // Antena já existe na posição
			}
			Antena* novaAntena = popularAntena(freq, coluna, linha);
			if (novaAntena == NULL) {
				return  -3; // Falha ao alocar memória
			}
			antenaAnterior->proxima = novaAntena;
			novaAntena->proxima = antenaPosterior;
		}
	}	
	cidade->totalAntenas++;
}

int adicionarAntenaFim(Grafo* cidade, char freq, int coluna, int linha) {

	if (cidade == NULL) {
		return -1;
	}
	if (cidade->antenas == NULL) {
		cidade->antenas = popularAntena(freq, coluna, linha);
		cidade->totalAntenas++;
	}
	else {
		for (Antena* antenaAtual = cidade->antenas; antenaAtual != NULL; antenaAtual = antenaAtual->proxima) {
			if (antenaAtual->proxima == NULL) {
				antenaAtual->proxima = popularAntena(freq, coluna, linha);
				if (antenaAtual->proxima == NULL) {
					return -1; // Falha ao alocar memória
				}
				cidade->totalAntenas++;
				return 0;
			}
		}
	}
}
Grafo* adicionarAntenaFimOtimizado(Grafo* cidade, Antena* ultimaAntena, char freq, int coluna, int linha) {
	Antena* novaAntena = popularAntena(freq, coluna, linha);
	if (novaAntena == NULL) {
		return NULL;
	}

	if (ultimaAntena == NULL) {
		cidade->antenas = novaAntena; 
	}

	ultimaAntena->proxima = novaAntena;
	cidade->totalAntenas++;

	return novaAntena;
}

int removerAntena(Grafo* cidade, int coluna, int linha) {
	if (cidade == NULL) {
		return -1;
	}
	Antena* antenaAnterior = NULL;
	Antena* antenaPosterior = cidade->antenas;
		
	//remove se for a primeira caso contrario percorre as que for necessario
	if (antenaPosterior != NULL && antenaPosterior->linha == linha && antenaPosterior->coluna == coluna) {
		cidade->antenas = antenaPosterior->proxima;
		free(antenaPosterior);
		
		return 0; // Antena removida com sucesso
	}
	else {
		while (antenaPosterior != NULL && antenaPosterior->linha <= linha) {

			if (antenaPosterior->linha == linha && antenaPosterior->coluna == coluna) {
				antenaAnterior->proxima = antenaPosterior->proxima; // Remove a antena encontrada
				free(antenaPosterior); 

				return 0; // Antena removida com sucesso
			}
			antenaAnterior = antenaPosterior;
			antenaPosterior = antenaPosterior->proxima;
		}
	}
	return -2; // Antena não encontrada
}

//ARESTAS

Adjacencia* popularAdjacencia(Antena* destino) {
	Adjacencia* novaAdjacencia = malloc(sizeof(Adjacencia));
	if(novaAdjacencia == NULL){
		return NULL; // Retorna NULL se não conseguir alocar memória
	}
	novaAdjacencia->destino = destino;
	novaAdjacencia->proxima = NULL;

	return novaAdjacencia;
}

Adjacencia* procurarAdjacencia(Antena* antenaAtual, Antena* destino) {
	if (antenaAtual == NULL || destino == NULL) {
		return NULL; 
	}

	for (Adjacencia* primeiraAdj = antenaAtual->ligacoes; primeiraAdj != NULL; primeiraAdj = primeiraAdj->proxima) {
		if (primeiraAdj->destino == destino) {
			return primeiraAdj; // Retorna a adjacência se encontrar o destino
		}
	}
	return NULL; // Retorna NULL se não encontrar a adjacência
}

int adicionarAresta(Antena* antenaAtual, Antena* destino, bool verificarRepetidos){
	if (antenaAtual == NULL) {
		if (destino == NULL){
			return -3;
		}
		return -1;
	}
	if (destino == NULL) {
		return -2; 
	}
	
	if (verificarRepetidos) {
		if (procurarAdjacencia(antenaAtual, destino) == NULL) {
			return -5; // Aresta já existe
		}
	}

	Adjacencia* novaAdjacencia = popularAdjacencia(destino);
	if (novaAdjacencia == NULL) {
		return -4; // Falha ao alocar memória
	}
	novaAdjacencia->proxima = antenaAtual->ligacoes; 
	antenaAtual->ligacoes = novaAdjacencia;
	 
	return 0; 
}

int removerAresta(Antena* antenaAtual, Antena* destino) {
	if (antenaAtual == NULL) {
		if (destino == NULL) {
			return -3;
		}
		return -1;
	}
	if (destino == NULL) {
		return -2;
	}

	Adjacencia* adjAnterior = NULL;
	Adjacencia* adjPosterior = antenaAtual->ligacoes;


	if (adjPosterior->destino == destino) {
		antenaAtual->ligacoes = adjPosterior->proxima;
		free(adjPosterior);
		return 0;
	}
	else { 
		adjAnterior = adjPosterior;
		adjPosterior = adjPosterior->proxima;

		while (adjPosterior != NULL) {
			if (adjPosterior->destino == destino) {
				adjAnterior->proxima = adjPosterior->proxima; // Remove a adjacência encontrada
				free(adjPosterior);
				return 0; // Aresta removida com sucesso
			}

			adjAnterior = adjPosterior;
			adjPosterior = adjPosterior->proxima;
		}
	}
	return -6;
}

int carregarAntenasDoFicheiro(Grafo* cidade, const char* nomeFicheiro) {
	FILE* ficheiro = fopen(nomeFicheiro, "r");

	if (nomeFicheiro == NULL) {
		return -1; // Se não conseguir abrir o ficheiro, retorna o grafo vazio
	}

	Antena* ultimaAntena = NULL;

	char bufferLinha[MAX_X];  // Buffer para ler cada linha do ficheiro
	for (int y = 0; fgets(bufferLinha, sizeof(bufferLinha), ficheiro); y++) {
		for (int x = 0; bufferLinha[x] != '\n' && bufferLinha[x] != '\0'; x++) {
			if (bufferLinha[x] != ".") {
				//adicionarAntenaOrdenado(cidade, bufferLinha[x], x, y); INSERIR NORMALMENTE

				//INSERIR OTIMIZADO
				ultimaAntena = adicionarAntenaFimOtimizado(cidade, ultimaAntena, bufferLinha[x], x, y);
			}
		}
	}
	return 0;
}

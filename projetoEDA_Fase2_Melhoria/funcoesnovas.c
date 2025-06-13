/*****************************************************************//**
 * \file   funcoesnovas.c
 * \brief  funcoes de Melhoria para manipula��o de antenas em um grafo.
 * 
 * \author vitor
 * \date   June 2025
 *********************************************************************/
#define _CRT_SECURE_NO_WARNINGS
#include "antenas.h"

#pragma region ProcurarAntena
 /**
  * Fun��o para procurar uma antena espec�fica no grafo.
  *
  * @param grafo --> Apontador para o grafo onde as antenas est�o localizadas.
  * @param coluna --> Posi��o horizontal da antena (coordenada X).
  * @param linha --> Posi��o vertical da antena (coordenada Y).
  * @param antenaAtual --> Apontador para a antena atual na lista do grafo.
  * @param proxima --> Apontador para a pr�xima antena na lista do grafo.
  */
Antena* procurarAntena(Grafo* cidade, int coluna, int linha){
	
	if (cidade == NULL) {
		return -1;
	}
	else {
		for (Antena* antenaAtual = cidade->antenas; antenaAtual != NULL; antenaAtual->proxima) {
			if (antenaAtual->coluna == coluna && antenaAtual->linha) {
				return antenaAtual; // Retorna a antena se encontrar a posi��o exata
			}
		}
		return NULL; 
	}
}
#pragma endregion

#pragma region PopularAntena
/**
 * Fun��o para popular uma nova antena com os dados fornecidos.
 *
 * @param freq --> Frequ�ncia da antena (car�cter �nico).
 * @param coluna --> Posi��o horizontal da antena (coordenada X).
 * @param linha --> Posi��o vertical da antena (coordenada Y).
 * @param novaAntena --> Apontador para a nova antena a ser criada.
 * @return Apontador para a nova antena ou NULL se n�o conseguir alocar mem�ria.
 */
Antena* popularAntena(char freq, int coluna, int linha) {
	Antena* novaAntena = malloc(sizeof(Antena));
	if (novaAntena == NULL) {
		return NULL; // Retorna NULL se n�o conseguir alocar mem�ria
	}
	novaAntena->frequencia = freq;
	novaAntena->coluna = coluna;
	novaAntena->linha = linha;
	novaAntena->ligacoes = NULL; // Inicializa sem liga��es
	novaAntena->visitada = false; // Inicializa como n�o visitada
	novaAntena->proxima = NULL; // Inicializa como o �ltimo elemento da lista
	return novaAntena;
}
#pragma endregion

#pragma region AdicionarAntenaOrdenado
/**
 * Fun��o para adicionar uma antena ao grafo de forma ordenada.
 *
 * @param cidade --> Apontador para o grafo onde a antena ser� adicionada.
 * @param freq --> Frequ�ncia da antena (car�cter �nico).
 * @param coluna --> Posi��o horizontal da antena (coordenada X).
 * @param linha --> Posi��o vertical da antena (coordenada Y).
 * @param antenaAnterior --> Apontador para a antena anterior na lista do grafo.
 * @param antenaPosterior --> Apontador para a antena posterior na lista do grafo.
 * @param novaAntena --> Apontador para a nova antena a ser criada.
 * @return 0 se a antena foi adicionada com sucesso, -1 se o grafo for nulo,
 *         -2 se a antena j� existir na posi��o, -3 se falhar ao alocar mem�ria.
 */
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
				return -2; // Antena j� existe na posi��o
			}
			Antena* novaAntena = popularAntena(freq, coluna, linha);
			if (novaAntena == NULL) {
				return  -3; // Falha ao alocar mem�ria
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
				return -2; // Antena j� existe na posi��o
			}
			Antena* novaAntena = popularAntena(freq, coluna, linha);
			if (novaAntena == NULL) {
				return  -3; // Falha ao alocar mem�ria
			}
			antenaAnterior->proxima = novaAntena;
			novaAntena->proxima = antenaPosterior;
		}
	}	
	cidade->totalAntenas++;
}
#pragma endregion

#pragma region AdicionarAntenaFim
/**
 * Fun��o para adicionar uma antena ao final da lista de antenas do grafo.
 *
 * @param cidade --> Apontador para o grafo onde a antena ser� adicionada.
 * @param freq --> Frequ�ncia da antena (car�cter �nico).
 * @param coluna --> Posi��o horizontal da antena (coordenada X).
 * @param linha --> Posi��o vertical da antena (coordenada Y).
 * @param antenaAtual --> Apontador para a antena atual na lista do grafo.
 * @return 0 se a antena foi adicionada com sucesso, -1 se o grafo for nulo,
 *         -2 se falhar ao alocar mem�ria.
 */
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
					return -1; // Falha ao alocar mem�ria
				}
				cidade->totalAntenas++;
				return 0;
			}
		}
	}
}
#pragma endregion

#pragma region AdicionarAntenaFimOtimizado
/**
 * Fun��o otimizada para adicionar uma antena ao final da lista de antenas do grafo.
 *
 * @param cidade --> Apontador para o grafo onde a antena ser� adicionada.
 * @param ultimaAntena --> Apontador para a �ltima antena na lista do grafo.
 * @param freq --> Frequ�ncia da antena (car�cter �nico).
 * @param coluna --> Posi��o horizontal da antena (coordenada X).
 * @param linha --> Posi��o vertical da antena (coordenada Y).
 * @param novaAntena --> Apontador para a nova antena a ser criada.
 * @return Apontador para a nova antena ou NULL se n�o conseguir alocar mem�ria.
 */
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
#pragma endregion

#pragma region removerAntena
/**
 * Fun��o para remover uma antena espec�fica do grafo.
 *
 * @param cidade --> Apontador para o grafo onde a antena ser� removida.
 * @param coluna --> Posi��o horizontal da antena (coordenada X).
 * @param linha --> Posi��o vertical da antena (coordenada Y).
 * @param antenaAnterior --> Apontador para a antena anterior na lista do grafo.
 * @param antenaPosterior --> Apontador para a antena posterior na lista do grafo.
 * @return 0 se a antena foi removida com sucesso, -1 se o grafo for nulo,
 *         -2 se a antena n�o for encontrada.
 */
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
	return -2; // Antena n�o encontrada
}
#pragma endregion

//ARESTAS

#pragma region PopularAdjacencia
/**
 * Fun��o para popular uma nova adjac�ncia entre antenas.
 *
 * @param destino --> Apontador para a antena de destino da adjac�ncia.
 * @return Apontador para a nova adjac�ncia ou NULL se n�o conseguir alocar mem�ria.
 */
Adjacencia* popularAdjacencia(Antena* destino) {
	Adjacencia* novaAdjacencia = malloc(sizeof(Adjacencia));
	if(novaAdjacencia == NULL){
		return NULL; // Retorna NULL se n�o conseguir alocar mem�ria
	}
	novaAdjacencia->destino = destino;
	novaAdjacencia->proxima = NULL;

	return novaAdjacencia;
}
#pragma endregion

#pragma region ProcurarAdjacencia
/**
 * Fun��o para procurar uma adjac�ncia entre antenas.
 *
 * @param antenaAtual --> Apontador para a antena atual na lista de liga��es.
 * @param destino --> Apontador para a antena de destino da adjac�ncia.
 * @param primeiraAdj --> Apontador para a primeira adjac�ncia na lista de liga��es.
 * @return Apontador para a adjac�ncia encontrada ou NULL se n�o encontrar.
 */
Adjacencia* procurarAdjacencia(Antena* antenaAtual, Antena* destino) {
	if (antenaAtual == NULL || destino == NULL) {
		return NULL; 
	}

	for (Adjacencia* primeiraAdj = antenaAtual->ligacoes; primeiraAdj != NULL; primeiraAdj = primeiraAdj->proxima) {
		if (primeiraAdj->destino == destino) {
			return primeiraAdj; // Retorna a adjac�ncia se encontrar o destino
		}
	}
	return NULL; // Retorna NULL se n�o encontrar a adjac�ncia
}
#pragma endregion

#pragma region AdicionarAresta
/**
 * Fun��o para adicionar uma aresta
 *
 * @param antenaAtual --> Apontador para a antena atual onde a aresta ser� adicionada.
 * @param destino --> Apontador para a antena de destino da aresta.
 * @param verificarRepetidos --> Indicador se deve verificar se a aresta j� existe.
 * @param novaAdjacencia --> Apontador para a nova adjac�ncia a ser criada.
 * @return 0 se a aresta foi adicionada com sucesso, -1 se a antena atual for nula,
 *         -2 se o destino for nulo, -3 se ambos forem nulos, -4 se falhar ao alocar mem�ria,
 *         -5 se a aresta j� existir, -6 se n�o encontrar a aresta para remover.
 */
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
			return -5; // Aresta j� existe
		}
	}

	Adjacencia* novaAdjacencia = popularAdjacencia(destino);
	if (novaAdjacencia == NULL) {
		return -4; // Falha ao alocar mem�ria
	}
	novaAdjacencia->proxima = antenaAtual->ligacoes; 
	antenaAtual->ligacoes = novaAdjacencia;
	 
	return 0; 
}
#pragma endregion

#pragma region RemoverAresta
/**
 * Fun��o para remover uma aresta entre antenas.
 *
 * @param antenaAtual --> Apontador para a antena atual onde a aresta ser� removida.
 * @param destino --> Apontador para a antena de destino da aresta.
 * @param adjAnterior --> Apontador para a adjac�ncia anterior na lista de liga��es.
 * @param adjPosterior --> Apontador para a adjac�ncia posterior na lista de liga��es.
 * @return 0 se a aresta foi removida com sucesso, -1 se a antena atual for nula,
 *         -2 se o destino for nulo, -3 se ambos forem nulos, -6 se n�o encontrar a aresta para remover.
 */
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
				adjAnterior->proxima = adjPosterior->proxima; // Remove a adjac�ncia encontrada
				free(adjPosterior);
				return 0; // Aresta removida com sucesso
			}

			adjAnterior = adjPosterior;
			adjPosterior = adjPosterior->proxima;
		}
	}
	return -6;
}
#pragma endregion

#pragma region CarregarAntenasDoFicheiro
/**
 * Fun��o para carregar antenas de um ficheiro e adicion�-las ao grafo.
 *
 * @param cidade --> Apontador para o grafo onde as antenas ser�o carregadas.
 * @param nomeFicheiro --> Nome do ficheiro de onde as antenas ser�o lidas.
 * @param ultimaAntena --> Apontador para a �ltima antena adicionada, usado para otimiza��o.
 * @param bufferLinha --> Buffer para ler cada linha do ficheiro.
 * @param MAX_X --> Tamanho m�ximo do buffer para leitura de linhas.
 * @return 0 se as antenas foram carregadas com sucesso, -1 se n�o conseguir abrir o ficheiro.
 */
int carregarAntenasDoFicheiro(Grafo* cidade, const char* nomeFicheiro) {
	FILE* ficheiro = fopen(nomeFicheiro, "r");

	if (nomeFicheiro == NULL) {
		return -1; // Se n�o conseguir abrir o ficheiro, retorna o grafo vazioo
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
#pragma endregion

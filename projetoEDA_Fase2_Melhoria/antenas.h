/*****************************************************************//**
 * \file   antenas.h
 * \brief  Estruturas e funções para manipulação de antenas em um grafo.
 * 
 * \author Vitor Moreira 31553
 * \date   May 2025
 *********************************************************************/
#ifndef FUNCOES_H
#define FUNCOES_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#define _CRT_SECURE_NO_WARNINGS

#pragma region Adjacencias
/**
 * Estrutura que representa uma ligação entre duas antenas.
 * @param destino --> Apontador para a antena de destino.
 * @param proxima --> Apontador para a próxima adjacência na lista de ligações.
 */
typedef struct Adjacencia {
    struct Antena* destino;   
    struct Adjacencia* proxima; 
} Adjacencia;
#pragma endregion

#pragma region Antenas
/**
 * Estrutura que representa uma antena no grafo.
 * @param frequencia --> Frequência de operação da antena (carácter único).
 * @param coluna --> Posição horizontal na matriz (coordenada X).
 * @param linha --> Posição vertical na matriz (coordenada Y).
 * @param ligacoes --> Lista de ligações a outras antenas.
 * @param visitada --> Indicador se a antena foi visitada em algoritmos de travessia.
 * @param proxima --> Apontador para a próxima antena na lista do grafo.
 */
typedef struct Antena {
    char frequencia;            
    int coluna;                
    int linha;                 
    Adjacencia* ligacoes;       
    bool visitada;              
    struct Antena* proxima;     
} Antena;
#pragma endregion

#pragma region CaminhoNo
/**
 * Estrutura que representa um nó em um caminho de antenas.
 * @param antena --> Apontador para a antena no caminho.
 * @param proxima --> Apontador para o próximo nó do caminho.
 */
typedef struct CaminhoNo {
    Antena* antena;             
    struct CaminhoNo* proxima; 
} CaminhoNo;
#pragma endregion

#pragma region FilaNo
/**
 * Estrutura que representa um nó em uma fila de antenas.
 * @param antena --> Apontador para a antena na fila.
 * @param proxima --> Apontador para o próximo nó da fila.
 */
typedef struct FilaNo {
    Antena* antena;            
    struct FilaNo* proxima;    
} FilaNo;
#pragma endregion

#pragma region Grafo
/**
 * Estrutura que representa o grafo de antenas.
 * @param antenas --> Apontador para a primeira antena do grafo.
 * @param totalAntenas --> Número total de antenas no grafo.
 */
typedef struct Grafo {
    Antena* antenas;           
    int totalAntenas;       
} Grafo;
#pragma endregion

#pragma region FuncoesGrafo
/**
 * Funções para manipulação de antenas em um grafo.
 */
bool AdicionarAdj(Antena* origem, Antena* destino);
bool AdicionarAntena(Grafo* grafo, char freq, int col, int lin);
Grafo CarregarAntenasDoFicheiro(const char* nome_ficheiro);
bool TravessiaEmProfundidade(Grafo* grafo, Antena* inicio, FILE* saida);
bool TravessiaEmLargura(Grafo* grafo, Antena* inicio, FILE* saida);
bool EncontrarCaminhos(Grafo* grafo, Antena* origem, Antena* destino, FILE* saida);
bool MostrarIntersecoes(Grafo* grafo, char freqA, char freqB, FILE* saida);
bool MostrarInterferencias(Grafo* grafo, FILE* saida);
bool Resultados(Grafo grafo, const char* nome_ficheiro);
bool LibertarGrafo(Grafo* grafo);
bool LimparVisitados(Grafo* grafo);
#pragma endregion

#endif
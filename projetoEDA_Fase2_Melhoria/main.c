/*****************************************************************//**
 * \file   main.c
 * \brief  Ficheiro principal do programa de an�lise de antenas em um grafo.
 * 
 * \author Vitor Moreira 31553
 * \date   May 2025
 *********************************************************************/
#include "antenas.h"

/**
 * Fun��o main do programa.
 * @param grafo --> Grafo contendo as antenas carregadas do ficheiro.
 * @param totalAntenas --> Total de antenas carregadas.
 * @param Resultados --> Fun��o que gera os resultados das an�lises e os escreve em um ficheiro.
 * @param LibertarGrafo --> Fun��o que liberta a mem�ria alocada para o grafo.
 */
#pragma region Main
int main() {
    // Carregar a rede de antenas do ficheiro
    Grafo grafo = CarregarAntenasDoFicheiro("antenas.txt");

    // Verificar se o carregamento foi bem sucedido
    if (grafo.totalAntenas == 0) {
        return 1;
    }

    // Exportar os resultados das an�lises
    if (!Resultados(grafo, "resultado.txt")) {
        LibertarGrafo(&grafo);
        return 1;
    }

    // Libertar a mem�ria alocada
    LibertarGrafo(&grafo);
    return 0;
}
#pragma endregion
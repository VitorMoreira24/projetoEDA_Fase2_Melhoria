/*****************************************************************//**
 * \file   funcoes.c
 * \brief  Fun��es para manipula��o de antenas em um grafo.
 * 
 * \author Vitor Moreira 31553
 * \date   May 2025
 *********************************************************************/
#define _CRT_SECURE_NO_WARNINGS
#include "antenas.h"

#pragma region AdicionarAdj
/**
 * Fun��o para adicionar uma adjac�ncia entre duas antenas.
 * 
 * @param origem --> Apontador para a antena de origem.
 * @param destino --> Apontador para a antena de destino.
 * @param nova --> Apontador para a nova adjac�ncia a ser adicionada.
 * @param return --> Verdadeiro se a adjac�ncia foi adicionada com sucesso, falso caso contr�rio.
 */
bool AdicionarAdj(Antena* origem, Antena* destino) {
    if (!origem || !destino) return false;

    Adjacencia* nova = malloc(sizeof(Adjacencia));
    if (!nova) return false;

    nova->destino = destino;
    nova->proxima = origem->ligacoes;
    origem->ligacoes = nova;
    return true;
}
#pragma endregion

#pragma region AdicionarAntena
/**
 * Fun��o para adicionar uma nova antena ao grafo.
 * 
 * @param grafo --> Apontador para o grafo onde a antena ser� adicionada.
 * @param freq --> Frequ�ncia da antena (car�cter �nico).
 * @param col --> Posi��o horizontal da antena (coordenada X).
 * @param lin --> Posi��o vertical da antena (coordenada Y).
 * @param return --> Verdadeiro se a antena foi adicionada com sucesso, falso caso contr�rio.
 */
bool AdicionarAntena(Grafo* grafo, char freq, int col, int lin) {
    if (!grafo) return false;

    Antena* nova = malloc(sizeof(Antena));
    if (!nova) return false;

    nova->frequencia = freq;
    nova->coluna = col;
    nova->linha = lin;
    nova->ligacoes = NULL;
    nova->visitada = false;
    nova->proxima = grafo->antenas;
    grafo->antenas = nova;
    grafo->totalAntenas++;
    return true;
}
#pragma endregion

#pragma region CarregarAntenasDoFicheiro
/**
 * Fun��o para carregar antenas de um ficheiro e construir o grafo.
 * 
 * @param nomeFicheiro --> Nome do ficheiro contendo as antenas.
 * @param grafo --> Apontador para o grafo onde as antenas ser�o carregadas.
 * @param ficheiro --> Apontador para o ficheiro aberto.
 * @param numLinhas --> N�mero de linhas do ficheiro (altura da matriz).
 * @param numColunas --> N�mero de colunas do ficheiro (largura da matriz).
 * @param bufferLinha --> Buffer para ler cada linha do ficheiro.
 * @param AdionarAntena --> Fun��o para adicionar uma antena ao grafo.
 * @return Grafo contendo as antenas carregadas.
 */
Grafo CarregarAntenasDoFicheiro(const char* nomeFicheiro) {
    Grafo grafo = { NULL, 0 };

    if (!nomeFicheiro) return grafo;

    FILE* ficheiro = fopen(nomeFicheiro, "r");
    if (!ficheiro) {
        return grafo;
    }

    int numLinhas, numColunas;  // Renomeei para evitar conflito com Antena->linha
    if (fscanf(ficheiro, "%d %d", &numLinhas, &numColunas) != 2) {
        fclose(ficheiro);
        return grafo;
    }

    while (fgetc(ficheiro) != '\n');  // Consome o restante da primeira linha

    char bufferLinha[1024];  // Buffer para ler cada linha do ficheiro
    for (int y = 0; y < numLinhas && fgets(bufferLinha, sizeof(bufferLinha), ficheiro) != NULL; y++) {
        bufferLinha[strcspn(bufferLinha, "\n")] = '\0';  // Remove newline
        for (int x = 0; x < numColunas && x < (int)strlen(bufferLinha); x++) {
            if (bufferLinha[x] != '.' && bufferLinha[x] != ' ') {
                AdicionarAntena(&grafo, bufferLinha[x], x, y);
            }
        }
    }

    fclose(ficheiro);

    // Cria as adjac�ncias entre antenas da mesma frequ�ncia
    for (Antena* a1 = grafo.antenas; a1 != NULL; a1 = a1->proxima) {
        for (Antena* a2 = grafo.antenas; a2 != NULL; a2 = a2->proxima) {
            if (a1 != a2 && a1->frequencia == a2->frequencia) {
                AdicionarAdj(a1, a2);
            }
        }
    }

    return grafo;
}
#pragma endregion

#pragma region TravessiaEmProfundidade
/**
 * Fun��o para realizar uma travessia em profundidade no grafo de antenas..
 * 
 * @param grafo --> Apontador para o grafo cujas antenas ser�o limpas.
 * @param atual --> Apontador para a antena atual.
 * @param saida --> Apontador para o ficheiro onde os resultados ser�o escritos.
 * @param visitada --> Indicador se a antena foi visitada.
 * @param adj --> Apontador para a lista de adjac�ncias da antena atual.
 * @param ligacoes --> Apontador para a lista de liga��es da antena atual.
 * @param proxima --> Apontador para a pr�xima antena na lista do grafo.
 * @param destino --> Apontador para a antena de destino na adjac�ncia.
 * @param return --> Verdadeiro se a travessia foi bem sucedida, falso caso contr�rio.
 */
bool TravessiaEmProfundidade(Grafo* grafo, Antena* atual, FILE* saida) {
    if (!grafo || !atual || !saida) return false;
    if (atual->visitada) return true;

    atual->visitada = true;
    fprintf(saida, "Antena %c @ (%d,%d)\n", atual->frequencia, atual->coluna, atual->linha);

    for (Adjacencia* adj = atual->ligacoes; adj != NULL; adj = adj->proxima) {
        TravessiaEmProfundidade(grafo, adj->destino, saida);
    }
    return true;
}
#pragma endregion

#pragma region TravessiaEmLargura
/**
 * Fun��o para realizar uma travessia em largura no grafo de antenas.
 * 
 * @param grafo --> Apontador para o grafo onde a travessia ser� realizada.
 * @param inicio --> Apontador para a antena de in�cio da travessia.
 * @param saida --> Apontador para o ficheiro onde os resultados ser�o escritos.
 * @param FilaNo --> Estrutura para representar um n� na fila de travessia.
 * @param frente --> Apontador para o in�cio da fila.
 * @param tras --> Apontador para o final da fila.
 * @param novo --> Apontador para um novo n� a ser adicionado � fila.
 * @param visitada --> Indicador se a antena foi visitada.
 * @param Antena --> Estrutura que representa uma antena no grafo.
 * @param temp --> Apontador tempor�rio para armazenar o n� atual da fila.
 * @param Adjacencia --> Estrutura que representa uma liga��o entre antenas.
 * @param adj --> Apontador para a lista de adjac�ncias da antena atual.
 * @param atual --> Apontador para a antena atual sendo processada na travessia.
 * @param novoNo --> Apontador para um novo n� a ser adicionado � fila.
 * @param return --> Verdadeiro se a travessia foi bem sucedida, falso caso contr�rio.
 */
bool TravessiaEmLargura(Grafo* grafo, Antena* inicio, FILE* saida) {
    if (!grafo || !inicio || !saida) return false;

    FilaNo* frente = NULL;
    FilaNo* tras = NULL;

    FilaNo* novo = malloc(sizeof(FilaNo));
    if (!novo) return false;
    novo->antena = inicio;
    novo->proxima = NULL;

    frente = tras = novo;
    inicio->visitada = true;

    while (frente) {
        Antena* atual = frente->antena;
        FilaNo* temp = frente;
        frente = frente->proxima;
        free(temp);

        fprintf(saida, "Antena %c @ (%d,%d)\n", atual->frequencia, atual->coluna, atual->linha);

        for (Adjacencia* adj = atual->ligacoes; adj != NULL; adj = adj->proxima) {
            if (!adj->destino->visitada) {
                adj->destino->visitada = true;
                FilaNo* novoNo = malloc(sizeof(FilaNo));
                if (!novoNo) continue;
                novoNo->antena = adj->destino;
                novoNo->proxima = NULL;

                if (!frente) frente = novoNo;
                else tras->proxima = novoNo;
                tras = novoNo;
            }
        }
    }
    return true;
}
#pragma endregion

#pragma region ImprimirCaminho
/**
 * Fun��o para imprimir o caminho de antenas.
 * 
 * @param caminho --> Apontador para o n� atual do caminho.
 * @param saida --> Apontador para o ficheiro onde o caminho ser� impresso.
 * @param return --> Verdadeiro se o caminho foi impresso com sucesso, falso caso contr�rio.
 */
bool ImprimirCaminho(CaminhoNo* caminho, FILE* saida) {
    if (!caminho) return false;

    ImprimirCaminho(caminho->proxima, saida);  // chamada recursiva sem guardar retorno

    fprintf(saida, "%c(%d,%d)", caminho->antena->frequencia,
        caminho->antena->coluna,
        caminho->antena->linha);
    if (caminho->proxima)
        fprintf(saida, " -> ");

    return true;
}
#pragma endregion

#pragma region EncontrarCaminhosRec
/**
 * Fun��o recursiva para encontrar todos os caminhos entre duas antenas.
 * 
 * @param grafo --> Apontador para o grafo onde as antenas est�o localizadas.
 * @param atual --> Apontador para a antena atual no caminho.
 * @param destino --> Apontador para a antena de destino.
 * @param caminho --> Apontador para o n� atual do caminho.
 * @param saida --> Apontador para o ficheiro onde os caminhos ser�o impressos.
 * @param novo --> Apontador para um novo n� a ser adicionado ao caminho.
 * @param CaminhoNo --> Estrutura que representa um n� em um caminho de antenas.
 * @param ImprimirCaminho --> Fun��o para imprimir o caminho encontrado.
 * @param Adjacencia --> Estrutura que representa uma liga��o entre antenas.
 * @param adj --> Apontador para a lista de adjac�ncias da antena atual.
 * @param EncontrarCaminhosRec --> Fun��o recursiva para encontrar caminhos.
 * @param return --> Verdadeiro se os caminhos foram encontrados com sucesso, falso caso contr�rio.
 */
bool EncontrarCaminhosRec(Grafo* grafo, Antena* atual, Antena* destino, CaminhoNo* caminho, FILE* saida) {
    CaminhoNo* novo = malloc(sizeof(CaminhoNo));
    if (!novo) return false;
    novo->antena = atual;
    novo->proxima = caminho;
    caminho = novo;

    atual->visitada = true;

    if (atual == destino) {
        ImprimirCaminho(caminho, saida);
        fprintf(saida, "\n");
    }
    else {
        for (Adjacencia* adj = atual->ligacoes; adj != NULL; adj = adj->proxima) {
            if (!adj->destino->visitada) {
                EncontrarCaminhosRec(grafo, adj->destino, destino, caminho, saida);
            }
        }
    }

    atual->visitada = false;
    free(caminho);
    return true;
}
#pragma endregion

#pragma region EncontrarCaminhos
/**
 * Fun��o para encontrar todos os caminhos entre duas antenas no grafo.
 *
 * @param grafo --> Apontador para o grafo onde as antenas est�o localizadas.
 * @param origem --> Apontador para a antena de origem.
 * @param destino --> Apontador para a antena de destino.
 * @param saida --> Apontador para o ficheiro onde os caminhos ser�o impressos.
 * @param EncontrarCaminhosRec --> Fun��o recursiva para encontrar caminhos.
 * @param LimparVisitados --> Fun��o para limpar os visitados das antenas.
 * @return Verdadeiro se os caminhos foram encontrados com sucesso, falso caso contr�rio.
 */
bool EncontrarCaminhos(Grafo* grafo, Antena* origem, Antena* destino, FILE* saida) {
    if (!grafo || !origem || !destino || !saida) return false;
    LimparVisitados(grafo);
    return EncontrarCaminhosRec(grafo, origem, destino, NULL, saida);
}
#pragma endregion

#pragma region MostrarIntersecoes
/**
 * Mostra as interse��es entre antenas de diferentes frequ�ncias.
 * 
 * @param grafo --> Apontador para o grafo onde as antenas est�o localizadas.
 * @param freqA --> Frequ�ncia da primeira antena.
 * @param freqB --> Frequ�ncia da segunda antena.
 * @param saida --> Apontador para o ficheiro onde as interse��es ser�o escritas.q
 * @param Antena --> Estrutura que representa uma antena no grafo.
 * @param proxima --> Apontador para a pr�xima antena na lista do grafo.
 * @param return --> Verdadeiro se as interse��es foram mostradas com sucesso, falso caso contr�rio.
 */
bool MostrarIntersecoes(Grafo* grafo, char freqA, char freqB, FILE* saida) {
    if (!grafo || !saida) return false;

    for (Antena* a = grafo->antenas; a != NULL; a = a->proxima) {
        if (a->frequencia == freqA) {
            for (Antena* b = grafo->antenas; b != NULL; b = b->proxima) {
                if (b->frequencia == freqB &&
                    (a->linha == b->linha || a->coluna == b->coluna)) {
                    fprintf(saida, "%c(%d,%d) - %c(%d,%d)\n",
                        freqA, a->coluna, a->linha,
                        freqB, b->coluna, b->linha);
                }
            }
        }
    }
    return true;
}
#pragma endregion

#pragma region MostrarInterferencias
/**
 * Fun��o para mostrar os pontos de interfer�ncia entre antenas.
 * 
 * @param grafo --> Apontador para o grafo onde as antenas est�o localizadas.
 * @param saida --> Apontador para o ficheiro onde os pontos de interfer�ncia ser�o escritos.
 * @param Antena --> Estrutura que representa uma antena no grafo.
 * @param proxima --> Apontador para a pr�xima antena na lista do grafo.
 * @param dx --> Diferen�a entre as colunas das antenas.
 * @param dy --> Diferen�a entre as linhas das antenas.
 * @param k --> Contador para calcular os pontos de interfer�ncia.
 * @param return --> Verdadeiro se os pontos de interfer�ncia foram mostrados com sucesso, falso caso contr�rio.
 */
bool MostrarInterferencias(Grafo* grafo, FILE* saida) {
    if (!grafo || !saida) return false;

    fprintf(saida, "=== PONTOS DE INTERFERENCIA ===\n");

    for (Antena* a1 = grafo->antenas; a1 != NULL; a1 = a1->proxima) {
        for (Antena* a2 = grafo->antenas; a2 != NULL; a2 = a2->proxima) {
            if (a1->frequencia == a2->frequencia) {
                int dx = a2->coluna - a1->coluna;
                int dy = a2->linha - a1->linha;

                if (dx == 0 || dy == 0 || abs(dx) == abs(dy)) {
                    for (int k = 1; k < 3; k++) {
                        int x = a1->coluna + (k * dx) / 3;
                        int y = a1->linha + (k * dy) / 3;
                        fprintf(saida, "(%d,%d)\n", x, y);
                    }
                }
            }
        }
    }
    return true;
}
#pragma endregion

#pragma region Resultados
/**
 * Fun��o para gerar os resultados das an�lises e escrever em um ficheiro.
 *
 * @param grafo --> Grafo contendo as antenas carregadas.
 * @param nomeFicheiro --> Nome do ficheiro onde os resultados ser�o escritos.
 * @param saida --> Apontador para o ficheiro aberto para escrita.
 * @param primeira --> Apontador para a primeira antena no grafo.
 * @param terceira --> Apontador para a terceira antena no grafo (se houver).
 * @param return --> Verdadeiro se os resultados foram gerados com sucesso, falso caso contr�rio.
 */
bool Resultados(Grafo grafo, const char* nomeFicheiro) {
    if (!nomeFicheiro) return false;

    FILE* saida = fopen(nomeFicheiro, "w");
    if (!saida) {
        return false;
    }

    fprintf(saida, "=== ANTENAS (%d) ===\n", grafo.totalAntenas);
    for (Antena* a = grafo.antenas; a != NULL; a = a->proxima) {
        fprintf(saida, "%c @ (%d,%d)\n", a->frequencia, a->coluna, a->linha);
    }

    if (grafo.totalAntenas > 0) {
        Antena* primeira = grafo.antenas;

        fprintf(saida, "\n=== BUSCA EM PROFUNDIDADE ===\n");
        TravessiaEmProfundidade(&grafo, primeira, saida);
        LimparVisitados(&grafo);

        fprintf(saida, "\n=== BUSCA EM LARGURA ===\n");
        TravessiaEmLargura(&grafo, primeira, saida);
        LimparVisitados(&grafo);

        if (grafo.totalAntenas > 2) {
            Antena* terceira = primeira->proxima->proxima;
            fprintf(saida, "\n=== CAMINHOS ENTRE ANTENAS ===\n");
            EncontrarCaminhos(&grafo, primeira, terceira, saida);
        }

        fprintf(saida, "\n=== INTERSECOES ===\n");
        for (Antena* a1 = grafo.antenas; a1 != NULL; a1 = a1->proxima) {
            for (Antena* a2 = grafo.antenas; a2 != NULL; a2 = a2->proxima) {
                if (a1->frequencia != a2->frequencia) {
                    MostrarIntersecoes(&grafo, a1->frequencia, a2->frequencia, saida);
                }
            }
        }

        fprintf(saida, "\n=== INTERFERENCIAS ===\n");
        MostrarInterferencias(&grafo, saida);
    }

    fclose(saida);
    return true;
}
#pragma endregion

#pragma region LibertarGrafo
/**
 * Fun��o para libertar a mem�ria alocada para o grafo e suas antenas.
 *
 * @param grafo --> Apontador para o grafo a ser libertado.
 * @param atual --> Apontador para a antena atual sendo processada.
 * @param adj --> Apontador para a lista de adjac�ncias da antena atual.
 * @param temp --> Apontador tempor�rio para armazenar a antena atual.
 * @return Verdadeiro se o grafo foi libertado com sucesso, falso caso contr�rio.
 */
bool LibertarGrafo(Grafo* grafo) {
    if (!grafo) return false;

    Antena* atual = grafo->antenas;
    while (atual) {
        Adjacencia* adj = atual->ligacoes;
        while (adj) {
            Adjacencia* temp = adj;
            adj = adj->proxima;
            free(temp);
        }

        Antena* temp = atual;
        atual = atual->proxima;
        free(temp);
    }

    grafo->antenas = NULL;
    grafo->totalAntenas = 0;
    return true;
}

#pragma endregion

#pragma region LimparVisitados
/**
 * Fun��o para limpar os visitados de todas as antenas no grafo.
 *
 * @param grafo --> Apontador para o grafo cujas antenas ser�o limpas.
 * @param Antena --> Estrutura que representa uma antena no grafo.
 * @param visitada --> Indicador se a antena foi visitada em algoritmos de travessia.
 * @param proxima --> Apontador para a pr�xima antena na lista do grafo.
 * @return Verdadeiro se a limpeza foi bem sucedida, falso caso contr�rio.
 */
bool LimparVisitados(Grafo* grafo) {
    if (!grafo) return false;
    for (Antena* a = grafo->antenas; a != NULL; a = a->proxima) {
        a->visitada = false;
    }
    return true;
}
#pragma endregion
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
int AdicionarAdj(Antena* origem, Antena* destino) {
	//Se as Antenas de destino ou origem forem nulas, n�o � poss�vel adicionar a adjac�ncia.
    if (!origem || !destino) return -15;

    //Aloca a memoria para a mem�ria para uma nova liga��o entre 2 vertices
    Adjacencia* nova = malloc(sizeof(Adjacencia));
	//Se a aloca��o falhar, retorna falso.
    if (!nova) return -2;
    nova->destino = destino;            //define o vertice de destino da nova adjacencia 
    nova->proxima = origem->ligacoes;   //Liga essa nova liga��o � lista de liga��es que o ponto de origem j� tem
    origem->ligacoes = nova;            //Atualiza o ponto de origem para que a nova liga��o seja a primeira da lista
    return 0;
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
int AdicionarAntena(Grafo* grafo, char freq, int col, int lin) {
    //Verifica se o grafo existe
    if (!grafo) return false;

    //Aloca a memoria para uma nova antena(aresta)
    Antena* nova = malloc(sizeof(Antena));

	//Se a aloca��o falhar, retorna falso.
    if (!nova) return -2;

    //preenche os dados da nova antena nos campos freq, col, linha
    nova->frequencia = freq;
    nova->coluna = col;
    nova->linha = lin;

    //Inicializa a Nova antena sem liga��es e sem ser visitada
    nova->ligacoes = NULL;
    nova->visitada = false;

    //Insere a nova antena no come�o da lista de antenas do grafo
    nova->proxima = grafo->antenas;
    grafo->antenas = nova;

    //Aumenta o total de antenas registadas
    grafo->totalAntenas++;
    return 0;
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
	//Inicializa o grafo vazio
    Grafo grafo = { NULL, 0 };

	//Se o nome do ficheiro for nulo, retorna o grafo vazio
    if (!nomeFicheiro) return grafo;

	//Abre o ficheiro para leitura
    FILE* ficheiro = fopen(nomeFicheiro, "r");
    if (!ficheiro) {
		return grafo; // Se n�o conseguir abrir o ficheiro, retorna o grafo vazio
    }

	//L� as dimens�es do grafo (n�mero de linhas e colunas)
    int numLinhas, numColunas; 
    if (fscanf(ficheiro, "%d %d", &numLinhas, &numColunas) != 2) {
        fclose(ficheiro);
        return grafo;
    }

	while (fgetc(ficheiro) != '\n');  //Ignora o restante da primeira linha(ex se tiver um espa�o)

    char bufferLinha[1024];  // Buffer para ler cada linha do ficheiro
    for (int y = 0; y < numLinhas && fgets(bufferLinha, sizeof(bufferLinha), ficheiro) != NULL; y++) {

        bufferLinha[strcspn(bufferLinha, "\n")] = '\0';//remove o caractere de \n do final da linha

		//Para cada caracter valido adiciona uma antena ao grafo com freq, coluna e linha
        for (int x = 0; x < numColunas && x < (int)strlen(bufferLinha); x++) {
            if (bufferLinha[x] != '.' && bufferLinha[x] != ' ') {
                AdicionarAntena(&grafo, bufferLinha[x], x, y);
            }
        }
    }

	//fecha o ficheiro ap�s a leitura
    fclose(ficheiro);

    // Cria as adjac�ncias entre antenas da mesma frequ�ncia
    //percorre todas as antenas duas a duas 
    for (Antena* a1 = grafo.antenas; a1 != NULL; a1 = a1->proxima) {
        for (Antena* a2 = grafo.antenas; a2 != NULL; a2 = a2->proxima) {
            //se nao forem a mesma antena e tiverem a mesma frequencia
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
int TravessiaEmProfundidade(Grafo* grafo, Antena* atual, FILE* saida) {
	if (!grafo || !atual || !saida) return -3; //se o grafo, antena atual e o arquivo de saida forem nulos, retorna falso
	if (atual->visitada) return 0; //se a antena atual j� tiver sido visitada, retorna verdadeiro

	atual->visitada = true; //marca a antena atual como visitada

    //Imprime no arquivo (ou onde for passado como saida) a frequ�ncia e a posi��o (coluna e linha) da antena atual.
    fprintf(saida, "Antena %c @ (%d,%d)\n", atual->frequencia, atual->coluna, atual->linha); 

	//Percorre todas as liga��es da antena atual
    for (Adjacencia* adj = atual->ligacoes; adj != NULL; adj = adj->proxima) {
		//para cada antena ligada a antena atual, chama recursivamente para visitar a proxima antena
        TravessiaEmProfundidade(grafo, adj->destino, saida);
    }
    return 0;
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
int TravessiaEmLargura(Grafo* grafo, Antena* inicio, FILE* saida) {
    //se o grafo, antena atual e o arquivo de saida forem nulos, retorna falso
    if (!grafo || !inicio || !saida) return -4;

    //Declara duas vari�veis para controlar a fila: frente (in�cio da fila) e tras (final da fila)
    FilaNo* frente = NULL;
    FilaNo* tras = NULL;

	//Aloca mem�ria para um novo n� da fila e verifica se a aloca��o foi bem sucedida
    FilaNo* novo = malloc(sizeof(FilaNo));
    if (!novo) return -5;
    novo->antena = inicio;
    novo->proxima = NULL;

    frente = tras = novo;//Define que o primeiro e �ltimo elemento da fila � esse n�
    inicio->visitada = true;//Marca a antena de in�cio como visitada para n�o visit�-la novamente

	//Enquanto houver elementos na fila, processa cada antena
    while (frente) {

		Antena* atual = frente->antena; //Pega a antena atual do in�cio da fila
		FilaNo* temp = frente; //Armazena o n� atual da fila para liberar mem�ria depois
		frente = frente->proxima; //Move o in�cio da fila para o pr�ximo n�
        free(temp);

        //Imprime no arquivo as informa��es da antena atual
        fprintf(saida, "Antena %c @ (%d,%d)\n", atual->frequencia, atual->coluna, atual->linha);

		//Percorre todas as liga��es da antena atual
        for (Adjacencia* adj = atual->ligacoes; adj != NULL; adj = adj->proxima) {

			//Se a antena de destino ainda n�o foi visitada, marca como visitada e adiciona � fila
            if (!adj->destino->visitada) {
                adj->destino->visitada = true;

				//Aloca mem�ria para um novo n� da fila
                FilaNo* novoNo = malloc(sizeof(FilaNo));
                if (!novoNo) continue;

                //Adiciona esse novo n� no final
                novoNo->antena = adj->destino;
                novoNo->proxima = NULL;

                if (!frente) frente = novoNo;
                else tras->proxima = novoNo;
                tras = novoNo;
            }
        }
    }
    return 0;
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
int ImprimirCaminho(CaminhoNo* caminho, FILE* saida) {
	// Se o caminho for nulo, n�o h� nada a imprimir
    if (!caminho) return -6;

    //Chama a fun��o recursivamente para imprimir o pr�ximo elemento da lista antes de imprimir o atual
    ImprimirCaminho(caminho->proxima, saida);

    //Imprime no arquivo 
    fprintf(saida, "%c(%d,%d)", caminho->antena->frequencia,
        caminho->antena->coluna,
        caminho->antena->linha);
    if (caminho->proxima)
        fprintf(saida, " -> ");

    return 0;
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
int EncontrarCaminhosRec(Grafo* grafo, Antena* atual, Antena* destino, CaminhoNo* caminho, FILE* saida) {

	//Cria um novo n� para guardar a antena atual no caminho e se n�o conseguir alocar mem�ria, retorna falso
    CaminhoNo* novo = malloc(sizeof(CaminhoNo));
    if (!novo) return -7;

    novo->antena = atual; //Guarda a antena atual no novo n�
	novo->proxima = caminho; //Coloca o novo n� no in�cio do caminho
    caminho = novo;

    atual->visitada = true; //Marca como visitada 

	//Se a antena atual for a antena de destino, imprime o caminho encontrado
    if (atual == destino) {
        ImprimirCaminho(caminho, saida);
        fprintf(saida, "\n");
    }
    else {
		//Caso contr�rio, percorre todas as liga��es da antena atual recursivamente
        for (Adjacencia* adj = atual->ligacoes; adj != NULL; adj = adj->proxima) {
            if (!adj->destino->visitada) {
                EncontrarCaminhosRec(grafo, adj->destino, destino, caminho, saida);
            }
        }
    }

	atual->visitada = false; //Quando acaba de procurar ,desmarca a antena atual para permitir verificar outros caminhos
    free(caminho);
    return 0;
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
int EncontrarCaminhos(Grafo* grafo, Antena* origem, Antena* destino, FILE* saida) {
	// Se o grafo, origem, destino ou saida forem nulos, n�o � poss�vel encontrar caminhos
    if (!grafo || !origem || !destino || !saida) return false;

    //Reseta o estado de todas as antenas para n�o visitadas
    LimparVisitados(grafo);
    //Chama a fun��o recursiva que vai procurar todos os caminhos da antena origem at� a destino
    //Passa NULL como caminho inicial (porque ainda n�o tem antenas no caminho)
    //Passa o arquivo saida para onde os caminhos encontrados ser�o impressos
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
int MostrarIntersecoes(Grafo* grafo, char freqA, char freqB, FILE* saida) {
    //Se o grafo ou o arquivo de sa�da forem nulos, retorna falso 
    if (!grafo || !saida) return -8;

    //Percorre todas as antenas do grafo (cada antena � apontada por a)
    for (Antena* a = grafo->antenas; a != NULL; a = a->proxima) {

		//Se a antena atual tiver a frequ�ncia freqA
        if (a->frequencia == freqA) {
			//Procura por antenas com a frequ�ncia freqB que estejam na mesma linha ou coluna
            for (Antena* b = grafo->antenas; b != NULL; b = b->proxima) {
				//Se a antena b tiver a frequ�ncia freqB e estiver na mesma linha ou coluna que A
                if (b->frequencia == freqB && (a->linha == b->linha || a->coluna == b->coluna)) {
					//Escreve no arquivo de sa�da a interse��o entre as antenas A e B
                    fprintf(saida, "%c(%d,%d) - %c(%d,%d)\n",
                        freqA, a->coluna, a->linha,
                        freqB, b->coluna, b->linha);
                }
            }
        }
    }
    return 0;
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
int MostrarInterferencias(Grafo* grafo, FILE* saida) {
    //Se o grafo ou o arquivo de sa�da forem nulos, retorna falso
    if (!grafo || !saida) return -9;

    fprintf(saida, "=== PONTOS DE INTERFERENCIA ===\n");

    //Faz um duplo loop para comparar todas as antenas com todas as outras
    for (Antena* a1 = grafo->antenas; a1 != NULL; a1 = a1->proxima) {
        for (Antena* a2 = grafo->antenas; a2 != NULL; a2 = a2->proxima) {
            //Considera s� antenas que tenham a mesma frequ�ncia
            if (a1->frequencia == a2->frequencia) {
                //Calcula a diferen�a nas colunas (x) e nas linhas (y) entre as duas antenas
                int dx = a2->coluna - a1->coluna;
                int dy = a2->linha - a1->linha;

                //Verifica se as antenas est�o alinhadas:Na mesma coluna(dx == 0),Na mesma linha(dy == 0),Ou numa diagonal perfeita
                if (dx == 0 || dy == 0 || abs(dx) == abs(dy)) {
                    for (int k = 1; k < 3; k++) {
                        int x = a1->coluna + (k * dx) / 3;
                        int y = a1->linha + (k * dy) / 3;

						//Escreve no arquivo de sa�da o ponto de interfer�ncia calculado
                        fprintf(saida, "(%d,%d)\n", x, y);
                    }
                }
            }
        }
    }
    return 0;
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
int Resultados(Grafo grafo, const char* nomeFicheiro) {
    //se o nome do ficheiro for invalido retorna falso
    if (!nomeFicheiro) return -10;

    //tenta abrir o ficheiro para escrita
    FILE* saida = fopen(nomeFicheiro, "w");
    if (!saida) {
        return -11;
    }

    //escreve o numero total de antenas no inicio
    fprintf(saida, "=== ANTENAS (%d) ===\n", grafo.totalAntenas);

    //percorre todas as antenas e as suas posi��es e frequencias 
    for (Antena* a = grafo.antenas; a != NULL; a = a->proxima) {
        fprintf(saida, "%c @ (%d,%d)\n", a->frequencia, a->coluna, a->linha);
    }

    //se houver antenas 
    if (grafo.totalAntenas > 0) {
        Antena* primeira = grafo.antenas;

        //procura e escreve em profundidade
        fprintf(saida, "\n=== PROCURA EM PROFUNDIDADE ===\n");
        TravessiaEmProfundidade(&grafo, primeira, saida);
        LimparVisitados(&grafo);
        
        //procura e escreve em largura
        fprintf(saida, "\n=== PROCURA EM LARGURA ===\n");
        TravessiaEmLargura(&grafo, primeira, saida);
        LimparVisitados(&grafo);

        //se houver mais de duas antenas ou at� uma terceira 
        if (grafo.totalAntenas > 2) {
            //mostra os caminhos entre elas
            Antena* terceira = primeira->proxima->proxima;
            fprintf(saida, "\n=== CAMINHOS ENTRE ANTENAS ===\n");
            EncontrarCaminhos(&grafo, primeira, terceira, saida);
        }

        //mostra todas as interse��es
        fprintf(saida, "\n=== INTERSECOES ===\n");
        for (Antena* a1 = grafo.antenas; a1 != NULL; a1 = a1->proxima) {
            for (Antena* a2 = grafo.antenas; a2 != NULL; a2 = a2->proxima) {
                if (a1->frequencia != a2->frequencia) {
                    MostrarIntersecoes(&grafo, a1->frequencia, a2->frequencia, saida);
                }
            }
        }

        //mostra as interderencias
        fprintf(saida, "\n=== INTERFERENCIAS ===\n");
        MostrarInterferencias(&grafo, saida);
    }

    fclose(saida);
    return 0;
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
int LibertarGrafo(Grafo* grafo) {
    //Se o ponteiro para o grafo for nulo retorma falso
    if (!grafo) return -13;

    //come�a pela primeria antena da lista
    Antena* atual = grafo->antenas;
    
    //vai percirrebdi todas as antenas e liberta aS Liga��es
    while (atual) {
        Adjacencia* adj = atual->ligacoes;
        while (adj) {
            Adjacencia* temp = adj;
            adj = adj->proxima;
            free(temp);
        }

        //depois liberta as anenas
        Antena* temp = atual;
        atual = atual->proxima;
        free(temp);
    }

    //marca agora que o grafo esta vazio 
    grafo->antenas = NULL;
    grafo->totalAntenas = 0;
    return 0;
}

#pragma endregion

#pragma region LimparVisitados
/**
 * Fun��o para limpar os visitados de todas as antenas no grafo
 *
 * @param grafo --> Apontador para o grafo cujas antenas ser�o limpas.
 * @param Antena --> Estrutura que representa uma antena no grafo.
 * @param visitada --> Indicador se a antena foi visitada em algoritmos de travessia.
 * @param proxima --> Apontador para a pr�xima antena na lista do grafo.
 * @return Verdadeiro se a limpeza foi bem sucedida, falso caso contr�rio.
 */
int LimparVisitados(Grafo* grafo) {
    //se o grafo for vazio retorna falso 
    if (!grafo) return -14;

    //vai percorrendo todas as antenas desdo inicio e desmarcando o visitado
    for (Antena* a = grafo->antenas; a != NULL; a = a->proxima) {
        a->visitada = false;
    }
    return 0;
}
#pragma endregion
/*****************************************************************//**
 * \file   funcoes.c
 * \brief  Funções para manipulação de antenas em um grafo.
 * 
 * \author Vitor Moreira 31553
 * \date   May 2025
 *********************************************************************/
#define _CRT_SECURE_NO_WARNINGS
#include "antenas.h"

#pragma region AdicionarAdj
/**
 * Função para adicionar uma adjacência entre duas antenas.
 * 
 * @param origem --> Apontador para a antena de origem.
 * @param destino --> Apontador para a antena de destino.
 * @param nova --> Apontador para a nova adjacência a ser adicionada.
 * @param return --> Verdadeiro se a adjacência foi adicionada com sucesso, falso caso contrário.
 */
int AdicionarAdj(Antena* origem, Antena* destino) {
	//Se as Antenas de destino ou origem forem nulas, não é possível adicionar a adjacência.
    if (!origem || !destino) return -15;

    //Aloca a memoria para a memória para uma nova ligação entre 2 vertices
    Adjacencia* nova = malloc(sizeof(Adjacencia));
	//Se a alocação falhar, retorna falso.
    if (!nova) return -2;
    nova->destino = destino;            //define o vertice de destino da nova adjacencia 
    nova->proxima = origem->ligacoes;   //Liga essa nova ligação à lista de ligações que o ponto de origem já tem
    origem->ligacoes = nova;            //Atualiza o ponto de origem para que a nova ligação seja a primeira da lista
    return 0;
}
#pragma endregion

#pragma region AdicionarAntena
/**
 * Função para adicionar uma nova antena ao grafo.
 * 
 * @param grafo --> Apontador para o grafo onde a antena será adicionada.
 * @param freq --> Frequência da antena (carácter único).
 * @param col --> Posição horizontal da antena (coordenada X).
 * @param lin --> Posição vertical da antena (coordenada Y).
 * @param return --> Verdadeiro se a antena foi adicionada com sucesso, falso caso contrário.
 */
int AdicionarAntena(Grafo* grafo, char freq, int col, int lin) {
    //Verifica se o grafo existe
    if (!grafo) return false;

    //Aloca a memoria para uma nova antena(aresta)
    Antena* nova = malloc(sizeof(Antena));

	//Se a alocação falhar, retorna falso.
    if (!nova) return -2;

    //preenche os dados da nova antena nos campos freq, col, linha
    nova->frequencia = freq;
    nova->coluna = col;
    nova->linha = lin;

    //Inicializa a Nova antena sem ligações e sem ser visitada
    nova->ligacoes = NULL;
    nova->visitada = false;

    //Insere a nova antena no começo da lista de antenas do grafo
    nova->proxima = grafo->antenas;
    grafo->antenas = nova;

    //Aumenta o total de antenas registadas
    grafo->totalAntenas++;
    return 0;
}
#pragma endregion

#pragma region CarregarAntenasDoFicheiro
/**
 * Função para carregar antenas de um ficheiro e construir o grafo.
 * 
 * @param nomeFicheiro --> Nome do ficheiro contendo as antenas.
 * @param grafo --> Apontador para o grafo onde as antenas serão carregadas.
 * @param ficheiro --> Apontador para o ficheiro aberto.
 * @param numLinhas --> Número de linhas do ficheiro (altura da matriz).
 * @param numColunas --> Número de colunas do ficheiro (largura da matriz).
 * @param bufferLinha --> Buffer para ler cada linha do ficheiro.
 * @param AdionarAntena --> Função para adicionar uma antena ao grafo.
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
		return grafo; // Se não conseguir abrir o ficheiro, retorna o grafo vazio
    }

	//Lê as dimensões do grafo (número de linhas e colunas)
    int numLinhas, numColunas; 
    if (fscanf(ficheiro, "%d %d", &numLinhas, &numColunas) != 2) {
        fclose(ficheiro);
        return grafo;
    }

	while (fgetc(ficheiro) != '\n');  //Ignora o restante da primeira linha(ex se tiver um espaço)

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

	//fecha o ficheiro após a leitura
    fclose(ficheiro);

    // Cria as adjacências entre antenas da mesma frequência
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
 * Função para realizar uma travessia em profundidade no grafo de antenas..
 * 
 * @param grafo --> Apontador para o grafo cujas antenas serão limpas.
 * @param atual --> Apontador para a antena atual.
 * @param saida --> Apontador para o ficheiro onde os resultados serão escritos.
 * @param visitada --> Indicador se a antena foi visitada.
 * @param adj --> Apontador para a lista de adjacências da antena atual.
 * @param ligacoes --> Apontador para a lista de ligações da antena atual.
 * @param proxima --> Apontador para a próxima antena na lista do grafo.
 * @param destino --> Apontador para a antena de destino na adjacência.
 * @param return --> Verdadeiro se a travessia foi bem sucedida, falso caso contrário.
 */
int TravessiaEmProfundidade(Grafo* grafo, Antena* atual, FILE* saida) {
	if (!grafo || !atual || !saida) return -3; //se o grafo, antena atual e o arquivo de saida forem nulos, retorna falso
	if (atual->visitada) return 0; //se a antena atual já tiver sido visitada, retorna verdadeiro

	atual->visitada = true; //marca a antena atual como visitada

    //Imprime no arquivo (ou onde for passado como saida) a frequência e a posição (coluna e linha) da antena atual.
    fprintf(saida, "Antena %c @ (%d,%d)\n", atual->frequencia, atual->coluna, atual->linha); 

	//Percorre todas as ligações da antena atual
    for (Adjacencia* adj = atual->ligacoes; adj != NULL; adj = adj->proxima) {
		//para cada antena ligada a antena atual, chama recursivamente para visitar a proxima antena
        TravessiaEmProfundidade(grafo, adj->destino, saida);
    }
    return 0;
}
#pragma endregion

#pragma region TravessiaEmLargura
/**
 * Função para realizar uma travessia em largura no grafo de antenas.
 * 
 * @param grafo --> Apontador para o grafo onde a travessia será realizada.
 * @param inicio --> Apontador para a antena de início da travessia.
 * @param saida --> Apontador para o ficheiro onde os resultados serão escritos.
 * @param FilaNo --> Estrutura para representar um nó na fila de travessia.
 * @param frente --> Apontador para o início da fila.
 * @param tras --> Apontador para o final da fila.
 * @param novo --> Apontador para um novo nó a ser adicionado à fila.
 * @param visitada --> Indicador se a antena foi visitada.
 * @param Antena --> Estrutura que representa uma antena no grafo.
 * @param temp --> Apontador temporário para armazenar o nó atual da fila.
 * @param Adjacencia --> Estrutura que representa uma ligação entre antenas.
 * @param adj --> Apontador para a lista de adjacências da antena atual.
 * @param atual --> Apontador para a antena atual sendo processada na travessia.
 * @param novoNo --> Apontador para um novo nó a ser adicionado à fila.
 * @param return --> Verdadeiro se a travessia foi bem sucedida, falso caso contrário.
 */
int TravessiaEmLargura(Grafo* grafo, Antena* inicio, FILE* saida) {
    //se o grafo, antena atual e o arquivo de saida forem nulos, retorna falso
    if (!grafo || !inicio || !saida) return -4;

    //Declara duas variáveis para controlar a fila: frente (início da fila) e tras (final da fila)
    FilaNo* frente = NULL;
    FilaNo* tras = NULL;

	//Aloca memória para um novo nó da fila e verifica se a alocação foi bem sucedida
    FilaNo* novo = malloc(sizeof(FilaNo));
    if (!novo) return -5;
    novo->antena = inicio;
    novo->proxima = NULL;

    frente = tras = novo;//Define que o primeiro e último elemento da fila é esse nó
    inicio->visitada = true;//Marca a antena de início como visitada para não visitá-la novamente

	//Enquanto houver elementos na fila, processa cada antena
    while (frente) {

		Antena* atual = frente->antena; //Pega a antena atual do início da fila
		FilaNo* temp = frente; //Armazena o nó atual da fila para liberar memória depois
		frente = frente->proxima; //Move o início da fila para o próximo nó
        free(temp);

        //Imprime no arquivo as informações da antena atual
        fprintf(saida, "Antena %c @ (%d,%d)\n", atual->frequencia, atual->coluna, atual->linha);

		//Percorre todas as ligações da antena atual
        for (Adjacencia* adj = atual->ligacoes; adj != NULL; adj = adj->proxima) {

			//Se a antena de destino ainda não foi visitada, marca como visitada e adiciona à fila
            if (!adj->destino->visitada) {
                adj->destino->visitada = true;

				//Aloca memória para um novo nó da fila
                FilaNo* novoNo = malloc(sizeof(FilaNo));
                if (!novoNo) continue;

                //Adiciona esse novo nó no final
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
 * Função para imprimir o caminho de antenas.
 * 
 * @param caminho --> Apontador para o nó atual do caminho.
 * @param saida --> Apontador para o ficheiro onde o caminho será impresso.
 * @param return --> Verdadeiro se o caminho foi impresso com sucesso, falso caso contrário.
 */
int ImprimirCaminho(CaminhoNo* caminho, FILE* saida) {
	// Se o caminho for nulo, não há nada a imprimir
    if (!caminho) return -6;

    //Chama a função recursivamente para imprimir o próximo elemento da lista antes de imprimir o atual
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
 * Função recursiva para encontrar todos os caminhos entre duas antenas.
 * 
 * @param grafo --> Apontador para o grafo onde as antenas estão localizadas.
 * @param atual --> Apontador para a antena atual no caminho.
 * @param destino --> Apontador para a antena de destino.
 * @param caminho --> Apontador para o nó atual do caminho.
 * @param saida --> Apontador para o ficheiro onde os caminhos serão impressos.
 * @param novo --> Apontador para um novo nó a ser adicionado ao caminho.
 * @param CaminhoNo --> Estrutura que representa um nó em um caminho de antenas.
 * @param ImprimirCaminho --> Função para imprimir o caminho encontrado.
 * @param Adjacencia --> Estrutura que representa uma ligação entre antenas.
 * @param adj --> Apontador para a lista de adjacências da antena atual.
 * @param EncontrarCaminhosRec --> Função recursiva para encontrar caminhos.
 * @param return --> Verdadeiro se os caminhos foram encontrados com sucesso, falso caso contrário.
 */
int EncontrarCaminhosRec(Grafo* grafo, Antena* atual, Antena* destino, CaminhoNo* caminho, FILE* saida) {

	//Cria um novo nó para guardar a antena atual no caminho e se não conseguir alocar memória, retorna falso
    CaminhoNo* novo = malloc(sizeof(CaminhoNo));
    if (!novo) return -7;

    novo->antena = atual; //Guarda a antena atual no novo nó
	novo->proxima = caminho; //Coloca o novo nó no início do caminho
    caminho = novo;

    atual->visitada = true; //Marca como visitada 

	//Se a antena atual for a antena de destino, imprime o caminho encontrado
    if (atual == destino) {
        ImprimirCaminho(caminho, saida);
        fprintf(saida, "\n");
    }
    else {
		//Caso contrário, percorre todas as ligações da antena atual recursivamente
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
 * Função para encontrar todos os caminhos entre duas antenas no grafo.
 *
 * @param grafo --> Apontador para o grafo onde as antenas estão localizadas.
 * @param origem --> Apontador para a antena de origem.
 * @param destino --> Apontador para a antena de destino.
 * @param saida --> Apontador para o ficheiro onde os caminhos serão impressos.
 * @param EncontrarCaminhosRec --> Função recursiva para encontrar caminhos.
 * @param LimparVisitados --> Função para limpar os visitados das antenas.
 * @return Verdadeiro se os caminhos foram encontrados com sucesso, falso caso contrário.
 */
int EncontrarCaminhos(Grafo* grafo, Antena* origem, Antena* destino, FILE* saida) {
	// Se o grafo, origem, destino ou saida forem nulos, não é possível encontrar caminhos
    if (!grafo || !origem || !destino || !saida) return false;

    //Reseta o estado de todas as antenas para não visitadas
    LimparVisitados(grafo);
    //Chama a função recursiva que vai procurar todos os caminhos da antena origem até a destino
    //Passa NULL como caminho inicial (porque ainda não tem antenas no caminho)
    //Passa o arquivo saida para onde os caminhos encontrados serão impressos
    return EncontrarCaminhosRec(grafo, origem, destino, NULL, saida);
}
#pragma endregion

#pragma region MostrarIntersecoes
/**
 * Mostra as interseções entre antenas de diferentes frequências.
 * 
 * @param grafo --> Apontador para o grafo onde as antenas estão localizadas.
 * @param freqA --> Frequência da primeira antena.
 * @param freqB --> Frequência da segunda antena.
 * @param saida --> Apontador para o ficheiro onde as interseções serão escritas.q
 * @param Antena --> Estrutura que representa uma antena no grafo.
 * @param proxima --> Apontador para a próxima antena na lista do grafo.
 * @param return --> Verdadeiro se as interseções foram mostradas com sucesso, falso caso contrário.
 */
int MostrarIntersecoes(Grafo* grafo, char freqA, char freqB, FILE* saida) {
    //Se o grafo ou o arquivo de saída forem nulos, retorna falso 
    if (!grafo || !saida) return -8;

    //Percorre todas as antenas do grafo (cada antena é apontada por a)
    for (Antena* a = grafo->antenas; a != NULL; a = a->proxima) {

		//Se a antena atual tiver a frequência freqA
        if (a->frequencia == freqA) {
			//Procura por antenas com a frequência freqB que estejam na mesma linha ou coluna
            for (Antena* b = grafo->antenas; b != NULL; b = b->proxima) {
				//Se a antena b tiver a frequência freqB e estiver na mesma linha ou coluna que A
                if (b->frequencia == freqB && (a->linha == b->linha || a->coluna == b->coluna)) {
					//Escreve no arquivo de saída a interseção entre as antenas A e B
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
 * Função para mostrar os pontos de interferência entre antenas.
 * 
 * @param grafo --> Apontador para o grafo onde as antenas estão localizadas.
 * @param saida --> Apontador para o ficheiro onde os pontos de interferência serão escritos.
 * @param Antena --> Estrutura que representa uma antena no grafo.
 * @param proxima --> Apontador para a próxima antena na lista do grafo.
 * @param dx --> Diferença entre as colunas das antenas.
 * @param dy --> Diferença entre as linhas das antenas.
 * @param k --> Contador para calcular os pontos de interferência.
 * @param return --> Verdadeiro se os pontos de interferência foram mostrados com sucesso, falso caso contrário.
 */
int MostrarInterferencias(Grafo* grafo, FILE* saida) {
    //Se o grafo ou o arquivo de saída forem nulos, retorna falso
    if (!grafo || !saida) return -9;

    fprintf(saida, "=== PONTOS DE INTERFERENCIA ===\n");

    //Faz um duplo loop para comparar todas as antenas com todas as outras
    for (Antena* a1 = grafo->antenas; a1 != NULL; a1 = a1->proxima) {
        for (Antena* a2 = grafo->antenas; a2 != NULL; a2 = a2->proxima) {
            //Considera só antenas que tenham a mesma frequência
            if (a1->frequencia == a2->frequencia) {
                //Calcula a diferença nas colunas (x) e nas linhas (y) entre as duas antenas
                int dx = a2->coluna - a1->coluna;
                int dy = a2->linha - a1->linha;

                //Verifica se as antenas estão alinhadas:Na mesma coluna(dx == 0),Na mesma linha(dy == 0),Ou numa diagonal perfeita
                if (dx == 0 || dy == 0 || abs(dx) == abs(dy)) {
                    for (int k = 1; k < 3; k++) {
                        int x = a1->coluna + (k * dx) / 3;
                        int y = a1->linha + (k * dy) / 3;

						//Escreve no arquivo de saída o ponto de interferência calculado
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
 * Função para gerar os resultados das análises e escrever em um ficheiro.
 *
 * @param grafo --> Grafo contendo as antenas carregadas.
 * @param nomeFicheiro --> Nome do ficheiro onde os resultados serão escritos.
 * @param saida --> Apontador para o ficheiro aberto para escrita.
 * @param primeira --> Apontador para a primeira antena no grafo.
 * @param terceira --> Apontador para a terceira antena no grafo (se houver).
 * @param return --> Verdadeiro se os resultados foram gerados com sucesso, falso caso contrário.
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

    //percorre todas as antenas e as suas posições e frequencias 
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

        //se houver mais de duas antenas ou até uma terceira 
        if (grafo.totalAntenas > 2) {
            //mostra os caminhos entre elas
            Antena* terceira = primeira->proxima->proxima;
            fprintf(saida, "\n=== CAMINHOS ENTRE ANTENAS ===\n");
            EncontrarCaminhos(&grafo, primeira, terceira, saida);
        }

        //mostra todas as interseções
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
 * Função para libertar a memória alocada para o grafo e suas antenas.
 *
 * @param grafo --> Apontador para o grafo a ser libertado.
 * @param atual --> Apontador para a antena atual sendo processada.
 * @param adj --> Apontador para a lista de adjacências da antena atual.
 * @param temp --> Apontador temporário para armazenar a antena atual.
 * @return Verdadeiro se o grafo foi libertado com sucesso, falso caso contrário.
 */
int LibertarGrafo(Grafo* grafo) {
    //Se o ponteiro para o grafo for nulo retorma falso
    if (!grafo) return -13;

    //começa pela primeria antena da lista
    Antena* atual = grafo->antenas;
    
    //vai percirrebdi todas as antenas e liberta aS Ligações
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
 * Função para limpar os visitados de todas as antenas no grafo
 *
 * @param grafo --> Apontador para o grafo cujas antenas serão limpas.
 * @param Antena --> Estrutura que representa uma antena no grafo.
 * @param visitada --> Indicador se a antena foi visitada em algoritmos de travessia.
 * @param proxima --> Apontador para a próxima antena na lista do grafo.
 * @return Verdadeiro se a limpeza foi bem sucedida, falso caso contrário.
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
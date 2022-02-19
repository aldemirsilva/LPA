#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

struct evento_t {
    double tempo;
    int alvo, tipo;
};
typedef struct evento_t evento_t;

struct lista_eventos_t {
    evento_t* evento;
    struct lista_eventos_t* proximo;
};
typedef struct lista_eventos_t lista_eventos_t;

struct lista_vizinhos {
	int id;
	struct lista_vizinhos *proximo;
};
typedef struct lista_vizinhos lista_vizinhos_t;

struct no {
    int id;
	double pos_x, pos_y;
	lista_vizinhos_t *lista;
    bool pacote_enviado;
};
typedef struct no no_t;
typedef no_t *grafo_t;

bool lista_eventos_adicionar_ordenado(evento_t *evento, lista_eventos_t **lista) {
    lista_eventos_t* item_novo = malloc(sizeof(lista_eventos_t));
    if (item_novo == NULL) return false;
    item_novo->evento = evento;
    // item_novo->proximo = *lista;

    lista_eventos_t* item_anterior = NULL;
    lista_eventos_t* item_atual = *lista;

    while (item_atual != NULL && item_novo->evento->tempo > item_atual->evento->tempo) { // iteração para encontrar ponto de inserçao
        item_anterior = item_atual;
        item_atual = item_atual->proximo;
    }
    if (item_atual == *lista) { // lista vazia ou início da lista
        item_novo->proximo = *lista; // aponta para NULL ou primeiro elemento
        *lista = item_novo; // muda o ponteiro da lista
    }
    else if (item_atual == NULL) { // fim da lista
        item_anterior->proximo = item_novo;
        item_novo->proximo = NULL;
    }
    else { // ponto intermediário
        item_anterior->proximo = item_novo;
        item_novo->proximo = item_atual;
    }
    return true;
}

bool lista_vizinhos_adicionar(int vizinho, lista_vizinhos_t **lista) {
    lista_vizinhos_t *novo = malloc(sizeof(lista_vizinhos_t));
	if (!novo) return false;
	novo->id = vizinho;
	novo->proximo = *lista;
	*lista = novo;
	return true;
}

grafo_t grafo_criar(int tam) {
	grafo_t novo = malloc(sizeof(no_t)*tam);
    if (!novo) exit(EXIT_FAILURE);
	return novo;
}

void grafo_atualizar_vizinhos(int tam, double raio_comunicacao, grafo_t grafo) {
    for (int i = 0; i < tam; i++) {
		grafo_t no = &grafo[i];
        for (int j = i + 1; j < tam; j++) {
			grafo_t vizinho = &grafo[j];
            if (i != j && (sqrt(pow(grafo[i].pos_x - grafo[j].pos_x, 2) + pow(grafo[i].pos_y - grafo[j].pos_y, 2)) <= raio_comunicacao)) {
                lista_vizinhos_adicionar(vizinho->id, &no->lista);
                lista_vizinhos_adicionar(no->id, &vizinho->lista);
			}
		}
	}
}

evento_t *evento_criar(int alvo, int tipo, double tempo) {
    evento_t *novo = malloc(sizeof(evento_t));
    novo->alvo = alvo;
    novo->tipo = tipo;
    novo->tempo = tempo;
    return novo;
}

void simulacao_iniciar(lista_eventos_t *lista_eventos, grafo_t grafo) {
    evento_t *evento_ptr;
	lista_vizinhos_t *lista_vizinhos;
	int alvo;
	double tempo;
	while (lista_eventos) {
		evento_ptr = lista_eventos->evento;
		lista_eventos = lista_eventos->proximo;
		alvo = evento_ptr->alvo;
		tempo = evento_ptr->tempo;
		printf("[%3.6f] No %d recebeu pacote.\n", tempo, alvo);
		if (!grafo[alvo].pacote_enviado) {
			lista_vizinhos = grafo[alvo].lista;
			while (lista_vizinhos) {
				printf("\t--> Repassando pacote para o nó %d ...\n", lista_vizinhos->id);
				evento_ptr = evento_criar(lista_vizinhos->id, 1, tempo + (0.1f + (lista_vizinhos->id*0.01f)));
				lista_eventos_adicionar_ordenado(evento_ptr, &lista_eventos);
				lista_vizinhos = lista_vizinhos->proximo;
			}
			grafo[alvo].pacote_enviado = true;
		}
    }
}

int main(int argc, char *argv[]) {
	int tam;
	double raio_comunicacao;
	grafo_t grafo;
	FILE *arquivo = fopen(argv[1], "r");
	if (!arquivo) exit(EXIT_FAILURE);
	else {
		fscanf(arquivo, "%d%lf", &tam, &raio_comunicacao);
		grafo = grafo_criar(tam);
		for (int i = 0; i < tam; i++) {
			fscanf(arquivo, "%d%lf%lf", &grafo[i].id, &grafo[i].pos_x, &grafo[i].pos_y);
			grafo[i].lista = NULL;
			grafo[i].pacote_enviado = false;
		}
		grafo_atualizar_vizinhos(tam, raio_comunicacao, grafo);
	}
	lista_eventos_t *lista_eventos = NULL;
	evento_t *evento = evento_criar(0, 1, 1.0);
	lista_eventos_adicionar_ordenado(evento, &lista_eventos);
	simulacao_iniciar(lista_eventos, grafo);
	fclose(arquivo);
	exit(EXIT_SUCCESS);
}

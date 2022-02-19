#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

struct lista_vizinhos {
	int id;
	struct lista_vizinhos *proximo;
};
typedef struct lista_vizinhos lista_vizinhos_t;

struct no {
    int id;
	double pos_x, pos_y;
	lista_vizinhos_t *lista;
};
typedef struct no no_t;
typedef no_t *grafo_t;

bool lista_vizinhos_adicionar(int vizinho, lista_vizinhos_t **lista) {
    lista_vizinhos_t *novo = malloc(sizeof(lista_vizinhos_t));
	if (!novo) return false;
	novo->id = vizinho;
	novo->proximo = *lista;
	*lista = novo;
	return true;
}

void lista_vizinhos_imprimir(lista_vizinhos_t *lista) {
	while (lista) {
		printf("%d ", lista->id);
		lista = lista->proximo;
	}
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

void grafo_imprimir(int tam, grafo_t grafo) {
    for (int i = 0; i < tam; i++) {
		printf("NÓ %d: ", i);
		lista_vizinhos_imprimir(grafo[i].lista);
		printf("\n");
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
		}
		grafo_atualizar_vizinhos(tam, raio_comunicacao, grafo);
		grafo_imprimir(tam, grafo);
	}
	fclose(arquivo);
	exit(EXIT_SUCCESS);
}

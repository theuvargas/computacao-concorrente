#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// maximo de 10^8, com 10^9 o processo eh finalizado pelo sistema operacional
#define TAM 100000

void printaVetor(int* vet, int n) {
    for (int i = 0; i < n; i++) {
        printf("%d ", vet[i]);
    }
    printf("\n");
}

// para comparar o resultado com o merge
void insertionSort(int* vet, int n){
    int i, j, valor;
    for (i = 1; i < n; i++) {
        valor = vet[i];
        j = i - 1;
        while (j >= 0 && vet[j] > valor) {
            vet[j + 1] = vet[j];
            j = j - 1;
        }
        vet[j + 1] = valor;
    }
}

// considerando que os trechos de inicio ate meio e de meio+1 ate fim estao ordenados, ordena o vetor v
void merge(int* v, int* resultado, int inicio, int meio, int fim) {
    int indiceEsq = inicio;
    int indiceDir = meio+1;

    int fimEsq = meio;
    int fimDir = fim;

    int indice = 0;

    // int resultado[fim-inicio+1];

    // compara os valores mais a esquerda dos vetores e copia para resultado[] o menor numero
    while (indiceEsq <= fimEsq && indiceDir <= fimDir) {
        if (v[indiceEsq] <= v[indiceDir]) {
            resultado[indice++] = v[indiceEsq++];
        }
        else {
            resultado[indice++] = v[indiceDir++];
        }
    }
    
    // copia o resto do vetor que falta, esquerda ou direita
    if (indiceEsq > fimEsq) {
        for (int i = indiceDir; i <= fimDir; i++) {
            resultado[indice++] = v[i];
        }
    }
    else if (indiceDir > fimDir) {
        for (int i = indiceEsq; i <= fimEsq; i++) {
            resultado[indice++] = v[i];
        }
    }
    
    for (int i = 0; i < fim-inicio+1; i++) {
        v[i+inicio] = resultado[i];
    }
}

void mergesort(int* vet, int* res, int inicio, int fim) {
    if (inicio < fim) {
        int meio = (inicio+fim) / 2;

        // ordena as metades do vetor
        mergesort(vet, res, inicio, meio);
        mergesort(vet, res, meio+1, fim);

        // funde metades de forma ordenada
        merge(vet, res, inicio, meio, fim);
    }
}

int vetoresSaoIguais(int* v1, int* v2, int tam)  {
    for (int i = 0; i < tam; i++) {
        if (v1[i] != v2[i]) return 0;
    }
    return 1;
}

void copiaVetor(int* fonte, int* destino, int tam) {
    for (int i = 0; i < tam; i++) {
        destino[i] = fonte[i];
    }
}

void preencheVetor(int* vet, int tam) {
    for (int i = 0; i < tam; i++) {
        vet[i] = rand() % 1000;
    }
}

void teste(int* vetor, int* referencia) {
    insertionSort(referencia, TAM);

    if (!vetoresSaoIguais(vetor, referencia, TAM)) {
        printf("vetores sao diferentes\n");
        return;
    }

    printf("teste passou\n");
}

// compara a ordenacao por mergesort e por insertion sort
int main() {
    srand(time(NULL));

    int* vetor = malloc(TAM*sizeof(int));
    if (vetor == NULL) printf("erro malloc 1\n");

    preencheVetor(vetor, TAM);

    int* comparacao = malloc(TAM * sizeof(int));
    if (comparacao == NULL) printf("erro malloc 2\n");
    copiaVetor(vetor, comparacao, TAM);

    int* res = malloc(TAM * sizeof(int)); // vetor auxiliar que ajuda a otimizar o codigo
    if (res == NULL) printf("erro malloc 3\n");

    mergesort(vetor, res, 0, TAM-1);

    // teste(vetor, comparacao); teste desligado

    free(vetor);
    free(res);
    free(comparacao);

    return 0;
}
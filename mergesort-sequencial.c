#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

// maximo de 10^8, com 10^9 o processo eh finalizado pelo sistema operacional
#define TAM 1000000
#define LIMITE 30 // tamanho minimo do vetor para que o algoritmo use Insertion Sort

void printaVetor(int* vet, int n) {
    for (int i = 0; i < n; i++) {
        printf("%d ", vet[i]);
    }
    printf("\n");
}

void insertionSort(int* vet, int inicio, int fim){
    int i, j, valor;
    for (i = inicio+1; i < fim; i++) {
        valor = vet[i];
        j = i - 1;
        while (j >= inicio && vet[j] > valor) {
            vet[j + 1] = vet[j];
            j = j - 1;
        }
        vet[j + 1] = valor;
    }
}

// considerando que os trechos de inicio ate meio e de meio+1 ate fim estao ordenados, ordena o vetor v
void merge(int* v, int* resultado, int inicio, int meio, int fim) {
    int indiceEsq = inicio;
    int indiceDir = meio;

    int fimEsq = meio;
    int fimDir = fim;

    int indice = 0;

    // compara os valores mais a esquerda dos vetores e copia para resultado[] o menor numero
    while (indiceEsq < fimEsq && indiceDir < fimDir) {
        if (v[indiceEsq] < v[indiceDir]) {
            resultado[indice++] = v[indiceEsq++];
        }
        else {
            resultado[indice++] = v[indiceDir++];
        }
    }
    
    // copia o resto do vetor que falta, esquerda ou direita
    if (indiceEsq >= fimEsq) {
        for (int i = indiceDir; i < fimDir; i++) {
            resultado[indice++] = v[i];
        }
    }
    else if (indiceDir >= fimDir) {
        for (int i = indiceEsq; i < fimEsq; i++) {
            resultado[indice++] = v[i];
        }
    }
    
    for (int i = 0; i < fim-inicio; i++) {
        v[i+inicio] = resultado[i];
    }
}

// se o tamanho vetor for menor ou igual a LIMITE, utiliza InsertionSort; caso contrario, utiliza MergeSort
void mergesortMisto(int* vet, int* res, int inicio, int fim) {
    if (fim-inicio > LIMITE) {
        int meio = (inicio+fim) / 2;

        // ordena as metades do vetor
        mergesortMisto(vet, res, inicio, meio);
        mergesortMisto(vet, res, meio, fim);

        // funde metades de forma ordenada
        merge(vet, res, inicio, meio, fim);
    }

    else if (fim-inicio >= 1) insertionSort(vet, inicio, fim);
}

void mergesort(int* vet, int* res, int inicio, int fim) {
    if (fim-inicio != 1) {
        int meio = (inicio+fim) / 2;

        // ordena as metades do vetor
        mergesort(vet, res, inicio, meio);
        mergesort(vet, res, meio, fim);

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

double get_wall_time(){
    struct timeval time;
    if (gettimeofday(&time, NULL)){
        printf("Erro na funcao gettimeofday()");
        return 0;
    }
    return (double)time.tv_sec + (double)time.tv_usec * .000001;
}

void teste(int* v, int tam) {
    int maior = v[0];
    for (int i = 1; i < tam; i++) {
        if (v[i] >= maior) maior = v[i];
        else {
            printf("ordenacao incorreta\n");
            return;
        }
    }
    printf("teste passou\n");
}

int main() {
    double inicio, fim;
    srand(time(NULL));

    int* vetor = malloc(TAM*sizeof(int));
    if (vetor == NULL) {
        printf("erro malloc 1\n");
        exit(1);
    }

    preencheVetor(vetor, TAM);

    int* comparacao = malloc(TAM * sizeof(int));
    if (comparacao == NULL) {
        printf("erro malloc 2\n");
        exit(1);
    }
    copiaVetor(vetor, comparacao, TAM);

    int* res = malloc(TAM * sizeof(int)); // vetor auxiliar que ajuda a otimizar o codigo
    if (res == NULL) {
        printf("erro malloc 3\n");
        exit(1);
    }

    inicio = get_wall_time();
    mergesort(vetor, res, 0, TAM);
    fim = get_wall_time();

    printf("Merge sort:       %lf segundos\n", fim-inicio);

    inicio = get_wall_time();
    mergesortMisto(comparacao, res, 0, TAM);
    fim = get_wall_time();

    printf("Merge sort misto: %lf segundos\n", fim-inicio);

    if (!vetoresSaoIguais(vetor, comparacao, TAM)) printf("vetores sao diferentes\n");

    free(vetor);
    free(res);
    free(comparacao);

    return 0;
}
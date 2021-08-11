#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>

int* vetor;
int* vetorAuxiliar;
long long int tamanhoVetor;
int nthreads;

void printaVetor(int* vet, int inicio, int fim) {
    for (int i = inicio; i < fim; i++) {
        printf("%d ", vet[i]);
    }
    printf("\n");
}

// considerando que os trechos de inicio ate meio e de meio+1 ate fim estao ordenados, ordena o vetor v
void merge(int* v, int* resultad, int inicio, int meio, int fim) {
    int indiceEsq = inicio;
    int indiceDir = meio;

    int fimEsq = meio;
    int fimDir = fim;

    int indice = 0;

    int resultado[tamanhoVetor];

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

void preencheVetor(int* vet, int tam) {
    for (int i = 0; i < tam; i++) {
        vet[i] = rand() % 10;
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

void* mergesortConcorrente(void* arg) {
    long id = (long) arg;

    int tamanhoBloco = tamanhoVetor / nthreads;
    int inicio = id * tamanhoBloco;
    int fim;

    if (id == nthreads - 1) fim = tamanhoVetor;
    else fim = inicio + tamanhoBloco;

    mergesort(vetor, vetorAuxiliar, inicio, fim);

    pthread_exit(NULL);
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

void compara(int* ordenado, int* desordenado, int tam) {
    insertionSort(desordenado, 0, tam);

    for (int i = 0; i < tam; i++) {
        if(ordenado[i] != desordenado[i]) {
            printf("%d != %d, %d kkkkkkk\n", ordenado[i], desordenado[i], i);
            return;
        }
    }
    printf("correto\n");
}

void copiaVetor(int* fonte, int* destino, int tam) {
    for (int i = 0; i < tam; i++) {
        destino[i] = fonte[i];
    }
}

int main(int argc, char** argv) {
    srand(time(NULL));
    double inicio, fim;
    
    if (argc < 3) {
        fprintf(stderr, "Digite %s <tamanho do vetor> <numero de threads>\n", argv[0]);
        exit(1);
    }

    tamanhoVetor = atoll(argv[1]);
    nthreads = atoi(argv[2]);
    
    vetor = malloc(tamanhoVetor * sizeof(int));
    vetorAuxiliar = malloc(tamanhoVetor * sizeof(int));

    preencheVetor(vetor, tamanhoVetor);

    int desordenado[tamanhoVetor];
    copiaVetor(vetor, desordenado, tamanhoVetor);
    //printaVetor(vetor, 0, tamanhoVetor);

    pthread_t* tid = malloc(nthreads * sizeof(pthread_t));
    
    inicio = get_wall_time();
    for (long int i = 0; i < nthreads; i++) {
        if (pthread_create((tid+i), NULL, mergesortConcorrente, (void*) i)) {
            fprintf(stderr, "Erro pthread_create\n");
            exit(2);
        }
    }
    
    for (int i = 0; i < nthreads; i++) {
        if (pthread_join(*(tid+i), NULL)) {
            fprintf(stderr, "Erro pthread_join\n");
            exit(3);
        }
    }

    int tamBloco = tamanhoVetor/nthreads;
    int i;
    for (i = 0; i < nthreads-2; i++) {
        merge(vetor, vetorAuxiliar, 0, (i+1)*tamBloco, (i+1)*tamBloco + tamBloco);
    }

    merge(vetor, vetorAuxiliar, 0, (i+1)*tamBloco, tamanhoVetor);
    fim = get_wall_time();

    teste(vetor, tamanhoVetor);

    printf("Tempo: %lf segundos\n", fim-inicio);

    free(vetor);
    free(vetorAuxiliar);
    free(tid);
}
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>
#include <string.h>

long long int* vetor;
long long int* vetorAuxiliar;
long long int tamanhoVetor;
int nthreads;

void printaVetor(int* vet, int inicio, int fim) {
    for (int i = inicio; i < fim; i++) {
        printf("%d ", vet[i]);
    }
    printf("\n");
}

// considerando que os trechos de inicio ate meio e de meio+1 ate fim estao ordenados, ordena o vetor v
void merge(long long int* v, long long int* vAuxiliar, long long inicio, long long int meio, long long int fim) {
    long long int indiceEsq = inicio;
    long long int indiceDir = meio;

    long long int fimEsq = meio;
    long long int fimDir = fim;

    long long int indice = inicio;

    // compara os valores mais a esquerda dos vetores e copia para vAuxiliar[] o menor numero
    while (indiceEsq < fimEsq && indiceDir < fimDir) {
        if (v[indiceEsq] < v[indiceDir]) {
            vAuxiliar[indice++] = v[indiceEsq++];
        }
        else {
            vAuxiliar[indice++] = v[indiceDir++];
        }
    }
    
    // copia o resto do vetor que falta, esquerda ou direita
    if (indiceEsq >= fimEsq) {
        for (int i = indiceDir; i < fimDir; i++) {
            vAuxiliar[indice++] = v[i];
        }
    }
    else if (indiceDir >= fimDir) {
        for (int i = indiceEsq; i < fimEsq; i++) {
            vAuxiliar[indice++] = v[i];
        }
    }
    
    for (long long int i = 0; i < fim-inicio; i++) {
        v[i+inicio] = vAuxiliar[inicio+i];
    }
}

void mergesort(long long int* vet, long long int* aux, long long int inicio, long long int fim) {
    if (fim-inicio != 1) {
        long long int meio = fim + (inicio-fim)/2;

        // ordena as metades do vetor
        mergesort(vet, aux, inicio, meio);
        mergesort(vet, aux, meio, fim);

        // funde metades de forma ordenada
        merge(vet, aux, inicio, meio, fim);
    }
}

void preencheVetor(long long int* vet, int tam) {
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

void teste(long long int* v, int tam) {
    long long int maior = v[0];
    for (long long int i = 1; i < tam; i++) {
        if (v[i] >= maior) maior = v[i];
        else {
            printf("ordenacao incorreta\n");
            return;
        }
    }
    printf("teste passou\n");
}

void* processaMergesortConcorrente(void* arg) {
    long id = (long) arg;

    long long int tamanhoBloco = tamanhoVetor / nthreads;
    long long int inicio = id * tamanhoBloco;
    long long int fim;

    if (id == nthreads - 1) fim = tamanhoVetor;
    else fim = inicio + tamanhoBloco;

    mergesort(vetor, vetorAuxiliar, inicio, fim);

    pthread_exit(NULL);
}

void mergesortConcorrente(long long int* vetor, int tamanho, int nthreads) {
    if (nthreads == 1) {
        mergesort(vetor, vetorAuxiliar, 0, tamanhoVetor);
    }

    else {    
        pthread_t* tid = malloc(nthreads * sizeof(pthread_t));
        
        if (tid == NULL) {
            fprintf(stderr, "Erro malloc\n");
            exit(2);
        }

        for (long int i = 0; i < nthreads; i++) {
            if (pthread_create((tid+i), NULL, processaMergesortConcorrente, (void*) i)) {
                fprintf(stderr, "Erro pthread_create\n");
                exit(3);
            }
        }
        
        for (int i = 0; i < nthreads; i++) {
            if (pthread_join(*(tid+i), NULL)) {
                fprintf(stderr, "Erro pthread_join\n");
                exit(4);
            }
        }

        long long int tamBloco = tamanhoVetor/nthreads;
        long long int i;
        for (i = 0; i < nthreads-2; i++) {
            merge(vetor, vetorAuxiliar, 0, (i+1)*tamBloco, (i+1)*tamBloco + tamBloco);
        }

        merge(vetor, vetorAuxiliar, 0, (i+1)*tamBloco, tamanhoVetor);
        
        free(tid);
    }
}

void testes(long long int* vetor, int tamanho, int nthreads) {
    printf("Caso de teste: vetor ordenado\n");

    for (size_t i = 0; i < tamanho; i++) {
        vetor[i] = i;
    }

    mergesortConcorrente(vetor, tamanho, nthreads);

    teste(vetor, tamanho);


    printf("Caso de teste: vetor decrescente\n");

    for (size_t i = 0; i < tamanho; i++) {
        vetor[i] = tamanho-i;
    }

    mergesortConcorrente(vetor, tamanho, nthreads);

    teste(vetor, tamanho);

    printf("Caso de teste: vetor aleatorio\n");

    for (size_t i = 0; i < tamanho; i++) {
        vetor[i] = rand() % 1000;
    }

    mergesortConcorrente(vetor, tamanho, nthreads);

    teste(vetor, tamanho);
}

int main(int argc, char** argv) {
    // srand(time(NULL));
    double inicio, fim;

    if (argc < 3) {
        fprintf(stderr, "Digite %s <tamanho do vetor> <numero de threads>\n", argv[0]);
        exit(1);
    }

    tamanhoVetor = atoll(argv[1]);
    nthreads = atoi(argv[2]);
    
    vetor = malloc(tamanhoVetor * sizeof(long long int));
    vetorAuxiliar = malloc(tamanhoVetor * sizeof(long long int));

    if (vetor == NULL || vetorAuxiliar == NULL) {
        fprintf(stderr, "Erro malloc\n");
        exit(2);
    }

    preencheVetor(vetor, tamanhoVetor);

    if (argc == 4) {
        if (strcmp(argv[3], "teste") == 0) {
            testes(vetor, tamanhoVetor, nthreads);
        }
        else printf("quarto argumento invalido\n");
    }

    else {
        inicio = get_wall_time();
        mergesortConcorrente(vetor, tamanhoVetor, nthreads);
        fim = get_wall_time();
        printf("%lf\n", fim-inicio);
    }

    
    free(vetor);
    free(vetorAuxiliar);

    return 0;
}
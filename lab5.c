#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

pthread_cond_t cond;
pthread_mutex_t lock;

int* vetor;
int nthreads;
int desbloqueadas;

void barreira(int nthreads) {
    pthread_mutex_lock(&lock);
    if (desbloqueadas == 1) { 
        // ultima thread a chegar na barreira
        printf("\n");
        pthread_cond_broadcast(&cond);
        desbloqueadas = nthreads;
    } else {
        desbloqueadas--;
        pthread_cond_wait(&cond, &lock);
    }
    pthread_mutex_unlock(&lock);
}

void* tarefa(void* arg) {
    long id = (long) arg;
    int* somatorio = malloc(sizeof(int));
    *somatorio = 0;

    size_t tamanho = nthreads;

    for (size_t i = 0; i < tamanho; i++) {
        for (size_t j = 0; j < tamanho; j++) {
            *somatorio += vetor[j];
        }
        printf("thread %ld terminou de somar: %d\n", id, *somatorio);
        barreira(nthreads);
        vetor[id] = rand() % 10;
        printf("thread %ld gerou o numero %d\n", id, vetor[id]);
        barreira(nthreads);
    }

    pthread_exit(somatorio);
}

void printaVetor(int* vetor, int tamanho) {
    for (size_t i = 0; i < tamanho; i++) {
        printf("%d ", vetor[i]);
    }
    printf("\n");
}

void teste(int* vetor, int tamanho) {
    int primeiro = vetor[0];
    for (size_t i = 1; i < tamanho; i++) {
        if (vetor[i] != primeiro) {
            printf("Teste falhou\n");
            return;
        }
    }
    printf("Teste passou, todos os numeros sao iguais\n");
}

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Digite: %s <numero de threads>\n", argv[0]);
        return 1;
    }
    pthread_cond_init(&cond, NULL);
    pthread_mutex_init(&lock, NULL);
    
    srand(time(NULL));

    nthreads = atoi(argv[1]);
    desbloqueadas = nthreads;
    pthread_t tid[nthreads];

    vetor = malloc(sizeof(int) * nthreads);
    if (vetor == NULL) {
        fprintf(stderr, "Erro malloc()\n");
        exit(2);
    }

    for (size_t i = 0; i < nthreads; i++) {
        vetor[i] = rand() % 10;
    }

    printf("Vetor inicial: ");
    printaVetor(vetor, nthreads);
    printf("\n");

    for (long i = 0; i < nthreads; i++) {
        if (pthread_create(&tid[i], NULL, tarefa, (void*) i)) {
            fprintf(stderr, "Erro pthread_create()\n");
            exit(3);
        }
    }

    int retornoVetor[nthreads];
    for (size_t i = 0; i < nthreads; i++) {
        int* retornoValor;
        if (pthread_join(tid[i], (void**) &retornoValor)) {
            fprintf(stderr, "Erro pthread_create()\n");
            exit(3);
        }
        retornoVetor[i] = *retornoValor;
    }

    printf("\nSomatorios das threads: ");
    printaVetor(retornoVetor, nthreads);
    printf("\n");

    teste(retornoVetor, nthreads);

    free(vetor);

    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&lock);

    return 0;
}
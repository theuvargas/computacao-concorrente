#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NTHREADS 2
#define TAM 10000

int vetor[TAM];

void* tarefa (void* arg) {
    int numThread = *(int*) arg;
    
    for (int i = 0; i < TAM; i++) {
        if (i % NTHREADS == numThread) {
            vetor[i] = vetor[i] * vetor[i];   
        }       
    }

    pthread_exit(NULL);
}

void teste(void) {
    for (int i = 0; i < TAM; i++) {
        if (vetor[i] != i * i) {
            printf("Erro na posicao %d: \'%d\' esperado, \'%d\' na posicao\n", i, i*i, vetor[i]);
            exit(-1);
        }
    }

    printf("Teste passou!\n");
}

int main(void) {
    pthread_t tid[NTHREADS];
    pthread_t threads[NTHREADS];

    for (int i = 0; i < NTHREADS; i++) {
        threads[i] = i;
    }
    
    for (int i = 0; i < TAM; i++) {
        vetor[i] = i;
    }
    
    for (int i = 0; i < NTHREADS; i++) {
        if (pthread_create(&tid[i], NULL, tarefa, (void*) &threads[i])) {
            printf("erro na funcao pthread_create()\n");
            exit(-1);
        }
    }
    
    for (int i = 0; i < NTHREADS; i++) {
        if (pthread_join(tid[i], NULL)) {
            printf("erro na funcao pthread_join()\n");
            exit(-1);
        }
    }

    teste();

    return 0;
}

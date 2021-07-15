#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NTHREADS 2
#define TAM 10000

int vetor[TAM];

void* tarefa (void* arg) {
    int numThread = *(int*) arg;
    
    if (numThread == 1) {
        for (int i = 0; i < TAM/2; i++) {
            vetor[i] = vetor[i] * vetor[i];
        }
    }

    else if (numThread == 2) {
        for (int i = TAM/2; i < TAM; i++) {
            vetor[i] = vetor[i] * vetor[i];
        }
    }

    pthread_exit(NULL);
}

int main(void) {
    pthread_t tid[NTHREADS];
    pthread_t threads[] = {1, 2};

    for (int i = 0; i < TAM; i++) {
        vetor[i] = i+1;
    }
    
    printf("ANTES:  posicao 9999 no vetor -> %d\n", vetor[TAM-2]);

    for (int i = 0; i < NTHREADS; i++) {
        if (pthread_create(&tid[i], NULL, tarefa, (void*) &threads[i])) {
            printf("erro na funcao pthread_create()");
            exit(-1);
        }
    }
    
    for (int i = 0; i < NTHREADS; i++) {
        if (pthread_join(tid[i], NULL)) {
            printf("erro na funcao pthread_join()");
            exit(-1);
        }
    }

    printf("DEPOIS: posicao 9999 no vetor -> %d\n", vetor[TAM-2]);

    return 0;
}
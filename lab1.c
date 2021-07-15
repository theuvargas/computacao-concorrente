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

int main(void) {
    pthread_t tid[NTHREADS];
    pthread_t threads[NTHREADS];

    for (int i = 0; i < NTHREADS; i++) {
        threads[i] = i;
    }
    

    for (int i = 0; i < TAM; i++) {
        vetor[i] = i;
    }
    
    printf("ANTES:  posicao 9999 no vetor -> %d\n", vetor[TAM-1]);

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

    printf("DEPOIS: posicao 9999 no vetor -> %d\n", vetor[TAM-1]);

    return 0;
}

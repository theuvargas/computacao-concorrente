#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>

#define NTHREADS 4

sem_t semaforo1, semaforo2, semaforo3;

void* thread1(void* arg) {
    sem_wait(&semaforo1);
    printf("Fique a vontade.\n");
    
    sem_post(&semaforo1);
    sem_post(&semaforo2);

    pthread_exit(NULL);
}

void* thread2(void* arg) {    
    printf("Seja bem vindo!\n");
    sem_post(&semaforo1);

    pthread_exit(NULL);
}

void* thread3(void* arg) {
    sem_wait(&semaforo2);
    sem_wait(&semaforo3);
    printf("Volte sempre!\n");
    
    pthread_exit(NULL);
}

void* thread4(void* arg) {
    sem_wait(&semaforo1);
    printf("Sente-se por favor.\n");
    
    sem_post(&semaforo1);
    sem_post(&semaforo3);

    pthread_exit(NULL);
}

int main() {
    sem_init(&semaforo1, 0, 0);
    sem_init(&semaforo2, 0, 0);
    sem_init(&semaforo3, 0, 0);

    pthread_t tid[NTHREADS];

    if (pthread_create(&tid[0], NULL, thread1, NULL) ||
        pthread_create(&tid[1], NULL, thread2, NULL) ||
        pthread_create(&tid[2], NULL, thread3, NULL) ||
        pthread_create(&tid[3], NULL, thread4, NULL)
    ) {
        fprintf(stderr, "Erro pthread_create()\n");
        exit(1);
    }

    for (int i = 0; i < NTHREADS; i++) {
        pthread_join(tid[i], NULL);
    }    

    return 0;
}
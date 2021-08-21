#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define NTHREADS 4

pthread_mutex_t lock;
pthread_cond_t cond;

// os numeros abaixo representam o numero da thread
bool imprimiu1 = false;
bool imprimiu2 = false;
bool imprimiu3 = false;
bool imprimiu4 = false;

void* thread1(void* arg) {
    pthread_mutex_lock(&lock);
    if (!imprimiu2) { // nao precisa de while
        pthread_cond_wait(&cond, &lock);
    }
    
    printf("Fique a vontade.\n");
    imprimiu1 = true;
    pthread_cond_signal(&cond);

    pthread_mutex_unlock(&lock);
    pthread_exit(NULL);
}

void* thread2(void* arg) {
    pthread_mutex_lock(&lock);
    
    printf("Seja bem vindo!\n");
    imprimiu2 = true;
    pthread_cond_broadcast(&cond); // caso seja a ultima thread a executar, sera necessario acordar todas as outras

    pthread_mutex_unlock(&lock);
    pthread_exit(NULL);
}

void* thread3(void* arg) {
    pthread_mutex_lock(&lock);
    while (!imprimiu1 || !imprimiu4) { // precisa de while pois outra thread pode acorda-la antes das threads 1 e 4 terem imprimido
        pthread_cond_wait(&cond, &lock);
    }

    printf("Volte sempre!\n");
    imprimiu3 = true; // linha desnecessaria

    pthread_mutex_unlock(&lock);
    pthread_exit(NULL);
}

void* thread4(void* arg) {
    pthread_mutex_lock(&lock);
    if (!imprimiu2) {
        pthread_cond_wait(&cond, &lock);
    }

    printf("Sente-se por favor.\n");
    imprimiu4 = true;
    pthread_cond_signal(&cond);

    pthread_mutex_unlock(&lock);
    pthread_exit(NULL);
}

int main() {
    pthread_mutex_init(&lock, NULL);
    pthread_cond_init(&cond, NULL);

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

    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&cond);
    
    return 0;
}
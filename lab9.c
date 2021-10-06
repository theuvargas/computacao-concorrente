#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define TAM_BUF 5
#define NCONSUMIDORES 2
#define NPRODUTORES 3

int buffer[TAM_BUF];
int in = 0; // posicao em que um item produzido deve ser inserido no buffer
int itensProduzidos = 0;

sem_t mutex, mutex2;
sem_t podeProduzir, podeConsumir;

// coloca um numero do intervalo [0, 9] no buffer
void* produz(void* arg) {
    int id = *(int*) arg;

    while (1) {
        sem_wait(&mutex);
        sem_wait(&podeProduzir);

        int r = rand() % 10;
        buffer[in] = r;
        printf("O produtor %d gerou o numero %d na posicao %d\n", id, r, in);
        in = (in+1) % TAM_BUF;
        itensProduzidos++;

        while (itensProduzidos == TAM_BUF) { // se o buffer esta cheio, libera o consumidor
            sem_post(&podeConsumir);
            sem_wait(&podeProduzir);
        }

        sem_post(&podeProduzir);
        sem_post(&mutex);
    }

    pthread_exit(NULL);
}

// quando o buffer esta cheio, printa a soma dos elementos do buffer
void* consome(void* arg) {
    int id = *(int*) arg;

    while (1) {
        sem_wait(&mutex2);
        sem_wait(&podeConsumir);

        while (itensProduzidos < TAM_BUF) {
            sem_wait(&podeConsumir);
        }

        int soma = 0;
        for (int i = 0; i < TAM_BUF; i++) {
            soma += buffer[i];
        }

        printf("O consumidor %d calculou a soma: %d\n", id, soma);
        sleep(2);

        itensProduzidos = 0;

        sem_post(&podeProduzir);
        sem_post(&mutex2);
    }

    pthread_exit(NULL);
}

int main() {
    pthread_t tid[NPRODUTORES+NCONSUMIDORES];
    int produtores[NPRODUTORES];
    int consumidores[NCONSUMIDORES];

    sem_init(&mutex, 0, 1);
    sem_init(&mutex2, 0, 1);
    sem_init(&podeConsumir, 0, 0);
    sem_init(&podeProduzir, 0, NPRODUTORES);

    for (int i = 0; i < NPRODUTORES; i++) {
        produtores[i] = i;
        if (pthread_create(&tid[i], NULL, produz, &produtores[i])) {
            printf("erro create\n");
            exit(1);
        }
    }
    
    for (int i = 0; i < NCONSUMIDORES; i++) {
        consumidores[i] = i;
        if (pthread_create(&tid[NPRODUTORES+i], NULL, consome, &consumidores[i])) {
            printf("erro create\n");
            exit(1);
        }
    }

    for (int i = 0; i < NPRODUTORES+NCONSUMIDORES; i++) {
        pthread_join(tid[i], NULL);
    }

    return 0;
}
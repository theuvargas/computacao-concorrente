#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

#define VALOR 1 // valor que preenche as matrizes

int* m1;
int* m2;
int* m3;

int dimensao;
int nthreads;

// funcao copiada do StackOverflow: https://stackoverflow.com/a/17440673
double get_wall_time(){
    struct timeval time;
    if (gettimeofday(&time, NULL)){
        printf("Erro na funcao gettimeofday()");
        return 0;
    }
    return (double)time.tv_sec + (double)time.tv_usec * .000001;
}

void preencheMatrizes(void) {
    for (size_t i = 0; i < dimensao; i++) {
        for (size_t j = 0; j < dimensao; j++) {
            m1[i*dimensao+j] = VALOR;
            m2[i*dimensao+j] = VALOR;
            m3[i*dimensao+j] = 0;
        }
    }
}

void multiplicaMatrizesSequencial(int* m1, int* m2) {
    for (size_t i = 0; i < dimensao; i++) {
        for (size_t j = 0; j < dimensao; j++) {
            for (size_t k = 0; k < dimensao; k++) {
                m3[i*dimensao+j] += m1[i*dimensao+k] * m2[k*dimensao+j];
            }
        }
    }
}

// funcao executada pelas threads
void* multiplicaMatrizesParalelo(void* arg) {
    int numThread = *(int*) arg;

    for (size_t i = numThread; i < dimensao; i += nthreads) {
        for (size_t j = 0; j < dimensao; j++) {
            for (size_t k = 0; k < dimensao; k++) {
                m3[i*dimensao+j] += m1[i*dimensao+k] * m2[k*dimensao+j];
            }            
        }
    }

    pthread_exit(NULL);
}

void teste(void) {
    for (size_t i = 0; i < dimensao; i++) {
        for (size_t j = 0; j < dimensao; j++) {
            if (m3[i*dimensao+j] != dimensao) {
                printf("Erro na linha %lu coluna %lu da matriz", i, j);
                exit(3);
            }
        }
    }
    // printf("Teste passou!\n");
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Erro - Digite: %s <dimensao da matriz> <numero de threads>\n", argv[0]);
        return 1;
    }

    dimensao = atoi(argv[1]);
    nthreads = atoi(argv[2]);

    pthread_t tid[nthreads];
    pthread_t threads[nthreads];
    double inicio, fim;

    inicio = get_wall_time();

    m1 = malloc(dimensao*dimensao*sizeof(int));
    m2 = malloc(dimensao*dimensao*sizeof(int));
    m3 = malloc(dimensao*dimensao*sizeof(int));

    if (m1 == NULL || m2 == NULL || m3 == NULL) {
        printf("Erro na funcao malloc()\n");
        return 2;
    }

    preencheMatrizes();

    fim = get_wall_time();
    double deltaInit = fim - inicio;

    inicio = get_wall_time();
    
    if (nthreads == 1) {
        multiplicaMatrizesSequencial(m1, m2);
    }
    else {
        for (size_t i = 0; i < nthreads; i++) {
            threads[i] = i;
            pthread_create(&tid[i], NULL, multiplicaMatrizesParalelo, (void*) &threads[i]);
        }

        for (size_t i = 0; i < nthreads; i++){
            pthread_join(tid[i], NULL);
        }
    }
    
    fim = get_wall_time();
    double deltaProcessamento = fim - inicio;

    teste(); // o tempo de teste nao esta sendo contabilizado

    inicio = get_wall_time();

    free(m1);
    free(m2);
    free(m3);

    fim = get_wall_time();
    double deltaFim = fim - inicio;

    // printf("Tempo inicializacao: %lf s\n", deltaInit);
    // printf("Tempo processamento: %lf s\n", deltaProcessamento);
    // printf("Tempo finalizacao:   %lf s\n", deltaFim);

    printf("Total: %lf segundos\n", deltaInit+deltaProcessamento+deltaFim);

    return 0;
}

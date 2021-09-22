#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

long k = 1000000000;
int nthreads = 6;

// retorna a serie de pi de 0 ate k inclusive, ou seja, com k+1 termos
double seriePi() {
    double soma = 0;
    for (int n = 0; n <= k; n++) {
        double termo = 1 / (2 * (double)n + 1);

        // termo par eh positivo
        if (n % 2 == 0) soma += termo;
        // termo impar eh negativo
        else soma -= termo;
    }
    return 4*soma;
}

void* tarefa(void* arg) {
    long id = (long) arg;

    double* soma = malloc(sizeof(double));

    for (int i = id; i <= k; i += nthreads) {
        double n = (double) i;

        double termo = 1 / (2*n+1);

        // termo par eh positivo
        if (i % 2 == 0) *soma += termo;
        // termo impar eh negativo
        else *soma -= termo;

    }
    
    pthread_exit(soma);
}

double seriePiConcorrente() {
    pthread_t tid[nthreads];

    for (long i = 0; i < nthreads; i++) {
        if (pthread_create(&tid[i], NULL, tarefa, (void*) i)) {
            printf("erro pthread_create()\n");
            exit(1);
        }
    }

    double soma = 0;
    for (int i = 0; i < nthreads; i++) {
        double* retorno = malloc(sizeof(double));
        if (pthread_join(tid[i], (void*) &retorno)) {
            printf("erro pthread_join()\n");
            exit(1);
        }
        soma += *retorno;
        free(retorno);
    }

    return 4*soma;   
}

double get_wall_time(){
    struct timeval time;
    if (gettimeofday(&time, NULL)){
        printf("Erro na funcao gettimeofday()");
        return 0;
    }
    return (double)time.tv_sec + (double)time.tv_usec * .000001;
}

int main() {
    double inicio, fim;
    
    inicio = get_wall_time();
    double concorrente = seriePiConcorrente();
    fim = get_wall_time();

    printf("%.15lf -> %lf segundos\n", concorrente, fim-inicio);

    inicio = get_wall_time();
    double sequencial = seriePi();
    fim = get_wall_time();

    printf("%.15lf -> %lf segundos\n", sequencial, fim-inicio);

    return 0;
}
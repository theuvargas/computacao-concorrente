#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#define EXECUCOES 10

float* vetor;
long long int N;
int nthreads;

double get_wall_time(){
    struct timeval time;
    if (gettimeofday(&time, NULL)){
        printf("Erro na funcao gettimeofday()");
        return 0;
    }
    return (double)time.tv_sec + (double)time.tv_usec * .000001;
}

// retorna um vetor com floats aleatorios de tamanho tam
float* criaVetor(long long int tam) {
    float* vet = malloc(sizeof(float) * tam);

    if (vet == NULL) {
        fprintf(stderr, "Erro malloc\n"); exit(2);
    }

    for (size_t i = 0; i < tam; i++) {
        vet[i] = (float) rand() / ((float) rand() + 0.001); // 0.0001 para evitar divisao por zero
    }
    return vet;
}

// retorna um vetor com o menor e maior numeros de vet no formato [menor, maior]
float* extremosVetor(float* vet, long long int tam) {
    float maior = vet[0];
    float menor = vet[0];
    for (size_t i = 1; i < tam; i++) {
        if (vet[i] < menor) {
            menor = vet[i];
        }
        if (vet[i] > maior) {
            maior = vet[i];
        }
    }
    
    float* extremos = malloc(2*sizeof(float));

    extremos[0] = menor;
    extremos[1] = maior;

    return extremos;
}

void printaVetor(float* vet, int tam) {
    for (size_t i = 0; i < tam; i++) {
        printf("%f ", vet[i]);
    }
    printf("\n");
}

void* extremosVetorConcorrente(void* arg) {
    long int id = (long int) arg;

    float menor = vetor[id];
    float maior = vetor[id];
    for (long int i = id; i < N; i+=nthreads) {
        if (vetor[i] < menor) {
            menor = vetor[i];
        }
        if (vetor[i] > maior) {
            maior = vetor[i];
        }
    }

    float* extremos = malloc(2*sizeof(float));
    extremos[0] = menor;
    extremos[1] = maior;

    pthread_exit((void*) extremos);
}

void teste(float* resultadoConc) {
    float* resultadoSeq = extremosVetor(vetor, N);

    if (resultadoConc[0] != resultadoSeq[0] || resultadoConc[1] != resultadoSeq[1]) {
        printf("%f != %f ou %f != %f\n", resultadoConc[0], resultadoSeq[0], resultadoConc[1], resultadoSeq[1]);
        fprintf(stderr, "O teste falhou\n"); exit(5);
    }
    // else printf("%f == %f ou %f == %f\n", resultadoConc[0], resultadoSeq[0], resultadoConc[1], resultadoSeq[1]);

    free(resultadoSeq);
}

double mediaVetor(double* vet, int tam) {
    double soma = 0;
    for (size_t i = 0; i < tam; i++) {
        soma += vet[i];
    }

    return soma / tam;
}

int main(int argc, char* argv[]) {
    srand(time(NULL)); // define a seed dos numeros aleatorios
    
    if (argc < 3) {
        printf("Digite: %s <tamanho do vetor> <numero de threads>\n", argv[0]);
        exit(1);
    }
    
    N = atoll(argv[1]);
    nthreads = atoi(argv[2]);

    vetor = criaVetor(N);
    
    double tempos[EXECUCOES];
    for (size_t i = 0; i < EXECUCOES; i++) {
        double inicio, fim;

        // inicio do tempo concorrente
        inicio = get_wall_time();

        float* extremos;
        if (nthreads == 1) {
            extremos = extremosVetor(vetor, N);
        }
        
        else {
            pthread_t *tid = malloc(nthreads * sizeof(pthread_t));
            float** retorno = malloc(nthreads*sizeof(float*));

            for (long int i = 0; i < nthreads; i++) {
                if (pthread_create((tid+i), NULL, extremosVetorConcorrente, (void*) i)) {
                    fprintf(stderr, "Erro pthread_create\n"); exit(3);
                }
            }

            float* retornoPlanificado = malloc(2*nthreads*sizeof(float)); // planificado no sentido de que [[x, y], [w, z]] vira [x, y, w, z]
            int j = 0;
            for (long int i = 0; i < nthreads; i++) {
                if (pthread_join(*(tid+i), (void**) retorno+i)) {
                    fprintf(stderr, "Erro join\n"); exit(4);
                }
                retornoPlanificado[j++] = retorno[i][0];
                retornoPlanificado[j++] = retorno[i][1];
            }

            extremos = extremosVetor(retornoPlanificado, 2*nthreads);

            for (size_t i = 0; i < nthreads; i++) {
                free(retorno[i]);
            }
            free(retorno);
            free(tid);
            free(retornoPlanificado);

            // printf("(%f, %f)\n", extremos[0], extremos[1]);
        }
        fim = get_wall_time();
        tempos[i] = fim - inicio;
        free(extremos);
    }

    // teste(extremos);

    free(vetor);
    
    double media = mediaVetor(tempos, EXECUCOES);

    printf("Tempo medio em %d execucoes:  %lf segundos\n", EXECUCOES, media);

    return 0;
}
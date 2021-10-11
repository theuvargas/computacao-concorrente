#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <stdbool.h>

#define TAMBUF 10

int** buffer;
int contador = 0, in = 0, out = 0;

int numElementos = -1;
int numLinhas = -1;
int tamBloco;

int elementosEscritos = 0;
bool arquivoCriado = false;

bool escrevendo = false;

char* resultado;

pthread_cond_t cond_cons, cond_prod, cond_escr;
pthread_mutex_t mutex, mutex2;

void criaArquivo(char* str) {
    FILE* fp = fopen("saida.txt", "w");
       
    if (fp == NULL) {
        fprintf(stderr, "Erro fopen\n");
        exit(5);
    }

    fputs(str, fp);

    fclose(fp);
}

void insere(int* vetor) {
    pthread_mutex_lock(&mutex);
    
    while (contador == TAMBUF) {
        pthread_cond_wait(&cond_prod, &mutex);
    }
    
    buffer[in] = vetor;
    in = (in+1) % TAMBUF;
    contador++;

    pthread_mutex_unlock(&mutex);
    pthread_cond_signal(&cond_cons);
}

int* stringParaVetorInt(char* str) {
    char* num;
    int* vetor = malloc(sizeof(int) * tamBloco);

    int i = 0;
    while((num = strsep(&str, " "))) {
        if (strcmp("\n", num) == 0) 
            break;
        
        vetor[i++] = atoi(num);
    }

    return vetor;
}

char* vetorParaString(int* vetor, int n) {
    char* linha = malloc(10000);
    for (int i = 0; i < n; i++) {
        sprintf(linha + strlen(linha), "%d ", vetor[i]);
    }
    return linha;
}

int* retira(long id) {
    pthread_mutex_lock(&mutex);

    int* vetor = NULL;
    while (contador == 0) {
        if (elementosEscritos >= numElementos) {
            break;
        }
        pthread_cond_wait(&cond_cons, &mutex);
    }

    if (elementosEscritos < numElementos) {
        contador--;
        vetor = buffer[out];

        out = (out+1) % TAMBUF;
    }

    pthread_mutex_unlock(&mutex);
    pthread_cond_signal(&cond_prod);

    return vetor;
}

void* produtor(void* arg) {
    int tamanhoLinha = 100000;
    char* linha = malloc(sizeof(char) * tamanhoLinha);
    FILE *fp = fopen("entrada.txt", "r");
    fgets(linha, tamanhoLinha, fp);
    numElementos = atoi(linha);

    numLinhas = numElementos/tamBloco;
    
    while (fgets(linha, tamanhoLinha, fp)) {
        insere(stringParaVetorInt(linha));
    }

    free(linha);
    fclose(fp);

    pthread_exit(NULL);
}

void insertionSort(int* vet, int tam){
    int i, j, valor;
    for (i = 1; i < tam; i++) {
        valor = vet[i];
        j = i - 1;
        while (j >= 0 && vet[j] > valor) {
            vet[j + 1] = vet[j];
            j = j - 1;
        }
        vet[j + 1] = valor;
    }
}

void entraEscritor(long id) {
    pthread_mutex_lock(&mutex);
    while (escrevendo) {
        pthread_cond_wait(&cond_escr, &mutex);
    }
    escrevendo = true;

    pthread_mutex_unlock(&mutex);
}

void saiEscritor() {
    pthread_mutex_lock(&mutex);
    escrevendo = false;
    pthread_cond_signal(&cond_escr);
    pthread_mutex_unlock(&mutex);
}

void* consumidorEscritor(void* arg) {
    long id = (long) arg;

    pthread_mutex_lock(&mutex);

    while (numElementos == -1) {
        pthread_cond_wait(&cond_cons, &mutex);
    }

    pthread_mutex_unlock(&mutex);

    while (elementosEscritos < numElementos) {
        int* vetor = retira(id);
        
        if (vetor == NULL) break;

        insertionSort(vetor, tamBloco);

        entraEscritor(id);        
        char* linha = vetorParaString(vetor, tamBloco);
        sprintf(resultado+strlen(resultado), "%s\n", linha);
        elementosEscritos += tamBloco;
        saiEscritor();

        free(vetor);
    }

    pthread_mutex_lock(&mutex);
    if (!arquivoCriado) {
        criaArquivo(resultado);
        arquivoCriado = true;
        free(resultado);
    }
    pthread_mutex_unlock(&mutex);

    pthread_cond_broadcast(&cond_cons);

    pthread_exit(NULL);
}

double get_wall_time(){
    struct timeval time;
    if (gettimeofday(&time, NULL)){
        printf("Erro na funcao gettimeofday()");
        return 0;
    }
    return (double)time.tv_sec + (double)time.tv_usec * .000001;
}

int main(int argc, char** argv) {
    double inicio = get_wall_time();

    buffer = malloc(sizeof(int*) * TAMBUF);
    resultado = malloc(10000000);

    if (buffer == NULL || resultado == NULL) {
        printf("erro malloc\n");
        exit(1);
    }

    if (argc < 3) {
        printf("Erro - Digite: %s <tamanho do bloco> <numero de threads>\n", argv[0]);
        return 1;
    }

    tamBloco = atoi(argv[1]);
    int nthreads = atoi(argv[2]);

    for (int i = 0; i < TAMBUF; i++) {
        buffer[i] = malloc(sizeof(int) * tamBloco);

        if (buffer[i] == NULL) {
            printf("erro malloc\n");
            exit(2);
        }
        
    }

    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&mutex2, NULL);

    pthread_cond_init(&cond_cons, NULL);
    pthread_cond_init(&cond_prod, NULL);
    pthread_cond_init(&cond_escr, NULL);

    pthread_t tidProdutor;
    if (pthread_create(&tidProdutor, NULL, produtor, NULL)) {
        fprintf(stderr, "Erro pthread_create()\n");
        exit(3);
    }

    pthread_t tidConsumidores[nthreads];
    for (long i = 0; i < nthreads; i++) {
        if (pthread_create(&tidConsumidores[i], NULL, consumidorEscritor, (void*) i)) {
            fprintf(stderr, "Erro pthread_create()\n");
            exit(4);
        }
    }
    
    pthread_join(tidProdutor, NULL);

    for (int i = 0; i < nthreads; i++) {
        pthread_join(tidConsumidores[i], NULL);
    }

    free(buffer);

    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&mutex2);

    pthread_cond_destroy(&cond_cons);
    pthread_cond_destroy(&cond_prod);
    pthread_cond_destroy(&cond_escr);

    printf("%lf\n", get_wall_time() - inicio);

    return 0;
}
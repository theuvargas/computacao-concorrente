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
pthread_mutex_t mutex;

// cria um arquivo e escreve a string 'conteudo' nele
void criaArquivo(char* conteudo) {
    FILE* fp = fopen("saida.txt", "w");
       
    if (fp == NULL) {
        fprintf(stderr, "Erro fopen\n");
        exit(5);
    }

    fputs(conteudo, fp);

    fclose(fp);
}

// insere um vetor de inteiros no buffer, se possivel
void insere(int* vetor) {
    pthread_mutex_lock(&mutex);
    
    while (contador == TAMBUF) { // vetor esta cheio, bloqueia a thread
        pthread_cond_wait(&cond_prod, &mutex);
    }
    
    buffer[in] = vetor;
    in = (in+1) % TAMBUF;
    contador++;

    pthread_mutex_unlock(&mutex);
    pthread_cond_signal(&cond_cons); // produtor acabou de inserir, acorda um consumidor
}

// recebe uma string com numeros e retorna um vetor com esses numeros
int* stringParaVetorInt(char* str) {
    char* num;
    int* vetor = malloc(sizeof(int) * tamBloco);

    int i = 0;
    while((num = strsep(&str, " "))) { // divide a string por espacos
        if (strcmp("\n", num) == 0) 
            break;
        
        vetor[i++] = atoi(num); // converte a string em um numero e armazena no vetor
    }

    return vetor;
}

// recebe um vetor de int e retorna uma string com esses numeros
char* vetorParaString(int* vetor, int n) {
    char* linha = malloc(10000);
    for (int i = 0; i < n; i++) {
        sprintf(linha + strlen(linha), "%d ", vetor[i]); // concatena 'linha' com o proximo numero
    }
    linha[strlen(linha)-1] = '\0';
    return linha;
}

// remove e retorna um vetor do buffer, se houver
int* retira(long id) {
    pthread_mutex_lock(&mutex);

    int* vetor = NULL;
    while (contador == 0) { // buffer esta vazio
        if (elementosEscritos >= numElementos) { // se todos os elementos ja foram escritos, da break e retorna NULL
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
    pthread_cond_signal(&cond_prod); // consumidor consumiu um vetor, acorda o produtor

    return vetor;
}

// thread que le o arquivo e o coloca no buffer
void* produtor(void* arg) {
    int tamanhoLinha = 100000;
    char* linha = malloc(sizeof(char) * tamanhoLinha);
    FILE *fp = fopen("entrada.txt", "r");
    fgets(linha, tamanhoLinha, fp);
    numElementos = atoi(linha);

    numLinhas = numElementos/tamBloco;
    
    while (fgets(linha, tamanhoLinha, fp)) { // transforma cada linha em um vetor e o insere no buffer
        insere(stringParaVetorInt(linha));
    }

    free(linha);
    fclose(fp);

    pthread_exit(NULL);
}

// ordena um vetor dado com complexidade O(n^2)
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

// coordena a entrada de threads escritoras
void entraEscritor(long id) {
    pthread_mutex_lock(&mutex);
    while (escrevendo) { // uma thread esta escrevendo, bloqueia
        pthread_cond_wait(&cond_escr, &mutex);
    }
    escrevendo = true;

    pthread_mutex_unlock(&mutex);
}

// coordena a saida de threads escritoras
void saiEscritor() {
    pthread_mutex_lock(&mutex);
    escrevendo = false;
    pthread_cond_signal(&cond_escr); // escritor saiu, acorda outro escritor
    pthread_mutex_unlock(&mutex);
}

// thread que ordena as linha e as escreve no arquivo de saida
void* consumidorEscritor(void* arg) {
    long id = (long) arg;

    pthread_mutex_lock(&mutex);

    while (numElementos == -1) { // isso significa que produtor ainda nao fez nada, entao bloqueia
        pthread_cond_wait(&cond_cons, &mutex);
    }

    pthread_mutex_unlock(&mutex);

    while (elementosEscritos < numElementos) {
        int* vetor = retira(id); // pega um vetor do buffer
        
        if (vetor == NULL) break;

        insertionSort(vetor, tamBloco); // ordena o vetor

        entraEscritor(id);        
        char* linha = vetorParaString(vetor, tamBloco); // transforma o vetor ordenado em uma string
        sprintf(resultado+strlen(resultado), "%s\n", linha); // concatena resultado (a string que sera escrita no arquivo) com o vetor ordenado
        elementosEscritos += tamBloco;
        saiEscritor();

        free(vetor);
    }

    pthread_mutex_lock(&mutex);
    if (!arquivoCriado) { // se o arquivo ainda nao foi criado, cria e escreve 'resultado' nele
        criaArquivo(resultado);
        arquivoCriado = true;
        free(resultado);
    }
    pthread_mutex_unlock(&mutex);

    pthread_cond_broadcast(&cond_cons); // acorda consumidores bloqueados para que eles possam finalizar

    pthread_exit(NULL);
}

// funcao para medir tempo
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

    pthread_cond_destroy(&cond_cons);
    pthread_cond_destroy(&cond_prod);
    pthread_cond_destroy(&cond_escr);

    printf("%lf\n", get_wall_time() - inicio); // printa o tempo em segundos que o programa levou para concluir

    return 0;
}
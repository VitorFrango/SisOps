//
// Created by Vitor Frango on 06/05/2023.
//
//
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_LETRAS 26

pthread_mutex_t lock; // variável mutex para controlar acesso ao vetor de frequências

char *buf; // buffer para armazenar o conteúdo do arquivo
int buf_size; // tamanho do arquivo em bytes
int tam_bloco; // tamanho do bloco a ser processado por cada thread

int frequencias[NUM_LETRAS]; // vetor de frequências das letras do alfabeto

void *trabalhadora(void *arg) {
    int id = *((int *) arg);
    int inicio = id * tam_bloco;
    int fim = inicio + tam_bloco;
    int i;

    int *contador_global = calloc(NUM_LETRAS, sizeof(int)); // vetor de contadores de letras
    if (contador_global == NULL) {
        printf("Erro ao alocar memoria.\n");
        exit(1);
    }

    // processa o bloco de caracteres atribuído à thread
    for (i = inicio; i < fim; i++) {
        if (buf[i] >= 'A' && buf[i] <= 'Z') { // letra maiúscula
            contador_global[buf[i] - 'A']++; // incrementa o contador global da letra
        } else if (buf[i] >= 'a' && buf[i] <= 'z') { // letra minúscula
            contador_global[buf[i] - 'a']++;
        }
    }

    // imprime o resultado da tarefa
    float total_caracteres = (float) (fim - inicio);
    printf("T%d= %.3f%%\n", id, (total_caracteres / buf_size) * 100);

    // adiciona o resultado da tarefa ao vetor de contadores global
    pthread_mutex_lock(&lock);
    for (i = 0; i < NUM_LETRAS; i++) {
        frequencias[i] += contador_global[i];
    }
    pthread_mutex_unlock(&lock);

    free(contador_global);

    return NULL;
}


int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Uso: %s <numero de tarefas trabalhadoras> <numero de caracteres por bloco> <ficheiro>\n", argv[0]);
        return 1;
    }

    int i;
    int num_tarefas = atoi(argv[1]); // número de threads
    int num_caracteres = atoi(argv[2]); // tamanho do bloco em caracteres
    pthread_t threads[num_tarefas]; // vetor de threads
    int ids[num_tarefas]; // vetor de IDs das threads

    FILE *f = fopen(argv[3], "rb"); // abre o arquivo para leitura em modo binário
    if (f == NULL) {
        printf("Erro ao abrir o ficheiro.\n");
        return 1;
    }

    fseek(f, 0, SEEK_END); // posiciona o cursor no fim do arquivo
    buf_size = ftell(f); // obtém o tamanho do arquivo em bytes
    fseek(f, 0, SEEK_SET); // reposiciona o cursor no início do arquivo

    buf = malloc(buf_size); // aloca memória para o buffer
    if (buf == NULL) {
        printf("Erro ao alocar memoria.\n");
        return 1;
    }

    if (fread(buf, 1, buf_size, f) != buf_size) { // lê o arquivo para o buffer
        printf("Erro ao ler o ficheiro.\n");
        return 1;
    }

    tam_bloco = num_caracteres * num_tarefas; // define o tamanho do bloco para cada thread

    printf("Análise de letras do alfabeto com %d tarefas e blocos de %d caracteres\n", num_tarefas, tam_bloco);
    printf("Ficheiro %s (%d bytes)\n", argv[3], buf_size);
    printf("Relatório de tarefas:\n");

    pthread_mutex_init(&lock, NULL);

    for (i = 0; i < num_tarefas; i++) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, trabalhadora, &ids[i]);
    }

    for (i = 0; i < num_tarefas; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("\nRelatório de caracteres:\n");

    for (i = 0; i < NUM_LETRAS; i++) {
        printf("%c: %.2f%%\n", 'A' + i, ((float) frequencias[i] / buf_size) * 100);
    }

    free(buf);
    fclose(f);
    pthread_mutex_destroy(&lock);

    return 0;
}

//
// Created by Vitor Frango on 06/05/2023.
//
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <ctype.h>
#include <sys/stat.h>

#define MAX_THREADS 100 // número máximo de threads
#define MAX_BUFFER_SIZE 1000 // tamanho máximo do buffer
#define ALPHABET_SIZE 26 // tamanho do alfabeto

// estrutura para os argumentos das tarefas trabalhadoras
typedef struct {
    char *buffer; // ponteiro para o buffer de leitura
    int buffer_size; // tamanho do buffer de leitura
    int start_index; // índice de início do bloco de leitura
    int end_index; // índice de fim do bloco de leitura
    int *occurrences; // ponteiro para o array de ocorrências
} TaskArgs;

int main(int argc, char *argv[]) {
    int nt, nc;
    char *filename;
    struct stat file_stats;

    // verificar o número de argumentos
    if (argc != 4) {
        fprintf(stderr, "Erro: número de argumentos inválido.\n");
        fprintf(stderr, "Uso: %s nt nc ficheiro\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // obter os argumentos
    nt = atoi(argv[1]);
    nc = atoi(argv[2]);
    filename = argv[3];

    // verificar os valores dos argumentos
    if (nt < 1 || nt > MAX_THREADS) {
        fprintf(stderr, "Erro: número de threads inválido.\n");
        exit(EXIT_FAILURE);
    }

    if (nc < 1 || nc > MAX_BUFFER_SIZE) {
        fprintf(stderr, "Erro: tamanho de bloco inválido.\n");
        exit(EXIT_FAILURE);
    }

    // abrir o arquivo para leitura em modo binário
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        fprintf(stderr, "Erro: não foi possível abrir o arquivo %s.\n", filename);
        exit(EXIT_FAILURE);
    }

    // obter as estatísticas do arquivo
    if (stat(filename, &file_stats) == -1) {
        fprintf(stderr, "Erro: não foi possível obter as estatísticas do arquivo %s.\n", filename);
        exit(EXIT_FAILURE);
    }

    // imprimir as mensagens de início do programa
    printf("Análise de letras do alfabeto com %d tarefas e blocos de %d caracteres\n", nt, nc);
    printf("Ficheiro %s (%ld bytes)\n", filename, file_stats.st_size);
    printf("Relatório de tarefas:\n");

    // fechar o arquivo
    fclose(file);

    return 0;
}

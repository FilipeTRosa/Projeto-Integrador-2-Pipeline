#include "memoriaDados.h"
#include "memoria.h"
#include "minimips.h"
#include "controle.h"
#include "step.h"
#include "multiplexadores.h"
#include "decodificador.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>




void carregarDados(const char *nomeArquivo, struct memoria_dados *memDados){
    FILE *arquivo = fopen(nomeArquivo, "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo %s.\n", nomeArquivo);
        return;
    }

    char linha[32];
    int posicao = 0;

    // Inicializa a memória com um valor padrão (0 para indicar "vazio")
    for (int i = 0; i < memDados->tamanho; i++) {
        memDados->mem_dados[i].dado = 0;
        strcpy(memDados->mem_dados[i].dado_char, "vazio");
    }

    while (posicao < memDados->tamanho && fgets(linha, sizeof(linha), arquivo) != NULL) {
        // Remove quebras de linha e espaços extras
        linha[strcspn(linha, "\r\n")] = '\0';

        if (strlen(linha) > 0) {  // Se a linha não estiver vazia
            memDados->mem_dados[posicao].dado = atoi(linha);
            snprintf(memDados->mem_dados[posicao].dado_char, sizeof(memDados->mem_dados[posicao].dado_char), "%d", memDados->mem_dados[posicao].dado);
        }
        posicao++;
    }

    fclose(arquivo);
}

void insereMemDados(struct memoria_dados *mem, int endereco, int valor, int sinalControle){
    if (sinalControle == 1)
    {
        mem->mem_dados[endereco].dado = valor;
    }
}

int getDado(struct memoria_dados *mem, int endereco){
    int valor = mem->mem_dados[endereco].dado;
    return valor;
}


void imprimeDado(struct dado dado){
    printf("Valor: [%d]\n", dado.dado);
}

void imprimeMemDados(struct memoria_dados *mem){
    printf("==== Memoria de dados ====\n");
    for (int i = 0; i < mem->tamanho; i++)
    {
        printf("Posicao: [%d], ", i);
        imprimeDado(mem->mem_dados[i]);
    }
    printf("==========================\n");
}

struct memoria_dados* copiaMemoriaDados(struct memoria_dados* memoriaDados) {
    struct memoria_dados *newMemoria = (struct memoria_dados *)malloc(sizeof(struct memoria_dados));
    newMemoria->mem_dados = (struct dado *)malloc(memoriaDados->tamanho * sizeof(struct dado));

    for(int i = 0; i < memoriaDados->tamanho; i++) {
        newMemoria->mem_dados[i] = memoriaDados->mem_dados[i];
    }

    return newMemoria;
}

void salvarMemoriaEmArquivo(const char *nomeArquivo, struct memoria_dados *memDados) {
    FILE *arquivo = fopen(nomeArquivo, "w"); // "w" cria ou sobrescreve
    if (arquivo == NULL) {
        perror("Erro ao abrir o arquivo para escrita");
        return;
    }

    for (int i = 0; i < memDados->tamanho; i++) {
        fprintf(arquivo, "%d\n", memDados->mem_dados[i].dado);
    }

    fclose(arquivo);
}
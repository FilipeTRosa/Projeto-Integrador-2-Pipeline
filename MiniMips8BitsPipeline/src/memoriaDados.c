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

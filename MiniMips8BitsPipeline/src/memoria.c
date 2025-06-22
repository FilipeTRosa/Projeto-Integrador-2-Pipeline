#include "memoria.h"
#include "minimips.h"
#include "controle.h"
#include "pipeline.h"
#include "step.h"
#include "multiplexadores.h"
#include "decodificador.h"
#include "memoriaDados.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>



void salvarAsm(const char *nomeArquivo, struct memoria_instrucao *memInst) {
    FILE *arquivo = fopen(nomeArquivo, "w"); // "w" cria ou sobrescreve
    if (arquivo == NULL) {
        perror("Erro ao abrir o arquivo para escrita");
        return;
    }

    for (int i = 0; i < memInst->tamanho; i++) {
        fprintf(arquivo, "%s\n", memInst->mem_inst[i].assembly);
    }

    fclose(arquivo);
}

void carregarInstrucoes(const char *nomeArquivo, struct memoria_instrucao *mem){
    struct instrucao instrucaoDecodificada;
    FILE *arquivoEntrada = fopen(nomeArquivo, "r");  
    if (!arquivoEntrada) {
        printf("Erro ao abrir o arquivo %s.\n", nomeArquivo);
        return;
    }

    char caractere;
    char palavra[17];  // Tamanho instrução + \0
    int n = 0, posicao = 0;

    while ((caractere = fgetc(arquivoEntrada)) != EOF) {
        if (caractere != '\n') {
            palavra[n] = caractere;
            n++;
        } else {
            if (n > 0) {
                palavra[n] = '\0';  // Coloca \0 no final da string
                strcpy(mem->mem_inst[posicao].inst_char, palavra);
                instrucaoDecodificada = decodificaInstrucao(mem->mem_inst[posicao]);
                mem->mem_inst[posicao].opcode = instrucaoDecodificada.opcode;
                mem->mem_inst[posicao].rs = instrucaoDecodificada.rs;
                mem->mem_inst[posicao].rt = instrucaoDecodificada.rt;
                mem->mem_inst[posicao].rd = instrucaoDecodificada.rd;
                mem->mem_inst[posicao].funct = instrucaoDecodificada.funct;
                mem->mem_inst[posicao].imm = instrucaoDecodificada.imm;
                mem->mem_inst[posicao].addr = instrucaoDecodificada.addr;
                mem->mem_inst[posicao].tipo_inst = instrucaoDecodificada.tipo_inst;
                strcpy(mem->mem_inst[posicao].assembly, instrucaoDecodificada.assembly);
                posicao++;
            }
            n = 0;  // Reseta para a próxima linha
        }
    }

    fclose(arquivoEntrada);    
}


void imprimeInstrucao(struct instrucao inst){ 
    printf("Binario: [%s], ASM: [%s], opcode: [%d], rs: [%d], rt: [%d], rd: [%d], funct: [%d], imm: [%d], addr: [%d], tipo: [%s]\n",
        inst.inst_char, inst.assembly, inst.opcode, inst.rs,
        inst.rt, inst.rd, inst.funct,
        inst.imm, inst.addr, imprimeTipo(inst.tipo_inst));
}

void imprimeMemInstrucoes(struct memoria_instrucao *mem){
    printf("==== Memoria de instruçoes ====\n");
    for (int i = 0; i < mem->tamanho; i++)
    {
        printf("Posicao: [%d], ", i);
        imprimeInstrucao(mem->mem_inst[i]);
    }
    printf("===============================\n");
}

const char* imprimeTipo(enum classe_inst tipo) {
    switch (tipo) {
        case tipo_R:      return "tipo_R";
        case tipo_I:      return "tipo_I";
        case tipo_J:      return "tipo_J";
        case tipo_OUTROS: return "tipo_OUTROS";
        default:          return "DESCONHECIDO";
    }
}

void atualizaIR(RegINST* ir, struct instrucao nova_inst, int sinalControle) {
    if (sinalControle) { 
        ir->inst = nova_inst; 
    }
}


RegINST* criaRegIR() {
    RegINST *newReg = (RegINST *)malloc(sizeof(RegINST));
   
    return newReg;
}


int isNOP(int rs, int rt, int rd){
    if (rd == 0 && rs == 0 && rt == 0)
    {
        return 1;
    }
    return 0;
}


// void atualizaMDR(RegMDR *regMDR, int new_dados) {
    
//     regMDR->dado = new_dados;
// }


// RegMDR* criaRegMDR() {
//     RegMDR *newReg = (RegMDR *)malloc(sizeof(RegMDR));

//     return newReg;
// }


// void insereDadosMem(struct memoria_instrucao *mem, int endereco, int valor, int sinalControle){
//     int desvidoMemoriaInst = 128;
//     if (sinalControle == 1)
//     {   
//         char palavra[17] = "00000000";
//         char sufixo[9] = "00000000";
//         converteDecimalParaBinario(sufixo, valor);
//         strcat(palavra, sufixo);
//         strcpy(mem->mem_inst[desvidoMemoriaInst + endereco].inst_char, palavra);
//         mem->mem_inst[desvidoMemoriaInst + endereco].dado = valor;
//     }
// }

// int getDado(struct memoria_instrucao *mem, int endereco){
//     int desvidoMemoriaInst = 128;
//     int valor = mem->mem_inst[desvidoMemoriaInst + endereco].dado;
//     return valor;
// }

// void salvarMemoriaEmArquivo(const char *nomeArquivo, struct memoria_instrucao *mem) {
//     FILE *arquivo = fopen(nomeArquivo, "w"); // "w" cria ou sobrescreve
//     if (arquivo == NULL) {
//         perror("Erro ao abrir o arquivo para escrita");
//         return;
//     }

//     for (int i = 0; i < (mem->tamanho); i++) {
//         if (i == 127)
//         {
//             fprintf(arquivo, "%s\n", ".data");
//         }else
//         {
//             fprintf(arquivo, "%s\n", mem->mem_inst[i].inst_char);
//         }
//     }

//     fclose(arquivo);
// }

#include "pipeline.h"
#include "memoria.h"
#include "minimips.h"
#include "controle.h"
#include "step.h"
#include "multiplexadores.h"
#include "decodificador.h"
#include "memoriaDados.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>


int conversorBinParaDecimal (int compDeDois, char * palavra){
    int decimal = 0;
    int len = strlen(palavra);
    int i, posicao = 0;

    //ASCII//
    // 0 = 48
    // 1 = 49
    // ... //

    if (compDeDois == 1)
    {
        /* complemento de 2 */
        if ((palavra[0] - '0') == 0)
        {
            /* numero positivo */
            return conversorBinParaDecimal(0, palavra);
        }else
        {
            char binario[len + 1]; //no windows reclama dessa linha
            binario[len] = '\0';
            for (int j = 0; j < len ; j++) // for para inverter os bits
            {
                if ((palavra[j] - '0') == 0)
                {
                    binario[j] = '1';

                }else
                {
                    binario[j] = '0';
                }
                
            }
            i = len -1;
            for (; i>=0; i--)// for para somar 1
            {
                if ((binario[i] - '0') == 1)
                {
                    binario[i] = '0';
                }else
                {
                    binario[i] = '1';
                    break;
                }
            }
                /* numero negativo */
                int valor = conversorBinParaDecimal(0, binario);
                return -valor;
        }
        
    }else
    {
        /* não é complemento de 2 */
        decimal = 0;
        posicao = 0;
        i = len - 1;

        for (; i>=0; i--)
        {
            int num = palavra[i] - '0';
            decimal += num * pow(2 , posicao);
            posicao++;
        }
    }

    return decimal;
    
}
// retorna uma copia da instrução
struct instrucao buscaInstrucao(struct memoria_instrucao * memoria, int pc){
    struct instrucao inst = {0};    
    if (pc < 0 || pc >= memoria->tamanho) {
        printf("Erro: PC fora do intervalo.\n");
        return inst;  // Retorna uma instrução inválida (inicializada)
    }
    
    return memoria->mem_inst[pc];
}


BRegs* alocaBancoRegistradores() {

    BRegs* newBanco = (BRegs *)malloc(sizeof(BRegs));
    newBanco->registradores = NULL;
    newBanco->tamanho = 0;

    return newBanco;
}

regs* criaRegistrador() {

    regs *newReg = (regs *)malloc(sizeof(regs));
    newReg->id = 0;
    newReg->valor = 0;
    newReg->prox = NULL;

    return newReg;
}

void criaBanco(BRegs* bancoRegs, regs* reg){
    if(bancoRegs->registradores == NULL) {
        bancoRegs->registradores = reg;
        bancoRegs->registradores->id = bancoRegs->tamanho;
        bancoRegs->tamanho++;
    }
    else {
        regs *aux = bancoRegs->registradores;

        while(aux->prox != NULL) {
        aux = aux->prox;
        }
        aux->prox = reg;
        aux->prox->id = bancoRegs->tamanho;
        bancoRegs->tamanho++;
    }
}

void imprimeReg(regs* reg) {

    printf("║ $%-2d: %-4d ", reg->id, reg->valor);
}

void imprimeBanco(BRegs* bancoRegs, WINDOW* stepInterface) {

        regs *aux = bancoRegs->registradores;
        int contador = 0;
        mvwprintw(stepInterface, 10, 2, "== Banco de Registradores ==");
        while (aux != NULL && aux->id <= 7) {
           mvwprintw(stepInterface, 11, 2, "$%-2d: %-4d  ", aux->id, aux->valor);
           contador++;
           aux = aux->prox;
        }

       mvwprintw(stepInterface, 12, 2, "============================");

       wrefresh(stepInterface);
}


int* buscaBancoRegs(BRegs* bancoRegs, int rs, int rt, int rd, int defDest) {
    
    regs *aux = bancoRegs->registradores;
    int* vetBusca = (int *)malloc(3 * sizeof(int));

    if(defDest == 0) {
        rd = rt;
    }

    while((aux != NULL) & (aux->id != rs)) {
        aux = aux->prox;
    }

    vetBusca[0] = aux->valor;
    aux = bancoRegs->registradores;

    while(aux->id != rt) {
        aux = aux->prox;
    }

    vetBusca[1] = aux->valor;
    aux = bancoRegs->registradores;

    while(aux->id != rd) {
        aux = aux->prox;
    }

    vetBusca[2] = aux->id;

    return vetBusca;
}

int* processamentoULA(int op1, int op2, int ulaOP) {

    int* vetResultadoULA = (int *)malloc(3 * sizeof(int));
    char result[8];
    char operando1[8];
    char operando2[8];

    vetResultadoULA[2] = 0;

    switch(ulaOP) {
        case 0:
        case 1:
        case 3:
        case 7:
            vetResultadoULA[0] = op1 + op2;
            vetResultadoULA[1] = verificaOverflow(vetResultadoULA[0]);
            break;
        case 2:
            vetResultadoULA[0] = op1 - op2;
            vetResultadoULA[1] = verificaOverflow(vetResultadoULA[0]);

            break;
        case 4:
            converteDecimalParaBinario(operando1, op1);
            converteDecimalParaBinario(operando2, op2);

            for(int i = 0; i < 8; i++) {
                if(operando1[i] == operando2[i]) {
                    result[i] = operando1[i];
                }
            }

            vetResultadoULA[0] = conversorBinParaDecimal(0, result);
            vetResultadoULA[1] = verificaOverflow(vetResultadoULA[0]);
            break;
        case 5:
            converteDecimalParaBinario(operando1, 0);
            converteDecimalParaBinario(operando2, 0);

            for(int i = 0; i < 8; i++) {
                if(operando1[i] == 1 || operando2[i] == 1) {
                    result[i] = 1;
                } 
            }

            vetResultadoULA[0] = conversorBinParaDecimal(0, result);
            vetResultadoULA[1] = verificaOverflow(vetResultadoULA[0]);
            break;
        case 6://beq
            vetResultadoULA[2] = comparaRegs(op1, op2);
            break;
    }

    return vetResultadoULA;
}

void saidaULA(int resultUla, int * saidaula, int clear){
    if (clear == 1)
    {
        *saidaula = 0;
        return;
    }
    *saidaula  = resultUla;
}

int regSaidaULA (int resultULA, int clear){
    if (clear == 1)
    {
        return 0;
    }
    return resultULA;
}

int fuctionMux(int op1, int op2, int controleULA) {

    if(controleULA == 0) {
        return op1;
    }
    else if(controleULA == 1) {
        return op2;
    }

    return 10; // erro 

}

int verificaOverflow(int opResult) {
    int flag = 0;

    if(opResult > 127 || opResult < -128) {
        flag = 1;
    }

    return flag;
}

int comparaRegs(int op1, int op2) {
    
    int flag = 0;

    if((op1 - op2) == 0) {
        flag = 1;
    }

    return flag;
}

void salvaDadoReg(BRegs* bancoRegs, int resultadoULA, int vetBuscaReg, int sinalControle) { 
    if (sinalControle == 1)
    {
        regs *aux = bancoRegs->registradores;

        while(aux->id != vetBuscaReg) {
            aux = aux->prox;
        }

        aux->valor = resultadoULA;
    }
}

struct estatistica * criaStat() {
    struct estatistica * new_stat = (struct estatistica *)malloc(sizeof(struct estatistica));
    new_stat->back = 0;
    new_stat->tipoI = 0;
    new_stat->tipoJ = 0;
    new_stat->tipoR = 0;
    new_stat->totalInstrucoes = 0;

    return new_stat;
}

void inverteString(const char *origem, char *destino) {
    int tamanho = strlen(origem);
    for (int i = 0; i < tamanho; i++) {
        destino[i] = origem[tamanho - 1 - i]; 
    }
    destino[tamanho] = '\0'; 
}

void converteDecimalParaBinario(char * palavra, int num){
    char aux[9];
    int i = 0;
    int negativo = (num < 0); // guada 1 para verdadeiro e 0 parra falso
    //ASCII//
    // 0 = 48
    // 1 = 49
    // ... //
    
    if (negativo) {
        num = -num; // inverte o sinal
    }

    if (num == 0) {
        strcpy(palavra, "00000000");
        return;
    }

    while (num > 0) {
        aux[i++] = (num % 2) + '0'; 
        num /= 2;
    }

    while (i < 8) {
        aux[i++] = '0';
    }

    aux[i] = '\0';

    inverteString(aux, palavra);

    // Se for negativo, aplica complemento de dois
    if (negativo) {
        // inverte os bits
        for (int j = 0; j < 8; j++) {
            palavra[j] = (palavra[j] == '0') ? '1' : '0';
        }

        //Soma 1
        for (int j = 7; j >= 0; j--) {
            if (palavra[j] == '0') {
                palavra[j] = '1';
                break;
            } else {
                palavra[j] = '0';
            }
        }
    }

    palavra[8] = '\0'; // Garante que a string está terminada

}

int regABsaida(int outBancoRegs, int clear) {
    if(clear == 1) {
        return 0;
    }

    return outBancoRegs;
}


void imprimeULA(int *resultadoULA){
    printf("Resultado ULA \n");
    printf("Resultado: [%d], Oveflow: [%d], ComparaREG: [%d]\n",
    resultadoULA[0], resultadoULA[1], resultadoULA[2]);
}

descPilha* criarPilha() {
    descPilha* new_pilha = (descPilha *)malloc(sizeof(descPilha));

    new_pilha->instrucoes = NULL;
    new_pilha->tamanho = 0;

    return new_pilha;
}

/*void printStack(descPilha * pilha){
    nodoPilha *aux = pilha->instrucoes;
    while (aux != NULL)
    {
        imprimeBanco(aux->bancoRegs);
        aux = aux->prox;
    }
    
}

*/

void inserePilha(descPilha* pilha, nodoPilha* nodo) {
    
    if(pilha->tamanho == 0) {
        pilha->instrucoes = nodo;
        pilha->tamanho++;
    }
    else {
        nodoPilha *aux = pilha->instrucoes;
        pilha->instrucoes = nodo;
        nodo->prox = aux;
        pilha->tamanho++;
    }
}

nodoPilha* removePilha(descPilha* pilha) {

    nodoPilha *aux = pilha->instrucoes;

    if(pilha->tamanho == 0) {
        printf("\nNenhuma instrucao foi executada!\n");
    }
    else {
        pilha->instrucoes = aux->prox;
        pilha->tamanho--;
    }

    return aux;
}

nodoPilha* criaNodo(int pc, BRegs* bancoRegs, memDados* memoriaDados, RegALL * regInAll, RegALL * regOutAll) {
    
    nodoPilha *new_nodo = (nodoPilha *)malloc(sizeof(nodoPilha));

    new_nodo->bancoRegs = bancoRegs;
    new_nodo->pc = pc;
    new_nodo->memoriaDados = memoriaDados;
    new_nodo->prox = NULL;
    new_nodo->regInAll = regInAll;
    new_nodo->regOutAll = regOutAll;

    return new_nodo;
}


BRegs* copiaBancoRegistradores(BRegs* bancoRegs) {
    
    BRegs *newBanco = (BRegs *)malloc(sizeof(BRegs));
    newBanco->registradores = NULL;
    newBanco->tamanho = 0;

    regs *aux = bancoRegs->registradores;
    regs *copiaAux = NULL;

    while(aux->prox != NULL) {
        copiaAux = criaRegistrador();
        copiaAux->valor = aux->valor;
        criaBanco(newBanco, copiaAux);
        aux = aux->prox;
    }

    return newBanco;
}

void imprimeLogNoTerminal(const char *nomeArquivo) {
    FILE *arquivo = fopen(nomeArquivo, "r");
    if (!arquivo) {
        printf("Erro ao abrir o arquivo de log: %s\n", nomeArquivo);
        return;
    }

    char linha[256];
    while (fgets(linha, sizeof(linha), arquivo)) {
        printf("%s", linha);
    }

    fclose(arquivo);
}

void imprimeEstatistica(struct estatistica * est) {
    printf("===== Estatísticas da Execução =====\n");
    printf("Total de Instruções: %d\n", est->totalInstrucoes);
    printf("Tipo R: %d\n", est->tipoR);
    printf("Tipo I: %d\n", est->tipoI);
    printf("Tipo J: %d\n", est->tipoJ);
    printf("Back: %d\n", est->back);
    printf("Total Back + Instruções: %d \n", (est->back + est->totalInstrucoes));
    printf("====================================\n");
}


ULAsaida* criaRegSaidaULA() {
    ULAsaida* newReg = (ULAsaida *)malloc(sizeof(ULAsaida));

    newReg->resultULA = 0;
    
    return newReg;
}
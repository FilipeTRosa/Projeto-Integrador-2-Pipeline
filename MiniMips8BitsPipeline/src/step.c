#include "pipeline.h"
#include "memoria.h"
#include "memoriaDados.h"
#include "minimips.h"
#include "controle.h"
#include "multiplexadores.h"
#include "decodificador.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "step.h"


// Estágio 1: Busca de Instrução
void estagio_BI(int *pc, RegBIDI *bidi_out, struct memoria_instrucao *memInst) {
    bidi_out->IR = buscaInstrucao(memInst, *pc);
    bidi_out->pc_incrementado = *pc + 1;
    printf("\n========= ESTAGIO BIDI =========\n");
    printf("Detalhes da Instrução:\n");
    printf("  - Instrução: \t%s\n", bidi_out->IR.assembly);
    printf("  - Campos: \topcode=%-2d | rs=%d | rt=%d | rd=%d | funct=%d | imm=%d | addr=%d\n",
           bidi_out->IR.opcode, bidi_out->IR.rs, bidi_out->IR.rt, bidi_out->IR.rd,
           bidi_out->IR.funct, bidi_out->IR.imm, bidi_out->IR.addr);
    printf("Conteudo BIDI-PCincrementado: [%d] \n", bidi_out->pc_incrementado);
    
}

void estagio_DI(RegBIDI *bidi_in, RegDIEX *diex_out, BRegs *bancoReg, int *parada) {
    // Acessa a instrução diretamente de 'bidi_in->IR'
    //if (strcmp(bidi_in->IR.inst_char, "0000000000000000") == 0) {
    //    *parada = 0;
    //    *diex_out = *criaRegDIEX(); 
    //    return;
    //}
    printf("\n========= ESTAGIO DIEX =========");

    setSignal(diex_out->controle_DIEX, bidi_in->IR.opcode, bidi_in->IR.funct);
    imprimeControle(diex_out->controle_DIEX);
    printf("Instrução -> [%s] \n", bidi_in->IR.assembly);

    printf(">>> rs: [%d] - ", bidi_in->IR.rs);
    printf("rt: [%d] - ", bidi_in->IR.rt);
    
    int *vetor_busca = buscaBancoRegs(bancoReg, bidi_in->IR.rs, bidi_in->IR.rt, bidi_in->IR.rd, diex_out->controle_DIEX->regDest);
    
    printf("rA: [%d] - ", vetor_busca[0]);
    printf("rB: [%d] - \n", vetor_busca[1]);

    strcpy(diex_out->assembly, bidi_in->IR.assembly);
    diex_out->RegA = vetor_busca[0];
    diex_out->RegB = vetor_busca[1];
    diex_out->imm = bidi_in->IR.imm;
    diex_out->rt = bidi_in->IR.rt;
    diex_out->rd = bidi_in->IR.rd;
    diex_out->pc_incrementado = bidi_in->pc_incrementado;
    
    // Não se esqueça de liberar a memória alocada por buscaBancoRegs
    // free(vetor_busca); 
}


void estagio_EX(RegDIEX *diex_in, RegEXMEM *exmem_out) {
    printf("\n========= ESTAGIO EXMEM =========");
    imprimeControle(diex_in->controle_DIEX);
    printf("Instrução -> [%s] \n", diex_in->assembly);

    *(exmem_out->controle_EXEMEM) = *(diex_in->controle_DIEX);
    strcpy(exmem_out->assembly, diex_in->assembly);

    Mux *muxop2 = criaMux(diex_in->RegB, diex_in->imm, 0, diex_in->controle_DIEX->srcB);
    int op2 = muxFuncition(muxop2);
    free(muxop2);

    exmem_out->resultULA = processamentoULA(diex_in->RegA, op2, diex_in->controle_DIEX->ulaOP);
    exmem_out->RegB = diex_in->RegB;

    // MUX para o registrador de destino (RegDst)
    Mux *muxrd = criaMux(diex_in->rt, diex_in->rd, 0, diex_in->controle_DIEX->regDest);
    exmem_out->rd = muxFuncition(muxrd);
    free(muxrd); 
}

// Estágio 4: Acesso à Memória
void estagio_MEM(RegEXMEM *exmem_in, RegMEMER *memer_out, struct memoria_dados *memDados) {

    printf("\n========= ESTAGIO MEM =========");
    imprimeControle(exmem_in->controle_EXEMEM);
    printf("Instrução -> [%s] \n", exmem_in->assembly);


    *(memer_out->controle_MEMER) = *(exmem_in->controle_EXEMEM);
    memer_out->resultULA = exmem_in->resultULA;
    memer_out->rd = exmem_in->rd;
    strcpy(memer_out->assembly, exmem_in->assembly);
    

    if ((exmem_in->controle_EXEMEM->memWrite == 1) && (exmem_in->controle_EXEMEM->memReg == 1)) {
        //addi
        printf("Entrou no ADDI \n");
        insereMemDados(memDados, exmem_in->resultULA[0], exmem_in->RegB, 1);
    } else if ((exmem_in->controle_EXEMEM->regWrite == 1) && (exmem_in->controle_EXEMEM->memReg == 0)) {
        //LW
        printf("Entrou no LW \n");
        memer_out->dado = getDado(memDados, exmem_in->resultULA[0]);
        printf("Dado buscado = [%d] \n", memer_out->dado);
    }
    
}

void estagio_ER(RegMEMER *memer_in, BRegs *bancoReg) {
    printf("\n========= ESTAGIO ER =========");
    imprimeControle(memer_in->controle_MEMER);
    printf("Instrução -> [%s] \n", memer_in->assembly);
    //printf("Resultado ULA -> [%d] \n", memer_in->resultULA[0]);
    printf("Dado a ser escrito = [%d] \n", memer_in->dado);

    if (memer_in->controle_MEMER->regWrite == 1) {
        //Mux *muxwb = criaMux(memer_in->resultULA[0], memer_in->dado, 0, memer_in->controle_MEMER->memReg); //ASSIM NAO FUNCIONAVA
        Mux *muxwb = criaMux(memer_in->dado, memer_in->resultULA[0], 0, memer_in->controle_MEMER->memReg);
        int dado_final = muxFuncition(muxwb);
        //printf("dado buscado = [%d] \n", dado_final);
        free(muxwb); 
        salvaDadoReg(bancoReg, dado_final, memer_in->rd, 1);
    }
    imprimeBanco(bancoReg);
}


void step(int *contClock, int *pc, int *parada, RegALL *regIN, RegALL *regOUT, BRegs *bancoReg, struct memoria_instrucao *memInst, struct memoria_dados *memDados) {
    *contClock += 1;
    printf("Cont Clock = [%d] \n", *contClock);
    estagio_ER(regIN->MEMER, bancoReg);
    estagio_MEM(regIN->EXMEM, regOUT->MEMER, memDados);
    estagio_EX(regIN->DIEX, regOUT->EXMEM);
    estagio_DI(regIN->BIDI, regOUT->DIEX, bancoReg, parada);
    if (*parada) {
        estagio_BI(pc, regOUT->BIDI, memInst);
    } else {
        *(regOUT->BIDI) = *criaRegBIDI(); 
    }

    // incrementar o PC
    if (regOUT->DIEX->controle_DIEX->jump == 1) {
        *pc = regOUT->DIEX->imm;
    } else if (regOUT->EXMEM->controle_EXEMEM->branch == 1 && regOUT->EXMEM->resultULA == 0) {
        *pc = regIN->DIEX->pc_incrementado + regIN->DIEX->imm;
    } else if (*parada) {
        *pc = regOUT->BIDI->pc_incrementado;
    }

}
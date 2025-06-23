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
    printf("\n========= ESTAGIO BI =========\n");
    printf("PC ----->> %d \n", *pc);
    printf("Detalhes da Instrução:\n");
    printf("  - Instrução: \t%s\n", bidi_out->IR.assembly);
    printf("  - Campos: \topcode=%-2d | rs=%d | rt=%d | rd=%d | funct=%d | imm=%d | addr=%d\n",
           bidi_out->IR.opcode, bidi_out->IR.rs, bidi_out->IR.rt, bidi_out->IR.rd,
           bidi_out->IR.funct, bidi_out->IR.imm, bidi_out->IR.addr);
    printf("Conteudo BIDI-PCincrementado: [%d] \n", bidi_out->pc_incrementado);
    
}

void estagio_DI(RegBIDI *bidi_in, RegDIEX *diex_out, BRegs *bancoReg, int *parada) {

    printf("\n========= ESTAGIO DI =========\n");
    setSignal(diex_out->controle_DIEX, bidi_in->IR.opcode, bidi_in->IR.funct);
    printf("Instrução -> [%s]", bidi_in->IR.assembly);
    imprimeControle(diex_out->controle_DIEX);
    printf(">>> rs: [%d] - ", bidi_in->IR.rs);
    printf("rt: [%d] - ", bidi_in->IR.rt);
    
    int *vetor_busca = buscaBancoRegs(bancoReg, bidi_in->IR.rs, bidi_in->IR.rt, bidi_in->IR.rd, diex_out->controle_DIEX->regDest);
    
    printf("rA: [%d] - ", vetor_busca[0]);
    printf("rB: [%d] - \n", vetor_busca[1]);

    strcpy(diex_out->assembly, bidi_in->IR.assembly); //copia inst
    strcpy(diex_out->binario, bidi_in->IR.inst_char); 
    diex_out->RegA = vetor_busca[0];
    diex_out->RegB = vetor_busca[1];
    diex_out->imm = bidi_in->IR.imm;
    diex_out->addr = bidi_in->IR.addr;
    diex_out->rt = bidi_in->IR.rt;
    diex_out->rd = bidi_in->IR.rd;
    diex_out->rs = bidi_in->IR.rs;
    diex_out->pc_incrementado = bidi_in->pc_incrementado;
    
}


void estagio_EX(RegDIEX *diex_in, RegEXMEM *exmem_out) {
    printf("\n========= ESTAGIO EX =========\n");
    printf("Instrução -> [%s]", diex_in->assembly);
    imprimeControle(diex_in->controle_DIEX);    

    *(exmem_out->controle_EXEMEM) = *(diex_in->controle_DIEX);
    strcpy(exmem_out->assembly, diex_in->assembly);

    Mux *muxop2 = criaMux(diex_in->RegB, diex_in->imm, 0, diex_in->controle_DIEX->srcB);
    int op2 = muxFuncition(muxop2);
    free(muxop2);
    printf("Operando 1 da ULA = [%d]\n", diex_in->RegA);
    printf("Operando 2 da ULA = [%d]\n", op2);
    exmem_out->resultULA = processamentoULA(diex_in->RegA, op2, diex_in->controle_DIEX->ulaOP);
    exmem_out->RegB = diex_in->RegB;
    printf("RESULTADO ULA = [Calc - %d | Over - %d | Comp - %d]\n", exmem_out->resultULA[0], exmem_out->resultULA[1], exmem_out->resultULA[2]);
    
    
    // MUX para o registrador de destino (RegDst)
    Mux *muxrd = criaMux(diex_in->rt, diex_in->rd, 0, diex_in->controle_DIEX->regDest);
    exmem_out->rd = muxFuncition(muxrd);
    printf("Registrador destino = [%d]\n", exmem_out->rd);
    free(muxrd); 
}

// Estágio 4: Acesso à Memória
void estagio_MEM(RegEXMEM *exmem_in, RegMEMER *memer_out, struct memoria_dados *memDados) {

    printf("\n========= ESTAGIO MEM =========\n");
    printf("Instrução -> [%s]", exmem_in->assembly);
    imprimeControle(exmem_in->controle_EXEMEM);
   
    *(memer_out->controle_MEMER) = *(exmem_in->controle_EXEMEM);
    memer_out->resultULA = exmem_in->resultULA;
    memer_out->rd = exmem_in->rd;
    strcpy(memer_out->assembly, exmem_in->assembly);
    
    if (exmem_in->controle_EXEMEM->memWrite == 1) {
        //SW
        printf("Entrou no ADDI (SW??) \n");
        printf("Dado a ser escrito [%d]\n", exmem_in->RegB);
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

    if (memer_in->controle_MEMER->regWrite == 1) {
        Mux *muxwb = criaMux(memer_in->dado, memer_in->resultULA[0], 0, memer_in->controle_MEMER->memReg);
        int dado_final = muxFuncition(muxwb);
        printf("dado buscado = [%d] \n", dado_final);
        free(muxwb); 
        salvaDadoReg(bancoReg, dado_final, memer_in->rd, 1);
    }
    imprimeBanco(bancoReg);
}


void step(descPilha *pilha, int *contClock, int *pc, int *parada, RegALL *regIN, RegALL *regOUT, BRegs *bancoReg, struct memoria_instrucao *memInst, struct memoria_dados *memDados) {
    *contClock += 1;
    printf("======== Inicio Step ========\n");
    printf("Cont Clock = [%d] \n", *contClock);

    //PILHA para StepBack
    BRegs * copiaBanco = copiaBancoRegistradores(bancoReg);
    struct memoria_dados * copiaMemDados = copiaMemoriaDados(memDados);
    nodoPilha *novoNodo = criaNodo(*pc, copiaBanco, copiaMemDados, regIN);
    inserePilha(pilha, novoNodo);

    estagio_ER(regIN->MEMER, bancoReg);
    estagio_MEM(regIN->EXMEM, regOUT->MEMER, memDados);
    estagio_EX(regIN->DIEX, regOUT->EXMEM);
    if (regOUT->EXMEM->controle_EXEMEM->branch == 1 && regOUT->EXMEM->resultULA[2] == 1)
    {

        // BEQ → branch verdadeiro
        *pc = (regIN->DIEX->pc_incrementado) + regIN->DIEX->imm;
        regOUT->BIDI = criaRegBIDI(); // bolha em BI
        regOUT->DIEX = criaRegDIEX(); // bolha em DI
    }else
    {
        estagio_DI(regIN->BIDI, regOUT->DIEX, bancoReg, parada);
        estagio_BI(pc, regOUT->BIDI, memInst);
        //HAZARDDDDD
        if (unidadeDeHazard(regIN,regOUT))
        {

            //enviar DI para BI
            regOUT->BIDI->IR = criaIR();
            //caso queira só 1 bolha usar este//regOUT->BIDI->IR = buscaInstrucao(memInst, regOUT->DIEX->pc_incrementado - 1);
            regOUT->BIDI->pc_incrementado = regOUT->DIEX->pc_incrementado;
            //bolha no DI
            *pc = regOUT->DIEX->pc_incrementado - 1;
            regOUT->DIEX = criaRegDIEX();
            //talvez eu tenha que copiar aquele pc incrementado
            //para essa bolha do DI
            
        }else{
            //JUMP
            if (regOUT->DIEX->controle_DIEX->jump == 1) {
                printf("Addr [%d]", regOUT->DIEX->addr);
                *pc = 0 + regOUT->DIEX->addr;
                regOUT->BIDI = criaRegBIDI(); // bolha em BI
                printf("PC ----->> %d", *pc);
                regOUT->DIEX = criaRegDIEX(); // bolha em DI
            }else{
                *pc = regOUT->BIDI->pc_incrementado;
            }
        }
    }

    if (*pc >= 256)
    {
        *parada = 0;
    }
    printf("======== Fim Step ========\n");
}
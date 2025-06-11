#include "memoria.h"
#include "memoriaDados.h"
#include "minimips.h"
#include "controle.h"
#include "multiplexadores.h"
#include "decodificador.h"
#include "pipeline.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "step.h"

/*
void step(int *parada, int *pc, struct memoria_dados *memDados, struct memoria_instrucao *memInst, BRegs *bancoReg, CTRL *controle, descPilha *pilha, struct estatistica * stat){
    int *buscaReg = NULL;
    struct instrucao instBuscada;
    instBuscada = buscaInstrucao(memInst, *pc);
    if (strcmp(instBuscada.inst_char, "0000000000000000") == 0) //condição DEFAULT de parada do programa
    {
        printf("Programa finalizado com sucesso!!!\n");
        *parada = 0;
    }else
    {
        //AQUI COLOCAR O NO DA PILHA
        BRegs* copiaBanco = copiaBancoRegistradores(bancoReg);
        struct memoria_dados* copiaMemDados = copiaMemoriaDados(memDados);
        nodoPilha *newNodo = criaNodo(*pc, copiaBanco, copiaMemDados);
        inserePilha(pilha, newNodo);

        //setando variaveis de funcinamento
        int *vetBusca = NULL;
        int *resultadoULA = NULL;
        int operando2;
        //fim configuração.

        printf("\n ********* Inicio da Instrução ********* \n");
        printf("->PC: [%d]\n",*pc);
        printf("->Instrução executada: [%s]\n", instBuscada.assembly);
        printf("->Registradores estado antigo");
        imprimeBanco(bancoReg);
        if (strlen(instBuscada.inst_char) > 1)
        {
            setSignal(controle, instBuscada.opcode, instBuscada.funct);
            //imprimeControle(controle);
            switch (instBuscada.tipo_inst)
            {
            case tipo_R:
                stat->tipoR++;
                vetBusca = buscaBancoRegs(bancoReg, instBuscada.rs, instBuscada.rt, instBuscada.rd, controle->regDest); // retorna [[rs][rt][rd]]
                operando2 = fuctionMux(vetBusca[1], instBuscada.imm, controle->srcB);// Mux para saber de onde vem o op2 da ula ->> REG/IMM <<-
                resultadoULA = processamentoULA(vetBusca[0], operando2, controle->ulaOP);// retorna [[resultado][overflow][comparaREG]]
                //testar flag antes
                salvaDadoReg(bancoReg,resultadoULA[0], vetBusca[2], controle->regWrite);
                printf("->Registradores estado novo");
                imprimeBanco(bancoReg);
                *pc = *pc + 1;
                break;
            case tipo_I:
                stat->tipoI++;
                if (instBuscada.opcode == 8) //beq
                {
                    vetBusca = buscaBancoRegs(bancoReg, instBuscada.rs, instBuscada.rt, instBuscada.rd, controle->regDest); // retorna [[rs][rt][rd]]
                    operando2 = fuctionMux(vetBusca[1], instBuscada.imm, controle->srcB);// Mux para saber de onde vem o op2 da ula ->> REG/IMM <<-
                    resultadoULA = processamentoULA(vetBusca[0], operando2, controle->ulaOP);// retorna [[resultado][overflow][comparaREG]]
                    if (resultadoULA[2] == 1 && controle->branch == 1)
                    {
                        *pc = *pc + instBuscada.imm + 1;
                        break;
                    }else
                    {
                        *pc = *pc + 1;
                        break;
                    }    
                }
                //teste addi
                vetBusca = buscaBancoRegs(bancoReg, instBuscada.rs, instBuscada.rt, instBuscada.rd, controle->regDest); // retorna [[rs][rt][rd]]
                operando2 = fuctionMux(vetBusca[1], instBuscada.imm, controle->srcB);// Mux para saber de onde vem o op2 da ula ->> REG/IMM <<-
                resultadoULA = processamentoULA(vetBusca[0], operando2, controle->ulaOP);// retorna [[resultado][overflow][comparaREG]]
                if (controle->memReg == 1 && controle->regWrite == 1) 
                {
                    //se for ADDI
                    salvaDadoReg(bancoReg,resultadoULA[0], vetBusca[2], controle->regWrite);
                }
                if (controle->memReg == 0 && controle->regWrite == 1)
                {
                    //Se for LW
                    salvaDadoReg(bancoReg, getDado(memDados, resultadoULA[0]), vetBusca[2], controle->regWrite);
                }
                printf("->Registradores estado novo"); 
                imprimeBanco(bancoReg);
                //se for SW só salva na mem se memWrite = 1
                insereMemDados(memDados, resultadoULA[0], vetBusca[1], controle->memWrite);
                *pc = *pc + 1;
                break;
            case tipo_J:
                stat->tipoJ++;
                *pc = 0 + instBuscada.addr;
                break;
            default:
                break;
            }
            printf(" ********* FIM da Instrução ********* \n\n");
            stat->totalInstrucoes++;
        }else
        {
            printf("\n Instrucao invalida.\n");
        }
    }
}
*/

// Estágio 1: Busca de Instrução
void estagio_BI(int *pc, RegBIDI *bidi_out, struct memoria_instrucao *memInst) {
    bidi_out->IR = buscaInstrucao(memInst, *pc);
    bidi_out->pc_incrementado = *pc + 1;

    printf("Detalhes da Instrução:\n");
    printf("  - Instrução: \t%s\n", bidi_out->IR.assembly);
    printf("  - Campos: \topcode=%-2d | rs=%d | rt=%d | rd=%d | funct=%d | imm=%d | addr=%d\n",
           bidi_out->IR.opcode, bidi_out->IR.rs, bidi_out->IR.rt, bidi_out->IR.rd,
           bidi_out->IR.funct, bidi_out->IR.imm, bidi_out->IR.addr);

}

void estagio_DI(RegBIDI *bidi_in, RegDIEX *diex_out, BRegs *bancoReg, int *parada) {
    // Acessa a instrução diretamente de 'bidi_in->IR'
    //if (strcmp(bidi_in->IR.inst_char, "0000000000000000") == 0) {
    //    *parada = 0;
    //    *diex_out = *criaRegDIEX(); 
    //    return;
    //}

    setSignal(diex_out->controle_DIEX, bidi_in->IR.opcode, bidi_in->IR.funct);

    printf("rs: %d", bidi_in->IR.rs);
    printf("rt: %d", bidi_in->IR.rt);
    
    int *vetor_busca = buscaBancoRegs(bancoReg, bidi_in->IR.rs, bidi_in->IR.rt, bidi_in->IR.rd, diex_out->controle_DIEX->regDest);
    
    printf("rA: %d", vetor_busca[0]);
    printf("rB: %d", vetor_busca[1]);

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
    
    *(exmem_out->controle_EXEMEM) = *(diex_in->controle_DIEX);

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
    *(memer_out->controle_MEMER) = *(exmem_in->controle_EXEMEM);
    memer_out->resultULA = exmem_in->resultULA;
    memer_out->rd = exmem_in->rd;
    
    if (exmem_in->controle_EXEMEM->memWrite == 1) {
        insereMemDados(memDados, exmem_in->resultULA, exmem_in->RegB, 1);
    } else if (exmem_in->controle_EXEMEM->memReg == 1) {
        memer_out->dado = getDado(memDados, exmem_in->resultULA);
    }
}

void estagio_ER(RegMEMER *memer_in, BRegs *bancoReg) {
    if (memer_in->controle_MEMER->regWrite == 1) {

        Mux *muxwb = criaMux(memer_in->resultULA, memer_in->dado, 0, memer_in->controle_MEMER->memReg);
        int dado_final = muxFuncition(muxwb);
        free(muxwb); 
        salvaDadoReg(bancoReg, dado_final, memer_in->rd, 1);
    }
}


void step(int *pc, int *parada, RegALL *regIN, RegALL *regOUT, BRegs *bancoReg, struct memoria_instrucao *memInst, struct memoria_dados *memDados) {

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
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
#include <ncurses.h>
#include "step.h"

// Estágio 1: Busca de Instrução
void estagio_BI(int *pc, RegBIDI *bidi_out, struct memoria_instrucao *memInst, WINDOW* stepInterface) {
    bidi_out->IR = buscaInstrucao(memInst, *pc);
    bidi_out->pc_incrementado = *pc + 1;
    //mvwprintw(stepInterface, 35, 2, "========= ESTAGIO BI =========");
    //mvwprintw(stepInterface, 36, 2, "PC ----->> %d \n", *pc);
    //mvwprintw(stepInterface, 37, 2, "Detalhes da Instrução:\n");
    //mvwprintw(stepInterface, 38, 2, "  - Instrução: \t%s\n", bidi_out->IR.assembly);
    //mvwprintw(stepInterface, 39, 2, "  - Campos: \topcode=%-2d | rs=%d | rt=%d | rd=%d | funct=%d | imm=%d | addr=%d\n",
           //bidi_out->IR.opcode, bidi_out->IR.rs, bidi_out->IR.rt, bidi_out->IR.rd,
           //bidi_out->IR.funct, bidi_out->IR.imm, bidi_out->IR.addr);
    //mvwprintw(stepInterface, 40, 2, "Conteudo BIDI-PCincrementado: [%d] \n", bidi_out->pc_incrementado);

    wrefresh(stepInterface);
    
}

void estagio_DI(RegBIDI *bidi_in, RegDIEX *diex_out, BRegs *bancoReg, int *parada, WINDOW* stepInterface) {

    setSignal(diex_out->controle_DIEX, bidi_in->IR.opcode, bidi_in->IR.funct);
    mvwprintw(stepInterface, 5, 11, "%s", bidi_in->IR.assembly);
    //imprimeControle(diex_out->controle_DIEX, stepInterface, flagInterface);
    //mvwprintw(stepInterface, 31, 2, ">>> rs: [%d] - ", bidi_in->IR.rs);
    //mvwprintw(stepInterface, 32, 2, "rt: [%d] - ", bidi_in->IR.rt);
    
    int *vetor_busca = buscaBancoRegs(bancoReg, bidi_in->IR.rs, bidi_in->IR.rt, bidi_in->IR.rd, diex_out->controle_DIEX->regDest);
    
    //mvwprintw(stepInterface, 33, 2, "rA: [%d] - ", vetor_busca[0]);
    //mvwprintw(stepInterface, 34, 2, "rB: [%d] - \n", vetor_busca[1]);

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

    wrefresh(stepInterface);
    
}


void estagio_EX(RegDIEX *diex_in, RegEXMEM *exmem_out, WINDOW* stepInterface) {

    //mvwprintw(stepInterface, 19, 2, "========= ESTAGIO EX =========");
    mvwprintw(stepInterface, 7, 11, "%s", diex_in->assembly);
    //imprimeControle(diex_in->controle_DIEX, stepInterface, flagInterface);    

    *(exmem_out->controle_EXEMEM) = *(diex_in->controle_DIEX);
    strcpy(exmem_out->assembly, diex_in->assembly);

    Mux *muxop2 = criaMux(diex_in->RegB, diex_in->imm, 0, diex_in->controle_DIEX->srcB);
    int op2 = muxFuncition(muxop2);
    free(muxop2);
    //mvwprintw(stepInterface, 23, 2, "Operando 1 da ULA = [%d]\n", diex_in->RegA);
    //mvwprintw(stepInterface, 24, 2, "Operando 2 da ULA = [%d]\n", op2);
    exmem_out->resultULA = processamentoULA(diex_in->RegA, op2, diex_in->controle_DIEX->ulaOP);
    exmem_out->RegB = diex_in->RegB;
    //mvwprintw(stepInterface, 25, 2, "RESULTADO ULA = [Calc - %d | Over - %d | Comp - %d]\n", exmem_out->resultULA[0], exmem_out->resultULA[1], exmem_out->resultULA[2]);
    
    
    // MUX para o registrador de destino (RegDst)
    Mux *muxrd = criaMux(diex_in->rt, diex_in->rd, 0, diex_in->controle_DIEX->regDest);
    exmem_out->rd = muxFuncition(muxrd);
    //mvwprintw(stepInterface, 26, 2, "Registrador destino = [%d]\n", exmem_out->rd);
    free(muxrd);

    wrefresh(stepInterface);
}

// Estágio 4: Acesso à Memória
void estagio_MEM(RegEXMEM *exmem_in, RegMEMER *memer_out, struct memoria_dados *memDados, WINDOW* stepInterface) {

    //mvwprintw(stepInterface, 13, 2, "========= ESTAGIO MEM =========");
    mvwprintw(stepInterface, 9, 11,"%s", exmem_in->assembly);
    //imprimeControle(exmem_in->controle_EXEMEM, stepInterface, flagInterface);
   
    *(memer_out->controle_MEMER) = *(exmem_in->controle_EXEMEM);
    memer_out->resultULA = exmem_in->resultULA;
    memer_out->rd = exmem_in->rd;
    strcpy(memer_out->assembly, exmem_in->assembly);
    
    if (exmem_in->controle_EXEMEM->memWrite == 1) {
        //SWstepInterface);
        void estagio_EX(RegDIEX *diex_in, RegEXMEM *exmem_out, WINDOW* stepInterface);
        insereMemDados(memDados, exmem_in->resultULA[0], exmem_in->RegB, 1);
    } else if ((exmem_in->controle_EXEMEM->regWrite == 1) && (exmem_in->controle_EXEMEM->memReg == 0)) {
        //LW
        //mvwprintw(stepInterface, 17, 2, "Entrou no LW \n");
        memer_out->dado = getDado(memDados, exmem_in->resultULA[0]);
        //mvwprintw(stepInterface, 18, 2, "Dado buscado = [%d] \n", memer_out->dado);
    }

    wrefresh(stepInterface);
    
}

void estagio_ER(RegMEMER *memer_in, BRegs *bancoReg, WINDOW* stepInterface) {
    
    //mvwprintw(stepInterface, 5, 2, "========= ESTAGIO ER =========");
    //imprimeControle(memer_in->controle_MEMER, stepInterface, flagInterface);
    mvwprintw(stepInterface, 11, 11, "%s", memer_in->assembly);

    if (memer_in->controle_MEMER->regWrite == 1) {
        Mux *muxwb = criaMux(memer_in->dado, memer_in->resultULA[0], 0, memer_in->controle_MEMER->memReg);
        int dado_final = muxFuncition(muxwb);
        //mvwprintw(stepInterface, 9, 2, "dado buscado = [%d] \n", dado_final);
        free(muxwb); 
        salvaDadoReg(bancoReg, dado_final, memer_in->rd, 1);
    }

    //imprimeBanco(bancoReg, stepInterface);
    wrefresh(stepInterface);
}


void step(descPilha *pilha, int *contClock, int *pc, int *parada, RegALL *regIN, RegALL *regOUT, BRegs *bancoReg, struct memoria_instrucao *memInst, struct memoria_dados *memDados) {
    *contClock += 1;


    WINDOW* telaFetchStage = newwin(30, 30, 5, 5);
    wrefresh(telaFetchStage);

    WINDOW* telaDecodeStage = newwin(30, 30, 5, 40);
    wrefresh(telaDecodeStage);

    WINDOW* telaExecStage = newwin(30, 30, 5, 75);
    wrefresh(telaExecStage);

    WINDOW* telaMemStage = newwin(30, 30, 5, 110);
    wrefresh(telaMemStage);
    
    WINDOW* telaWriteStage = newwin(30, 30, 5, 145);
    wrefresh(telaWriteStage);


    WINDOW* telaBIDI = newwin(30, 5, 5, 35);
    box(telaBIDI, 0, 0);
    mvwprintw(stdscr, 3, 35, "IF/ID");
    wrefresh(telaBIDI);
    wrefresh(stdscr);

    WINDOW* telaDIEX = newwin(30, 5, 5, 70);
    box(telaDIEX, 0, 0);
    mvwprintw(stdscr, 3, 70, "ID/EX");
    wrefresh(telaDIEX);
    wrefresh(stdscr);

    WINDOW* telaEXMEM = newwin(30, 5, 5, 105);
    box(telaEXMEM, 0, 0);
    mvwprintw(stdscr, 3, 105, "EX/MEM");
    wrefresh(telaEXMEM);
    wrefresh(stdscr);

    WINDOW* telaMEMWD = newwin(30, 5, 5, 140);
    box(telaMEMWD, 0, 0);
    mvwprintw(stdscr, 3, 140, "MEM/WD");
    wrefresh(telaMEMWD);
    wrefresh(stdscr);

    //mvwprintw(stepInterface, 3, 2, "======== Inicio Step ========");
    //mvwprintw(stepInterface, 4, 2, "Cont Clock = [%d]", *contClock);
    //PILHA para StepBack
    BRegs * copiaBanco = copiaBancoRegistradores(bancoReg);
    struct memoria_dados * copiaMemDados = copiaMemoriaDados(memDados);
    RegALL * copiaRegIn = criaRegAll();
    RegALL * copiaRegOut = criaRegAll();
    copiaRegALL(copiaRegIn, regIN);
    copiaRegALL(copiaRegOut, regOUT);
    nodoPilha *novoNodo = criaNodo(*pc, copiaBanco, copiaMemDados, copiaRegIn, copiaRegOut);
    inserePilha(pilha, novoNodo);
    estagio_ER(regIN->MEMER, bancoReg, telaWriteStage);
    estagio_MEM(regIN->EXMEM, regOUT->MEMER, memDados, telaMemStage);
    estagio_EX(regIN->DIEX, regOUT->EXMEM, telaExecStage);

    wrefresh(telaWriteStage);
    wrefresh(telaMemStage);
    wrefresh(telaExecStage);

    if (regOUT->EXMEM->controle_EXEMEM->branch == 1 && regOUT->EXMEM->resultULA[2] == 1)
    {

        // BEQ → branch verdadeiro
        *pc = (regIN->DIEX->pc_incrementado) + regIN->DIEX->imm;
        regOUT->BIDI = criaRegBIDI(); // bolha em BI
        regOUT->DIEX = criaRegDIEX(); // bolha em DI
    }else
    {
        estagio_DI(regIN->BIDI, regOUT->DIEX, bancoReg, parada, telaDecodeStage);
        estagio_BI(pc, regOUT->BIDI, memInst, telaFetchStage);

        wrefresh(telaDecodeStage);
        wrefresh(telaFetchStage);

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
                //printf("Addr [%d]", regOUT->DIEX->addr);
                *pc = 0 + regOUT->DIEX->addr;
                regOUT->BIDI = criaRegBIDI(); // bolha em BI
                //printf("PC ----->> %d", *pc);
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


    WINDOW* inputInterface = newwin(6, 91, 38, (COLS/2)-52);
    box(inputInterface, 0, 0);
    mvwprintw(inputInterface, 1, 2, "---------------------------------------------------------------------------------------");
    mvwprintw(inputInterface, 3, (91 - (strlen("Pressinone qualquer tecla para continuar...")))/2,"Pressinone qualquer tecla para continuar...");
    wrefresh(inputInterface);
    wgetch(inputInterface);
    delwin(inputInterface);

    delwin(telaFetchStage);
    delwin(telaDecodeStage);
    delwin(telaExecStage);
    delwin(telaMemStage);
    delwin(telaWriteStage);
    delwin(telaBIDI);
    delwin(telaDIEX);
    delwin(telaEXMEM);
    delwin(telaMEMWD);

    clear();
    wrefresh(stdscr);

}


void stepRUN(descPilha *pilha, int *contClock, int *pc, int *parada, RegALL *regIN, RegALL *regOUT, BRegs *bancoReg, struct memoria_instrucao *memInst, struct memoria_dados *memDados) {
    *contClock += 1;
    BRegs * copiaBanco = copiaBancoRegistradores(bancoReg);
    struct memoria_dados * copiaMemDados = copiaMemoriaDados(memDados);
    RegALL * copiaRegIn = criaRegAll();
    RegALL * copiaRegOut = criaRegAll();
    copiaRegALL(copiaRegIn, regIN);
    copiaRegALL(copiaRegOut, regOUT);
    nodoPilha *novoNodo = criaNodo(*pc, copiaBanco, copiaMemDados, copiaRegIn, copiaRegOut);
    inserePilha(pilha, novoNodo);
    estagio_ER(regIN->MEMER, bancoReg, NULL);
    estagio_MEM(regIN->EXMEM, regOUT->MEMER, memDados, NULL);
    estagio_EX(regIN->DIEX, regOUT->EXMEM, NULL);
    if (regOUT->EXMEM->controle_EXEMEM->branch == 1 && regOUT->EXMEM->resultULA[2] == 1)
    {

        // BEQ → branch verdadeiro
        *pc = (regIN->DIEX->pc_incrementado) + regIN->DIEX->imm;
        regOUT->BIDI = criaRegBIDI(); // bolha em BI
        regOUT->DIEX = criaRegDIEX(); // bolha em DI
    }else
    {
        estagio_DI(regIN->BIDI, regOUT->DIEX, bancoReg, parada, NULL);
        estagio_BI(pc, regOUT->BIDI, memInst, NULL);
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
                //printf("Addr [%d]", regOUT->DIEX->addr);
                *pc = 0 + regOUT->DIEX->addr;
                regOUT->BIDI = criaRegBIDI(); // bolha em BI
                //printf("PC ----->> %d", *pc);
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
}
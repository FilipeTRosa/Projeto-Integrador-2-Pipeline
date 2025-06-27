#include "memoria.h"
#include "minimips.h"
#include "controle.h"
#include "step.h"
#include "pipeline.h"
#include "multiplexadores.h"
#include "decodificador.h"
#include "memoriaDados.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>



RegBIDI* criaRegBIDI() {
    RegBIDI* new_reg = (RegBIDI*)malloc(sizeof(RegBIDI));
    
    new_reg->IR = criaIR();
    new_reg->pc_incrementado = 0;

    return new_reg;
}

RegDIEX* criaRegDIEX(){
    RegDIEX* new_reg = (RegDIEX*)malloc(sizeof(RegDIEX));

    strcpy(new_reg->assembly, "NOP");
    strcpy(new_reg->binario, "0000000000000000");
    new_reg->controle_DIEX = criaControle();
    new_reg->RegA = 0;
    new_reg->RegB = 0;
    new_reg->imm = 0;
    new_reg->addr = 0;
    new_reg->rt = 0;
    new_reg->rd = 0;
    new_reg->rs = 0;
    new_reg->pc_incrementado = 0;
    
    return new_reg;
}

RegEXMEM* criaRegEXMEM(){
    RegEXMEM* new_reg = (RegEXMEM*)malloc(sizeof(RegEXMEM));

    strcpy(new_reg->assembly, "NOP");
    new_reg->controle_EXEMEM = criaControle();
    new_reg->resultULA = 0;
    new_reg->RegB = 0;
    new_reg->rd = 0;
    
    return new_reg;
}

RegMEMER* criaRegMEMER(){
    RegMEMER* new_reg = (RegMEMER*)malloc(sizeof(RegMEMER));

    strcpy(new_reg->assembly, "NOP");
    new_reg->controle_MEMER = criaControle();
    new_reg->resultULA = 0;
    new_reg->dado = 0;
    new_reg->rd = 0;
    
    return new_reg;
}

struct instrucao criaIR(){
    struct instrucao inst;
    char copyAssembly[50];

    inst.tipo_inst = tipo_OUTROS;
    inst.inst_char[0] = '\0';
    strcpy(copyAssembly, "NOP");
    strcpy(inst.assembly, copyAssembly);
    inst.opcode = 0;
    inst.rs = 0;
    inst.rt = 0;
    inst.rd = 0;
    inst.funct = 0;
    inst.imm = 0;
    inst.addr = 0;
    return inst;
}

RegALL* criaRegAll(){
    RegALL* new_reg = (RegALL*)malloc(sizeof(RegALL));
    // RegMEMER* new_memer = criaRegMEMER();
    // RegEXMEM* new_exmem = criaRegEXMEM();
    // RegDIEX* new_diex = criaRegDIEX();
    // RegBIDI* new_bidi = criaRegBIDI();

    new_reg->BIDI = criaRegBIDI();
    new_reg->DIEX = criaRegDIEX();
    new_reg->EXMEM = criaRegEXMEM(); 
    new_reg->MEMER = criaRegMEMER(); 

    return new_reg;
}

void copiaRegBIDI(RegBIDI *in, RegBIDI *out) {
    *in = *out;
}

void copiaRegDIEX(RegDIEX *in, RegDIEX *out) {
    *(in->controle_DIEX) = *(out->controle_DIEX);
    //(in->binario, out->binario);
    in->RegA = out->RegA;
    in->RegB = out->RegB;
    in->imm = out->imm;
    in->addr = out->addr;
    in->rt = out->rt;
    in->rd = out->rd;
    in->rs = out->rs;
    in->pc_incrementado = out->pc_incrementado;
}

void copiaRegEXMEM(RegEXMEM *in, RegEXMEM *out) {
    *(in->controle_EXEMEM) = *(out->controle_EXEMEM);
    in->resultULA = out->resultULA;
    in->RegB = out->RegB;
    in->rd = out->rd;
}

void copiaRegMEMER(RegMEMER *in, RegMEMER *out) {
    *(in->controle_MEMER) = *(out->controle_MEMER);
    in->resultULA = out->resultULA;
    in->dado = out->dado;
    in->rd = out->rd;
}

void copiaRegALL(RegALL *in, RegALL *out) {
    copiaRegBIDI(in->BIDI, out->BIDI);
    copiaRegDIEX(in->DIEX, out->DIEX);
    copiaRegEXMEM(in->EXMEM, out->EXMEM);
    copiaRegMEMER(in->MEMER, out->MEMER);
}


// FUNCOES PARA IMPRIMIR REGs
void imprimeBIDI(struct Registrador_BIDI b) {
    printf("[BIDI] PC: [%d] | ", b.pc_incrementado);
    //imprimeInstrucao(b.IR);
}

void imprimeDIEX(struct Registrador_DIEX d) {
    printf("[DIEX] ASM: [%s]  A: [%d] B: [%d] IMM: [%d] rt: [%d] rd: [%d] PC: [%d] | ",
        d.assembly, d.RegA, d.RegB,d.imm, d.rt, d.rd, d.pc_incrementado);
    if (d.controle_DIEX) imprimeControle(d.controle_DIEX, NULL, 0);
    else printf("CTRL: NULL\n");
}

void imprimeEXMEM(struct Registrador_EXMEM e) {
    printf("[EXMEM] ASM: [%s] rd: [%d] B: [%d] | ", e.assembly, e.rd, e.RegB);
    if (e.resultULA)
        printf("ULA: [%d,%d,%d] | ", e.resultULA[0], e.resultULA[1], e.resultULA[2]);
    else
        printf("ULA: NULL | ");
    if (e.controle_EXEMEM) imprimeControle(e.controle_EXEMEM, NULL, 0);
    else printf("CTRL: NULL\n");
}

void imprimeMEMER(struct Registrador_MEMER m) {
    printf("[MEMER] ASM: [%s] rd: [%d] Dado: [%d] | ", m.assembly ,m.rd, m.dado);
    if (m.resultULA)
        printf("ULA: [%d,%d,%d] | ", m.resultULA[0], m.resultULA[1], m.resultULA[2]);
    else
        printf("ULA: NULL | ");
    if (m.controle_MEMER) imprimeControle(m.controle_MEMER, NULL, 0);
    else printf("CTRL: NULL\n");
}

// FUNCAO QUE IMPRIME O PIPELINE

void imprimePipeline(
    int ciclo,
    struct Registrador_BIDI *bidi,
    struct Registrador_DIEX *diex,
    struct Registrador_EXMEM *exmem,
    struct Registrador_MEMER *memer
) {
    printf("\n======= CICLO %d =======\n", ciclo);

    printf("BI/DI   → ");
    imprimeBIDI(*bidi);

    printf("\nDI/EX   → ");
    imprimeDIEX(*diex);

    printf("\nEX/MEM  → ");
    imprimeEXMEM(*exmem);

    printf("\nMEM/ER  → ");
    imprimeMEMER(*memer);

    printf("========================\n");
}




//////// UNIDADE DE HAZARD /////// Depois ver se precisa mudar para uma outra TAD

int detectaDataHazard(RegDIEX *diex, struct instrucao instBuscada) {
    //(diex->controle_DIEX->regWrite == 1 && diex->controle_DIEX->memReg == 0) --- LW ---
    if ((diex->controle_DIEX->regWrite == 1 && diex->controle_DIEX->memReg == 0) && diex->rd != 0) {
        if (diex->rd == instBuscada.rs || diex->rd == instBuscada.rt) {
            return 1;  // Hazard detectado
        }
    }
    return 0;
}

int unidadeDeHazard(RegALL *regIN, RegALL *regOUT){

    if (isNOP(regOUT->DIEX->rs, regOUT->DIEX->rt, regOUT->DIEX->rd))
    {
        //printf("VAZIO ou NULLO\n");
        return 0;
    }else{
        //hazard dados para SW -
        if (regOUT->DIEX->controle_DIEX->memWrite == 1)
        {
            if (regOUT->DIEX->rt != 0)
            {
                //    -- teste 1 tipo r no EX --             --- teste 2 tipoR no ER--                       
                if((regOUT->EXMEM->rd == regOUT->DIEX->rt) || (regOUT->MEMER->rd == regOUT->DIEX->rt)){
                    //printf("Hazard Detectado --- SW ---\n");
                    return 1;
                }
                //tem LW no EX                                             rd do lw == rd do sw
                if (isLW(regOUT->EXMEM->controle_EXEMEM) && (regOUT->EXMEM->rd == regOUT->DIEX->rt))
                {
                    //printf("Hazard Detectado --- SW ---\n");
                    return 1;
                }
                //tem ADDI no EX ou no ER
                if (isADDI(regOUT->EXMEM->controle_EXEMEM) || isADDI(regOUT->MEMER->controle_MEMER))
                {
                    //agora tenho que testar o RT
                    if (regOUT->EXMEM->rd == regOUT->DIEX->rt || regOUT->MEMER->rd == regOUT->DIEX->rt)
                    {
                        //printf("Hazard Detectado --- SW ---\n");
                        return 1;
                    } 
                }
            } 
        }

        //--- LW ---
        if (isLW(regOUT->DIEX->controle_DIEX) && (regOUT->DIEX->rs != 0)){
            //printf("Entrou na U-H para LW\n");
            if (regOUT->DIEX->rs != 0)
            {
                if ((regOUT->EXMEM->rd == regOUT->DIEX->rs) || (regOUT->MEMER->rd == regOUT->DIEX->rs)) {
                //printf("Hazard Detectado --- LW ---\n");
                return 1;  // Hazard detectado
                }
            }
        }

        //hazard dados para TipoR
        if(isTipoR(regOUT->DIEX->controle_DIEX)){
            //printf("Entrou na U-H para TipoR\n");
            int rs = regOUT->DIEX->rs;
            int rt = regOUT->DIEX->rt;
            
            if (rs != 0)
            {
                if ((regOUT->EXMEM->rd == rs) || (regOUT->MEMER->rd == rs))
                {
                    //printf("Hazard Detectado --- TipoR ---\n");
                    return 1;
                }
            }
            if (rt != 0)
            {
                if ((regOUT->EXMEM->rd == rt) || (regOUT->MEMER->rd == rt))
                {
                    //printf("Hazard Detectado --- TipoR ---\n");
                    return 1;
                }
            }
        }

        //ADDI
        if (isADDI(regOUT->DIEX->controle_DIEX))
        {
            if (regOUT->DIEX->rs != 0)
            {
                if ((regOUT->EXMEM->rd == regOUT->DIEX->rs) || (regOUT->MEMER->rd == regOUT->DIEX->rs))
                {
                    //printf("Hazard Detectado --- ADDI ---\n");
                    return 1;
                }  
            }   
        }
        
        //BEQ
        if (regOUT->DIEX->controle_DIEX->branch == 1)
        {
            int rs = regOUT->DIEX->rs;
            int rt = regOUT->DIEX->rt;

             if (rs != 0)
            {
                if ((regOUT->EXMEM->rd == rs) || (regOUT->MEMER->rd == rs))
                {
                    //printf("Hazard Detectado --- BEQ ---\n");
                    return 1;
                }
            }
            if (rt != 0)
            {
                if ((regOUT->EXMEM->rd == rt) || (regOUT->MEMER->rd == rt))
                {
                    //printf("Hazard Detectado BEQ\n");
                    return 1;
                }
            }
            
        }
    }
    return 0;
}
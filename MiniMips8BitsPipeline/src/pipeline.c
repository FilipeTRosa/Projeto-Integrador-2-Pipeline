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
#include <pipeline.h>


RegBIDI* criaRegBIDI() {
    RegBIDI* new_reg = (RegBIDI*)malloc(sizeof(RegBIDI));
    
    new_reg->IR = criaIR();
    new_reg->pc_incrementado = 0;

    return new_reg;
}

RegDIEX* criaRegDIEX(){
    RegDIEX* new_reg = (RegDIEX*)malloc(sizeof(RegDIEX));

    new_reg->controle_DIEX = criaControle();
    new_reg->RegA = 0;
    new_reg->RegB = 0;
    new_reg->imm = 0;
    new_reg->rt = 0;
    new_reg->rd = 0;
    new_reg->pc_incrementado = 0;
    
    return new_reg;
}

RegEXMEM* criaRegEXMEM(){
    RegEXMEM* new_reg = (RegEXMEM*)malloc(sizeof(RegEXMEM));

    new_reg->controle_EXEMEM = criaControle();
    new_reg->resultULA = 0;
    new_reg->RegB = 0;
    new_reg->rd = 0;
    
    return new_reg;
}

RegMEMER* criaRegMEMER(){
    RegMEMER* new_reg = (RegMEMER*)malloc(sizeof(RegMEMER));

    new_reg->controle_MEMER = criaControle();
    new_reg->resultULA = 0;
    new_reg->dado = 0;
    new_reg->rd = 0;
    
    return new_reg;
}

struct instrucao criaIR(){
    struct instrucao inst;

    inst.tipo_inst = tipo_OUTROS;
    inst.inst_char[0] = '\0';
    inst.assembly[0] = '\0';
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

    new_reg->reg_bidi = criaRegBIDI();
    new_reg->reg_diex = criaRegDIEX();
    new_reg->reg_exmem = criaRegEXMEM(); 
    new_reg->reg_memer = criaRegMEMER(); 

    return new_reg;
}
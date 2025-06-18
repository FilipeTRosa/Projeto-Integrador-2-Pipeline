#ifndef PIPELINE_H
#define PIPELINE_H
#include "controle.h"
#include "memoria.h"

typedef struct Registrador_BIDI RegBIDI;
typedef struct Registrador_DIEX RegDIEX;
typedef struct Registrador_EXMEM RegEXMEM;
typedef struct Registrador_MEMER RegMEMER;
typedef struct RegistradoresALL RegALL;

struct Registrador_BIDI {
    struct instrucao IR;
    int pc_incrementado;
    //ADICIONAR ASSEMBLY PARA VIZUALIZAR INSTRUCAO
};

struct Registrador_DIEX {
    //ADICIONAR ASSEMBLY
    char assembly[50];
    CTRL * controle_DIEX; //arrumar questao do ponteiro   
    int RegA;
    int RegB;
    int imm;
    int addr;
    int rt;
    int rd;
    int pc_incrementado;
};

struct Registrador_EXMEM{
    //assembly
    char assembly[50];
    CTRL * controle_EXEMEM;
    int * resultULA;
    int RegB;
    int rd;
    
};

struct Registrador_MEMER{
    //assembly
    char assembly[50];
    CTRL * controle_MEMER;
    int * resultULA;
    int dado;
    int rd;
};

struct RegistradoresALL {
    RegBIDI *BIDI;
    RegDIEX *DIEX;
    RegEXMEM *EXMEM;
    RegMEMER *MEMER;
};


RegALL* criaRegAll();
RegBIDI* criaRegBIDI();
RegDIEX* criaRegDIEX();
RegEXMEM* criaRegEXMEM();
RegMEMER* criaRegMEMER();
struct instrucao criaIR();

void copiaRegBIDI(RegBIDI *out, RegBIDI *in);
void copiaRegDIEX(RegDIEX *out, RegDIEX *in);
void copiaRegEXMEM(RegEXMEM *out, RegEXMEM *in);
void copiaRegMEMER(RegMEMER *out, RegMEMER *in);
void copiaRegALL(RegALL *out, RegALL *in);


// IMPRESSÔES DOS REGISTRADORES //
void imprimeBIDI(struct Registrador_BIDI b);
void imprimeDIEX(struct Registrador_DIEX d);
void imprimeEXMEM(struct Registrador_EXMEM e);
void imprimeMEMER(struct Registrador_MEMER m);
void imprimePipeline(
    int ciclo,
    struct Registrador_BIDI *bidi,
    struct Registrador_DIEX *diex,
    struct Registrador_EXMEM *exmem,
    struct Registrador_MEMER *memer
);

// TRATAMENTO DE HAZARD //
int detectaDataHazard(RegDIEX *diex, struct instrucao instrucao_ID);



#endif 
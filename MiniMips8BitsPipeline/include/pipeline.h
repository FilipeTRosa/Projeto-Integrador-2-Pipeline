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
    CTRL *controle_DIEX; //arrumar questao do ponteiro   
    int RegA;
    int RegB;
    int imm;
    int rt;
    int rd;
    int pc_incrementado;
};

struct Registrador_EXMEM{
    //assembly
    CTRL *controle_EXEMEM;
    int resultULA;
    int RegB;
    int rd;
    
};

struct Registrador_MEMER{
    //assembly
    CTRL *controle_MEMER;
    int resultULA;
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
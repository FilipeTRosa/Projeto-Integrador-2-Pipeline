#ifndef CONTROLE_H
#define CONTROLE_H

typedef struct controle CTRL;

struct controle {
    int regDest;
    int srcB;
    int memReg;
    int ulaOP;
    int memWrite;
    int regWrite;
    int branch;
    int jump;
};


CTRL* criaControle();
//void setSignal(CTRL* control, int opcode, int funct);
void setSignal(CTRL* control, int opcode, int funct);
void imprimeControle(CTRL *controle);
void nextState(int *state, int opcode, int funct);
int isLW(CTRL *controle);
int isADDI(CTRL *controle);
int isTipoR(CTRL * controle);
#endif 
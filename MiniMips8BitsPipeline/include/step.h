#include "pipeline.h"

void estagio_BI(int *pc, RegBIDI *bidi_out, struct memoria_instrucao *memInst);
void estagio_DI(RegBIDI *bidi_in, RegDIEX *diex_out, BRegs *bancoReg, int *parada);
void estagio_EX(RegDIEX *diex_in, RegEXMEM *exmem_out);
void estagio_MEM(RegEXMEM *exmem_in, RegMEMER *memer_out, struct memoria_dados *memDados);
void estagio_ER(RegMEMER *memer_in, BRegs *bancoReg);
void step(descPilha *pilha, int *contClock, int *pc, int *parada, RegALL *regIN, RegALL *regOUT, BRegs *bancoReg, struct memoria_instrucao *memInst, struct memoria_dados *memDados);
//void step(int *parada, int *pc, struct memoria_dados *memDados, struct memoria_instrucao *memInst, BRegs *bancoReg, CTRL *controle, descPilha *pilha, struct estatistica * stat);

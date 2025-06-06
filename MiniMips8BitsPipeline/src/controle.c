#include "memoria.h"
#include "controle.h"
#include "decodificador.h"
#include "multiplexadores.h"
#include "minimips.h"
#include "step.h"
#include "memoriaDados.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>


CTRL* criaControle() {
    CTRL *new_controle = (CTRL *)malloc(sizeof(CTRL));
    new_controle->regDest = 0;
    new_controle->srcB = 0;
    new_controle->memReg = 0;
    new_controle->ulaOP = 0;
    new_controle->memWrite = 0;
    new_controle->regWrite = 0;
    new_controle->branch = 0;
    new_controle->jump = 0;

    return new_controle;
}


void setSignal(CTRL* control, int opcode, int funct) {
    
    switch(opcode) {
        case 0: //TIPO R
            if(funct == 0) { //ADD
                control->ulaOP = 0;
            }
            else if(funct == 2) {//SUB
                control->ulaOP = 2;
            }
            else if(funct == 4) {//AND
                control->ulaOP = 4;
            }
            else if (funct == 5) {//OR
                control->ulaOP = 5;
            }

            control->regDest = 1;
            control->srcB = 0;
            control->memReg = 0;
            control->memWrite = 0;
            control->regWrite = 1;
            control->branch = 100000;
            break;
        case 2://jump
            control->regDest = 0;
            control->srcB = 0;
            control->memReg = 0;
            control->memWrite = 0;
            control->regWrite = 0;
            control->branch = 0;
            control->ulaOP = 0;
            
            break;
        case 4: //ADDI
            control->regDest = 0;
            control->srcB = 1;
            control->memReg = 1;
            control->memWrite = 0;
            control->regWrite = 1;
            control->branch = 1000000000;
            control->ulaOP = 1;
            break;
        case 8: //BEQ
            control->regDest = 0;
            control->srcB = 0;
            control->memReg = 0;
            control->memWrite = 0;
            control->regWrite = 0;
            control->branch = 1;
            control->ulaOP = 6;
            break;
        case 11://LW
            control->regDest = 0;
            control->srcB = 1;
            control->memReg = 0;
            control->memWrite = 0;
            control->regWrite = 1;
            control->branch = 0;
            control->ulaOP = 3; //o certo seria 3 mas ta dando conflito na ULA com o case 3

            break;
        case 15://SW
            control->regDest = 0;
            control->srcB = 1;
            control->memReg = 0;
            control->memWrite = 1;
            control->regWrite = 0;
            control->branch = 0;
            control->ulaOP = 7;
            break;
        default:
            printf("\nOpcode nao pertence ao conjunto de instrucoes!!\n");
    }

}


/*void imprimeControle(CTRL *controle){
    printf("\nControle\n");
    printf("RegDst: [%d], ULAFonteA: [%d], ULAFonteB: [%d], MemParaReg: [%d], ULAControle: [%d], EscMem: [%d]\n",
        controle->RegDst, controle->ULAFonteA, controle->ULAFonteB, controle->MemParaReg, controle->ULAControle, controle->EscMem);
    printf("EscReg: [%d], branch: [%d], IouD: [%d], IREsc: [%d], PCEsc: [%d], PCFonte: [%d]\n",
        controle->EscReg, controle->branch, controle->IouD, controle->IREsc, controle->PCEsc, controle->PCFonte);
}
*/

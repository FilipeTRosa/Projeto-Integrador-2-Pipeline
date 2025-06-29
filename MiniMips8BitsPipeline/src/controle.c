#include "pipeline.h"
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
#include <ncurses.h>


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
            control->memReg = 1;
            control->memWrite = 0;
            control->regWrite = 1;
            control->branch = 0;
            control->jump = 0;
            break;
        case 2://jump
            control->regDest = 0;
            control->srcB = 0;
            control->memReg = 0;
            control->memWrite = 0;
            control->regWrite = 0;
            control->branch = 0;
            control->ulaOP = 0;
            control->jump = 1;
            break;
        case 4: //ADDI
            control->regDest = 0;
            control->srcB = 1;
            control->memReg = 1;
            control->memWrite = 0;
            control->regWrite = 1;
            control->branch = 0;
            control->ulaOP = 1;
            control->jump = 0;
            break;
        case 8: //BEQ
            control->regDest = 0;
            control->srcB = 0;
            control->memReg = 0;
            control->memWrite = 0;
            control->regWrite = 0;
            control->branch = 1;
            control->ulaOP = 6;
            control->jump = 0;
            break;
        case 11://LW
            control->regDest = 0;
            control->srcB = 1;
            control->memReg = 0;
            control->memWrite = 0;
            control->regWrite = 1;
            control->branch = 0;
            control->ulaOP = 7; //o certo seria 3 mas ta dando conflito na ULA com o case 3
            control->jump = 0;
            break;
        case 15://SW
            control->regDest = 0;
            control->srcB = 1;
            control->memReg = 0;
            control->memWrite = 1;
            control->regWrite = 0;
            control->branch = 0;
            control->ulaOP = 7;
            control->jump = 0;
            break;
        default:
            printf("\nOpcode nao pertence ao conjunto de instrucoes!!\n");
    }

}


void imprimeControle(CTRL *controle, WINDOW* stepInterface, int flagInterface){

    int linha = 0;

    if(flagInterface == 1) {
        linha = 6;
    }
    else if(flagInterface == 2) {
        linha = 15;
    }
    else if(flagInterface == 3) {
        linha = 21;
    }
    else if(flagInterface == 4) {
        linha = 29;
    }

    mvwprintw(stepInterface, linha, 2, "Controle"); //ta faltando incPC
    mvwprintw(stepInterface, (linha+1), 2, "RegDst: [%d], ULAFonte [%d], MemParaReg: [%d], ULAControle: [%d], EscMem: [%d]\n",
        controle->regDest, controle->srcB ,controle->memReg, controle->ulaOP, controle->memWrite);
    mvwprintw(stepInterface, 7, 2, "EscReg: [%d], branch: [%d], jump [%d]\n",
        controle->regWrite, controle->branch, controle->jump);
    wrefresh(stepInterface);
}

int isLW(CTRL *controle){
    if (controle->regWrite == 1 && controle->memReg == 0)
    {
        return 1;
    }
    return 0;
}

int isADDI(CTRL *controle){
    if (controle->regDest == 0 && controle->regWrite == 1 && controle->memReg == 1)
    {
        return 1;
    }
    return 0;
}

int isTipoR(CTRL * controle){
    if (controle->regDest == 1 && controle->regWrite == 1 && controle->memReg == 1)
    {
        return 1;
    }
    return 0;
}
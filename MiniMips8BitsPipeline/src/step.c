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

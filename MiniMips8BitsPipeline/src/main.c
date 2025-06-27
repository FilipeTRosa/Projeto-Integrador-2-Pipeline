#include "memoria.h"
#include "memoriaDados.h"
#include "minimips.h"
#include "controle.h"
#include "pipeline.h"
#include "multiplexadores.h"
#include "decodificador.h"
#include "step.h"
#include "interface.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ncurses.h>



int main(int argc, char const *argv[])
{

    // Configurações da interface //
    
    initscr();
    cbreak();
    noecho();
    WINDOW* inputInterface;

    //Alocando memoria de instrução
    struct memoria_instrucao mem;
    mem.mem_inst = (struct instrucao *)malloc(256 *sizeof(struct instrucao));
    mem.tamanho = 256;
    char arquivoMemInstrucoes[256];
    //Fim alocação de memoria de instrucao

    //Alocando memoria de dados
    struct memoria_dados memDados;
    memDados.mem_dados = (struct dado*)malloc(256 *sizeof(struct dado));
    memDados.tamanho = 256;
    char arquivoMemDados[50];
    //Fim alocação de memoria de dados

    // pipeline
    RegALL *RegIN = criaRegAll();
    RegALL *RegOUT = criaRegAll();
    int contClock = 0;
    


    //
    struct estatistica *stat = NULL;
    stat = criaStat();
    //

    //Variaveis do conversor de bin
    //int dec;
    //int comp2 = 1;
    //Fim conversor
    
    //Variaveis de teste
    //char palavra[17];
    //FIm testes

    // REGISTRADORES TEMPORAIS //
    //int RegA = 0;
    //int RegB = 0;
    //RegINST *regIR = criaRegIR();
    //RegMDR *regMDR = criaRegMDR();
    //ULAsaida *regSaidaULA = criaRegSaidaULA();

    //Configuracao de variaveis do sistema
    int menu = 0;
    int pc = 0;
    //int operando2;
    //int* buscaReg = NULL;
    char arquivoAsm[50];
    int parada = 1;
    //int estadoControle = 0;
    //Fim sistema

    descPilha* pilha = criarPilha();

    // CRIANDO CONTROLE //

    //CTRL *controle = NULL;
    //controle = criaControle();

    // CRIANDO BANCO DE REGISTRADORES //

    regs *reg = NULL;
    BRegs *bancoRegistradores = NULL;

    bancoRegistradores = alocaBancoRegistradores();

    for (int i = 0; i <= 8; i++) {
        reg = criaRegistrador();
        criaBanco(bancoRegistradores, reg);
    }

    // TESTANDO BUSCA NO BANCO DE REGISTRADORES //

    //int *vetBusca = NULL;

    // TESTANDO A UNIDADE LOGICA ARITMETICA //

    //int *resultadoULA = NULL;
    //resultadoULA = processamentoULA(vetBusca[0], vetBusca[1], 0);

    //Fim config do sistema

    do {
        
        menu = menuInterface();

        switch (menu) {
            case 1:
                inputInterface = newwin(6, 91, 11, (COLS/2)-43);
                box(inputInterface, 0, 0);
                mvwprintw(inputInterface, 2, (91-(strlen("Digite o nome do arquivo .asm")))/2, "Digite o nome do arquivo .asm\n\n\t\t\t\t\t");
                wrefresh(inputInterface);

                keypad(inputInterface, TRUE);
                echo();
                wgetnstr(inputInterface, arquivoMemInstrucoes, 255);
                delwin(inputInterface);
                noecho();
                carregarInstrucoes(arquivoMemInstrucoes, &mem);
                clear();
                wrefresh(stdscr);
                break;
            case 2:
                inputInterface = newwin(6, 91, 11, (COLS/2)-43);
                box(inputInterface, 0, 0);
                mvwprintw(inputInterface, 2, (91-(strlen("Digite o nome do arquivo .dat\n")))/2, "Digite o nome do arquivo .dat\n\n\t\t\t\t\t");
                wrefresh(inputInterface);

                keypad(inputInterface, TRUE);
                echo();
                wgetnstr(inputInterface, arquivoMemDados, 255);
                delwin(inputInterface);
                noecho();
                carregarDados(arquivoMemDados, &memDados);
                clear();
                wrefresh(stdscr);
                break;
            
            case 3:

                int select = 1;
                int optionSelect = -1;
                int ch;
                int runFlag = -1;

                inputInterface = newwin(6, 91, 11, (COLS/2)-43);
                box(inputInterface, 0, 0);
                keypad(inputInterface, TRUE);

                do {
                    
                    menuButton(inputInterface, 2, (91-(strlen("Instrucao")))/2, "Instrucoes", (select==1));
                    menuButton(inputInterface, 4, (91-(strlen("Dados")))/2, "Dados", (select==2));
                    wrefresh(inputInterface);

                    ch = wgetch(inputInterface);
                    
                    switch(ch) {
                        case KEY_UP:
                            if(select > 1) {
                                select--;
                            }
                            break;
                        case KEY_DOWN:
                            if(select < 2) {
                                select++;
                            }
                            break;
                        case '\n':
                            optionSelect = select;
                            delwin(inputInterface);
                            clear();
                            runFlag = 0;
                            wrefresh(stdscr);
                            break;
                    }

                }while(runFlag == -1);

                switch(optionSelect) {
                    case 1:
                        clear();
                        wrefresh(stdscr);
                        imprimeMemInstrucoes(stdscr, &mem);
                        wgetch(stdscr);
                        clear();
                        wrefresh(stdscr);                       
                        break;
                    case 2:
                        clear();
                        wrefresh(stdscr);
                        imprimeMemDados(stdscr, &memDados);
                        wgetch(stdscr);
                        clear();
                        wrefresh(stdscr); 
                        break;
                    }
                break;
            
            case 4:
                int selectB = 1;
                int optionSelectB = -1;
                int chB;
                int runFlagB = -1;

                inputInterface = newwin(6, 91, 11, (COLS/2)-43);
                box(inputInterface, 0, 0);
                keypad(inputInterface, TRUE);

                do {
                    
                    menuButton(inputInterface, 2, (91-(strlen("Banco de Registradores")))/2, "Banco de Registradores", (selectB==1));
                    menuButton(inputInterface, 4, (91-(strlen("Registradores de Pipeline")))/2, "Registradores de Pipeline", (selectB==2));
                    wrefresh(inputInterface);

                    chB = wgetch(inputInterface);
                    
                    switch(chB) {
                        case KEY_UP:
                            if(selectB > 1) {
                                selectB--;
                            }
                            break;
                        case KEY_DOWN:
                            if(selectB < 2) {
                                selectB++;
                            }
                            break;
                        case '\n':
                            optionSelectB = selectB;
                            delwin(inputInterface);
                            clear();
                            runFlagB = 0;
                            wrefresh(stdscr);
                            break;
                    }

                }while(runFlagB == -1);

                switch(optionSelectB) {
                    case 1:
                        clear();
                        wrefresh(stdscr);
                        imprimeBanco(bancoRegistradores);
                        clear();
                        wrefresh(stdscr);                       
                        break;
                    case 2:
                        clear();
                        wrefresh(stdscr);
                        break;
                    }
                break;
            case 5:
                inputInterface = newwin(6, 91, 11, (COLS/2)-43);
                box(inputInterface, 0, 0);
                mvwprintw(inputInterface, 2, (91-(strlen("Digite o nome do arquivo para salvar\n")))/2, "Digite o nome do arquivo para salvar\n\n\t\t\t\t");
                wrefresh(inputInterface);

                keypad(inputInterface, TRUE);
                echo();
                wgetnstr(inputInterface, arquivoAsm, 49);
                delwin(inputInterface);
                noecho();
                salvarAsm(arquivoAsm, &mem);
                clear();
                wrefresh(stdscr);
                break;
            case 6:
                inputInterface = newwin(6, 91, 11, (COLS/2)-43);
                box(inputInterface, 0, 0);
                mvwprintw(inputInterface, 2, (91-(strlen("Digite o nome do arquivo para salvar\n")))/2, "Digite o nome do arquivo para salvar\n\n\t\t\t\t");
                wrefresh(inputInterface);

                keypad(inputInterface, TRUE);
                echo();
                wgetnstr(inputInterface, arquivoMemDados, 49);
                delwin(inputInterface);
                noecho();
                salvarMemoriaEmArquivo(arquivoMemDados, &memDados);
                clear();
                wrefresh(stdscr);
                break;
            case 7:
                while (parada)
                {
                    stepRUN(pilha, &contClock, &pc, &parada, RegIN, RegOUT, bancoRegistradores, &mem, &memDados);
                }
                break;
            case 8:

                    //step(&parada, &pc, &mem, bancoRegistradores, controle, pilha, stat, &estadoControle, &regSaidaULA->resultULA, regMDR, &RegA, &RegB, regIR);
                    // step(&parada, &pc, &memDados, &mem, bancoRegistradores, controle, pilha, stat
                    clear();
                    wrefresh(stdscr);

                    step(pilha, &contClock, &pc, &parada, RegIN, RegOUT, bancoRegistradores, &mem, &memDados);
                    RegIN = RegOUT;

                    wrefresh(stdscr);

                break;
            case 9:
                nodoPilha *voltaInstrucao = removePilha(pilha);
                if (voltaInstrucao != NULL)
                {
                    stat->back++;
                    bancoRegistradores = voltaInstrucao->bancoRegs;
                    pc = voltaInstrucao->pc;
                    memDados = *voltaInstrucao->memoriaDados;

                    WINDOW* inputInterface = newwin(12, 91, 11, (COLS/2)-43);
                    box(inputInterface, 0, 0);
                    mvwprintw(inputInterface, 1, 2, "---------------------------------------------------------------------------------------");
                    mvwprintw(inputInterface, 2, (91-(strlen("STEP BACK")))/2, "STEP BACK");
                
                    mvwprintw(inputInterface, 3, (91-(strlen("Reconfigurando o simulador...")))/2, "Reconfigurando o simulador...");
                    mvwprintw(inputInterface, 5, 2, "->PC: [%d]",pc);
                    mvwprintw(inputInterface, 6, 2, "->Instrução: [%s]", buscaInstrucao(&mem, pc).assembly);
                    mvwprintw(inputInterface, 8, 2,"Pressinone qualquer tecla para continuar...");
                    wrefresh(inputInterface);
                    wgetch(inputInterface);
                    delwin(inputInterface);
                    delwin(inputInterface);
                    wrefresh(stdscr);
                }
                break;
            case 10: 
                endwin();
                break;
        }
    } while (menu != 10);
    
    free(mem.mem_inst);
    return 0;
}

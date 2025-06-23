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
                inputInterface = newwin(6, 95, 10, (COLS/2)-45);
                box(inputInterface, 0, 0);
                mvwprintw(inputInterface, 2, (95-(strlen("Digite o nome do arquivo .asm\n")))/2, "Digite o nome do arquivo .asm\n\n");
                wrefresh(inputInterface);

                keypad(inputInterface, TRUE);
                echo();
                wgetnstr(inputInterface, arquivoMemInstrucoes, 255);
                delwin(inputInterface);
                noecho();
                carregarInstrucoes(arquivoMemInstrucoes, &mem);
                break;
            case 2:
                inputInterface = newwin(6, 95, 10, (COLS/2)-45);
                box(inputInterface, 0, 0);
                mvwprintw(inputInterface, 2, (95-(strlen("Digite o nome do arquivo .dat\n")))/2, "Digite o nome do arquivo .dat\n\n");
                wrefresh(inputInterface);

                keypad(inputInterface, TRUE);
                echo();
                wgetnstr(inputInterface, arquivoMemDados, 255);
                delwin(inputInterface);
                noecho();
                carregarDados(arquivoMemDados, &memDados); 
                break;
            /*
            case 3:
                //imprime memorias
                system("clear");
                imprimeMemInstrucoes(&mem);
                imprimeMemDados(&memDados);
                break;
            case 4:
                system("clear");
                imprimeBanco(bancoRegistradores); // Testando se o banco de registradores foi criado de maneira correta
                break;
            case 5: 
                imprimeMemInstrucoes(&mem);
                imprimeMemDados(&memDados);
                imprimeBanco(bancoRegistradores);
                imprimeEstatistica(stat);
                break;
                */
            case 6:
                inputInterface = newwin(6, 95, 10, (COLS/2)-45);
                box(inputInterface, 0, 0);
                mvwprintw(inputInterface, 2, (95-(strlen("Digite o nome do arquivo para salvar\n")))/2, "Digite o nome do arquivo para salvar\n\n");
                wrefresh(inputInterface);

                keypad(inputInterface, TRUE);
                echo();
                wgetnstr(inputInterface, arquivoAsm, 49);
                delwin(inputInterface);
                noecho();
                salvarAsm(arquivoAsm, &mem);

                break;
            case 7:
                inputInterface = newwin(6, 95, 10, (COLS/2)-45);
                box(inputInterface, 0, 0);
                mvwprintw(inputInterface, 2, (95-(strlen("Digite o nome do arquivo para salvar\n")))/2, "Digite o nome do arquivo para salvar\n\n");
                wrefresh(inputInterface);

                keypad(inputInterface, TRUE);
                echo();
                wgetnstr(inputInterface, arquivoMemDados, 49);
                delwin(inputInterface);
                noecho();
                salvarMemoriaEmArquivo(arquivoMemDados, &memDados);
                
                break;
            /*case 8:
                {FILE *log = freopen("log_run.txt", "w", stdout);
                if (!log) { perror("Erro ao abrir log"); break; }
                // int contador = 0;

                while (parada)
                {
                    step(pilha, &contClock, &pc, &parada, RegIN, RegOUT, bancoRegistradores, &mem, &memDados);
                    RegIN = RegOUT;
                }
                fflush(stdout);
                fclose(log);
                freopen("/dev/tty", "w", stdout); // volta para terminal
                imprimeLogNoTerminal("log_run.txt");
                */
            case 9:

                    //step(&parada, &pc, &mem, bancoRegistradores, controle, pilha, stat, &estadoControle, &regSaidaULA->resultULA, regMDR, &RegA, &RegB, regIR);
                    // step(&parada, &pc, &memDados, &mem, bancoRegistradores, controle, pilha, stat

                    endwin();

                    step(pilha, &contClock, &pc, &parada, RegIN, RegOUT, bancoRegistradores, &mem, &memDados);
                    RegIN = RegOUT;

                break;
            /*
            case 10:
                printf("\nBACK\n");
                nodoPilha *voltaInstrucao = removePilha(pilha);
                if (voltaInstrucao != NULL)
                {
                    stat->back++;
                    bancoRegistradores = voltaInstrucao->bancoRegs;
                    pc = voltaInstrucao->pc;
                    memDados = *voltaInstrucao->memoriaDados;
                    RegIN = *voltaInstrucao->regInAll;
                }
                printf("\n ********* Instrucao Atutal apos BACK ********* \n");
                printf("->PC: [%d]\n",pc);
                printf("->Instrução executada: [%s]\n", buscaInstrucao(&mem, pc).assembly);
                printf("->Registradores estado antigo");
                imprimeBanco(bancoRegistradores);

                //printStack(pilha);
                break;
                */
            case 12: 
                endwin();
                break;
        }
    } while (menu != 12);
    
    free(mem.mem_inst);
    return 0;
}

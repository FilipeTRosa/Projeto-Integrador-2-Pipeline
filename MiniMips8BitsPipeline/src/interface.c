#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <string.h>
#include "interface.h"

WINDOW* criaJanela(int height, int width, int startY, int startX) {
    WINDOW* newWindow = newwin(height, width, startY, startX);
    
    return newWindow;
}

void menuButton(WINDOW* janela, int posY, int posX, char* option, int select) {
    
    if(select == 1) {
        wattron(janela, A_REVERSE);
    }
    mvwprintw(janela, posY, posX, option);
    wattroff(janela, A_REVERSE);
    wrefresh(janela);
}

int menuInterface() {

    curs_set(0);

    int select = 1;
    int optionSelect =-1;

    // ----------- CONFIGURAÇÕES DO TITULO ------------------//

    int titleHeight = 3;
    int titleWidth = 95;
    int titleStartY = 2;
    int titleStartX = (COLS)/2 - 45;

    WINDOW *titleWin = newwin(titleHeight, titleWidth, titleStartY, titleStartX);

    // -----------------------------------------------------//

    // ----------- CONFIGURAÇÕES DO MENU ------------------//

    int menuHeight = 30;
    int menuWidth = 45;
    int menuStartY = (titleStartY + 3);
    int menuStartX = (COLS/2) - menuWidth;

    WINDOW *menuWin = newwin(menuHeight, menuWidth, menuStartY, menuStartX);

    // -----------------------------------------------------//

    // ----------- CONFIGURAÇÕES DOS DETALHES ------------------//

    int infoHeight = 30;
    int infoWidth = 45;
    int infoStartY = (titleStartY + 3);
    int infoStartX = (COLS/2) + 5;

    WINDOW *infoWin = newwin(infoHeight, infoWidth, infoStartY, infoStartX);

    // -----------------------------------------------------//


    char* details[] = {
        "Inicialize seu programa",
        "Insira dados para execução",
        "Imprime as memorias de dados e instruções",
        "Veja o estado dos registradores",
        "Salve seu programa em assembly",
        "Salve sua memoria de dados",
        "Executa o programa carregado",
        "Executa uma instrução",
        "Volta uma instrução",
        "Encerrar programa",
    };

    while(1) {
        
        wattron(titleWin,A_BLINK | A_BOLD);
        mvwprintw(titleWin, 1, (titleWidth-strlen("MiniMIPS 8 Bits - Pipeline"))/2, "MiniMIPS 8 Bits - Pipeline");
        wattroff(titleWin,A_BLINK | A_BOLD);
        box(titleWin, 0, 0);
        wrefresh(titleWin);
            

        
        box(menuWin, 0, 0);
        menuButton(menuWin, 2, 2, "Carregar memoria de instrucoes (.mem)", (select==1));
        menuButton(menuWin, 4, 2, "Carregar memoria de Dados (.dat)", (select==2));
        menuButton(menuWin, 6, 2, "Ver memorias", (select==3));
        menuButton(menuWin, 8, 2, "Ver registradores", (select==4));
        menuButton(menuWin, 10, 2, "Salvar assembly", (select==5));
        menuButton(menuWin, 12, 2, "Salvar dados", (select==6));
        menuButton(menuWin, 14, 2, "Run", (select==7));
        menuButton(menuWin, 16, 2, "Step", (select==8));
        menuButton(menuWin, 18, 2, "Step Back", (select==9));
        menuButton(menuWin, 20, 2, "Sair", (select==10));

        wrefresh(menuWin);

        werase(infoWin);
        box(infoWin, 0, 0);
        clear();
        mvwprintw(infoWin, 2, 2, "%s", details[select-1]);
        wrefresh(infoWin);

        keypad(menuWin, TRUE);
        char menuMove = wgetch(menuWin);

        switch(menuMove) {
            case 'w':
                if(select > 1) {
                    select--;
                }
                break;
            case 's':
                if(select < 10) {
                    select++;
                }
                break;
            case '\n':
                optionSelect = select;
                return optionSelect;
        }

        wrefresh(menuWin);
        refresh();
    }

}
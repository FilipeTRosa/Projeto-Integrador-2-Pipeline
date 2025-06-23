#include <ncurses.h>


typedef struct bancoRegistradores BRegs;
typedef struct registrador regs;

typedef struct memoria_instrucao memInstruc;
typedef struct memoria_dados memDados;
typedef struct pilha descPilha;
typedef struct nodo nodoPilha;


typedef struct RegistradorDados RegMDR;
typedef struct saidaULA ULAsaida;
typedef struct dado dados;

struct bancoRegistradores
{
    regs *registradores;
    int tamanho;
};

struct registrador
{
    int id;
    int valor;
    regs *prox;
};

struct estatistica{
    int totalInstrucoes;
    int tipoR;
    int tipoI;
    int tipoJ;
    int back;
};

// ================= CONFIGURAÇÕES DA FUNÇÃO BACK =================== //

struct pilha {
    nodoPilha *instrucoes;
    int tamanho;
};

struct nodo {
    int pc;
    BRegs* bancoRegs;
    memDados* memoriaDados;
    nodoPilha *prox;
    
};

descPilha* criarPilha();
void inserePilha(descPilha* pilha, nodoPilha* nodo);
nodoPilha* removePilha(descPilha* pilha);
nodoPilha* criaNodo(int pc, BRegs* bancoRegs, memDados* memoriaDados);
// struct memoria_dados* copiaMemoriaDados(struct memoria_dados* memoriaDados);
BRegs* copiaBancoRegistradores(BRegs* bancoRegs);
void printStack(descPilha * pilha);

// ================= BANCO DE REGISTRADORES ========================= //

BRegs* alocaBancoRegistradores();
regs* criaRegistrador();
void criaBanco(BRegs* bancoRegs, regs* reg);
void imprimeReg(regs* reg);
void imprimeBanco(BRegs* bancoRegs, WINDOW* stepInterface);
int* buscaBancoRegs(BRegs* bancoRegs, int rs, int rt, int rd, int defDest);
void salvaDadoReg(BRegs* bancoRegistradores, int resultadoULA, int vetBuscaReg,  int sinalControle);


// =================== CONTROLE ===================================== //

int fuctionMux(int op1, int op2, int controleULA);

// ===================== ULA ======================================== //
void imprimeULA(int *resultadoULA);
int* processamentoULA(int op1, int op2, int ulaOP);
int verificaOverflow(int opResult);
int comparaRegs(int op1, int op2);
void converteDecimalParaBinario(char * palavra, int num);
int conversorBinParaDecimal (int compDeDois, char * palavra);
struct instrucao buscaInstrucao(struct memoria_instrucao * memoria, int pc);
void saidaULA(int resultUla, int * saidaula, int clear); // função para saidaULA usada com váriavel // não usada



// ===================== STEP ======================================= //


//void step(int *parada, int *pc, struct memoria_dados *memDados, struct memoria_instrucao *memInst, BRegs *bancoReg, CTRL *controle, descPilha *pilha, struct estatistica * stat);
void imprimeLogNoTerminal(const char *nomeArquivo);
// ===================== DECODIFICACAO ============================== //

// ================================================================== //
struct estatistica * criaStat();
void imprimeEstatistica(struct estatistica * est);

// ================ REGISTRADORES DE ESTADO ===== =================== //

struct saidaULA{
    int resultULA;
};

int regSaidaULA (int resultULA, int clear);
ULAsaida* criaRegSaidaULA();
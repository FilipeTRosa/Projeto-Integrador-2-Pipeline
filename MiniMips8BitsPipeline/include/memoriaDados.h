struct dado{
char dado_char[8];
int dado;
    };

struct memoria_dados{
struct dado *mem_dados;
int tamanho;
    };





// ================== MEMORIA DE DADOS  ============================= //
void carregarDados(const char *nomeArquivo, struct memoria_dados *memDados);
void imprimeDado(struct dado dado);
void imprimeMemDados(struct memoria_dados *mem);
void insereMemDados(struct memoria_dados *mem, int endereco, int valor, int sinalControle);
int getDado(struct memoria_dados *mem, int endereco);
//void salvarMemoriaEmArquivo(const char *nomeArquivo, struct memoria_dados *memDados);
struct memoria_dados* copiaMemoriaDados(struct memoria_dados* memoriaDados);
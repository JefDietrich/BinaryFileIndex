typedef struct
{
    char codCliente[12];
    char codVeiculo[8];
    char nomeCliente[50];
    char nomeVeiculo[50];
    char dia[4];
} LOCACAO_VEICULO;

typedef struct
{
    char codCliente[12];
    char codVeiculo[8];
} CHAVE_PRIMARIA;

typedef struct
{
    CHAVE_PRIMARIA chavePrimaria;
    int offset;
} BUSCA_INDICE;

int verificarIndice(FILE* indice, FILE* resultado, BUSCA_INDICE dadosIndice[]);

int inserirRegistro(LOCACAO_VEICULO veiculoAInserir, FILE* arquivoResultado);

void inserirIndice(LOCACAO_VEICULO veiculoAInserir, BUSCA_INDICE dadosIndice[], int offset, int j);

FILE* verificaArquivo(char* arquivo);

int buscaBinaria(FILE *resultado, BUSCA_INDICE dadosIndice[], CHAVE_PRIMARIA chaveBusca, int tamanhoVetor);

void atualizarArquivoIndice(FILE* indice, BUSCA_INDICE dadosIndice[], int j);
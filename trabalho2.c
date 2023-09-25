#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "header.h"

char INDICE_ATUALIZADO = '1';
char INDICE_NAO_ATUALIZADO = '0';

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

int construirIndice(FILE* indice, FILE* resultado, BUSCA_INDICE dadosIndice[]);

int verificarIndice(FILE* indice, FILE* resultado, BUSCA_INDICE dadosIndice[]);

int inserirRegistro(LOCACAO_VEICULO veiculoAInserir, FILE* arquivoResultado);

void inserirIndice(LOCACAO_VEICULO veiculoAInserir, BUSCA_INDICE dadosIndice[], int offset, int j);

int compareDadosIndice(const void *a, const void *b);

FILE* verificaArquivo(char* arquivo);

int buscaBinaria(FILE *resultado, BUSCA_INDICE dadosIndice[], CHAVE_PRIMARIA chaveBusca, int tamanhoVetor);

void atualizarArquivoIndice(FILE* indice, BUSCA_INDICE dadosIndice[], int j);

int main() {
    FILE *arquivo = verificaArquivo("insere.bin");
    LOCACAO_VEICULO locacaoVeiculos[8];
    fread(locacaoVeiculos, sizeof(LOCACAO_VEICULO), 8, arquivo);

    arquivo = verificaArquivo("busca_p.bin");
    CHAVE_PRIMARIA chavePrimaria[4];
    fread(chavePrimaria, sizeof(CHAVE_PRIMARIA), 4, arquivo);
    
    fclose(arquivo);

    FILE *resultado = fopen("resultado.bin", "a+b");    
    FILE *indice = fopen("indice.bin", "rb");
    
    BUSCA_INDICE dadosIndice[8];
    int j = verificarIndice(indice, resultado, dadosIndice);    

    fclose(resultado);

    if(indice != NULL) {
        fclose(indice);
    }

    int opcao, i;

    do {
        printf("\n+------------------------------------------+");
        printf("\nSelecione uma das opções abaixo:\n\n");
        printf("(1) Inserir um registro.\n");
        printf("(2) Buscar um registro.\n");
        printf("(0) Sair do programa.\n");
        printf("\n+------------------------------------------+\n");
        printf("Opção: ");
        scanf("%d", &opcao);

        switch (opcao)
        {
            case 1:             
                while (1) {
                    printf("\nDigite '0' para sair.");      
                    printf("\nInforme um número de 1 a 8: ");      
                    scanf("%d", &i);

                    if (i == 0) 
                        break;
                    if (i < 1 || i > 8) {
                        printf("Opção inválida!");
                    } else {
                        indice = fopen("indice.bin", "r+b");
                        rewind(indice);
                        fwrite(&INDICE_NAO_ATUALIZADO, sizeof(char), 1, indice);
                        fclose(indice);

                        resultado = fopen("resultado.bin", "r+b");
                        int offSet = inserirRegistro(locacaoVeiculos[i-1], resultado);
                        fclose(resultado);                                
                        inserirIndice(locacaoVeiculos[i-1], dadosIndice, offSet, j);
                        j++;
                    }
                }
                break;
            case 2:
                while (1) {
                    printf("\nDigite '0' para sair.");      
                    printf("\nInforme um número de 1 a 4: ");      
                    scanf("%d", &i);            

                    if (i == 0)
                        break;
                    if (i < 1 || i > 4) {
                        printf("Opção inválida!");
                    } else {
                        resultado = fopen("resultado.bin", "r+b");
                        int offSet = buscaBinaria(resultado, dadosIndice, chavePrimaria[i-1], j);
                        fseek(resultado, offSet, SEEK_SET);

                        int tamanhoRegistro;
                        char registroBuscado[130];
                        fread(&tamanhoRegistro, sizeof(int), 1, resultado);
                        fread(registroBuscado, sizeof(char), tamanhoRegistro, resultado);

                        printf("%s\n", registroBuscado);
                        fclose(resultado);
                    }
                }
                break;                  
            case 0:
                printf("\nSaindo do programa...\n\n");
                indice = fopen("indice.bin", "r+b");  
                atualizarArquivoIndice(indice, dadosIndice, j);
                fclose(indice);

                break;            
            default:
                break;
        }
    } while (opcao != 0);    
}

int construirIndice(FILE* indice, FILE* resultado, BUSCA_INDICE dadosIndice[]) {
    int j = 0, tamanhoRegistro, posicaoRegistro, contadorDeCaninho, i; 
    char letraLida, codigoLido[20];       
    
    rewind(resultado);
    while(fread(&tamanhoRegistro, sizeof(int), 1, resultado) != 0) { 
        posicaoRegistro = ftell(resultado);
        
        LOCACAO_VEICULO veiculoAInserir;
        contadorDeCaninho = 0; i = 0;
        
        while(true) {                                            
            if(letraLida == '|') {              
                contadorDeCaninho++;
                codigoLido[i] = '\0';  
                i = 0;

                if(contadorDeCaninho == 1) {
                    strcpy(veiculoAInserir.codCliente, codigoLido);
                } else {
                    strcpy(veiculoAInserir.codVeiculo, codigoLido);
                    break;
                }   
            } else {
                codigoLido[i] = letraLida;
                i++;
            }           
            fread(&letraLida, sizeof(char), 1, resultado); 
        }                                   

        inserirIndice(veiculoAInserir, dadosIndice, posicaoRegistro, j);
        j++;
    }

    atualizarArquivoIndice(indice, dadosIndice, j);
    return j;
}

int verificarIndice(FILE* indice, FILE* resultado, BUSCA_INDICE dadosIndice[]) {
    if(indice == NULL) {
        printf("Criando índice...\n");
        indice = fopen("indice.bin", "a+b");                
        int qtdIndice = construirIndice(indice, resultado, dadosIndice);
        fclose(indice);
        return qtdIndice;
    }

    rewind(indice);
    char statusAtualizacao;
    fread(&statusAtualizacao, sizeof(char), 1, indice);

    if(statusAtualizacao == INDICE_NAO_ATUALIZADO){
        printf("Índice desatualizado, reconstruindo índice...\n");
        return construirIndice(indice, resultado, dadosIndice);
    }
    int j;
    BUSCA_INDICE buscaIndice;
    while (fread(&buscaIndice, sizeof(BUSCA_INDICE), 1, indice) > 0) {        
        j++;
    }
        
    return j;    
}

int inserirRegistro(LOCACAO_VEICULO veiculoAInserir, FILE* arquivoResultado) {
    char registro[130];
    sprintf(registro, "%s|%s|%s|%s|%s|", veiculoAInserir.codCliente,
            veiculoAInserir.codVeiculo,
            veiculoAInserir.nomeCliente,
            veiculoAInserir.nomeVeiculo,
            veiculoAInserir.dia);

    int tamRegistro = strlen(registro), header, posicaoRegistro, tamRegistroDeletado;
    fseek(arquivoResultado, 0, SEEK_END);  

    int offSet = ftell(arquivoResultado);

    fwrite(&tamRegistro, sizeof(int), 1, arquivoResultado);    
    fwrite(registro, sizeof(char), tamRegistro, arquivoResultado);

    return offSet;
}

void inserirIndice(LOCACAO_VEICULO veiculoAInserir, BUSCA_INDICE dadosIndice[], int offset, int j) {   
    BUSCA_INDICE indiceAInserir;
    indiceAInserir.offset = offset; 
    
    CHAVE_PRIMARIA chave;
    strcpy(chave.codCliente, veiculoAInserir.codCliente);
    strcpy(chave.codVeiculo, veiculoAInserir.codVeiculo);

    indiceAInserir.chavePrimaria = chave;

    dadosIndice[j] = indiceAInserir;
    qsort(dadosIndice, (j + 1), sizeof(BUSCA_INDICE), compareDadosIndice);
}

int compareDadosIndice(const void *a, const void *b) {
   BUSCA_INDICE *dadoA = (BUSCA_INDICE *)a;
   BUSCA_INDICE *dadoB = (BUSCA_INDICE *)b;    

    int i = strcmp(dadoA->chavePrimaria.codCliente, dadoB->chavePrimaria.codCliente);

    if(i == 0) {
        return strcmp(dadoA->chavePrimaria.codVeiculo, dadoB->chavePrimaria.codVeiculo);
    }

    return i;
}

FILE* verificaArquivo(char* arquivo) {
    FILE* fp = fopen(arquivo, "rb");
    
    if( (fp == NULL) ) {
        printf("O arquivo %s não existe.", arquivo);
        exit(0);
    }    
    return fp;
}

int buscaBinaria(FILE *resultado, BUSCA_INDICE dadosIndice[], CHAVE_PRIMARIA chaveBusca, int tamanhoVetor) {
    int inicio = 0;
    int final = tamanhoVetor - 1;
    int metade;
    CHAVE_PRIMARIA chaveLida;
    
    while(inicio <= final) {
        metade = (inicio + final) / 2;
        chaveLida = dadosIndice[metade].chavePrimaria;
        
        int encontrado = strcmp(chaveLida.codCliente, chaveBusca.codCliente);
        if( encontrado == 0 ) {
            encontrado = strcmp(chaveLida.codVeiculo, chaveBusca.codVeiculo);
        }

        if( encontrado == 0) {
            return dadosIndice[metade].offset;
        } else if( encontrado < 0) {
            inicio = metade + 1;
        } else {
            final = metade - 1;
        }
    }

    return -1;
}

void atualizarArquivoIndice(FILE* indice, BUSCA_INDICE dadosIndice[], int j) {
    if(j != 0) {
        fseek(indice, sizeof(char), SEEK_SET);    
        fwrite(dadosIndice, sizeof(BUSCA_INDICE), j, indice);
    }    
    
    rewind(indice);
    fwrite(&INDICE_ATUALIZADO, sizeof(char), 1, indice);
}
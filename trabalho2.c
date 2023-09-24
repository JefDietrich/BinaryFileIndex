#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

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
} REMOCAO_VEICULO;

void inserirHeader(FILE* arquivoResultado) {
    rewind(arquivoResultado);
    int header;
    if(fread(&header, sizeof(int), 1, arquivoResultado) == 0) {
        header = -1;
        fwrite(&header, sizeof(int), 1, arquivoResultado);
    }
}

/* Estrutura do registro deletado */
/*   100         #          304       [ registro ] */
/* tamanho end.      proximo deletado [ registro ] */
void inserirRegistro(LOCACAO_VEICULO veiculoAInserir, FILE* arquivoResultado) {
    char registro[130];
    sprintf(registro, "%s|%s|%s|%s|%s|", veiculoAInserir.codCliente,
            veiculoAInserir.codVeiculo,
            veiculoAInserir.nomeCliente,
            veiculoAInserir.nomeVeiculo,
            veiculoAInserir.dia);

    int tamRegistro = strlen(registro), header, posicaoRegistro, tamRegistroDeletado;
    bool espacoEncontrado = false;

    rewind(arquivoResultado);    
    fread(&header, sizeof(int), 1, arquivoResultado);

    if(header != -1) {        
        // Até achar um espaço ou o header for igual a -1
        while(!espacoEncontrado && header != -1) {            
            fseek(arquivoResultado, header, SEEK_SET);

            // Guarda posição do registro para inserir depois caso o tamanho seja ideal   
            posicaoRegistro = ftell(arquivoResultado);

            // Verifica se o tamanho do registro deletado consegue armazenar o registro novo
            fread(&tamRegistroDeletado, sizeof(int), 1, arquivoResultado);                                                    
            if (tamRegistroDeletado >= tamRegistro) {   
                espacoEncontrado = true;                            
            } 

            // Anda um para pular o # e lê o próximo registro deletado
            fseek(arquivoResultado, sizeof(char), SEEK_CUR);
            fread(&header, sizeof(int), 1, arquivoResultado);
        }     
    }  
    
    if(!espacoEncontrado) {
        // Se não encontrou um espaço, insere no fim do arquivo
        fseek(arquivoResultado, 0, SEEK_END);                 
        fwrite(&tamRegistro, sizeof(int), 1, arquivoResultado);    
        fwrite(registro, sizeof(char), tamRegistro, arquivoResultado);
    } else {
        fseek(arquivoResultado, posicaoRegistro + sizeof(int), SEEK_SET);        
        // fwrite(&tamRegistro, sizeof(int), 1, arquivoResultado);    
        fwrite(registro, sizeof(char), tamRegistro, arquivoResultado);

        // Atualiza o header com a posição próximo registro deletado
        rewind(arquivoResultado);
        fwrite(&header, sizeof(int), 1, arquivoResultado);
    } 
}

int main()
{
    FILE *arquivo = verificaArquivo("insere.bin");

    LOCACAO_VEICULO locacaoVeiculos[8];
    fread(locacaoVeiculos, sizeof(LOCACAO_VEICULO), 8, arquivo);

    arquivo = verificaArquivo("remove.bin");
    
    REMOCAO_VEICULO remocaoVeiculo[4];
    fread(remocaoVeiculo, sizeof(REMOCAO_VEICULO), 4, arquivo);

    fclose(arquivo);

    FILE *resultado = fopen("resultado.bin", "a+b");
    inserirHeader(resultado);
    fclose(resultado);

    int opcao, i = 0;

    do {
        printf("\n+------------------------------------------+");
        printf("\nSelecione uma das opções abaixo:\n\n");
        printf("(1) Inserir um registro.\n");
        printf("(2) Remover um registro.\n");
        printf("(3) Compactar o arquivo.\n");
        printf("(0) Sair do programa.\n\n");
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

                    if (i == 0) {
                        break;
                    }else if (i < 1 || i > 8) {
                        printf("Opção inválida!");
                    }
                    resultado = fopen("resultado.bin", "r+b");
                    inserirRegistro(locacaoVeiculos[i-1], resultado);
                    fclose(resultado);
                }
                break;
            case 2:
                while (1) {
                    printf("\nDigite '0' para sair.");      
                    printf("\nInforme um número de 1 a 4: ");      
                    scanf("%d", &i);            

                    if (i == 0) {
                        break;
                    }else if (i < 1 || i > 4) {
                        printf("Opção inválida!");
                    }
                    resultado = fopen("resultado.bin", "r+b");
                    removerRegistro(remocaoVeiculo[i-1], resultado);
                    fclose(resultado);
                }
                break;        
            case 3:                
                compactarArquivo();
                printf("\nArquivo compactado com sucesso!\n");
                break;            
            case 0:
                printf("\nSaindo do programa...\n\n");
                break;            
            default:
                break;
        }
    } while (opcao != 0);    
}
#include<stdio.h>

int main() {
    FILE *fd;
    
    //////////////////////////////
    typedef struct veiculo {
        char cod_cli[12];
        char cod_vei[8];
        char client[50];
        char veiculo[50];
        char dias[4];
    } veic_t;
	
	veic_t regs_locs_vei[8] = {	{"11111111111", "ABC1234", "Cliente-1", "Veiculo-11", "2" },    // testar busca
		                		{"22222222222", "ABC1234", "Cliente-2", "Veiculo-11", "8" },    // testar busca
		                		{"33333333333", "CDE9874", "Cliente-3", "Veiculo-33", "1" },    // testar busca
		                		{"44444444444", "ERT4561", "Cliente-4", "Veiculo-44", "11"},     
		                		{"11111111111", "ERT4561", "Cliente-1", "Veiculo-44", "2" },    // testar busca
		                		{"11111111111", "UJG3574", "Cliente-1", "Veiculo-66", "3" },    // testar busca
		                		{"77777777777", "TOP5487", "Cliente-7", "Veiculo-22", "2" },
		                		{"88888888888", "IUY3214", "Cliente-8", "Veiculo-88", "2" }};
       
    fd = fopen("insere.bin", "w+b");
    fwrite(regs_locs_vei, sizeof(regs_locs_vei), 1, fd);
    fclose(fd);
    
    //////////////////////////////
    typedef struct remove {
        char cod_cli[12];
        char cod_vei[8];
    } reg_id_t; 
	
	reg_id_t regs_id_list[5] = {{"11111111111", "ABC1234"},    // testar busca
		                		{"22222222222", "ABC1234"},    // testar busca
		                		{"33333333333", "CDE9874"},    // testar busca
		                		{"44444444444", "ERT4561"},
		                		{"11111111111", "ERT4561"}};
       
    fd = fopen("busca_p.bin", "w+b");
    fwrite(regs_id_list, sizeof(regs_id_list), 1, fd);
    fclose(fd);
    
    
}


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>


//DEFINE AS STRUCTS: O QUE VAI SER USADO NO FINAL:
//-------------------------------------------------------------------------//
typedef struct Registradores {
    int R0, R1, R2, R3, R4, R5, R6, R7;
} Registradores;


typedef struct flags {
    int S, Z, C, Ov;
} flags;

//Endereço: PC; Instruçao: IR
typedef struct memoriaPrograma {
    int endereco, instrução;
} memoriaPrograma;

//-------------------------------------------------------------------------//
//IMPRIME O VALOR DAS FLAGS: TEMPORARIO
void imprimirFlags(flags flag) {
    printf("Valor das flags: ------------------------\n");
    printf("S: %d\n", flag.S);
    printf("Z: %d\n", flag.Z);
    printf("C: %d\n", flag.C);
    printf("Ov: %d\n", flag.Ov);
}
//-------------------------------------------------------------------------//


//SEPARA OS BITS: RETORNA O BIT NECESSARIO PARA CADA AÇAO DA TABELA
//-------------------------------------------------------------------------//
//QUAL INSTRUÇAO:
int bitsEntre15e12(int num) {
    return num >> 12;
}
//QUAL TIPO DE INSTRUÇAO:
int bit11(int num) {
    return (num >> 11) & 0x0001;
}
//-------------------------------------------------------------------------//
//RD: (MOV, LOAD, ULA, POP, ROTATE):
int bitsEntre10e8(int num) {
    return (num >> 8) & 0x0007;
}
//RM: (MOV, LOAD, ULA, ROTATE):
int bitsEntre7e5(int num) {
    return (num >> 5) & 0x0007;
}
//RN: (ADD, SUB, MUL, AND, ORR, XOR, PSH):
int bitsEntre4e2(int num) {
    return (num >> 2) & 0x0007;
}
//-------------------------------------------------------------------------//
//IMEDIATO(MOV):
int bitsEntre7e0(int num) {
    return num & 0x00FF;
}
//IMEDIATO(SHIFT):
void bitsEntre4e0(int num) {
    printf("Valor Immed: %d\n", num &= 0x001F);
}
//IMEDIATO(JUMP):
void bitsEntre10e2(int num) {
    printf("Valor Immed: %d\n", num &= 0x07FF);
}
//-------------------------------------------------------------------------//
//FLAG(PUSH, POP, JUMP):
void bits1e0(int num) {
    printf("Valor Immed: %d\n", num &= 0x0003);
}
//-------------------------------------------------------------------------//
//NOP OU HALT (15-00):
void haltOuNop(int num){
    if(num == 0xFFFF){
        printf("HALT\n");
    } else if(num == 0x0000){
        printf("NOP\n");
    }
}
//-------------------------------------------------------------------------//


void imprimirBinario(unsigned int num) {
    for(int i = 15; i >= 0; i--) {
        printf("%d", (num >> i) & 1);
        if(i % 4 == 0) {
            printf(" ");
        }
    }
    printf("\n");
}

int oQueEstaSendoFeito(int num) {
    if(num % 2 == 0) {
        return 0;
    }
    return 1;
}

//-------------------------------------------------------------------------//

void movRmParaRd(int numHexa, int registradores[]) {
    printf("MOV R%d, R%d\n", bitsEntre10e8(numHexa), bitsEntre7e5(numHexa));
    registradores[bitsEntre10e8(numHexa)] = registradores[bitsEntre7e5(numHexa)];
}

void movImmed(int numHexa, int registradores[]) {
    printf("MOV R%d, #%d\n", bitsEntre10e8(numHexa), bitsEntre7e0(numHexa));
    registradores[bitsEntre10e8(numHexa)] = bitsEntre7e0(numHexa);
}

//-------------------------------------------------------------------------//
void decodificacao(int numHexa, int registradores[]) {
    if(bitsEntre15e12(numHexa) == 0b0001) {
        if(bit11(numHexa) == 0) {   
            movRmParaRd(numHexa, registradores);
            return;
        }
        movImmed(numHexa, registradores);
        return;
    }

    if(bitsEntre15e12(numHexa) == 0b0100) {
        printf("ADD R%d, R%d, R%d\n", bitsEntre10e8(numHexa), bitsEntre7e5(numHexa), bitsEntre4e2(numHexa));
        return;
    }

    if(bitsEntre15e12(numHexa) == 0b0101) {
        printf("SUB R%d, R%d, R%d\n", bitsEntre10e8(numHexa), bitsEntre7e5(numHexa), bitsEntre4e2(numHexa));
        return;
    }
    printf("Instrução não reconhecida\n");
}
//-------------------------------------------------------------------------//

int lerArquivo(char *nomeArquivo, MemoriaPrograma memoriaPrograma[]) {
    FILE *arquivo;
    char linha[256];
    char *token;
    
    strcat(nomeArquivo, ".txt");
    arquivo = fopen(nomeArquivo , "r");
    int j = 0;
    while(!feof(arquivo)) {
        fgets(linha, 256, arquivo);
        token = strtok(linha, ": \n");
        int i = 0;
        while (token != NULL) {
        
            unsigned int numeroHexa = strtol(token, NULL, 16);
            if(i% 2 == 0) {
                memoriaPrograma[j].endereco = numeroHexa;
            } else {
                memoriaPrograma[j].instrucao = numeroHexa;
            }
            /*
            testes
            
            if(oQueEstaSendoFeito(i)) {
                
                mostrarExecucao(numeroHexa);
                printf("\n_______________\n");
                //decodificacao(numeroHexa);
            }
            */
            i++;
            token = strtok(NULL, ": \n"); 
        }
        j++;
    }

    fclose(arquivo);  
    return j;
}

int main() {
    
    printf("Digite o nome do arquivo: ");
    char *nomeArquivo = (char*) malloc(50* sizeof(char));
    scanf("%s", nomeArquivo);
    lerArquivo(nomeArquivo);
    free(nomeArquivo);
    memoriaPrograma memoriaPrograma[256];
    Registradores registradores;
}

void mostrarExecucao(int numeroHexa) {  
/*

    printf("Numero Hexadecimal: %04X\n", numeroHexa);
    printf("Numero Decimal: %d\n", numeroHexa);
    printf("Numero Binario: ");
    imprimirBinario(numeroHexa);
    bitsEntre15e12(numeroHexa);
    bit11(numeroHexa);
    bitsEntre10e8(numeroHexa);
    bitsEntre7e5(numeroHexa);
    bitsEntre4e2(numeroHexa);
    bitsEntre7e0(numeroHexa);
    haltOuNop(numeroHexa);
    bitsEntre4e0(numeroHexa);
    bitsEntre10e2(numeroHexa);
    bits1e0(numeroHexa);
*/
}
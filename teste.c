#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct Registradores {
    int R0;
    int R1;
    int R2;
    int R3;
    int R4;
    int R5;
    int R6;
    int R7;
} Registradores;

typedef struct Flags {
    int S;
    int Z;
    int C;
    int Ov;
} Flags;

typedef struct MemoriaPrograma {
    int endereco;
    int instrucao;
} MemoriaPrograma;

void imprimirFlags(Flags flag) {
    printf("Valor das flags: ------------------------\n");
    printf("S: %d\n", flag.S);
    printf("Z: %d\n", flag.Z);
    printf("C: %d\n", flag.C);
    printf("Ov: %d\n", flag.Ov);
}
int bitsEntre15e12(int num) {
    return num >> 12;
}
int bit11(int num) {
    return (num >> 11) & 0x0001;
}

int bitsEntre10e8(int num) {
    return (num >> 8) & 0x0007;
}

int bitsEntre7e5(int num) {
    return (num >> 5) & 0x0007;
}

int bitsEntre4e2(int num) {
    return (num >> 2) & 0x0007;
}

int bitsEntre7e0(int num) {
    return num & 0x00FF;
}

void bitsEntre4e0(int num) {
    printf("Valor Immed: %d\n", num &= 0x001F);
}

/*talvez aqui dê erro*/
void bitsEntre10e2(int num) {
    printf("Valor Immed: %d\n", num &= 0x07FF);
}

void bits1e0(int num) {
    printf("Valor Immed: %d\n", num &= 0x0003);
}

void haltOuNop(int num){
    if(num == 0xFFFF){
        printf("HALT\n");
    } else if(num == 0x0000){
        printf("NOP\n");
    }
}

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

void movRmParaRd(int numHexa, int registradores[]) {
    printf("MOV R%d, R%d\n", bitsEntre10e8(numHexa), bitsEntre7e5(numHexa));
    registradores[bitsEntre10e8(numHexa)] = registradores[bitsEntre7e5(numHexa)];
}

void movImmed(int numHexa, int registradores[]) {
    printf("MOV R%d, #%d\n", bitsEntre10e8(numHexa), bitsEntre7e0(numHexa));
    registradores[bitsEntre10e8(numHexa)] = bitsEntre7e0(numHexa);
}


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

    char nomeArquivo[300];
    unsigned int PC = 0x0000;
    int registradores[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    int16_t teste = 0b1101;
    uint16_t teste2 = 0b1101;
    /*olhar isso*/
    printf("signed int: %d\n", teste);
    printf("unsigned int: %u\n", teste2); 

    printf("Digite o nome do arquivo: ");
    scanf("%s", nomeArquivo);
    MemoriaPrograma memoriaPrograma[256];
    int tam = lerArquivo(nomeArquivo, memoriaPrograma);

    printf("Valor da memoria do programa: ------------------------\n");
    for (int i = 0; i < tam; i++){
        printf("%04X:%04X\n", memoriaPrograma[i].endereco, memoriaPrograma[i].instrucao);
    }

    printf("Intrucoess: ------------------------\n");
    while(PC != 0x0014) {
        unsigned int IR;
        for(int i = 0; i < tam; i++) {
            if(memoriaPrograma[i].endereco == PC) {
                IR = memoriaPrograma[i].instrucao;
                break;
            }
        }
        PC += 0x0002;
        decodificacao(IR, registradores);
    }

    for(int i = 0; i < 8; i++) {
        printf("R%d: %d\n", i, registradores[i]);
    }
}
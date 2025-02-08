#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

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

typedef struct MemoriaDados {
    uint16_t dado;
    int possuiDado;
} MemoriaDados;

typedef struct Stack {
    int endereco;
    int dado;
    int possuiDado;
} Stack;

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

int bitsEntre4e0(int num) {
    return num & 0x001F;
}

/*talvez aqui dê erro*/
void bitsEntre10e2(int num) {
    printf("Valor Immed: %d\n", num &= 0x07FF);
}

int bits1e0(int num) {
    return num & 0x0003;
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

void movRmParaRd(int numHexa, int16_t registradores[]) {
    printf("MOV R%d, R%d\n", bitsEntre10e8(numHexa), bitsEntre7e5(numHexa));
    registradores[bitsEntre10e8(numHexa)] = registradores[bitsEntre7e5(numHexa)];
}

void movImmed(int numHexa, int16_t registradores[]) {
    printf("MOV R%d, #%d\n", bitsEntre10e8(numHexa), bitsEntre7e0(numHexa));
    registradores[bitsEntre10e8(numHexa)] = bitsEntre7e0(numHexa);
}

void add(int numHexa, int16_t registradores[]) {
    printf("ADD R%d, R%d, R%d\n", bitsEntre10e8(numHexa), bitsEntre7e5(numHexa), bitsEntre4e2(numHexa));
    registradores[bitsEntre10e8(numHexa)] = registradores[bitsEntre7e5(numHexa)] + registradores[bitsEntre4e2(numHexa)];
}

void push(int numHexa, int16_t registradores[], unsigned int *SP, Stack stack[]) {
    printf("PUSH R%d\n", bitsEntre4e2(numHexa));
    stack[*SP].endereco = *SP;
    stack[*SP].dado = registradores[bitsEntre4e2(numHexa)];
    stack[*SP].possuiDado = 1;
    *SP -= 0x0002;
}

void pop(int numHexa, int16_t registradores[], unsigned int *SP, Stack stack[]) {
    printf("POP R%d\n", bitsEntre10e8(numHexa));
    *SP += 0x0002;
    registradores[bitsEntre10e8(numHexa)] = stack[*SP].dado;
}

void storeImmed(int numHexa, int16_t registradores[], MemoriaDados memomoriaDeDados[]) {
   
    
    uint16_t fistImmedPart = bitsEntre4e0(numHexa);
    uint16_t secondImmedPart = bitsEntre10e8(numHexa);
    uint16_t immed = (secondImmedPart << 5) + (fistImmedPart);
    //uint8_t valorParaMEM1 = immed;
    //uint8_t valorParaMEM2 = immed >> 8;
    uint16_t valor = registradores[bitsEntre7e5(numHexa)];
    uint8_t valorParaMEM1 = valor;
    uint8_t valorParaMEM2 = valor >> 8;
    printf("STR R%d, #%d\n", bitsEntre7e5(numHexa), immed);
    int endereco = registradores[bitsEntre7e5(numHexa)];
    memomoriaDeDados[immed].dado = valorParaMEM1;
    memomoriaDeDados[immed + 1].dado = valorParaMEM2;
    memomoriaDeDados[immed].possuiDado = 1;
    if(immed % 2 == 1) {
        memomoriaDeDados[immed - 1].possuiDado = 1;
        memomoriaDeDados[immed + 2].possuiDado = 1;
    }
    memomoriaDeDados[immed + 1].possuiDado = 1;

    
    printf("Endereco: %04X\n", immed);
    printf("Endereco: %04X\n", immed+1);
    printf("Valor: %d\n", memomoriaDeDados[immed].dado);
    printf("Valor: %d\n", memomoriaDeDados[immed + 1].dado);
}

void decodificacao(int numHexa, int16_t registradores[], unsigned int *SP, Stack stack[], MemoriaDados memoriaDeDados[], Flags flags) {

    //MOV
    if(bitsEntre15e12(numHexa) == 0b0001) {
        if(bit11(numHexa) == 0) {   
            movRmParaRd(numHexa, registradores);
            return;
        }
        movImmed(numHexa, registradores);
        return;
    }

    //ADD
    if(bitsEntre15e12(numHexa) == 0b0100) {
        add(numHexa, registradores);
        return;
    }
    //SUB
    if(bitsEntre15e12(numHexa) == 0b0101) {
        printf("SUB R%d, R%d, R%d\n", bitsEntre10e8(numHexa), bitsEntre7e5(numHexa), bitsEntre4e2(numHexa));
        return;
    }
    //PSH
    if(bitsEntre15e12(numHexa) == 0b0000 && bit11(numHexa) == 0 && (bits1e0(numHexa) == 0b01)) {
        push(numHexa, registradores, SP, stack);
        return;
    }

    //POP
    if(bitsEntre15e12(numHexa) == 0b0000 && bit11(numHexa) == 0 && (bits1e0(numHexa) == 0b10)) {
        pop(numHexa, registradores, SP, stack);
        return;
    }

    //STR
    if(bitsEntre15e12(numHexa) == 0b0010) {
       if(bit11(numHexa) == 1) {
            storeImmed(numHexa, registradores, memoriaDeDados);
            return;
        }
        printf("STR R%d, #%d\n", bitsEntre10e8(numHexa), bitsEntre7e0(numHexa));
        return;
    }

    printf("Instrução não reconhecida\n");
}

void mostrarStack(Stack stack[]) {
    printf("Stack: ------------------------\n");
    for(int i = 65536; i >= 0; i--) {
        if(stack[i].possuiDado) {
            printf("%04X: %04X\n", stack[i].endereco, stack[i].dado);
        }
    }
}

void mostrarRegistradores(int16_t registradores[]) {
    printf("Registradores: ------------------------\n");
    for(int i = 0; i < 8; i++) {
        printf("R%d: %04X\n", i, registradores[i]);
    }
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
    int16_t registradores[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    MemoriaPrograma memoriaPrograma[256];
    MemoriaDados memoriaDeDados[65536];
    Stack stack[65536];
    unsigned valorSP = 0x8200;
    unsigned int *SP = &valorSP;
    Flags flags = {0, 0, 0, 0};

    for(int i = 0; i < 65536; i++) {
        memoriaDeDados[i].possuiDado = 0;
        stack[i].possuiDado = 0;
    }

    printf("Digite o nome do arquivo: ");
    scanf("%s", nomeArquivo);
    int tam = lerArquivo(nomeArquivo, memoriaPrograma);

    printf("Valor da memoria do programa: ------------------------\n");
    for (int i = 0; i < tam; i++){
        printf("%04X:%04X\n", memoriaPrograma[i].endereco, memoriaPrograma[i].instrucao);
    }

    printf("Intrucoes: ------------------------\n");
    while(PC != 0x0004) {
        unsigned int IR;
        for(int i = 0; i < tam; i++) {
            if(memoriaPrograma[i].endereco == PC) {
                IR = memoriaPrograma[i].instrucao;
                break;
            }
        }
        PC += 0x0002;
        decodificacao(IR, registradores, SP, stack, memoriaDeDados, flags);
    }

    
    mostrarStack(stack);
    mostrarRegistradores(registradores);

    printf("Memoria de dados: ------------------------\n");
    
    for(int i = 0; i < 65536; i+=2) {
        if(memoriaDeDados[i].possuiDado) {
            printf("%04X: %02X%02x\n", i, memoriaDeDados[i+1].dado, memoriaDeDados[i].dado);
        }
    }

    printf("\nSP e PC ----------------------\n");
    printf("SP: %04X\n", *SP);
    printf("PC: %04X\n", PC);
    imprimirFlags(flags);
}
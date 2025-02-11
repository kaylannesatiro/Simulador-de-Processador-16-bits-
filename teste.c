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

typedef struct MemoriaDados {
    uint8_t dado;
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

void sub(int numHexa, int16_t registradores[]) {
    printf("SUB R%d, R%d, R%d\n", bitsEntre10e8(numHexa), bitsEntre7e5(numHexa), bitsEntre4e2(numHexa));
    registradores[bitsEntre10e8(numHexa)] = registradores[bitsEntre7e5(numHexa)] - registradores[bitsEntre4e2(numHexa)];
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


}

void storeRd(int num, int16_t registradores[], MemoriaDados memoriaDeDados[]) {
    printf("STR [R%d], R%d\n", bitsEntre7e5(num), bitsEntre4e2(num));
    int endereco = registradores[bitsEntre7e5(num)];
    int16_t valor = registradores[bitsEntre4e2(num)];
    uint8_t valorParaMEM1 = valor;
    uint8_t valorParaMEM2 = valor >> 8;
    memoriaDeDados[endereco].dado = valorParaMEM1;
    memoriaDeDados[endereco + 1].dado = valorParaMEM2;
    memoriaDeDados[endereco].possuiDado = 1;
    if(endereco % 2 == 1) {
        memoriaDeDados[endereco - 1].possuiDado = 1;
        memoriaDeDados[endereco + 2].possuiDado = 1;
    }
    memoriaDeDados[endereco + 1].possuiDado = 1;
}

void load(int num, int16_t registradores[], MemoriaDados memoriaDeDados[]) {
    printf("LDR R%d, [R%d]\n", bitsEntre10e8(num), bitsEntre7e5(num));
    int endereco = registradores[bitsEntre7e5(num)];
    int16_t valor = memoriaDeDados[endereco].dado + (memoriaDeDados[endereco + 1].dado << 8);
    registradores[bitsEntre10e8(num)] = valor;
}

void not(int num, int16_t registradores[]) {
    printf("NOT R%d, R%d\n", bitsEntre10e8(num), bitsEntre7e5(num));
    registradores[bitsEntre10e8(num)] = ~registradores[bitsEntre7e5(num)];
}

void and(int num, int16_t registradores[]) {
    printf("AND R%d, R%d, R%d\n", bitsEntre10e8(num), bitsEntre7e5(num), bitsEntre4e2(num));
    registradores[bitsEntre10e8(num)] = registradores[bitsEntre7e5(num)] & registradores[bitsEntre4e2(num)];
}

void xor(int num, int16_t registradores[]) {
    printf("XOR R%d, R%d, R%d\n", bitsEntre10e8(num), bitsEntre7e5(num), bitsEntre4e2(num));
    registradores[bitsEntre10e8(num)] = registradores[bitsEntre7e5(num)] ^ registradores[bitsEntre4e2(num)];
}

void orr(int num, int16_t registradores[]) {
    printf("ORR R%d, R%d, R%d\n", bitsEntre10e8(num), bitsEntre7e5(num), bitsEntre4e2(num));
    registradores[bitsEntre10e8(num)] = registradores[bitsEntre7e5(num)] | registradores[bitsEntre4e2(num)];
}

void shr(int num, int16_t registradores[]) {
    printf("SHR R%d, R%d, #%d\n", bitsEntre10e8(num), bitsEntre7e5(num), bitsEntre4e0(num));
    registradores[bitsEntre10e8(num)] = registradores[bitsEntre7e5(num)] >> bitsEntre4e0(num);
}

void shl(int num, int16_t registradores[]) {
    printf("SHL R%d, R%d, #%d\n", bitsEntre10e8(num), bitsEntre7e5(num), bitsEntre4e0(num));
    registradores[bitsEntre10e8(num)] = registradores[bitsEntre7e5(num)] << bitsEntre4e0(num);
}

void ror(int num, int16_t registradores[]) {
    printf("ROR R%d, R%d\n", bitsEntre10e8(num), bitsEntre7e5(num));
    registradores[bitsEntre10e8(num)] = (registradores[bitsEntre7e5(num)] >> 1) | (registradores[bitsEntre7e5(num)] << 15);
}

void rol(int num, int16_t registradores[]) {
    printf("ROL R%d, R%d\n", bitsEntre10e8(num), bitsEntre7e5(num));
    registradores[bitsEntre10e8(num)] = (registradores[bitsEntre7e5(num)] << 1) | (registradores[bitsEntre7e5(num)] >> 15);
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
    
    //STR
    if(bitsEntre15e12(numHexa) == 0b0010) {
       if(bit11(numHexa) == 1) {
            storeImmed(numHexa, registradores, memoriaDeDados);
            return;
        }
        storeRd(numHexa, registradores, memoriaDeDados);
        return;
    }

    //LDR
    if(bitsEntre15e12(numHexa) == 0b0011) {
        load(numHexa, registradores, memoriaDeDados);
        return;
    }

    //ADD
    if(bitsEntre15e12(numHexa) == 0b0100) {
        add(numHexa, registradores);
        return;
    }
    //SUB
    if(bitsEntre15e12(numHexa) == 0b0101) {
        sub(numHexa, registradores);
        return;
    }


    
    //AND 
    if(bitsEntre15e12(numHexa) == 0b0111) {
        and(numHexa, registradores);
        return;
    }
    
    //ORR 
    if(bitsEntre15e12(numHexa) == 0b1000) {
        orr(numHexa, registradores);
        return;
    }
    
    //NOT
    if(bitsEntre15e12(numHexa) == 0b1001) {
        not(numHexa, registradores);
        return;
    }

    //XOR
    if(bitsEntre15e12(numHexa) == 0b1010) {
        xor(numHexa, registradores);
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


    //SHR
    if(bitsEntre15e12(numHexa) == 0b1011) {
        shr(numHexa, registradores);
        return;
    }

    //SHL
    if(bitsEntre15e12(numHexa) == 0b1100) {
        shl(numHexa, registradores);
        return;
    }

    //ROR
    if(bitsEntre15e12(numHexa) == 0b1101) {
        ror(numHexa, registradores);
        return;
    }

    //ROL
    if(bitsEntre15e12(numHexa) == 0b1110) {
        rol(numHexa, registradores);
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

uint16_t lerArquivo(char *nomeArquivo, uint8_t memoriaPrograma[]) {
    FILE *arquivo;
    char linha[256];
    char *token;
    
    strcat(nomeArquivo, ".txt");
    arquivo = fopen(nomeArquivo , "r");
    uint16_t j = 0x0000;
    while(!feof(arquivo)) {
        fgets(linha, 256, arquivo);
        token = strtok(linha, ": \n");
        int i = 0;
        while (token != NULL) {
        
            unsigned int numeroHexa = strtol(token, NULL, 16);
            uint8_t primeiraParteInstrucao = numeroHexa;
            uint8_t segundaParteInstrucao = numeroHexa >> 8;

            if(i% 2 == 1) {
                memoriaPrograma[j] = primeiraParteInstrucao;
                memoriaPrograma[j+1] = segundaParteInstrucao;
            } 
            i++;
            token = strtok(NULL, ": \n"); 
        }
        j += 0x0002;
    }

    fclose(arquivo); 
    return j;
}

int main() {

    char nomeArquivo[300];
    unsigned int PC = 0x0000;
    unsigned valorSP = 0x8200;
    unsigned int *SP = &valorSP;
    uint16_t paradaPrograma = 0;
    uint16_t valorUltimaInstrucao = 0;
    int16_t registradores[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    uint8_t memoriaPrograma[65536];
    MemoriaDados memoriaDeDados[65536];
    Stack stack[65536];
    
    Flags flags = {0, 0, 0, 0};


    for(int i = 0; i < 65536; i++) {
        memoriaDeDados[i].possuiDado = 0;
        stack[i].possuiDado = 0;
    }

    printf("Digite o nome do arquivo: ");
    scanf("%s", nomeArquivo);
    valorUltimaInstrucao = lerArquivo(nomeArquivo, memoriaPrograma);

    printf("Valor da memoria do programa: ------------------------\n");
    for (int i = 0; i < valorUltimaInstrucao; i+=2){
        printf("%04X:%02X%02X\n", i, memoriaPrograma[i+1], memoriaPrograma[i]);
    }

    printf("Intrucoes: ------------------------\n");
    while(PC != valorUltimaInstrucao) {
        unsigned int IR;
        IR = memoriaPrograma[PC] + (memoriaPrograma[PC + 1] << 8);
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
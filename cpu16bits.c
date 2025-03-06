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
    uint8_t dado;
    int possuiDado;
} Stack;

void imprimirFlags(Flags flag) {
    printf("\nValor das flags: ------------------------\n\n");
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

int bitsEntre10e2(int num) {
    return (num >> 2) & 0x01FF;
}

int bits1e0(int num) {
    return num & 0x0003;
}

int haltOuNop(int num){
    if(num == 0xFFFF){
        return 1;
    } else if(num == 0x0000){
        return 0;
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

uint16_t complementoDois16bits(uint16_t num) {
    return ~num + 1;
}

int16_t complementoDois16bitsSinalizado(uint16_t num) {
    return ~num + 1;
}

uint8_t complementoDois8bits(uint16_t num) {
    return ~num + 1;
}

int oQueEstaSendoFeito(int num) {
    if(num % 2 == 0) {
        return 0;
    }
    return 1;
}

void zerarFlags(Flags *flags) {
    flags->S = 0;
    flags->Z = 0;
    flags->C = 0;
    flags->Ov = 0;
}

void movRmParaRd(int numHexa, uint16_t registradores[]) {
    registradores[bitsEntre10e8(numHexa)] = registradores[bitsEntre7e5(numHexa)];
}

void movImmed(int numHexa, uint16_t registradores[]) {
    registradores[bitsEntre10e8(numHexa)] = bitsEntre7e0(numHexa);
}

void add(int numHexa, uint16_t registradores[], Flags *flags) {
    uint16_t valorRd = registradores[bitsEntre7e5(numHexa)];
    int16_t valorRdSinalizado = valorRd;
    
    uint16_t valorRm = registradores[bitsEntre4e2(numHexa)];
    int16_t valorRmSinalizado = valorRm;

    uint16_t resultado = valorRd + valorRm;
    int16_t resultadoSinalizado = resultado;

    zerarFlags(flags);

    if(valorRd + valorRm > 0xFFFF) {
        flags->C = 1;
    }

    if(resultado > 0x7FFF || 
        (valorRdSinalizado > 0 && valorRmSinalizado > 0 && resultadoSinalizado < 0) || 
        (valorRdSinalizado < 0 && valorRmSinalizado < 0 && resultadoSinalizado > 0)) {
        flags->Ov = 1;
    }

    if(resultado == 0) {
        flags->Z = 1;
    }

    if(resultadoSinalizado < 0) {
        flags->S = 1;
    }

    registradores[bitsEntre10e8(numHexa)] = resultado;
}

void sub(int numHexa, uint16_t registradores[], Flags *flags) { 
    int16_t valorRd = registradores[bitsEntre7e5(numHexa)];
    int16_t valorRdSinalizado = valorRd;
    
    uint16_t valorRm = complementoDois16bits(registradores[bitsEntre4e2(numHexa)]);
    int16_t valorRmSinalizado = valorRm;

    uint16_t resultado = valorRd + valorRm;
    int16_t resultadoSinalizado = resultado;

    zerarFlags(flags);

    if(valorRd + valorRm > 0xFFFF) {
        flags->C = 1;
    }

    if(resultado > 0x7FFF || 
        (valorRdSinalizado > 0 && valorRmSinalizado > 0 && resultadoSinalizado < 0) || 
        (valorRdSinalizado < 0 && valorRmSinalizado < 0 && resultadoSinalizado > 0)) {
        flags->Ov = 1;
    }

    if(resultado == 0) {
        flags->Z = 1;
    }

    if(resultadoSinalizado < 0) {
        flags->S = 1;
    }

    registradores[bitsEntre10e8(numHexa)] = resultado;

}


void mul(int numHexa, uint16_t registradores[], Flags *flags) {
    uint16_t valorRd = registradores[bitsEntre7e5(numHexa)];
    int16_t valorRdSinalizado = valorRd;
    
    uint16_t valorRm = registradores[bitsEntre4e2(numHexa)];
    int16_t valorRmSinalizado = valorRm;

    uint16_t resultado = valorRd * valorRm;
    int16_t resultadoSinalizado = resultado;

    zerarFlags(flags);

    if(valorRd + valorRm > 0xFFFF) {
        flags->C = 1;
    }

    if(resultado > 0x7FFF || 
        (valorRdSinalizado > 0 && valorRmSinalizado > 0 && resultadoSinalizado < 0) || 
        (valorRdSinalizado < 0 && valorRmSinalizado < 0 && resultadoSinalizado > 0)) {
        flags->Ov = 1;
    }

    if(resultado == 0) {
        flags->Z = 1;
    }

    if(resultadoSinalizado < 0) {
        flags->S = 1;
    }

    registradores[bitsEntre10e8(numHexa)] = resultado;
}

void push(int numHexa, uint16_t registradores[], unsigned int *SP, Stack stack[]) {
    uint8_t dadoParaStack1 = registradores[bitsEntre4e2(numHexa)];
    uint8_t dadoParaStack2 = registradores[bitsEntre4e2(numHexa)] >> 8;
    stack[*SP].dado = dadoParaStack1;
    stack[*SP + 1].dado = dadoParaStack2;
    stack[*SP].possuiDado = 1;
    stack[*SP + 1].possuiDado = 1;
    *SP -= 0x0002;
}

void pop(int numHexa, uint16_t registradores[], unsigned int *SP, Stack stack[]) {
    *SP += 0x0002;
    uint16_t valorParaRegistrador = stack[*SP].dado + (stack[*SP + 1].dado << 8);
    registradores[bitsEntre10e8(numHexa)] = valorParaRegistrador;
}

void storeImmed(int numHexa, uint16_t registradores[], MemoriaDados memomoriaDeDados[]) {
   
    
    uint16_t fistImmedPart = bitsEntre4e0(numHexa);
    uint16_t secondImmedPart = bitsEntre10e8(numHexa);
    uint16_t immed = (secondImmedPart << 5) + (fistImmedPart);
    uint8_t valorParaMEM1 = immed;
    uint8_t valorParaMEM2 = immed >> 8;
    
    uint16_t endereco = registradores[bitsEntre7e5(numHexa)];

    memomoriaDeDados[endereco].dado = valorParaMEM1;
    memomoriaDeDados[endereco + 1].dado = valorParaMEM2;
    memomoriaDeDados[endereco].possuiDado = 1;
    if(endereco % 2 == 1) {
        memomoriaDeDados[endereco - 1].possuiDado = 1;
        memomoriaDeDados[endereco + 2].possuiDado = 1;
    }
    memomoriaDeDados[endereco + 1].possuiDado = 1;


}

void storeRd(int num, uint16_t registradores[], MemoriaDados memoriaDeDados[]) {
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

void load(int num, uint16_t registradores[], MemoriaDados memoriaDeDados[]) {;
    int endereco = registradores[bitsEntre7e5(num)];
    int16_t valor = memoriaDeDados[endereco].dado + (memoriaDeDados[endereco + 1].dado << 8);
    registradores[bitsEntre10e8(num)] = valor;
}

void not(int num, uint16_t registradores[], Flags *flags) {
    zerarFlags(flags);

    uint16_t valorFinal =  ~registradores[bitsEntre7e5(num)];
    int16_t valorFinalSinalizado = valorFinal;
    
    if(valorFinal > 0xFFFF)  {
        flags->C = 1;
    }

    if (valorFinal > 0x7FFF) {
        flags->Ov = 1; 
    }

    if(valorFinal == 0) {
        flags->Z = 1;
    }

    if(valorFinalSinalizado < 0) {
        flags->S = 1;
    }


    registradores[bitsEntre10e8(num)] = valorFinal;
}

void and(int num, uint16_t registradores[], Flags *flags) {
    zerarFlags(flags);

    uint16_t valorFinal = registradores[bitsEntre7e5(num)] & registradores[bitsEntre4e2(num)];
    int16_t valorFinalSinalizado = valorFinal;

    uint16_t valorRd = registradores[bitsEntre7e5(num)];
    int16_t valorRdSinalizado = valorRd;
    
    uint16_t valorRm = registradores[bitsEntre4e2(num)];
    int16_t valorRmSinalizado = valorRm;
    
    if(valorFinal > 0xFFFF)  {
        flags->C = 1;
    }

    if (valorFinal > 0x7FFF || 
        (valorRdSinalizado > 0 && valorRmSinalizado > 0 && valorFinalSinalizado < 0) || 
        (valorRdSinalizado < 0 && valorRmSinalizado < 0 && valorFinalSinalizado > 0)) {
        flags->Ov = 1; 
    }

    if(valorFinal == 0) {
        flags->Z = 1;
    }

    if(valorFinalSinalizado < 0) {
        flags->S = 1;
    }

    
    registradores[bitsEntre10e8(num)] = valorFinal;
}

void xor(int num, uint16_t registradores[], Flags *flags) {
    zerarFlags(flags);

    uint16_t valorFinal = registradores[bitsEntre7e5(num)] ^ registradores[bitsEntre4e2(num)];
    int16_t valorFinalSinalizado = valorFinal;

    uint16_t valorRd = registradores[bitsEntre7e5(num)];
    int16_t valorRdSinalizado = valorRd;
    
    uint16_t valorRm = registradores[bitsEntre4e2(num)];
    int16_t valorRmSinalizado = valorRm;
    
    if(valorFinal > 0xFFFF)  {
        flags->C = 1;
    }

    if (valorFinal > 0x7FFF || 
        (valorRdSinalizado > 0 && valorRmSinalizado > 0 && valorFinalSinalizado < 0) || 
        (valorRdSinalizado < 0 && valorRmSinalizado < 0 && valorFinalSinalizado > 0)) {
        flags->Ov = 1; 
    }

    if(valorFinal == 0) {
        flags->Z = 1;
    }

    if(valorFinalSinalizado < 0) {
        flags->S = 1;
    }

    
    registradores[bitsEntre10e8(num)] = valorFinal;
}

void orr(int num, uint16_t registradores[], Flags *flags) {
    zerarFlags(flags);

    uint16_t valorFinal = registradores[bitsEntre7e5(num)] | registradores[bitsEntre4e2(num)];
    int16_t valorFinalSinalizado = valorFinal;

    uint16_t valorRd = registradores[bitsEntre7e5(num)];
    int16_t valorRdSinalizado = valorRd;
    
    uint16_t valorRm = registradores[bitsEntre4e2(num)];
    int16_t valorRmSinalizado = valorRm;
    
    if(valorFinal > 0xFFFF)  {
        flags->C = 1;
    }

    if (valorFinal > 0x7FFF || 
        (valorRdSinalizado > 0 && valorRmSinalizado > 0 && valorFinalSinalizado < 0) || 
        (valorRdSinalizado < 0 && valorRmSinalizado < 0 && valorFinalSinalizado > 0)) {
        flags->Ov = 1; 
    }

    if(valorFinal == 0) {
        flags->Z = 1;
    }

    if(valorFinalSinalizado < 0) {
        flags->S = 1;
    }

    
    registradores[bitsEntre10e8(num)] = valorFinal;
}

void shr(int num, uint16_t registradores[], Flags *flags) {
    zerarFlags(flags);

    uint16_t valorFinal = registradores[bitsEntre7e5(num)] >> bitsEntre4e0(num);
    int16_t valorFinalSinalizado = valorFinal;
    int16_t valorRdSinalizado = registradores[bitsEntre7e5(num)];

    if(registradores[bitsEntre7e5(num)] >> bitsEntre4e0(num) > 0xFFFF)  {
        flags->C = 1;
    }

    if (valorFinal > 0x7FFF ||
        ((valorRdSinalizado < 0) && valorFinalSinalizado > 0 ) ||
        ((valorRdSinalizado > 0) && valorFinalSinalizado < 0 )) {
        flags->Ov = 1; 
    }

    if(valorFinal == 0) {
        flags->Z = 1;
    }

    if(valorFinalSinalizado < 0) {
        flags->S = 1;
    }

    
    registradores[bitsEntre10e8(num)] = valorFinal;
}

void shl(int num, uint16_t registradores[], Flags *flags) {
    zerarFlags(flags);

    uint16_t valorFinal = registradores[bitsEntre7e5(num)] << bitsEntre4e0(num);
    int16_t valorFinalSinalizado = valorFinal;
    int16_t valorRdSinalizado = registradores[bitsEntre7e5(num)];

    if(registradores[bitsEntre7e5(num)] << bitsEntre4e0(num) > 0xFFFF)  {
        flags->C = 1;
    }

    if (valorFinal > 0x7FFF ||
        ((valorRdSinalizado < 0) && valorFinalSinalizado > 0 ) ||
        ((valorRdSinalizado > 0) && valorFinalSinalizado < 0 )) {
        flags->Ov = 1; 
    }

    if(valorFinal == 0) {
        flags->Z = 1;
    }

    if(valorFinalSinalizado < 0) {
        flags->S = 1;
    }

    
    registradores[bitsEntre10e8(num)] = valorFinal;
}

void ror(int num, uint16_t registradores[], Flags *flags) {
    zerarFlags(flags);

    uint16_t valorFinal = (registradores[bitsEntre7e5(num)] >> 1) | (registradores[bitsEntre7e5(num)] << 15);
    int16_t valorFinalSinalizado = valorFinal;
    int16_t valorRdSinalizado = registradores[bitsEntre7e5(num)];

    if((registradores[bitsEntre7e5(num)] >> 1) | (registradores[bitsEntre7e5(num)] << 15) > 0xFFFF)  {
        flags->C = 1;
    }

    if (valorFinal > 0x7FFF ||
        ((valorRdSinalizado < 0) && valorFinalSinalizado > 0 ) ||
        ((valorRdSinalizado > 0) && valorFinalSinalizado < 0 )) {
        flags->Ov = 1; 
    }

    if(valorFinal == 0) {
        flags->Z = 1;
    }

    if(valorFinalSinalizado < 0) {
        flags->S = 1;
    }

    
    registradores[bitsEntre10e8(num)] = valorFinal;
}

void rol(int num, uint16_t registradores[], Flags *flags) {
    zerarFlags(flags);

    uint16_t valorFinal = (registradores[bitsEntre7e5(num)] << 1) | (registradores[bitsEntre7e5(num)] >> 15);
    int16_t valorFinalSinalizado = valorFinal;
    int16_t valorRdSinalizado = registradores[bitsEntre7e5(num)];

    if((registradores[bitsEntre7e5(num)] << 1) | (registradores[bitsEntre7e5(num)] >> 15) > 0xFFFF)  {
        flags->C = 1;
    }

    if (valorFinal > 0x7FFF ||
        ((valorRdSinalizado < 0) && valorFinalSinalizado > 0 ) ||
        ((valorRdSinalizado > 0) && valorFinalSinalizado < 0 )) {
        flags->Ov = 1; 
    }

    if(valorFinal == 0) {
        flags->Z = 1;
    }

    if(valorFinalSinalizado < 0) {
        flags->S = 1;
    }
    
    registradores[bitsEntre10e8(num)] = valorFinal;
}

void cmp(int num, uint16_t registradores[], Flags *flags) {
    int16_t valorRd = registradores[bitsEntre7e5(num)];
    int16_t valorRdSinalizado = valorRd;
    
    uint16_t valorRm = registradores[bitsEntre4e2(num)];
    int16_t valorRmSinalizado = valorRm;

    uint16_t resultado = valorRd + complementoDois16bits(valorRm);
    int16_t resultadoSinalizado = resultado;

    zerarFlags(flags);

    if(valorRd + valorRm > 0xFFFF) {
        flags->C = 1;
    }

    if(resultado > 0x7FFF ||
        (valorRdSinalizado > 0 && complementoDois16bitsSinalizado(valorRm) > 0 && resultadoSinalizado < 0) || 
        (valorRdSinalizado < 0 && complementoDois16bitsSinalizado(valorRm) < 0 && resultadoSinalizado > 0)) {
        flags->Ov = 1;
    }

    if(valorRd - valorRm == 0) {
        flags->Z = 1;
    }

    if(resultadoSinalizado < 0) {
        flags->S = 1;
    }

}

void jeq(int num, unsigned int *PC, uint16_t *paradaPrograma, Flags *flags, uint16_t valorUltimaInstrucao) {
    uint16_t valorEndereco = bitsEntre10e2(num);
    uint16_t valorPC16bits = *PC;

    if(valorPC16bits + valorEndereco > valorUltimaInstrucao + 0x0002) {
        uint16_t valorParaPulo = 0b1111111000000000 + valorEndereco + valorPC16bits;
    
        if(valorParaPulo == *PC - 0x0002 && flags->Z == 1 && flags->S == 0) {
            *paradaPrograma = 1;
            return;
        }
        
        if(flags->Z == 1 && flags->S == 0) {
            *PC = valorParaPulo;
            return;
        }
        return;
    }
    
    if(flags->Z == 1 && flags->S == 0) {
        *PC += valorEndereco;
    }
}

void jlt(int num, unsigned int *PC, uint16_t *paradaPrograma, Flags *flags, uint16_t valorUltimaInstrucao) {
    uint16_t valorEndereco = bitsEntre10e2(num);
    uint16_t valorPC16bits = *PC;

    if(valorPC16bits + valorEndereco > valorUltimaInstrucao + 0x0002) {
        uint16_t valorParaPulo = 0b1111111000000000 + valorEndereco + valorPC16bits;
    
        if(valorParaPulo == *PC - 0x0002 && flags->Z == 0 && flags->S == 1) {
            *paradaPrograma = 1;
            return;
        }
        if(flags->Z == 0 && flags->S == 1) {
            *PC = valorParaPulo;
            return;
        }
        return;
    } 
    
    if(flags->Z == 0 && flags->S == 1) {
        *PC += valorEndereco;
    }
}


void jgt(int num, unsigned int *PC, uint16_t *paradaPrograma, Flags *flags, uint16_t valorUltimaInstrucao) {
    uint16_t valorEndereco = bitsEntre10e2(num);
    uint16_t valorPC16bits = *PC;
 
    if(valorPC16bits + valorEndereco > valorUltimaInstrucao + 0x0002) {
        uint16_t valorParaPulo = 0b1111111000000000 + valorEndereco + valorPC16bits;
    
        if(valorParaPulo == *PC - 0x0002 && flags->Z == 0 && flags->S == 0) {
            *paradaPrograma = 1;
            return;
        }
        if(flags->Z == 0 && flags->S == 0) {
            *PC = valorParaPulo;
            return;
        }
        return;
    }
    
    if(flags->Z == 0 && flags->S == 0) {
        *PC += valorEndereco;
    }

}

void jump(int num, unsigned int *PC, uint16_t *paradaPrograma, uint16_t valorUltimaInstrucao) {
    uint16_t valorEndereco = bitsEntre10e2(num);
    uint16_t valorPC16bits = *PC;

    if(*PC + valorEndereco > valorUltimaInstrucao) {
        uint16_t valorParaPulo = 0b1111111000000000 + valorEndereco + valorPC16bits;
    
        if(valorParaPulo  == *PC - 0x0002) {
            *paradaPrograma = 1;
            return;
        }
        
        *PC = valorParaPulo;
        return;
    }

    *PC += valorEndereco;

}

void printarPrograma(uint8_t memoriaPrograma[], uint16_t valorUltimaInstrucao, uint16_t registradores[], 
                    unsigned int *SP, Stack stack[], MemoriaDados memoriaDeDados[], Flags flags, uint16_t *paradaPrograma,
                    unsigned *PC, Flags *flagsPointer) {


    printf("\nRegistradores, PC e SP: ------------------------\n\n");
    for(int i = 0; i < 8; i++) {
        printf("R%d: 0x%04X\n", i, registradores[i]);
    }
    printf("\nPC: 0x%04X\nSP: 0x%04X\n", *PC, *SP);

    
    printf("\nMemoria de dados: ------------------------\n\n");
    int possuiDado = 0;
    for(int i = 0; i < 65536; i+=0x0002) {
        if(memoriaDeDados[i].possuiDado) {
            printf("%04X: 0x%02X%02X\n", i, memoriaDeDados[i+1].dado, memoriaDeDados[i].dado);
            possuiDado = 1;
        }
    }
    if(!possuiDado) {
        printf("Memoria de dados nao foi alterada\n");
    }
    
    printf("\nStack: ------------------------\n\n");
    int possuiAlgoPilha = 0;
    for(int i = 65536; i >= 0 ; i-=0x0002) {
        if(stack[i].possuiDado) {
            printf("%04X: 0x%02X%02X\n", i, stack[i+1].dado, stack[i].dado);
            possuiAlgoPilha = 1;
        } else if(i >= 0x81FA && i <= 0x8200) {
            printf("%04X: 0x%02X%02X\n", i, stack[i+1].dado, stack[i].dado);
        }
    }
    if(!possuiAlgoPilha) {
        printf("\nStack nao foi alterada\n");
    }

    imprimirFlags(flags);
    
}

void mostrarStack(Stack stack[]) {
    printf("Stack: ------------------------\n");
    for(int i = 0; i <= 65536 ; i+=0x0002) {
        if(stack[i].possuiDado) {
            printf("%04X: 0x%02X%02X\n", i, stack[i+1].dado, stack[i].dado);
        }
    }
}

void mostrarRegistradores(uint16_t registradores[]) {
    printf("Registradores: ------------------------\n");
    for(int i = 0; i < 8; i++) {
        printf("R%d: 0x%04X\n", i, registradores[i]);
    }
}

void decodificador(int numHexa, uint16_t registradores[], unsigned int *SP, Stack stack[], 
    MemoriaDados memoriaDeDados[], Flags flags, uint16_t *paradaPrograma,
    unsigned *PC, Flags *flagsPointer, uint8_t memoriaPrograma[], uint16_t valorUltimaInstrucao,int *qtdNop) {

    //NOP and HALT
    if(numHexa == 0xFFFF || numHexa == 0x0000) {
        if(haltOuNop(numHexa)) {
            *paradaPrograma = 1;
            return;
        }
        printf("\n######################## NOP %d ########################\n", ++(*qtdNop));
        printarPrograma(memoriaPrograma, valorUltimaInstrucao, registradores, SP, stack, memoriaDeDados, flags, paradaPrograma, PC, flagsPointer);
        printf("\n######################## FIM NOP %d ########################\n", *qtdNop);
        return;
    }

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
        add(numHexa, registradores, flagsPointer);
        return;
    }
    //SUB
    if(bitsEntre15e12(numHexa) == 0b0101) {
        sub(numHexa, registradores, flagsPointer);
        return;
    }

    //MUL
    if(bitsEntre15e12(numHexa) == 0b0110) {
        mul(numHexa, registradores, flagsPointer);
        return;
    }
    
    
    //AND 
    if(bitsEntre15e12(numHexa) == 0b0111) {
        and(numHexa, registradores, flagsPointer);
        return;
    }
    
    //ORR 
    if(bitsEntre15e12(numHexa) == 0b1000) {
        orr(numHexa, registradores, flagsPointer);
        return;
    }
    
    //NOT
    if(bitsEntre15e12(numHexa) == 0b1001) {
        not(numHexa, registradores, flagsPointer);
        return;
    }

    //XOR
    if(bitsEntre15e12(numHexa) == 0b1010) {
        xor(numHexa, registradores, flagsPointer);
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

    //CMP 
    if(bitsEntre15e12(numHexa) == 0b0000 && bit11(numHexa) == 0 && (bits1e0(numHexa) == 0b11)) {
        cmp(numHexa, registradores, flagsPointer);
        return;
    }

    //JMP
    if(bitsEntre15e12(numHexa) == 0b0000 && bit11(numHexa) == 1 && (bits1e0(numHexa) == 0b00)) {
        jump(numHexa, PC, paradaPrograma,valorUltimaInstrucao);
        return;
    }

    //JEQ
    if(bitsEntre15e12(numHexa) == 0b0000 && bit11(numHexa) == 1 && (bits1e0(numHexa) == 0b01)) {
        jeq(numHexa, PC, paradaPrograma, flagsPointer, valorUltimaInstrucao);
        return;
    }

    //JLT
    if(bitsEntre15e12(numHexa) == 0b0000 && bit11(numHexa) == 1 && (bits1e0(numHexa) == 0b10)) {
        jlt(numHexa, PC, paradaPrograma, flagsPointer, valorUltimaInstrucao);
        return;
    }

    //JGT
    if(bitsEntre15e12(numHexa) == 0b0000 && bit11(numHexa) == 1 && (bits1e0(numHexa) == 0b11)) {
        jgt(numHexa, PC, paradaPrograma, flagsPointer, valorUltimaInstrucao);
        return;
    }

    //SHR
    if(bitsEntre15e12(numHexa) == 0b1011) {
        shr(numHexa, registradores, flagsPointer);
        return;
    }

    //SHL
    if(bitsEntre15e12(numHexa) == 0b1100) {
        shl(numHexa, registradores, flagsPointer);
        return;
    }

    //ROR
    if(bitsEntre15e12(numHexa) == 0b1101) {
        ror(numHexa, registradores, flagsPointer);
        return;
    }

    //ROL
    if(bitsEntre15e12(numHexa) == 0b1110) {
        rol(numHexa, registradores, flagsPointer);
        return;
    }

    printf("Instrução não reconhecida\n");
    *paradaPrograma = 1;
}

void instrucoesDoPrograma(int numHexa) {
    if(numHexa == 0xFFFF) {
        printf("HALT\n");
        return;
    }
    if(numHexa == 0x0000) {
        printf("NOP\n");
        return;
    }
    if(bitsEntre15e12(numHexa) == 0b0001) {
        if(bit11(numHexa) == 0) {
            printf("MOV R%d, R%d\n", bitsEntre10e8(numHexa), bitsEntre7e5(numHexa));
            return;
        }
        printf("MOV R%d, #%d\n", bitsEntre10e8(numHexa), bitsEntre7e0(numHexa));
        return;
    }
    if(bitsEntre15e12(numHexa) == 0b0010) {
        if(bit11(numHexa) == 1) {
            uint16_t fistImmedPart = bitsEntre4e0(numHexa);
            uint16_t secondImmedPart = bitsEntre10e8(numHexa);
            uint16_t immed = (secondImmedPart << 5) + (fistImmedPart);      
            printf("STR [R%d], #%d\n", bitsEntre7e5(numHexa), immed);
            return;
        }
        printf("STR [R%d], R%d\n", bitsEntre7e5(numHexa), bitsEntre4e2(numHexa));
        return;
    }
    if(bitsEntre15e12(numHexa) == 0b0011) {
        printf("LDR R%d, [R%d]\n", bitsEntre10e8(numHexa), bitsEntre7e5(numHexa));
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
    if(bitsEntre15e12(numHexa) == 0b0110) {
        printf("MUL R%d, R%d, R%d\n", bitsEntre10e8(numHexa), bitsEntre7e5(numHexa), bitsEntre4e2(numHexa));
        return;
    }
    if(bitsEntre15e12(numHexa) == 0b0111) {
        printf("AND R%d, R%d, R%d\n", bitsEntre10e8(numHexa), bitsEntre7e5(numHexa), bitsEntre4e2(numHexa));
        return;
    }
    if(bitsEntre15e12(numHexa) == 0b1000) {
        printf("ORR R%d, R%d, R%d\n", bitsEntre10e8(numHexa), bitsEntre7e5(numHexa), bitsEntre4e2(numHexa));
        return;
    }
    if(bitsEntre15e12(numHexa) == 0b1001) {
        printf("NOT R%d, R%d\n", bitsEntre10e8(numHexa), bitsEntre7e5(numHexa));
        return;
    }
    if(bitsEntre15e12(numHexa) == 0b1010) {
        printf("XOR R%d, R%d, R%d\n", bitsEntre10e8(numHexa), bitsEntre7e5(numHexa), bitsEntre4e2(numHexa));
        return;
    }
    if(bitsEntre15e12(numHexa) == 0b0000 && bit11(numHexa) == 0 && (bits1e0(numHexa) == 0b01)) {
        printf("PUSH R%d\n", bitsEntre4e2(numHexa));
        return;
    }
    if(bitsEntre15e12(numHexa) == 0b0000 && bit11(numHexa) == 0 && (bits1e0(numHexa) == 0b10)) {
        printf("POP R%d\n", bitsEntre10e8(numHexa));
        return;
    }
    if(bitsEntre15e12(numHexa) == 0b0000 && bit11(numHexa) == 0 && (bits1e0(numHexa) == 0b11)) {
        printf("CMP R%d, R%d\n", bitsEntre7e5(numHexa), bitsEntre4e2(numHexa));
        return;
    }
    if(bitsEntre15e12(numHexa) == 0b0000 && bit11(numHexa) == 1 && (bits1e0(numHexa) == 0b00)) {
        printf("JMP #%04X\n", bitsEntre10e2(numHexa));
        return;
    }
    if(bitsEntre15e12(numHexa) == 0b0000 && bit11(numHexa) == 1 && (bits1e0(numHexa) == 0b01)) {
        printf("JEQ #%04X\n", bitsEntre10e2(numHexa));
        return;
    }
    if(bitsEntre15e12(numHexa) == 0b0000 && bit11(numHexa) == 1 && (bits1e0(numHexa) == 0b10)) {
        printf("JLT #%04X\n", bitsEntre10e2(numHexa));
        return;
    }
    if(bitsEntre15e12(numHexa) == 0b0000 && bit11(numHexa) == 1 && (bits1e0(numHexa) == 0b11)) {
        printf("JGT #%04X\n", bitsEntre10e2(numHexa));
        return;
    }
    if(bitsEntre15e12(numHexa) == 0b1011) {
        printf("SHR R%d, R%d, #%d\n", bitsEntre10e8(numHexa), bitsEntre7e5(numHexa), bitsEntre4e0(numHexa));
        return;
    }
    if(bitsEntre15e12(numHexa) == 0b1100) {
        printf("SHL R%d, R%d, #%d\n", bitsEntre10e8(numHexa), bitsEntre7e5(numHexa), bitsEntre4e0(numHexa));
        return;
    }
    if(bitsEntre15e12(numHexa) == 0b1101) {
        printf("ROR R%d, R%d\n", bitsEntre10e8(numHexa), bitsEntre7e5(numHexa));
        return;
    }
    if(bitsEntre15e12(numHexa) == 0b1110) {
        printf("ROL R%d, R%d\n", bitsEntre10e8(numHexa), bitsEntre7e5(numHexa));
        return;
    }
    printf("Instrução não reconhecida\n");
}

uint16_t lerArquivo(char *nomeArquivo, uint8_t memoriaPrograma[], unsigned *PC) {
    FILE *arquivo;
    char linha[256];
    char *token;
    
    strcat(nomeArquivo, ".txt");
    arquivo = fopen(nomeArquivo, "r");
    
    if (!arquivo) {
        perror("Erro ao abrir o arquivo");
        return 0;
    }
    int primeiraRodada = 1;
    uint16_t j = 0;

    while (fgets(linha, sizeof(linha), arquivo)) {
        
        char *enderecoStr = strtok(linha, ": \n");
        char *instrucaoStr = strtok(NULL, ": \n"); 
        if(primeiraRodada) {
            *PC = (uint16_t)strtol(enderecoStr, NULL, 16);
            primeiraRodada = 0;
        }
        if (enderecoStr && instrucaoStr) {
            j = (uint16_t)strtol(enderecoStr, NULL, 16);

            unsigned int numeroHexa = strtol(instrucaoStr, NULL, 16);
            uint8_t primeiraParteInstrucao = numeroHexa;
            uint8_t segundaParteInstrucao = numeroHexa >> 8;

            memoriaPrograma[j] = primeiraParteInstrucao;
            memoriaPrograma[j + 1] = segundaParteInstrucao;
        }
    }

    fclose(arquivo);
    return j;
}

int main() {
    
    char nomeArquivo[500];
    unsigned int PC = 0x0000;
    unsigned int *PCPointer = &PC;
    unsigned valorSP = 0x8200;
    Stack stack[65536];
    unsigned int *SP = &valorSP;
    uint16_t paradaPrograma = 0;
    uint16_t *paradaProgramaPointer = &paradaPrograma;
    uint16_t valorUltimaInstrucao = 0;
    uint16_t registradores[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    uint8_t memoriaPrograma[65536];
    MemoriaDados memoriaDeDados[65536];
    
    int qtdNop = 0;
    int *qtdNopPointer = &qtdNop;
    
    Flags flags = {0, 0, 0, 0};
    Flags *flagsPointer = &flags;

    for(int i = 0; i < 65536; i++) {
        memoriaDeDados[i].dado = 0;
        memoriaDeDados[i].possuiDado = 0;
        stack[i].possuiDado = 0;
        stack[i].dado = 0;
    }

    printf("Digite o caminho e o nome do arquivo(nao precisa incluir .txt no final do nome do arquivo): ");
    scanf("%s", nomeArquivo);
    
    valorUltimaInstrucao = lerArquivo(nomeArquivo, memoriaPrograma, PCPointer);
    int comecarMemPrograma = PC;

    printf("\n######################## Instrucoes do Arquivo ########################\n");
    for(int i = comecarMemPrograma; i <= valorUltimaInstrucao; i+=0x0002) {
        uint16_t instrucao = memoriaPrograma[i] + (memoriaPrograma[i + 1] << 8);
        instrucoesDoPrograma(instrucao);
    }


    while(PC != valorUltimaInstrucao + 0x0002) {
        unsigned int IR;
        IR = memoriaPrograma[PC] + (memoriaPrograma[PC + 1] << 8);
        PC += 0x0002;
        decodificador(IR, registradores, SP, stack, memoriaDeDados, flags, 
                        paradaProgramaPointer, PCPointer, flagsPointer, memoriaPrograma,
                        valorUltimaInstrucao,qtdNopPointer);
        if(paradaPrograma == 1) {
            break;
        }
    }
    printf("\n######################## Resultado final do Programa ########################\n");
    printarPrograma(memoriaPrograma, valorUltimaInstrucao, registradores, SP, stack, memoriaDeDados, flags, paradaProgramaPointer, PCPointer, flagsPointer);

}
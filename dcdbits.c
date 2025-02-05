#include "dcdbits.h"
#include <stdio.h>

void bitsEntre15e12(int num) {
    printf("Codigo de instrucao: %d\n", num >> 12);
}
void bit11(int num) {
    num = num >> 11; 
    printf("Codigo da casa 11: %d\n", num &= 0x0001);
}

void bitsEntre10e8(int num) {
    num = num >> 8; 
    printf("Registrador Rd: %d\n", num &= 0x0007);
}

void bitsEntre7e5(int num) {
    num = num >> 5; 
    printf("Registrador Rm: %d\n", num &= 0x0007);
}

void bitsEntre4e2(int num) {
    num = num >> 2; 
    printf("Registrador Rn: %d\n", num &= 0x0007);
}

void bitsEntre7e0(int num) {
    printf("Valor Immed: %d\n", num &= 0x00FF);
}

void bitsEntre4e0(int num) {
    printf("Valor Immed: %d\n", num &= 0x001F);
}

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
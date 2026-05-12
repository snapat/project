// UART0.c
#include <stdint.h>
#include "TM4C123GH6PM.h"
#include "UART0.h"

void UART0_Init(void){
    SYSCTL_RCGCUART_R |= 0x01;            // activate uart0
    SYSCTL_RCGCGPIO_R |= 0x01;            // activate port a
    UART0_CTL_R &= ~0x0001;               // disable uart
    UART0_IBRD_R = 27;                    // ibrd = int(50,000,000 / (16 * 115,200)) = int(27.1267)
    UART0_FBRD_R = 8;                     // fbrd = int(0.1267 * 64 + 0.5) = 8
    UART0_LCRH_R = 0x0070;                // 8 bit word length (no parity bits, one stop bit, fifos)
    UART0_CTL_R |= 0x0301;                // enable uart
    GPIO_PORTA_AFSEL_R |= 0x03;           // enable alt funct on pa1-0
    GPIO_PORTA_DEN_R |= 0x03;             // enable digital i/o on pa1-0
    GPIO_PORTA_PCTL_R = (GPIO_PORTA_PCTL_R&0xFFFFFF00)+0x00000011; 
    GPIO_PORTA_AMSEL_R &= ~0x03;          // disable analog functionality on pa
}

void UART0_Output_Character(char data){
    while((UART0_FR_R&0x0020) != 0);      // wait until tx buffer not full
    UART0_DR_R = data;                    // before giving it another byte
}

void UART0_Output_String(char *pt){
    while(*pt){
        UART0_Output_Character(*pt);
        pt++;
    }
}

void UART0_Output_Unsigned_Decimal(uint32_t n){
    if(n >= 10){
        UART0_Output_Unsigned_Decimal(n/10);
        n = n%10;
    }
    UART0_Output_Character(n+'0'); 
}
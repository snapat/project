// UART0.h
#ifndef UART0_H
#define UART0_H

void UART0_Init(void);
void UART0_Output_Character(char data);
void UART0_Output_String(char *pt);
void UART0_Output_Unsigned_Decimal(uint32_t n);

#endif
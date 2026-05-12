// SysTick.h
#ifndef SYSTICK_H
#define SYSTICK_H

void SysTick_Init(void);
void SysTick_Wait(uint32_t delay);
void SysTick_Wait1ms(uint32_t delay);
void SysTick_Wait1us(uint32_t delay);

#endif
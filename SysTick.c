// SysTick.c
#include <stdint.h>
#include "TM4C123GH6PM.h"
#include "SysTick.h"

void SysTick_Init(void){
    NVIC_ST_CTRL_R = 0;               // disable systick during setup
    NVIC_ST_CTRL_R = 0x00000005;      // enable systick with core clock
}

void SysTick_Wait(uint32_t delay){
    NVIC_ST_RELOAD_R = delay - 1;     // number of counts to wait
    NVIC_ST_CURRENT_R = 0;            // any value written to current clears
    while((NVIC_ST_CTRL_R&0x00010000)==0){
        // wait for count flag
    }
}

void SysTick_Wait1ms(uint32_t delay){
    uint32_t i;
    for(i=0; i<delay; i++){
        SysTick_Wait(50000);          // wait 1ms (50mhz clock)
    }
}

void SysTick_Wait1us(uint32_t delay){
    uint32_t i;
    for(i=0; i<delay; i++){
        SysTick_Wait(50);             // wait 1us (50mhz clock)
    }
}
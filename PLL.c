// PLL.c
#include <stdint.h>
#include "TM4C123GH6PM.h"
#include "PLL.h"

void PLL_Init(void){
    // configure system clock to 50 mhz using phase-locked loop
    SYSCTL_RCC2_R |=  0x80000000;  
    SYSCTL_RCC2_R |=  0x00000800;  
    SYSCTL_RCC_R = (SYSCTL_RCC_R &~0x000007C0) + 0x00000540; 
    SYSCTL_RCC2_R &= ~0x00000070; 
    SYSCTL_RCC2_R &= ~0x00002000;
    SYSCTL_RCC2_R = (SYSCTL_RCC2_R & ~0x1FC00000) + (7<<22); 
    while((SYSCTL_RIS_R&0x00000040)==0){}; 
    SYSCTL_RCC2_R &= ~0x00000800;
}
// HCSR04.c
#include <stdint.h>
#include "TM4C123GH6PM.h"
#include "HCSR04.h"
#include "SysTick.h"

void HCSR04_Init(void){
    // initialize trigger pin on pa5
    SYSCTL_RCGCGPIO_R |= 0x01;            // enable clock for port a
    GPIO_PORTA_DIR_R |= 0x20;             // make pa5 output
    GPIO_PORTA_DEN_R |= 0x20;             // enable digital on pa5
    
    // initialize echo pin on pb6 using timer0a input capture
    SYSCTL_RCGCGPIO_R |= 0x02;            // enable clock for port b
    SYSCTL_RCGCTIMER_R |= 0x01;           // enable timer 0
    GPIO_PORTB_DIR_R &= ~0x40;            // make pb6 input
    GPIO_PORTB_DEN_R |= 0x40;             // enable digital on pb6
    GPIO_PORTB_AFSEL_R |= 0x40;           // enable alt funct on pb6
    GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R&0xF0FFFFFF)+0x07000000;
    
    TIMER0_CTL_R &= ~0x01;                // disable timer0a
    TIMER0_CFG_R = 0x04;                  // configure for 16-bit timer mode
    TIMER0_TAMR_R = 0x17;                 // capture mode, edge-time, up-count
    TIMER0_CTL_R |= 0x0C;                 // capture on both edges
    TIMER0_CTL_R |= 0x01;                 // enable timer0a
}

uint32_t HCSR04_GetDistance(void){
    uint32_t start_time, end_time, time_diff;
    uint32_t distance_cm;
    
    // send 10us trigger pulse to pa5
    GPIO_PORTA_DATA_R &= ~0x20;           // low
    SysTick_Wait1us(2);
    GPIO_PORTA_DATA_R |= 0x20;            // high
    SysTick_Wait1us(10);
    GPIO_PORTA_DATA_R &= ~0x20;           // low
    
    // clear timer flag and wait for rising edge
    TIMER0_ICR_R = 0x04;
    while((TIMER0_RIS_R & 0x04) == 0){};
    start_time = TIMER0_TAR_R;            // record start time
    
    // clear timer flag and wait for falling edge
    TIMER0_ICR_R = 0x04;
    while((TIMER0_RIS_R & 0x04) == 0){};
    end_time = TIMER0_TAR_R;              // record end time
    
    // calculate pulse width in clock cycles
    // use bitwise and to prevent 24-bit underflow
    time_diff = (end_time - start_time) & 0x00FFFFFF; 
    
    // convert clock cycles to centimeters
    // (time_diff * (1 / 50mhz)) * 34000 cm/s / 2
    distance_cm = time_diff / 2941;
    
    return distance_cm;
}
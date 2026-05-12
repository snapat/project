// Servo.c
#include <stdint.h>
#include "TM4C123GH6PM.h"
#include "Servo.h"

void Servo_Init(void){
    SYSCTL_RCGCPWM_R |= 0x01;             // enable pwm0
    SYSCTL_RCGCGPIO_R |= 0x02;            // enable port b
    GPIO_PORTB_AFSEL_R |= 0x10;           // enable alt funct on pb4
    GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R&0xFFF0FFFF)+0x00040000;
    GPIO_PORTB_DEN_R |= 0x10;             // enable digital i/o on pb4
    
    SYSCTL_RCC_R |= 0x00100000;           // use pwm divider
    SYSCTL_RCC_R = (SYSCTL_RCC_R&~0x000E0000)+0x000C0000; // divide clock by 64
    
    PWM0_1_CTL_R = 0;                     // re-loading down-counting mode
    PWM0_1_GENA_R = 0x0000008C;           // high on load, low on match
    PWM0_1_LOAD_R = 15625 - 1;            // cycles needed to output 50hz
    PWM0_1_CMPA_R = 15624;                // default 0% duty cycle
    PWM0_1_CTL_R |= 0x01;                 // start timer
    PWM0_ENABLE_R |= 0x04;                // enable pwm0_1 output
}

void Servo_SetAngle(uint8_t angle_degrees){
    // map 0-180 degrees to the servo pwm pulse width range
    // 0 degrees is approx 1ms pulse, 180 degrees is approx 2ms pulse
    // calculation based on 15625 load value
    uint32_t duty_cycle = 390 + ((angle_degrees * 780) / 180);
    PWM0_1_CMPA_R = 15625 - duty_cycle;
}
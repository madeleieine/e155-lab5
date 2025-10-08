// main.h
// Madeleine Kan
// mkan@hmc.edu
// 10/8/25

#ifndef MAIN_H
#define MAIN_H

#include  "C:\Users\mkan\Documents\GitHub\e155-lab5\lab5_mk\lib\STM32L432KC.h"
#include <stm32l432xx.h>


#define LED_PIN PB3
#define BUTTON_PIN PA4
#define QUAD_ENCODER_A PA1
#define INTERRUPT_A PA7
#define QUAD_ENCODER_B PA2
#define DELAY_TIM TIM2

#endif // MAIN_H

void motorInterrupt(void);
void motorPoll(void);
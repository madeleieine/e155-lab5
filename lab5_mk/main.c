// main.c
// reads quadrature encoder signals
// prints speed of motor in rp
// Madeleine Kan
// mkan@hmc.edu
// 10/7/25

#include "C:\Users\mkan\Documents\e155-lab5\lab5_mk\src\main.h"
//#include "main.h"

_Bool startA = 0;

int main(void) {
    // Enable button as input
    gpioEnable(GPIO_PORT_A);
    pinMode(QUAD_ENCODER_A, GPIO_INPUT);
    pinMode(QUAD_ENCODER_B, GPIO_INPUT);
    GPIOA->PUPDR |= _VAL2FLD(GPIO_PUPDR_PUPD1, 0b01); // Set PA1 as pull-up
    GPIOA->PUPDR |= _VAL2FLD(GPIO_PUPDR_PUPD2, 0b01); // Set PA2 as pull-up

    // TODO: Initialize timer
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;
    initTIM(DELAY_TIM);

    // 1. Enable SYSCFG clock domain in RCC
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
    // 2. Configure EXTICR for the input quadrature encoder interrupts PA1 and PA2
    SYSCFG->EXTICR[0] |= _VAL2FLD(SYSCFG_EXTICR1_EXTI1, 0b000);
    SYSCFG->EXTICR[0] |= _VAL2FLD(SYSCFG_EXTICR1_EXTI2, 0b000);

    // Enable interrupts globally
    __enable_irq();

    // Configure interrupt for rising and falling edge of GPIO pin for quadrature encoders
    // 1. Configure mask bit
    EXTI->IMR1 |= (1 << gpioPinOffset(QUAD_ENCODER_A)); 
    EXTI->IMR1 |= (1 << gpioPinOffset(QUAD_ENCODER_B));
    // 2. Enable rising edge trigger
    EXTI->RTSR1 |= (1 << gpioPinOffset(QUAD_ENCODER_A));
    EXTI->RTSR1 |= (1 << gpioPinOffset(QUAD_ENCODER_B));
    // 3. Enable falling edge trigger
    EXTI->FTSR1 |= (1 << gpioPinOffset(QUAD_ENCODER_A));
    EXTI->FTSR1 |= (1 << gpioPinOffset(QUAD_ENCODER_B));
    // 4. Turn on EXTI interrupts in NVIC_ISER
    NVIC->ISER[0] |= (1 << EXTI1_IRQn);
    NVIC->ISER[0] |= (1 << EXTI2_IRQn);
    NVIC->ISER[0] |= (1 << TIM2_IRQn);

    while(1){   
        delay_millis(TIM2, 200);
    }

}

void TIM2_IRQHandler(void){
    // check stuff
    // do stuff
    // HOW TO ENABLE TRIGGER EVENT???
    // Clear the interrupt (trigger interrupt flag)
    clearTrigTIM(TIM2);
}

void EXTI1_IRQHandler(void){
    // Check that quad_encoder_a was what triggered our interrupt
    if (EXTI->PR1 & (1 << gpioPinOffset(QUAD_ENCODER_A))){
        // If so, clear the interrupt (NB: Write 1 to reset.)
        EXTI->PR1 |= (1 << gpioPinOffset(QUAD_ENCODER_A));
        // Then toggle the LED
        togglePin(LED_PIN);

    }
}

void EXTI2_IRQHandler(void){
    // Check that quad_encoder_b was what triggered our interrupt
    if (EXTI->PR1 & (1 << gpioPinOffset(QUAD_ENCODER_B))){
        // If so, clear the interrupt (NB: Write 1 to reset.)
        EXTI->PR1 |= (1 << gpioPinOffset(QUAD_ENCODER_B)); 
        // Then toggle the LED
        togglePin(LED_PIN);

    }
}


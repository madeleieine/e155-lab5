// main.c
// reads quadrature encoder signals
// prints speed of motor in rp
// Madeleine Kan
// mkan@hmc.edu
// 10/7/25

#include "C:\Users\mkan\Documents\GitHub\e155-lab5\lab5_mk\src\main.h"
//#include "main.h"

float count = 0;
int A;
int B;

int notmain(void) {
    int interrupt = 1;
    if (interrupt == 1){
      motorInterrupt();
    } else {
      motorPoll();
    }
    return 0;

}

void motorInterrupt(void){
  // Enable quadrature encoder inputs
    gpioEnable(GPIO_PORT_A);
    pinMode(QUAD_ENCODER_A, GPIO_INPUT);
    pinMode(QUAD_ENCODER_B, GPIO_INPUT);
    GPIOA->PUPDR |= _VAL2FLD(GPIO_PUPDR_PUPD1, 0b01); // Set PA1 as pull-up
    GPIOA->PUPDR |= _VAL2FLD(GPIO_PUPDR_PUPD2, 0b01); // Set PA2 as pull-up

    // Enable PA7 as a flag for quadtrature encoder A
    pinMode(INTERRUPT_A, GPIO_OUTPUT);
    digitalWrite(INTERRUPT_A, 0);

    // Initialize timer
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

    while(1){   
        delay_millis(TIM2, 500);
        float speed = count / (4 * 0.5 * 408);
        printf("motor speed: %f rev/s \n", speed);
        count = 0;
    }
}

void motorPoll(void){
    // Polling
    int volatile curA = digitalRead(QUAD_ENCODER_A);
    int volatile curB = digitalRead(QUAD_ENCODER_B);
    int volatile prevA = curA;
    int volatile prevB = curB;
    uint32_t ms = 500;
    
    // Enable quadrature encoder inputs
    gpioEnable(GPIO_PORT_A);
    pinMode(QUAD_ENCODER_A, GPIO_INPUT);
    pinMode(QUAD_ENCODER_B, GPIO_INPUT);
    GPIOA->PUPDR |= _VAL2FLD(GPIO_PUPDR_PUPD1, 0b01); // Set PA1 as pull-up
    GPIOA->PUPDR |= _VAL2FLD(GPIO_PUPDR_PUPD2, 0b01); // Set PA2 as pull-up

    // Enable PA7 as a flag for quadtrature encoder A
    pinMode(INTERRUPT_A, GPIO_OUTPUT);
    digitalWrite(INTERRUPT_A, 0);

    // Initialize timer
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;
    initTIM(DELAY_TIM);

    // 1. Enable SYSCFG clock domain in RCC
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
    // 2. Configure EXTICR for the input quadrature encoder interrupts PA1 and PA2
    SYSCFG->EXTICR[0] |= _VAL2FLD(SYSCFG_EXTICR1_EXTI1, 0b000);
    SYSCFG->EXTICR[0] |= _VAL2FLD(SYSCFG_EXTICR1_EXTI2, 0b000);

    
    
    while(1){
      DELAY_TIM->ARR = ms;// Set timer max count
      DELAY_TIM->EGR |= 1;     // Force update
      DELAY_TIM->SR &= ~(0x1); // Clear UIF
      DELAY_TIM->CNT = 0;      // Reset count
      while(!(DELAY_TIM->SR & 1)){ // Wait for UIF to go high
        // printf("waiting");
        prevA = curA;
        prevB = curB;
        curA = digitalRead(QUAD_ENCODER_A);
        curB = digitalRead(QUAD_ENCODER_B);
        if (prevA != curA) { // A changes
          if (curA != curB){ // B lag behind A
            count++;
          } else {
            count--;
          }
          togglePin(INTERRUPT_A);
        } else if (prevB != curB) { // B changes
            if (curA == curB) { // B lag behind A
              count++;
            } else {
              count --;
            }
        }
      }
      float speed = count / (4 * 0.5 * 408);
      printf("motor speed: %f rev/s \n", speed);
      count = 0;
    }
}


// quad encoder A
void EXTI1_IRQHandler(void){
    // Check that quad_encoder_a was what triggered our interrupt
    A = digitalRead(gpioPinOffset(QUAD_ENCODER_A));
    B = digitalRead(gpioPinOffset(QUAD_ENCODER_B));
    if (EXTI->PR1 & (1 << gpioPinOffset(QUAD_ENCODER_A))){
        // If so, clear the interrupt (NB: Write 1 to reset.)
        EXTI->PR1 |= (1 << gpioPinOffset(QUAD_ENCODER_A));
        if (A != B){ // B lags after A
          count++;
        } else { // A lags after B
          count--;
        }
        togglePin(INTERRUPT_A);
    }
}

// quad encoder B
void EXTI2_IRQHandler(void){
    // Check that quad_encoder_b was what triggered our interrupt
    A = digitalRead(gpioPinOffset(QUAD_ENCODER_A));
    B = digitalRead(gpioPinOffset(QUAD_ENCODER_B));
    if (EXTI->PR1 & (1 << gpioPinOffset(QUAD_ENCODER_B))){
        // If so, clear the interrupt (NB: Write 1 to reset.)
        EXTI->PR1 |= (1 << gpioPinOffset(QUAD_ENCODER_B)); 
        if (B == A){ // B lags after A
          count++;
        } else { // A lags after B
          count--;
        }
        //togglePin(LED_PIN);

    }
}


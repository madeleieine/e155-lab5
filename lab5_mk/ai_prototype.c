#include "C:\Users\mkan\Documents\GitHub\e155-lab5\lab5_mk\src\main.h"

int notmain(void){
  // Configure PA0 and PA1 as alternate function (AF mode) for Timer
  GPIOA->MODER &= ~(GPIO_MODER_MODE0_Msk | GPIO_MODER_MODE1_Msk); // Clear PA0 and PA1 mode
  GPIOA->MODER |= (GPIO_MODER_MODE0_1 | GPIO_MODER_MODE1_1); // Set PA0 and PA1 to alternate function
  GPIOA->AFR[0] |= (0x2 << GPIO_AFRL_AFSEL0_Pos); // Set AF2 for PA0 (TIM2_CH1)
  GPIOA->AFR[0] |= (0x2 << GPIO_AFRL_AFSEL1_Pos); // Set AF2 for PA1 (TIM2_CH2)

  // Enable the TIM2 clock
  RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;

  // Configure TIM2 in Encoder Mode
  TIM2->PSC = 0;  // Prescaler (no division)
  TIM2->ARR = 0xFFFF;  // Auto-reload register (max count)
  TIM2->CR1 = TIM_CR1_CEN;  // Enable the timer

  // Set the encoder mode (e.g., Encoder mode 1: Count rising edges of encoder signals)
  TIM2->SMCR = (TIM_SMCR_SMS_0 | TIM_SMCR_SMS_1);  // Encoder Mode 1 (TI1 + TI2)

  TIM2->DIER |= TIM_DIER_UIE;  // Enable update interrupt (overflow interrupt)

  // Enable NVIC for TIM2 interrupt
  NVIC_EnableIRQ(TIM2_IRQn);

}

void TIM2_IRQHandler(void) {
    if (TIM2->SR & TIM_SR_UIF) {  // Check if interrupt was caused by update event (overflow)
        TIM2->SR &= ~TIM_SR_UIF;  // Clear the interrupt flag
        // Handle your encoder position change here
        uint32_t encoder_position = TIM2->CNT;  // Read the counter value
        // Example: Print or process the encoder position
        printf("encoder position: %d", encoder_position);
    }
}

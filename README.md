# e155-lab4

use pa6, 5v tolerant and connects to on-board LED (FT in gpio table means tolerates 5V)

Notes on quick print 1:
- blue line is quadA
- yellow line is interrupt for quadA
- show delay (810uS - 550uS = 0.33ms)
- frequency = 1.01kHz, do some math to get speed
- also corroborate that with speed readou of 2.49 rev/s

on print 4
- blue line is quadA
- yellow line is polled for quadA
- printed out motor speed of 2.49 rev/s... twice as fast??
- delay is 620 uS, a bit under twice the delay of interrupts
- 1kHz vs 980Hz frequency. do some math to get speed (apparent vs actual?)
- 

HOURS like 10!



## AI response

Notes on AI response:
- didn't compile at first
- had to change APB1ENR to APB1ENR1
- had to change RCC_APB1ENR_TIM2EN to RCC_APB1ENR1_TIM2EN
- eventually, code compiled and ran but nothing printed
- timer perhaps was not fully enabled/configured?
- also structure wasn't super organied (eg timer bit manipulation in the TIM2_IRQ)
- minimal use of helper functions

To interface with a quadrature encoder using the STM32L432KC, you can utilize the External Interrupt/Event Controller (EXTI) along with the Timer Encoder Interface for more precise handling. STM32L432KC has built-in support for quadrature encoder inputs on certain pins, making it easier to trigger interrupts based on the encoder’s pulses.

Pin Selection:

For the STM32L432KC, you can use Timer Channels to directly interface with the quadrature encoder, as the timers have built-in quadrature encoder interfaces.

The encoder’s two signals (A and B) will typically be connected to Timer Channels. Below is the most common pin configuration:

Encoder A (Phase A) → PA0 or PB6 (depending on which Timer you want to use)

Encoder B (Phase B) → PA1 or PB7 (again, depending on the Timer)

These pins are connected to timers that support the encoder interface, particularly TIM2, TIM3, TIM4, and TIM5.

For instance:

PA0 → TIM2_CH1 (Channel 1) for Encoder A

PA1 → TIM2_CH2 (Channel 2) for Encoder B

Steps for Interrupt Handling:

Configure Timer to capture encoder signals:
You’ll configure a timer to work in encoder mode to automatically count the pulses from the encoder. The timer will increment or decrement the counter based on the A/B phase transitions.

Set up Interrupt on Encoder:
You’ll set an interrupt to trigger based on the encoder’s movement (position changes). You can trigger an interrupt on either overflow, underflow, or a specific position.

Configure NVIC (Nested Vectored Interrupt Controller):
Ensure that the interrupt from the timer is enabled in the NVIC to handle the interrupt requests.

Example Code:

Here’s how you can set up the interrupts and configure the timer for the encoder:

1. Pin Configuration:

You will need to configure the GPIO pins for alternate function mode (AF) to connect to the timer channels.

// Configure PA0 and PA1 as alternate function (AF mode) for Timer
GPIOA->MODER &= ~(GPIO_MODER_MODE0_Msk | GPIO_MODER_MODE1_Msk); // Clear PA0 and PA1 mode
GPIOA->MODER |= (GPIO_MODER_MODE0_1 | GPIO_MODER_MODE1_1); // Set PA0 and PA1 to alternate function
GPIOA->AFR[0] |= (0x2 << GPIO_AFRL_AFSEL0_Pos); // Set AF2 for PA0 (TIM2_CH1)
GPIOA->AFR[0] |= (0x2 << GPIO_AFRL_AFSEL1_Pos); // Set AF2 for PA1 (TIM2_CH2)

2. Timer Configuration for Encoder Mode:
// Enable the TIM2 clock
RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

// Configure TIM2 in Encoder Mode
TIM2->PSC = 0;  // Prescaler (no division)
TIM2->ARR = 0xFFFF;  // Auto-reload register (max count)
TIM2->CR1 = TIM_CR1_CEN;  // Enable the timer

// Set the encoder mode (e.g., Encoder mode 1: Count rising edges of encoder signals)
TIM2->SMCR = (TIM_SMCR_SMS_0 | TIM_SMCR_SMS_1);  // Encoder Mode 1 (TI1 + TI2)

TIM2->DIER |= TIM_DIER_UIE;  // Enable update interrupt (overflow interrupt)

// Enable NVIC for TIM2 interrupt
NVIC_EnableIRQ(TIM2_IRQn);

3. Interrupt Handler:
void TIM2_IRQHandler(void) {
    if (TIM2->SR & TIM_SR_UIF) {  // Check if interrupt was caused by update event (overflow)
        TIM2->SR &= ~TIM_SR_UIF;  // Clear the interrupt flag
        // Handle your encoder position change here
        uint32_t encoder_position = TIM2->CNT;  // Read the counter value
        // Example: Print or process the encoder position
    }
}

Explanation of Key Components:

SMCR (Slave Mode Control Register): This is set to configure the timer for encoder mode. In Encoder Mode 1, the timer counts based on rising edges of both Encoder A (TIM2_CH1) and Encoder B (TIM2_CH2).

DIER (DMA/Interrupt Enable Register): This is used to enable the timer update interrupt (UIE), which will trigger an interrupt when the counter overflows or reaches a predefined limit.

CNT (Counter Register): The value in this register will give you the current position of the encoder (the number of pulses counted).

TIM2_IRQn: The interrupt request for TIM2, used to handle the overflow interrupt.

Additional Considerations:

Debouncing: If the encoder is noisy, you might need to debounce the signals either in hardware or software.

Timer Clock: The frequency of the encoder counting is influenced by the timer clock. Ensure your timer's prescaler and the encoder's maximum speed are compatible.

Direction Detection: You can also use the direction bit (DIR in SMCR) to determine whether the encoder is moving clockwise or counterclockwise.

Debugging Tips:

You can set breakpoints in the interrupt handler to verify that the encoder’s state is being read correctly.

Monitor the timer's counter register to track the encoder's position and direction.

Let me know if you need further clarification or assistance with a specific part!
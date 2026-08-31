#include "servo.h"

void Servo_Init(void)
{
    /* 1. Enable Clock for GPIOA and TIM3 */
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA, ENABLE);
    RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_TIM3, ENABLE);

    /* 2+3. Configure GPIO PA1 as Alternate Function for TIM3 CH2
       (This SDK does not have GPIO_ConfigPinAf — set via GPIO_Alternate field instead) */
    GPIO_InitType GPIO_InitStructure;
    GPIO_InitStruct(&GPIO_InitStructure);                   /* Previously: GPIO_StructInit */
    GPIO_InitStructure.Pin            = SERVO_PIN;
    GPIO_InitStructure.GPIO_Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStructure.GPIO_Speed     = GPIO_SPEED_HIGH;
    GPIO_InitStructure.GPIO_Pull      = GPIO_PULL_UP;
    GPIO_InitStructure.GPIO_Alternate = SERVO_AF_AFx;       /* Replaces GPIO_ConfigPinAf */
    GPIO_InitPeripheral(SERVO_PORT, &GPIO_InitStructure);   /* Previously: GPIO_Init */

    /* 4. Configure TIM3 for 50Hz frequency (Standard Servo) */
    extern uint32_t SystemCoreClock; // Fetch the actual system clock speed at runtime

    TIM_TimeBaseInitType TIM_TimeBaseStructure;
    TIM_InitTimBaseStruct(&TIM_TimeBaseStructure);          
    
    // Change only this line to enable automatic calculation
    TIM_TimeBaseStructure.Prescaler = (SystemCoreClock / 1000000) - 1; 
    
    TIM_TimeBaseStructure.CntMode   = TIM_CNT_MODE_UP;
    TIM_TimeBaseStructure.Period    = 20000 - 1;           
    TIM_TimeBaseStructure.ClkDiv    = TIM_CLK_DIV1;
    TIM_InitTimeBase(TIM3, &TIM_TimeBaseStructure);

    /* 5. Configure PWM Channel 2 (PA1) */
    OCInitType TIM_OCInitStructure;
    TIM_InitOcStruct(&TIM_OCInitStructure);                 /* Previously: TIM_OCStructInit */
    TIM_OCInitStructure.OcMode      = TIM_OCMODE_PWM1;
    TIM_OCInitStructure.OutputState = TIM_OUTPUT_STATE_ENABLE;
    TIM_OCInitStructure.Pulse       = 1500;                 /* Initialize at 0 degrees */
    TIM_OCInitStructure.OcPolarity  = TIM_OC_POLARITY_HIGH;
    TIM_InitOc2(TIM3, &TIM_OCInitStructure);                /* Previously: TIM_OC2Init */

    /* 6. Enable Preload register (required for PWM output) */
    TIM_ConfigOc2Preload(TIM3, TIM_OC_PRE_LOAD_ENABLE);
    TIM_ConfigArPreload(TIM3, ENABLE);

    /* 7. Enable the Timer */
    TIM_Enable(TIM3, ENABLE);
}

void Servo_SetAngle(uint8_t angle)
{
    uint32_t pulse_width;
    if (angle > 180) angle = 180;

    /* Map 0-180 degrees to 1000-2000 microseconds pulse width */
    pulse_width = 1000 + ((angle * 1000) / 180);

    /* Send value to TIM3 Channel 2 Register */
    TIM3->CCDAT2 = (uint16_t)pulse_width;
}

#include <stdio.h>
#include <stdlib.h>
#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <ctype.h>
#include <string.h>

#include "common.h"
#include "main.h"

TIM_HandleTypeDef tim1;
TIM_OC_InitTypeDef sConfig;

ParserReturnVal_t dcinit(int mode)
{
    if(mode != CMD_INTERACTIVE) return CmdReturnOk;
    

    // Enable TIM1 clock
    __HAL_RCC_TIM1_CLK_ENABLE();
    
    // Initialize TIM11 parameters
    tim1.Instance = TIM1;
    tim1.Init.Prescaler = HAL_RCC_GetPCLK2Freq() / 1000 - 1;
    tim1.Init.CounterMode = TIM_COUNTERMODE_UP;
    tim1.Init.Period = 100;
    tim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    tim1.Init.RepetitionCounter = 0;
    tim1.Init.AutoReloadPreload = 0;
    HAL_TIM_Base_Init(&tim1);

    printf("Timer initialized\n");
    
    // Initialize PWM configuration
    
    sConfig.OCMode = TIM_OCMODE_PWM1;
    sConfig.Pulse = 0; // Initial duty cycle
    sConfig.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfig.OCNPolarity = TIM_OCNPOLARITY_LOW;
    sConfig.OCFastMode = TIM_OCFAST_DISABLE;
    sConfig.OCIdleState = TIM_OCIDLESTATE_RESET;
    sConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;
    HAL_TIM_PWM_ConfigChannel(&tim1, &sConfig, TIM_CHANNEL_1);

    // Initialize GPIO configuration for PWM output
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitStruct.Pin = GPIO_PIN_8; // Set the pin number accordingly
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = 1; // Update with correct alternate function
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // Initialize GPIO configuration for normal output
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1; // Set the pin number accordingly
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = 0; // Update with correct alternate function
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);


    HAL_TIM_PWM_Start(&tim1, TIM_CHANNEL_1);
    
    return CmdReturnOk;
}

ParserReturnVal_t direction(int mode) {
    if (mode != CMD_INTERACTIVE) return CmdReturnOk;

    HAL_TIM_PWM_Start(&tim1, TIM_CHANNEL_1);

    
    uint16_t val, gear;
    printf("Enter '1' for For '2' for Rever and '0' for Brake\n");
    fetch_uint16_arg(&val);
    fetch_uint16_arg(&gear);


    switch(val)
{
        case 0:
                 TIM1->CCR1 = 0; 
                 HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,0);
                 HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,0);
                 break;
        case 1:
                 TIM1->CCR1 = gear*20; 
                 HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,1);
                 HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,0);
                 HAL_Delay(5000);
                 break;
        case 2:
                 TIM1->CCR1 = gear*20; 
                 HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,0);
                 HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,1);
                 HAL_Delay(5000);
                 break;
        default: printf("Please enter 0,1 or 2\n");
    }


    return CmdReturnOk;
}

ParserReturnVal_t Auto(int mode) {
    if (mode != CMD_INTERACTIVE) return CmdReturnOk;

    HAL_TIM_PWM_Start(&tim1, TIM_CHANNEL_1);
    uint16_t delay;
    fetch_uint16_arg(&delay);
         while(1)
         {
                 TIM1->CCR1 = 50; 
                 HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,0);
                 HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,1);
                 HAL_Delay(delay);
                 HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,1);
                 HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,0);
                 HAL_Delay(delay);
         }

    return CmdReturnOk;
}

ADD_CMD("dcinit", dcinit, "Motor initilization");
ADD_CMD("dir", direction, "Enter '1' for For '2' for Rever and '0' for Brake And Gear number from 1 to 5 for speed");
ADD_CMD("AutoMode", Auto, "Motor rotate CLockwise and Anticlockwise with desired milisecond delay");

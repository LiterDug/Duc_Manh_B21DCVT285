/*
**
**                           Main.c
**
**
**********************************************************************/
/*
   Last committed:     $Revision: 00 $
   Last changed by:    $Author: $
   Last changed date:  $Date:  $
   ID:                 $Id:  $

**********************************************************************/
#include "stm32f10x.h"
#define SERVO_H

/********************************************< Cấu hình delay TIM2   **********************************************/
void TIM2_INIT()
{
	TIM_TimeBaseInitTypeDef timerInit;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);// Cấp clock cho TIM 2
	timerInit.TIM_CounterMode = TIM_CounterMode_Up; // Cài đặt cho TIM 2
	timerInit.TIM_Period = 0xFFFF;
	timerInit.TIM_Prescaler = 72 - 1;
	TIM_TimeBaseInit(TIM2, &timerInit);
	TIM_Cmd(TIM2, ENABLE);
}

void Delay1Ms(void)

{	TIM_SetCounter(TIM2, 0);
	while (TIM_GetCounter(TIM2) < 1000) {
	}
}

void delay_us(uint32_t delay)
{
	TIM_SetCounter(TIM2, 0);
	while (TIM_GetCounter(TIM2) < delay) {
	}
}

void Delay_Ms(uint32_t u32DelayInMs)
{
	while (u32DelayInMs) {
		Delay1Ms();
		--u32DelayInMs;
	}
}

/********************************************< PWM- TIM4- Channel1   **********************************************/
#define PWM_TIM TIM4
#define PWM_PIN GPIO_Pin_9
#define PWM_PORT GPIOB

typedef struct
{
	TIM_TimeBaseInitTypeDef *htim;
	TIM_TimeBaseInitTypeDef *timInit;
	TIM_OCInitTypeDef *pwmInit;
	uint16_t chanel;
	uint8_t angle;

}Servo;

void PWM_INIT()
{
	GPIO_InitTypeDef gpioInit;
	TIM_TimeBaseInitTypeDef timInit;
	TIM_OCInitTypeDef pwmInit;

	/* PB9 out */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); /**< Clock */
	gpioInit.GPIO_Mode = GPIO_Mode_AF_PP;
	gpioInit.GPIO_Pin = PWM_PIN;
	gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(PWM_PORT, &gpioInit);

	/* timer 4 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); /**< Clock */
	timInit.TIM_CounterMode = TIM_CounterMode_Up;
	timInit.TIM_Prescaler = 3600 - 1;
	timInit.TIM_Period = 40-1; //  => f = 500Hz => T = 2ms
	timInit.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInit(PWM_TIM, &timInit);
	TIM_Cmd(PWM_TIM, ENABLE);

	/* Channel 1 with duty 50% */
	pwmInit.TIM_OCMode = TIM_OCMode_PWM1;
	pwmInit.TIM_OCPolarity = TIM_OCPolarity_High;
	pwmInit.TIM_OutputState = TIM_OutputState_Enable;
//    pwmInit.TIM_Pulse = (50*40)/100;
//	TIM_OC4Init(PWM_TIM, pwmInit);
}


uint8_t map(uint8_t x, uint8_t in_min, uint8_t in_max, uint8_t out_min, uint8_t out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void servo_write(uint8_t angle, TIM_OCInitTypeDef* pwmInit)
{
    uint8_t new_TIM_Pulse = map(angle,10,180,19,99);
    pwmInit->TIM_Pulse = (new_TIM_Pulse*40)/100;
	TIM_OC4Init(PWM_TIM, pwmInit);
}

void servo_write2(uint8_t angle, Servo* sv)
{
    uint8_t new_TIM_Pulse = map(angle,10,180,19,99);
    sv->pwmInit->TIM_Pulse = (new_TIM_Pulse*40)/100;
	TIM_OC4Init(PWM_TIM, sv->pwmInit);
}

uint16_t servo_read2(Servo* sv)
{
	return sv->angle;
}



int main(void)
{
    TIM2_INIT();

	GPIO_InitTypeDef gpioInit;
	TIM_TimeBaseInitTypeDef tim4Init;
	TIM_OCInitTypeDef pwmInit;

	/* PB9 out */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	gpioInit.GPIO_Mode = GPIO_Mode_AF_PP;
	gpioInit.GPIO_Pin = GPIO_Pin_9;
	gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &gpioInit);

	/* timer 4 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	tim4Init.TIM_CounterMode = TIM_CounterMode_Up;
	tim4Init.TIM_Prescaler = 3600 - 1;
	tim4Init.TIM_Period = 40-1; //  => f = 500Hz => T = 2ms
	tim4Init.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIM4, &tim4Init);
	TIM_Cmd(TIM4, ENABLE);

	/* Channel 1 with duty 50% */
	pwmInit.TIM_OCMode = TIM_OCMode_PWM1;
	pwmInit.TIM_OCPolarity = TIM_OCPolarity_High;
	pwmInit.TIM_OutputState = TIM_OutputState_Enable;

	while (1) {

	servo_write(10, &pwmInit);
    Delay_Ms(1000);

	servo_write(180, &pwmInit);
    Delay_Ms(1000);

	}
}




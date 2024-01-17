#ifndef SERVO_H
#define SERVO_H

#include "stm32f10x.h"

#define PWM_TIM TIM4
#define PWM_PIN GPIO_Pin_9
#define PWM_PORT GPIOB

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

/********************************************< Struct type(not finish)   **********************************************/
typedef struct
{
	TIM_TimeBaseInitTypeDef *htim;
	TIM_TimeBaseInitTypeDef *timInit;
	TIM_OCInitTypeDef *pwmInit;
	uint16_t chanel;
	uint8_t angle;

}Servo;

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


#endif

































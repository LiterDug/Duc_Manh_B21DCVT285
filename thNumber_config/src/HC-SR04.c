#include <HC-SR04.h>



static void initPins() {
    GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = US_TRIG_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(US_TRIG_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = US_ECHO_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(US_ECHO_PORT, &GPIO_InitStructure);
}

void HCSR04_Init() {
    initPins();

	//Enable Clock();
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	//////////////////////////////////////////////////////

	RCC_ClocksTypeDef RCC_ClocksStatus;
	RCC_GetClocksFreq(&RCC_ClocksStatus);
	uint16_t prescaler = RCC_ClocksStatus.SYSCLK_Frequency / 1000000 - 1; //1 tick = 1us = 0.1724mm

	TIM_DeInit(US_TIMER);               // Khởi tạo cấu hình cho TIM3:
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_TimeBaseInitStruct.TIM_Prescaler = prescaler;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period = 0xFFFF;
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInit(US_TIMER, &TIM_TimeBaseInitStruct);

	TIM_OCInitTypeDef TIM_OCInitStruct; // Cấu hình chế độ PWM cho đầu ra của TIM3:
	TIM_OCStructInit(&TIM_OCInitStruct);
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStruct.TIM_Pulse = 15; //us
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC3Init(US_TIMER, &TIM_OCInitStruct);

	TIM_ICInitTypeDef TIM_ICInitStruct; // Cấu hình bộ đếm thời gian và cài đặt chế độ PWM:
	TIM_ICInitStruct.TIM_Channel = TIM_Channel_1;
	TIM_ICInitStruct.TIM_ICPolarity = TIM_ICPolarity_Rising;
	TIM_ICInitStruct.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInitStruct.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInitStruct.TIM_ICFilter = 0;
	TIM_PWMIConfig(US_TIMER, &TIM_ICInitStruct);

	TIM_SelectInputTrigger(US_TIMER, US_TIMER_TRIG_SOURCE);     // chọn nguồn kích hoạt
	TIM_SelectMasterSlaveMode(US_TIMER, TIM_MasterSlaveMode_Enable);// cấu hình chế độ làm việc (ở đây là chế độ Master/Slave được kích hoạt).

	TIM_CtrlPWMOutputs(US_TIMER, ENABLE);   // bật đầu ra PWM.

	TIM_ClearFlag(US_TIMER, TIM_FLAG_Update);
}

u16 HCSR04_GetDistance() {
	(US_TIMER)->CNT = 0;
	TIM_Cmd(US_TIMER, ENABLE);
	while(!TIM_GetFlagStatus(US_TIMER, TIM_FLAG_Update));
	TIM_Cmd(US_TIMER, DISABLE);
	TIM_ClearFlag(US_TIMER, TIM_FLAG_Update);
	return (TIM_GetCapture2(US_TIMER)-TIM_GetCapture1(US_TIMER))*0.177; //////mm=//=>cm=T*0.0177=(1s/58)
}

#include <HC-SR04.h>

static void initPins() {
    GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = US_TRIG_PIN; // PB0
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(US_TRIG_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = US_ECHO_PIN; // PA6
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


	RCC_ClocksTypeDef RCC_ClocksStatus;
	RCC_GetClocksFreq(&RCC_ClocksStatus);
	uint16_t prescaler = RCC_ClocksStatus.SYSCLK_Frequency / 1000000 - 1; //1 tick = 1us = 0.1724mm

// Khởi tạo TIM
	TIM_DeInit(US_TIMER);   // Reset US_TIMER
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_TimeBaseInitStruct.TIM_Prescaler = prescaler;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period = 0xFFFF;
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;    // Thiết lập chia tỷ lệ giữa đồng hồ vào bộ đếm thời gian.
	TIM_TimeBaseInit(US_TIMER, &TIM_TimeBaseInitStruct);

// Cấu hình chế độ PWM Output Capture cho US_TIMER- kênh 3
	TIM_OCInitTypeDef TIM_OCInitStruct;
	TIM_OCStructInit(&TIM_OCInitStruct);    // Khởi tạo giá trị mặc định
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;  // Bật đầu ra PWM trên chân GPIO
	TIM_OCInitStruct.TIM_Pulse = 15;    // Thiết lập độ rộng xung 15us
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;  // Xung bắt đầu từ mức cao
	TIM_OC3Init(US_TIMER, &TIM_OCInitStruct);   // Lưu cấu hình vào kênh 3

//  cấu hình chế độ PWM Input Capture cho US_TIMER
	TIM_ICInitTypeDef TIM_ICInitStruct;
	TIM_ICInitStruct.TIM_Channel = TIM_Channel_1; // IC channel
	TIM_ICInitStruct.TIM_ICPolarity = TIM_ICPolarity_Rising;    // cực của xung cần được đo: Rising edge
	TIM_ICInitStruct.TIM_ICSelection = TIM_ICSelection_DirectTI;// chọn trực tiếp đầu vào từ chân ngoại vi
	TIM_ICInitStruct.TIM_ICPrescaler = TIM_ICPSC_DIV1;  // Thiết lập bộ chia tỷ lệ cho đầu vào 1:1
	TIM_ICInitStruct.TIM_ICFilter = 0;  //  áp dụng một bộ lọc trung bình trên k chu kỳ xung
	TIM_PWMIConfig(US_TIMER, &TIM_ICInitStruct);

	TIM_SelectInputTrigger(US_TIMER, US_TIMER_TRIG_SOURCE);     // chọn nguồn kích hoạt =>  Triggers từ kênh 1, mỗi lần xảy ra cạnh dương.
	TIM_SelectMasterSlaveMode(US_TIMER, TIM_MasterSlaveMode_Enable);// chế độ Master/Slave được kích hoạt

// bật đầu ra PWM từ US_TIMER cho cả Input Capture và Output Capture
	TIM_CtrlPWMOutputs(US_TIMER, ENABLE);

	TIM_ClearFlag(US_TIMER, TIM_FLAG_Update);
}

u16 HCSR04_GetDistance() {
	(US_TIMER)->CNT = 0;    // Reset thanh ghi đếm của US_TIMER
	TIM_Cmd(US_TIMER, ENABLE);
	while(!TIM_GetFlagStatus(US_TIMER, TIM_FLAG_Update));   // Đợi bộ đếm thời gian đạt tới giá trị đếm tối đa
	TIM_Cmd(US_TIMER, DISABLE);
	TIM_ClearFlag(US_TIMER, TIM_FLAG_Update);
	return (TIM_GetCapture2(US_TIMER)-TIM_GetCapture1(US_TIMER))*0.0177; //////mm=//=>cm=T*0.0177=(1s/58)
}

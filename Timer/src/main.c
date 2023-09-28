#include "stm32f10x.h"


// cau hinh led chan PB2
void GPIO_configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}
// cau hinh timer 4
void TIM4_configuration(void)
{
    TIM_TimeBaseInitTypeDef TIM_InitStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

    TIM_InitStructure.TIM_ClockDivision = 0;
    TIM_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_InitStructure.TIM_Period = 2000 - 1; // 1 giây 2000 clock
    TIM_InitStructure.TIM_Prescaler = 72000000-1; // (72x10^6 / 2000) - 1
    TIM_InitStructure.TIM_RepetitionCounter = 0;

    TIM_TimeBaseInit(TIM4, &TIM_InitStructure);
    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE); //enable update (interrup
    TIM_Cmd(TIM4, ENABLE); // khởi động TIM4
}
// su kien ngat
void NVIC_configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_PriorityGroupConfig( NVIC_PriorityGroup_0); // ko có mức ưu tiên chính
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; // mức ưu tiên phụ bằng 0

    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;    // bật kênh ngắt
    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;    // kết nối kênh ngắt với TIM4

    NVIC_Init(&NVIC_InitStructure);
}

void TIM4_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET) //if update flagturn on
    {
        TIM_ClearITPendingBit(TIM4, TIM_IT_Update); //clear update flag
        GPIO_WriteBit(GPIOB, GPIO_Pin_2,
                      (BitAction)(1^GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_2)));
    }
}
int main(void)
{
    SystemInit();
    GPIO_configuration();
    TIM4_configuration();
    NVIC_configuration();
    while(1)
    { }
}

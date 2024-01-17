#include "stm32f10x.h"

/**< Cấu hình delay TIM2   *******************************************************************/
#define TIMclock TIM2
void GPIO_INIT()
{
    TIM_TimeBaseInitTypeDef timerInit;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); /**< Cấp clock cho TIM  */
    timerInit.TIM_CounterMode = TIM_CounterMode_Up; // Cài đặt cho TIM 2
    timerInit.TIM_Period = 0xFFFF;
    timerInit.TIM_Prescaler = 72 - 1;
    TIM_TimeBaseInit(TIMclock, &timerInit);
    TIM_Cmd(TIMclock, ENABLE);
}
void delay_us(uint32_t delay)
{
    TIM_SetCounter(TIMclock, 0);
    while (TIM_GetCounter(TIMclock) < delay)
    {
    }
}

void delay_ms(uint32_t u32DelayInMs)
{
    while (u32DelayInMs)
    {
        TIM_SetCounter(TIMclock, 0);
        while (TIM_GetCounter(TIMclock) < 1000)
        {
        }
        --u32DelayInMs;
    }
}

/*************************< Cấu hinh led chan PB2 *******************************************************************/
void GPIO_configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/**********************************************< Cấu hình TIM4  ********************************************/
// cau hinh timer 4
void TIM4_configuration(void)
{
    TIM_TimeBaseInitTypeDef TIM_InitStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

    TIM_InitStructure.TIM_ClockDivision = 0;
    TIM_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_InitStructure.TIM_Period = 10000 - 1;
    TIM_InitStructure.TIM_Prescaler = 7200-1; /// 72MHz / (10000 * 7200) = 1Hz
    TIM_InitStructure.TIM_RepetitionCounter = 0;

    TIM_TimeBaseInit(TIM4, &TIM_InitStructure);
    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE); //enable update (interrupt)
    TIM_Cmd(TIM4, ENABLE); // khởi động TIM4
}


/**********************************************< Cấu hình ngắt  ********************************************/
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
        GPIO_WriteBit(GPIOB, GPIO_Pin_2,  (BitAction)(1^GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_2)));
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

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
#include "stm32f10x_conf.h"

int main(void)
{
    while(1)
    {

    }
}

void TIM3_IRQHandler(void)
{
#if 1
    if((TIM_GetITStatus(TIM3, TIM_IT_CC1)==SET))
    {
        TIM_ClearITPendingBit(TIM3, TIM_IT_CC1);
        GetFrequencyCycleCount(NUM1, TIM_GetCapture1(TIM3));
    }
#endif

#if 2

    if((TIM_GetITStatus(TIM3, TIM_IT_CC2)==SET))
    {
        TIM_ClearITPendingBit(TIM3, TIM_IT_CC2);
        GetFrequencyCycleCount(NUM2, TIM_GetCapture2(TIM3));
    }
#endif
}

void TIM3_TimerSet(void)

{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    NVIC_InitTypeDef NVIC_InitStructure;
    TIM_ICInitTypeDef  TIM_ICInitStructure;
    /* TIM2 clock enable */

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    /* GPIOA clock enable */

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    /* Enable the TIM3 global Interrupt */

    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6|GPIO_Pin_7;

    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP ;
    GPIO_Init(GPIOA, &GPIO_InitStructure);  //GPIO_PinAFConfig(GPIOA, GPIO_PinSource6|GPIO_PinSource7, GPIO_AF_TIM3);  GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_TIM3);  GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_TIM3);
#if 1

    TIM_TimeBaseStructure.TIM_Period = TIMER_PERIOD;
    TIM_TimeBaseStructure.TIM_Prescaler = 84-1;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
#endif
#if 1

    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    TIM_ICInitStructure.TIM_ICFilter = 0x0;
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
    TIM_PWMIConfig(TIM3, &TIM_ICInitStructure);
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;

    TIM_PWMIConfig(TIM3, &TIM_ICInitStructure);
#endif
#if 1  /* Select the TIM3 Input Trigger*/  TIM_SelectInputTrigger(TIM3, TIM_TS_TI1FP1|TIM_TS_TI2FP2);
    /* Select the slave Mode: Reset Mode */

    TIM_SelectSlaveMode(TIM3, TIM_SlaveMode_Reset);
    TIM_SelectMasterSlaveMode(TIM3,TIM_MasterSlaveMode_Enable);
#endif
    /* TIM enable counter */

    TIM_Cmd(TIM3, ENABLE);
    /* Enable the CC2 Interrupt Request */

    TIM_ITConfig(TIM3, TIM_IT_CC1|TIM_IT_CC2, ENABLE);
}
#endif


/**< Cấu hình ngắt ngoài B13  *******************************************************************/
void GPIO_EXTI(void);
void configEXTI(void);
void NVIC_EXTI();
// B1:cap xung va cau hinh chan nhan tin hieu ngat ngoai la ngo vao keo len
void GPIO_EXTI(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
//do ngat ngoai la chuc nang thay the nen phai bat AIFO

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource13);

    configEXTI();
    NVIC_EXTI();
}

// B2:Cau hinh va cho phep ngat ngoai o EXTI
void configEXTI(void)
{
    EXTI_InitTypeDef EXTI_InitStructure;
    EXTI_InitStructure.EXTI_Line = EXTI_Line13;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
}

// B3: cau hinh cap do uu tien va cho phep ngat ngoai o NVIC
void NVIC_EXTI()
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0); //0 cap PreemptionPriority va 16 cap SubPriority

    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}


// B4: Viet chuong trinh con phuc vu ngat ngoai chan
void EXTI15_10_IRQHandler(void)
{
    static uint32_t startTime = 0;
    uint32_t endTime = 0;
    TIM_SetCounter(TIM2, 0);

    if(EXTI_GetITStatus(EXTI_IMR_MR13) != RESET) // thay bằng EXTI_Line
    {
        EXTI_ClearITPendingBit(EXTI_IMR_MR13); // thay bằng EXTI_Line
        endTime = TIM_GetCounter(TIM2);
        uint8_t Distance = (endTime - startTime) * 0.0343 / 2.0;  // Calculate distance in cm

        if( Distance < 10)
        {
            GPIO_SetBits(GPIOB, GPIO_Pin_2);
            delay_ms(1000);
        }
        else
        {
            GPIO_ResetBits(GPIOB, GPIO_Pin_2);
            delay_ms(1000);
        }
    }
}

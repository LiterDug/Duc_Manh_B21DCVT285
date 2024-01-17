#include "main.h"
#include "stm32f10x.h"
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_usart.h"




/*******************************************************************************************/
#define TIMclock TIM2
void GPIO_INIT()
{
    TIM_TimeBaseInitTypeDef timerInit;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); /**< Cấp clock cho TIM  */
    timerInit.TIM_CounterMode = TIM_CounterMode_Up;
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

/*************************< Cấu hinh led chan PB2 ****/
void GPIO_configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}



/*******************************************************************************************/
#define  BAUD_3M     3000000
#define  BAUD_2M     2000000
#define  BAUD_1M5    1500000
#define  BAUD_1M     1000000
#define  BAUD_115200  115200
#define  BAUD_9600      9600

/*******************************************************************************************/

/**new retarget**/



int _write(int fd, char * ch, int len)
{

  USART_ClearFlag(USART1,USART_FLAG_TC);
    for (int i = len; i > 0; i--)
    {
        while (USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
        USART_SendData(USART1,*ch++);
    }
    while (USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET);

    return len;
}



/*******************************************************************************************/
void usart1_cfg_A9A10(uint32_t baud_rate) /// datasheet, trang 31, bang remap
{
    /// cap clock cho uart2
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE); /// Cáº¥p clock cho USART1R
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);/// Cáº¥p clock cho GPIOA

    /// cáº¥u hÃ¬nh gpio cho A9,A10
    GPIO_PinRemapConfig(GPIO_Remap_USART1,ENABLE);
    /// remap cÃ¡c pin cho USART1.
    /// datasheet, trang 31, bang remap

    GPIO_InitTypeDef GPIO_struct;
    GPIO_struct.GPIO_Pin = GPIO_Pin_9;
    GPIO_struct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_struct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA,&GPIO_struct);

    GPIO_struct.GPIO_Pin = GPIO_Pin_10;
    GPIO_struct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_struct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA,&GPIO_struct);


    /// cau hinh cho UART2

    USART_InitTypeDef USART_struct;
    USART_struct.USART_BaudRate = baud_rate;
    USART_struct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;    /// mode truyền
    USART_struct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_struct.USART_Parity = USART_Parity_No;    /// bit kiểm tra chẵn lẻ
    USART_struct.USART_StopBits = USART_StopBits_1;
    USART_struct.USART_WordLength = USART_WordLength_8b;
    USART_Init(USART1,&USART_struct);

    USART_ClearFlag(USART1,USART_FLAG_TC);          /// reset cờ truyền hoàn tất

    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);  /// Cấu hình ngắt cho sự kiện "Receive Data Register Not Empty,"
    NVIC_EnableIRQ(USART1_IRQn);                    /// cho phép hàm xử lí ngắt ở UART1
    NVIC_SetPriority(USART1_IRQn,1);

    USART_Cmd(USART1,ENABLE);
}

/*******************************************************************************************/

/**/

#define string_size 80
volatile uint8_t RX_available = 0;
volatile uint8_t RX_Buffer[string_size];
volatile uint8_t RX_index = 0;
volatile uint16_t receive_length = 0;

/**/

/*******************************************************************************************/
int main()
{
    usart1_cfg_A9A10(BAUD_115200);
    printf("hello world\n");

    GPIO_configuration();
    GPIO_INIT();
    while(1)
    {
//        GPIO_WriteBit(GPIOB, GPIO_Pin_2,  (BitAction)(1^GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_2)));

//        _write(1,"abcme",5);

    delay_ms(1000);
    printf("%d\n",RX_available);


        if(RX_available == 1)
        {
            GPIO_SetBits(GPIOB, GPIO_Pin_2);

            for(int i=0; i<receive_length; i++)
            {
                printf("%c",RX_Buffer[i]);
            }
            printf("\n");
            RX_available = 0;
        }
    }

}

void USART1_IRQHandler()
{
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        char temp_char = USART_ReceiveData(USART1);

        if(temp_char != '\n')
        {
            RX_Buffer[RX_index] = temp_char;
            RX_index++;
        }
        else
        {
            RX_Buffer[RX_index] = 0x00;
            receive_length = RX_index;
            RX_index = 0;
            RX_available = 1;
        }
    }
    USART_ClearITPendingBit(USART1, USART_IT_RXNE);
}




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
#define ADC1_DR_Address     0x4001244C
#define TIM1_CCR1_Address   0x40012C34

/*Khoi tao bien cau hinh*/
GPIO_InitTypeDef		GPIO_InitStructure;
ADC_InitTypeDef           ADC_InitStructure;
TIM_TimeBaseInitTypeDef   TIM_TimeBaseStructure;
TIM_OCInitTypeDef         TIM_OCInitStructure;
DMA_InitTypeDef           DMA_InitStructure;

void RCC_Configuration(void);
void GPIO_Configuration(void);
void Delay_ms(uint16_t time);
void ADC_Configuration(void);
void TIM_PWM_Configuration(void);
void DMA_Configuration(void);

int main(void)
{
    RCC_Configuration();
    GPIO_Configuration();
    ADC_Configuration();
    TIM_PWM_Configuration();
    DMA_Configuration();

    while (1)
    {

    }
}

void RCC_Configuration(void)
{
    /* ADCCLK = PCLK2/8 */
    RCC_ADCCLKConfig(RCC_PCLK2_Div8);

    /* Enable peripheral clocks ------------------------------------------------*/
    /* Enable DMA1 clock */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    /* Enable GPIOA, GPIOC, ADC1 and TIM1 Periph clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC |  RCC_APB2Periph_ADC1 | RCC_APB2Periph_TIM1, ENABLE);
}

void GPIO_Configuration(void)   /// PB9 input floating
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 ;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}
void Delay_ms(uint16_t time)
{
    uint32_t time_n=time*12000;
    while(time_n!=0)
    {
        time_n--;
    }

}

void ADC_Configuration(void)    /// PA0 Input ADC
{
    /*cap clock cho chan GPIO va bo ADC1*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 , ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    /*cau hinh chan Input cua bo ADC1 la chan PA0*/
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /*cau hinh ADC1*/
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;  // hoạt
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; /// ko sử dụng trigger ngoại vi để kích hoạt ADC
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;  // Dữ liệu được lưu trữ ở bên phải thanh ghi ADC
    ADC_InitStructure.ADC_NbrOfChannel = 1;                 // Số lượng kênh chuyển đổi là 1
    ADC_Init(ADC1, &ADC_InitStructure);

    /* Cau hinh chanel, rank, thoi gian lay mau */
    ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);
    /* Cho phep bo ADC1 hoa dong */
    ADC_Cmd(ADC1, ENABLE);
    /* cho phep cam bien nhiet hoat dong */
    ADC_TempSensorVrefintCmd(ENABLE);
    /* Reset thanh ghi cablib  */
    ADC_ResetCalibration(ADC1);
    /* Cho thanh ghi cablib reset xong */
    while(ADC_GetResetCalibrationStatus(ADC1));
    /* Khoi dong bo ADC */
    ADC_StartCalibration(ADC1);
    /* Cho trang thai cablib duoc bat */
    while(ADC_GetCalibrationStatus(ADC1));

    /* Bat dau chuyen doi ADC */
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}


void TIM_PWM_Configuration(void) /// A8 ouput PWM
{
    /* Configure TIM1 Channel1 output */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    /* TIM1 configuration ------------------------------------------------------*/
    /* Time Base configuration */
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Period = 0xFF0;
    TIM_TimeBaseStructure.TIM_Prescaler = 17646;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
    /* Channel1 Configuration in PWM mode */
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC1Init(TIM1, &TIM_OCInitStructure);

    /* Enable TIM1 */
    TIM_Cmd(TIM1, ENABLE);
    /* Enable TIM1 outputs */
    TIM_CtrlPWMOutputs(TIM1, ENABLE);

    /* Enable TIM1 DMA interface */
    TIM_DMACmd(TIM1, TIM_DMA_Update, ENABLE);
}

void DMA_Configuration(void)
{
    /* DMA1 Channel5 configuration ----------------------------------------------*/
    DMA_DeInit(DMA1_Channel5);

    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)TIM1_CCR1_Address;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)ADC1_DR_Address;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;

    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;

    DMA_InitStructure.DMA_BufferSize = 1;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;

    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel5, &DMA_InitStructure);
    /* Enable DMA1 Channel5 */
    DMA_Cmd(DMA1_Channel5, ENABLE);
}


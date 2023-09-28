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
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stdio.h"
#include "Delay.h"

#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker-
>Libraries->Small printf
set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

PUTCHAR_PROTOTYPE
{
    /* Place your implementation of fputc here */
    /* e.g. write a character to the LCD */
//lcd_Data_Write((u8)ch);
    USART_SendData(USART2,(u8)ch);
    /*Loop until the end of transmission */
    while
    (USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET)
    {}
    return ch;
}

/**< Cấu hình thNumber */
uint16_t dataPin  = GPIO_Pin_5;
uint16_t clockPin = GPIO_Pin_6;
uint16_t latchPin = GPIO_Pin_7;

#define LSBFIRST 0b10000000
#define MSBFIRST 0b01000000

#define test_f 0b00000001
#define dot 0b00000010
#define test_e 0b00000100
#define test_c 0b00001000
#define test_d 0b00010000
#define test_b 0b00100000
#define test_a 0b01000000
#define test_g 0b10000000

#define num_0 (test_a|test_b|test_c|test_d|test_e|test_f)
#define num_1 (test_b|test_c)
#define num_2 (test_a|test_b|test_d|test_e|test_g)
#define num_3 (test_a|test_b|test_c|test_d|test_g)
#define num_4 (test_b|test_c|test_f|test_g)
#define num_5 (test_a|test_c|test_d|test_f|test_g)
#define num_6 (test_a|test_c|test_d|test_e|test_f|test_g)
#define num_7 (test_a|test_b|test_c)
#define num_8 (test_a|test_b|test_c|test_d|test_e|test_f|test_g)
#define num_9 (test_a|test_b|test_c|test_d|test_f|test_g)

uint8_t digit[4] = {0B00000000, 0B00000000, 0B00000000, 0B00000000};

// Khai bao ham
void thNumberConfig();
uint8_t numTodigit(int num);
void shipOut(uint16_t dataPin, uint16_t clockPin, uint16_t bitOrder, uint16_t val);
void shipOut4Num();
void setNum(int num);
void setDot(int position);
void deleteDot(int position);

/*Khoi tao bien cau hinh*/
GPIO_InitTypeDef GPIO_InitStructure;
USART_InitTypeDef UART_InitStructure;
ADC_InitTypeDef ADC_InitStructure;

uint8_t data =10;
float value_adc1 = 0,sum_adc1=0, adc_tb=0;

void GPIO_Configuration(void);
void UART_Configuration (void);
void ADC_Configuration(void);

int main(void)
{

    ADC_Configuration();

    SysTick_Init();
    thNumberConfig();

    while(1)
    {


        for(int b=0; b<10; b++)
        {
            value_adc1 =ADC_GetConversionValue(ADC1);
            sum_adc1 = sum_adc1 +value_adc1;
            delay_ms(1);
        }
        adc_tb = sum_adc1/10;
        sum_adc1 = 0;

//            setNum(adc_tb);

        setNum(3000);

        delay_ms(1000);
    }
}
void GPIO_Configuration(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 ;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void UART_Configuration (void)
{
    /*Cap clock cho USART và port su dung*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
    /* Cau Tx mode AF_PP, Rx mode FLOATING */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode =GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed =GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode =GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    /*Cau hinh USART*/
    UART_InitStructure.USART_BaudRate = 115200;
    UART_InitStructure.USART_WordLength =USART_WordLength_8b;
    UART_InitStructure.USART_StopBits =USART_StopBits_1;
    UART_InitStructure.USART_Parity =USART_Parity_No;
    UART_InitStructure.USART_HardwareFlowControl =USART_HardwareFlowControl_None;
    UART_InitStructure.USART_Mode =USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART2, &UART_InitStructure);
    /* Cho phep UART hoat dong */
    USART_Cmd(USART2, ENABLE);
}
void ADC_Configuration(void)
{
    /*cap clock cho chan GPIO va bo ADC1*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 ,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
    /*cau hinh chan Input cua bo ADC1 la chan PA0*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    /*cau hinh ADC1*/
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStructure.ADC_ExternalTrigConv =
        ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStructure);/* Cau hinh chanel, rank, thoi gian lay mau */
    ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1,ADC_SampleTime_55Cycles5);
    /* Cho phep bo ADC1 hoa dong */
    ADC_Cmd(ADC1, ENABLE);
    /* cho phep cam bien nhiet hoat dong */
    ADC_TempSensorVrefintCmd(ENABLE);
    /* Reset thanh ghi cablib */
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

/**< Hàm cấu hình thNumber */
void thNumberConfig()
{
    GPIO_InitTypeDef GPIO_Struct;
// cap clock cho GPIOA
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
//cai dat cho chan
    GPIO_Struct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Struct.GPIO_Pin =  dataPin | clockPin | latchPin;
    GPIO_Struct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_Struct);
}


void shipOut(uint16_t dataPin, uint16_t clockPin, uint16_t bitOrder, uint16_t val)
{

    int bitt = 8;
    val = ~val;
    for (int i = 0; i < bitt; i++)
    {
        if(bitOrder == LSBFIRST)
        {
            GPIO_WriteBit(GPIOA, dataPin, val & (1<<i));
        }
        else
        {
            GPIO_WriteBit(GPIOA, dataPin, val & (1<<(bitt-1-i)));
        }
        GPIO_SetBits(GPIOA, clockPin);
        GPIO_ResetBits(GPIOA, clockPin);
    }
}

uint8_t numTodigit(int num)
{
    if(num == 0) return num_0;
    if(num == 1) return num_1;
    if(num == 2) return num_2;
    if(num == 3) return num_3;
    if(num == 4) return num_4;
    if(num == 5) return num_5;
    if(num == 6) return num_6;
    if(num == 7) return num_7;
    if(num == 8) return num_8;
    if(num == 9) return num_9;
}

void shipOut4Num()
{

    for(int i = 0; i < 4; i++)
    {
        shipOut(dataPin, clockPin, LSBFIRST, digit[i]);
    }
}

void setNum(int num)
{
    for(int i = 0; i < 4; i++)
    {
        digit[i] = numTodigit(num%10);
        num /= 10;
    }
    shipOut4Num();
    GPIO_SetBits(GPIOA, latchPin);
    delay_ms(1);
    GPIO_ResetBits(GPIOA, latchPin);
    delay_ms(1);
}

void setDot(int position)
{
    digit[position] |= dot;
}

void deleteDot(int position)
{
    digit[position] &= -dot;
}











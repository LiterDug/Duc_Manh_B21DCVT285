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
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "Delay.h"

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
#define num_5 (test_a|test_c|test_d|test_e|test_f|test_g)
#define num_6 (test_a|test_c|test_d|test_f|test_g)
#define num_7 (test_a|test_b|test_c)
#define num_8 (test_a|test_b|test_c|test_d|test_e|test_f|test_g)
#define num_9 (test_a|test_b|test_c|test_d|test_f|test_g)

uint8_t digit[4] = {0B00000000, 0B00000000, 0B00000000, 0B00000000};

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

// Khai bao ham
GPIO_InitTypeDef GPIO_Struct, GPIO_StructB;
uint8_t numTodigit(int num);
void shipOut(uint16_t dataPin, uint16_t clockPin, uint16_t bitOrder, uint16_t val);
void shipOut4Num();
void setNum(int num);
void setDot(int position);
void deleteDot(int position);

void thNumberConfig()
{
// cap clock cho GPIOA
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
//cai dat cho chan
    GPIO_Struct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Struct.GPIO_Pin =  dataPin | clockPin | latchPin;
    GPIO_Struct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_Struct);
}

int main(void)
{
    SysTick_Init();
    thNumberConfig();

    while(1)
    {
        setNum(1234);
    }
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






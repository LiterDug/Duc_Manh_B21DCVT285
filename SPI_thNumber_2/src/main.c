#include "stm32f10x.h"

void TIM2_config()
{
    TIM_TimeBaseInitTypeDef timerInit;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    timerInit.TIM_CounterMode = TIM_CounterMode_Up;
    timerInit.TIM_Period = 0xFFFF;
    timerInit.TIM_Prescaler = 72 - 1;
    TIM_TimeBaseInit(TIM2, &timerInit);
    TIM_Cmd(TIM2, ENABLE);
}

void Delay1Ms(void)
{

    TIM_SetCounter(TIM2, 0);
    while (TIM_GetCounter(TIM2) < 1000)
    {
    }
}

void Delay_Ms(uint32_t u32DelayInMs)
{

    while (u32DelayInMs)
    {
        Delay1Ms();
        --u32DelayInMs;
    }
}



/*********************************************< SPI 2 setup  *******************************/
#define MOSI GPIO_Pin_15
#define MISO GPIO_Pin_14
#define SCK GPIO_Pin_13
#define NSS GPIO_Pin_12
#define SPI_Port GPIOB

void SPI2_Master_Config()
{
    SPI_InitTypeDef spi2Init;
    GPIO_InitTypeDef gpioInit;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); /**< Clock  */
    /* PB12 - NSS =>  */
    gpioInit.GPIO_Mode = GPIO_Mode_Out_PP;
    gpioInit.GPIO_Pin = NSS;
    gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SPI_Port, &gpioInit);
    /* PB13 - SCK */
    gpioInit.GPIO_Mode = GPIO_Mode_AF_PP;
    gpioInit.GPIO_Pin = SCK;
    gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SPI_Port, &gpioInit);
    /* PB14 - MISO  */
    gpioInit.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    gpioInit.GPIO_Pin = MISO;
    gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SPI_Port, &gpioInit);
    /* PB15 - MOSI */
    gpioInit.GPIO_Mode = GPIO_Mode_AF_PP;
    gpioInit.GPIO_Pin = MOSI;
    gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SPI_Port, &gpioInit);
    /* SPI2 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
    spi2Init.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
    spi2Init.SPI_CPHA = SPI_CPHA_1Edge; /// Chốt data tại xung clock đầu tiên
    spi2Init.SPI_CPOL = SPI_CPOL_Low;   /// Clock đầu vào mức cao
    spi2Init.SPI_DataSize = SPI_DataSize_8b;    /// Kích thước 1 lần truyền data
    spi2Init.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    spi2Init.SPI_FirstBit = SPI_FirstBit_LSB;
    spi2Init.SPI_Mode = SPI_Mode_Master;    /// Tự điều khiển chân clock
    spi2Init.SPI_NSS = SPI_NSS_Soft;        /// điều khiển chân NSS mềm

    SPI_Init(SPI2, &spi2Init);  /**< SPI  */
    SPI_Cmd(SPI2, ENABLE);      /**< SPI  */
}



#define test_c 0b00000001
#define dot    0b00000010
#define test_b 0b00000100
#define test_f 0b00001000
#define test_a 0b00010000
#define test_e 0b00100000
#define test_d 0b01000000
#define test_g 0b10000000

#define num_0 (test_a|test_b|test_c|test_d|test_e|test_f)
#define num_1 (test_b|test_c)
#define num_2 (test_a|test_b|test_d|test_e|test_g)
#define num_3 (test_a|test_b|test_c|test_d|test_g)
#define num_4 (test_b|test_c|test_f|test_g)
#define num_6 (test_a|test_c|test_d|test_e|test_f|test_g)
#define num_5 (test_a|test_c|test_d|test_f|test_g)
#define num_7 (test_a|test_b|test_c)
#define num_8 (test_a|test_b|test_c|test_d|test_e|test_f|test_g)
#define num_9 (test_a|test_b|test_c|test_d|test_f|test_g)

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

#define BufferSize 4

uint8_t SPI_MASTER_Buffer_Tx[BufferSize];
uint8_t SPI_MASTER_Buffer_Rx[BufferSize];
uint8_t M_Tx = 0, M_Rx = 0;

void setNum(uint16_t num)
{

    for(int i=BufferSize-1; i >= 0; i--)
    {
        SPI_MASTER_Buffer_Tx[i] |= numTodigit(num%10);
        num /= 10;
    }

    for(M_Tx=0; M_Tx<BufferSize; M_Tx++)
    {
        SPI_I2S_SendData(SPI2, ~SPI_MASTER_Buffer_Tx[M_Tx]);
        Delay_Ms(1);
    }

    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY) == SET) {}
    GPIO_SetBits(GPIOB, NSS);
    Delay_Ms(1);
    GPIO_ResetBits(GPIOB, NSS);
    Delay_Ms(1);

}



int main(void)
{
    TIM2_config();
    SPI2_Master_Config();

    while (1)
    {
        setNum(5678);
    }

}

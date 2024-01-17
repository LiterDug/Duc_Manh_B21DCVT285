#include "stm32f10x.h"

#define BufferSize 32


GPIO_InitTypeDef 					GPIO_InitStructure;
SPI_InitTypeDef   				SPI_InitStructure;
NVIC_InitTypeDef 					NVIC_InitStructure;


// SPI1 : MASTER.
//SPI2	:	SLAVE

uint8_t SPI_MASTER_Buffer_Tx[BufferSize] =
{
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
    0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C,
    0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12,
    0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
    0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E,
    0x1F, 0x20
};

uint8_t SPI_MASTER_Buffer_Rx[BufferSize];

uint8_t SPI_SLAVE_Buffer_Tx[BufferSize];
uint8_t SPI_SLAVE_Buffer_Rx[BufferSize];

uint8_t M_Tx = 0, M_Rx = 0;
uint8_t S_Tx = 0, S_Rx = 0;
uint8_t Flag_end=0;

void GPIO_Configuration(void);
void SPI1_Master_Con(void);
void SPI2_Slave_Con(void);


int main(void)
{
    SysTick_Config(SystemCoreClock/1000);
    GPIO_Configuration();
    SPI1_Master_Con();
    SPI2_Slave_Con();
    delay_ms(10);
    while (1)
    {

        SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_RXNE, ENABLE);
        delay_ms(10);
        GPIO_WriteBit(GPIOB, GPIO_Pin_2, (BitAction) (0));
        S_Rx = 0;
        M_Tx = 0;
        for(M_Tx=0; M_Tx<32; M_Tx++)
        {
            while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) {}
            SPI_I2S_SendData(SPI1, SPI_MASTER_Buffer_Tx[M_Tx]);
        }
        delay_ms(1000);

//		if(Flag_end==1)
//		{
        Flag_end=0;
        SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_RXNE, DISABLE);
        GPIO_WriteBit(GPIOB, GPIO_Pin_2, (BitAction) (1));
        delay_ms(1000);
//		}
    }
}

void GPIO_Configuration(void)
{
    //RCC_PCLK2Config(RCC_HCLK_Div2);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 ;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 ;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_WriteBit(GPIOB, GPIO_Pin_2, (BitAction) (0));
}

/**************************************< SPI1 master config  *************************************/
/*
MASTERS
PA4 CS-GPIO_Mode_Out_PP
PA5 CLK-GPIO_Mode_Out_PP
PA6-MISO-GPIO_Mode_IN_FLOATING
PA7-MOSI-GPIO_Mode_AF_PP
*/

void SPI1_Master_Con(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1|RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5| GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_LSB;
    //SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI1, &SPI_InitStructure);

    SPI_Cmd(SPI1, ENABLE);
}

/**************************************< SPI2 slave config  *************************************/
/*
SLAVE
PB12-CS GPIO_Mode_AF_PP
PB13-CLK-GPIO_Mode_IN_FLOATING
PB14-MOSI - GPIO_Mode_AF_PP
PB15 MISO GPIO_Mode_IN_FLOATING
*/
void SPI2_Slave_Con(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_LSB;
    SPI_Init(SPI2, &SPI_InitStructure);

    SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_RXNE, DISABLE);
    SPI_Cmd(SPI2, ENABLE);

    /* Configure and enable SPI_SLAVE interrupt --------------------------------*/
    NVIC_InitStructure.NVIC_IRQChannel = SPI2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}




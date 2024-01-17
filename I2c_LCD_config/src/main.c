#include "stm32f10x.h"
#include "i2c_lcd.h"

void Delay1Ms(void);
void Delay_Ms(uint32_t u32DelayInMs);
void delay_us(uint32_t delay);

void Delay1Ms(void)
{

	TIM_SetCounter(TIM2, 0);
	while (TIM_GetCounter(TIM2) < 1000) {
	}
}

void delay_us(uint32_t delay)
{

	TIM_SetCounter(TIM2, 0);
	while (TIM_GetCounter(TIM2) < delay) {
	}
}

void Delay_Ms(uint32_t u32DelayInMs)
{

	while (u32DelayInMs) {
		Delay1Ms();
		--u32DelayInMs;
	}
}



#define SDA_0 GPIO_ResetBits(GPIOA, GPIO_Pin_0)
#define SDA_1 GPIO_SetBits(GPIOA, GPIO_Pin_0)
#define SCL_0 GPIO_ResetBits(GPIOA, GPIO_Pin_1)
#define SCL_1 GPIO_SetBits(GPIOA, GPIO_Pin_1)
#define SDA_VAL (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0))

void i2c_init(void);
void i2c_start(void);
void i2c_stop(void);
uint8_t i2c_write(uint8_t u8Data);
uint8_t i2c_read(uint8_t u8Ack);

void i2c_init(void) //Khởi tạo chân SDA-A0 và SCL_A1
{
	GPIO_InitTypeDef gpioInit;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	gpioInit.GPIO_Mode = GPIO_Mode_Out_OD;
	gpioInit.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	gpioInit.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(GPIOA, &gpioInit);

	SDA_1;
	SCL_1;
}

void i2c_start(void) // SDA chuyển từ 1->0  => SCL chuyển từ 1->0
{

	SCL_1;
	delay_us(3);
	SDA_1;
	delay_us(3);
	SDA_0;
	delay_us(3);
	SCL_0;
	delay_us(3);
}

void i2c_stop(void)  // SCL chuyển 0->1  =>  SDA chuyển 0->1
{

	SDA_0;
	delay_us(3);
	SCL_1;
	delay_us(3);
	SDA_1;
	delay_us(3);
}

uint8_t i2c_write(uint8_t u8Data)
{
	uint8_t i;
	uint8_t u8Ret;

    // Äiá»u khiá»ƒn chÃ¢n SDA Ä‘á»ƒ truyá»n 8 bit data Ä‘i, báº¯t Ä‘áº§u tá»« bÃªn trÃ¡i
	for (i = 0; i < 8; ++i) {
		if (u8Data & 0x80) {
			SDA_1;
		} else {
			SDA_0;
		}
		delay_us(3);

		// Táº¡o 1 xung clock
		SCL_1;
		delay_us(5);
		SCL_0;
		delay_us(2);

		u8Data <<= 1;   // Dá»‹ch bit sang trÃ¡i
	}

	SDA_1;  // Bit thá»© 9 á»Ÿ má»©c cao -> nháº­n tÃ­n hiá»‡u tá»« slave
	delay_us(3);
	SCL_1;
	delay_us(3);

	// Kiá»ƒm tra tÃ­n hiá»‡u xÃ¡c nháº­n cá»§a slave
	if (SDA_VAL) {
		u8Ret = 0;  // SDA á»Ÿ má»©c cao 1 -> slave ko xÃ¡c nháº­n
	} else {
		u8Ret = 1;  // SDA á»Ÿ má»©c tháº¥p 0 -> slave xÃ¡c nháº­n
	}
	delay_us(2);
	SCL_0;
	delay_us(5);

	return u8Ret;
}

uint8_t i2c_read(uint8_t u8Ack)
{
	uint8_t i;
	uint8_t u8Ret;

	SDA_1;      // Bit thá»© 9 á»Ÿ má»©c cao -> nháº­n tÃ­n hiá»‡u tá»« slave
	delay_us(3);

	for (i = 0; i < 8; ++i) {
		u8Ret <<= 1;    // Dá»‹ch bit sang trÃ¡i -> sáºµn sÃ ng nháº­n thÃªm bit má»›i vÃ o bÃªn pháº£i
		SCL_1;
		delay_us(3);
		if (SDA_VAL) {
			u8Ret |= 0x01; // LÆ°u giÃ¡ trá»‹ Ä‘á»c Ä‘Æ°á»£c tá»« SDA_VAL vÃ o u8Ret
		}
		delay_us(2);
		SCL_0;
		delay_us(5);
	}

	if (u8Ack) {
		SDA_0;     //  master xÃ¡c nháº­n nháº­n Ä‘Æ°á»£c dá»¯ liá»‡u
	} else {
		SDA_1;
	}
	delay_us(3);

    // Táº¡o 1 xung clock
	SCL_1;
	delay_us(5);
	SCL_0;
	delay_us(5);

	return u8Ret;
}

void send(uint8_t u8Data)   // Hàm gửi địa chỉ đến i2c
{
	uint8_t i;

	for (i = 0; i < 8; ++i) {
		if (u8Data & 0x80) {
			GPIO_SetBits(GPIOA, GPIO_Pin_0);
			Delay_Ms(4);
			GPIO_ResetBits(GPIOA, GPIO_Pin_0);
			Delay_Ms(1);
		} else {
			GPIO_SetBits(GPIOA, GPIO_Pin_0);
			Delay_Ms(1);
			GPIO_ResetBits(GPIOA, GPIO_Pin_0);
			Delay_Ms(4);
		}
		u8Data <<= 1;
	}
}

int main(void)
{
    uint8_t i, ACK, Address = 0;

	GPIO_InitTypeDef gpioInit;
	TIM_TimeBaseInitTypeDef timerInit;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); // Cáº¥p clock cho TIM 2

	gpioInit.GPIO_Mode = GPIO_Mode_Out_PP; // CÃ i Ä‘áº·t chÃ¢n C13
	gpioInit.GPIO_Pin = GPIO_Pin_13;
	gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &gpioInit);

	gpioInit.GPIO_Mode = GPIO_Mode_Out_PP; // CÃ i Ä‘áº·t chÃ¢n A0
	gpioInit.GPIO_Pin = GPIO_Pin_0;
	gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpioInit);

	timerInit.TIM_CounterMode = TIM_CounterMode_Up; // CÃ i Ä‘áº·t cho TIM 2
	timerInit.TIM_Period = 0xFFFF;
	timerInit.TIM_Prescaler = 72 - 1;
	TIM_TimeBaseInit(TIM2, &timerInit);
	TIM_Cmd(TIM2, ENABLE);

	// DÃ² Ä‘á»‹a chá»‰ cá»§a i2c
	for(i = 2; i < 255; ++i)
    {
        if(i%2 == 0)
        {
            i2c_start();
            ACK = i2c_write(i);
            i2c_stop();
            if(ACK)
            {
                 Address = i;
                 break;
            }
        }
    }



	i2c_init();

	I2C_LCD_Init();
	I2C_LCD_Clear();
	I2C_LCD_BackLight(1);
	I2C_LCD_Puts("Hello");
	I2C_LCD_NewLine();
	I2C_LCD_Puts("I2C: PA0 - PA1");

	while (1) {
        send(Address);
	}
}

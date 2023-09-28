#include "stm32f10x.h"
#include "i2c_lcd.h"

void GPIO_INIT()
{
    GPIO_InitTypeDef gpioInit;
    TIM_TimeBaseInitTypeDef timerInit;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); // Cấp clock cho TIM 2

    gpioInit.GPIO_Mode = GPIO_Mode_Out_PP; // Cài đặt chân C13
    gpioInit.GPIO_Pin = GPIO_Pin_13;
    gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &gpioInit);

    gpioInit.GPIO_Mode = GPIO_Mode_Out_PP; // Cài đặt chân A0
    gpioInit.GPIO_Pin = GPIO_Pin_0;
    gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpioInit);

    timerInit.TIM_CounterMode = TIM_CounterMode_Up; // Cài đặt cho TIM 2
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

void delay_us(uint32_t delay)
{

    TIM_SetCounter(TIM2, 0);
    while (TIM_GetCounter(TIM2) < delay)
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

void i2c_init(void) // Khởi tạo chân SDA- chân A0 và SCL- chân A1
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

void i2c_start(void) // SDA chuyển 1->0  =>  SCL chuyển 1->0
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

uint8_t i2c_write(uint8_t u8Data) // Hàm gửi 8 bit data đến i2c
{
    uint8_t i;
    uint8_t u8Ret;

    // Điều khiển chân SDA để truyền 8 bit data đi từ trái sang phải
    for (i = 0; i < 8; ++i)
    {
        if (u8Data & 0x80)
        {
            SDA_1;
        }
        else
        {
            SDA_0;
        }
        delay_us(3);

        // Tạo 1 xung clock
        SCL_1;
        delay_us(5);
        SCL_0;
        delay_us(2);

        u8Data <<= 1;   // Dịch bit sang trái
    }

    SDA_1;  // Bit thứ 9 ở mức cao -> nhận tín hiệu từ slave
    delay_us(3);
    SCL_1;
    delay_us(3);

    // Kiểm tra tín hiệu xác nhận của slave
    if (SDA_VAL)
    {
        u8Ret = 0;  // SDA ở mức cao 1 -> slave ko xác nhận
    }
    else
    {
        u8Ret = 1;  // SDA ở mức thấp 0 -> slave xác nhận
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

    SDA_1;      // Bit thứ 9 ở mức cao -> nhận tín hiệu từ slave
    delay_us(3);

    for (i = 0; i < 8; ++i)
    {
        u8Ret <<= 1;    // Dịch bit sang trái -> sẵn sàng nhận thêm bit mới vào bên phải
        SCL_1;
        delay_us(3);
        if (SDA_VAL)
        {
            u8Ret |= 0x01; // Lưu giá trị đọc được từ SDA_VAL vào u8Ret
        }
        delay_us(2);
        SCL_0;
        delay_us(5);
    }

    if (u8Ack)
    {
        SDA_0;     //  master xác nhận nhận được dữ liệu
    }
    else
    {
        SDA_1;
    }
    delay_us(3);

    // Tạo 1 xung clock
    SCL_1;
    delay_us(5);
    SCL_0;
    delay_us(5);

    return u8Ret;
}

void send(uint8_t u8Data)
{
    uint8_t i;

    for (i = 0; i < 8; ++i)
    {
        if (u8Data & 0x80)
        {
            GPIO_SetBits(GPIOA, GPIO_Pin_0);
            Delay_Ms(4);
            GPIO_ResetBits(GPIOA, GPIO_Pin_0);
            Delay_Ms(1);
        }
        else
        {
            GPIO_SetBits(GPIOA, GPIO_Pin_0);
            Delay_Ms(1);
            GPIO_ResetBits(GPIOA, GPIO_Pin_0);
            Delay_Ms(4);
        }
        u8Data <<= 1;
    }
}

uint8_t findAddress()
{
    uint8_t i, ACK, Address = 0;
    // Dò địa chỉ của i2c
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
    return Address;
}

int main(void)
{
    uint8_t Address = 0;
    Address =  findAddress();

    GPIO_INIT();
    i2c_init();
    I2C_LCD_Init();

    I2C_LCD_Clear();
    I2C_LCD_BackLight(1);
    I2C_LCD_Puts("Hello world");
    I2C_LCD_NewLine();
    I2C_LCD_Puts("I2C: PA0 - PA1");

    while (1)
    {
        send(Address);
        GPIO_SetBits(GPIOC, GPIO_Pin_13);
        Delay_Ms(500);
        GPIO_ResetBits(GPIOC, GPIO_Pin_13);
        Delay_Ms(500);
    }
}

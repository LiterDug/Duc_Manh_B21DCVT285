#ifndef I2C_LCD_H_
#define I2C_LCD_H_

#include "stm32f10x.h"
#include "i2c.h"
#include <stdint.h>

#define I2C_LCD_ADDR 0x4E

extern void Delay_Ms(uint32_t u32DelayInMs);

#define I2C_LCD_Delay_Ms(u16DelayMs) Delay_Ms(u16DelayMs)

#define LCD_BL 3
#define LCD_EN 2
#define LCD_RW 1
#define LCD_RS 0
#define LCD_D4 4
#define LCD_D5 5
#define LCD_D6 6
#define LCD_D7 7


void I2C_LCD_Init(void);
void I2C_LCD_Puts(char *szStr);
void I2C_LCD_Clear(void);
void I2C_LCD_NewLine(void);
void I2C_LCD_BackLight(uint8_t u8BackLight);

#endif

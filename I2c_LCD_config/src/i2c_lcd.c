#include "i2c.h"
#include "i2c_lcd.h"

static uint8_t u8LCD_Buff[8];//bo nho dem luu lai toan bo
static uint8_t u8LcdTmp;

#define	MODE_4_BIT		0x28
#define	CLR_SCR			0x01
#define	DISP_ON			0x0C
#define	CURSOR_ON		0x0E
#define	CURSOR_HOME		0x80
#define BLINK_OFF       0x0C

static void I2C_LCD_Write_4bit(uint8_t u8Data);
static void I2C_LCD_FlushVal(void);
static void I2C_LCD_WriteCmd(uint8_t u8Cmd);

void I2C_LCD_FlushVal(void)
{
	uint8_t i;

	for (i = 0; i < 8; ++i) {   // Lặp qua từng bit 0-7 trong buffer đưa vào bên trái biến nhớ
		u8LcdTmp >>= 1;
		if(u8LCD_Buff[i]) {
			u8LcdTmp |= 0x80;
		}
	}
	I2C_Write(I2C_LCD_ADDR, &u8LcdTmp, 1);  // Gửi biến nhớ đến i2c
}

void I2C_LCD_Init(void)
{
	uint8_t i;

	I2C_LCD_Delay_Ms(50);

	My_I2C_Init();

	for (i = 0; i < 8; ++i) {
		u8LCD_Buff[i] = 0;
	}

	I2C_LCD_FlushVal();

	u8LCD_Buff[LCD_RS] = 0; // Đặt bit RS = 0  =>  Cho phép gửi lệnh đến i2c
	I2C_LCD_FlushVal();

	u8LCD_Buff[LCD_RW] = 0;
	I2C_LCD_FlushVal();

	I2C_LCD_Write_4bit(0x03); // Gửi 4 bit dữ liệu tới màn hình LCD để cấu hình giao tiếp 4 bit.
	I2C_LCD_Delay_Ms(5);

	I2C_LCD_Write_4bit(0x03);
	I2C_LCD_Delay_Ms(1);

	I2C_LCD_Write_4bit(0x03);
	I2C_LCD_Delay_Ms(1);

	I2C_LCD_Write_4bit(MODE_4_BIT >> 4);
	I2C_LCD_Delay_Ms(1);

	I2C_LCD_WriteCmd(MODE_4_BIT);   //  Gửi lệnh tới màn hình LCD để cấu hình giao tiếp 4 bit.
	I2C_LCD_WriteCmd(DISP_ON);  // Bật hiển thị
	I2C_LCD_WriteCmd(CURSOR_ON);    // Bật con trỏ
	I2C_LCD_WriteCmd(CLR_SCR);  // Xóa màn hình
//	I2C_LCD_WriteCmd(BLINK_OFF);
}

void I2C_LCD_Write_4bit(uint8_t u8Data) // Gửi 4 bit dữ liệu tới màn hình LCD
{
	//4 bit can ghi chinh la 4 5 6 7 lưu vào buffer


	//sau do gan LCD_E=0

	if(u8Data & 0x08) {         //ghi du lieu vao buffer
		u8LCD_Buff[LCD_D7] = 1;
	} else {
		u8LCD_Buff[LCD_D7] = 0;
	}
	if(u8Data & 0x04) {
		u8LCD_Buff[LCD_D6] = 1;
	} else {
		u8LCD_Buff[LCD_D6] = 0;
	}
	if(u8Data & 0x02) {
		u8LCD_Buff[LCD_D5] = 1;
	} else {
		u8LCD_Buff[LCD_D5] = 0;
	}
	if(u8Data & 0x01) {
		u8LCD_Buff[LCD_D4] = 1;
	} else {
		u8LCD_Buff[LCD_D4] = 0;
	}

	u8LCD_Buff[LCD_EN] = 1; // Gán LCD_E=1  => sẵn sàng gửi dữ liêu đi
	I2C_LCD_FlushVal();

	u8LCD_Buff[LCD_EN] = 0; // Gán LCD_E=0  => kết thúc quá trình gửi dữ liệu.
	I2C_LCD_FlushVal();

}

void LCD_WaitBusy(void)
{
    char temp;

    // Đặt các bit thấp (4 bit thấp) của LCD_D4, LCD_D5, LCD_D6, LCD_D7 thành 1
    u8LCD_Buff[LCD_D4] = 1;
    u8LCD_Buff[LCD_D5] = 1;
    u8LCD_Buff[LCD_D6] = 1;
    u8LCD_Buff[LCD_D7] = 1;
    I2C_LCD_FlushVal();

    // Đặt RS (Register Select) thành 0 (gửi lệnh)
    u8LCD_Buff[LCD_RS] = 0;
    I2C_LCD_FlushVal();

    // Đặt RW (Read/Write) thành 1 (để đọc)
    u8LCD_Buff[LCD_RW] = 1;
    I2C_LCD_FlushVal();

    // Kiểm tra bit busy flag (bit thứ 7 của LCD) bằng cách gửi ENABLE
    do
    {
        u8LCD_Buff[LCD_EN] = 1;
        I2C_LCD_FlushVal();
        I2C_Read(I2C_LCD_ADDR + 1, &temp, 1);

        u8LCD_Buff[LCD_EN] = 0;
        I2C_LCD_FlushVal();

        u8LCD_Buff[LCD_EN] = 1;
        I2C_LCD_FlushVal();

        u8LCD_Buff[LCD_EN] = 0;
        I2C_LCD_FlushVal();
    } while (temp & 0x08);
}

void I2C_LCD_WriteCmd(uint8_t u8Cmd)
{

	LCD_WaitBusy();

	u8LCD_Buff[LCD_RS] = 0;
	I2C_LCD_FlushVal();

	u8LCD_Buff[LCD_RW] = 0;
	I2C_LCD_FlushVal();

	I2C_LCD_Write_4bit(u8Cmd >> 4);
	I2C_LCD_Write_4bit(u8Cmd);
}

void LCD_Write_Chr(char chr)
{

	LCD_WaitBusy();
	u8LCD_Buff[LCD_RS] = 1;
	I2C_LCD_FlushVal();
	u8LCD_Buff[LCD_RW] = 0;
	I2C_LCD_FlushVal();
	I2C_LCD_Write_4bit(chr >> 4);
	I2C_LCD_Write_4bit(chr);

}

void I2C_LCD_Puts(char *sz)
{

	while (1) {
		if (*sz) {
			LCD_Write_Chr(*sz++);
		} else {
			break;
		}
	}
}

void I2C_LCD_Clear(void)
{

	I2C_LCD_WriteCmd(CLR_SCR);
}

void I2C_LCD_NewLine(void)
{

	I2C_LCD_WriteCmd(0xc0);
}

void I2C_LCD_BackLight(uint8_t u8BackLight)
{

	if(u8BackLight) {
		u8LCD_Buff[LCD_BL] = 1;
	} else {
		u8LCD_Buff[LCD_BL] = 0;
	}
	I2C_LCD_FlushVal();
}

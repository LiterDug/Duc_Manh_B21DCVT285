#include <stm32f10x.h>
#include "I2C.h"

void My_I2C_Init(void)
{
	i2c_init();
}

uint8_t I2C_Write(uint8_t Address, uint8_t *pData, uint8_t length)
{
    uint8_t i;

    // Bắt đầu truyền (start) trên giao diện I2C.
    i2c_start();

    // Ghi địa chỉ của thiết bị. Nếu không thành công, dừng truyền và trả về 0.
    if (i2c_write(Address) == 0) {
        i2c_stop();
        return 0;
    }

    // Ghi lần lượt các byte dữ liệu từ mảng pData.
    for (i = 0; i < length; ++i) {
        // Nếu ghi không thành công, dừng truyền và trả về 0.
        if (i2c_write(pData[i]) == 0) {
            i2c_stop();
            return 0;
        }
    }

    // Kết thúc truyền (stop).
    i2c_stop();

    // Trả về 1 nếu quá trình ghi thành công.
    return 1;
}


uint8_t I2C_Read(uint8_t Address, uint8_t *pData, uint8_t length)
{
    uint8_t i;

    // Bắt đầu truyền (start) trên giao diện I2C.
    i2c_start();

    // Ghi địa chỉ của thiết bị. Nếu không thành công, dừng truyền và trả về 0.
    if (i2c_write(Address) == 0) {
        i2c_stop();
        return 0;
    }

    // Đọc lần lượt các byte dữ liệu từ thiết bị, trừ byte cuối cùng.
    for (i = 0; i < length - 1; ++i) {
        pData[i] = i2c_read(1);
    }

    // Đọc byte cuối cùng và gửi tín hiệu NACK.
    pData[i] = i2c_read(0);

    // Kết thúc truyền (stop).
    i2c_stop();

    // Trả về 1 nếu quá trình đọc thành công.
    return 1;
}


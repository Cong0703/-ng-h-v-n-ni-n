#ifndef DS3231_H
#define DS3231_H
#include "main.h"
typedef struct
{
	int8_t hour;
	int8_t min;
	int8_t sec;
	int8_t day;
	int8_t date;
	int8_t mon;
	int8_t year;
}DateTime;
void rtc_write_time(DateTime *dt);
void rtc_read_time(DateTime *dt);
uint8_t rtc_read_day_of_week(DateTime *dt);
uint8_t rtc_read_temp(DateTime *dt);
void rtc_init(I2C_HandleTypeDef *i2c);
void rtc_write_reg(uint8_t addr, uint8_t value); // ghi gia tri vao mot thanh ghi
uint8_t rtc_read_reg(uint8_t addr); // Doc gia tri tu mot thanh ghi
void rtc_read_Nreg(uint8_t addr,uint8_t *data, uint8_t count);  // Doc gia tri tu nhieu thanh ghi
#endif

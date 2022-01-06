#include "DS3231.h"
#define RTC_ADDR	(0x68<<1)
//cach 1
//extern I2C_HandleTypeDef hi2c1;
//cach 2
static I2C_HandleTypeDef *ds_i2c;

uint8_t Decimal2BCD(uint8_t num)
{ 
  return (num/10)<<4|(num%10);
}
uint8_t BCD2Decimal(uint8_t num)
{
 return (num>>4)*10+(num&0x0F);
}

void rtc_write_reg(uint8_t addr, uint8_t value)
{
	uint8_t data[2];
	data[0]=addr;
	data[1]=value;
	HAL_I2C_Master_Transmit(ds_i2c,RTC_ADDR,data,2,100);	
}

// Doc tu mot thanh ghi
uint8_t rtc_read_reg(uint8_t addr)
{
	uint8_t data;
	HAL_I2C_Master_Transmit(ds_i2c,RTC_ADDR,&addr,1,100);
	HAL_I2C_Master_Receive(ds_i2c,RTC_ADDR,&data,1,100);
	return data;
}
// doc tu N regitor
// doc tu thanh ghi dau tien 
void rtc_read_Nreg(uint8_t addr,uint8_t *data, uint8_t count)
{
	HAL_I2C_Master_Transmit(ds_i2c,RTC_ADDR,&addr,1,100);
	HAL_I2C_Master_Receive(ds_i2c,RTC_ADDR,data,count,100);
}

void rtc_write_time(DateTime *dt)
{
	uint8_t data[8];
	data[0] = 0x00;
	data[1] = Decimal2BCD(dt->sec);
	data[2] = Decimal2BCD(dt->min);
	data[3] = Decimal2BCD(dt->hour);
	data[4] = Decimal2BCD(dt->day);
	data[5] = Decimal2BCD(dt->date);
	data[6] = Decimal2BCD(dt->mon);
	data[7] = Decimal2BCD(dt->year);
	HAL_I2C_Master_Transmit(ds_i2c,RTC_ADDR,data,8,100);
}
void rtc_read_time(DateTime *dt)
{
	uint8_t data[7];
	uint8_t add_reg = 0x00;
	HAL_I2C_Master_Transmit(ds_i2c,RTC_ADDR,&add_reg,1,100);
	HAL_I2C_Master_Receive(ds_i2c,RTC_ADDR,data,7,100);
	dt->sec =  BCD2Decimal(data[0]);
	dt->min =  BCD2Decimal(data[1]);
	dt->hour = BCD2Decimal(data[2]);
	dt->day =  BCD2Decimal(data[3]);
	dt->date = BCD2Decimal(data[4]);
	dt->mon =  BCD2Decimal(data[5]);
	dt->year = BCD2Decimal(data[6]);
	
}

uint8_t rtc_read_temp(DateTime *dt)
{
	//uint8_t temp[2];
	uint16_t data_temp;
	uint8_t add_reg = 0x11;
	HAL_I2C_Master_Transmit(ds_i2c,RTC_ADDR,&add_reg,1,100);
	HAL_I2C_Master_Receive(ds_i2c,RTC_ADDR,(uint8_t *)&data_temp,2,100);
	//temp = ((uint16_t)data_temp[0] << 2)|( data_temp[1]>>6);
	return data_temp & 0x00FF;
}
uint8_t rtc_read_day_of_week(DateTime *dt)
{
		uint16_t d    = dt->date  ; 
    uint16_t m    = dt->mon   ; 
    uint16_t y    = 2000 + dt->year ; 
    uint8_t weekday  = (d += m < 3 ? y-- : y - 2, 23*m/9 + d + 4 + y/4- y/100 + y/400)%7;  
		return weekday;
}
void rtc_init(I2C_HandleTypeDef *i2c)
{
	ds_i2c = i2c;
}

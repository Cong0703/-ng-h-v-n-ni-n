#include "utils.h"
#include "delay_timer.h"
#include "DHT.h"
//--------------------utils-------------------------
void up_down_number(int8_t *number, int8_t max,int8_t min, uint8_t plus)
{
	if(plus)
	{
		(*number)++;
		if(	*number > 	max)
		{
			*number = min;
		}
	}
	else
	{
		(*number)--;
		if(	*number < 	min)
		{
			*number = max;
		}
	}
	
}
uint8_t check_leap_year(uint16_t year)
{
	if (((year % 4 == 0) && (year % 100!= 0)) || (year%400 == 0))
   
	{
		return 1;
	}
	return 0;
}
uint8_t get_max_date(uint8_t mon, uint16_t year)
{
	switch(mon)
	{
		case 1:case 3:case 5:case 7:case 8:case 10:case 12:
			return 31;
		case 4:case 6:case 9:case 11:
			return 30;
		case 2:
			if(check_leap_year(year))
			{
				return 29;
			}
	}
	return 28;
}


//---------------------DHT11-------------------------
/* cam bien doc nhiet do va do am*/
//DHT_Name DHT1;
//void dht_init(TIM_HandleTypeDef* htim4)
//{
// DHT_Init(&DHT1, DHT11, htim4, GPIOB, GPIO_PIN_14);
//}

//void read_temp_humi()
//{
//    DHT_ReadTempHum(&DHT1);
//	  HAL_Delay(1000);
//}


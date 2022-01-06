#include "clock_handle.h"
#include "LiquidCrystal_I2C.h"
#include "DS3231.h"
#include "Button.h"
#include "stdio.h"
#include "button_callback.h"
#include "stdlib.h"
#include "utils.h"
#include "delay_timer.h"
#include "DHT.h"
/*
Nhung ham xu ly thoi gian cho het vao day
chinh thoi gian
chinh alarm
nhap nhay...
*/
LiquidCrystal_I2C lcd;
ClockState clock_state = NORMAL_STATE;
DateTime date_time;

uint32_t time_start_press =0 ; // thoi gian ke tu luc nhan nut
int8_t hour_alarm, min_alarm;
extern DHT_Name DHT1;

void reset_time_start()
{
		time_start_press = HAL_GetTick();
}

const char arr_day[][3] = 
{
{"SU"},
{"MO"},
{"TU"},
{"WE"},
{"TH"},
{"FR"},
{"SA"}
};


//-----------------------adc1_IT-------------------------
/*
Ham nay de dieu chinh do sang cua den lcd, troi sang den tat, troi toi den mo
+ Khi den tat, nhan nut bat ki thi den sang
+ trong vong 5s khong co dieu chinh gi thi den tu tat
*/
static uint32_t t_off_led_backlight=0;
uint32_t adc1_value,adc2_value;
static uint8_t flag_adc1,flag_adc2;
static uint32_t check_btn_backled=0;


extern ADC_HandleTypeDef hadc2;
extern ADC_HandleTypeDef hadc1;
extern TIM_HandleTypeDef htim3;

void PWM_ADC_init(TIM_HandleTypeDef *htim,ADC_HandleTypeDef *hadc){
  HAL_TIM_PWM_Start(htim,TIM_CHANNEL_1);
//  HAL_ADCEx_Calibration_Start(hadc);	
	HAL_ADC_Start_IT(hadc);
}


 void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc){
    if(hadc->Instance==hadc1.Instance)
		{
		  adc1_value= HAL_ADC_GetValue(hadc);
		  if(adc1_value>3800) flag_adc1=1;
	    else if(adc1_value<=3800) flag_adc1 = 0;
		}
		if (hadc->Instance == hadc2.Instance){
		 adc2_value = HAL_ADC_GetValue(hadc);
		 flag_adc2=1;
	 }
}
 
void btn_backled()
{
 check_btn_backled = 1;
}

void handle_backlight_5s(){
if(flag_adc1==1) // bat den
	lcd_backlight_on(&lcd);
if (flag_adc1 ==0) //tat den
	{ 
	if (t_off_led_backlight==0) lcd_backlight_off(&lcd); // chua an nut 
	if(check_btn_backled==1)  // kiem tra xem co nut nao nhan khong ?
	{
	 lcd_backlight_on(&lcd);
	 t_off_led_backlight=HAL_GetTick();
	 check_btn_backled=0;
	}
	if (HAL_GetTick() - t_off_led_backlight>=5000)
		 {
	      if(check_btn_backled==0)
					{
          lcd_backlight_off(&lcd);
	        }
	   }
	 }
 }

//---------------mo_phong_lm35_adc2-------------------
static uint16_t pwm,temp=0;

void pwm_set_duty(TIM_HandleTypeDef *htim, uint32_t Channel, uint8_t duty){
uint16_t arr=htim->Instance->ARR;
	if (Channel==TIM_CHANNEL_1){
	htim->Instance->CCR1=(arr*duty)/100;
	}
	else if (Channel==TIM_CHANNEL_2){
	htim->Instance->CCR2=(arr*duty)/100;
	}
	else if (Channel==TIM_CHANNEL_3){
	htim->Instance->CCR3=(arr*duty)/100;
	}
	else if (Channel==TIM_CHANNEL_4){
	htim->Instance->CCR4=(arr*duty)/100;
	}
}


void set_up_DC(uint8_t signal_A4)
{
 HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,signal_A4);
 HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,0);
}

void handle_ADC_PWM()
{
//		HAL_ADC_Start(hadc);
//		HAL_ADC_PollForConversion(hadc,100);
//		adc_value= HAL_ADC_GetValue(hadc);
//		HAL_ADC_Stop(hadc);
		//adc : 0->4095
		//pwm : 0->100	
	if (flag_adc2)
	{ 
		pwm=adc2_value*100/4095;
		temp=pwm/2;
		pwm_set_duty(&htim3,TIM_CHANNEL_1,pwm);
		if (temp>=32)
		{
		set_up_DC(1);
		}
		else set_up_DC(0);
	}
}

void update_time_now()
{   	
		/* Do update time len man hinh la 300ms ma thoi gian tang la 200ms nen co 
		the bi tang 2 lan xong moi quay lai update time
		vi vay moi lan tang can update lai mot lan
		*/
		rtc_read_time(&date_time);
		uint8_t day_of_week = rtc_read_day_of_week(&date_time);
		lcd_set_cursor(&lcd,0,0);
		lcd_printf(&lcd,"%s %02d:%02d:%02d",arr_day[day_of_week],date_time.hour,date_time.min, date_time.sec);
		lcd_set_cursor(&lcd,1,0);
	  lcd_printf(&lcd,"%02d/%02d/20%02d T:%02d",date_time.date,date_time.mon, date_time.year,temp);

}

void update_alarm_now()
	{
/*
	Can mot ham update rieng cho alarm vi neu update_time_now cho mode alarm
	thi se bi chen man hinh cua cac mode khac vao mode alarm
	*/
    char line2[16];
		sprintf(line2,"%02d:%02d",hour_alarm,min_alarm);
		lcd_set_cursor(&lcd,0,0);
		lcd_printf(&lcd,"     ");
		lcd_set_cursor(&lcd,0,5);
		lcd_printf(&lcd,"ALARM");
		lcd_set_cursor(&lcd,0,10);
		lcd_printf(&lcd,"       ");
    lcd_set_cursor(&lcd,1,0);
		lcd_printf(&lcd,"     ");
		lcd_set_cursor(&lcd,1,5);
		lcd_printf(&lcd,"%s",line2);
		lcd_set_cursor(&lcd,1,10);
		lcd_printf(&lcd,"       ");
}
void clock_time_update()
{
	static uint32_t t_update = 0;
	update_time_now();
	if(HAL_GetTick() - t_update >= 500)
	{	
		update_time_now();
		t_update = HAL_GetTick();
	}
}
//--------------------blink --------------------------
void setting_blink()
{
	static uint32_t t_blink = 0;
	static uint8_t is_show = 1;
	char line1[16];
	char line2[16];
	
	if(HAL_GetTick() - t_blink >= 300)
	{
		is_show = !is_show;
		rtc_read_time(&date_time);
		uint8_t day_of_week = rtc_read_day_of_week(&date_time);
		sprintf(line1,"%s %02d:%02d:%02d",arr_day[day_of_week],date_time.hour,date_time.min, date_time.sec);
		sprintf(line2,"%02d/%02d/20%02d T:%02d",date_time.date,date_time.mon, date_time.year,temp);
		
		if(is_show)
		{
			switch(clock_state)
			{
			case SETTING_HOUR_STATE:
				line1[3] = line1[4] = ' ';
				break;
			case SETTING_MIN_STATE:
				line1[6] = line1[7] = ' ';
				break;
			case SETTING_DATE_STATE:
				line2[0] = line2[1] = ' ';
				break;
			case SETTING_MON_STATE:
				line2[3] = line2[4] = ' ';
				break;
			case SETTING_YEAR_STATE:
				line2[6] = line2[7]=line2[8] = line2[9] = ' ';
				break;
			default:
				break;
			}
		}
		
		lcd_set_cursor(&lcd,0,0);
		lcd_printf(&lcd,"%s",line1);
		lcd_set_cursor(&lcd,1,0);
		lcd_printf(&lcd,"%s",line2);
		
		t_blink = HAL_GetTick();
	}
}
//----------------adjust time-----------------------
void adjust_time(uint8_t plus)
{
	switch(clock_state)
	{
		case SETTING_HOUR_STATE:
		{
			up_down_number(&date_time.hour,23,0,plus);
			rtc_write_time(&date_time);
			update_time_now();
			break;
		}
		case SETTING_MIN_STATE:
		{
			up_down_number(&date_time.min,59,0,plus);
			rtc_write_time(&date_time);
			update_time_now();
			break;
		}
		case SETTING_DATE_STATE:
		{
			uint8_t max_day = get_max_date(date_time.mon, date_time.year);
			up_down_number(&date_time.date,max_day,1,plus);
			rtc_write_time(&date_time);
			update_time_now();
			break;
		}
		case SETTING_MON_STATE:
		{
			up_down_number(&date_time.mon,12,1,plus);
			uint8_t max_date = get_max_date(date_time.mon, date_time.year);
			if(date_time.date > max_date) // 31 >28
			{
				date_time.date = max_date;
			}
			rtc_write_time(&date_time);
			update_time_now();
			break;
		}
		case SETTING_YEAR_STATE:
		{
			up_down_number(&date_time.year,99,0,plus);
			rtc_write_time(&date_time);
			update_time_now();
			break;
		}
		case ALARM_HOUR_STATE:
		{
			up_down_number(&hour_alarm,23,0,plus);
			/*
			De gio bao thuc sau khi luu khong bi thay doi thi can luu vao thanh ghi hour_alarm va 
			minute alarm 
			Viet them mot ham rtc_write_reg trong lib.ds3231 de luu thoi gian vao thanh ghi
			*/
			rtc_write_reg(0x09,hour_alarm); // o nho 09 la alarm_hour
			update_alarm_now();
			break;
		}
		case ALARM_MIN_STATE:
		{
			up_down_number(&min_alarm,59,0,plus);
			rtc_write_reg(0x08,min_alarm);
			update_alarm_now();
			break;
		}
		default:break;
	}
}
//---------------------alarm-----------------

static uint32_t time_on=0; 
static uint8_t count=0;
static uint8_t turn_on=1;


void check_alarm()
{
	if(hour_alarm == date_time.hour && min_alarm == date_time.min)
	{ 
		if (HAL_GetTick() - time_on >= 1000)
			{
		   HAL_GPIO_TogglePin(GPIOA,GPIO_PIN_10);	
			 time_on=HAL_GetTick();
				count++;
		   }
  }
	if(hour_alarm != date_time.hour || min_alarm != date_time.min)
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_10,0);
}

void alarm_screen()
{
	static uint32_t t_blink = 0;
	static uint8_t is_show = 1;
	char line2[16];
	
	if(HAL_GetTick() - t_blink >= 300)
	{
		is_show = !is_show;
		sprintf(line2,"%02d:%02d",hour_alarm,min_alarm);
		if(is_show)
		{
			switch(clock_state)
			{
				case ALARM_HOUR_STATE:
					line2[0] = line2[1] = ' ';
					break;
				case ALARM_MIN_STATE:
					line2[3] = line2[4] = ' ';
					break;
				default:
					break;
			}
				
		}
		lcd_set_cursor(&lcd,0,0);
		lcd_printf(&lcd,"     ");
		lcd_set_cursor(&lcd,0,5);
		lcd_printf(&lcd,"ALARM");
		lcd_set_cursor(&lcd,0,10);
		lcd_printf(&lcd,"       ");
    lcd_set_cursor(&lcd,1,0);
		lcd_printf(&lcd,"     ");
		lcd_set_cursor(&lcd,1,5);
		lcd_printf(&lcd,"%s",line2);
		lcd_set_cursor(&lcd,1,10);
		lcd_printf(&lcd,"       ");
		t_blink = HAL_GetTick();
	}
}
//------------------------handle clock-----------------------------------
void clock_handle()
{
	switch(clock_state)
	{
		case NORMAL_STATE:
			clock_time_update();
			check_alarm();
			break;
		case SETTING_HOUR_STATE:
		case SETTING_MIN_STATE:
		case SETTING_DATE_STATE:
		case SETTING_MON_STATE:
		case SETTING_YEAR_STATE:
		{
			setting_blink();
			if (HAL_GetTick()-time_start_press >=5000)
			{
				clock_state = NORMAL_STATE;
				// khong can co vi neu den giay thu 5 co nut nhan thi no se reset lai cai time_start_press
			}
		}			
		break;
		case ALARM_HOUR_STATE:
		case ALARM_MIN_STATE:
		{
			alarm_screen();		
			if (HAL_GetTick()-time_start_press >=5000)
			{
				clock_state = NORMAL_STATE;
				// khong can co vi neu den giay thu 5 co nut nhan thi no se reset lai cai time_start_press	
			}
		}
		break;
		default:break;
	}
}

void clock_init(I2C_HandleTypeDef *hi2c1)
{
  lcd_init(&lcd,hi2c1,LCD_ADDR_DEFAULT);
	rtc_init(hi2c1);
  hour_alarm = rtc_read_reg(0x09);
	min_alarm  = rtc_read_reg(0x08);
}

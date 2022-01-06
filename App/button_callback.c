//Fix loi
/*
B1 : 
  + add button.h de co ham dinh nghia button_typedef
  + Nhung bien khoi tao o button_callback ma can dung o main thi be ca init sang button callback

Chu y : nhung ham duoc khai bao o button_Callback ma muon su dung o main thi co hai cach
!!!! loi bien nao thi cho bien y sang button_callback
+ c1 : tao 1 file.h va luu ten ham do vao
+ c2 : Dung ham extern
Bien nao chinh o ctc con nao thi de o ctc do !!!

B2 : Trong chuong trinh con nao ma can khoi tao init thi nen tao rieng mot ham init

Chu y 2 : Muon dung hi2c, hadc,.. thi dau vao cua chuong trinh con phai la cam bien do
ex : void clock_init(I2C_HandleTypeDef *hi2c1)

Chu y 3 : Static la bien chi dung rieng cho mot ham
extern : Dung lai mot bien da co o ham khac
vi vay khong dung extern + static
	*/

#include "button.h"
#include "button_callback.h"
#include "clock_handle.h"
#include "utils.h"

//-------------------callback button ------------------------
Button_Typdef btn_up, btn_ok, btn_down;
extern ClockState clock_state;





void	btn_pressing_callback(Button_Typdef *ButtonX)
{
	if(ButtonX == &btn_up)
	{
		btn_backled();
		adjust_time(1);
	}
	else if(ButtonX == &btn_down)
	{
		btn_backled();
		adjust_time(0);
	}
	reset_time_start();
}
void btn_press_short_callback(Button_Typdef *ButtonX )
{
	if(ButtonX == &btn_ok)
	{
		btn_backled();
		switch(clock_state)
		{
			case NORMAL_STATE:
				clock_state = SETTING_HOUR_STATE;
				break;
			case SETTING_HOUR_STATE:
				clock_state = SETTING_MIN_STATE;
				break;
			case SETTING_MIN_STATE:
				clock_state = SETTING_DATE_STATE;
				break;
			case SETTING_DATE_STATE:
				clock_state = SETTING_MON_STATE;
				break;
			case SETTING_MON_STATE:
				clock_state = SETTING_YEAR_STATE;
				break;
			case SETTING_YEAR_STATE:
				clock_state = NORMAL_STATE;
				break;
			case ALARM_HOUR_STATE:
				clock_state = ALARM_MIN_STATE;
				break;
			case ALARM_MIN_STATE:
				clock_state = NORMAL_STATE;
				break;
			default:
				break;
		}
	}
}
void btn_press_timeout_callback(Button_Typdef *ButtonX)
{ 
	reset_time_start();
	if(ButtonX == &btn_ok)
	{
		btn_backled();
		clock_state = ALARM_HOUR_STATE;
	}
}
void btn_press_timeout_repeat_callback(Button_Typdef *ButtonX)
{
  if(ButtonX == &btn_up)
	{
		btn_backled();
		adjust_time(1);

	}
	else if(ButtonX == &btn_down)
	{
		btn_backled();
		adjust_time(0);
	}
	reset_time_start();
}

void button_callback_init()
{
	button_init(&btn_down,GPIOA,GPIO_PIN_0);
	button_init(&btn_ok,GPIOA,GPIO_PIN_1);
	button_init(&btn_up,GPIOA,GPIO_PIN_2);

}

void button_callback_handle()
{
		button_handle(&btn_down);
		button_handle(&btn_up);
		button_handle(&btn_ok);
}



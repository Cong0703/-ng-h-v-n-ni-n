#ifndef CLOCK_HANDLE_H
#define CLOCK_HANDLE_H

#include "main.h"

typedef enum
{
	NORMAL_STATE,
	SETTING_HOUR_STATE,
	SETTING_MIN_STATE,
	SETTING_SEC_STATE,
	SETTING_DATE_STATE,
	SETTING_MON_STATE,
	SETTING_YEAR_STATE,
	ALARM_HOUR_STATE,
	ALARM_MIN_STATE
}ClockState;

void update_time_now();
void update_alarm_now();
void clock_time_update();
void setting_blink();
void adjust_time(uint8_t plus);
void check_alarm();
void alarm_screen();
void clock_handle();
void clock_init(I2C_HandleTypeDef *hi2c1);
void reset_time_start();
void handle_backlight_5s();
void btn_backled();
void handle_ADC_PWM();
void set_up_DC(uint8_t signal_A4);
void pwm_set_duty(TIM_HandleTypeDef *htim, uint32_t Channel, uint8_t duty);
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc);
void PWM_ADC_init(TIM_HandleTypeDef *htim,ADC_HandleTypeDef *hadc);

#endif

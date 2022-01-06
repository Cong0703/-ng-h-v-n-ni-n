#ifndef UTILS_H
#define UTILS_H
#include "stdint.h"
#include "main.h"

void up_down_number(int8_t *number, int8_t max,int8_t min, uint8_t plus);
uint8_t check_leap_year(uint16_t year);
uint8_t get_max_date(uint8_t mon, uint16_t year);
//void dht_init(TIM_HandleTypeDef* htim4);
//void read_temp_humi();
//void PWM_ADC_init(TIM_HandleTypeDef *htim,ADC_HandleTypeDef *hadc);
//void pwm_set_duty(TIM_HandleTypeDef *htim, uint32_t Channel, uint8_t duty);
//void handle_ADC_PWM(TIM_HandleTypeDef *htim,ADC_HandleTypeDef *hadc);

#endif

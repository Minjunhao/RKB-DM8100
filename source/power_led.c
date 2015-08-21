/**************************************************************************//**
 * @file     power_led.c
 * @brief    manage the power and the led
 * @version  V0.01
 * @date     13. August 2015
 *
 * @note
 * Copyright (C) 2015 ROTEL Limited. All rights reserved.
 *
 ******************************************************************************/
#include "power_led.h"

POWER_TYPE power_status;
LED_TYPE   led_status;
/*
   initialize the power status
*/
void power_status_initial(void)
{
   power_status.pwr_check_timer = 0;
   power_status.pwr_off_delay_counter = 0;
   power_status.pwr_status = MAIN_POWER_OFF;
}
void power_on(void)
{
   power_status.pwr_status = MAIN_POWER_ON;
   Standby(power_status.pwr_status);
}
void power_off(void)
{
   power_status.pwr_status = MAIN_POWER_OFF;
   Standby(power_status.pwr_status);  
}
/*
   turn the SMPS power on
*/
void SMPS_On(void)
{
   power_status.SMPS_power_status = SMPS_ON;
   SMPS_nPowerOn(power_status.SMPS_power_status);                //port=LowActive: AMP on
}
/*
   turn the SMPS power off
*/
void SMPS_Off(void)
{
   power_status.SMPS_power_status = SMPS_OFF;
   SMPS_nPowerOn(power_status.SMPS_power_status);
}
void AMP_On(void)
{
   power_status.amp_power_status = AMP_POWER_ON;
   amp_on(power_status.amp_power_status);
}
void AMP_Off(void)
{
   power_status.amp_power_status = AMP_POWER_OFF;
   amp_on(power_status.amp_power_status);   
}
/*
  led status initialize
*/
void led_status_initial(void)
{
   led_status.pwr_led_dim_counter = 0;
   led_status.pwr_led_mode = 0;
   led_status.ss_led_dim_counter = 0;
   led_status.ss_led_mode = 0;
}

void mode_power_on(void)
{
   switch(power_status.power_mode)
   	{
       case POWER_MODE_12V_TRIG:
	   case POWER_MODE_NORMAL: 
	   case POWER_MODE_SS_NORMAL:
	   	    power_on();
	     break;
	   case POWER_MODE_SS_QUICK:
	   	    power_on();
	   	 break;
	   default:
	   	 break;
    }
   fan_initial();
   amp_status_initial();
   SMPS_On();
   AMP_On();
}
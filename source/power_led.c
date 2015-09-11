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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "initmcu.h"
#include "uart.h"
#include "main.h"
#include "timer.h"
#include "adckey.h"
//#include "adckey.ext"
#include "eeprom.h"
#include "i2c.h"
#include "command_key.h"
#include "proc_function.h"
#include "fan_ctrl.h"
#include "signal_sense.h"
#include "power_led.h"
#include "ak4114.h"

#define MAX_TEMP        100
#define IS_TEMP(x)  ((x)<100)? 0:1

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
   power_status.pwr_status = MAIN_POWER_ON;   //SMPS POWER
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
   amp_on_off(power_status.amp_power_status);
}
void AMP_Off(void)
{
   power_status.amp_power_status = AMP_POWER_OFF;
   amp_on_off(power_status.amp_power_status);   
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

void pwr_led_on(void)
{
   led_status.pwr_led_mode=1;
   StandbyLED(led_status.pwr_led_mode);
}
void pwr_led_off(void)
{
   led_status.pwr_led_mode=0;
   StandbyLED(led_status.pwr_led_mode);
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

   
   DelayMs(200);
      
   pwr_led_on();
   //fan status initialize
   fan_initial();
   //amp status initialize
   amp_status_initial();
   //SMPS Power on
   SMPS_On();
   //amp power on
   AMP_On();

   reply_mode_power_status();
}
void mode_power_off(void)
{
   switch(power_status.power_mode)
   	{
       case POWER_MODE_12V_TRIG:
	   case POWER_MODE_NORMAL:
	   case POWER_MODE_SS_NORMAL:
	      power_off();
		  break;
	   case POWER_MODE_SS_QUICK:
	   	  power_off();
		  break;
    }


   SMPS_Off();
   AMP_Off();
}

void set_amp_shutdown_temp(u8 temp)
{
    if(IS_TEMP(temp)) return;
    power_status.amp_shut_down_temp=temp;
}
u8 get_amp_shutdown_temp(void)
{
    return (power_status.amp_shut_down_temp);
}
void set_smps_shutdown_temp(u8 temp)
{
    if(IS_TEMP(temp)) return;
	
    power_status.smps_shut_down_temp=temp;
}
u8 get_smps_shutdown_temp(void)
{
    return (power_status.smps_shut_down_temp);
}
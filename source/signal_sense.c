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
#include "power_led.h"
#include "signal_sense.h"

SS_TYPE ss_status;
AMP_TYPE   amp_status;

void signal_sense_initial(void)
{
   //ss_status.amp_status = 0;
    ss_status.chk_timer  = 0;
    ss_status.no_signal_counter= 0;
    ss_status.mode_on    = 0;
    ss_status.noise_counter = 0;
    //ss_status.reset_counter = 0;
}

void amp_status_initial(void)
{
    amp_status.check_timer = 0;
    amp_status.protect_status = AMP_PROTECT_ON;
    amp_status.protect_func_on= PROTECT_FUNCTION_OFF;
}
void amp_protection_on(void)
{
    if(amp_status.protect_status == AMP_PROTECT_ON) return;

	amp_status.protect_status=AMP_PROTECT_OFF;
	AMP_Off();
}
void amp_protection_off(void)
{
    if(amp_status.protect_status == AMP_PROTECT_OFF) return;

	amp_status.protect_status=AMP_PROTECT_ON;
	AMP_On();
}
/*
    signal sense check when amp power on
*/
void signal_sense_check_off(void)
{
    if(1==GetSignalSensing_nSW)
    	{
          if(SIGNAL_ON==GetSignalSense )
          	{
               ss_status.no_signal_counter=0;
		    }
		  else
		  	{
               ss_status.no_signal_counter++;
			   if((ss_status.no_signal_counter)>TIMER_SIGNAL_OFF_TIME)
			   	{
			   	  mode_mute_on(MUTE_ALL);
                  AMP_Off();
			    }
		    }
	    }
	else
		{
           ss_status.no_signal_counter=0;
	    }
}
/*
    signal sense check when amp power off
*/
void signal_sense_check_on(void)
{
    if(1==GetSignalSensing_nSW)
    	{
           if(SIGNAL_ON == GetSignalSense)
           	{
              if(++(ss_status.noise_counter) > 5)
              	{
                   AMP_On();
				   mode_mute_off(MUTE_ALL);
			    }
		    }
		   else
		   	{
              ss_status.noise_counter = 0;
		    }
	    }
	else
		{
           //CPU OFF?
	    }
}

void signal_sense_check(void)
{
    switch(power_status.amp_power_status)
    	{
           case AMP_POWER_ON:
		   	signal_sense_check_off();
		   	break;
		   case AMP_POWER_OFF:
		   	signal_sense_check_on();
		   	break;
	    }
}
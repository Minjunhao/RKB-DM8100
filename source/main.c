/**************************************************************************//**
 * @file     main.c
 * @brief    main function
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
#include "eeprom.h"
#include "i2c.h"
#include "power_led.h"
#include "proc_function.h"
#include "fan_ctrl.h"
#include "signal_sense.h"
#include "ak4117.h"
#include "command_key.h"
#include "njw1112.h"

#define GET_MODE_ID   (unsigned char)( ( GPIO_ReadInputData(GPIOE) >>3 )&0x0003 ) 


void check_timer_10ms(void);
#ifdef _TEST
void test_condition(void);
#endif

void main(void)
{
    u8 dummy,data;
  
    mode_AHB_prescaler = 1;  //for normal mode

    //structure initialize
    RKB_channel_initial();
  
    power_status_initial();

    led_status_initial();

	amp_status_initial();

    signal_sense_initial();	

	rs232_key_type_init();
  
    ini_InitMcu();

    //SMPS_Off();

    rkb_mode_id = GET_MODE_ID;

    uart_Init();

#ifdef UART4_ENABLE
    uart4_Init();		
#endif
#ifdef UART5_ENABLE
    uart5_Init();		
#endif  


    fan_initial();	

    adc_KeyInit();

    DelayMs(200);
  

#ifdef IAP_UPGRADE
    Initial_I2C();
    main_DisableUpgrade();
#endif

    dummy = nvr_xGetData(ADDR_DUMMYDUMMY);	//page0 setting!!!!!!!!!
    dummy = nvr_xGetData(ADDR_DUMMYDUMMY);	//page0 setting!!!!!!!!!
    dummy = nvr_xGetData(ADDR_DUMMYDUMMY);	//page0 setting!!!!!!!!!

    data = nvr_xGetData(ADDR_RESET);
    if(data == ON)
  	  {
        //reset function 
        mode_reset();
      }
    else
  	  {
        //power on
        if(Check_Backup()==TRUE)
      	  {
            mode_power_on();
	      }
	    else
	  	  {
            mode_reset();
	      }
      }

    
  	#ifdef _TEST
    DelayMs(2000);
	njw1112_write_start();
    DelayMs(1000);
	test_condition();
    #endif
    while(1)
  	  {
       switch(power_status.pwr_status)
       	{
         case MAIN_POWER_ON:
		     check_timer_10ms();
			 key_input_check();
			 temperature_read();
			 uart1_TxData();       
		    break;

         
		 case MAIN_POWER_OFF:
             key_input_check();
		    break;
	    }
      }
}
/*
    check timer every 10ms when power on
*/
void check_timer_10ms(void)
{
   if(timer_Get10ms()==TRUE)
   	{
      adc_KeyBoardPer10ms();                 //check the temperature

	  fan_timer_check();                     //check the fan status

	  mode_amp_protection_check();           //check the mode status

	  signal_sense_check();

	  mode_ak4117_check();
    }
}
#ifdef _TEST
void test_condition(void)
{
   u8 i,data=96;

   for(i=CHANNEL_A;i<=CHANNEL_D;i++)
   	{
      RKB_Channel[i].p_mute_ctrl(&RKB_Channel[i],MUTE_ON);
	  RKB_Channel[i].p_local_speaker_src_sel(&RKB_Channel[i],INPUT_A1_B1+i);
	  RKB_Channel[i].p_link_chanel_src_sel(&RKB_Channel[i],INPUT_A1_B1+i);
    }
    DelayMs(500);
    for(i=CHANNEL_A;i<=CHANNEL_D;i++)
    {
      RKB_Channel[i].p_mute_ctrl(&RKB_Channel[i],MUTE_OFF);
      RKB_Channel[i].p_vol_control(&RKB_Channel[i],0,0,data);
    }
}
#endif
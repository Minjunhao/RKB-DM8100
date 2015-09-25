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

#ifdef _ETHERNET
#include "netconf.h"
#endif

#define GET_MODE_ID   (unsigned char)( ( GPIO_ReadInputData(GPIOE) >>3 )&0x0003 ) 


typedef	void (*pFunction)(void);
#define ApplicationAddress    0x8000000
pFunction 	Jump_To_Application;
u32 		JumpAddress;

u16 task_initialize_time;
void main_DisableUpgrade(void);
void check_timer_10ms(void);
void check_timer_100ms(void);
void check_timer20ms(void);
void unit_status_check(void);
void task_initializing(void);
void task_suspend(void);
void task_running(void);
void main_iAPUpgrade(void);

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

    SMPS_Off();

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
           task_running();  		
		  break;
         case MAIN_POWER_INITIAL:
		   task_initializing(); 
		  break;
		 case MAIN_POWER_OFF:
		   task_suspend();	
		  break;
	    }

	   unit_status_check();
      }
}
/*
   initialize the task
*/
void task_initializing(void)
{
    check_timer20ms();
}
/*

*/
void task_running(void)
{
    check_timer_10ms();
	
    key_input_check();
	
    //uart1_TxData(); 
	
	#ifdef _ETHERNET
	EthernetMain();
	#endif
	
    rs232_CheckUpgrade();
 	if (rs232_GetUpgradeMode() == TRUE) 
		{
  			main_iAPUpgrade();
  		}
}
void main_DisableUpgrade(void)
{
	byte itemp[7] = {0};
//	EEPROM_Write(0x0010,itemp,7);		//0x0010 [page=0, addr=0x10]
	nvr_xPutData(0x10,itemp[0]);
	nvr_xPutData(0x11,itemp[1]);
	nvr_xPutData(0x12,itemp[2]);
	nvr_xPutData(0x13,itemp[3]);
	nvr_xPutData(0x14,itemp[4]);
	nvr_xPutData(0x15,itemp[5]);
	nvr_xPutData(0x16,itemp[6]);
}
void main_iAPUpgrade(void)
{
   byte itemp[7] = "upgrade";
	
   rs232_printf("if it is upgrade menu not appeared then AC power off and then ON.\n");
   uart1_TxData();

//	EEPROM_Write(0x0010,itemp,7);		//0x0010 [page=0, addr=0x10]
	nvr_xPutData(0x10,itemp[0]);
	nvr_xPutData(0x11,itemp[1]);
	nvr_xPutData(0x12,itemp[2]);
	nvr_xPutData(0x13,itemp[3]);
	nvr_xPutData(0x14,itemp[4]);
	nvr_xPutData(0x15,itemp[5]);
	nvr_xPutData(0x16,itemp[6]);
/*
	itemp[0] = nvr_xGetData(0x10);
	itemp[1] = nvr_xGetData(0x11);
	itemp[2] = nvr_xGetData(0x12);
	itemp[3] = nvr_xGetData(0x13);
	itemp[4] = nvr_xGetData(0x14);
	itemp[5] = nvr_xGetData(0x15);
	itemp[6] = nvr_xGetData(0x16);
*/
	DelayMs(1000);
  	ini_DisableInterrupt();
	rs232_SetUpgradeMode (OFF);

  	/* Test if upgrade code is programmed starting from address "ApplicationAddress" */
  	if (((*(vu32*)ApplicationAddress) & 0x2FFE0000 ) == 0x20000000)
  	{ 	/* Jump to upgrade application */
  	  	JumpAddress = *(vu32*) (ApplicationAddress + 4);
  	  	Jump_To_Application = (pFunction) JumpAddress;
  	  	/* Initialize upgrade application's Stack Pointer */
  	  	__MSR_MSP(*(vu32*) ApplicationAddress);
  	  	Jump_To_Application();
  	}
}
/*
  
*/
void task_suspend(void)
{
    key_input_check();  
	uart1_TxData(); 
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

	  temperature_read();

	  mode_input_check();

	  uart1_TxData(); 
    }
}
void unit_status_check(void)
{
    //check_timer_100ms();
    if(timer_Get100ms() == TRUE)
    	{
          if(((fan_status.flag) & (F_SMPS_TEMP|F_AMP_TEMP|F_TEMP_CHANGE)) != 0 && mode_display_update==0)
          	{
          	  fan_status.flag = 0x00;
              reply_current_temperature(ENTIRE_MODE);
		    }
		  if(((power_status.flag)&FLAG_POWER_STATUS) != 0)
		  	{
              power_status.flag = 0x00;
			  reply_mode_power_status(ENTIRE_MODE);
		    }
		  	
	    }    
}
#if 0
void check_timer_100ms(void)
{
    if(timer_Get100ms() == TRUE)
    	{
          if(((fan_status.flag) & (F_SMPS_TEMP|F_AMP_TEMP|F_TEMP_CHANGE)) != 0)
          	{
          	  fan_status.flag = 0x00;
			  if(mode_display_update==0)
			  	{
                  reply_current_temperature();
			  	}
		    }
		  
		  if(((power_status.flag)&FLAG_POWER_STATUS) != 0)
		  	{
              power_status.flag = 0x00;
			  reply_mode_power_status();
		    }
		  	
	    }
}
#endif
void check_timer20ms(void)
{
    if(timer_Get20ms() == TRUE)
    	{
          if(--task_initialize_time==0)
          	{
              mode_status_initial();
		    }
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
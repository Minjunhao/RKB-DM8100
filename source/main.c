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
//#include "adckey.ext"
#include "eeprom.h"
#include "i2c.h"


#include "proc_function.h"

#define GET_MODE_ID   (unsigned char)( ( GPIO_ReadInputData(GPIOE) >>3 )&0x0003 ) 


//#include "proc_display.h"

//#include "ak4114.ext"
#include "ak4117.h"

void main(void)
{
  u8 dummy,data;
  
  mode_AHB_prescaler = 1;  //for normal mode

  //structure initialize
  RKB_channel_initial();
  
  power_status_initial();

  led_status_initial();
  
  ini_InitMcu();

  SMPS_Off();

  fan_initial();

  signal_sense_initial();

  rkb_mode_id = GET_MODE_ID;

  power_status_initial();

  led_status_initial();

  uart_Init();

#ifdef UART4_ENABLE
  uart4_Init();		
#endif
#ifdef UART5_ENABLE
  uart5_Init();		
#endif  

  adc_KeyInit();

  amp_status_initial();

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
  
  while(1)
  	{
      
    }
}
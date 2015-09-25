/**************************************************************************//**
 * @file     njw1112.c
 * @brief    njw1112 register control
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
#include "main.h"
#include "uart.h"
#include "timer.h"
#include "proc_function.h"
#include "njw1112.h"

#define _DELAY_TIME         500              //3.735us
void njw1112_delay(u32 delay_time)
{
  
  while(--delay_time) {}
}
void njw1112_write_start(void)
{
   njw1112_clk_ctrl(NJW1112_HIGH);
   njw1112_lat_ctrl(NJW1112_HIGH);
}
/*
  write one byte to the njw1112(MSB)
*/
void njw1112_write_byte(u8 data)
{
   u8 i;

   for(i=0;i<8;i++)
   	{
   	  //njw1112_clk_ctrl(NJW1112_LOW);
	  njw1112_delay(_DELAY_TIME);     
      if(data & (0x80>>i))
      {
	  	njw1112_data_ctrl(NJW1112_HIGH);
      }
	  else
      {
	  	njw1112_data_ctrl(NJW1112_LOW);
      }

      njw1112_delay(_DELAY_TIME);
	  njw1112_clk_ctrl(NJW1112_LOW);
	  njw1112_delay(_DELAY_TIME);
	  njw1112_clk_ctrl(NJW1112_HIGH);
    }
}

/*
  write the register of the njw1112
*/
void njw1112_register_write(u8 data,u8 addr)
{
   njw1112_lat_ctrl(NJW1112_LOW);
   
   njw1112_write_byte(data);
   njw1112_write_byte(addr);

   njw1112_delay(_DELAY_TIME);
   njw1112_delay(_DELAY_TIME);
   njw1112_lat_ctrl(NJW1112_HIGH);
   njw1112_delay(_DELAY_TIME);
}
/*
  mute the njw1112 for the input channel chosing
*/
void njw1112_in_mute(u8 n_mute_on)
{
   if(n_mute_on == 0)
   	{
      njw1112_register_write(0x00,OUTPUT_A_ADDR | CHIP_ADDR_LOW);
	  njw1112_register_write(0x00,OUTPUT_B_ADDR | CHIP_ADDR_LOW);
	  njw1112_register_write(0x00,OUTPUT_C_ADDR | CHIP_ADDR_LOW);
	  njw1112_register_write(0x00,OUTPUT_D_ADDR | CHIP_ADDR_LOW);
    }
}
/*
  mute the njw1112 for the output channel chosing
*/
void njw1112_out_mute(u8 n_mute_on)
{
   if(n_mute_on == 0)
   	{
      njw1112_register_write(0x00,OUTPUT_A_ADDR | CHIP_ADDR_HIGH);
	  njw1112_register_write(0x00,OUTPUT_B_ADDR | CHIP_ADDR_HIGH);
	  njw1112_register_write(0x00,OUTPUT_C_ADDR | CHIP_ADDR_HIGH);
	  njw1112_register_write(0x00,OUTPUT_D_ADDR | CHIP_ADDR_HIGH);
    }
}
/*
  a function for the Channel_TYPE
*/

void njw1112_local_speaker_ctrl(Channel_TYPE *chunnel,u8 selector)
{
   //chip_addr=0x0a
   u8 channel_stat=chunnel->local_speaker_src;
   //if(selector == channel_stat) return;

   if(selector>INPUT_A8_B8) return;
   switch(chunnel->channel)
   	{
       case CHANNEL_A:
	   	njw1112_register_write(selector,OUTPUT_A_ADDR | CHIP_ADDR_LOW);	
	   	break;
	   case CHANNEL_B:
	   	njw1112_register_write(selector,OUTPUT_B_ADDR | CHIP_ADDR_LOW);
	   	break;
	   case CHANNEL_C:
	   	njw1112_register_write(selector,OUTPUT_C_ADDR | CHIP_ADDR_LOW);
	   	break;	
	   case CHANNEL_D:
	   	njw1112_register_write(selector,OUTPUT_D_ADDR | CHIP_ADDR_LOW);
		break;
    }
   chunnel->local_speaker_src=selector;  //update the data
}

void njw1112_link_speaker_ctrl(Channel_TYPE *chunnel,u8 selector)
{
   //chip_addr=0x0a
   u8 channel_stat=chunnel->link_channel_src;
   if(selector == channel_stat) return;

   switch(chunnel->channel)
   	{
       case CHANNEL_A:
	   	njw1112_register_write(selector,OUTPUT_A_ADDR | CHIP_ADDR_HIGH);
	   	break;
	   case CHANNEL_B:
	   	njw1112_register_write(selector,OUTPUT_B_ADDR | CHIP_ADDR_HIGH);
	   	break;
	   case CHANNEL_C:
	   	njw1112_register_write(selector,OUTPUT_C_ADDR | CHIP_ADDR_HIGH);
	   	break;	
	   case CHANNEL_D:
	   	njw1112_register_write(selector,OUTPUT_D_ADDR | CHIP_ADDR_HIGH);
		break;
    }
   chunnel->link_channel_src=selector;  //update the data
}
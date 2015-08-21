/**************************************************************************//**
 * @file     njw1194.c
 * @brief    njw1194 register control
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
#include "njw1194.h"
#define _DELAY_TIME         500              //3.735us
#define BALANCE_MAX         30               //CENTER:15
#define BALANCE_MIN         0

#define TREB_MAX            10
#define TREB_MIN            0

#define BASS_MAX            10
#define BASS_MIN            0

#define TREB_BST_nCUT       0x80
#define BASS_BST_nCUT       0x80

#define GET_BALANCE_LEFT_INX(x)       ((x)-((BALANCE_MAX)/2)-1)
#define GET_BALANCE_RIGHT_INX(x)       (((BALANCE_MAX)/2)-(x)-1)

const char balanceLevelTable[12][15] = {                      //20121101 : suda simple table
	{1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,14,16,0},       //0?
	{1, 2, 3, 4, 5, 6, 8,10,12,16,20,24,28,32,0},       //1?
	{1, 2, 3, 4, 5, 6, 8,10,12,16,20,24,28,32,0},       //2?
    {1, 2, 3, 4, 5, 6, 8,10,12,16,20,24,28,32,0},       //3?
    {1, 2, 3, 4, 5, 6, 8,10,12,16,20,24,28,32,0},       //4?
    {1, 2, 3, 4, 5, 6, 8,10,12,16,20,24,28,32,0},       //5?
    {1, 2, 3, 4, 5, 6, 8,10,12,16,20,24,28,32,0},       //6?
    {1, 2, 3, 4, 5, 6, 8,10,12,16,20,24,28,32,0},       //7?
    {1, 2, 3, 4, 5, 6, 8,10,12,16,20,24,28,32,0},       //8?
    {1, 2, 3, 4, 5, 6, 8,10,12,16,20,24,28,32,0},       //9?
    {1, 2, 3, 4, 5, 6, 8,10,12,16,20,24,28,32,0},       //10?
    {1, 2, 3, 4, 5, 6, 8,10,12,16,20,24,28,32,0}
};      //11?
    

void njw1194_delay(u32 delay_time)
{
  while(--delay_time) {}
}

void njw1194_write_start(void)
{
   njw1194_clk_ctrl(NJW1194_HIGH);
   njw1194_lat_ctrl(NJW1194_HIGH);
}
/*
  write one byte to the njw1112(MSB)
*/
void njw1194_write_byte(u8 data)
{
   u8 i;

   for(i=0;i<8;i++)
   	{
   	  
	  njw1194_delay(_DELAY_TIME);     
      if(data & (0x80>>i))
      {
	  	njw1194_data_ctrl(NJW1194_HIGH);
      }
	  else
      {
	  	njw1194_data_ctrl(NJW1194_LOW);
      }

      njw1194_delay(_DELAY_TIME);
	  njw1194_clk_ctrl(NJW1194_LOW);
	  njw1194_delay(_DELAY_TIME);
	  njw1194_clk_ctrl(NJW1194_HIGH);
    }
}
/*
  njw1194 write register
*/
void njw1194_write_register(u8 data, u8 addr)
{
   njw1194_lat_ctrl(NJW1194_LOW);
   
   njw1194_write_byte(data);
   njw1194_write_byte(addr);

   njw1194_delay(_DELAY_TIME);
   njw1194_delay(_DELAY_TIME);
   njw1194_clk_ctrl(NJW1194_HIGH);
   njw1194_delay(_DELAY_TIME);   
}
/*
  njw1194 ajust the volume of the left channel 
*/
void njw1194_vol_ctrl_left(u8 data,u8 chip_addr)
{
   njw1194_write_register(data,VOL_CTRL_LEFT | chip_addr);
}
/*
  njw1194 ajust the volume of the right channel 
*/
void njw1194_vol_ctrl_right(u8 data,u8 chip_addr)
{
   njw1194_write_register(data,VOL_CTRL_RIGHT | chip_addr);
}
/*
  njw1194 input channel select 
*/
void njw1194_input_channel_sel(u8 data,u8 chip_addr)
{
   njw1194_write_register(data,STERO_INPUT_SEL | chip_addr);
}
/*
  njw1194 ajust the treble(TONE control)
*/
void njw1194_treble_adjust(u8 data,u8 chip_addr)
{
   data |= TREB_BST_nCUT;            //boost mode
   njw1194_write_register(data,TONE_CTRL_TREBLE | chip_addr);
}
/*
  njw1194 ajust the bass(TONE control)
*/
void njw1194_bass_adjust(u8 data,u8 chip_addr)
{
   data |= BASS_BST_nCUT;
   njw1194_write_register(data,TONE_CTRL_BASS | chip_addr);
}
/*
  njw1194 ajust volume of the channel
*/
void njw1194_vol_ctrl(Channel_TYPE *chunnel,u8 volume)
{
   u8 balance=chunnel->vol_data->balance;
   u8 vol_adjust_by_balance=0,temp_vol,temp_vol_left,temp_vol_right;

   chunnel->vol_data->vol=volume;
   temp_vol=volume*2;

   
   if(volume==0)
   	{
       chunnel->vol_data->vol_left=VOLUME_MUTE_VAL;       //mute
	   chunnel->vol_data->vol_right=VOLUME_MUTE_VAL;
    }
   else
   	{
      if(balance == BALANCE_MAX/2)
   	      {
             chunnel->vol_data->vol_left = (temp_vol);
	         chunnel->vol_data->vol_right = (temp_vol);
          }
	  else if(balance > BALANCE_MAX/2)
	  	{
            //left channel -  and right channel no adjust
            
            vol_adjust_by_balance=balanceLevelTable[volume/10][GET_BALANCE_LEFT_INX(balance)];
			if(vol_adjust_by_balance>0)
				{
                   if(temp_vol>vol_adjust_by_balance)
                   	{
                      chunnel->vol_data->vol_left = temp_vol-vol_adjust_by_balance;
				    }
				   else
				   	{
                      chunnel->vol_data->vol_left = VOLUME_MUTE_VAL;  
				    }
			    }
			else
				{
                   chunnel->vol_data->vol_left = VOLUME_MUTE_VAL;
			    }
			
			chunnel->vol_data->vol_right = temp_vol;
	    }
	  else
	  	{
           //right channel -  and left channel no adjust
           vol_adjust_by_balance = balanceLevelTable[volume/10][GET_BALANCE_RIGHT_INX(balance)];
		   if(vol_adjust_by_balance > 0)
		   	{
               if(temp_vol>vol_adjust_by_balance)
               	{
                  chunnel->vol_data->vol_right = temp_vol - vol_adjust_by_balance;
			    }
			   else
			   	{
                  chunnel->vol_data->vol_right = VOLUME_MUTE_VAL;
			    }
		    }
		   else
		   	{
                 chunnel->vol_data->vol_right = VOLUME_MUTE_VAL;
		     }
	    }
   	}
   temp_vol_left = VOLUME_MAX_VAL-chunnel->vol_data->vol_left;
   temp_vol_right = VOLUME_MAX_VAL-chunnel->vol_data->vol_right;

   njw1194_vol_ctrl_left(temp_vol_left,  chunnel->channel);
   njw1194_vol_ctrl_right(temp_vol_right,chunnel->channel);  
   
   #if 0
   switch(chunnel->channel)
   	{
   	  case CHANNEL_A:
         njw1194_vol_ctrl_left(temp_vol_left,NJW1194_CHIP_A);
		 njw1194_vol_ctrl_right(temp_vol_right,NJW1194_CHIP_A);
		 break;
   	  case CHANNEL_B:
         njw1194_vol_ctrl_left(temp_vol_left,NJW1194_CHIP_B);
		 njw1194_vol_ctrl_right(temp_vol_right,NJW1194_CHIP_B);
		 break;		
   	  case CHANNEL_C:
         njw1194_vol_ctrl_left(temp_vol_left,NJW1194_CHIP_C);
		 njw1194_vol_ctrl_right(temp_vol_right,NJW1194_CHIP_C);
		 break;	
   	  case CHANNEL_D:
         njw1194_vol_ctrl_left(temp_vol_left,NJW1194_CHIP_D);
		 njw1194_vol_ctrl_right(temp_vol_right,NJW1194_CHIP_D);
		 break;		 
   	}
   #endif
}
/*
  njw1194 balance control
*/
void njw1194_balance_ctrl(Channel_TYPE *chunnel,u8 dir)
{
   u8 temp_vol;

   switch(dir)
   	{
       case DIR_UP:
	   	if(chunnel->vol_data->balance==BALANCE_MAX) return;
          (chunnel->vol_data->balance)++;
	   	break;
	   case DIR_DOWN:
	   	if(chunnel->vol_data->balance==BALANCE_MIN) return;
	   	(chunnel->vol_data->balance)--;
	   	break;
	   default:
	   	return;
    }
   	
  // chunnel->vol_data->balance=balance_vol;           //update the balance value
   temp_vol = chunnel->vol_data->vol;
   njw1194_vol_ctrl(chunnel,temp_vol);               //set the volume
}
/*
  treble 
*/
void njw1194_treble_ctrl(Channel_TYPE *chunnel,u8 dir)
{
   u8 temp_data; //BIT7--TC_B BIT6~3 treb_value BIT2--TONE SWITCH
   u8 temp_tone_sw;
   switch(dir)
   	{
       case DIR_UP:
	   	if(chunnel->vol_data->treb_level == TREB_MAX) return;
	   	(chunnel->vol_data->treb_level)++;
	   	break;
	   case DIR_DOWN:
	   	if(chunnel->vol_data->treb_level == TREB_MIN) return;
	   	(chunnel->vol_data->treb_level)--;
	   	break;
	   case DIR_HOLD:
	   	//if(chunnel->vol_data->tone_sw == TONE_ON)
	   	chunnel->vol_data->tone_sw = !(chunnel->vol_data->tone_sw);  //TONE ON/OFF
	   	break;
	   default:
	   	return;
    }
   temp_tone_sw=chunnel->vol_data->tone_sw;
   temp_data=chunnel->vol_data->treb_level;
   temp_data=temp_data<<3;
   //temp_data |= 0x80;
   temp_data |= (temp_tone_sw<<2);

   njw1194_treble_adjust(temp_data,chunnel->channel);

#if 0
   switch(chunnel->channel)
   	{
   	  case CHANNEL_A:
         njw1194_treble_adjust(temp_data,NJW1194_CHIP_A);
		 break;
   	  case CHANNEL_B:
         njw1194_treble_adjust(temp_data,NJW1194_CHIP_B);
		 break;		
   	  case CHANNEL_C:
         njw1194_treble_adjust(temp_data,NJW1194_CHIP_C);
		 break;	
   	  case CHANNEL_D:
         njw1194_treble_adjust(temp_data,NJW1194_CHIP_D);
		 break;		 
   	}
#endif
}
void njw1194_bass_ctrl(Channel_TYPE *chunnel,u8 dir)
{
     u8 temp_bass;
     temp_bass=chunnel->vol_data->bass_level;
	 
     switch(dir)
     	{
          case DIR_UP:
		  	if(temp_bass == BASS_MAX) return;
			
			(chunnel->vol_data->bass_level)++;
		  	break;
		  case DIR_DOWN:
		  	if(temp_bass == BASS_MIN) return;
			
			(chunnel->vol_data->bass_level)--;
		  	break;		  	
          case DIR_HOLD:
		  	break;
		  default:
		  	return;
	    }
	 temp_bass=chunnel->vol_data->bass_level;

	 temp_bass = temp_bass << 3;

	 njw1194_bass_adjust(temp_bass, chunnel->channel);
}
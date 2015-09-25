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

#define TREB_MAX            20
#define TREB_MIN            0

#define BASS_MAX            20
#define BASS_MIN            0

#define TREB_BST            0x80
#define TREB_CUT            0x00

#define BASS_BST            0x80
#define BASS_CUT            0x00

#define GET_BALANCE_LEFT_INX(x)       ((x)-((BALANCE_MAX)/2)-1)
#define GET_BALANCE_RIGHT_INX(x)       (((BALANCE_MAX)/2)-(x)-1)

#define IS_VOLUME(x)                  ((x)<=96)? (0):(1)

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
    njw1194_lat_ctrl(NJW1194_HIGH);
   njw1194_clk_ctrl(NJW1194_HIGH);
   njw1194_delay(_DELAY_TIME);   
}
/*
  njw1194 ajust the volume of the left channel 
*/
u8 data_test;
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
   njw1194_write_register(data,TONE_CTRL_TREBLE | chip_addr);
}
/*
  njw1194 ajust the bass(TONE control)
*/
void njw1194_bass_adjust(u8 data,u8 chip_addr)
{
   njw1194_write_register(data,TONE_CTRL_BASS | chip_addr);
}
/*
  njw1194 ajust volume of the channel
*/
void njw1194_vol_ctrl(Channel_TYPE *chunnel,u8 volume)
{
   u8 balance=chunnel->vol_data->balance;
   u8 vol_adjust_by_balance=0,temp_vol,temp_vol_left=0,temp_vol_right=0;

   if(IS_VOLUME(volume)) 
   	{   	   
   	  return;
   	}
   if(chunnel == NULL) return;
   
   chunnel->vol_data->vol = volume;
   chunnel->vol_data->vol_left =  volume;
   chunnel->vol_data->vol_right = volume;

   /*caculate the balance only when vol_left==vol_right*/
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
             chunnel->vol_data->vol_left = volume;
	         chunnel->vol_data->vol_right = volume;
             temp_vol_left=chunnel->vol_data->vol_left ;
             temp_vol_right=chunnel->vol_data->vol_right;
          }
	  else if(balance > BALANCE_MAX/2)
	  	{
            //left channel -  and right channel no adjust            
            vol_adjust_by_balance=balanceLevelTable[volume/10][GET_BALANCE_LEFT_INX(balance)];
			if(vol_adjust_by_balance>0)
				{
                   if(temp_vol>vol_adjust_by_balance)
                   	{
                      temp_vol_left = volume-vol_adjust_by_balance;
				    }
				   else
				   	{
                      temp_vol_left = VOLUME_MUTE_VAL;  
				    }
			    }
			else
				{
                   temp_vol_left = VOLUME_MUTE_VAL;
			    }
			
			temp_vol_right = volume;
	    }
	  else
	  	{
           //right channel -  and left channel no adjust
           vol_adjust_by_balance = balanceLevelTable[volume/10][GET_BALANCE_RIGHT_INX(balance)];
		   if(vol_adjust_by_balance > 0)
		   	{
               if(temp_vol>vol_adjust_by_balance)
               	{
                  temp_vol_right = volume - vol_adjust_by_balance;
			    }
			   else
			   	{
                  temp_vol_right = VOLUME_MUTE_VAL;
			    }
		    }
		   else
		   	{
                 temp_vol_right = VOLUME_MUTE_VAL;
		     }
		   temp_vol_left = volume;
	    }
   	}
   if(volume!=0)
   	{
      temp_vol_left = (VOLUME_MAX_VAL-(temp_vol_left*2))+1;
      temp_vol_right = (VOLUME_MAX_VAL-(temp_vol_right*2))+1;
   	}
   else
   	{
      temp_vol_left=VOLUME_MAX_VAL-VOLUME_MUTE_VAL;
	  temp_vol_right=VOLUME_MAX_VAL-VOLUME_MUTE_VAL;	  
     }

   njw1194_vol_ctrl_left(temp_vol_left,  chunnel->channel);
   njw1194_vol_ctrl_right(temp_vol_right,chunnel->channel);  

   chunnel->p_eeprom_write(ADDR_A_VOL_LCH+chunnel->channel,chunnel->vol_data->vol_left);
   chunnel->p_eeprom_write(ADDR_A_VOL_RCH+chunnel->channel,chunnel->vol_data->vol_right);
}
void njw1194_vol_ctrl_l(Channel_TYPE *chunnel,u8 volume)
{
    u8 temp;

    if(chunnel == NULL) return;
	
	if(IS_VOLUME(volume)) return;
	
    chunnel->vol_data->vol_left=volume;

	if(chunnel->vol_data->vol_left == chunnel->vol_data->vol_right)
		{
          chunnel->vol_data->vol=volume;  //update the main volume
	    }

	if(volume != 0)
		{
          temp=(VOLUME_MAX_VAL-volume*2)+1;
	    }
    else
    	{
          temp=VOLUME_MAX_VAL-VOLUME_MUTE_VAL;
	    }

	njw1194_vol_ctrl_left(temp,  chunnel->channel);
	chunnel->p_eeprom_write(ADDR_A_VOL_LCH+chunnel->channel,chunnel->vol_data->vol_left);
}
void njw1194_vol_ctrl_r(Channel_TYPE *chunnel,u8 volume)
{
    u8 temp;
	if(chunnel == NULL) return;
	
	if(IS_VOLUME(volume)) return;
	
    chunnel->vol_data->vol_right=volume;

	if(chunnel->vol_data->vol_left == chunnel->vol_data->vol_right)
		{
          chunnel->vol_data->vol=volume;  //update the main volume
	    }
	
	if(volume != 0)
		{
          temp=(VOLUME_MAX_VAL-volume*2)+1;
	    }
	else
		{
          temp=VOLUME_MAX_VAL-VOLUME_MUTE_VAL;
	    }

	njw1194_vol_ctrl_right(temp,  chunnel->channel);
	chunnel->p_eeprom_write(ADDR_A_VOL_RCH+chunnel->channel,chunnel->vol_data->vol_right);
}
/*
  njw1194 balance control
*/
void njw1194_balance_ctrl(Channel_TYPE *chunnel,u8 dir,u8 data)
{
   u8 temp_vol;

   if(chunnel == NULL) return;

   /*only when LCH==RCH*/
   if(chunnel->vol_data->vol_left == chunnel->vol_data->vol_right)
   	{
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
		  case DIR_DIRECT:
		  	  if(chunnel->vol_data->balance<=BALANCE_MAX)
		  	  	{
                 chunnel->vol_data->balance=data;
			    }
		  	break;
	      default:
	   	       return;
        }
   	
       temp_vol = chunnel->vol_data->vol;
       njw1194_vol_ctrl(chunnel,temp_vol);               //set the volume
   	}
}
/*
  treble 
*/
void njw1194_treble_ctrl(Channel_TYPE *chunnel,u8 dir,u8 val)
{
   u8 temp_data=0; //BIT7--TC_B BIT6~3 treb_value BIT2--TONE SWITCH
   u8 temp_tone_sw;
   if(chunnel == NULL) return;
   
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
	   	//chunnel->vol_data->tone_sw = !(chunnel->vol_data->tone_sw);  //TONE ON/OFF
	   	break;
	   case DIR_DIRECT:
	   	if(chunnel->vol_data->treb_level > TREB_MAX) return;
	   	   chunnel->vol_data->treb_level=val;
	   	break;
	   default:
	   	return;
    }
   if(chunnel->vol_data->tone_sw == TONE_ON)
   	{
      temp_tone_sw=TONE_ON;
    }
   else
   	{
      temp_tone_sw=TONE_OFF;
    }
   if(chunnel->vol_data->treb_level >10)
   	{
   	  temp_data |= TREB_BST;
      temp_data |= ((chunnel->vol_data->treb_level)-10)<<3;
    }
   else
   	{
   	  temp_data &= TREB_CUT;
      temp_data |=  (10-(chunnel->vol_data->treb_level))<<3;
    }
   temp_data |= (temp_tone_sw<<2);

   njw1194_treble_adjust(temp_data,chunnel->channel);

}
void njw1194_bass_ctrl(Channel_TYPE *chunnel,u8 dir,u8 val)
{
     u8 temp_bass=0;
	 if(chunnel == NULL) return;
	 
     temp_bass=chunnel->vol_data->bass_level;
	 
     switch(dir)
     	{
          case DIR_UP:
		  	if(temp_bass == BASS_MAX) 
				return;
			
			(chunnel->vol_data->bass_level)++;
		  	break;
		  case DIR_DOWN:
		  	if(temp_bass == BASS_MIN) 
				return;			
			
			(chunnel->vol_data->bass_level)--;
		  	break;		  	
          case DIR_HOLD:
		  	break;
		  case DIR_DIRECT:
		  	if(temp_bass >BASS_MAX) return;

			chunnel->vol_data->bass_level=val;
		  	break;
		  default:
		  	return;
	    }

	 if((chunnel->vol_data->bass_level)>10)
	 	{
          temp_bass |= BASS_BST;
		  temp_bass |= ((chunnel->vol_data->bass_level)-10)<<3;
	    }
	 else
	 	{
          temp_bass &= BASS_CUT;
		  temp_bass |= (10-(chunnel->vol_data->bass_level))<<3;
	    }

	 njw1194_bass_adjust(temp_bass, chunnel->channel);
}
void njw1194_tone_switch(Channel_TYPE *chunnel,u8 tone_val)
{
    u8 temp_data=0;
    chunnel->vol_data->tone_sw=tone_val;
    if(chunnel->vol_data->treb_level >10)
   	 {
   	   temp_data |= TREB_BST;
       temp_data |= ((chunnel->vol_data->treb_level)-10)<<3;
     }
    else
   	 {
   	   temp_data &= TREB_CUT;
       temp_data |=  (10-(chunnel->vol_data->treb_level))<<3;
     }
    temp_data |= (tone_val<<2);	
	
	njw1194_treble_adjust(temp_data,chunnel->channel);
}
void volume_ctrl(Channel_TYPE *chunnel,u8 left_or_right,u8 direct,u8 set_volume)
{
   u8 temp_val=0;
   if(chunnel == NULL) return;
   switch(left_or_right)
   	{
      case VOL_LEFT:
	  		{
			  temp_val=chunnel->vol_data->vol_left;	
              switch(direct)
              	{
                   case DIR_UP:
				   	njw1194_vol_ctrl_l(chunnel,++temp_val);
				   	break;
				   case DIR_DOWN:
				   	njw1194_vol_ctrl_l(chunnel,--temp_val);
				    break;
				   default:
				   	njw1194_vol_ctrl_l(chunnel,set_volume);
				   	break;
			    }
	        }
	  	break;
	  case VOL_RIGHT:
	  		{
		      temp_val=chunnel->vol_data->vol_right;
			  switch(direct)
				{
                  case DIR_UP:
				  	njw1194_vol_ctrl_r(chunnel,++temp_val);
				 	break;
				  case DIR_DOWN:
				  	njw1194_vol_ctrl_r(chunnel,--temp_val);
				 	break;
				  default:
				 	njw1194_vol_ctrl_r(chunnel,set_volume);
				 	break;
			  	}
	        }
	  	break;
	  case VOL_ALL:
	  	 	{
			  temp_val=chunnel->vol_data->vol;
              switch(direct)
              	{
                  case DIR_UP:
				  	njw1194_vol_ctrl(chunnel,++temp_val);
				  	break;
				  case DIR_DOWN:
				  	njw1194_vol_ctrl(chunnel,--temp_val);
				  	break;
				  default:
				  	njw1194_vol_ctrl(chunnel,set_volume);
				  	break;
			    }
	        }
	  	break;
    }
}
void volume_mute_ctrl(Channel_TYPE *chunnel,u8 stat)
{
    if(chunnel == NULL) return;
    if(stat == MUTE_ON)
    	{
          njw1194_input_channel_sel(INPUT_MUTE,chunnel->chip_id);    //select the mute channel
    	}
	else if(stat == MUTE_OFF)
		{
          njw1194_input_channel_sel(INPUT_A, chunnel->chip_id);      //select the input channel A
	    }
    chunnel->mute_status=stat;
}
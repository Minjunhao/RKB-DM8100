/**************************************************************************//**
 * @file     ak4117.c
 * @brief    ak4117 register control
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
#include "ak4117.h"
#include "adckey.h"

#define BIT_TEST(data,i)   (data&0x80>>i)? (1):(0)
/*
   MSB
   addr: C1 C0 R/W A4 A3 A2 A1 A0
   data: D7~D0
   
*/


/*
  add pulse time 
*/
void s_Ak4114_Write_Byte(unsigned char t_data);
void ak4117_delay(u32 delay_count)
{
   /*
   *instruction cycle: 1=0.83ns
   *9 instruction cycle = 9*0.83ns = 7.47ns
   */
   //delay_count=delay_count*10;
   while(--delay_count){}           //instruction cycle: 1=0.83ns
}
/*
  AK4117 RESET
  0: reset
  1: normal
*/
void ak4117_reset(u8 bit_set)
{
   if(AK4117_BIT_HIGH == bit_set)  //0:reset, 1:normal
   	{
      ak4117_reset_ctrl(AK4117_BIT_HIGH);
    }
   else
   	{
      ak4117_reset_ctrl(AK4117_BIT_LOW);
    }
}
void ak4117_reset_pwr_on(void)
{
  /*
  * reset pin hold for 150ns(Min) more 
  */
  ak4117_reset(AK4117_BIT_LOW);
  ak4117_delay(40);
  ak4117_reset(AK4117_BIT_HIGH);
  ak4117_delay(40);
}
/*
  ak4117_cs
  for chip selecting
*/
void ak4117_cs(u8 chip_sel, u8 bit_set)
{
   if(bit_set >1) return;
   switch(chip_sel)
   	{
      case CHIP_A:
	  	ak4117_a_cs_ctrl(bit_set);
	  	break;
	  case CHIP_B:
	  	ak4117_b_cs_ctrl(bit_set);
		break;
	  case CHIP_C:
	  	ak4117_c_cs_ctrl(bit_set);
	  	break;
	  case CHIP_D:
	  	ak4117_d_cs_ctrl(bit_set);
	  	break;
    }
}
/*
     ak4117_spi_start
     initial pin before starting register w/r
*/
void ak4117_spi_start(u8 chip_sel)
{
   ak4117_reset(AK4117_BIT_HIGH);
   ak4117_cs(chip_sel,AK4117_BIT_LOW);
   ak4117_di_ctrl(AK4117_BIT_HIGH);
   ak4117_clk_ctrl(AK4117_BIT_LOW);
}
/*
     ak4117 write one byte to ak4117
*/
u8 data;
#define BIT_TEST(data,i)   (data&0x80>>i)? (1):(0)
void ak4117_write_byte(u8 byte)
{
   //ak4117_cs()
   #ifdef _TEST 
   s_Ak4114_Write_Byte(byte);
   #else
   u8 i=0;
   data=byte;
   for(i=0;i<8;i++)
   	{
      ak4117_clk_ctrl(AK4117_BIT_LOW);
	  if(AK4117_BIT_HIGH == (BIT_TEST(byte,i)))
	  	{
          ak4117_di_ctrl(AK4117_BIT_HIGH);
	    }
	  else
	  	{
          ak4117_di_ctrl(AK4117_BIT_LOW);
	    }
	  ak4117_delay(20);
	  ak4117_clk_ctrl(AK4117_BIT_HIGH);
	  ak4117_delay(20);
    }
   ak4117_clk_ctrl(AK4117_BIT_LOW);
   #endif
}
#ifdef _TEST
void s_SmallWait(int s_data)
{
    int i,j;
     
	for (j=0;j<s_data;j++) {
		//2us for STM32F207
		for (i=0;i<9;i++) {
			__asm(" nop" );	
		};
	}
}
void s_Ak4114_Write_Byte(unsigned char t_data)
{
    unsigned char i;

    for(i=0;i<8;i++){
        p_4114_clk(0);
        s_SmallWait(10/2);	//jyj_Nec
        if((t_data & (0x80 >> i)) == 0){
            p_4114_do(0);
        } else {
            p_4114_do(1);
        }
        s_SmallWait(10/2);	//jyj_Nec
         p_4114_clk(1);
        s_SmallWait(10/2);	//jyj_Nec
    }
    p_4114_clk(0);
}
#endif
/*
* ak4117 read one byte from ak4117
*/
u8 ak4117_read_byte(void)
{
   u8 i=0,val=0x00;
   for(i=0;i<8;i++)
   	{
      ak4117_clk_ctrl(AK4117_BIT_HIGH);
	  ak4117_delay(20);
	  val=val<<1;
	  if(AK4117_BIT_HIGH == (get_ak4117_do))
	  	{
	  	   val=val | 0x01;
	  	}
#if 0	  
      if(i<7)
        val=val<<1;
#endif
	  ak4117_clk_ctrl(AK4117_BIT_LOW);	  
    }
   ak4117_clk_ctrl(AK4117_BIT_LOW);
   return val;
}
/*
* ak4117 register write
*/
void ak4117_register_write(u8 chip,u8 addr, u8 data)
{
   if(chip > CHIP_D) return;   //chip:0~4
   ak4117_cs(chip,AK4117_BIT_LOW); 
   ak4117_write_byte(addr | 0x20);   //ensure write bit is 1
   ak4117_write_byte(data);
   ak4117_cs(chip,AK4117_BIT_HIGH);
}
/*
* ak4117 register read
*/
u8 data_t;
void ak4117_register_read(u8 chip, u8 addr, u8 *data)
{
   if(chip > CHIP_D) return;   //chip:0~4
   ak4117_cs(chip,AK4117_BIT_LOW);
   //ak4117_write_byte(addr & 0xDF);   //ensure read bit is 0
   ak4117_write_byte(addr & 0xDF);
   data_t=addr&0xDF;
   *data=ak4117_read_byte();
   ak4117_cs(chip,AK4117_BIT_HIGH);
}
/*
  ak4117 in(RX0,RX1) and out format setting
*/
void ak4117_in_out_set(u8 chip,u8 in_out_set)
{
   /*
   ** BIT2~0 AUDIO DATA FORMAT(101--I2S OUT)
   ** BIT4~3 INT0 HOLD COUNT SELECT(01--1024LRCK)
   ** BIT5   CHANNEL STATUS SELECT(0-RX0)   
   ** BIT6   U-bit OUTPUT ENABLE
   ** BIT7   RECOVER DATA SELECT
   */
   ak4117_register_write(chip,AK4117_INOUT_CTRL,in_out_set);
}
/*
  ak4117 int0 mask configure
*/
void ak4117_int0_mask_set(u8 chip, u8 int0_mask)
{
   /*
   ** MASK FOR INT0, 0: MASK DISABLE, 1: MASK ENABLE
   ** BIT0  QINT bit
   ** BIT1  CINT bit
   ** BIT2  STC  bit
   ** BIT3  AUDION bit
   ** BIT4  V bit
   ** BIT5  AUTO bit
   ** BIT6  PAR bit
   ** BIT7  UNLOCK bit
   */
   ak4117_register_write(chip,AK4117_INT0_MASK,int0_mask);
}
/*
  ak4117 power x'tal configure
*/
void ak4117_pwr_xtal_set(u8 chip,u8 xtal_bit_set)
{
   /*
   ** POWER DOWN CONTROL
   ** BIT0    TIMING RESET & REGISTER INITIAL (0 reset,1 default)
   ** BIT1    POWER DOWN FOR CLOCK RECOVERY PART
   ** BIT3~2  X'TAL FREQUENCY SELECT
   ** BIT4    EXTERNAL CLOCK MODE SELECT
   ** BIT7~5  NO USE
   */
   ak4117_register_write(chip,AK4117_PWR_DOWN_CTRL,xtal_bit_set);
}
/*
  ak4117 master clock configure
*/
void ak4117_clock_set(u8 chip, u8 clock_bit_set)
{
   /*
   ** CLOCK CONTROL
   ** BIT1~0  MASTER CLOCK OPERATION MODE SELECT
   ** BIT3~2  MASTER CLOCK FREQUENCY SELECT AT X'TAL MODE
   ** BIT4    MASTER CLOCK OUTPUT SELECT AT X'TAL MODE
   ** BIT6~5  MASTER CLOCK FREQUENCY SELECT AT PLL CLOCK
   ** BIT7    LOW POWER MODE SELECT
   */
   ak4117_register_write(chip,AK4117_CLOCK_CTRL,clock_bit_set);
}
/*
  ak4117 int0 configure and channel configure
*/
void ak4117_dsp_config(u8 chip)
{
   ak4117_int0_mask_set(chip,0x1f);               //enable UNLOCK, PAR, AUTO
   ak4117_in_out_set(chip, 0x05);                 //input RX0, ouput format I2S
}

void ak4117_clock_config(const Channel_TYPE *chunnel)
{
   u8 freq_status=chunnel->ak4117_data->freq;
   u8 chip=chunnel->chip_id;


   ak4117_pwr_xtal_set(chip,0x0b);                //use 24.576Mhz X'TAL

   switch(freq_status)
   	{
       case FRQ_176KHZ:
	   case FRQ_192KHZ:
	   	ak4117_clock_set(chip, 0x44);            //support 32~192Khz
	   	break;
	   case FRQ_96KHZ:
	   case FRQ_88KHZ:
	   	ak4117_clock_set(chip,0x24);             //support 32~96Khz
	   	break;
	   case FRQ_48KHZ:
	   case FRQ_44KHZ:
	   case FRQ_32KHZ:
	   default:
	   	ak4117_clock_set(chip,0x04);
		break;
    }
   
}
void ak4117_get_frq(Channel_TYPE *chunnel)
{
   u8 freq;
   ak4117_register_read(chunnel->chip_id, AK4117_REV_STATUS_1, &freq);
   freq=freq&0x0f;                                  //0~4bit
   if(chunnel->ak4117_data->freq != freq)
   	{
   	 chunnel->ak4117_data->freq = freq;
	 chunnel->ak4117_data->bit_map |= 0x02;     //set flag bit 1 
   	}
}
void ak4117_get_stream(Channel_TYPE *chunnel)
{
   u8 pcm_dt=0,strm_type,dts_type;

   //strm_type = &(chunnel.ak4117_data->stream_kind);
   ak4117_register_read(chunnel->chip_id,AK4117_REV_STATUS_1, &pcm_dt);

   if(pcm_dt & 0x20 != 0)    //non-PCM bit detected
   	{
      if(pcm_dt & 0x40 != 0)  //DTS-CD detected
      	{
          strm_type = STREAM_DTS;
	    }
	  else
	  	{
           ak4117_register_read(chunnel->chip_id,AK4117_BST_PRMB_PC_0,&dts_type);
		   switch(dts_type & 0x1f)
		   	{
              case 1:
			  	strm_type = STREAM_AC3;
			  	break;
			  case 0:
			  case 3:
			  	strm_type = STREAM_OUTOFPCM;
			  	break;
			  case 4:
			  case 6:
			  case 8:
			  case 9:
			  	strm_type = STREAM_MPEG;
				break;
			  case 5:
			  	strm_type = STREAM_MP3;
			    break;
			  case 11:
			  case 12:
			  case 13:
			  	strm_type = STREAM_DTS;
			  	break;
			  default:
			  	strm_type = STREAM_NONA;
			  	break;
		    }
	    }
    }
   else
   	{
      if(pcm_dt & 0x40 ==0)
      	{
          strm_type = STREAM_PCM;
	    }
	  else
	  	{
          strm_type = STREAM_DTS;
	    }
    }

   
   if(strm_type != (chunnel->ak4117_data->stream_kind))
   	{
       chunnel->ak4117_data->stream_kind = strm_type;
	   chunnel->ak4117_data->bit_map |= 0x08;           //  set flag;
    }
   
}
void ak4117_int0_check(Channel_TYPE *chunnel)
{
   u8 chip=chunnel->chip_id,int0_status=0;
   switch(chip)
   	{
      case CHIP_A:
	  	if(ak4117_a_int0_read==AK4117_BIT_LOW)
	  		{
	  		  goto int0_low;
               //return;
		    }
	  	break;
	  case CHIP_B:
	  	if(ak4117_b_int0_read==AK4117_BIT_LOW)
	  		{
	  		  goto int0_low;
               //return;
		    }	  	
	  	break;
	  case CHIP_C:
	  	if(ak4117_c_int0_read==AK4117_BIT_LOW)
	  		{
	  		  goto int0_low;
               //return;
		    }	  	
	  	break;
	  case CHIP_D:
	  	if(ak4117_d_int0_read==AK4117_BIT_LOW)
	  		{
	  		  goto int0_low;
               //return;
		    }	  	
	  	break;
    }
   
   ak4117_register_read(chip,AK4117_REV_STATUS_0,&int0_status);
   if((int0_status & 0x80)!=0)
   	{
      chunnel->ak4117_data->stream_kind = STREAM_CHECKING;

	  switch(chunnel->ak4117_data->pll_status)
	  	{
	  	  case PLL_CHECKING:
		  	chunnel->ak4117_data->pll_status = PLL_UNLOCK;
			//ak4117_register_read(chip,AK4117_REV_STATUS_0,&int0_status);
		  	break;
		  case PLL_UNLOCK:
		  	//ak4117_register_read(chip,AK4117_REV_STATUS_0,&int0_status);
		  	break;
		  case PLL_LOCK:
		  	chunnel->ak4117_data->pll_status = PLL_CHECKING;
			//ak4117_register_read(chip,AK4117_REV_STATUS_0,&int0_status);
		  	break;
	  	}
	  ak4117_register_read(chip,AK4117_REV_STATUS_0,&int0_status);
	  
    }
   else
   	{
   	int0_low:
       chunnel->ak4117_data->pll_status = PLL_LOCK;
    }
}
void ak4117_ctrl(Channel_TYPE *chunnel)
{
   u8 *status=&(chunnel->ak4117_data->status);
   switch(*status)
   	{
       case AK4117_INITIAL:
	   	ak4117_spi_start(chunnel->chip_id);
		*status=AK4117_FREQSET;
	   	break;
	   case AK4117_FREQSET:
	   	ak4117_dsp_config(chunnel->chip_id);
		ak4117_clock_config(chunnel);
        *status=AK4117_CHECKING;
	   	break;
	   case AK4117_MUTE_CHECK:
	   	++(*status);
	   	break;
	   case AK4117_CHECKING:
	   	if((chunnel->ak4117_data->pll_status) != PLL_UNLOCK)
	   		{
               ak4117_get_frq(chunnel);
			   ak4117_get_stream(chunnel);
		    }
	   	break;
	   default:
	   	break;
    }
   if((--(chunnel->ak4117_data->pll_check_time)) == 0)
   	{
      ak4117_int0_check(chunnel);
	  chunnel->ak4117_data->pll_check_time=PLL_CHECK_TIME_40MS;      //reset the pll check time;
   	}
}
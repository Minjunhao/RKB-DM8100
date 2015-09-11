#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "initmcu.h"
#include "main.h"
#include "uart.h"
#include "timer.h"

#include "adckey.h"
//#include "adckey.ext"

#include "timer.h"
#include "proc_function.h"
#include "proc_display.h"

//#include "ak4114.ext"
#include "ak4114.h"

#include "eeprom.h"
#include "i2c.h"

#include "RKB_function.h"
#include "ak4117.h"
#include "njw1112.h"
#include "njw1194.h"

#ifdef IPOD_INCLUDE
#include "iPod_config.h"
#endif
#include "power_led.h"
#include "fan_ctrl.h"
#include "signal_sense.h"
//----------------------------------------------------------
extern void VFDClearDisplay(void);


#ifdef _RKB_DM8100
Channel_TYPE      RKB_Channel[4];
AK4117_TYPE       AK4117_Data[4];
VOL_CTRL_TYPE     VOL_Data[4];
u8                mode_display_update=0;
u8                volume_send_delay_time;
#endif
const char reconstruct_Table[CONSTRUCT_TABLE_SIZE][4] = { 				//20150820

	0x00,MAX_TEMP_VOLUME,TRIM_INITIAL,	ADDR_CURRENT_KEY0,			// current_key - 2
	0x00,MAX_TEMP_VOLUME,TRIM_INITIAL,	ADDR_CURRENT_KEY1,			// current_key - 3
	0x00,MAX_TEMP_VOLUME,TEMP_INITIAL,	ADDR_CURRENT_KEY2,			// current_key - 4
	0x00,MAX_TEMP_VOLUME,TEMP_INITIAL,	ADDR_CURRENT_KEY3,			// current_key - 5

	30,85,FAN_HS_ON_TEMP_INITIAL,   ADDR_FAN_HS_ON_TEMP,//4			// fan_hs_on_temp			//20140521
	28,85,FAN_HS_OFF_TEMP_INITIAL,	ADDR_FAN_HS_OFF_TEMP,		// fan_hs_off_temp			//20140521
	31,86,SHUTDOWN_TEMP_INITIAL,	ADDR_SHUTDOWN_TEMP,		// shutdown_temp
	31,99,SMPS_SHUTDOWN_TEMP_INITIAL,ADDR_SMPS_SHUTDOWN_TEMP, 		// SMPS_shutdown_temp
	30,85,SMPS_FAN_OFF_TEMP_INITIAL, ADDR_SMPS_FAN_ON_TEMP,		// smps_fan_on_temp			//20140311
	28,85,SMPS_FAN_ON_TEMP_INITIAL,	ADDR_SMPS_FAN_OFF_TEMP,		// smps_fan_off_temp		//20140311
	30,85,FAN_LS_ON_TEMP_INITIAL,	ADDR_FAN_LS_ON_TEMP,		// fan_ls_on_temp			//20140521
	28,85,FAN_LS_OFF_TEMP_INITIAL,	ADDR_FAN_LS_OFF_TEMP,		// fan_ls_off_temp			//20140521

	0,1,AMP_ON_INITIAL,	 ADDR_AMP_A_ONOFF,   //12				// amp_on_off[0]
	0,1,AMP_ON_INITIAL,	 ADDR_AMP_B_ONOFF,				// amp_on_off[1]
	0,1,AMP_ON_INITIAL,	 ADDR_AMP_C_ONOFF,				// amp_on_off[2]
	0,1,AMP_ON_INITIAL,	 ADDR_AMP_D_ONOFF,				// amp_on_off[3]

	0,MAX_MASTER_VOLUME,VOLUME_INITIAL,	 ADDR_A_VOL_LCH,   //16		// volume_Lch[0]
	0,MAX_MASTER_VOLUME,VOLUME_INITIAL,	 ADDR_B_VOL_LCH,		// volume_Lch[1]
	0,MAX_MASTER_VOLUME,VOLUME_INITIAL,	 ADDR_C_VOL_LCH,		// volume_Lch[2]
	0,MAX_MASTER_VOLUME,VOLUME_INITIAL,	 ADDR_D_VOL_LCH,		// volume_Lch[3]

	0,MAX_MASTER_VOLUME,VOLUME_INITIAL,	 ADDR_A_VOL_RCH,   //20		// volume_Rch[0]
	0,MAX_MASTER_VOLUME,VOLUME_INITIAL,	 ADDR_B_VOL_RCH,		// volume_Rch[1]
	0,MAX_MASTER_VOLUME,VOLUME_INITIAL,	 ADDR_C_VOL_RCH,		// volume_Rch[2]
	0,MAX_MASTER_VOLUME,VOLUME_INITIAL,	 ADDR_D_VOL_RCH,		// volume_Rch[3]

	0,MAX_BALANCE_VOLUME,BALANCE_VOLUME_INITIAL, ADDR_A_BAL, 	//24		// balance[0]
	0,MAX_BALANCE_VOLUME,BALANCE_VOLUME_INITIAL, ADDR_B_BAL,			// balance[1]
	0,MAX_BALANCE_VOLUME,BALANCE_VOLUME_INITIAL, ADDR_C_BAL,			// balance[2]
	0,MAX_BALANCE_VOLUME,BALANCE_VOLUME_INITIAL, ADDR_D_BAL,			// balance[3]

	0,MAX_INPUT_SEL,INPUT_SEL_INITIAL,	ADDR_AMP_A_INPUTSELMODE,  //28			// input_selMode[0]
	0,MAX_INPUT_SEL,INPUT_SEL_INITIAL,	ADDR_AMP_B_INPUTSELMODE,			// input_selMode[1]
	0,MAX_INPUT_SEL,INPUT_SEL_INITIAL,	ADDR_AMP_C_INPUTSELMODE,			// input_selMode[2]
	0,MAX_INPUT_SEL,INPUT_SEL_INITIAL,	ADDR_AMP_D_INPUTSELMODE,			// input_selMode[3]

    0,2,1                            ,  ADDR_POWEROPTION
};

void RKB_channel_initial(void)
{
   u8 i;
   for(i=CHANNEL_A;i<=CHANNEL_D;i++)
   	{
   	  //data
   	  RKB_Channel[i].chip_id         = i;
	  RKB_Channel[i].channel         = i;
	  RKB_Channel[i].mute_status     = MUTE_ON;
	  RKB_Channel[i].ak4117_data     = &(AK4117_Data[i]);
	  RKB_Channel[i].vol_data        = &(VOL_Data[i]);
	  RKB_Channel[i].digital_analog_stat = 0;
	  RKB_Channel[i].digital_nAnalog = 0;
	  RKB_Channel[i].local_speaker_src = 0;
	  RKB_Channel[i].link_channel_src = 0;
	  //func
	  RKB_Channel[i].p_ak4117_func            =  ak4117_ctrl;
	  RKB_Channel[i].p_link_chanel_src_sel    = njw1112_link_speaker_ctrl;
	  RKB_Channel[i].p_local_speaker_src_sel  = njw1112_local_speaker_ctrl;
	  RKB_Channel[i].p_vol_control            = volume_ctrl;
	  RKB_Channel[i].p_digital_nAnalog_sw     = njm2752_ctrl;
	  RKB_Channel[i].p_treb_control           = njw1194_treble_ctrl;
	  RKB_Channel[i].p_bass_control           = njw1194_bass_ctrl;
	  RKB_Channel[i].p_balance_contrl         = njw1194_balance_ctrl;
	  RKB_Channel[i].p_mute_ctrl              = volume_mute_ctrl;
   	}
}
/*
   njm2752 control
   0: Analog
   1: digital
*/
void njm2752_ctrl(Channel_TYPE *chunnel,u8 digital_nAnalog)
{
   u8 state=chunnel->digital_nAnalog;

   if(state == digital_nAnalog)  return;
   switch(chunnel->channel)
   	{
   	  case CHANNEL_A:
	  	njm2752_a_cnt_ctrl(digital_nAnalog);
		break;
	  case CHANNEL_B:
	  	njm2752_b_cnt_ctrl(digital_nAnalog);
		break;
	  case CHANNEL_C:
	  	njm2752_c_cnt_ctrl(digital_nAnalog);
	  	break;
	  case CHANNEL_D:
	  	njm2752_d_cnt_ctrl(digital_nAnalog);
	  	break;
   	}
   chunnel->digital_nAnalog=digital_nAnalog;
}
/*
  check backup in the EEPROM
  
*/
u8 Check_Backup(void)
{
    u8 i,a,b,result,temp;
   	a = 0xA5;
	b = (unsigned char)nvr_xGetData(ADDR_CHECKRAM);
	if(a == b)
		{

		   //amp temperature 
           fan_status.fan_hs_on_amp_temp = (u8)nvr_xGetData(ADDR_FAN_HS_ON_TEMP);
		   fan_status.fan_hs_off_amp_temp= (u8)nvr_xGetData(ADDR_FAN_HS_OFF_TEMP);
		   if(fan_status.fan_hs_on_amp_temp<(fan_status.fan_hs_off_amp_temp+2))
		   	{
              fan_status.fan_hs_off_amp_temp = fan_status.fan_hs_on_amp_temp-2;
			  nvr_xPutData(ADDR_FAN_HS_OFF_TEMP,fan_status.fan_hs_off_amp_temp);
		    }
		   fan_status.fan_ls_on_amp_temp = (u8)nvr_xGetData(ADDR_FAN_LS_ON_TEMP);
		   fan_status.fan_ls_off_amp_temp= (u8)nvr_xGetData(ADDR_FAN_LS_OFF_TEMP);
           if(fan_status.fan_ls_on_amp_temp<(fan_status.fan_ls_off_amp_temp+2))
           	{
              fan_status.fan_ls_off_amp_temp = fan_status.fan_ls_on_amp_temp-2;
			  nvr_xPutData(ADDR_FAN_LS_OFF_TEMP,fan_status.fan_ls_off_amp_temp);
		    }
		   //smps temperature
           fan_status.fan_on_smps_temp = (u8)nvr_xGetData(ADDR_SMPS_FAN_ON_TEMP);	
		   fan_status.fan_off_smps_temp = (u8)nvr_xGetData(ADDR_SMPS_FAN_OFF_TEMP);
		   if(fan_status.fan_on_smps_temp<(fan_status.fan_off_smps_temp+2))
		   	{
              fan_status.fan_off_smps_temp = fan_status.fan_on_smps_temp-2;
			  nvr_xPutData(ADDR_SMPS_FAN_OFF_TEMP, fan_status.fan_off_smps_temp);
		    }
           //amp shutdown power
		   power_status.amp_shut_down_temp = (u8)nvr_xGetData(ADDR_SHUTDOWN_TEMP);
           temp=(fan_status.fan_hs_on_amp_temp >= fan_status.fan_on_smps_temp)?
		   	    (fan_status.fan_hs_on_amp_temp):(fan_status.fan_on_smps_temp);
		   if(power_status.amp_shut_down_temp <= temp)
		   	{
              power_status.amp_shut_down_temp = temp+1;
			  nvr_xPutData(ADDR_SHUTDOWN_TEMP,power_status.amp_shut_down_temp);
		    }
           //smps shutdown power
		   power_status.smps_shut_down_temp = (u8)nvr_xGetData(ADDR_SMPS_SHUTDOWN_TEMP);

		   //channel a~d configuration
           for(i=CHANNEL_A;i<CHANNEL_D+1;i++)
           	{
              RKB_Channel[i].amp_channel_on     = (u8)nvr_xGetData(ADDR_AMP_A_ONOFF+i);
			  RKB_Channel[i].vol_data->vol_left = (u8)nvr_xGetData(ADDR_A_VOL_LCH+i);
			  RKB_Channel[i].vol_data->vol_right= (u8)nvr_xGetData(ADDR_A_VOL_RCH+i);
			  RKB_Channel[i].vol_data->balance  = (u8)nvr_xGetData(ADDR_A_BAL+i);
			  RKB_Channel[i].digital_analog_mode = (u8)nvr_xGetData(ADDR_AMP_A_INPUTSELMODE+i);
		    }

           //power mode
           power_status.power_mode = (u8) nvr_xGetData(ADDR_POWEROPTION);

		   //display update mode

		   
           		   
	       backup_range_check(1);

		   result = TRUE;
		   
	    }
	else
		{
           result = FALSE;
	    }

	return result;
}
/*
   backup_range_check
   type:1--check range, 0--reload the default data
*/
void backup_range_check(u8 type)
{
    u8 i;
	backup_reconstruct(0,	&current_key[0],type);	
	backup_reconstruct(1,	&current_key[1],type);	
	backup_reconstruct(2,	&current_key[2],type);	
	backup_reconstruct(3,	&current_key[3],type);	

	backup_reconstruct(4,	&(fan_status.fan_hs_on_amp_temp),type);		//20140521
	backup_reconstruct(5,	&(fan_status.fan_hs_off_amp_temp),type);		//20140521	
	backup_reconstruct(6,	&(power_status.amp_shut_down_temp),type);		//20130916
	backup_reconstruct(7,	&(power_status.smps_shut_down_temp),type);	//20131024
	backup_reconstruct(8,	&(fan_status.fan_on_smps_temp),type);		//20140311
	backup_reconstruct(9,	&(fan_status.fan_off_smps_temp),type);	//20140311
	backup_reconstruct(10,	&(fan_status.fan_ls_on_amp_temp),type);		//20140521
	backup_reconstruct(11,	&(fan_status.fan_ls_off_amp_temp),type);		//20140521

    for(i=CHANNEL_A;i<CHANNEL_D+1;i++)
    	{
          backup_reconstruct(12+i,&(RKB_Channel[i].amp_channel_on),type);
		  backup_reconstruct(16+i,&(RKB_Channel[i].vol_data->vol_left),type);
		  backup_reconstruct(20+i,&(RKB_Channel[i].vol_data->vol_right),type);
		  backup_reconstruct(24+i,&(RKB_Channel[i].vol_data->balance),type);
		  backup_reconstruct(28+i,&(RKB_Channel[i].digital_nAnalog),type);
	    }
	
   backup_reconstruct(32,&(power_status.power_mode),type); 
   //backup_reconstruct();
}
void backup_reconstruct(unsigned char i, unsigned char *addr,unsigned char flag)
{	
   if(flag == 1)
   	{
	 if ((*addr >= (unsigned char)reconstruct_Table[i][0]) && (*addr <= (unsigned char)reconstruct_Table[i][1])) {
		 return;
	  }else{
		    *addr = (unsigned char)reconstruct_Table[i][2];
		    nvr_xPutData(reconstruct_Table[i][3],*addr);				//(eeprom addr, data) : 20070724
		}
   	}
   else
   	{
		    *addr = (unsigned char)reconstruct_Table[i][2];
		    nvr_xPutData(reconstruct_Table[i][3],*addr);	
     }
}

void reload_default_configuration(void)
{
   u8 val=0xA5;
   backup_range_check(0);  //reload the default data					
   nvr_xPutData(ADDR_CHECKRAM,val);  
}
void mode_reset(void)
{
   reload_default_configuration();
   nvr_xPutData(ADDR_RESET,OFF);
   mode_power_on();
}


/*
  AK4117 check
*/
void mode_dir_check(void)
{
   u8 i;
   for(i=0;i<=CHANNEL_D;i++)
   	{
   	 if(IS_NULL(RKB_Channel[0].p_ak4117_func)) break;
       RKB_Channel[0].p_ak4117_func(&RKB_Channel[0]);
   	}
}
/*
  ALL CHANNEL MUTE ON
*/
void mode_mute_on(u8 chunnel)
{
   u8 i;
   for(i=0;i<=CHANNEL_D;i++)
   	{
       if(IS_NULL(RKB_Channel[i].p_mute_ctrl)) return;
   }
   switch(chunnel)
   	{
       case MUTE_CH_A:
	   	RKB_Channel[CHANNEL_A].p_mute_ctrl(&RKB_Channel[CHANNEL_A],MUTE_ON);
	   	break;
	   case MUTE_CH_B:
	   	RKB_Channel[CHANNEL_B].p_mute_ctrl(&RKB_Channel[CHANNEL_B],MUTE_ON);
	   	break;
	   case MUTE_CH_C:
	   	RKB_Channel[CHANNEL_C].p_mute_ctrl(&RKB_Channel[CHANNEL_C],MUTE_ON);
	   	break;
	   case MUTE_CH_D:
	   	RKB_Channel[CHANNEL_D].p_mute_ctrl(&RKB_Channel[CHANNEL_D],MUTE_ON);
	   	break;
	   case MUTE_ALL:
	   	RKB_Channel[CHANNEL_A].p_mute_ctrl(&RKB_Channel[CHANNEL_A],MUTE_ON);
		RKB_Channel[CHANNEL_B].p_mute_ctrl(&RKB_Channel[CHANNEL_B],MUTE_ON);
		RKB_Channel[CHANNEL_C].p_mute_ctrl(&RKB_Channel[CHANNEL_C],MUTE_ON);
		RKB_Channel[CHANNEL_D].p_mute_ctrl(&RKB_Channel[CHANNEL_D],MUTE_ON);
	   	break;
    }
}
/*
  ALL CHANNEL MUTE OFF
*/
void mode_mute_off(u8 chunnel)
{
   u8 i;
   for(i=0;i<=CHANNEL_D;i++)
   	{
       if(IS_NULL(RKB_Channel[i].p_mute_ctrl)) return;
   }
   
   switch(chunnel)
   	{
       case MUTE_CH_A:
	   	RKB_Channel[CHANNEL_A].p_mute_ctrl(&RKB_Channel[CHANNEL_A],MUTE_OFF);
	   	break;
	   case MUTE_CH_B:
	   	RKB_Channel[CHANNEL_B].p_mute_ctrl(&RKB_Channel[CHANNEL_B],MUTE_OFF);
	   	break;
	   case MUTE_CH_C:
	   	RKB_Channel[CHANNEL_C].p_mute_ctrl(&RKB_Channel[CHANNEL_C],MUTE_OFF);
	   	break;
	   case MUTE_CH_D:
	   	RKB_Channel[CHANNEL_D].p_mute_ctrl(&RKB_Channel[CHANNEL_D],MUTE_OFF);
	   	break;
	   case MUTE_ALL:
	   	RKB_Channel[CHANNEL_A].p_mute_ctrl(&RKB_Channel[CHANNEL_A],MUTE_OFF);
		RKB_Channel[CHANNEL_B].p_mute_ctrl(&RKB_Channel[CHANNEL_B],MUTE_OFF);
		RKB_Channel[CHANNEL_C].p_mute_ctrl(&RKB_Channel[CHANNEL_C],MUTE_OFF);
		RKB_Channel[CHANNEL_D].p_mute_ctrl(&RKB_Channel[CHANNEL_D],MUTE_OFF);
	   	break;
    }
}

/*
   amp status check
   amp_status.protect_func_on is set by external command
   if amp_status.protect_status is AMP_PROTECT_OFF,check the amp_ready port and amp error port
   and the temperature of the amp and the SMPS
*/
void mode_amp_protection_check(void)
{
    if(amp_status.protect_func_on == PROTECT_FUNCTION_OFF) return;
    if(amp_status.protect_status == AMP_PROTECT_OFF)
    	{
          if(get_amp_n_ready)
            {
       	      if(++(amp_status.check_timer)>AMP_CHECK_TIME)
       	        {
       	           //if amp ready pin is high
	   	           amp_protection_on();
       	        }
            }
          else if(get_amp_n_err)
	          {
	            if(++(amp_status.check_timer)>AMP_CHECK_TIME)
	   	          {
                     //amp_status.protect_status = AMP_PROTECT_ON;    //OFF--amp no error
                     //if amp error pin is high
                     amp_protection_on();
	   	          }
	           }
		  else
	      	{
                amp_status.check_timer=0;
		    }

         //check the temperature of the SMPS and the amp
         if(fan_status.amp_temperature>power_status.amp_shut_down_temp)
   	       {
              //amp_status.protect_status = AMP_PROTECT_ON;
              amp_protection_on();
              
           }
         if(fan_status.smps_temperature>power_status.smps_shut_down_temp)
   	      {
             //amp_status.protect_status = AMP_PROTECT_ON;
             amp_protection_on();
          }
      }
	else 
		{
           if(get_amp_n_ready==0 && get_amp_n_err == 0)
           	{
               if(++(amp_status.check_timer)>AMP_CHECK_TIME)
               	{
                   if(fan_status.amp_temperature<=power_status.amp_shut_down_temp && fan_status.smps_temperature <= power_status.smps_shut_down_temp)
                   	{
                       amp_protection_off();
				    }
				   else
				   	{
                       amp_status.check_timer=0;
				    }
			    }
		    }
		   else
		   	{
                amp_status.check_timer=0;
		    }
	    }
}
void mode_ak4117_check(void)
{
    RKB_Channel[CHANNEL_A].p_ak4117_func(&RKB_Channel[CHANNEL_A]);
	RKB_Channel[CHANNEL_B].p_ak4117_func(&RKB_Channel[CHANNEL_B]);
	RKB_Channel[CHANNEL_C].p_ak4117_func(&RKB_Channel[CHANNEL_C]);
	RKB_Channel[CHANNEL_D].p_ak4117_func(&RKB_Channel[CHANNEL_D]);

	//s_4114_Exec(0);
	//s_4114_Exec(1);
	//s_4114_Exec(2);
	//s_4114_Exec(3);
}
void mode_input_check(void)
{
    u8 i;
	for(i=CHANNEL_A;i<CHANNEL_D;i++)
		{
		  switch(RKB_Channel[i].digital_analog_mode)
    	    {
    	     case INPUT_MODE_AUTO:
                  switch(RKB_Channel[i].ak4117_data->pll_status)
                  	{
			  	       case PLL_LOCK:
                          RKB_Channel[i].p_digital_nAnalog_sw(&RKB_Channel[i],1);  //auto switch to digital
                         break;
				       case PLL_UNLOCK:
				          RKB_Channel[i].p_digital_nAnalog_sw(&RKB_Channel[i],0);  //auto switch to analog	
				         break;
                  	}
			   break;
			 case INPUT_MODE_ANALOG:
			 	RKB_Channel[i].p_digital_nAnalog_sw(&RKB_Channel[i],0);  //auto switch to analog	
			   break;
			 case INPUT_MODE_DIGITAL:
			 	RKB_Channel[i].p_digital_nAnalog_sw(&RKB_Channel[i],1);  //auto switch to digital
			   break;
			  
	        }
		  
		}
}
/*
  check the backup and configure the unit
*/
void mode_status_initial(void)
{

    u8 i=0;
    //initial the SPI port
    njw1112_write_start();    
	njw1194_write_start();

	for(i=CHANNEL_A;i<=CHANNEL_D;i++)
		{
		  //set the volume
	      if(RKB_Channel[i].vol_data->vol_left == RKB_Channel[i].vol_data->vol_right)
	      	{
              RKB_Channel[i].vol_data->vol=RKB_Channel[i].vol_data->vol_left;
			  RKB_Channel[i].p_vol_control(&RKB_Channel[i],RKB_Channel[i].vol_data->vol);
		    }
		  else
		  	{
			  RKB_Channel[i].p_vol_ctrl_l(&RKB_Channel[i],RKB_Channel[i].vol_data->vol_left);
			  RKB_Channel[i].p_vol_ctrl_r(&RKB_Channel[i],RKB_Channel[i].vol_data->vol_right);
		    }

		  //set the input
		  RKB_Channel[i].p_local_speaker_src_sel(&RKB_Channel[i],RKB_Channel[i].local_speaker_src);

		  //set output(DB15)
		  RKB_Channel[i].p_link_chanel_src_sel(&RKB_Channel[i],RKB_Channel[i].link_channel_src);

          //set the tone control
          if(RKB_Channel[i].vol_data->tone_sw == TONE_ON)
          	{
		      //set the bass
		      RKB_Channel[i].p_bass_control(&RKB_Channel[i],RKB_Channel[i].vol_data->bass_level);
			  //set the treb
			  RKB_Channel[i].p_treb_control(&RKB_Channel[i],RKB_Channel[i].vol_data->treb_level);

          	}
		}
}

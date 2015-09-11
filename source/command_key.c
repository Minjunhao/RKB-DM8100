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
#include "njw1112.h"
#include "njw1194.h"
#include "ak4117.h"
#include "signal_sense.h"

#define ISEQUAL(a,b)   ((a)==(b))? 1:0
const u8 mode_id[2]={'0','0'};
const u8 *prefix[]={
                    "000",
					"00A",
					"00B",
					"00C",
					"00D"
                  };
unsigned char command[COMMAND_SIZE];
unsigned char command_count;

STRING_CMD_TYPE RS232_KEY;
#define NUM_CMD    (59)     			//20150302  
#define NUM_CMD_X  (15)
#define NUM_INPUT_CHANNEL (8)
u8 response_data[RESPONSE_SIZE];
const byte AsciiKeyValueTable_RKB[] = {
			SK_POWERON,				//20131118
			SK_POWEROFF,
			SK_POWER,
			SK_GET_POWER,
			SK_PRODUCT_TYPE,
			SK_PRODUCT_VERSION,
//=6
			SK_GET_FREQ,
			SK_GET_AMP_TRIM,
			SK_GET_TEMPERATURE,
			SK_GET_FAN_STATUS,
			SK_GET_AMP_STATUS,
//+5=11
			SK_GET_FAN_HS_ON_TEMP,			//20140521
			SK_GET_FAN_HS_OFF_TEMP,			//20140521
			SK_GET_SHUTDOWN_TEMP,
			SK_GET_SMPS_SHUTDOWN_TEMP,	//20131024
//+4=15
			SK_SET_TEMPERATURE_ADC,
			SK_SET_TEMPERATURE_ON,		//20131129
			SK_FACTORY_DEFAULT_SET,		//20131129
			SK_AMP_PROTECTION_OFF,		//20131129
			SK_AMP_PROTECTION_ON,		//20131129
//+5=20
			SK_DISP_UPDATE,				//20131212
			SK_DISP_AUTO,				//20131212
			SK_DISP_MANUAL,				//20131212
//+3=23
			SK_GET_DELAYTIME_VOLUME,	//20131218
//+1=24
			//20140213 : add RS232 trim L/R control
			SK_MUTE,SK_MUTE_ON,SK_MUTE_OFF,
			
			SK_GET_AMPCH_STATUS,		
			SK_AMPCH_ON,
			SK_AMPCH_OFF,
			SK_GET_FRONT_TRIM_STATUS,
			SK_FRONT_TRIM_ENABLE,
			SK_FRONT_TRIM_DISABLE,

			SK_GET_VOLUME,
			SK_GET_VOLUME_MIN,
			SK_GET_VOLUME_MAX,
			SK_GET_VOLUME_L,
			SK_GET_VOLUME_R,
			SK_GET_BALANCE,
//+15=39
			SK_GET_LED_DIMMER,
//+1=40
			SK_VOL_UP,
			SK_VOL_DN,
			SK_VOL_L_UP,
			SK_VOL_L_DN,
			SK_VOL_R_UP,
			SK_VOL_R_DN,
//+6=46
			SK_GET_SMPS_FAN_ON_TEMP,			//20140311
			SK_GET_SMPS_FAN_OFF_TEMP,			//20140311
//+2=48
			SK_BALANCE_RIGHT,					//20140408
			SK_BALANCE_LEFT,					//20140408
//+2=50
			SK_GET_OPTI_POWER_MODE,				//20140515
			SK_OPTI_POWER_ENABLE,				//20140515
			SK_OPTI_POWER_DISABLE,				//20140515
//+3=53
			SK_GET_FAN_LS_ON_TEMP,			//20140523
			SK_GET_FAN_LS_OFF_TEMP,			//20140523
//+2=55
			SK_GET_INPUT_SEL_MODE,			//20150302
			SK_INPUT_SEL_AUTO,				//20150302
			SK_INPUT_SEL_DIGITAL,			//20150302
			SK_INPUT_SEL_ANALOG				//20150302
//+4=59
};

const char cmd_list[NUM_CMD][21+1] = {  //(max_size + 1)
			"power_on",				//20131118
			"power_off",
			"power_toggle",
			"get_current_power",
			"get_product_type",
			"get_product_version",
//=6
			"get_current_freq",
			"get_amp_trim",
			"get_temperature",
			"get_fan_status",
			"get_amp_status",
//+5=11
			"get_fan_hs_on",			//20140521
			"get_fan_hs_off",			//20140521
			"get_shutdown",			//20130916
			"get_smps_shutdown",	//20131024
//+4=15
			"temperature_adc",		//20131129
			"temperature_on",		//20131129
			"factory_default_on",	//20131129
			"amp_protection_off",	//20131129 : test : AMP protection disable
			"amp_protection_on",	//20131129 : test : AMP protection enable
//+5=20
			"get_display_update",		//20131212
			"display_update_auto",		//20131212
			"display_update_manual",	//20131212
//+3=23
			"get_delay_time_volume",	//20131218
//+1=24
			//20140213 : add rs232 trim L/R control <= add keys : volume, trim_l, trim_r, balance(+/-15db), mute(on/off)
			"mute","mute_on","mute_off",
			
			"get_channel_status",		
			"channel_on",
			"channel_off",
			"get_front_trim_status",
			"front_trim_enable",
			"front_trim_disable",

			"get_volume",
			"get_volume_min",
			"get_volume_max",
			"get_volume_l",
			"get_volume_r",
			"get_balance",
//+15=39
			"get_led_dimmer",
//+1=40
			"volume_up",
			"volume_down",
			"volume_l_up",
			"volume_l_down",
			"volume_r_up",
			"volume_r_down",
//+6=46
			"get_smps_fan_on",			//20140311
			"get_smps_fan_off",			//20140311
//+2=48
			"balance_right",			//20140408
			"balance_left",				//20140408
//+2=50
			"get_optical_power",		//20140515
			"optical_power_enable",		//20140515
			"optical_power_disable",	//20140515
//+3=53			
			"get_fan_ls_on",			//20140521
			"get_fan_ls_off",			//20140521
//+2=55			
			"get_input_sel_mode",		//20150302
			"input_sel_auto",			//20150302
			"input_sel_digital",		//20150302
			"input_sel_analog"			//20150302
//+4=59			
};

//cmd **_vol
const byte AsciiKeyValueTable_RKB_X[]={
            SK_SET_BALANCE_L,
			SK_SET_BALANCE_R,
			SK_SET_FAN_LS_ON,
			SK_SET_FAN_LS_OFF,
			SK_SET_FAN_HS_ON,
			SK_SET_FAN_HS_OFF,
			SK_SET_SHUTDOWN,
			SK_SET_SMPS_SHUTDOWN,
			SK_SET_VOLUME_TX_DLY,
			SK_SET_SMPS_FAN_ON,
			SK_SET_SMPS_FAN_OFF,
			SK_SET_VOLUME,
			SK_SET_VOLUME_L,
			SK_SET_VOLUME_R,
			SK_SET_LED_DIMMER
 
};
const char cmd_list_x[NUM_CMD_X][21+1]={
            "balance_L",
			"balance_R",
			"fan_ls_on_",
			"fan_ls_off_",
			"fan_hs_on_",
			"fan_hs_off_",
			"shutdown_",
			"smps_shutdown_",
			"volume_tx_delay_",
			"smps_fan_on_",
			"smps_fan_off_",
			"volume_",
			"volume_l_",
			"volume_r_",
			"led_dimmer_"
			
};
const char AsciiKeyValueTable_RKB_IN_CHANNEL[NUM_INPUT_CHANNEL]={
            SK_LOCAL_INPUT_A,
			SK_LOCAL_INPUT_B,
			SK_LOCAL_INPUT_C,
			SK_LOCAL_INPUT_D,
			SK_LINK_INPUT_A,
			SK_LINK_INPUT_B,
			SK_LINK_INPUT_C,
			SK_LINK_INPUT_D
};
const char cmd_list_input_channel[NUM_INPUT_CHANNEL][4]={
            "00A",
			"00B",
			"00C",
			"00D",
			"01A",
			"01B",
			"01C",
			"01D"
};
void rs232_key_type_init(void)
{
   u8 i;
   RS232_KEY.cmd_for_channel = UNDEFINE;
   RS232_KEY.id              = UNDEFINE;
   RS232_KEY.cmd_key         = UNDEFINE;
   RS232_KEY.key_para        = UNDEFINE;

   for(i=0;i<COMMAND_SIZE;i++) command[i]=0;
   RS232_KEY.p_cmd           = command;
   command_count=0;
   RS232_KEY.key_to_be_pro = 0;
}

u8 add_prefix(u8 *des,u8 prefix_id)
{
    u8 i=0;
	for(i=0;i<strlen((char const*)prefix[prefix_id]);i++)
		{
           des[i]=prefix[prefix_id][i];
	    }
	return i;
}

bool key_rs232_check(void)
{
	char data,result;

	if(rs232_Rx_Tail == rs232_Rx_Head || RS232_KEY.key_to_be_pro) {  //add key structure check
		return(FALSE);
	}else{
		data = GetChar();
		if (data == '!') {
			result = rs232_key_parse();					//?ASCiiKey check
			//rs232_key_type_init();
		}else{
		    RS232_KEY.p_cmd[command_count] = data;
			command_count++;
			if (command_count >= 32) {						//? key length : Max
				rs232_key_type_init();
			}
			result = FALSE;
		}
    	return(result);
	}   
}
bool rs232_key_parse(void)
{
    bool result=FALSE;
    u8 i,command_buffer[COMMAND_SIZE],key_found=0;
	for(i=0;i<COMMAND_SIZE && RS232_KEY.p_cmd[i] != '\0';i++)
		{
          command_buffer[i]=RS232_KEY.p_cmd[i];
	    }
	RS232_KEY.id = get_mode_id(command_buffer);

	RS232_KEY.cmd_for_channel = command_buffer[2];

    if(RS232_KEY.cmd_for_channel == '0')
    {
       RS232_KEY.cmd_for_channel=ALL_CHANNEL;
     }
    else
    {
        RS232_KEY.cmd_for_channel=command_buffer[2]-'A';
    }
	if(RS232_KEY.id == RS232_KEY.id)
		{
          //trans to other mode;
          //rs232_key_type_init();  //initialize the struct
          result=TRUE;
          key_found=get_info_from_command(&RS232_KEY);
		  RS232_KEY.key_to_be_pro = 1;
	    }

	if(key_found != 1)
		{
          //transmit_to_other_mode();
		  rs232_key_type_init();
	    }
	
	return result;
}
u8 get_mode_id(u8 * cmd)
{
   u8 result=MODE_ERROR;
   if(ISEQUAL(cmd[0], '0' )&& ISEQUAL(cmd[1],'0'))
   	{
      result=MODE_ID_00;
    }
   else if(ISEQUAL(cmd[0], '0' )&& ISEQUAL(cmd[1],'1'))
   	{
      result=MODE_ID_01;
    }

   return result;
}
/*
   get the info from the command
   return:1--found,0--not found
*/
u8 get_info_from_command(STRING_CMD_TYPE *key_struct)
{
   u8 i,found=0,temp_para[2],cmd_size,len;
   char cmd[COMMAND_SIZE];

   for(i=0;i<COMMAND_SIZE-3 && key_struct->p_cmd[i+3] != '\0';i++)
   	{
        cmd[i]=key_struct->p_cmd[i+3];
    }
   cmd[i]=0;
   cmd_size=i;

   for(i=0;i<NUM_CMD_X;i++)
   	{
   	  len=strlen(cmd_list_x[i]);
      if(!strncmp(cmd,cmd_list_x[i],len))
      	{
          found=1;
		  key_struct->key_type = KEY_SET;
		  key_struct->cmd_key=AsciiKeyValueTable_RKB_X[i];
		  switch(cmd_size-len)
		  	{
              case 1:
			  	temp_para[0]=cmd[len];
				if(ISDIGIT(temp_para[0]))
					{
                      key_struct->key_para=(temp_para[0]-'0');
				    }
			  	break;
			  case 2:
			  	temp_para[0]=cmd[len];
				temp_para[1]=cmd[len+1];
				if(ISDIGIT(temp_para[0]) && ISDIGIT(temp_para[1]))
					{
                      key_struct->key_para=(temp_para[0]*10+temp_para[1]);
				    }
			  	break;
		    }
          break;
	    }
    } //NUM_CMD_X key check
   if(found==1) return found;

   for(i=0;i<NUM_INPUT_CHANNEL;i++)
   	{
       if(!strcmp(cmd,cmd_list_input_channel[i]))
       	{
           found = 1;
		   key_struct->cmd_key=AsciiKeyValueTable_RKB_IN_CHANNEL[i];
		   key_struct->key_type = KEY_INPUT_SET;	
           break;
	    }
    }//INPUT SET COMMMAND check
   if(found==1) return found;

   for(i=0;i<NUM_CMD;i++)
   	{
      if(!strcmp(cmd,cmd_list[i]))
      	{
          found = 1;
		  key_struct->cmd_key=AsciiKeyValueTable_RKB[i];
		  key_struct->key_type = KEY_NORMAL_CMD;
          break;
	    }
    }//normal command check

   return found;
}
void key_input_check(void)
{
   if(TRUE==key_rs232_check())
   	{
       rs232_key_process(&RS232_KEY);
    }
}
void rs232_key_process(STRING_CMD_TYPE * cmd)
{
   switch(cmd->key_type)
   	{
      case KEY_SET:
	  	proc_value_set(cmd->cmd_key,cmd->key_para,cmd->cmd_for_channel);
	  	break;
	  case KEY_INPUT_SET:
	  	proc_input_channel_set(cmd->cmd_key,cmd->cmd_for_channel);
	  	break;
	  case KEY_NORMAL_CMD:
	  	proc_normal_cmd(cmd->cmd_key,cmd->cmd_for_channel);
	  	break;
    }
   
   rs232_key_type_init();
}
/*
  process the command from RS232 and Ethernet
  cmd_key:  COMMAND
  cmd_para: para command
  chnl: channel to set
*/
void proc_value_set(u8 cmd_key,u8 cmd_para,u8 chnl)
{ 
   u8 i;
   if(IS_CHANNEL(chnl)) return;
   switch(cmd_key)
   	{
      case SK_SET_BALANCE_L:
	  	if(chnl==ALL_CHANNEL)
	  		{
	  		 for(i=CHANNEL_A;i<=CHANNEL_D;i++)
	  		   RKB_Channel[i].p_balance_contrl(&RKB_Channel[i],(15 - cmd_para)); 
	  		}
		else
			{
	  	     RKB_Channel[chnl].p_balance_contrl(&RKB_Channel[chnl],(15 - cmd_para));
			}
	  	break;
	  case SK_SET_BALANCE_R:
	  	if(chnl==ALL_CHANNEL)
	  		{
  	  		 for(i=CHANNEL_A;i<=CHANNEL_D;i++)
	  		   RKB_Channel[i].p_balance_contrl(&RKB_Channel[i],(15 + cmd_para));             
		    }
		else
			{
	  	       RKB_Channel[chnl].p_balance_contrl(&RKB_Channel[chnl],(15 + cmd_para));
			}
		break;
	  case SK_SET_FAN_HS_OFF:
	  	set_fan_hs_off_amp_temp(cmd_para);
	  	break;
	  case SK_SET_FAN_HS_ON:
	  	set_fan_hs_on_amp_temp(cmd_para);
	  	break;
	  case SK_SET_FAN_LS_OFF:
	  	set_fan_ls_off_amp_temp(cmd_para);
	  	break;
	  case SK_SET_SMPS_FAN_OFF:
	  	set_fan_off_smps_temp(cmd_para);
	  	break;
	  case SK_SET_SMPS_FAN_ON:
	  	set_fan_on_smps_temp(cmd_para);
	    break;
	  case SK_SET_SHUTDOWN:
	  	set_amp_shutdown_temp(cmd_para);
	  	break;
	  case SK_SET_SMPS_SHUTDOWN:
	  	set_smps_shutdown_temp(cmd_para);
	  	break;
	  case SK_SET_VOLUME:
	  	//if(cmd_para<=96)
	  	if(chnl==ALL_CHANNEL)
	  		{
              for(i=CHANNEL_A;i<=CHANNEL_D;i++)
			  	RKB_Channel[i].p_vol_control(&RKB_Channel[i],VOL_ALL,0,cmd_para);
			  	
		    }
		else
			{
	  	      RKB_Channel[chnl].p_vol_control(&RKB_Channel[chnl],VOL_ALL,0,cmd_para);
			}
	  	break;
	  case SK_SET_VOLUME_L:
	  	//if(cmd_para<=96)
	  	if(chnl==ALL_CHANNEL)
	  		{
	  		 for(i=CHANNEL_A;i<=CHANNEL_D;i++)
	  	      RKB_Channel[i].p_vol_control(&RKB_Channel[i],VOL_LEFT,0,cmd_para);
	  		}
		else
			{
               RKB_Channel[chnl].p_vol_control(&RKB_Channel[chnl],VOL_LEFT,0,cmd_para);
		    }
	  	break;
	  case SK_SET_VOLUME_R:
	  	if(chnl==ALL_CHANNEL)
	  		{
              for(i=CHANNEL_A;i<=CHANNEL_D;i++)
			  	RKB_Channel[i].p_vol_control(&RKB_Channel[i],VOL_RIGHT,0,cmd_para);
		    }
		else
			{
	  	       RKB_Channel[chnl].p_vol_control(&RKB_Channel[chnl],VOL_RIGHT,0,cmd_para);
			}
	  	break;
	  case SK_SET_VOLUME_TX_DLY:
	  	break;
    }
}
/*
     process the command from RS232 and Ethernet
     cmd_key: command key
     chnl: Channel to set
*/
void proc_input_channel_set(u8 cmd_key,u8 chnl)
{
    if(IS_CHANNEL(chnl)) return;
    switch(cmd_key)
    	{
           case SK_LOCAL_INPUT_A:
		   	RKB_Channel[chnl].p_local_speaker_src_sel(&RKB_Channel[chnl],INPUT_A1_B1);
		   	break;
		   case SK_LOCAL_INPUT_B:
		   	RKB_Channel[chnl].p_local_speaker_src_sel(&RKB_Channel[chnl],INPUT_A2_B2);
		   	break;
		   case SK_LOCAL_INPUT_C:
		   	RKB_Channel[chnl].p_local_speaker_src_sel(&RKB_Channel[chnl],INPUT_A3_B3);
		   	break;	
		   case SK_LOCAL_INPUT_D:
		   	RKB_Channel[chnl].p_local_speaker_src_sel(&RKB_Channel[chnl],INPUT_A4_B4);
		   	break;	
			
		   case SK_LINK_INPUT_A:
		   	RKB_Channel[chnl].p_local_speaker_src_sel(&RKB_Channel[chnl],INPUT_A5_B5);
		   	break;
		   case SK_LINK_INPUT_B:
		   	RKB_Channel[chnl].p_local_speaker_src_sel(&RKB_Channel[chnl],INPUT_A6_B6);
		   	break;
		   case SK_LINK_INPUT_C:
		   	RKB_Channel[chnl].p_local_speaker_src_sel(&RKB_Channel[chnl],INPUT_A7_B7);
		   	break;	
		   case SK_LINK_INPUT_D:
		   	RKB_Channel[chnl].p_local_speaker_src_sel(&RKB_Channel[chnl],INPUT_A8_B8);
		   	break;			
	    }
}
/*
     process the command from RS232 and Ethernet
     cmd_key: command key
     chnl: Channel to set
*/
void proc_normal_cmd(u8 cmd_key,u8 chnl)
{
   switch(cmd_key)
   	{
      case SK_POWERON:
		exec_mode_power_on();
	  	break;
	  case SK_POWEROFF:
	  	exec_mode_power_off();
	  	break;
	  case SK_POWER:
	  	exec_mode_power_toggle();
	  	break;
	  case SK_GET_POWER:
	  	reply_mode_power_status();
	  	break;
	  case SK_PRODUCT_TYPE:
	  	reply_product_type();
	  	break;
	  case SK_PRODUCT_VERSION:
	  	reply_product_version();
	  	break;
	  case SK_GET_FREQ:
	  	reply_current_freq(chnl);
	  	break;
	  case SK_GET_TEMPERATURE:
	  	
	  	break;
	  case SK_GET_FAN_STATUS:
	  	reply_fan_status();
	  	break;
	  case SK_GET_AMP_STATUS:
	  	reply_amp_status();
	  	break;
	  case SK_GET_FAN_HS_ON_TEMP:
	  	reply_fan_hs_on_temp_val();
	  	break;
	  case SK_GET_FAN_HS_OFF_TEMP:
	  	reply_fan_hs_off_temp_val();
	  	break;
	  case SK_GET_SHUTDOWN_TEMP:
	  	reply_shutdown_temp_val();
	  	break;
	  case SK_GET_FAN_LS_ON_TEMP:
	  	reply_fan_ls_on_temp_val();
	  	break;
	  case SK_GET_FAN_LS_OFF_TEMP:
	  	reply_fan_ls_off_temp_val();
	  	break;	
	  case SK_GET_SMPS_FAN_ON_TEMP:
	  	reply_smps_fan_on_temp_val();
	  	break;
	  case SK_GET_SMPS_FAN_OFF_TEMP:
	  	reply_smps_fan_off_temp_val();
	  	break;		
	  case SK_SET_TEMPERATURE_ADC:
	  	
	  	break;
	  case SK_SET_TEMPERATURE_ON:
	  	
	  	break;
	  case SK_FACTORY_DEFAULT_SET:
	  	
	  	break;
	  case SK_AMP_PROTECTION_OFF:
	  	exec_amp_protection_off();
	  	break;
	  case SK_AMP_PROTECTION_ON:
	  	exec_amp_protection_on();
	  	break;
	  case SK_DISP_UPDATE:
	  	reply_display_update_status();
	  	break;
	  case SK_DISP_AUTO:
	  	exec_set_display_update_auto();                 //auto
	  	break;
	  case SK_DISP_MANUAL:
	  	exec_set_display_update_manual();                 //manual
	  	break;
	  case SK_GET_DELAYTIME_VOLUME:
	  	reply_delay_to_send_volume();
	  	break;
	  case SK_MUTE:
	  	exec_mute_toggle(chnl);
	    break;
	  case SK_MUTE_ON:
	  	exec_mute_on(chnl);
	  	break;
	  case SK_MUTE_OFF:
	  	exec_mute_off(chnl);
	  	break;
	  case SK_GET_AMPCH_STATUS:
	  	break;
	  case SK_AMPCH_ON:
	  	break;
	  case SK_AMPCH_OFF:
	  	break;
	  case SK_GET_VOLUME:
	  	break;
	  case SK_GET_VOLUME_MAX:
	  	break;
	  case SK_GET_VOLUME_MIN:
	  	break;
	  case SK_GET_VOLUME_L:
	  	break;
	  case SK_GET_VOLUME_R:
	  	break;
	  case SK_GET_BALANCE:
	  	break;
	  case SK_GET_LED_DIMMER:
	  	break;
	  case SK_VOL_UP:
	  	exec_volume_up(chnl);
	  	break;
	  case SK_VOL_DN:
	  	exec_volume_down(chnl);
	  	break;
	  case SK_VOL_L_UP:
	  	exec_volume_left_up(chnl);
	    break;
	  case SK_VOL_R_UP:
	  	exec_volume_right_up(chnl);
	  	break;
	  case SK_VOL_L_DN:
	  	exec_volume_left_down(chnl);
	  	break;
	  case SK_VOL_R_DN:
	  	exec_volume_right_down(chnl);
	  	break;
	  case SK_BALANCE_LEFT:
	  	exec_balance_left(chnl);
	  	break;
	  case SK_BALANCE_RIGHT:
	  	exec_balance_right(chnl);
	  	break;
	  case SK_GET_INPUT_SEL_MODE:
	  	reply_input_sel_mode(chnl);
	  	break;
	  case SK_INPUT_SEL_ANALOG:
	  	break;
	  case SK_INPUT_SEL_AUTO:
	  	break;
	  case SK_INPUT_SEL_DIGITAL:
	  	break;
    }
}
/*
    return the product version
*/
const u8 str_rkb_dm8100[]="product_type=10,RKB-DM8100";
void reply_product_type(void)
{
    u8 i = 0,j;
	
	i=add_prefix(response_data,0);
	
	for(j=0;j<strlen((char const*)str_rkb_dm8100);i++,j++)
		{
           response_data[i]=str_rkb_dm8100[j];
	    }
	response_data[i++]='\0';

	data_send(response_data,i,TUNNEL_ALL);
	
}
/*
   return the product version
*/

const u8 str_version[]="product_version=05,V1.01";
void reply_product_version(void)
{
    u8 i,j;

    i=add_prefix(response_data,0);
	
    for(j=0;j<strlen((char const*)str_version);i++,j++)
    	{
           response_data[i]=str_version[j];
	    }
	response_data[i++]='\0';

	data_send(response_data,i,TUNNEL_ALL);
	
}
/*
  return the current sampling frequency
*/
const u8 *freq_str[]={
            "freq=48!",
			"freq=44.1!",
			"freq=32!",
			"freq=96!",
			"freq=88.2!",
			"freq=192!",
			"freq=176.4!",
			"freq=off!"
 };
void reply_current_freq(u8 chnl)
{
    //u8 index;
    if(chnl>CHANNEL_D)
    	{
		  	freq_send_out(CHANNEL_A);
		  	freq_send_out(CHANNEL_B);
		  	freq_send_out(CHANNEL_C);
		  	freq_send_out(CHANNEL_D);		  
	    }
	else
		{
          if(RKB_Channel[chnl].amp_channel_on == 1)
          	{
				  freq_send_out(chnl);  
		    }
	    }
}

/*
   return the status of the fan speed
*/
const char *fan_speed[]={
            "fan=off!",
			"fan=low!",
			"fan=high!"
};
void reply_fan_status(void)
{
    u8 i,fan_index=0,j=0;
	i=add_prefix(response_data, 0);
    if(fan_status.fan_hs_status == FAN_ON)
    	{
           fan_index=2;
	    }
    else if(fan_status.fan_ls_status == FAN_ON)
        {
           fan_index=1;
	    }
	else
		{
           fan_index=0;
	    }
	for(j=0;j<strlen(fan_speed[fan_index]);i++,j++)    //strlen(fan_speed[fan_index])
		{
           response_data[i]=fan_speed[fan_index][j];
	    }
	response_data[i]='\0';

	data_send(response_data,i,TUNNEL_ALL);
}

/*
    return the status of the amp
*/
const char *str_amp_status[]={
                          "amp=normal!",
						  "amp=protection!"
};
void reply_amp_status(void)
{
    u8 i,index,j=0;

	i=add_prefix(response_data, 0);

	if(amp_status.protect_status == AMP_PROTECT_OFF)
		{
           index=0;
	    }
	else
		{
           index=1;
	    }
	for(j=0;j<strlen(str_amp_status[index]);i++,j++)
		{
           response_data[i]=str_amp_status[index][j];
	    }
	response_data[i]='\0';

	data_send(response_data,i,TUNNEL_ALL);
}
/*
   return  the high fan speed on temperature value
*/
const char *str_fan_hs_on_temp="fan_hs_on=  !";
void reply_fan_hs_on_temp_val(void)
{
    u8 i,j,val,val_10,val_1;

    i=add_prefix(response_data,0);
	val=get_fan_hs_on_amp_temp();

	val_10=val/10;
	val_1 =val%10;

	for(j=0;j<strlen(str_fan_hs_on_temp);i++,j++)
		{
          response_data[i]=str_fan_hs_on_temp[j];
	    }
	response_data[i-3]=val_10+'0';
	response_data[i-2]=val_1+'0';

	response_data[i]='\0';

	data_send(response_data,i,TUNNEL_ALL);
}
/*
  return the high fan speed off temperature value
*/
const char *str_fan_hs_off_temp="fan_hs_off=  !";
void reply_fan_hs_off_temp_val(void)
{
    u8 i,j,val,val_10,val_1;

    i=add_prefix(response_data,0);
	val=get_fan_hs_off_amp_temp();

	val_10=val/10;
	val_1 =val%10;

	for(j=0;j<strlen(str_fan_hs_off_temp);i++,j++)
		{
          response_data[i]=str_fan_hs_off_temp[j];
	    }
	response_data[i-3]=val_10+'0';
	response_data[i-2]=val_1+'0';

	response_data[i]='\0';

	data_send(response_data,i,TUNNEL_ALL);
}
/*
  return the amp's shutdown temperature value
*/
const char *str_shutdown_temp="shutdown_temp=  !";
void reply_shutdown_temp_val(void)
{
    u8 i,j,val,val_10,val_1;

    i=add_prefix(response_data,0);
	val=get_amp_shutdown_temp();

	val_10=val/10;
	val_1 =val%10;

	for(j=0;i<strlen(str_shutdown_temp);i++,j++)
		{
          response_data[i]=str_shutdown_temp[j];
	    }
	response_data[i-3]=val_10+'0';
	response_data[i-2]=val_1+'0';

	response_data[i]='\0';

	data_send(response_data,i,TUNNEL_ALL);
}
/*
  return the fan's low speed on temperature value
*/
const char *str_fan_ls_on_temp="fan_ls_on=  !";
void reply_fan_ls_on_temp_val(void)
{
    u8 i,j,val,val_10,val_1;

    i=add_prefix(response_data,0);
	val=get_fan_ls_on_amp_temp();

	val_10=val/10;
	val_1 =val%10;

	for(j=0;j<strlen(str_fan_ls_on_temp);i++,j++)
		{
          response_data[i]=str_fan_ls_on_temp[j];
	    }
	response_data[i-3]=val_10+'0';
	response_data[i-2]=val_1+'0';

	response_data[i]='\0';

	data_send(response_data,i,TUNNEL_ALL);    
}
/*
  return the fan's low speed off temperature value
*/
const char *str_fan_ls_off_temp="fan_ls_off=  !";
void reply_fan_ls_off_temp_val(void)
{
    u8 i,j,val,val_10,val_1;

    i=add_prefix(response_data,0);
	val=get_fan_ls_off_amp_temp();

	val_10=val/10;
	val_1 =val%10;

	for(j=0;i<strlen(str_fan_ls_off_temp);i++,j++)
		{
          response_data[i]=str_fan_ls_off_temp[j];
	    }
	response_data[i-3]=val_10+'0';
	response_data[i-2]=val_1+'0';

	response_data[i]='\0';

	data_send(response_data,i,TUNNEL_ALL);     
}
/*
  return the smps fan on temperature value
*/
const char *str_smps_fan_on="smps_fan_on=  !";
void reply_smps_fan_on_temp_val(void)
{
    u8 i,j,val,val_10,val_1;

    i=add_prefix(response_data,0);
	val=get_fan_on_smps_temp();

	val_10=val/10;
	val_1 =val%10;

	for(j=0;j<strlen(str_smps_fan_on);j++)
		{
          response_data[i]=str_smps_fan_on[j];
	    }
	response_data[i-3]=val_10+'0';
	response_data[i-2]=val_1+'0';

	response_data[i]='\0';

	data_send(response_data,i,TUNNEL_ALL);     
}
/*
  return the smps fan off temperature value
*/
const char *str_smps_fan_off_temp="smps_fan_off=  !";
void reply_smps_fan_off_temp_val(void)
{
    u8 i,j,val,val_10,val_1;

    i=add_prefix(response_data,0);
	val=get_fan_off_smps_temp();

	val_10=val/10;
	val_1 =val%10;

	for(j=0;j<strlen(str_smps_fan_off_temp);j++)
		{
          response_data[i]=str_smps_fan_off_temp[j];
	    }
	response_data[i-3]=val_10+'0';
	response_data[i-2]=val_1+'0';

	response_data[i]='\0';

	data_send(response_data,i,TUNNEL_ALL);         
}
/*

*/
void set_amp_protection(u8 val)
{
    amp_status.protect_func_on = val;
}
/*
  get the status of the display update mode
*/
const char *str_update_status[]={
                                 "display_update=auto!",
								 "display_update=manual!"
};
void reply_display_update_status(void)
{ 
    u8 i,j,index;
	i = add_prefix(response_data, 0);
    if(mode_display_update==0)
    	{
          index = 0;      //display update auto
	    }
	else 
		{
          index = 1;     //display update manual
	    }
	for(j=0;j<strlen(str_update_status[index]);i++,j++)
		{
          response_data[i]=str_update_status[index][j];
	    }

    response_data[i]='\0';
	data_send(response_data,i,TUNNEL_ALL);
}
/*
  set the display update mode
*/
void set_display_update(u8 data)
{
    if(data != 0 || data != 1) return;
    mode_display_update = data;
	reply_display_update_status();
}
/*
  get the delay time to send the volume
*/
const char *str_delay_time_volume_send="delay_time_volume_tx=  !";
void reply_delay_to_send_volume(void)
{
    u8 i,j,val,val_10,val_1;
	
    val=volume_send_delay_time;
	val_10=volume_send_delay_time/10;
	val_1 =volume_send_delay_time%10;
	
	i=add_prefix(response_data, 0);
    for(j=0;j<strlen(str_delay_time_volume_send);i++,j++)
    	{
          response_data[i]=str_delay_time_volume_send[j];
	    }
    response_data[i-2]=val_10+'0';
    response_data[i-2]=val_1+'0';
	response_data[i]='\0';

	data_send(response_data,i,TUNNEL_ALL);
	
}

void reply_balance_val(u8 chnl)
{
          
}
/*
  send back the input mode(auto,digital or analog)
*/
const char *str_input_sel_mode[]={
                                   "input_sel_mode=auto!",
								   "input_sel_mode=digital!",
								   "input_sel_mode=analog!"
                                 };
void reply_input_sel_mode(u8 chnl)
{
    u8 i,j,index;

	index=RKB_Channel[chnl].digital_nAnalog;
	i=add_prefix(response_data,chnl);

	for(j=0;j<strlen(str_input_sel_mode[index]);i++,j++)
		{
           response_data[i]=str_input_sel_mode[index][i];
	    }
	response_data[i]='\0';

	data_send(response_data,i,TUNNEL_ALL);
}
/*
  send out the mute status of the channel
*/
const char *str_mute_status[]={
                             "mute=on!",
							 "mute=off!"
};
void reply_mute_status(u8 chnl)
{
    u8 i,j,index;

	i=add_prefix(response_data, chnl+1);

	if(RKB_Channel[chnl].mute_status == MUTE_ON)
		{
          index=0;
	    }
	else if(RKB_Channel[chnl].mute_status == MUTE_OFF)
		{
          index=1;
	    }
	for(j=0;j<strlen(str_mute_status[index]);i++,j++)
		{
          response_data[i]=str_mute_status[index][j];
	    }
	response_data[i]='\0';

	data_send(response_data,i,TUNNEL_ALL);
}
/*
  send out the frequency
*/
void freq_send_out(u8 chnl)
{
    u8 index,size=0,temp_size;

    if(RKB_Channel[chnl].amp_channel_on == 0) return;
	
    temp_size=rotel_strncpy(response_data,mode_id,2); //copy mode id
	response_data[2]='A'+chnl;              //add channel
	size=temp_size+1;	
	
	if(RKB_Channel[chnl].ak4117_data->stream_kind != STREAM_PCM || RKB_Channel[chnl].ak4117_data->pll_status==PLL_UNLOCK)
		{
		  index=get_freq_index(0xff);                                //freq off
		}
	else
		{
	      index=get_freq_index(RKB_Channel[chnl].ak4117_data->freq); //get the index;
	    }
	
	temp_size=rotel_strncpy(&response_data[3], freq_str[index], strlen((const char*)freq_str[index]));  
	size+=temp_size;
    //size++;
    response_data[size]='\0';

	data_send(response_data,size,TUNNEL_ALL);	
}
/*
  get the index of the freq_str array
*/
u8 get_freq_index(u8 data)
{
   u8 result;
   switch(data)
   	{
       case FRQ_48KHZ:  result = 0;  break;
	   case FRQ_44KHZ:  result = 1;  break;
	   case FRQ_32KHZ:  result = 2;  break;
	   case FRQ_96KHZ:  result = 3;  break;
	   case FRQ_88KHZ:  result = 4;  break;
	   case FRQ_192KHZ: result = 5;  break;
	   case FRQ_176KHZ: result = 6;  break;
	   default:         result = 7;  break;
    }
   return result;
}
u8 rotel_strncpy(u8 *des,const u8 *src,u8 num)
{
    u8 i=0;
    for(i=0;i<num;i++)
    	{
          des[i] = src[i]; 
	    }
	return i;
}

/*
  send data to RS232,ETHERNET,UART_OUT
*/
void data_send(u8 * data,u8 size ,u8 send_kind)
{
    u8 i=0;

    switch(send_kind)
    	{
          case TUNNEL_ALL:
            for(i=0;i<size && i <RESPONSE_SIZE;i++)
          	   {
                 PutChar(data[i]);  //PC
				 
				 PutChar5(data[i]); //output uart5         		 
		       }		  	
		  	break;
		  case TUNNEL_PC:
		  	break;
		  case TUNNEL_ETH:
		  	break;
		  case TUNNEL_UART5:
		  	break;
	    }
}

void reply_mode_power_status(void)
{
    switch(power_status.pwr_status)
    	{
           case MAIN_POWER_OFF:
		   	
		   	break;
		   case MAIN_POWER_ON:
		   	break;
	    }
}
/*
  execute the power command from Master
*/
void exec_mode_power_on(void)
{
    //call the power on func
    mode_power_on();
}
/*
*/
void exec_mode_power_off(void)
{
    mode_power_off();
}
/**/
void exec_mode_power_toggle(void)
{
    if(power_status.pwr_status == MAIN_POWER_OFF)
    	{
          mode_power_on();
	    }
	else if(power_status.pwr_status == MAIN_POWER_ON)
		{
          mode_power_off();
	    }
}
/*
  execute the amp protection set
*/
void exec_amp_protection_off(void)
{
    set_amp_protection(PROTECT_FUNCTION_OFF);
}
/*
*/
void exec_amp_protection_on(void)
{
    set_amp_protection(PROTECT_FUNCTION_ON);
}

/*
  execute the setup of the update mode
*/
void exec_set_display_update_auto(void)
{
    //if(mode_display_update == 0) return;
	
    mode_display_update = 0;
	reply_display_update_status();
}
void exec_set_display_update_manual(void)
{
    //if(mode_display_update == 1) return;
	
    mode_display_update = 1;
	reply_display_update_status();
}

/*
  execute the mute control
*/
void exec_mute_toggle(u8 chnl)
{
    if(RKB_Channel[chnl].amp_channel_on == 1)
    	{
          if(RKB_Channel[chnl].mute_status == MUTE_ON)
          	{
              mode_mute_off(chnl);
		    }
		  else
		  	{
              mode_mute_on(chnl); 
		    }
		  reply_mute_status(chnl);
	    }
}
/*
*/
void exec_mute_on(u8 chnl)
{
    u8 i;
    if(RKB_Channel[chnl].amp_channel_on == 1)
    	{
          if(chnl>CHANNEL_D)
    	    {
              mode_mute_on(MUTE_ALL);
			  for(i=CHANNEL_A;i<=CHANNEL_D;i++)
			  	{
                  reply_mute_status(i);
			    }
	        }
		  else
		  	{
              mode_mute_on(chnl);
			  reply_mute_status(chnl);
		    }
    	}
}
/*
*/
void exec_mute_off(u8 chnl)
{
    u8 i;
    if(RKB_Channel[chnl].amp_channel_on == 1)
    	{
          if(chnl>CHANNEL_D)
    	    {
              mode_mute_off(MUTE_ALL);
			  for(i=CHANNEL_A;i<=CHANNEL_D;i++)
			  	{
                  reply_mute_status(i);
			    }
	        }
		  else
		  	{
              mode_mute_off(chnl);
			  reply_mute_status(chnl);
		    }
    	}  
}
/*
  execute the volume control
*/
void exec_volume_up(u8 chnl)
{
    RKB_Channel[chnl].p_vol_control(&RKB_Channel[chnl],VOL_ALL,DIR_UP,0);    
}
/*
*/
void exec_volume_down(u8 chnl)
{
    RKB_Channel[chnl].p_vol_control(&RKB_Channel[chnl],VOL_ALL,DIR_DOWN,0);      
}
/*
*/
void exec_volume_left_up(u8 chnl)
{
    RKB_Channel[chnl].p_vol_control(&RKB_Channel[chnl],VOL_LEFT,DIR_UP,0);   
}
/*
*/
void exec_volume_left_down(u8 chnl)
{
    RKB_Channel[chnl].p_vol_control(&RKB_Channel[chnl],VOL_LEFT,DIR_DOWN,0);   
}
/*
*/
void exec_volume_right_up(u8 chnl)
{
    RKB_Channel[chnl].p_vol_control(&RKB_Channel[chnl],VOL_RIGHT,DIR_UP,0);   
}
/*
*/
void exec_volume_right_down(u8 chnl)
{
    RKB_Channel[chnl].p_vol_control(&RKB_Channel[chnl],VOL_RIGHT,DIR_DOWN,0);   
}
/*
   execute the balance control
*/
void exec_balance_left(u8 chnl)
{
    RKB_Channel[chnl].p_balance_contrl(&RKB_Channel[chnl],DIR_DOWN);
	reply_balance_val(chnl);
}
void exec_balance_right(u8 chnl)
{
    RKB_Channel[chnl].p_balance_contrl(&RKB_Channel[chnl],DIR_UP);
	reply_balance_val(chnl);
}
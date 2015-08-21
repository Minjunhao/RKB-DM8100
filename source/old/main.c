/**************** (c)2012 Rotel ************************************************
PROJECT         : RKB-850,8100,D850,D8100
COMPILER        : IAR Embeded Workbench 5.50
MODULE          : main.c
VERSION         : V 1.0
USED CHIP       : stm32f207ZE [Z=144pin  E=512KB/128KB]
CREATION DATE   : /2015
AUTHORS         : /Rotel
DESCRIPTION     :
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "initmcu.h"
#include "uart.h"
#include "main.h"
#include "timer.h"
#include "adckey.h"
#include "adckey.ext"
#include "eeprom.h"
#include "i2c.h"


#include "proc_function.h"

#include "function.ext"

#include "proc_display.ext"
#include "proc_display.h"

#include "ak4114.ext"
#include "ak4114.h"

#include "RKB_function.h"

#ifdef IPOD_INCLUDE
#include "iPod_config.h"
#endif

/*----------------------------------------------------------------------------
UART1 : RS232(DB-9)
UART2 : not used
UART3 : not used
UART4 : not used
UART5 : not used
    
----------------------------------------------------------------------------*/


#ifdef IC_DEBUG
void IC_DebugHandler(void);
#endif



#ifdef IAP_UPGRADE
/* for jump to upgrade code */
typedef	void (*pFunction)(void);
#define ApplicationAddress    0x8000000
pFunction 	Jump_To_Application;
u32 		JumpAddress;
#endif

extern const byte tbl_ascii[];
extern void __iar_program_start( void );		//20131203


/*----------------------------------------------------------------------------*/

#define _MN_ "MAIN: "

#ifdef test_VFD
const unsigned char str_Test[4][20] = { /*2345678901234567890*/
										"ABCDEFGHIJKLMNOPQRST",
										"ABCDEFGHIJKLMNOPQRST",
										"ABCDEFGHIJKLMNOPQRST",
										"ABCDEFGHIJKLMNOPQRST"};
#endif

/*********************************************************************/
//--------------------------------------------------------------------
//====================================================================
#define NUM_CMD (59)     			//20150302     
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
//--------------------------------------------------------------------
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
};  // command list
//====================================================================
const char power_list[3][12+1] = {
			"power_on","power_off","power_toggle"
};
const byte power_listValue[] = {
			SK_POWERON,SK_POWEROFF,SK_POWER
};
//---------------------------------------------------------
const char RKB_subID_productType_list[5][3+1] = {
			"00:",
			"0A:",
			"0B:",
			"0C:",
			"0D:"
};
//---------------------------------------------------------
const char get_RotelLinkGetCommande_list[16+8][21+1] = {		//20140211 => range must re-check ???????????????
			"get_current_power",
			"get_product_type",
			"get_product_version",

			"get_current_freq",
			"get_amp_trim",
			"get_temperature",
			"get_fan_status",
			"get_amp_status",
//8
			"get_fan_on",
			"get_fan_off",
			"get_shutdown",	
			"get_smps_shutdown",
//12
			"get_display_update",		//20131212
			"display_update_auto",		//20131212
			"display_update_manual",	//20131212
//15
			"get_delay_time_volume",	//20131218
//16
			"get_channel_status",		//20140211 : add rs232 trim L/R control
			"get_front_trim_status",
			"get_volume",
			"get_volume_min",
			"get_volume_max",
			"get_volume_l",
			"get_volume_r",
			"get_balance"
//+8
};
//--------------------------------------------------------------------
//--------------------------------------------------------------------
const char balanceLevel_list[31][11+1] = {
			"balance_L15","balance_L14","balance_L13","balance_L12","balance_L11","balance_L10","balance_L09","balance_L08",
			"balance_L07","balance_L06","balance_L05","balance_L04","balance_L03","balance_L02","balance_L01","balance_000",
			"balance_R01","balance_R02","balance_R03","balance_R04","balance_R05","balance_R06","balance_R07","balance_R08",
			"balance_R09","balance_R10","balance_R11","balance_R12","balance_R13","balance_R14","balance_R15"
};
//--------------------------------------------------------------------
//====================================================================
/*

	key_Type = 0x10;    //normal
	key_Type = 0x30;    //volume_##!
	key_Type = 0x31;    //volume_l_##!
	key_Type = 0x32;    //volume_r_##!
	
	key_Type = 0x37;    //led_dimmer_#!

	key_Type = 0x38;    //direct Fan LS On Temperature  [30..85]				: (fan_ls_on_temp) >= (fan_ls_off_temp)+2
	key_Type = 0x39;    //direct Fan LS Off Temperature [28..85]				:
	key_Type = 0x40;    //direct Fan HS On Temperature  [30..85]				: (fan_hs_on_temp) >= (fan_hs_off_temp)+2
	key_Type = 0x41;    //direct Fan Hs Off Temperature [28..85]				:
	key_Type = 0x42;    //direct Main Heat Sink sensor Temperature [31..86] : (shutdown temp >= (fan_hs_on_temp)+1
	key_Type = 0x43;    //direct SMPS Heat Sink sensor Temperature [31..99]
	key_Type = 0x44;    //direct direct volume tx delay time set : [00..30]

	key_Type = 0x47;    //balance_###!
	key_Type = 0x48;    //direct SMPS Fan On Temperature  [30..85]			: (smps_fan_on_temp) >= (smps_fan_off_temp)+2
	key_Type = 0x49;    //direct SMPS Fan Off Temperature [28..85]			:
	
call RS232 check
call uart3_GetRxCommand
*********************************************************************/
//20140521 : add/change ls_ and hs_
char str_fan_ls_onIs[10] =	"fan_ls_on_";
char str_fan_ls_offIs[11] =	"fan_ls_off_";
char str_fan_hs_onIs[10] =	"fan_hs_on_";
char str_fan_hs_offIs[11] =	"fan_hs_off_";
char str_shutdownIs[9] =	"shutdown_";
char str_SMPSshutdownIs[14] =		"smps_shutdown_";						//20131024
char str_DelayTimeVolumeTxIs[16] =	"volume_tx_delay_";						//20131218
char str_smps_fan_onIs[12] =		"smps_fan_on_";							//20140311
char str_smps_fan_offIs[13] =		"smps_fan_off_";						//20140311
//--------------------------------------------------------------------
char str_volumeIs[7] = 		"volume_";										//20140212
char str_volume_L_Is[9] = 	"volume_l_";									//20140212
char str_volume_R_Is[9] = 	"volume_r_";									//20140212
//--------------------------------------------------------------------
const char ledDimmerLevel_list[10][12+1] = {
				"led_dimmer_0",
				"led_dimmer_1",
				"led_dimmer_2",
				"led_dimmer_3",
				"led_dimmer_4",
				"led_dimmer_5",
				"led_dimmer_6",
				"led_dimmer_7",
				"led_dimmer_8",
				"led_dimmer_9"
};
//--------------------------------------------------------------------
//--------------------------------------------------------------------
//--------------------------------------------------------------------
int getCmdID(char* comand, char* cmd_type)
{
	int ret=-1;
	unsigned char data;
	unsigned char i,k,size;


    for (i=0,size=0; (i<32)&&(comand[i]!='\0'); i++,size++){ }			//MAX=32
//	for (i=0;i<size;i++) {								
//		PutChar(comand[i]); 
//	}
    //--------------------------------------------------------------------
	//volume_#!
	//volume_##!
    //  type=0x30
    // 20140218
    //--------------------------------------------------------------------
	if (front_trimKnob_enable == OFF)
	{    
        for (i=0,k=0;(i<size)&&(i<7);i++) {
            if ( comand[i] == str_volumeIs[i] ) k++;
        }
        if (k == 7)
        {	    
			switch(size-7) {
				case 1:
		            //direct volume : volume_#!
                	// 0 ~ 9,0x30 ~ 0x39
                    if ( (comand[7] >= 0x30) && (comand[7] < 0x3a) ) {
                        data = comand[7] - 0x30;
                        //direct volume                 
		                *cmd_type = 0x30;
        	
            		    ret = data;         //20121008
            		    return ret;
            		}
                    break;
				case 2:
		            //direct volume : volume_##!
                    if ( (comand[7] >= 0x30) && (comand[7] < 0x3a) && (comand[8] >= 0x30) && (comand[8] < 0x3a) ) {
                        data = (comand[7]-0x30)*0x0a + (comand[8]-0x30);                  
                        //direct volume                 
		                *cmd_type = 0x30;
        
            		    ret = data;         //20121008
                		return ret;
                	}
                    break;
        		default:
		    		break;
            }
        }
    }
    //--------------------------------------------------------------------
	//volume_l_#!
	//volume_l_##!
    //  type=0x31
    // 20140218
    //--------------------------------------------------------------------
	if (front_trimKnob_enable == OFF)
	{    
        for (i=0,k=0;(i<size)&&(i<9);i++) {
            if ( comand[i] == str_volume_L_Is[i] ) k++;
        }
        if (k == 9)
        {	    
			switch(size-9) {
				case 1:
		            //direct volume : volume_l_#!
                	// 0 ~ 9,0x30 ~ 0x39
                    if ( (comand[9] >= 0x30) && (comand[9] < 0x3a) ) {
                        data = comand[9] - 0x30;
                        //direct volume                 
		                *cmd_type = 0x31;
        	
            		    ret = data;         //20121008
            		    return ret;
            		}
                    break;
				case 2:
		            //direct volume : volume_l_##!
                    if ( (comand[9] >= 0x30) && (comand[9] < 0x3a) && (comand[10] >= 0x30) && (comand[10] < 0x3a) ) {
                        data = (comand[9]-0x30)*0x0a + (comand[10]-0x30);                  
                        //direct volume                 
		                *cmd_type = 0x31;
        
            		    ret = data;         //20121008
                		return ret;
                	}
                    break;
        		default:
		    		break;
            }
        }
    }
    //--------------------------------------------------------------------
	//volume_r_#!
	//volume_r_##!
    //  type=0x32
    // 20140218
    //--------------------------------------------------------------------
	if (front_trimKnob_enable == OFF)
	{    
        for (i=0,k=0;(i<size)&&(i<9);i++) {
            if ( comand[i] == str_volume_R_Is[i] ) k++;
        }
        if (k == 9)
        {	    
			switch(size-9) {
				case 1:
		            //direct volume : volume_r_#!
                	// 0 ~ 9,0x30 ~ 0x39
                    if ( (comand[9] >= 0x30) && (comand[9] < 0x3a) ) {
                        data = comand[9] - 0x30;
                        //direct volume                 
		                *cmd_type = 0x32;
        	
            		    ret = data;         //20121008
            		    return ret;
            		}
                    break;
				case 2:
		            //direct volume : volume_r_##!
                    if ( (comand[9] >= 0x30) && (comand[9] < 0x3a) && (comand[10] >= 0x30) && (comand[10] < 0x3a) ) {
                        data = (comand[9]-0x30)*0x0a + (comand[10]-0x30);                  
                        //direct volume                 
		                *cmd_type = 0x32;
        
            		    ret = data;         //20121008
                		return ret;
                	}
                    break;
        		default:
		    		break;
            }
        }
    }
    //--------------------------------------------------------------------
    //  type=0x37
    //--------------------------------------------------------------------
    //led_dimmer_#! [0..9]
	for (i=0; i<10; i++) {
		if (strcmp(comand, ledDimmerLevel_list[i])==0) {
            //led_dimmer_??            
            *cmd_type = 0x37;
           	ret = i;
            return ret;			//exit
		}
	} // for
    //--------------------------------------------------------------------
    //  type=0x38 : fan_ls_on_xx : range [ 30..85 ]
    //--------------------------------------------------------------------
    //20140521
    //fan_ls_on_##!
    for (i=0,k=0;(i<size)&&(i<10);i++) {
        if ( comand[i] == str_fan_ls_onIs[i] ) k++;
    }
    if (k == 10)
    {
    	if (size == 12) {  
	        //direct fan ls on : fan_ls_on_##!
			data = (comand[10]-0x30)*0x0a + (comand[11]-0x30);                  
            if ( ( data >= 30 ) && ( data <= 85) ) {
                //direct fan ls on xx                 
	            *cmd_type = 0x38;

        	    ret = data;
            	return ret;
            }
        }
    }    
    //--------------------------------------------------------------------
    //  type=0x39 : fan_ls_off_xx : range [ 28..85 ]
    //--------------------------------------------------------------------
    //20140521
    //fan_ls_off_##!
    for (i=0,k=0;(i<size)&&(i<11);i++) {
        if ( comand[i] == str_fan_ls_offIs[i] ) k++;
    }
    if (k == 11)
    {
    	if (size == 13) {  
	        //direct fan ls off : fan_ls_off_##!
			data = (comand[11]-0x30)*0x0a + (comand[12]-0x30);                  
            if ( ( data >= 28 ) && ( data <= 85) ) {
                //direct fan ls off xx                 
	            *cmd_type = 0x39;

        	    ret = data;
            	return ret;
            }
        }
    }    
    //--------------------------------------------------------------------
    //  type=0x40 : fan_hs_on_xx : range [ 30..85 ]
    //--------------------------------------------------------------------
    //20140521
    //fan_hs_on_##!
    for (i=0,k=0;(i<size)&&(i<10);i++) {
        if ( comand[i] == str_fan_hs_onIs[i] ) k++;
    }
    if (k == 10)
    {
    	if (size == 12) {  
	        //direct fan hs on : fan_hs_on_##!
			data = (comand[10]-0x30)*0x0a + (comand[11]-0x30);                  
            if ( ( data >= 30 ) && ( data <= 85) ) {
                //direct fan hs on xx                 
	            *cmd_type = 0x40;

        	    ret = data;
            	return ret;
            }
        }
    }    
    //--------------------------------------------------------------------
    //  type=0x41 : fan_hs_off_xx : range [ 28..85 ]
    //--------------------------------------------------------------------
    //20140521
    //fan_hs_off_##!
    for (i=0,k=0;(i<size)&&(i<11);i++) {
        if ( comand[i] == str_fan_hs_offIs[i] ) k++;
    }
    if (k == 11)
    {
    	if (size == 13) {  
	        //direct fan hs off : fan_hs_off_##!
			data = (comand[11]-0x30)*0x0a + (comand[12]-0x30);                  
            if ( ( data >= 28 ) && ( data <= 85) ) {
                //direct fan off xx                 
	            *cmd_type = 0x41;

        	    ret = data;
            	return ret;
            }
        }
    }    
    //--------------------------------------------------------------------
    //  type=0x48 : smps_fan_on_xx : range [ 30..85 ]
    //20140311
    //--------------------------------------------------------------------
    //smps_fan_on_##!
    for (i=0,k=0;(i<size)&&(i<12);i++) {
        if ( comand[i] == str_smps_fan_onIs[i] ) k++;
    }
    if (k == 12)
    {
    	if (size == 14) {  
	        //direct fan on : fan_on_##!
			data = (comand[12]-0x30)*0x0a + (comand[13]-0x30);                  
            if ( ( data >= 30 ) && ( data <= 85) ) {
                //direct fan on xx                 
	            *cmd_type = 0x48;

        	    ret = data;
            	return ret;
            }
        }
    }    
    //--------------------------------------------------------------------
    //  type=0x49 : smps_fan_off_xx : range [ 28..85 ]
    //--------------------------------------------------------------------
    //smps_fan_off_##!
    for (i=0,k=0;(i<size)&&(i<13);i++) {
        if ( comand[i] == str_smps_fan_offIs[i] ) k++;			//20140325
    }
    if (k == 13)
    {
    	if (size == 15) {  
	        //direct fan off : fan_off_##!
			data = (comand[13]-0x30)*0x0a + (comand[14]-0x30);                  
            if ( ( data >= 28 ) && ( data <= 85) ) {
                //direct fan off xx                 
	            *cmd_type = 0x49;

        	    ret = data;
            	return ret;
            }
        }
    }    
    //--------------------------------------------------------------------
    //  type=0x42 : shutdown_xx : range [ 31..86 ]
    //--------------------------------------------------------------------
    //shutdown_##!
    for (i=0,k=0;(i<size)&&(i<9);i++) {
        if ( comand[i] == str_shutdownIs[i] ) k++;
    }
    if (k == 9)
    {
    	if (size == 11) {  
	        //direct shutdown set : shutdown_##!
			data = (comand[9]-0x30)*0x0a + (comand[10]-0x30);                  
            if ( ( data >= 31 ) && ( data <= 86) ) {
                //direct shutdown xx                 
	            *cmd_type = 0x42;

        	    ret = data;
            	return ret;
            }
        }
    }    
    //--------------------------------------------------------------------
    //  type=0x43 : smps_shutdown_xx : range [ 31..99 ]
    //--------------------------------------------------------------------
    //smps_shutdown_##!
    for (i=0,k=0;(i<size)&&(i<14);i++) {
        if ( comand[i] == str_SMPSshutdownIs[i] ) k++;
    }
    if (k == 14)
    {
    	if (size == 16) {  
	        //direct smps shutdown set : smps_shutdown_##!
			data = (comand[14]-0x30)*0x0a + (comand[15]-0x30);				//20131024            
            if ( ( data >= 31 ) && ( data <= SMPS_TEMPERATURE_MAX) ) {		//20131205
                //direct smps shutdown xx                 
	            *cmd_type = 0x43;

        	    ret = data;
            	return ret;
            }
        }
    }    
    //--------------------------------------------------------------------
    //  type=0x44 : volume_tx_delay_xx : range [ 00..30 ]
    //--------------------------------------------------------------------
    //volume_tx_delay_##!
    for (i=0,k=0;(i<size)&&(i<16);i++) {
        if ( comand[i] == str_DelayTimeVolumeTxIs[i] ) k++;
    }
    if (k == 16)
    {
    	if (size == 18) {  
	        //direct volume tx delay time set : volume_tx_delay_##!
			data = (comand[16]-0x30)*0x0a + (comand[17]-0x30);				//20131218            
            if ( ( data >= 0 ) && ( data <= 30) ) {							//20131218
                //direct volume_tx_delay xx                 
	            *cmd_type = 0x44;

        	    ret = data;
            	return ret;
            }
        }
    }    
    //--------------------------------------------------------------------
	//balance_###!
    //  type=0x47
    // 20140212
    //--------------------------------------------------------------------
		for (i=0; i<31; i++) {
			if (strcmp(comand, balanceLevel_list[i])==0) {
                //balance volume                 
                *cmd_type = 0x47;
               	ret = i;
                return ret;			//exit
			}
		} // for
    //--------------------------------------------------------------------
    //  type=0x10 : Normal keys
    //--------------------------------------------------------------------
	for (i=0; i<NUM_CMD; i++) {
		if (strcmp(comand, cmd_list[i])==0) {
            //normal key
	        *cmd_type = 0x10;
			ret = i;
			break;
		}
	} // for
	return ret;
}
///////////////////////////////////////////////////////////
//	rx string check : (uart4/5)
///////////////////////////////////////////////////////////
//---------------------------------------------------------
//
//---------------------------------------------------------

/* 
     ascii to bin
     
*********************************************************************/
void makeRotelUTF8(unsigned short c,unsigned char* UTF8_buf, unsigned char* cnt)
{

	if (c < 0x0080) {
		UTF8_buf[0] = (unsigned char)c;
		*cnt = 1;
	}else if (c < 0x0800) {
		UTF8_buf[0] = 0xc0|(unsigned char)(c>>6);
		UTF8_buf[1] = 0x80|(unsigned char)(c&0x3f);
		*cnt = 2;
	}else{
		UTF8_buf[0] = 0xe0|(unsigned char)(c>>12);
		UTF8_buf[1] = 0x80|(unsigned char)((c>>6)&0x3f);
		UTF8_buf[2] = 0x80|(unsigned char)(c&0x3f);
		*cnt = 3;
	}
}
/* 
     ascii to bin
     
*********************************************************************/
unsigned char Asc2Bin(unsigned char ch1,unsigned char ch2)
{
	unsigned char comb;
	comb = 0;

	/* printf("%c,%c:",ch1,ch2); */
	/* 0 ~ 9,0x30 ~ 0x39 */
	if(ch1 < 0x3a){
		ch1 = ch1-0x30;
	/* A ~ F,0x41 ~ 0x46 */
	}else if(ch1 < 0x47){
		ch1 = ch1-0x41+0x0a;
	/* a ~ f,0x61 ~ 0x66 */
	}else{
		ch1 = ch1-0x61+0x0a;
	}
	ch1 = ch1 << 4;
	comb = ch1;
	comb &= 0xf0;

	/* printf(" %x ",comb); */
	/* 0 ~ 9,0x30 ~ 0x39 */
	if(ch2 < 0x3a){
		ch2 = ch2-0x30;
	/* A ~ F,0x41 ~ 0x46 */
	}else if(ch2 < 0x47){
		ch2 = ch2-0x41+0x0a;
	/* a ~ f,0x61 ~ 0x66 */
	}else{
		ch2 = ch2-0x61+0x0a;
	}
	comb |= ch2;
	/* printf("%x\n",comb); */
	return(comb);
}

/* 	
        shiftDatIdx[40]
        [ FTX_DISP_BUFF ] <== [ TDISP_TEMP_BUFF ]
*********************************************************************/
void 	CompareVFDDisplayBuffer_20msec(void)
{
	unsigned char i,j;

	if(mode_PowerStatus >= POWER_ON3_ING) 				//20121106
    {
#ifdef OLED_DISPLAY_UTF8
#ifdef STINGRAY_UTF8_4321
		//
		//OLED display
		//
		if(memcmp((void*)FTX_DISP_BUFF_UTF8.display_Buffer,(void*)TDISP_TEMP_BUFF_UTF8.display_Buffer,166) != 0){
			memcpy((void*)FTX_DISP_BUFF_UTF8.display_Buffer,(void*)TDISP_TEMP_BUFF_UTF8.display_Buffer,166);
			//			
			//OLED buffer setting
			//
			//			
#ifdef UART1_DEBUG_x
			//UART display(test)
				PutChar('\r');
				rs232_printf("OLED data :\r",blank);
				for (i=0;i<4;i++) {
					for (j=0;j<20;j++) {
						if (TDISP_TEMP_BUFF_UTF8.sUtf8Stingray.iDispBuf[i][j] < 0x7f) {
							PutChar((unsigned char)TDISP_TEMP_BUFF_UTF8.sUtf8Stingray.iDispBuf[i][j]);	//uart 1 : temp!
							//PutChar('*');
						}else{	
							//PutChar(TDISP_TEMP_BUFF_UTF8.sUtf8Stingray.iDispBuf[i][j]);			//uart 1 : temp!!!!!!!!!!!!!!!!!!!!
							PutChar(Asc2Bin((unsigned char)((TDISP_TEMP_BUFF_UTF8.sUtf8Stingray.iDispBuf[i][j]>>12)&0x000f),(unsigned char)((TDISP_TEMP_BUFF_UTF8.sUtf8Stingray.iDispBuf[i][j]>>8)&0x000f)));
							PutChar(Asc2Bin((unsigned char)((TDISP_TEMP_BUFF_UTF8.sUtf8Stingray.iDispBuf[i][j]>>4)&0x000f),(unsigned char)(TDISP_TEMP_BUFF_UTF8.sUtf8Stingray.iDispBuf[i][j]&0x000f)));
						}
					}
					PutChar('\r');
				}
#endif
		}
		//
		//
		//
#endif	//STINGRAY_UTF8_4321
#endif	//OLED_DISPLAY_UTF8
		//
		//VFD buffer loading
		//
		if (count_delayVFD == 0) {
        	//--------------------------------------------------------------------
        	//VFD display
        	//
			if(memcmp((void*)FTX_DISP_BUFF.display_Buffer,(void*)TDISP_TEMP_BUFF.display_Buffer,DISPLAYBUFFER_MAX_SIZE) != 0){
				memcpy((void*)FTX_DISP_BUFF.display_Buffer,(void*)TDISP_TEMP_BUFF.display_Buffer,DISPLAYBUFFER_MAX_SIZE);
#ifdef VFD_DISPLAY_ENABLE
				//VFD buffer setting
				for(i=0;i<20;i++)
				{
					//
					//VFD 2x20
					//
					shiftDatIdx[i+20] = TDISP_TEMP_BUFF.display_Buffer[i];
					shiftDatIdx[i] = TDISP_TEMP_BUFF.display_Buffer[i+20];
				}
#endif	//VFD_DISPLAY_ENABLE
#ifdef UART1_DEBUG_x
				//
				// 2x20
				//
				PutChar('\r');
				rs232_printf("--------------------\r",blank);
				for (i=0;i<2;i++) {
					for (j=0;j<20;j++) {
						if (TDISP_TEMP_BUFF.sStingray.DispBuf[i][j] == IRREGULAR_DATA) {
							PutChar('*');
						}else{	
							PutChar(TDISP_TEMP_BUFF.sStingray.DispBuf[i][j]);			//uart 1 : temp!!!!!!!!!!!!!!!!!!!!
						}
					}
					PutChar('\r');
				}
				rs232_printf("--------------------\r",blank);
				PutChar('\r');
#endif
			}	
    	}
    	//
    	//
    	//
    }
}

/*
	main_iPodReset(key)
*********************************************************************/
void	main_iPodKeyReset(void)
{
#ifdef IPOD_INCLUDE
extern unsigned char   currentScrollMode;
	KeyConfirm_BK = iPod_OFF;
	iPod_key_BK = KEY_IPOD_NULL;
    //next display : Title(0) => Artist(1) => Album(2) : RA1570 20120810
    currentScrollMode = 0;      //Title
#endif
}

/*
	TCLS_NaviMain(key)
*********************************************************************/
void	Main_TCLS_NaviMain(unsigned char key)
{
#ifdef IPOD_INCLUDE
	TCLS_NaviMain(key);
#endif
}

/*
	10msec

*********************************************************************/
void	CheckFunction_10msec(void)
{
//	PutString("timer_10msec val = %\n",timer_10msec);
	if (timer_Get10ms()==TRUE) {
	
        //
		if ( (RKB_ModelID == RKB_D850) || (RKB_ModelID == RKB_D8100) ) {
        	//20140108 : [ * >= POWER_ON2_ING ]           
			s_4114_Exec(0);
#ifdef MULTI_DIR_ENABLE
			s_4114_Exec(1);
			s_4114_Exec(2);
			s_4114_Exec(3);
#endif
		}
        //
        //
		switch(mode_PowerStatus) {
			case POWER_ON0:

#ifdef IPOD_INCLUDE
                TCLS_iPodMain(iPod_key_BK,&KeyConfirm_BK);	//iPod main
#endif
				if (amp_protextion_skip == 0){
                	// powerOn(after 5sec) start check
                	// AMP./RDY =>(High)=> AMP protection [ Low Level = 50msec ]
                	main_AmpProtectionCheck();          //?Amp_nProtection==High
                }
                //
				break;
			case POWER_ON0_ING:		//move 20131217
			case POWER_ON1_ING:		//move 20130708
			case POWER_ON2_ING:		//move 20130708
			case POWER_ON3_ING:		//move 20130708
			case POWER_ON4_ING:		//move 20131216
			case POWER_ON5_ING:		//move 20131216
			case POWER_OFF:
			default:
				break;
		}
		//read ADC keys : [ * >= POWER_ON3_ING ]
		adc_KeyBoardPer10ms();
        //----------------------------------------------------------------------
#ifdef SIGNAL_SENSING_ENABLE
        //20140523 : for signal sensing feature
        signal_sensingPort_check();
#endif
        //----------------------------------------------------------------------
	}
}
/* 	
	rs232 key 'refresh'

*********************************************************************/
void 	Func_RS232_TX(void)
{
//	PutString("key [refresh] \n",blank);
	f_tx_disp_buff = 0x03;					//RS232 TX : 20101110 : Line 1~4
}

/* 	
	20msec
	RS232 Tx
	f_tx_disp_buff
        [ TTX_DISP_BUFF ] <== [ TDISP_TEMP_BUFF ]
        [ TTX_DISP_BUFF_UTF8 ] <== [ TDISP_TEMP_BUFF_UTF8 ]
*********************************************************************/
void 	CompareTxDisplayBuffer_100msec(void)
{

	if(mode_PowerStatus >= POWER_ON3_ING) 				//20121106
    {
	//20110121 : refresh key? =>skip!
	if (f_tx_disp_buff == 0x03) return;
		
#ifdef STINGRAY_UTF8_4321	//20111129
//---------------------------------------------------------		
	if(memcmp((void*)TTX_DISP_BUFF_UTF8.display_Buffer,(void*)TDISP_TEMP_BUFF_UTF8.display_Buffer,DISPLAYBUFFER_UTF_MAX_SIZE) != 0){
//
//RS232 Tx[Unicode]
//
		memcpy((void*)TTX_DISP_BUFF_UTF8.display_Buffer,(void*)TDISP_TEMP_BUFF_UTF8.display_Buffer,DISPLAYBUFFER_UTF_MAX_SIZE);
	    // rs232 tx flag set
        //? display_update=auto!
        //20131212
        if(mode_DisplayUpdate == DISPLAY_AUTO) {
	    	// rs232 tx flag set
	    	f_tx_disp_buff = 0x03;
	    }
	}
//---------------------------------------------------------		
#else	//STINGRAY_UTF8_4321
//---------------------------------------------------------		
	if(memcmp(&TTX_DISP_BUFF.display_Buffer, &TDISP_TEMP_BUFF.display_Buffer,DISPLAYBUFFER_MAX_SIZE) != 0){
//
//RS232 Tx
//
		memcpy((void*)TTX_DISP_BUFF.display_Buffer,(void*)TDISP_TEMP_BUFF.display_Buffer,DISPLAYBUFFER_MAX_SIZE);
	    // rs232 tx flag set
        //? display_update=auto!
        //20131212
        if(mode_DisplayUpdate == DISPLAY_AUTO) {
	    	// rs232 tx flag set
	    	f_tx_disp_buff = 0x03;
	    }
	}
//---------------------------------------------------------		
#endif	//STINGRAY_UTF8_4321
    //
    }
}
/*
	20msec

*********************************************************************/
void 	CheckFunction_20msec(void)
{
	
	if (timer_Get20ms()==TRUE) {
        //power on -> change mode_PowerStatus
		Display_UpdateDisplayBuffer_20msec();		//[ TDISP_TEMP_BUFF ] <== [ TDISP_BUFF ]
        //vfd
		CompareVFDDisplayBuffer_20msec();           //[ FTX_DISP_BUFF ] <== [ TDISP_TEMP_BUFF ]
        //
        // f_tx_disp_buff == "0x03"
        // 125msec check time 을 20msec 로 변경 test ( 100msec subroutine call 은 삭제 ? )
#ifdef STINGRAY_UTF8_4321
		CompareTxDisplayBuffer_100msec();		    //[ TTX_DISP_BUFF ] <== [ TDISP_TEMP_BUFF ]
#endif
//				
        //----------------------------------------------------------------------
		// long key(repeatkey)처리.
        //----------------------------------------------------------------------
		//VFD buffer loading delay time /20msec
		if (count_delayVFD > 0) {
			--count_delayVFD;
		}
        //----------------------------------------------------------------------
        //ASCii data Tx
		if (f_tx_disp_buff == 3) {
			f_tx_disp_buff = 0;
			Func_DisplayAll();
		}
        //----------------------------------------------------------------------
        // 	call main_AmpProtectionExec();
        //	call Func_PowerStandByOn();
        //	call Func_PowerOff();
        //
    	// delay time 후, standby relay off
    	// PP : (500msec)->Standby(0):AllPortLow
    	// WS : (3000msec)->(2400msec)Standby(0) ->(0)AllPortLow
    	// 20131216 : (3000msec)->(2400msec) ->(0)AllPortLow
        //----------------------------------------------------------------------
	    if (count_powerOffDelay != 0) {
	    	--count_powerOffDelay;
	    	//20131206 : for WS
		    if (count_powerOffDelay == TIME20MSTO2400MS) {
			    Standby(0);			//stanby power OFF : 20130709
		    }
		    //
		    if (count_powerOffDelay == 0) {
				//----------------------------------------------------			
			    Standby(0);			//stanby power OFF : 20130709
				//----------------------------------------------------			
#ifdef POWER_CONSUMPTION	//20121030
			    SetConfigUART2n3ToTelechipsReciva(0);
#endif			
				//----------------------------------------------------			
			    //20220711 : stanby 시 port 정리..
			    Func_PowerOff_PortLow();          		//=> proc_function.c      
				//----------------------------------------------------			
				//----------------------------------------------------			
			    //after 500msec =>power consumption
			    //
#ifdef POWER_CONSUMPTION	//20121030
                //20121030 power consumption =>GPIO change GPIO_Mode_IN_FLOATING.
				if (RKB_mode_PP_MP == 0) {							//?(pin53:PC12): WS1/2=0, PP=1(add SMPS_sensor_1/2) : 20131024
                	GPIO_Configuration(0);      //20121030 : power consumption(0), Normal(1) [ =>Standby(1) 을 유지함... ]
                	//SetSDA;                 	//20121106 : IIC_SDA => GPIO_Mode_IN_FLOATING <== deleted 20140224
                }else{
					if (RKB_mode_PP_nMP2 == 0) {							//20140523 : add signal sensing feature
                		GPIO_Configuration_MP2(0);	//20131024 : power consumption(0), Normal(1) [ =>Standby(1) 을 유지함... ]
                		//SetSDA;                 	
					}else{
                		GPIO_Configuration_PP(0);	//20131024 : power consumption(0), Normal(1) [ =>Standby(1) 을 유지함... ]
                		//SetSDA;                 	//20121106 : IIC_SDA => GPIO_Mode_IN_FLOATING <== deleted 20140224
                	}
                }
#endif		
				//----------------------------------------------------			
				//20140403 deleted :
                SetSDA;                 //20121106 : IIC_SDA => GPIO_Mode_IN_FLOATING
				//----------------------------------------------------			
				//
				//
				//----------------------------------------------------
				//	power consumption mode -> current clock/8
				//----------------------------------------------------
  				//----------------------------------------------------------------
  				//	mode_AHB_prescaler
  				//	1=36MHz, 2=18MHz, 4=9MHz, 8=4.5MHz
  				// 20140409
  				//----------------------------------------------------------------
				mode_AHB_prescaler = 8;
				//
  				RCC_Configuration();   
  				//GPIO_Configuration(0);			//20121030 : normal(1)<==== option port reading ????
				UART_Configuration();
				//ADC_Configuration(0);		//20110719 : power_consumption(0)
				TIM1_Configuration();		//(250usec)
//20110824
#ifdef TIMER2_ENABLE
				TIM2_Configuration();
#endif
  				/* Disable SysTick interrupt */
  				//SysTick_ITConfig(DISABLE);
 				/* Disable the SysTick Counter */
  				//SysTick_CounterCmd(SysTick_Counter_Disable);
				SYSTICK_Configuration();	//(1msec)
  				//----------------------------------------------------------------
				//----------------------------------------------------			
				//----------------------------------------------------			
		    }
	    }				
        //----------------------------------------------------------------------
        //	analog <-> digital : [ * >= POWER_ON3_ING ]
        //----------------------------------------------------------------------
        ListenFunctionAutoChange();
        //----------------------------------------------------------------------
        //----------------------------------------------------------------------
        //----------------------------------------------------------------------
        //----------------------------------------------------------------------
	}
}


/*

*********************************************************************/
void 	CheckFunction_100msec(void)
{

	if (timer_Get100ms()==TRUE) {

		//----------------------------------------------------------------------
		Func_TriggerJackCheck_Power();		//12VTrigger-in check?
		//return : mode_DispBlockC <== BLOCKC_NORMAL
		Display_BlockCModeCheck_100msec();		//20050907 : display mode timer check?!
		//----------------------------------------------------------------------
		//mute off check
		Mute_CheckMuteOff_100msec();		
		//----------------------------------------------------------------------
		//20080618
		//stby led ---> on
#ifdef LED_DIMMER_ENABLE_x
		if (counter_StbyLed > 0) {
			--counter_StbyLed;
			if(counter_StbyLed == 0) {
				//20140115 : LED dimmer data reading
				LED_Dimmer_value = nvr_xGetData(ADDR_LED_DIMMER);			
				StandbyLED(1);				//stby led ---> on
			}
		}
#endif
		//----------------------------------------------------------------------      
		//FAN check
		//20140523 : add for signal sensing feature		
		FAN_check();
		//----------------------------------------------------------------------
#ifdef SIGNAL_SENSING_ENABLE
		//20140619 : for signal sensing feature :
		//counter : /100msec
		if ( (RKB_mode_PP_MP == 1)&&(RKB_mode_PP_nMP2 == 0)&&(GetSignalSensing_nSW == 0) )
		{
			//MP2 add signal sensing feature!
			if (mode_PowerStatus == POWER_ON0) {		
				//20140813	
				if (signalSensingAMPStatus == ON) {
					if (signal_sensing_counter < SIGNAL_SENSING_OFF_TIME) ++signal_sensing_counter;
				}
			}
		}
#endif
        //----------------------------------------------------------------------
	}
}
/*
unsigned char T125msec_count;
RT1570 10msec use VFD

*********************************************************************/
void	CheckFunction_125msec(void)
{
	if (timer_Get125ms()==TRUE) {           //<== now, 10msec
/*
		++T125msec_count;

		if (T125msec_count >= 2) {
			T125msec_count = 0;
		}*/
		Led_control_125msec();			//RKB LED control
		//
        //----------------------------------------------------------------------

	}
}
/*


*********************************************************************/
void	CheckFunction_250msec(void)
{
	if (timer_Get250ms()==TRUE) {

		// b_floff_mode : floff key에의한 mode
		// mode_floff   : 잠시 key input에의한 mode
		// timer_floff  : 잠시 key input에의한 timer buffer [일정시간후에 go FL off! ]
		floff_counter_250msec();	
		++b1_CommTimer_250msec;			//20090804

	}
}
/*


*********************************************************************/
void	CheckFunction_500msec(void)
{
	unsigned char i;

	if (timer_Get500ms()==TRUE) {
		//500msec flag set
		f_500msec_on = 1;						//20090819 : VFD cursor on blinking
		//---------------------------------------------------------
		// delay time volume data Tx [0 .. 30]
		//	time_delay_volume_Tx = 0;     //default(0)									
		//	nvr_xPutData(ADDR_DELAY_VOLUME_TX,time_delay_volume_Tx);
		//---------------------------------------------------------
		//---------------------------------------------------------
		//---------------------------------------------------------
	}
}

/*
* Function Name  : main
* Description    : Main program.
* Input          : None
* Output         : None
* Return         : None

UART1 : RS232(DB-9)
PB9 PB8
 0   0	: RKB-850
 0   1	: RKB-8100
 1   0	: RKB-D850
 1   1	: RKB-D8100
*********************************************************************/
void main(void)
{
	unsigned char data,dummy;

	//ini_DisableInterrupt();
	//RCC_Configuration();			//RCC_HSE_ON	//AHB = HCLK = SYSCLK	//APB2 = PCLK2 = HCLK 	//APB1 = PCLK1 = HCLK	//PLLCLK = 8MHz * 9 = 72 MHz
	//GPIO_Configuration(1);
	//UART_Configuration();
	//ADC_Configuration(0);
	//TIM1_Configuration();
	//SYSTICK_Configuration();	//1msec
	//NVIC_Configuration();
  	//----------------------------------------------------------------
  	//	mode_AHB_prescaler
  	//	1=36MHz, 2=18MHz, 4=9MHz, 8=4.5MHz
  	// 20140409
  	//----------------------------------------------------------------
	mode_AHB_prescaler = 1;  //for normal mode
	//
	ini_InitMcu();		//port,uart,adc,timer initialize

	cmd_ampType = 4;	//20140218
	//20131129 : temperature=xx,xx,xx,xx!
	TemperatureADC_Mode = TEMP_ON;
	amp_protextion_skip = 0;
	//AMP off : SMPS_off
	SMPS_Off();			//port SMPS_/ON(17)=high
	//--------------------------------------------
	//REC-mute : ON
	//PP : Low active
	//WS : High active
	#if 0
	if (RKB_mode_PP_MP == 1) {							//? PP unit
		preOut_Mute(0);
	}else{
		preOut_Mute(1);
	}
	#endif
	//--------------------------------------------
	//20140521
	//FAN-off 
	FAN_HS_mode = FAN_READY;
	FAN_LS_mode = FAN_READY;
	FAN_hs_on(0);
#ifdef SIGNAL_SENSING_ENABLE
	FAN_ls_on(0);
	//
	signalSensingAMPStatus = ON;
	//reset counter
	signal_sensing_counter = 0;
	//20140703
	signalOn_500m_timer = 0;
	singalOn_low_counter = 0;
	//20140812
	singalOn_noise_counter = 0;
	//20140804 : Off [ s/w center ]
	current_SignalSensingMode = 0;
#endif
	//
	count_powerOffDelay = 0;				//20120927
	//20140515
	opticalpower_bk = 1;
	count_opticalPower = 0;

    //power_txMode==0, status Tx
    //power_txMode==1, command Tx
	RS232_TxSkip = 0;					//20131212
    power_txMode = 0;

	count_delayVFD =TIME20MSTO500MS;

    //20140115 LED_dimming_counter
	LED_dimming_counter = 0;

	//LED mode = SKIP!
	//20140409
	set_LED_mode(LED_MODE_SKIP);
#ifdef LED_DIMMER_ENABLE
	//20140225 : timer => directly on/off
	Led_current_status = OFF;
#endif
	StandbyLED(0);

#ifdef _RKB_DM8100
    //RKB_ModelID:PE3,PE4
    RKB_ModelID = (unsigned char)( ( GPIO_ReadInputData(GPIOE) >>3 )&0x0003 );
#else
	//------------------------------------------------------
    //RKB_ModelID:PE8,PE9: [ 00=850, 01=8100, 10=D850, 11=D8100 ]
    RKB_ModelID = (unsigned char)( ( GPIO_ReadInputData(GPIOE) >>8 )&0x0003 );
#endif
	//------------------------------------------------------
    //RKB sub-ID:PC11,10 [ 11=0, 10=1, 01=2, 00=3 ]
    //20131024 deleted : add ADC2/3
    //RKB_SubID = (unsigned char)( ( GPIO_ReadInputData(GPIOC) >>10 )&0x0003 );
    //RKB sub-ID:PC11,10 [ 00=0, 01=1, 10=2, 11=3 ]
	//RKB_SubID = (~RKB_SubID)&0x03;    
	//------------------------------------------------------
	//
	//------------------------------------------------------
	uart_Init();

#ifdef UART2_ENABLE
	uart2_Init();		//
#endif
#ifdef UART3_ENABLE
	uart3_Init();		//not used
#endif
#ifdef UART4_ENABLE
	uart4_Init();		//
#endif
#ifdef UART5_ENABLE
	uart5_Init();		//not used
#endif

	adc_KeyInit();
		
	//main_SystemInit();	//20130709 : ADC disable 이니까 필요없슴..			//Trim all Min => ? 
    //flag setting : f_display_on , f_power_on
    main_Power(ON);
    //
	//protection mode clear!!!!
    mode_amp_protect = OFF;     
    Amp_protectionTime = 0;		//AMP.RDY port check : level check time
    count_delay_AMPProtectionCheck = TIME10MSTO5SEC;    //20121012

	DelayMs(200);

#ifdef UART1_DEBUG_x
	debug((_MN_ __DATE__" "__TIME__" Rotel Technology Global Inc..\n"));
#endif
//---------------------------------------------------------
//20140108
//	ASCii, Rotel-Link 에 필요한 initialize
//	
	Stingray_Startup_mode = S_CONFIGURE_NO;
	//Stingray_Startup_mode = S_CONFIGURE_OK;

	//---------------------------------------------------------
#ifdef IAP_UPGRADE
	Initial_I2C();
	main_DisableUpgrade();
#endif
	//------------------------------------------------------	
#ifdef UART1_DEBUG_x
	rs232_printf("Reset=>%x\n",data);
#endif
	//eeprom dummy :
	dummy = nvr_xGetData(ADDR_DUMMYDUMMY);	//page0 setting!!!!!!!!!
	dummy = nvr_xGetData(ADDR_DUMMYDUMMY);	//page0 setting!!!!!!!!!
	dummy = nvr_xGetData(ADDR_DUMMYDUMMY);	//page0 setting!!!!!!!!!
	data = nvr_xGetData(ADDR_RESET);			//?RESET
	//------------------------------------------------------	
	if(data == ON) goto __reset;
	//------------------------------------------------------	
	if(Check_Backup() == TRUE){
		//
		Func_PowerAcOn();
		__asm("nop");
	}else{
		__asm("nop");
		//clearMemory((void *)0x10000,0x08000);	
__reset:		
		Reload_DefaultBackup();				// mode_func, key_init
		main_PowerDownCheckSumSave();		//eeprom writing!

		nvr_xPutData(ADDR_RESET,OFF);

		Func_PowerAcOn();	
	}

#ifdef test_VFD
        //test VFD
        //VFD display
        //
		for(j=0;j<2;j++) {
			for(i=0;i<20;i++)	TDISP_TEMP_BUFF.sStingray.DispBuf[j][i] = str_Test[j][i];
		}
		Func_PowerControl();
#endif
	//==============================================================================
	/* Infinite loop */
	while (1)
	{
#ifdef IAP_UPGRADE
		rs232_CheckUpgrade();
 		if (rs232_GetUpgradeMode() == TRUE) {
  			main_iAPUpgrade();
  		} else 
#endif  		
//----------------------------------------------------------
        {
//----------------------------------------------------------
#ifdef test_VFD
            //test VFD
            //VFD display
            //
		    if(memcmp((void*)FTX_DISP_BUFF.display_Buffer,(void*)TDISP_TEMP_BUFF.display_Buffer,DISPLAYBUFFER_MAX_SIZE) != 0){
goto_FTX_reflesh:
			    memcpy((void*)FTX_DISP_BUFF.display_Buffer,(void*)TDISP_TEMP_BUFF.display_Buffer,DISPLAYBUFFER_MAX_SIZE);

			    //VFD buffer setting
			    for(i=0;i<20;i++)
			    {
					shiftDatIdx[i+20] = TDISP_TEMP_BUFF.display_Buffer[i];
					shiftDatIdx[i] = TDISP_TEMP_BUFF.display_Buffer[i+20];
			    }
		    }
		    goto main_keyAll;				//test
#endif
//----------------------------------------------------------
//----------------------------------------------------------
			// every time check
			main_PowerDownCheck();			//RKB : 20130624
			Check_DirectMuteOnOff();		//RKB : 20140224
			CheckFunction_10msec();
			CheckFunction_20msec();
			CheckFunction_100msec();
			CheckFunction_125msec();
			CheckFunction_250msec();
			CheckFunction_500msec();
//-------------------------------------------------------------------------------
			//UART4 (RCD1570) key
			//
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
			//Front,RS232
			//
			if(Key_InputCheck() == TRUE) {

			/* all input에 대한 key check 및 key처리 */                
#ifdef IC_DEBUG_x
	rs232_printf("key kind : %x\n",key_Kind);
	rs232_printf("key value: %x\n",key_Value);
#endif
//	PutString("pm=>%\n",mode_Power);
//	PutString("ps=>%\n",mode_PowerStatus);
//	PutString("MBK=>%\n",mode_func_bk);
//	PutString("M=>%\n",mode_func);
				//iPod upgrading ?
#ifdef IPOD_INCLUDE
				if (TInfo.fwUpgrade.Upgrading == iPod_ON) goto main_keySkip;	//20100426
#endif
				//-----------------------------------------------------
				//									
				switch(key_Kind){
					case KEY_NONE:
	            		break;
					case KEY_FRONT:
						//proc_KeyFront(key_Value);
						break;
					case KEY_RS232:                  
						//key_Kind = KEY_RS232;
						//key_Type check해야함....
						//key_Type : 	
						//		0x10=normal
						switch(key_Type) {
							case 0x10:
								//NORMAL
								if(Key_DirectMainKeyCheck(key_Value) == TRUE ) {		//20040127
									//20100316 : RS232 key pressed ->(5sec) -> released
									Rs232_pressed_key = key_Value;
									Rs232_pressed_count = COUNT100M_5SEC;
									proc_KeyRs232(key_Value);
								}
								break;
							default:
								//NORMAL
								//proc_KeyRs232(key_Value);
								break;
							}			
						break;
					case KEY_SPECIALKEY:
						break;			
					default:
						break;	
					}
			}
main_keySkip:
            //--------------------------------------------------------                        
            //--------------------------------------------------------                        
  			uart1_TxData();			//RS232 TX
	        if(mode_PowerStatus >= POWER_ON3_ING) 				//20121106
            {
#ifdef UART2_ENABLE
  			    uart2_TxData();			//Telechips
#endif
  			}
#ifdef UART3_ENABLE		//20120614
  			uart3_TxData();			//not used!!!!!!!!!
#endif
#ifdef UART4_ENABLE
  			uart4_TxData();			//RotelLink [57600,n,8,1]
#endif
#ifdef UART5_ENABLE
  			uart5_TxData();			//not used!!!!!!!!!
#endif
            //--------------------------------------------------------                        
main_keyAll:			
            //--------------------------------------------------------                        
#ifdef IC_DEBUG
	IC_DebugHandler();
#endif
//--------------------------------------------------------                        
        }
//--------------------------------------------------------                        
  	} 	//while()
}
/*********************************************************************/
/*
//(mode_func) :
	- if (Power off and Protection),  mode_func = FUNCTION_DISP_OFF
#define	FUNCTION_DISP_OFF			0
#define	FUNCTION_AMP_A				1
#define	FUNCTION_AMP_B				2
#define	FUNCTION_AMP_C				3
#define	FUNCTION_AMP_D				4
*/
/*********************************************************************/

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void	proc_KeyRs232(unsigned char key)
{
	switch(key){
	    
	case SK_POWER:			Func_PowerControl();		break;                      //RS232, Rotel Link 처리에서 power_txMode 를 정해야한다.	
	case SK_POWERON:		Func_SeperatePowerOn();		break;                      //RS232, Rotel Link 처리에서 power_txMode 를 정해야한다.		
	case SK_POWEROFF:		Func_SeperatePowerOff(); 	break;                      //RS232, Rotel Link 처리에서 power_txMode 를 정해야한다.		
	case SK_GET_POWER:		power_txMode=0;
	                        Func_PowerStatus();		    break;		//power_txMode==0, status Tx	

	case SK_REFRESH:		Func_RS232_TX(); 			break;		//20090819

	case SK_DISPLAY_ALL:	Func_DisplayAll();			break;      //20110304	
	//case SK_DISPLAY3:	    Func_Display1234(2);		break;      //20110316	
	//case SK_DISPLAY4:	    Func_Display1234(3);		break;      //20110316	
	case SK_PRODUCT_TYPE:	Func_ProductType();			break;      //20110304	
	case SK_PRODUCT_VERSION: Func_ProductVersion();		break;      //20110304	

	case SK_GET_FREQ:       	Func_GetCurrentFreq(cmd_ampType);		break;		//20140220
	case SK_GET_AMP_TRIM:       Func_GetCurrentTrim(cmd_ampType);		break;		//20140220
	case SK_GET_TEMPERATURE:	
								//20131129
								//RKB : 0=2,2digit , 1= 4,4digit
								//(NTC-L,NTC-R)xx,xx,xx(,xx)
								//(NTC-L,NTC-R)xxxx,xxxx,xxxx(,xxxx)
								Func_AmpTemperatureCurrentStatus(TemperatureADC_Mode);			
								break;		
	case SK_GET_FAN_STATUS:		Func_FAN_HS_Status();					break;		//RKB	//20140521
	case SK_GET_AMP_STATUS:		Func_AMPStatus();						break;		//RKB //20130708

	case SK_GET_FAN_LS_ON_TEMP:		Func_FanLsOnStatus();				break;		//RKB //20140521
	case SK_GET_FAN_LS_OFF_TEMP:	Func_FanLsOffStatus();				break;		//RKB //20140521
	case SK_GET_FAN_HS_ON_TEMP:		Func_FanHsOnStatus();				break;		//RKB //20140521
	case SK_GET_FAN_HS_OFF_TEMP:	Func_FanHsOffStatus();				break;		//RKB //20140521
	case SK_GET_SHUTDOWN_TEMP:	Func_getShutdownTemp();					break;		//RKB //20130916
	case SK_GET_SMPS_SHUTDOWN_TEMP:	Func_getSmpsShutdownTemp();			break;		//RKB //20131024

	case SK_SET_TEMPERATURE_ADC:	TemperatureADC_Mode = TEMP_ADC;		break;		//RKB //20131129
	case SK_SET_TEMPERATURE_ON:		TemperatureADC_Mode = TEMP_ON;		break;		//RKB //20131129
	case SK_FACTORY_DEFAULT_SET:
							//RKB //20131129
							//Eeprom Factory Reset!!!
							nvr_xPutData(ADDR_RESET,ON);
							DelayMs(500);
							//STM32 HW Reset!
							//STM32ResetEnable(1);
							__iar_program_start();				//20110608
							break;		
	case SK_AMP_PROTECTION_OFF:	amp_protextion_skip = 1;		break;		//RKB //20131129
	case SK_AMP_PROTECTION_ON:	amp_protextion_skip = 0;		break;		//RKB //20131129

	case SK_DISP_UPDATE:    Func_DisplayUpdateStatus(); break;      //20131212
	case SK_DISP_AUTO:      Func_SetDisplayUpdate(0);   break;      //20131212
	case SK_DISP_MANUAL:    Func_SetDisplayUpdate(1);   break;      //20131212
		
	case SK_GET_DELAYTIME_VOLUME:    Func_getDelayTimeVolumeDataTx();	break;      //20131218
	//case SK_SET_DELAYTIME_VOLUME:    Func_SetDelayTimeVolumeDataTx(0);	break;      //20131218
	//----------------------------------------------------------------------------------------------
	//20140213 : s/w RS232 commands add :
	//		
	case SK_MUTE:					Func_Mute(cmd_ampType);				break;		//RKB 20140217 done
	case SK_MUTE_ON:				Func_MuteOn(cmd_ampType);			break;		//RKB 20140217 done	
	case SK_MUTE_OFF:				Func_MuteOff(cmd_ampType);			break;		//RKB 20140217 done
		
	case SK_GET_AMPCH_STATUS:		Func_AMP_CH_status(cmd_ampType);	break;		//RKB 20140217 done

	case SK_AMPCH_ON:				Func_AMP_CH_on(cmd_ampType);		break;
	case SK_AMPCH_OFF:				Func_AMP_CH_off(cmd_ampType);		break;

	case SK_GET_FRONT_TRIM_STATUS:	Func_frontTrimStatus();				break;		//RKB 20140217 done
	case SK_FRONT_TRIM_ENABLE:
								#ifdef RS232_VOL_ENABLE_IS_TRIMMIN		//used
								#else
									Func_frontTrim_enable();	//RKB 20140304 : deleted
								#endif		
							break;		
	case SK_FRONT_TRIM_DISABLE:		
								#ifdef RS232_VOL_ENABLE_IS_TRIMMIN		//used
								#else
									Func_frontTrim_disable();	//RKB 20140304 : deleted			
								#endif		
							break;

	case SK_GET_VOLUME_MAX:			Func_GetVolumeMax();	    		break;		//RKB 20140217 done
	case SK_GET_VOLUME_MIN:			Func_GetVolumeMin();	    		break;		//RKB 20140217 done	
	case SK_GET_VOLUME:
									Func_VolumeLRStatus(cmd_ampType);				//RKB 20140404 done					
									//Func_VolumeLchStatus(cmd_ampType);			//RKB 20140217 done					
									//Func_VolumeRchStatus(cmd_ampType);			//RKB 20140217 done	
									break;	
	case SK_GET_VOLUME_L:			Func_VolumeLchStatus(cmd_ampType);		break;	//RKB 20140217 done	
	case SK_GET_VOLUME_R:			Func_VolumeRchStatus(cmd_ampType);		break;	//RKB 20140217 done	
	case SK_GET_BALANCE:			Func_GetCurrentBalance(cmd_ampType);   	break;	//RKB 20140217 done
		
	case SK_GET_LED_DIMMER:			Func_ledDimmerStatus();   				break;	//RKB 20140225 done		
		
	case SK_VOL_UP:					Func_VolUp(cmd_ampType);   				break;	//RKB 20140304
	case SK_VOL_DN:					Func_VolDown(cmd_ampType);   			break;	//RKB 20140304
	case SK_VOL_L_UP:				Func_VolLeftChUp(cmd_ampType);   		break;	//RKB 20140304
	case SK_VOL_L_DN:				Func_VolLeftChDown(cmd_ampType);   		break;	//RKB 20140304
	case SK_VOL_R_UP:				Func_VolRightChUp(cmd_ampType);   		break;	//RKB 20140304
	case SK_VOL_R_DN:				Func_VolRightChDown(cmd_ampType);   	break;	//RKB 20140304

	case SK_GET_SMPS_FAN_ON_TEMP:	Func_SMPSFanOnStatus();						break;		//RKB //20140311
	case SK_GET_SMPS_FAN_OFF_TEMP:	Func_SMPSFanOffStatus();					break;		//RKB //20140311

	case SK_BALANCE_RIGHT:			Func_BalanceRight(cmd_ampType);        	break;		//RKB //20140408
	case SK_BALANCE_LEFT:			Func_BalanceLeft(cmd_ampType);         	break;		//RKB //20140408
	//----------------------------------------------------------------
	//20140515 : digital model  only
	case SK_GET_OPTI_POWER_MODE:	
								#ifdef OPTICAL_POWER_ENABLE
									Func_opticalPowerModeStatus();				
								#else
								#endif		
							break;
		
	case SK_OPTI_POWER_ENABLE:
								#ifdef OPTICAL_POWER_ENABLE
									Func_opticalPower_enable();
								#else
								#endif		
							break;		
	case SK_OPTI_POWER_DISABLE:		
								#ifdef OPTICAL_POWER_ENABLE
									Func_opticalPower_disable();			
								#else
								#endif		
							break;
	//----------------------------------------------------------------
	//----------------------------------------------------------------
	case SK_GET_INPUT_SEL_MODE:		Func_inputSelStatus(cmd_ampType);         	break;		//RKB //20150302
	case SK_INPUT_SEL_AUTO:			Func_inputSel_auto(cmd_ampType);         	break;		//RKB //20150302
	case SK_INPUT_SEL_DIGITAL:		Func_inputSel_digital(cmd_ampType);         break;		//RKB //20150302
	case SK_INPUT_SEL_ANALOG:		Func_inputSel_analog(cmd_ampType);         	break;		//RKB //20150302
	//----------------------------------------------------------------
	default:															break;
	}
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// 20010831 : floff counter check [/250msec]
// b_floff_mode : floff key에의한 mode
// mode_floff   : 잠시 key input에의한 mode
// timer_floff  : 잠시 key input에의한 timer buffer [일정시간후에 go FL off! ]
//-----------------------------------------------------------------------------
void floff_counter_250msec(void)
{
	switch(mode_PowerStatus) {
		case POWER_ON0:
			if(b_floff_mode == 1) {				// 1 = FL-OFF mode!
				if(timer_floff != 0) {
					--timer_floff;
					if(timer_floff == 0) {
						mode_floff = 1;			// FL off! command!
						timer_floff = 0;			// counter clear
					}
				}
			}else{
				timer_floff = 0;
			}
			break;
		default:
			break;
	}
}

//-----------------------------------------------------------------------------
//	RS232 enable power key check
//-----------------------------------------------------------------------------
//	RS232 enable power key check
const unsigned char tbl_EnableKeyForPowerOff[13] = { 
						SK_POWERON,SK_POWER,SK_POWEROFF,SK_GET_POWER,
						SK_PRODUCT_TYPE,SK_PRODUCT_VERSION,
						SK_GET_TEMPERATURE,SK_GET_FAN_STATUS,
						SK_FACTORY_DEFAULT_SET,
						SK_GET_AMP_STATUS,
						SK_GET_AMPCH_STATUS,SK_AMPCH_ON,SK_AMPCH_OFF
						};
//----------------------------------------------------------						
unsigned char	Check_ActiveKeyForPowerOff(unsigned char key)	//20140219
{
	unsigned char i,result;
	
	result = FALSE;
	for(i=0;i<13;i++) {
		if(key == tbl_EnableKeyForPowerOff[i]) {
			result = TRUE;
		}
	}
	return(result);
}
//---------------------------------------------------------
//RKB
unsigned char Key_DirectMainKeyCheck(unsigned char key)
{
	if(Power_GetPowerStatus() == 0) {			//?power off(0)
		// Main power off상태에서 enable되는 키를 선택.[function, power]
		if(Check_ActiveKeyForPowerOff(key) == TRUE) {
			return(TRUE);
		}else{
			return(FALSE);
		}
	}else{
		return(TRUE);
	}
}



//-----------------------------------------------------------------------------
//	front key check
//	=> ADC0/1/2/3 key => key_Value => volume Tx
//	=> ACD4/5 key => key_Value => TEMP compare ==> Protection and Temp Tx
//
//	key_target[i] <= key_data[i]
// 	current_key[i] : key_target[i]
//
//volatile byte amp_on_off[4];
//volatile byte volume_Lch[4];
//volatile byte volume_Rch[4];
//volatile byte balance[4];
//volatile byte ch_muteOn[4];
//unsigned char ch_muteOn_volValue[4];
//volatile byte front_trimKnob_enable;
//-----------------------------------------------------------------------------
//---------------------------------------------------------
//RKB
unsigned char func_volume2dB_cha(unsigned char tgt, unsigned char curr)
{
	if ( (tgt > 96) || (curr > 96) ) return(FALSE);
	//
	if (tgt >= curr) {
		if ( (tgt - curr) >= 2 ) {
			return(TRUE);
		}else{
			return(FALSE);
		}
	}else{
		if ( (curr - tgt) >= 2 ) {
			return(TRUE);
		}else{
			return(FALSE);
		}
	}
}
//---------------------------------------------------------
//---------------------------------------------------------
//RKB
unsigned char Key_TactCheck(void)
{
	unsigned char result,i,j;
	char step;

	result = FALSE;
	
	//-------------------------------------------------------------
	// key Trim 
	//	key.KeyData[0]~[3] : 0<->96
	//-------------------------------------------------------------
	//power on : protection : mute status : check
    if ( (mode_amp_protect == OFF)&&(mute_mode == MUTE_OFF)&&(f_MainMute == 0) ) {      //20130618
    	///////////////////////////////////////////////////////////////
    	//20131204 : power on 후 2sec 동안은 20msec 주기로 변경할 수 있도록 추가!
    	//20131204 : delete!!!
    	///////////////////////////////////////////////////////////////
		//------------------------------------------------------------------
		//20140218
#ifndef _RKB_DM8100
#ifdef RS232_VOL_ENABLE_IS_TRIMMIN			//used
			//? All TRIM min(0)
			j=0;
			for (i=0;i<4;i++) if (key.KeyData[i] == 0) ++j;
			if (j == 4) {
				//All TRIM = min(0)!!! 
				if (front_trimKnob_enable == ON) {
					//
					//front knob disable!
					//
					front_trimKnob_enable = OFF;
					//status Tx!
					Func_frontTrimStatus();
					//--------------------------------------
					//	1st : balance All CH , if ( Lch != Rch ) , and then default setting
					//	2nd : volume All CH re-setting!
					//			RS232 volume data load and Tx?
					//--------------------------------------
					//
					// balance All CH , if ( Lch != Rch ) , and then default setting
					// 20140305
					for (i=0;i<4;i++) {
						if ( volume_Lch[i] != volume_Rch[i] ) {
			    			balance[i] = BALANCE_VOLUME_INITIAL;	//15
							eepromAddr = ADDR_A_BAL+i;					//20140224
							nvr_xPutData(eepromAddr,balance[i]);
						}
					}
					// volume All CH re-setting
					//RS232 L/R-ch volume data Tx
					//20140305
					for (i=0;i<4;i++) {
						if (amp_on_off[i] == 1) {
							//mute off ->volume data Tx
							//20140305 : if same Lch & Rch and then have to balance calculation
							if ( volume_Lch[i] != volume_Rch[i] ) {
                    			Set_MainVolumeLeft((unsigned char)volume_Lch[i],(i));
                    			Set_MainVolumeRight((unsigned char)volume_Rch[i],(i));
                    		}else{
                    			//(Lch = Rch) calculate balance value!
                    			//20140305
								Set_MainVolume(volume_Lch[i],i);
                    		}
							//rs232_printf("ASCiiKey=>%x\n",cmd_code);
						}else{
							//mute
							Set_MainVolume(0,i);
						}
						Func_VolumeLchStatus((i));
						Func_VolumeRchStatus((i));
					}
				}
			}else{
				//All TRIM <> min(0)!!! 
				if (front_trimKnob_enable == OFF) {
					//
					//front knob enable!
					//
					front_trimKnob_enable = ON;
					//status Tx!
					Func_frontTrimStatus();
					//--------------------------------------
					//	volume All CH re-setting!
					//			RS232 volume data load and Tx?
					//--------------------------------------
					//
					//	volume ADC read -> TX
					//
					//20140401
					//
					for (i=0;i<4;i++) {
			            //Set_MainVolume(vol,ampType) => L/R Tx
			            key_target[i] = 0;
			            current_key[i] = 0;
						Set_MainVolume(current_key[i],i);
						//
						//20131204 : power on 2sec 동안은 skip
						//
						//RS232 Tx
						Func_AmpTrimCurrentStatus(i);
					}
				}
			}	
#endif
		//------------------------------------------------------------------
		//20140218 : 
		if (front_trimKnob_enable == ON)
		{
		//------------------------------------------------------------------
			for (i=0;i<4;i++) {
				//20140226 : add compare : amp_on_off[i] , and then ch_muteOn[i]=OFF
				if ( (amp_on_off[i] == ON)&&(key.KeyOk[i] == ON) ) {
					//key.KeyOk[i] = OFF;
					key_target[i] = key.KeyData[i];
#ifdef UART1_DEBUG_x
					//debug((_MN_"ADC1 key data is [%d]\n",key.KeyData[i]));
					debug((_MN_"ADC";%d ;"key data is [%d]\n",i, key.KeyData[i]));
#endif
					//--------------------------------------------------------------------
					if ( current_key[i] == key_target[i] ) {
						// key target 까지 도달해야 만 하니까 , 도달해서 key off!!!!
						key.KeyOk[i] = OFF;
					}else{										
						//20140226 : 
						//when now mute_on,
						//if( | key_target[i] - ch_muteOn_volValue[i] | >= 2) , mute off
						if (ch_muteOn[i] == MUTE_ON) {
							if (func_volume2dB_cha(key_target[i], ch_muteOn_volValue[i] ) == OFF) break;
							ch_muteOn[i]= MUTE_OFF;
						}						
						//test : 20140226
						//rs232_printf("AD key(current,target,read)=>%d, %d, %d\n",current_key[i], key_target[i],key.KeyData[i] );
						//--------------------------------------------------------------------
						//? current_key[i] == key_target[i]
						if ( current_key[i] < key_target[i]	) {
							//(current_key) < (key_target)
							//goto vol up!
							step = key_target[i] - current_key[i];
							//
							//20131204 : power on 2sec 동안은 1dB 씩 변경으로 수정.
							//
							if (step > 1) {
								step = step >> 1;	//DIV /2
								//step adjust
								if ( step > 8 ) step = 4;
								else if ( step > 6 ) step = 3;
								step = 2;	
								//key.KeyOk[i] = ON;
							}
							//save
							current_key[i] += step;
			            	//Set_MainVolume(vol,ampType) => L/R Tx
							Set_MainVolume(current_key[i],i);
							//
							//20131204 : power on 2sec 동안은 skip
							//
							//RS232 Tx
							Func_AmpTrimCurrentStatus(i);
							//20130620
							mode_func = FUNCTION_AMP_A+i;			//(FUNCTION_AMP_A = 1)+0/1/2/3
						//--------------------------------------------------------------------
						}else if ( current_key[i] > key_target[i] ) {
						//--------------------------------------------------------------------
							//(current_key) > (key_target)
							//goto vol down!
							step = current_key[i] - key_target[i];
							//
							//20131204 : power on 2sec 동안은 1dB 씩 변경으로 수정.
							//
							if (step > 1) {
								step = step >> 1;	//DIV /2
								//step adjust
								if ( step > 8 ) step = 4;
								else if ( step > 6 ) step = 3;
								step = 2;	
								//key.KeyOk[i] = ON;
							}
							//save
							current_key[i] -= step;
			        		//Set_MainVolume(vol,ampType) => L/R Tx
							Set_MainVolume(current_key[i],i);
							//
							//RS232 Tx
							Func_AmpTrimCurrentStatus(i);
							//20130620
							mode_func = FUNCTION_AMP_A+i;			//(FUNCTION_AMP_A = 1)+0/1/2/3
						}
					}
					//--------------------------------------------------------------------
				}
			}	//for
		}else{
			//------------------------------------------------------------------
			//front key knob disable , so "get_amp_trim!" command support!
			//------------------------------------------------------------------
			for (i=0;i<4;i++) {
				if (key.KeyOk[i] == ON) {
					key.KeyOk[i] = OFF;
					key_target[i] = key.KeyData[i];
					//current_key[i] = key_target[i];
				}
			}	
		}
	}
#endif //_RKB_DM8100
goto_tempKey_exec:	
	//-------------------------------------------------------------
	// key temperature : add 2ch : 20131024
	//	key.KeyData[4],[5],[6],[7]
	//-------------------------------------------------------------
    if (count_delay_AMPProtectionCheck == 0) {		//20130708 : after power on, 5sec
    //
	for (i=4;i<8;i++) {
		if (key.KeyOk[i] == ON) {
			key.KeyOk[i] = OFF;
			key_target[i] = key.KeyData[i];
			//? current_key[i] != key_target[i]
			if ( current_key[i] != key_target[i] ) {
				//(current_key) <= (key_target)
				current_key[i] = key_target[i];				
				//Tx current Temp...
				//
				//typ = 0 : current_key[] : xx,xx
				//typ = 1 : current_ADC_Val[] : xxxx,xxxx
				//20130708 : temp 변경시 둘다 변한 경우에는 한번에 Tx 하려고 합니다.
				if ( (i == 4)&&( (key.KeyOk[5] == ON)||(key.KeyOk[6] == ON)||(key.KeyOk[7] == ON) ) ) {
				}else if ( (i == 5)&&( (key.KeyOk[6] == ON)||(key.KeyOk[7] == ON) ) ) {
				}else if ( (i == 6)&&( (key.KeyOk[7] == ON) ) ) {
				}else{
					//20131129
					//RKB : 0=2,2digit , 1= 4,4digit
					//(NTC-L,NTC-R)xx,xx,xx(,xx)
					//(NTC-L,NTC-R)xxxx,xxxx,xxxx(,xxxx)
					//20140403 : display_updatae=manual 하에서도 get_temperature! 는 진행해야한다. but auto temp. data 는 manual(1) 일때 skip!!!
					RS232_TxSkip = mode_DisplayUpdate;		//20140403
					Func_AmpTemperatureCurrentStatus(TemperatureADC_Mode);			
				}			
				//20131024
				//mode_func = FUNCTION_AMP_A+i;			//(FUNCTION_AMP_A = 1)+0/1/2/3
			}
#ifdef UART1_DEBUG_x
			rs232_printf("temp key =>%d %d\n",i, current_key[i]);
			//rs232_printf("temp key =>%d %d\n",i, key.KeyData[i]));
#endif
		}	
	}	//for
	}
	//-------------------------------------------------------------
		__asm(" nop" );	
	//-------------------------------------------------------------
	return(result);
}

/***********************************************************
//	uart1 RS232 key
//
//
// rs232 key buffer
volatile byte rs232_key_buffer[38]; //20130520
volatile byte rs232_key_mode;
volatile byte rs232_key_count;
***********************************************************/
void	Key_Init(void)
{
	key_Value = 0x7f;
}
//----------------------------------------------------------
#define RS232_KEY_READY		0
#define RS232_KEY_DATA		1
#define RS232_KEY_CHECKSUM	2
#define RS232_KEY_CHECKSUM1	3
//----------------------------------------------------------
void 	rs232_Key_Init(void)
{
	unsigned char i;
	
	rs232_key_count = 0;
	rs232_key_mode = RS232_KEY_READY;
	for(i=0;i<35;i++) rs232_key_buffer[i] = 0;
}
/////////////////////////////////////////////////
//  ASCii Key
//  
//
//
//////////////////////////////////////////////////
unsigned char Check_Rs232_key(void)
{
	unsigned char i,j,k;
	unsigned char size;

	char comand[35]; 
	int cmd_code;
	char cmd_type;

        //------------------------------------------------------
        // load comand <= rs232_key_buffer
        //------------------------------------------------------
        for (i=0;(i<35)&&(rs232_key_buffer[i]!='\0');i++) {
            comand[i] = rs232_key_buffer[i];
        }
        comand[i]=0x00;     //null(0x00)
        //------------------------------------------------------
        //RKB : 
        //0A:get_volume!  cmd_ampType=0
        //0B:get_volume!  cmd_ampType=1
        //0C:get_volume!  cmd_ampType=2
        //0D:get_volume!  cmd_ampType=3
        //get_volume!     cmd_ampType=4
        //------------------------------------------------------
        // AMP type
        //------------------------------------------------------
		cmd_ampType = 4;
		if ( (comand[0] =='0') && (comand[1] =='A') && (comand[2] ==':') ) {
			//check "0A:" + RotelOpaqueData(m);
			for (i=0;i<32&&(comand[i]!='\0');i++) {
				comand[i] = comand[i+3];
			}
			cmd_ampType = 0;
			goto next_step_0;
		}else if ( (comand[0] =='0') && (comand[1] =='B') && (comand[2] ==':') ) {
            //check "0B:" + RotelOpaqueData(m);
			for (i=0;i<32&&(comand[i]!='\0');i++) {
				comand[i] = comand[i+3];
			}
			cmd_ampType = 1;
			goto next_step_0;
		}else if ( (comand[0] =='0') && (comand[1] =='C') && (comand[2] ==':') ) {
            //check "0C:" + RotelOpaqueData(m);
			for (i=0;i<32&&(comand[i]!='\0');i++) {
				comand[i] = comand[i+3];
			}
			cmd_ampType = 2;
			goto next_step_0;
		}else if ( (comand[0] =='0') && (comand[1] =='D') && (comand[2] ==':') ) {
            //check "0D:" + RotelOpaqueData(m);
			for (i=0;i<32&&(comand[i]!='\0');i++) {
				comand[i] = comand[i+3];
			}
			cmd_ampType = 3;
			goto next_step_0;
		//--------------------------------------------------------------------------------	
		}else if ( (comand[0] =='0') && (comand[1] =='0') && (comand[2] ==':') ) {
            //check "00:" + RotelOpaqueData(m);
			for (i=0;i<32&&(comand[i]!='\0');i++) {
				comand[i] = comand[i+3];
			}
			cmd_ampType = 4;
			goto next_step_0;
		}
        //------------------------------------------------------
next_step_0:
        //------------------------------------------------------
    	//	now RS232
        //------------------------------------------------------
        //------------------------------------------------------
    	//	1st step : command!
        //------------------------------------------------------
		// in "command"
		cmd_code = getCmdID(comand,&cmd_type);
		// out "command"
		// out cmd_type
		// Is "command" : >= 0
		// Is not "command" : -1
		if (cmd_code < 0) {
			//key error!
			//rs232_printf("ASCiiKey error!",blank);
			return(FALSE);
		}else{
        	//for (i=0,size=0; (i<32)&&(comand[i]!='\0'); i++,size++){ }			//MAX=32
            //20140212
			//---------------------------------------------------------
			//volume_#!
			//volume_##!
    		//  type=0x30
			//---------------------------------------------------------
			//?direct volume L/R [from RS232]
			//volatile byte volume_Lch[4];
			//volatile byte volume_Rch[4];
			//volatile byte balance[4];
			//volatile byte ch_muteOn[4];
			//volatile byte front_trimKnob_enable;
			//20140523 : for singal sensing feature, skip
			//---------------------------------------------------------
			if (cmd_type == 0x30) {
				//? PP & MP2 unit & signal sensing nSW
				//20140819 deleted
				//if ( (RKB_mode_PP_MP == 1)&&(RKB_mode_PP_nMP2 == 0)&&(GetSignalSensing_nSW == 0) ) return(FALSE);			
				//? Max
				if (cmd_code > MAX_TRIM_VOLUME) return(FALSE);
			    //? power on > current CH on >     												
		        if(Power_GetPowerStatus() == 0) {				//?power off(0), skip
		        	//
		        }else{
		        	if (cmd_ampType > 3 ) {					//?cmd_ampType > 3, All ch
						if (front_trimKnob_enable == OFF) {
							for(i=0;i<4;i++){
								if (amp_on_off[i] == 1 ) {		//?amp_on_off[]==on
									//save All L_ch=R_ch=cmd_code
									volume_Lch[i] = cmd_code;
									eepromAddr = ADDR_A_VOL_LCH+i;					//20140224
									nvr_xPutData(eepromAddr,volume_Lch[i]);										
									volume_Rch[i] = cmd_code;
									eepromAddr = ADDR_A_VOL_RCH+i;					//20140224
									nvr_xPutData(eepromAddr,volume_Rch[i]);
									//Tx L/R ch									
                        			Set_MainVolume((unsigned char)cmd_code,(i));      //=>RKB_function.c
			            			//rs232_printf("ASCiiKey=>%x\n",cmd_code);
			            			Func_VolumeLchStatus((i));
			            			Func_VolumeRchStatus((i));
			            		}
							}
						}		        	
					}else{		        	
						//===>DirectTx==>RKB CH
						if (front_trimKnob_enable == OFF) {
		                	if (amp_on_off[cmd_ampType] == 1 ) {	//?amp_on_off[]==on
								//save L_ch=R_ch=cmd_code
								volume_Lch[cmd_ampType] = cmd_code;
									eepromAddr = ADDR_A_VOL_LCH+cmd_ampType;					//20140224
								nvr_xPutData(eepromAddr,volume_Lch[cmd_ampType]);										
								volume_Rch[cmd_ampType] = cmd_code;
									eepromAddr = ADDR_A_VOL_RCH+cmd_ampType;					//20140224
								nvr_xPutData(eepromAddr,volume_Rch[cmd_ampType]);
								//Tx L/R ch								
                            	Set_MainVolume((unsigned char)cmd_code,(cmd_ampType));      //=>RKB_function.c
			                	//rs232_printf("ASCiiKey=>%x\n",cmd_code);
			                	Func_VolumeLchStatus((cmd_ampType));
			                	Func_VolumeRchStatus((cmd_ampType));
                            }
                        }
                    }
			    }
                return(FALSE);		    
				//return(TRUE);		    
			}	    
			//---------------------------------------------------------
			//volume_l_#!
			//volume_l_##!
    		//  type=0x31
			//---------------------------------------------------------
			//?direct volume L [from RS232] 
			//	=> balance = cener
			//20140523 : for singal sensing feature, skip
			//---------------------------------------------------------
			else if (cmd_type == 0x31) {
				//? PP & MP2 unit & signal sensing nSW
				//20140819
				//if ( (RKB_mode_PP_MP == 1)&&(RKB_mode_PP_nMP2 == 0)&&(GetSignalSensing_nSW == 0) ) return(FALSE);			
				//? Max
				if (cmd_code > MAX_TRIM_VOLUME) return(FALSE);
			    //? power on > current CH on >     												
		        if(Power_GetPowerStatus() == 0) {				//?power off(0), skip
		        	//
		        }else{
		        	if (cmd_ampType > 3) {					//?cmd_ampType >3, All ch
						if (front_trimKnob_enable == OFF) {
							//
							for(i=0;i<4;i++){
								if (amp_on_off[i] == 1 ) {		//?amp_on_off[]==on
									//save L_ch=cmd_code => (balance=initial)
									volume_Lch[i] = cmd_code;
									eepromAddr = ADDR_A_VOL_LCH+i;					//20140224
									nvr_xPutData(eepromAddr,volume_Lch[i]);										
									//volume_Rch[i] = cmd_code;
									//eepromAddr = ADDR_A_VOL_RCH+i;					//20140224
									//nvr_xPutData(eepromAddr,volume_Rch[i]);
									//20140305
									//20140305 : if same Lch & Rch and then have to balance calculation
									if ( volume_Lch[i] != volume_Rch[i] ) {
                        				//? if balance != center, Rch Tx
                        				if (balance[i] != BALANCE_VOLUME_INITIAL) {
											//reset balance value
                        					balance[i] = BALANCE_VOLUME_INITIAL;
											eepromAddr = ADDR_A_BAL+i;					//20140224
											nvr_xPutData(eepromAddr,balance[i]);
											//Tx L/R ch									
                        					Set_MainVolumeLeft((unsigned char)volume_Lch[i],(i));
                        					Set_MainVolumeRight((unsigned char)volume_Rch[i],(i));
                        					//Tx Status
			            					Func_VolumeLchStatus((i));
			            					Func_VolumeRchStatus((i));
                        				}else{
											//Tx Lch									
                        					Set_MainVolumeLeft((unsigned char)volume_Lch[i],(i));
                        					//Tx Status
			            					Func_VolumeLchStatus((i));
			            					//Func_VolumeRchStatus((i));
                        				}
                        			}else{
										//Tx L/R ch								
                            			Set_MainVolume((unsigned char)cmd_code,(i));
                        				//Tx Status
			            				Func_VolumeLchStatus((i));
			            				//Func_VolumeRchStatus((i));
                        			}
			            		}
							}
						}
					}else{
			            //===>DirectTx==>RKB CH
						if (front_trimKnob_enable == OFF) {
							if (amp_on_off[cmd_ampType] == 1 ) {	//?amp_on_off[]==on
								//save L_ch=cmd_code
								volume_Lch[cmd_ampType] = cmd_code;
								eepromAddr = ADDR_A_VOL_LCH+cmd_ampType;					//20140224
								nvr_xPutData(eepromAddr,volume_Lch[cmd_ampType]);										
								//volume_Rch[cmd_ampType] = cmd_code;
								//	eepromAddr = ADDR_A_VOL_RCH+cmd_ampType;					//20140224
								//nvr_xPutData(eepromAddr,volume_Rch[cmd_ampType]);
								//20140305
								//20140305 : if same Lch & Rch and then have to balance calculation
								if ( volume_Lch[cmd_ampType] != volume_Rch[cmd_ampType] ) {
                        			//? if balance != center, Rch Tx
                        			if (balance[cmd_ampType] != BALANCE_VOLUME_INITIAL) {
										//reset balance value
                        				balance[cmd_ampType] = BALANCE_VOLUME_INITIAL;
										eepromAddr = ADDR_A_BAL+cmd_ampType;					//20140224
										nvr_xPutData(eepromAddr,balance[cmd_ampType]);
										//Tx L/R ch									
                        				Set_MainVolumeLeft((unsigned char)volume_Lch[cmd_ampType],(cmd_ampType));
                        				Set_MainVolumeRight((unsigned char)volume_Rch[cmd_ampType],(cmd_ampType));
                        				//Tx Status
			            				Func_VolumeLchStatus((cmd_ampType));
			            				Func_VolumeRchStatus((cmd_ampType));
                        			}else{
										//Tx Lch									
                        				Set_MainVolumeLeft((unsigned char)volume_Lch[cmd_ampType],(cmd_ampType));
                        				//Tx Status
			            				Func_VolumeLchStatus((cmd_ampType));
			            				//Func_VolumeRchStatus((cmd_ampType));
                        			}
                        		}else{
									//Tx L/R ch								
                            		Set_MainVolume((unsigned char)cmd_code,(cmd_ampType));
                        			//Tx Status
			            			Func_VolumeLchStatus((cmd_ampType));
			            			//Func_VolumeRchStatus((cmd_ampType));
                        		}
			            	}
                        }
                    }
			    }
                return(FALSE);		    
				//return(TRUE);		    
			}	    
			//---------------------------------------------------------
			//volume_r_#!
			//volume_r_##!
    		//  type=0x32
			//---------------------------------------------------------
			//?direct volume R [from RS232]
			//20140523 : for singal sensing feature, skip
			//---------------------------------------------------------
			else if (cmd_type == 0x32) {
				//? PP & MP2 unit & signal sensing nSW
				//20140819
				//if ( (RKB_mode_PP_MP == 1)&&(RKB_mode_PP_nMP2 == 0)&&(GetSignalSensing_nSW == 0) ) return(FALSE);			
				//? Max
				if (cmd_code > MAX_TRIM_VOLUME) return(FALSE);
			    //? power on > current CH on >     												
		        if(Power_GetPowerStatus() == 0) {				//?power off(0), skip
					//		        	
		        }else{
		        	if (cmd_ampType > 3) {						//?cmd_ampType >3, All ch
						if (front_trimKnob_enable == OFF) {
							//
							for(i=0;i<4;i++){
								if (amp_on_off[i] == 1 ) {		//?amp_on_off[]==on
									//save R_ch=cmd_code => (balance=initial)
									//volume_Lch[i] = cmd_code;
									//eepromAddr = ADDR_A_VOL_LCH+i;					//20140224
									//nvr_xPutData(eepromAddr,volume_Lch[i]);										
									volume_Rch[i] = cmd_code;
									eepromAddr = ADDR_A_VOL_RCH+i;					//20140224
									nvr_xPutData(eepromAddr,volume_Rch[i]);										
									//20140305
									//20140305 : if same Lch & Rch and then have to balance calculation
									if ( volume_Lch[i] != volume_Rch[i] ) {
                        				//? if balance != center, Rch Tx
                        				if (balance[i] != BALANCE_VOLUME_INITIAL) {
											//reset balance value
                        					balance[i] = BALANCE_VOLUME_INITIAL;
											eepromAddr = ADDR_A_BAL+i;					//20140224
											nvr_xPutData(eepromAddr,balance[i]);
											//Tx L/R ch									
                        					Set_MainVolumeLeft((unsigned char)volume_Lch[i],(i));
                        					Set_MainVolumeRight((unsigned char)volume_Rch[i],(i));
                        					//Tx Status
			            					Func_VolumeLchStatus((i));
			            					Func_VolumeRchStatus((i));
                        				}else{
											//Tx Rch									
                        					Set_MainVolumeRight((unsigned char)volume_Rch[i],(i));
                        					//Tx Status
			            					//Func_VolumeLchStatus((i));
			            					Func_VolumeRchStatus((i));
                        				}
                        			}else{
										//Tx L/R ch								
                            			Set_MainVolume((unsigned char)cmd_code,(i));
                        				//Tx Status
			            				//Func_VolumeLchStatus((i));
			            				Func_VolumeRchStatus((i));
                        			}
			            		}
							}
						}
					//		        	
		        	}else{
			        	//===>DirectTx==>RKB
						if (front_trimKnob_enable == OFF) {
		        			if (amp_on_off[cmd_ampType] == 1 ) {	//?amp_on_off[]==on
								//save R_ch=cmd_code
								//volume_Lch[cmd_ampType] = cmd_code;
								//	eepromAddr = ADDR_A_VOL_LCH+cmd_ampType;					//20140224
								//nvr_xPutData(eepromAddr,volume_Lch[cmd_ampType]);										
								volume_Rch[cmd_ampType] = cmd_code;
								eepromAddr = ADDR_A_VOL_RCH+cmd_ampType;					//20140224
								nvr_xPutData(eepromAddr,volume_Rch[cmd_ampType]);
								//20140305
								//20140305 : if same Lch & Rch and then have to balance calculation
								if ( volume_Lch[cmd_ampType] != volume_Rch[cmd_ampType] ) {
                        			//? if balance != center, Rch Tx
                        			if (balance[cmd_ampType] != BALANCE_VOLUME_INITIAL) {
										//reset balance value
                        				balance[cmd_ampType] = BALANCE_VOLUME_INITIAL;
										eepromAddr = ADDR_A_BAL+cmd_ampType;					//20140224
										nvr_xPutData(eepromAddr,balance[cmd_ampType]);
										//Tx L/R ch									
                        				Set_MainVolumeLeft((unsigned char)volume_Lch[cmd_ampType],(cmd_ampType));
                        				Set_MainVolumeRight((unsigned char)volume_Rch[cmd_ampType],(cmd_ampType));
                        				//Tx Status
			            				Func_VolumeLchStatus((cmd_ampType));
			            				Func_VolumeRchStatus((cmd_ampType));
                        			}else{
										//Tx Rch									
                        				Set_MainVolumeRight((unsigned char)volume_Rch[cmd_ampType],(cmd_ampType));
                        				//Tx Status
			            				//Func_VolumeLchStatus((cmd_ampType));
			            				Func_VolumeRchStatus((cmd_ampType));
                        			}
                        		}else{
									//Tx L/R ch								
                            		Set_MainVolume((unsigned char)cmd_code,(cmd_ampType));
                        			//Tx Status
			            			//Func_VolumeLchStatus((cmd_ampType));
			            			Func_VolumeRchStatus((cmd_ampType));
                        		}
			        		}
			        	}
                    }
			    }
                return(FALSE);		    
				//return(TRUE);		    
			}	    
#ifdef LED_DIMMER_ENABLE
			//20140225
			//---------------------------------------------------------
			//?led dimmer = [ 0..9 ] [from RS232]
			//---------------------------------------------------------
			else if (cmd_type == 0x37) {
				//? [0..9] => [1..10]
				Func_LED_Dimmer_set(cmd_code+1);
                return(FALSE);		    
				//return(TRUE);	
			}
#endif
            //20140521
			//---------------------------------------------------------
			//?direct fan ls on
			//---------------------------------------------------------
			else if (cmd_type == 0x38) {
			    //? power on     												
		        if(Power_GetPowerStatus() == 0) {			//?power off(0)
		        }else{
			        //
                    proc_KeyRs232_FanLsOnSet((unsigned char)cmd_code);
			        //rs232_printf("ASCiiKey=>%x\n",cmd_code);
			    }
                return(FALSE);		    
				//return(TRUE);	
			}	    
			//---------------------------------------------------------
			//?direct fan ls off
			//---------------------------------------------------------
			else if (cmd_type == 0x39) {
			    //? power on     												
		        if(Power_GetPowerStatus() == 0) {			//?power off(0)
		        }else{
			        //
                    proc_KeyRs232_FanLsOffSet((unsigned char)cmd_code);
			        //rs232_printf("ASCiiKey=>%x\n",cmd_code);
			    }
                return(FALSE);		    
				//return(TRUE);		
			}    
            //20130712
			//---------------------------------------------------------
			//?direct fan hs on
			//---------------------------------------------------------
			else if (cmd_type == 0x40) {
			    //? power on     												
		        if(Power_GetPowerStatus() == 0) {			//?power off(0)
		        }else{
			        //
                    proc_KeyRs232_FanHsOnSet((unsigned char)cmd_code);
			        //rs232_printf("ASCiiKey=>%x\n",cmd_code);
			    }
                return(FALSE);		    
				//return(TRUE);	
			}	    
			//---------------------------------------------------------
			//?direct fan hs off
			//---------------------------------------------------------
			else if (cmd_type == 0x41) {
			    //? power on     												
		        if(Power_GetPowerStatus() == 0) {			//?power off(0)
		        }else{
			        //
                    proc_KeyRs232_FanHsOffSet((unsigned char)cmd_code);
			        //rs232_printf("ASCiiKey=>%x\n",cmd_code);
			    }
                return(FALSE);		    
				//return(TRUE);		
			}    
            //20140311
			//---------------------------------------------------------
			//?direct smps fan on
			//---------------------------------------------------------
			else if (cmd_type == 0x48) {
			    //? power on     												
		        if(Power_GetPowerStatus() == 0) {			//?power off(0)
		        }else{
			        //
                    proc_KeyRs232_SMPSFanOnSet((unsigned char)cmd_code);
			        //rs232_printf("ASCiiKey=>%x\n",cmd_code);
			    }
                return(FALSE);		    
				//return(TRUE);	
			}	    
			//---------------------------------------------------------
			//?direct smps fan off
			//---------------------------------------------------------
			else if (cmd_type == 0x49) {
			    //? power on     												
		        if(Power_GetPowerStatus() == 0) {			//?power off(0)
		        }else{
			        //
                    proc_KeyRs232_SMPSFanOffSet((unsigned char)cmd_code);
			        //rs232_printf("ASCiiKey=>%x\n",cmd_code);
			    }
                return(FALSE);		    
				//return(TRUE);		
			}    
			//---------------------------------------------------------
			//?direct shutdown temp set
			//---------------------------------------------------------
			else if (cmd_type == 0x42) {
			    //? power on     												
		        if(Power_GetPowerStatus() == 0) {			//?power off(0)
		        }else{
			        //20130916
                    proc_KeyRs232_ShutdownSet((unsigned char)cmd_code);
			        //rs232_printf("ASCiiKey=>%x\n",cmd_code);
			    }
                return(FALSE);		    
				//return(TRUE);		
			}    
			//---------------------------------------------------------
			//?direct SMPS shutdown temp set : 20131024
			//---------------------------------------------------------
			else if (cmd_type == 0x43) {
			    //? power on     												
		        if(Power_GetPowerStatus() == 0) {			//?power off(0)
		        }else{
			        //20131024
                    proc_KeyRs232_SmpsShutdownSet((unsigned char)cmd_code);
			        //rs232_printf("ASCiiKey=>%x\n",cmd_code);
			    }
                return(FALSE);		    
				//return(TRUE);		
			}    
			//---------------------------------------------------------
			//?direct SMPS shutdown temp set : 20131024
			//---------------------------------------------------------
			else if (cmd_type == 0x44) {
			    //? power on     												
		        if(Power_GetPowerStatus() == 0) {			//?power off(0)
		        }else{
			        //20131218
                    Func_SetDelayTimeVolumeDataTx((unsigned char)cmd_code);
			        //rs232_printf("ASCiiKey=>%x\n",cmd_code);
			    }
                return(FALSE);		    
				//return(TRUE);		
			}    
			//---------------------------------------------------------
			//balance_###!
    		//  type=0x47
			//---------------------------------------------------------
			//?balance [from RS232]
			//20140523 : for singal sensing feature, skip
			//---------------------------------------------------------
			else if (cmd_type == 0x47) {
				//? PP & MP2 unit & signal sensing nSW
				//20140819
				//if ( (RKB_mode_PP_MP == 1)&&(RKB_mode_PP_nMP2 == 0)&&(GetSignalSensing_nSW == 0) ) return(FALSE);			
				//? Max
				if (cmd_code > MAX_BALANCE_VOLUME) return(FALSE);
			    //? power on > current CH on >     												
		        if(Power_GetPowerStatus() == 0) {				//?power off(0), skip
		        	//
		        }else{
		        	if (cmd_ampType > 3) {					//?cmd_ampType >3, All ch
						for(i=0;i<4;i++){
							if (amp_on_off[i] == 1 ) {
			        			//balance value         
			        			//===>DirectTx==>RKB
                    			func_DirectBalanceSet((unsigned char)cmd_code,(i));	    
			        			//rs232_printf("ASCiiKey=>%x\n",cmd_code);
			        			//
			        		}
						}
		        	}else{
		        		if (amp_on_off[cmd_ampType] == 1 ) {	//?amp_on_off[]==on
			        		//balance value         
			        		//===>DirectTx==>RKB
                    		func_DirectBalanceSet((unsigned char)cmd_code,(cmd_ampType));	    
			        		//rs232_printf("ASCiiKey=>%x\n",cmd_code);
			        		//
			        	}
			        }
			    }
                return(FALSE);		    
				//return(TRUE);	
			}    
			//---------------------------------------------------------
			//RKB-D850 [RS232]
			//---------------------------------------------------------
			else if (cmd_type == 0x10) {
			    key_Type = cmd_type;
			    key_Value = AsciiKeyValueTable_RKB[(unsigned char)cmd_code];
				//long key counter clear!!
				//20110624
				count_rsLongKey = 0;
			    //rs232_printf("ASCiiKey=>%x\n",cmd_code);
                return(TRUE);
            }else{
				return(FALSE);
            }
		} 
        //------------------------------------------------------
}
//---------------------------------------------------------
//----------------------------------------------------------
// normal key
//
//----------------------------------------------------------
//----------------------------------------------------------
unsigned char Key_Rs232Check(void)
{
	char data,result;

	if(rs232_Rx_Tail == rs232_Rx_Head) {
		return(FALSE);
	}else{
		data = GetChar();
		if (data == '!') {
			result = Check_Rs232_key();					//?ASCiiKey check
			rs232_Key_Init();
			//rs232_key_count = 0;
			//rs232_key_mode = RS232_KEY_READY;
			//for(i=0;i<38;i++) rs232_key_buffer[i] = 0;
		}else{
			rs232_key_buffer[rs232_key_count] = data;
			rs232_key_count++;
			if (rs232_key_count >= 32) {						//? key length : Max
				rs232_Key_Init();
			}
			result = FALSE;
		}
    	return(result);
	}
}
/*
	key buffer를 검사하여 new key가 있으면 1을 return


****************************************************************/  
unsigned char	Key_InputCheck(void)
{
	if(Key_TactCheck() == TRUE ) {		//internal exec. done.....
		key_Kind = KEY_FRONT;
		return(TRUE);
	}else if(Key_Rs232Check() == TRUE) {
		key_Kind = KEY_RS232;
		return(TRUE);
	}else{
		return(FALSE);
	}
}


/*-----------------------------------------------------------------------------
ROUTINE NAME : main_SystemInit
INPUT        :
OUTPUT       :
DESCRIPTION  : processing special key,
COMMENTS     : VFD all On
-----------------------------------------------------------------------------*/
void main_SystemInit(void)
{
	const char ON_VFDALLSEG[4] ={0,0,0,0};		//Trim all Min
	char KeyMap[4],i,j;
	
	PowerDownCounter = TIME30MS;
	
	for (j=0;j<4;j++) {		//for 40ms
		adc_KeyRead(4);
		for (i=0;i<4;i++) adc_KeyMake(i);
	}
	memcpy(KeyMap,key.KeyBuf,4);
	if (memcmp((void*)KeyMap,(void*)ON_VFDALLSEG,4) == 0){
//		if (strcmp(KeyMap,ON_VFDALLSEG) == 0) {
//		f_initialize = ON;                  //?
    }
}
/*-----------------------------------------------------------------------------
ROUTINE NAME : main_Power(byte onoff)
INPUT        :
OUTPUT       :
DESCRIPTION  : power on or off
COMMENTS     : 
-----------------------------------------------------------------------------*/
void main_Power(byte onoff)
{
    f_display_on = onoff;
    f_power_on = onoff;
}
/*-----------------------------------------------------------------------------
ROUTINE NAME : main_AmpProtectionCheck (10msec)
INPUT        :
OUTPUT       :
DESCRIPTION  : check AMP protection 50msec
COMMENTS     : from main , 10msec everytime check
#define Amp_nProtection			GPIO_ReadInputDataBit(AMP_PORT, AMP_nRDY)
#define nOverLoadLowLevelCheck	GPIO_ReadInputDataBit(PORTD_PORT, nOverLoad)

// FAN hs on/off TEMP
#define	FAN_HS_OFF_TEMP_INITIAL		40			//20131105
#define	FAN_HS_ON_TEMP_INITIAL		45			//20131105
// FAN ls on/off TEMP
#define	FAN_LS_OFF_TEMP_INITIAL		30			//20140521
#define	FAN_LS_ON_TEMP_INITIAL		35			//20140521
// ShutDown TEMP -> got to AMP Protection Mode
//#define	SHUTDOWN_TEMP_INITIAL	75			//20131107
#define	SHUTDOWN_TEMP_INITIAL		80			//20131211
// SMPS ShutDown TEMP -> got to AMP Protection Mode
#define	SMPS_SHUTDOWN_TEMP_INITIAL	95			//20131107
#define SMPS_TEMPERATURE_MAX 		99			//20131211
// SMPS FAN on/off TEMP
#define	SMPS_FAN_OFF_TEMP_INITIAL	45			//20140325
#define	SMPS_FAN_ON_TEMP_INITIAL	50			//20140325
-----------------------------------------------------------------------------*/
void main_AmpProtectionExec(unsigned char mod)
{
    unsigned int i;
    
	//mute on
	Mute_AllMuteOn();
	//20131209 : mute Volume data Tx :
	NJW1159V_AllMuteOn();
	//AMP off : SMPS_off
	SMPS_Off();
	
   	//for(i=0;i<10000;i++);
   	
   	mode_amp_protect = ON;
   	//protection mode start!!! ==>[ muteOn, AmpOff ]
	//display message alert! => display rounte port checking!!!!!!!!!!!!!!!!!!11
	//LED mode = LED_MODE_PROTECTION
	set_LED_mode(LED_MODE_PROTECTION);
#ifdef LED_DIMMER_ENABLE
	//20140225 : timer => directly on/off
	Led_current_status = OFF;
#endif
	StandbyLED(0);
	//--------------------------------------
	mode_func = FUNCTION_DISP_OFF;
	
	Display_PowerAllBufferClear();          //20120927
	//f_tx_disp_buff = 0x03;					//RS232 TX : VFD Line 1~2
	//Display_FrontDispBufferClear();
	
	Set_DisplayBlockCTimer(BLOCKC_NORMAL,0);
	
	Power_SetPowerStatus(POWER_OFF);
	//main_PowerDownCheckSumSave();			//eeprom writing!

	//VFD all on flag clear!
	f_initialize = OFF;			//20090619
	
	Func_ProtectionStatus(mod);	//20130915 : protection=amp! or protection=overload! or protection=temperature!

	Func_AMPStatus();		//20130709
#ifdef ASCiiKEY
	Func_PowerStatus();     //20130709
#endif

	// stby led off ->on
	//set_LED_mode(LED_MODE_POWER_OFF);
	//StandbyLED(0);

    //20120927 : delay time 후, standby relay off
	//SMPS off, power relay off
	StandbyRelay_Off();			//20051220
    count_powerOffDelay = TIME20MSTO500MS;          //20121115
	//12V Out = Off
	//T12VOut(0);
    //    
	for(i=0;i<60000;i++);	
	//ADC resetting : disable
	ADC_DeInit();
    #if 0  //20150811
	if (RKB_mode_PP_MP == 0) {							//?(pin53:PC12): WS1/2=0, PP=1(add SMPS_sensor_1/2) : 20131024
		ADC_Configuration(0);
	}else{
		ADC_Configuration_PP(0);		//20131024
	}
	#endif
/*	if (mode_STMPower == STM_POWER_NORMAL) {		//20090818
		//20091015 : stanby 시 port 정리..
		Func_PowerOff_PortLow();
	}*/		
}
//--------------------------------------------------------------------
//	(10msec)
//--------------------------------------------------------------------
void main_AmpProtectionCheck(void)
{
    unsigned char mod;
    unsigned int i;
   
	if(mode_PowerStatus == POWER_ON0) {					//20131220
		//after 5sec, start port checking
    	if (count_delay_AMPProtectionCheck > 0) {
       	 --count_delay_AMPProtectionCheck;
        	return;             //20121012
    	}
        //skip, for temperature test : 20130915
        //return;
        //
        if (mode_amp_protect == OFF) {
			// powerOn(after 5sec) start check
			// AMP./RDY =>(High)=> AMPnReadyPort protection [ High Level = 50msec Hold ]
			// 20130710 : nOverLoad =>(Low)=> AMP protection [ Low Level = 50msec Hold ]
			if (RKB_mode_PP_MP == 1) {							//? PP unit
				// for PP/MP
				//------------------------------------------------------------
				//	AMP.RDY port check
				//------------------------------------------------------------
				if ( (f_power_on == ON)&&(Amp_nReadyPortC10Protection == 1) ) {   	//20131029 new port AMP./RDY for PP/MP
				    if (++Amp_protectionTime > TIME10MSTO40MS) {
				    	//20130915 : amp_rdy port=High
				    	mod =0;
				    	main_AmpProtectionExec(mod);
				    }
				//------------------------------------------------------------
				//	nOverLoad port check
				//------------------------------------------------------------
				}else if ( (f_power_on == ON)&&(nOverLoadLowLevelCheck == 0) ) {
				    if (++Amp_protectionTime > TIME10MSTO40MS) {
				    	//20130915 : nOverload = Low
				    	mod = 1;	
				    	main_AmpProtectionExec(mod);
					}
				}else{
				    Amp_protectionTime = 0;
				} 
			}else{
				// for WS 1/2
				//------------------------------------------------------------
				//	AMP.RDY port check
				//------------------------------------------------------------
				if ( (f_power_on == ON)&&(Amp_nReadyPortProtection == 1) ) {		//20131029 old port AMP./RDY  for WS1/2 	
				    if (++Amp_protectionTime > TIME10MSTO40MS) {
				    	//20130915 : amp_rdy port=High
				    	mod =0;
				    	main_AmpProtectionExec(mod);
				    }
				//------------------------------------------------------------
				//	nOverLoad port check
				//------------------------------------------------------------
				}else if ( (f_power_on == ON)&&(nOverLoadLowLevelCheck == 0) ) {
				    if (++Amp_protectionTime > TIME10MSTO40MS) {
				    	//20130915 : nOverload = Low
				    	mod = 1;	
				    	main_AmpProtectionExec(mod);
					}
				}else{
				    Amp_protectionTime = 0;
				} 
			}
			//============================================================
			//	ShutDown Check 
			//============================================================
			if ( (current_key[4] >= shutdown_temp)||(current_key[5] >= shutdown_temp) ) {	//20130917
				//goto AMP Protection
			    //20131008 : temp. => 80
			    mod = 2;	
			    main_AmpProtectionExec(mod);
			}
			//============================================================
			//	SMPS Heat Sink ShutDown Check 
			//============================================================
			if (RKB_mode_PP_MP == 1) {							//? PP unit
				//PP : add SMPS_sensor_1/2		
#ifdef SIGNAL_SENSING_ENABLE
				//20140619 : always 8100/D8100
				if ( (current_key[6] >= SMPS_shutdown_temp)||(current_key[7] >= SMPS_shutdown_temp) ) {	//20131024
					//goto AMP Protection
				    //20131024 : temp. => 95
				    mod = 3;	
				    main_AmpProtectionExec(mod);
				}
#else
				if ( (RKB_ModelID == RKB_8100) || (RKB_ModelID == RKB_D8100) ) {		
					//!8100 or D8100 [ 00=850, 01=8100, 10=D850, 11=D8100 ]
					if ( (current_key[6] >= SMPS_shutdown_temp)||(current_key[7] >= SMPS_shutdown_temp) ) {	//20131024
						//goto AMP Protection
					    //20131024 : temp. => 95
					    mod = 3;	
					    main_AmpProtectionExec(mod);
					}
				}else{
					//!850 or D850 [ 00=850, 01=8100, 10=D850, 11=D8100 ]
					if (current_key[6] >= SMPS_shutdown_temp) {	//20131024
						//goto AMP Protection
					    //20131024 : temp. => 99
					    mod = 3;	
					    main_AmpProtectionExec(mod);
					}
				}
#endif
			}
		}
	}else{
#ifdef OPTICAL_POWER_ENABLE
		if(mode_PowerStatus == POWER_OFF) {					//20140516
			//
			//	now standby on
			//
			//20140515 : optical_power_mode
			//20140516 : change time 200msec
			//
			//nOverLoad port = Low = is OPTICAL ON
			//
			// optical input 의  변화를 감지해야함...
			// 예를들어 optical=on 상태에서 command 로 끄면 , optical_power sensor 는 계속 입력이 있기 때문에 다시 power on 한다 --> 이것이 문제로 발생함.,
			// 그래서 command 로 끌 때, optical sensor 의 상태를 기억해서 비교하는 것으로 생각함...
			//
			//	when power-off --> [ opticalpower_bk = nOverLoadLowLevelCheck ]
			//
			if (optical_power_mode == ON) {
				//
				//check if (nOverLoad port == Low) , goto power on
				//
				if ( (opticalpower_bk == 0)&&(nOverLoadLowLevelCheck == 0) ) {
					//
					//BK=LOW, CURRENT=LOW
					//
				    count_opticalPower = 0;
				} else if ( (opticalpower_bk == 0)&&(nOverLoadLowLevelCheck == 1) ) {
					//
					//BK=LOW, CURRENT=HIGH(x)
					//
					if (++count_opticalPower > TIME10MSTO200MS) {
						//back-up change
						opticalpower_bk = 1;
					}
				} else if ( (opticalpower_bk == 1)&&(nOverLoadLowLevelCheck == 0) ) {				
					//
					//BK=HIGH, CURRENT=LOW(ON)
					//
					if (++count_opticalPower > TIME10MSTO200MS) {
						//back-up change
						opticalpower_bk = 0;
						//
						//goto power-on!
						//
						Func_SeperatePowerOn();
					}
				}else{
					//
					//BK=HIGH, CURRENT=HIGH(x)
					//
				    count_opticalPower = 0;
				} 
			}		
		}else{
			count_opticalPower = 0;
		}
#endif
	}
}
/*-----------------------------------------------------------------------------
ROUTINE NAME : main_PowerDownCheck
INPUT        :
OUTPUT       :
DESCRIPTION  : check power down
COMMENTS     : from main
-----------------------------------------------------------------------------*/
void main_PowerDownCheck(void)
{
   unsigned int i;

	if (GPIO_ReadInputDataBit(GPIOG,POWER_DOWN_CHK)==0) {
		if (f_power_on == ON && PowerDownCounter <= 0 ) {

				//mute on -> volume mute Tx
				Mute_AllMuteOn();
				//20131209 : mute Volume data Tx :
				#ifdef _RKB_DM8100
				#else
				NJW1159V_AllMuteOn();
				#endif

		        //----------------------------------------------------
		        //----------------------------------------------------
		        //power off command Tx
		        //20140107
				RS232_TxSkip = 0;			
				Func_PowerOffCommand_Tx();		        
		        //uart Tx
  				uart1_TxData();			//RS232 TX
		        //----------------------------------------------------
		        //----------------------------------------------------

				//AMP off : SMPS_off : 20130821
	            SMPS_Off();

				#ifdef _RKB_DM8100
				Amp_On(0);
				#endif

				//relay off
				Standby(0);			//stanby power OFF

                //flag setting : f_display_on , f_power_on
                main_Power(OFF);

				if(mode_PowerStatus != POWER_OFF){
		            //main_PowerDown();                   //AMP_volume, mode_speakerAB
				}
  			
            	while (GPIO_ReadInputDataBit(POWERDOWN_PORT,POWER_DOWN)==0) {};
     			for(i=0;i<60000;i++);
     			for(i=0;i<60000;i++);
     			for(i=0;i<60000;i++);
     			for(i=0;i<60000;i++);
     			for(i=0;i<60000;i++);
     			for(i=0;i<60000;i++);
     			for(i=0;i<60000;i++);
     			for(i=0;i<60000;i++);
     			for(i=0;i<60000;i++);
     			for(i=0;i<60000;i++);
            main();
		}
	}
}

/*
		Backup check
*********************************************************************/
unsigned char	Check_Backup(void)
{
	unsigned char a,b,result;

	//20140416 : change 0xAA to 0x55
	//20140811 : change 0x55 to 0x5a
	//20150302 : change 0x5a to 0xA5
	a = 0xA5;
	b = (unsigned char)nvr_xGetData(ADDR_CHECKRAM);

	if(a == b) {
		//20130515		
		current_key[0] = nvr_xGetData(ADDR_CURRENT_KEY0);			    //ADC0
		current_key[1] = nvr_xGetData(ADDR_CURRENT_KEY1);			    //ADC1
		current_key[2] = nvr_xGetData(ADDR_CURRENT_KEY2);			    //ADC2
		current_key[3] = nvr_xGetData(ADDR_CURRENT_KEY3);			    //ADC3
		#ifndef _RKB_DM8100
		current_key[4] = nvr_xGetData(ADDR_CURRENT_KEY4);			    //ADC4
		current_key[5] = nvr_xGetData(ADDR_CURRENT_KEY5);			    //ADC5
		#endif
		//20140521 : HS
		fan_hs_on_temp = nvr_xGetData(ADDR_FAN_HS_ON_TEMP);			    //Fan hs on
		fan_hs_off_temp = nvr_xGetData(ADDR_FAN_HS_OFF_TEMP);			//Fan hs off
		if ( fan_hs_on_temp < (fan_hs_off_temp+2) ) {
			fan_hs_off_temp = fan_hs_on_temp - 2;
			nvr_xPutData(ADDR_FAN_HS_OFF_TEMP,fan_hs_off_temp);
		}
		//20140521 : LS
		fan_ls_on_temp = nvr_xGetData(ADDR_FAN_LS_ON_TEMP);			    //Fan ls on
		fan_ls_off_temp = nvr_xGetData(ADDR_FAN_LS_OFF_TEMP);			//Fan ls off
		if ( fan_ls_on_temp < (fan_ls_off_temp+2) ) {
			fan_ls_off_temp = fan_ls_on_temp - 2;
			nvr_xPutData(ADDR_FAN_LS_OFF_TEMP,fan_ls_off_temp);
		}
		//20140311 : add SMPS_fan_on/off
		smps_fan_on_temp = nvr_xGetData(ADDR_SMPS_FAN_ON_TEMP);			    //SMPS Fan on
		smps_fan_off_temp = nvr_xGetData(ADDR_SMPS_FAN_OFF_TEMP);			//SMPS Fan off
		if ( smps_fan_on_temp < (smps_fan_off_temp+2) ) {
			smps_fan_off_temp = smps_fan_on_temp - 2;
			nvr_xPutData(ADDR_SMPS_FAN_OFF_TEMP,smps_fan_off_temp);
		}
		//20130916 : add shutdown
		shutdown_temp = nvr_xGetData(ADDR_SHUTDOWN_TEMP);			//Shutdown temp => (fan_hs_on_temp+1)
		if ( shutdown_temp <= fan_hs_on_temp ) {
			shutdown_temp = fan_hs_on_temp + 1;
			nvr_xPutData(ADDR_SHUTDOWN_TEMP,shutdown_temp);
		}
		//20140311
		if ( shutdown_temp <= smps_fan_on_temp ) {
			shutdown_temp = smps_fan_on_temp + 1;
			nvr_xPutData(ADDR_SHUTDOWN_TEMP,shutdown_temp);
		}
		//20131024 : add SMPS_shutdown_temp
		SMPS_shutdown_temp = nvr_xGetData(ADDR_SMPS_SHUTDOWN_TEMP);			//SMPS_shutdown_temp
		
		//----------------------------------------------------------------------
		//20140212 : add RS232 commands : AMP on/off, volume L/R control, balance
		amp_on_off[0] = nvr_xGetData(ADDR_AMP_A_ONOFF);			//AMP A
		amp_on_off[1] = nvr_xGetData(ADDR_AMP_B_ONOFF);			//AMP B
		amp_on_off[2] = nvr_xGetData(ADDR_AMP_C_ONOFF);			//AMP C
		amp_on_off[3] = nvr_xGetData(ADDR_AMP_D_ONOFF);			//AMP D
		
		volume_Lch[0] = nvr_xGetData(ADDR_A_VOL_LCH);			//AMP A
		volume_Lch[1] = nvr_xGetData(ADDR_B_VOL_LCH);			//AMP B
		volume_Lch[2] = nvr_xGetData(ADDR_C_VOL_LCH);			//AMP C
		volume_Lch[3] = nvr_xGetData(ADDR_D_VOL_LCH);			//AMP D

		volume_Rch[0] = nvr_xGetData(ADDR_A_VOL_RCH);			//AMP A
		volume_Rch[1] = nvr_xGetData(ADDR_B_VOL_RCH);			//AMP B
		volume_Rch[2] = nvr_xGetData(ADDR_C_VOL_RCH);			//AMP C
		volume_Rch[3] = nvr_xGetData(ADDR_D_VOL_RCH);			//AMP D

		balance[0] = nvr_xGetData(ADDR_A_BAL);			//AMP A
		balance[1] = nvr_xGetData(ADDR_B_BAL);			//AMP B
		balance[2] = nvr_xGetData(ADDR_C_BAL);			//AMP C
		balance[3] = nvr_xGetData(ADDR_D_BAL);			//AMP D
		//----------------------------------------------------------------------	
		//20150302 : for D850/D8100	
		input_selMode[0] = nvr_xGetData(ADDR_AMP_A_INPUTSELMODE);			//AMP A
		input_selMode[1] = nvr_xGetData(ADDR_AMP_B_INPUTSELMODE);			//AMP B
		input_selMode[2] = nvr_xGetData(ADDR_AMP_C_INPUTSELMODE);			//AMP C
		input_selMode[3] = nvr_xGetData(ADDR_AMP_D_INPUTSELMODE);			//AMP D
		//----------------------------------------------------------------------				
		backup_reconstruct_Start();					//checking over range
		//----------------------------------------------------------------------				
		Get_OneByteBackup();						// 1 byte load
		//----------------------------------------------------------------------				
		result = TRUE;	
	}else{
		result = FALSE;
	}
	return(result);
}

/*
	backup OK! => eeprom data load	
	(start,end,default)
*********************************************************************/
void	Get_OneByteBackup(void)
{
	// power option(0,2,1)
	mode_Power = nvr_xGetData(ADDR_POWEROPTION);
	if (mode_Power > POWER_ALWAYSON) {					//POWER_ALWAYSON=2
		mode_Power = POWER_STANDBY;						//POWER_STANDBY=1
		nvr_xPutData(ADDR_POWEROPTION,mode_Power);      //POWER_DIRECT=0
	}	
	//----------------------------------------------------------------	
	// display update (auto,manual)(0,1,0)
	//20131212
	mode_DisplayUpdate = nvr_xGetData(ADDR_DISPLAYUPDATE);
	if (mode_DisplayUpdate > 1) {								//20110323
		mode_DisplayUpdate = 0;     //Auto(0)									
		nvr_xPutData(ADDR_DISPLAYUPDATE,mode_DisplayUpdate);
	}
#ifdef DISPLAY_AUTOMANUAL_ALL_COMMAND	//20140402
	//20131212 : manual = 1 = RS232_TxSkip;
	RS232_TxSkip = mode_DisplayUpdate;		//20131212
#else	
	RS232_TxSkip = 0;
#endif	
	//----------------------------------------------------------------	
	//----------------------------------------------------------------	
	// delay time : volume data Tx
	//20131218
	time_delay_volume_Tx = nvr_xGetData(ADDR_DELAY_VOLUME_TX);
	if (time_delay_volume_Tx > 30) {	//max = 30 * 0.5sec = 15sec
		time_delay_volume_Tx = 0;     //default(0)									
		nvr_xPutData(ADDR_DELAY_VOLUME_TX,time_delay_volume_Tx);
	}
	//----------------------------------------------------------------	
	// Front Trim Knob enable(default)/disable
	//20140304
#ifdef RS232_VOL_ENABLE_IS_TRIMMIN			//used
		front_trimKnob_enable = 1;		//default(1) : 20140304							
#else
	front_trimKnob_enable = nvr_xGetData(ADDR_FRONT_TRIM_ENABLE);
	if (front_trimKnob_enable > 1) {	//
		front_trimKnob_enable = 1;		//default(1)									
		nvr_xPutData(ADDR_FRONT_TRIM_ENABLE,front_trimKnob_enable);
	}
#endif
	//----------------------------------------------------------------	
#ifdef LED_DIMMER_ENABLE
	// LED Dimmer(1,10,1)
	LED_Dimmer_value = nvr_xGetData(ADDR_LED_DIMMER);			
	if (LED_Dimmer_value > LED_DIMMER_MAX) {			//20140115
		//LED_Dimmer_value = 6;							//6=default						//20140828
		LED_Dimmer_value = LED_DIMMER_MIN;				//1=default(bright),10=(dark)	//20140225
		nvr_xPutData(ADDR_LED_DIMMER,LED_Dimmer_value);			
	}	
#endif
	//----------------------------------------------------------------	
	//20140515
#ifdef OPTICAL_POWER_ENABLE
	// optical_power_mode(0,1,0)
	optical_power_mode = nvr_xGetData(ADDR_OPTI_POWER_MODE);			
	if (optical_power_mode > 1) {
		optical_power_mode = 0;				//0=DISABLE(default),1=ENABLE
		nvr_xPutData(optical_power_mode,ADDR_OPTI_POWER_MODE);			
	}	
#endif
	//----------------------------------------------------------------	
}
/*
	FACTORY DEFAULT Reset! => 
	BACKUP ERROR! =>
                           
*********************************************************************/
void	Reload_DefaultBackup(void)
{                       
	unsigned char i,val;

	//power option
	mode_Power = POWER_STANDBY;						
	nvr_xPutData(ADDR_POWEROPTION,mode_Power);
	//----------------------------------------------------------------	
	// display update (auto,manual)(0,1,0)
	//20131212
	mode_DisplayUpdate = DISPLAY_AUTO;						
	nvr_xPutData(ADDR_DISPLAYUPDATE,mode_DisplayUpdate);
	//20131212 : manual = 1 = RS232_TxSkip;
	RS232_TxSkip = mode_DisplayUpdate;		//20131212
	//----------------------------------------------------------------
	//20131218 
	// delay time volume data Tx
	time_delay_volume_Tx = 0;     //default(0)									
	nvr_xPutData(ADDR_DELAY_VOLUME_TX,time_delay_volume_Tx);
	//----------------------------------------------------------------	
	//fan hs on temp
	fan_hs_on_temp = FAN_HS_ON_TEMP_INITIAL;				//45				
	nvr_xPutData(ADDR_FAN_HS_ON_TEMP,fan_hs_on_temp);
	//fan hs off temp
	fan_hs_off_temp = FAN_HS_OFF_TEMP_INITIAL;				//40			
	nvr_xPutData(ADDR_FAN_HS_OFF_TEMP,fan_hs_off_temp);
	//----------------------------------------------------------------	
	//20140521
	//fan ls on temp
	fan_ls_on_temp = FAN_LS_ON_TEMP_INITIAL;				//35				
	nvr_xPutData(ADDR_FAN_LS_ON_TEMP,fan_ls_on_temp);
	//fan ls off temp
	fan_ls_off_temp = FAN_LS_OFF_TEMP_INITIAL;				//30			
	nvr_xPutData(ADDR_FAN_LS_OFF_TEMP,fan_ls_off_temp);
	//----------------------------------------------------------------	
	//20130916 : add shutdown
	shutdown_temp = SHUTDOWN_TEMP_INITIAL;					//80	
	nvr_xPutData(ADDR_SHUTDOWN_TEMP,shutdown_temp);
	//20131024 : add SMPS_shutdown_temp
	SMPS_shutdown_temp = SMPS_SHUTDOWN_TEMP_INITIAL;		//95	
	nvr_xPutData(ADDR_SMPS_SHUTDOWN_TEMP,SMPS_shutdown_temp);
	//----------------------------------------------------------------	
	//20140325
	//SMPS fan on temp
	smps_fan_on_temp = SMPS_FAN_ON_TEMP_INITIAL;					//50				
	nvr_xPutData(ADDR_SMPS_FAN_ON_TEMP,smps_fan_on_temp);
	//SMPS fan off temp
	smps_fan_off_temp = SMPS_FAN_OFF_TEMP_INITIAL;					//45			
	nvr_xPutData(ADDR_SMPS_FAN_OFF_TEMP,smps_fan_off_temp);

	//----------------------------------------------------------------	
#ifdef LED_DIMMER_ENABLE
	//LED dimmer[1..10]
	//LED_Dimmer_value = 6;						//6=default						//20140828
	LED_Dimmer_value = LED_DIMMER_MIN;			//1=default(bright),10=(dark)	//20140225
	nvr_xPutData(ADDR_LED_DIMMER,LED_Dimmer_value);
#endif
	//----------------------------------------------------------------	
	//backup OK ram check
	//20140811 : change 0x55 to 0x5a
	//20150302 : change 0x5a to 0xA5
	val = 0xA5;						
	nvr_xPutData(ADDR_CHECKRAM,val);
	//----------------------------------------------------------------	
	//current_key
	for (i=0;i<4;i++) {
	    current_key[i] = TRIM_INITIAL;
	}
	current_key[4] = TEMP_INITIAL;
	current_key[5] = TEMP_INITIAL;
	current_key[6] = TEMP_INITIAL;
	current_key[7] = TEMP_INITIAL;
	
	nvr_xPutData(ADDR_CURRENT_KEY0	,current_key[0]);
	nvr_xPutData(ADDR_CURRENT_KEY1	,current_key[1]);
	nvr_xPutData(ADDR_CURRENT_KEY2	,current_key[2]);
	nvr_xPutData(ADDR_CURRENT_KEY3	,current_key[3]);
	nvr_xPutData(ADDR_CURRENT_KEY4	,current_key[4]);
	nvr_xPutData(ADDR_CURRENT_KEY5	,current_key[5]);
	//----------------------------------------------------------------
	//20140212 : add RS232 commands : AMP on/off, volume L/R control, balance
	for (i=0;i<4;i++) {
	    amp_on_off[i] = AMP_ON_INITIAL;			//1
	    volume_Lch[i] =	VOLUME_INITIAL;			//0
	    volume_Rch[i] =	VOLUME_INITIAL;			//0
	    balance[i] = BALANCE_VOLUME_INITIAL;	//15
	    //20150302
	    input_selMode[i] = INPUT_SEL_AUTO;		//0
	}
	nvr_xPutData(ADDR_AMP_A_ONOFF,amp_on_off[0]);
	nvr_xPutData(ADDR_AMP_B_ONOFF,amp_on_off[1]);
	nvr_xPutData(ADDR_AMP_C_ONOFF,amp_on_off[2]);
	nvr_xPutData(ADDR_AMP_D_ONOFF,amp_on_off[3]);
	nvr_xPutData(ADDR_A_VOL_LCH,volume_Lch[0]);
	nvr_xPutData(ADDR_B_VOL_LCH,volume_Lch[1]);
	nvr_xPutData(ADDR_C_VOL_LCH,volume_Lch[2]);
	nvr_xPutData(ADDR_D_VOL_LCH,volume_Lch[3]);
	nvr_xPutData(ADDR_A_VOL_RCH,volume_Rch[0]);
	nvr_xPutData(ADDR_B_VOL_RCH,volume_Rch[1]);
	nvr_xPutData(ADDR_C_VOL_RCH,volume_Rch[2]);
	nvr_xPutData(ADDR_D_VOL_RCH,volume_Rch[3]);
	nvr_xPutData(ADDR_A_BAL,balance[0]);
	nvr_xPutData(ADDR_B_BAL,balance[1]);
	nvr_xPutData(ADDR_C_BAL,balance[2]);
	nvr_xPutData(ADDR_D_BAL,balance[3]);
	//----------------------------------------------------------------
	//20150302
	nvr_xPutData(ADDR_AMP_A_INPUTSELMODE,input_selMode[0]);
	nvr_xPutData(ADDR_AMP_B_INPUTSELMODE,input_selMode[1]);
	nvr_xPutData(ADDR_AMP_C_INPUTSELMODE,input_selMode[2]);
	nvr_xPutData(ADDR_AMP_D_INPUTSELMODE,input_selMode[3]);
	//----------------------------------------------------------------
	// Front Trim Knob enable(default)/disable
#ifdef RS232_VOL_ENABLE_IS_TRIMMIN			//used
#ifdef _RKB_DM8100
    front_trimKnob_enable = 0;
#else
	front_trimKnob_enable = 1;     				//default(1) : 20140304	
#endif //_RKB_DM8100
#else
	front_trimKnob_enable = 1;     				//default(1)									
	nvr_xPutData(ADDR_FRONT_TRIM_ENABLE,front_trimKnob_enable);
#endif
	//----------------------------------------------------------------	
	//20140515
#ifdef OPTICAL_POWER_ENABLE
	// optical_power_mode(0,1,0)
	optical_power_mode = 0;				//0=DISABLE(default),1=ENABLE
	nvr_xPutData(optical_power_mode,ADDR_OPTI_POWER_MODE);			
#endif
	//----------------------------------------------------------------	
	
}


/*
	power off 시 call
*********************************************************************/	
void	main_PowerDownCheckSumSave(void)
{
	unsigned char sum;
	
	sum = 0;
//00~05		
	sum += current_key[0];
	sum += current_key[1];
	sum += current_key[2];
	sum += current_key[3];
	sum += current_key[4];
	sum += current_key[5];
				
	nvr_xPutData(EEPROM_ADDR_CHECKSUM,sum);			// 0x??	
}

#ifdef IAP_UPGRADE
/*-----------------------------------------------------------------------------
ROUTINE NAME : main_iAPUpgrade
INPUT        : 
OUTPUT       :
DESCRIPTION  : 
COMMENTS     : 2009-04-16 ..damho an
-----------------------------------------------------------------------------*/
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
/*-----------------------------------------------------------------------------
ROUTINE NAME : main_iAPUpgrade
INPUT        : 
OUTPUT       :
DESCRIPTION  : 
COMMENTS     : 2009-04-16 ..damho an
-----------------------------------------------------------------------------*/
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
#endif
//
//**************************************************************************************************
//
#ifdef IC_DEBUG
//
//
#if 0
	rs232_printf("\n");
	rs232_printf("--------------------------------------------------------------- \n");
	rs232_printf("        RS232 2 LINE Monitor                  \n");
	rs232_printf("                                              \n");
	rs232_printf("                                              \n");
	rs232_printf("                                              \n");
	rs232_nprintf0("          %s,8,N,1                  \n",str_baud[option_read],9);	
	rs232_printf("                                              \n");	
	rs232_printf("  i2r xx : KwangSung 2 byte read(addr)			 \n");
	rs232_printf("  i4r xx : KwangSung 4 byte read(addr)			 \n");
	rs232_printf("  i4t : kwang sung test flow	\n");
	rs232_printf("  i2w xx xx xx : KwangSung 2 byte write(addr,data1,2) \n");
	rs232_printf("  i4w xx xx xx xx xx : KwangSung 4 byte write(addr,data1,2,3,4)\n");
	rs232_printf("  iiw xx xx xx : iic write(dev_addr,reg_addr,data)\n");
	rs232_printf("  iir xx : iic read(dev_addr,reg_addr) \n");
	rs232_printf("  iv : version			 \n");
	rs232_printf("  ib : current baudrate \n");
	rs232_printf("           								\n");
	rs232_printf("         ROTEL Technology Global Inc.  \n");
	rs232_printf("         %s,%s\n",str_time,str_date);
	rs232_printf("---------------------------------------------------------------\n");
#endif	

/***********************************************************/

#if 0
/* 
     ascii to bin
     
********************************************/
unsigned char Asc2Bin(unsigned char ch1,unsigned char ch2)
{
	unsigned char comb;
	comb = 0;

	/* printf("%c,%c:",ch1,ch2); */
	/* 0 ~ 9,0x30 ~ 0x39 */
	if(ch1 < 0x3a){
		ch1 = ch1-0x30;
	/* A ~ F,0x41 ~ 0x46 */
	}else if(ch1 < 0x47){
		ch1 = ch1-0x41+0x0a;
	/* a ~ f,0x61 ~ 0x66 */
	}else{
		ch1 = ch1-0x61+0x0a;
	}
	ch1 = ch1 << 4;
	comb = ch1;
	comb &= 0xf0;

	/* printf(" %x ",comb); */
	/* 0 ~ 9,0x30 ~ 0x39 */
	if(ch2 < 0x3a){
		ch2 = ch2-0x30;
	/* A ~ F,0x41 ~ 0x46 */
	}else if(ch2 < 0x47){
		ch2 = ch2-0x41+0x0a;
	/* a ~ f,0x61 ~ 0x66 */
	}else{
		ch2 = ch2-0x61+0x0a;
	}
	comb |= ch2;
	/* printf("%x\n",comb); */
	return(comb);
}
#endif	


void IC_DebugHandler(void)
{	
    unsigned char count,i,j;
    unsigned char addr,data,data1,data2,data3;
	unsigned char CM_reg_L, CM_reg_H;
    unsigned char temp_data[16];
    unsigned char tmp;
    unsigned char a,b,c,d;
    unsigned long k;
    word w_data;
    int tmp1;
//ASCiiKey test
	char comand[32]; 
	int cmd_code;
	char cmd_type;
//	          
	if(ic_comm_ok == 1){
		ic_comm_use = 1;
		switch(ic_comm_buf[0]){
		    //------------------------------------------------------------------
/*
			case 'k':
				//ik cccccccccc!
				//if (ic_comm_index == 4) {
				for (i=0;i<32;i++) comand[i]= 0;
				for (i=0;i<ic_comm_index;i++) {
					if (ic_comm_buf[i+2] == '!') break;
					comand[i] = ic_comm_buf[i+2];
				}
				//rs232_printf("ASCiiKey=>",blank);
				//for (j=0;j<i;j++) PutChar(temp_data[j]);
				//PutChar('\r');
				//
	            cmd_code = getCmdID(comand,&cmd_type);
				if (cmd_code < 0) {
					//key error!
					rs232_printf("ASCiiKey error!",blank);
				}else{
					rs232_printf("ASCiiKey=>%x\n",cmd_code);
				}
				break;	
*/							
		    //------------------------------------------------------------------

			case '1':
				switch(ic_comm_buf[1]){
/*
					// i1k : AK4114 
					case 'k':
					case 'K':
						//opti, coax
						a_Work[0] = 0x21;
						a_Work[1] = 0x5a;        // 24bit I2S / LRCK=O
						s_Ak4114MsgTrans(typ);
						//opt1 / coax1 
						a_Work[0] = 0x23;
						a_Work[1] = 0x00;
						s_Ak4114MsgTrans(typ);
						//
						a_Work[0] = 0x22;
						a_Work[1] = 0x88;
						s_Ak4114MsgTrans(typ);
						a_Work[0] = 0x20;
						a_Work[1] = 0x4b;						//CM[1:0]=11(DAUX),(48K), 512fs
						s_Ak4114MsgTrans(typ);
						//
						s_Ak4114MsgRead(0, 0x00);							
						rs232_printf("00=>%x\n",a_Work[2]);
						s_Ak4114MsgRead(0, 0x01);							
						rs232_printf("01=>%x\n",a_Work[2]);
						s_Ak4114MsgRead(0, 0x02);							
						rs232_printf("02=>%x\n",a_Work[2]);
						s_Ak4114MsgRead(0, 0x03);							
						rs232_printf("03=>%x\n",a_Work[2]);
						break;										
					// i1i : iRadio 
					case 'i':
					case 'I':
						PutChar('\r');
						rs232_printf("iRadio :\r",blank);
						for (i=0;i<30;i++) {
							rs232_printf("%x :",i+1);
							for (j=0;j<20;j++) {
								PutChar(uStingray.sSName.IRadioStationName[i][j]);			//uart 1 
							}
							PutChar('\r');
						}
						break;										
					// i1f : FM 
					case 'f':
					case 'F':
						rs232_printf("FM :\r",blank);
						for (i=0;i<30;i++) {
							rs232_printf("%x :",i+1);
							for (j=0;j<20;j++) {
								PutChar(uStingray.sSName.FMStationName[i][j]);			//uart 1 
							}
							PutChar('\r');
						}
						break;										
					// i1b : DAB 
					case 'b':
					case 'B':
						rs232_printf("DAB :\r",blank);
						for (i=0;i<30;i++) {
							rs232_printf("%x :",i+1);
							for (j=0;j<20;j++) {
								PutChar(uStingray.sSName.DABStationName[i][j]);			//uart 1 
							}
							PutChar('\r');
						}
						break;										
					case 'm':
					case 'M':
							//key Setup Start!!!!
							Func_Setup();
						break;									
					case 't':
					case 'T':
							Stingray_Tx_GetState();
						break;									
					case 'r':
					case 'R':
						Stingray_Tx_FactoryReset();
						//Eeprom Factory Reset!!!
						//nvr_xPutData(ADDR_RESET,ON);
						break;									
*/
//--------------------------------------------------------------------------------------------------
//----------------------------------------------------------    
//readInput : 16Bits
//#define GetInputTC2002A_u16BUS		GPIO_ReadInputData(GPIOB)
//readOutput : 16Bits
//#define GetOutputTC2002A_u16BUS		GPIO_ReadOutputData(GPIOB)
//STM32F read PORTA/B/C/D/E
					case 'p':
					case 'P':
                        rs232_printf("AK4114 ReadInputData ===>\n",blank);
			            rs232_printf("PA=>%x\n",GPIO_ReadInputData(GPIOA));
			            rs232_printf("PB=>%x\n",GPIO_ReadInputData(GPIOB));
			            rs232_printf("PC=>%x\n",GPIO_ReadInputData(GPIOC));
			            rs232_printf("PD=>%x\n",GPIO_ReadInputData(GPIOD));
			            rs232_printf("PE=>%x\n",GPIO_ReadInputData(GPIOE));
                        rs232_printf("AK4114 ReadOutputData :\n",blank);
			            rs232_printf("PA=>%x\n",GPIO_ReadOutputData(GPIOA));
			            rs232_printf("PB=>%x\n",GPIO_ReadOutputData(GPIOB));
			            rs232_printf("PC=>%x\n",GPIO_ReadOutputData(GPIOC));
			            rs232_printf("PD=>%x\n",GPIO_ReadOutputData(GPIOD));
			            rs232_printf("PE=>%x\n",GPIO_ReadOutputData(GPIOE));
						break;									
//--------------------------------------------------------------------------------------------------
/*					case 'c':
					case 'C':           //CMedia reg_00 read
					    //i1cxx
        if (ic_comm_index == 4) {
            data = Asc2Bin(ic_comm_buf[2],ic_comm_buf[3]);
		
            //20130111
            //f_CM_i2cSuspend = Low : OK read
            if (f_CM_i2cSuspend == 0) {
			    //read reg_00
                CM_xGetData(data, &CM_reg_L, &CM_reg_H);  //(r) channels, frequency, bit-depth
                //bit
                data1 = (CM_reg_L >>1) & 0x03;
                rs232_printf("CMedia ============================> \n",blank);
                //rs232_printf("CM bit =>%x\n",data1);
                if (data1 == 0 ) rs232_printf("bit0 =>> 16 bit\n",blank);
                if (data1 == 1 ) rs232_printf("bit1 ==> 24 bit\n",blank);
                if (data1 == 2 ) rs232_printf("bit2 ==> 32 bit\n",blank);
                if (data1 == 3 ) rs232_printf("bit3 ==> 24 bit\n",blank);
                //freq
                data2 = (CM_reg_L >> 3) & 0x07;
                //rs232_printf("CM freq =>%x\n",data2);
                if (data2 == 0 ) rs232_printf("freq0 =>> ?? kHz\n",blank);
                if (data2 == 1 ) rs232_printf("freq1 =>> 44.1 kHz\n",blank);
                if (data2 == 2 ) rs232_printf("freq2 =>> 48 kHz\n",blank);
                if (data2 == 3 ) rs232_printf("freq3 =>> ?? kHz\n",blank);
                if (data2 == 4 ) rs232_printf("freq4 =>> 88.2 kHz\n",blank);
                if (data2 == 5 ) rs232_printf("freq5 =>> 96 kHz\n",blank);
                if (data2 == 6 ) rs232_printf("freq6 =>> 176.4 kHz\n",blank);
                if (data2 == 7 ) rs232_printf("freq1 =>> 192 kHz\n",blank);
                //channel
                data3 = CM_reg_H & 0x03;
                //rs232_printf("CM CH =>%x\n",data3);
                if (data3 == 0 ) rs232_printf("CH0 =>> 2 CH\n",blank);
                if (data3 == 1 ) rs232_printf("CH1 =>> 4 CH\n",blank);
                if (data3 == 2 ) rs232_printf("CH2 =>> 6 CH\n",blank);
                if (data3 == 3 ) rs232_printf("CH3 =>> 8 CH\n",blank);
            }
	    }
						break;
*/						
//--------------------------------------------------------------------------------------------------
					case 'r':
					case 'R':
					    //i1rxx
		if (ic_comm_index == 4) {
		data = Asc2Bin(ic_comm_buf[2],ic_comm_buf[3]);
		nvr_SetRead(data);
		temp_data[0] = (unsigned char)nvr_GetData();
		temp_data[1] = (unsigned char)nvr_GetData();
		temp_data[2] = (unsigned char)nvr_GetData();
		temp_data[3] = (unsigned char)nvr_GetData();
		temp_data[4] = (unsigned char)nvr_GetData();
		temp_data[5] = (unsigned char)nvr_GetData();
		temp_data[6] = (unsigned char)nvr_GetData();
		temp_data[7] = (unsigned char)nvr_GetData();
		temp_data[8] = (unsigned char)nvr_GetData();
		temp_data[9] = (unsigned char)nvr_GetData();
		temp_data[10] = (unsigned char)nvr_GetData();
		temp_data[11] = (unsigned char)nvr_GetData();
		temp_data[12] = (unsigned char)nvr_GetData();
		temp_data[13] = (unsigned char)nvr_GetData();
		temp_data[14] = (unsigned char)nvr_GetData();
		temp_data[15] = (unsigned char)nvr_GetData_NoAck();
		iic_Stop();
		for (i=0;i<16;i++) rs232_printf("read=>%x\n",temp_data[i]);
	}
						break;
								
					case 'w':
					case 'W':
		if (ic_comm_index == 4) {
		data = Asc2Bin(ic_comm_buf[2],ic_comm_buf[3]);
	nvr_SetWrite(data);												//16 EA 단위로 writing
	nvr_PutData('a');
	nvr_PutData('a');
	nvr_PutData('b');
	nvr_PutData('b');
	nvr_PutData('c');
	nvr_PutData('c');
	nvr_PutData('d');
	nvr_PutData('d');
	nvr_PutData('e');
	nvr_PutData('e');
	nvr_PutData('f');
	nvr_PutData('f');
	nvr_PutData('g');
	nvr_PutData('g');
	nvr_PutData('h');
	nvr_PutData('h');
	nvr_StopWrite();
}
						break;
/*								
					case 'd':
					case 'D':
//DAC	
#if 1
//	iDAC_format = 0x0408;							//24bit i2s
//	DAC_8740_Tx16(iDAC_format);					//24bit i2s
//	iDAC_format = 0x0601;							//i2s
//	DAC_8740_Tx16(iDAC_format);					//i2s
							if (++DAC_format >= 8) DAC_format = 0x00;
							switch (DAC_format){
								case 0:
									//0000 0100 000x x000
									iDAC_format = 0x0400;						//16bit_i2s / 16bit Standard right justified
									DAC_8740_Tx16(iDAC_format); 				//
									//0000 0110 0000 000x
									iDAC_format = 0x0600;
									DAC_8740_Tx16(iDAC_format); 				//
									break;
								case 1:
									iDAC_format = 0x0408;						//
									DAC_8740_Tx16(iDAC_format); 				//
									iDAC_format = 0x0600;
									DAC_8740_Tx16(iDAC_format); 				//
									break;
								case 2:
									iDAC_format = 0x0410;						//
									DAC_8740_Tx16(iDAC_format); 				//
									iDAC_format = 0x0600;
									DAC_8740_Tx16(iDAC_format); 				//
									break;
								case 3:
									iDAC_format = 0x0418;						//
									DAC_8740_Tx16(iDAC_format); 				//
									iDAC_format = 0x0600;
									DAC_8740_Tx16(iDAC_format); 				//
									break;
								case 4:
									iDAC_format = 0x0400;						//
									DAC_8740_Tx16(iDAC_format); 				//
									iDAC_format = 0x0601;
									DAC_8740_Tx16(iDAC_format); 				//
									break;
								case 5:
									iDAC_format = 0x0408;						//
									DAC_8740_Tx16(iDAC_format); 				//
									iDAC_format = 0x0601;
									DAC_8740_Tx16(iDAC_format); 				//
									break;
								case 6:
									iDAC_format = 0x0410;						//
									DAC_8740_Tx16(iDAC_format); 				//
									iDAC_format = 0x0601;
									DAC_8740_Tx16(iDAC_format); 				//
									break;
								case 7:
									iDAC_format = 0x0418;						//
									DAC_8740_Tx16(iDAC_format); 				//
									iDAC_format = 0x0601;
									DAC_8740_Tx16(iDAC_format); 				//
									break;
								default:
									break;
							}
			rs232_printf("DAC8740 : %x\n",DAC_format);
#else
							if (DAC_format >= 0x0b) {
								DAC_format = 0x01;
							}else{
								DAC_format += 0x02;
							}
							AK4353_SetWriteOne(0x20,0x00,DAC_format);			//64fs				// Control 1(0000/DIF2/DIF1/DIF0/RSTN)
	rs232_printf("Format=>%x\n",DAC_format);
#endif
			rs232_printf("Format=>%x\n",DAC_format);
						break;
*/														
					// i1r xx(command) xx(nByte) 
/*					case 'r':
					case 'R':
						freesat_Tx_skip = 1; 	//normal dab tx cancle!!! 20071026

						fI2C_Write_1byte(0x80, Asc2Bin(ic_comm_buf[3],ic_comm_buf[4]));
						rs232_printf("iic 1w 80 %x\n",Asc2Bin(ic_comm_buf[3],ic_comm_buf[4]));

						//delay 20msec? : 20071026
				  		for(k=0;k<40000;k++);	
						
						if(Asc2Bin(ic_comm_buf[6],ic_comm_buf[7]) == 1) {
							fI2C_Read_1Byte(0x81, 1, &Get_string[0]);			//ack
						}else{
							freesatI2C_read_nbyte(0x81,Asc2Bin(ic_comm_buf[6],ic_comm_buf[7]), Get_string);			//DLS=129byte
						}
						rs232_printf("iic 1r 81 %x",Asc2Bin(ic_comm_buf[6],ic_comm_buf[7]));
						for(i=0;i<Asc2Bin(ic_comm_buf[6],ic_comm_buf[7]);i++){
							rs232_printf(" %x",Get_string[i]);
						}
						rs232_printf("\n",blank);
						break;*/
					default:
						rs232_printf("unknown command\n");
						break;						
				}
				break;		
		    //------------------------------------------------------------------
			case '2':
				switch(ic_comm_buf[1]){
					// i2r xx : eeprom Read addr(xx)
					case 'r':
					case 'R':
						if (ic_comm_index == 4) {
		data = Asc2Bin(ic_comm_buf[2],ic_comm_buf[3]);
		data = nvr_xGetData(data);			//?data
	rs232_printf("data=>%x\n",data);
						}
						break;										
					// i2w xx dd: eeprom Write addr(xx) data(dd)
					case 'w':
					case 'W':
						if (ic_comm_index == 6) {
		addr = Asc2Bin(ic_comm_buf[2],ic_comm_buf[3]);
		data = Asc2Bin(ic_comm_buf[4],ic_comm_buf[5]);
		nvr_xPutData(addr,data);
	rs232_printf("addr=>%x",addr);
	rs232_printf("data=>%x\n",data);
						}
						break;										
/*
					// i2k xx
					case 'k':
					case 'K':
						if (ic_comm_index == 4) {
							Stingray_Key_Value = Asc2Bin(ic_comm_buf[2],ic_comm_buf[3]);
							Stingray_Tx_KeyPressed(Stingray_Key_Value);
//chd
							Stingray_Tx_KeyReleased(Stingray_Key_Value);
							Stingray_Key_Value = 0;
						}
						break;										
					// i2m xx	: MPEG TX(data)
					case 'm':
					case 'M':
						if (ic_comm_index == 4) {
							data = Asc2Bin(ic_comm_buf[2],ic_comm_buf[3]);
#ifdef MPEG_IS
							tx_MTK(data);
#endif
						}
						break;										
					// i2s xx xx 
					case 's':
					case 'S':
						if (ic_comm_index == 6) {
							a = Asc2Bin(ic_comm_buf[2],ic_comm_buf[3]);
							b = Asc2Bin(ic_comm_buf[4],ic_comm_buf[5]);
							Stingray_StationPresetNumber = b;
							Stingray_Tx_GetStationName(a,b);
							Stingray_Startup_mode = S_STATIONNAME_READ;
						}
						break;										
					// i2v xx
					case 'v':
					case 'V':
						if (ic_comm_index == 4) {
				a = Asc2Bin(ic_comm_buf[2],ic_comm_buf[3]);
				w_data = 0x00ff & (word) a;
	rs232_printf("8740=>%d\n",(w_data & 0x00ff));
				DAC_8740_Tx16(w_data | 0x0100);				//Left vol
				DAC_8740_Tx16(w_data | 0x0300);				//Right vol
						}
						break;
*/											
					default:
						rs232_printf("unknown command\n");
						break;						
				}
				break;		
/*
		    //------------------------------------------------------------------
			case '3':
				// Freesat DAB
				switch(ic_comm_buf[1]){
					// buffer 
					case 'm':
					case 'M':
						rs232_printf("\ncurr_StatusRead :%x\n",curr_StatusRead);		
						for (i=1;i<=200 ;i++ );
						rs232_printf("curr_amount_ensemble :%x\n",curr_amount_ensemble);		
						for (i=1;i<=200 ;i++ );
						rs232_printf("curr_amount_service :%x\n",curr_amount_service);		
						for (i=1;i<=200 ;i++ );
						rs232_printf("curr_freq_index :%x\n",curr_freq_index);		
						for (i=1;i<=200 ;i++ );
						rs232_printf("curr_pty_index : %x\n",curr_pty_index);		
						for (i=1;i<=200 ;i++ );
						rs232_printf("curr_signal_power :%x\n",curr_signal_power);
						break;										
					default:
						rs232_printf("unknown command\n");
						break;						
				}
				break;
		    //------------------------------------------------------------------
			case '4':
				// Freesat DAB
				switch(ic_comm_buf[1]){
					// buffer 
					case 's':
					case 'S':
						//Status Read
						if(freesat_Tx_skip == 0){
							freesat_Tx_skip = 1;
							//curr_StatusRead |= 0x04;	//reConfig flag setting : 20060719
							fManual_buffer[fManual_Memory_count][16] = 0;	//reConfig flag setting : 20060719
						}else{
							freesat_Tx_skip = 0;
						}
						rs232_printf("Freesat TX skip=>%x\n",freesat_Tx_skip);
						break;
					case 'm':
					case 'M':
						for (k=1;k<1000 ;k++ );
						rs232_printf("\ncurr_service_label_id(17): ",blank);
						for (k=1;k<1000 ;k++ );
						for (i=0;i<17;i++) rs232_printf(" %x",curr_service_label_id[i]);
						for (k=1;k<1000 ;k++ );
						rs232_printf("\nMcurr_service_label_id(17): ",blank);
						for (k=1;k<1000 ;k++ );
						for (i=0;i<17;i++) rs232_printf(" %x",Mcurr_service_label_id[i]);
						for (k=1;k<1000 ;k++ );
						rs232_printf("\ncurr_ensemble_label(16): ",blank);
						for (k=1;k<1000 ;k++ );
						for (i=0;i<16;i++) rs232_printf(" %x",curr_ensemble_label[i]);
						//rs232_printf("\ncurr_bitrate(2): ",blank);
						//for (i=0;i<2;i++) rs232_printf(" %x",curr_bitrate[i]);
						//rs232_printf("\ncurr_date_time(5): ",blank);
						//for (i=0;i<5;i++) rs232_printf(" %x",curr_date_time[i]);
                  
						for (k=1;k<1000 ;k++ );
						rs232_printf("\nfManual_Memory_count :%x\n",fManual_Memory_count);
						for (k=1;k<1000 ;k++ );
						rs232_printf("\fManual_Memory_total :%x\n",fManual_Memory_total);
						for (k=1;k<1000 ;k++ );
						rs232_printf("\nfManual_buffer(n): ",blank);
						for (i=0;i<fManual_Memory_total;i++){
							rs232_printf("\n%x :",i);
							for (k=1;k<1000 ;k++ );
							for (j=0;j<18;j++) rs232_printf(" %x",fManual_buffer[i][j]);
							for (k=1;k<1000 ;k++ );
						}
						rs232_printf("\n",blank);
						break;
					default:
						rs232_printf("unknown command\n");
						break;						
				}
				break;		
		    //------------------------------------------------------------------
			case '5':
				// Freesat DAB
				switch(ic_comm_buf[1]){
					// buffer 
					case 'm':
					case 'M':
						rs232_printf("\n mode_setup :%x\n",mode_setup);		
						for (i=1;i<=200 ;i++ );
						rs232_printf("mode_DispBlockC :%x\n",mode_DispBlockC);		
						for (i=1;i<=200 ;i++ );
						rs232_printf("mode_DisplayDAB :%x\n",mode_DisplayDAB);		
						for (i=1;i<=200 ;i++ );
						rs232_printf("mode_DAB_Exec :%x\n",mode_DAB_Exec);		
						for (i=1;i<=200 ;i++ );
						rs232_printf("AUTO_TuneMode :%x\n",AUTO_TuneMode);		
						break;
					// buffer 
					case 'p':
					case 'P':
						for (k=1;k<1000 ;k++ );
						rs232_printf("\nserviceLabel_buffer[99][16]: ",blank);
						for (i=0;i<99;i++){
							rs232_printf("\n%x :",i);
							for (k=1;k<1000 ;k++ );
							for (j=0;j<16;j++) rs232_printf(" %x",serviceLabel_buffer[i][j]);
							for (k=1;k<1000 ;k++ );
						}
						rs232_printf("\n",blank);
						break;						
					default:
						rs232_printf("unknown command\n");
						break;						
				}
				break;
		    //------------------------------------------------------------------
			case '6':
				// Freesat DAB
				switch(ic_comm_buf[1]){
					// buffer 
					case 'm':
					case 'M':
						rs232_printf("\ncurr_StatusRead :%x\n",curr_StatusRead);		
						rs232_printf("mode_DLS :%x\n",mode_DLS);		
						//for (i=1;i<=200 ;i++ );
						rs232_printf("mode_DLS_read :%x\n",mode_DLS_read);		
						rs232_printf("count_DLS_max :%x\n",count_DLS_max);		//20061002
						rs232_printf("count_DLS_current :%x\n",count_DLS_current);		//20061009
						//for (i=1;i<=200 ;i++ );
			         
						for (i=1;i<=200 ;i++ );
						rs232_printf("\n DLS_buffer(n): ",blank);
						for (i=0;i<129;i++) rs232_printf(" %x",DLS_buffer[i]);
						rs232_printf("\n",blank);
						
						for (i=1;i<=200 ;i++ );
						rs232_printf("\n curr_DLS_buffer(n): ",blank);
						for (i=0;i<129;i++) rs232_printf(" %x",curr_DLS_buffer[i]);
						rs232_printf("\n",blank);
						break;
					default:
						rs232_printf("unknown command\n");
						break;						
				}
				break;		
		    //------------------------------------------------------------------
			case '7':
				// Freesat DAB
				switch(ic_comm_buf[1]){
					// buffer 
					case 'm':
					case 'M':
						freesat_Tx_test = Asc2Bin(ic_comm_buf[3],ic_comm_buf[4]);
						rs232_printf("\n command read => %x\n",freesat_Tx_test);		
						break;
				}
				break;
*/
			default:
				rs232_printf("unknown command\n");
				break;				
	    }
		   
		rs232_printf("RTG>>");   
        ic_start_frame = 0;
        ic_comm_index = 0; 
        ic_comm_use = 0;
        ic_comm_ok = 0;
    }          
    
}
#endif
/**********************************************************
20090818 : eeprom check :  
	min, max, default		: 0x00 ~ 0xae
************************************************************/
const char reconstruct_Table[14+16+4][3] = { 				//20150302

	0x00,MAX_TRIM_VOLUME,TRIM_INITIAL,				// current_key - 0
	0x00,MAX_TRIM_VOLUME,TRIM_INITIAL,				// current_key - 1
	0x00,MAX_TRIM_VOLUME,TRIM_INITIAL,				// current_key - 2
	0x00,MAX_TRIM_VOLUME,TRIM_INITIAL,				// current_key - 3
	0x00,MAX_TEMP_VOLUME,TEMP_INITIAL,				// current_key - 4
	0x00,MAX_TEMP_VOLUME,TEMP_INITIAL,				// current_key - 5

	30,85,FAN_HS_ON_TEMP_INITIAL,			// fan_hs_on_temp			//20140521
	28,85,FAN_HS_OFF_TEMP_INITIAL,			// fan_hs_off_temp			//20140521
	31,86,SHUTDOWN_TEMP_INITIAL,			// shutdown_temp
	31,99,SMPS_SHUTDOWN_TEMP_INITIAL,		// SMPS_shutdown_temp
	30,85,SMPS_FAN_OFF_TEMP_INITIAL,		// smps_fan_on_temp			//20140311
	28,85,SMPS_FAN_ON_TEMP_INITIAL,			// smps_fan_off_temp		//20140311
	30,85,FAN_LS_ON_TEMP_INITIAL,			// fan_ls_on_temp			//20140521
	28,85,FAN_LS_OFF_TEMP_INITIAL,			// fan_ls_off_temp			//20140521
//14
	0,1,AMP_ON_INITIAL,					// amp_on_off[0]
	0,1,AMP_ON_INITIAL,					// amp_on_off[1]
	0,1,AMP_ON_INITIAL,					// amp_on_off[2]
	0,1,AMP_ON_INITIAL,					// amp_on_off[3]

	0,MAX_MASTER_VOLUME,VOLUME_INITIAL,			// volume_Lch[0]
	0,MAX_MASTER_VOLUME,VOLUME_INITIAL,			// volume_Lch[1]
	0,MAX_MASTER_VOLUME,VOLUME_INITIAL,			// volume_Lch[2]
	0,MAX_MASTER_VOLUME,VOLUME_INITIAL,			// volume_Lch[3]

	0,MAX_MASTER_VOLUME,VOLUME_INITIAL,			// volume_Rch[0]
	0,MAX_MASTER_VOLUME,VOLUME_INITIAL,			// volume_Rch[1]
	0,MAX_MASTER_VOLUME,VOLUME_INITIAL,			// volume_Rch[2]
	0,MAX_MASTER_VOLUME,VOLUME_INITIAL,			// volume_Rch[3]

	0,MAX_BALANCE_VOLUME,BALANCE_VOLUME_INITIAL,			// balance[0]
	0,MAX_BALANCE_VOLUME,BALANCE_VOLUME_INITIAL,			// balance[1]
	0,MAX_BALANCE_VOLUME,BALANCE_VOLUME_INITIAL,			// balance[2]
	0,MAX_BALANCE_VOLUME,BALANCE_VOLUME_INITIAL,			// balance[3]
//+16
	0,MAX_INPUT_SEL,INPUT_SEL_INITIAL,				// input_selMode[0]
	0,MAX_INPUT_SEL,INPUT_SEL_INITIAL,				// input_selMode[1]
	0,MAX_INPUT_SEL,INPUT_SEL_INITIAL,				// input_selMode[2]
	0,MAX_INPUT_SEL,INPUT_SEL_INITIAL				// input_selMode[3]
//+4
};
void backup_reconstruct(unsigned char i, unsigned char *addr)
{
//	rs232_printf("i = %x\n",i);					//test : 20070724
	
	if ((*addr >= (unsigned char)reconstruct_Table[i][0]) && (*addr <= (unsigned char)reconstruct_Table[i][1])) {
		return;
	}else{
		*addr = (unsigned char)reconstruct_Table[i][2];
		nvr_xPutData(i,*addr);				//(eeprom addr, data) : 20070724
	//rs232_printf("i = %x\n",i);					//test : 20070724
	//rs232_printf("*addr = %x\n",*addr);
	//rs232_printf("T012 = %x,%x,%x\n",reconstruct_Table[i][0],reconstruct_Table[i][1],reconstruct_Table[i][2]);
	}
}
void backup_reconstruct_singed(unsigned char i, char *addr)
{
//	rs232_printf("i = %x\n",i);					//test : 20070724
	
	if ((*addr >= reconstruct_Table[i][0]) && (*addr <= reconstruct_Table[i][1])) {
		return;
	}else{
		*addr = reconstruct_Table[i][2];
		nvr_xPutData(i,*addr);				//(eeprom addr, data) : 20070724
	//rs232_printf("i = %x\n",i);					//test : 20070724
	//rs232_printf("*addr = %x\n",*addr);
	//rs232_printf("T012 = %x,%x,%x\n",reconstruct_Table[i][0],reconstruct_Table[i][1],reconstruct_Table[i][2]);
	}
}
void backup_reconstruct_Start(void)
{

	backup_reconstruct(0,	&current_key[0]);	
	backup_reconstruct(1,	&current_key[1]);	
	backup_reconstruct(2,	&current_key[2]);	
	backup_reconstruct(3,	&current_key[3]);	
	backup_reconstruct(4,	&current_key[4]);	
	backup_reconstruct(5,	&current_key[5]);

	backup_reconstruct(6,	&fan_hs_on_temp);		//20140521
	backup_reconstruct(7,	&fan_hs_off_temp);		//20140521	
	backup_reconstruct(8,	&shutdown_temp);		//20130916
	backup_reconstruct(9,	&SMPS_shutdown_temp);	//20131024
	backup_reconstruct(10,	&smps_fan_on_temp);		//20140311
	backup_reconstruct(11,	&smps_fan_off_temp);	//20140311
	backup_reconstruct(12,	&fan_ls_on_temp);		//20140521
	backup_reconstruct(13,	&fan_ls_off_temp);		//20140521
//14
	backup_reconstruct(14,	&amp_on_off[0]);		//20140521
	backup_reconstruct(15,	&amp_on_off[1]);		
	backup_reconstruct(16,	&amp_on_off[2]);		
	backup_reconstruct(17,	&amp_on_off[3]);
	backup_reconstruct(18,	&volume_Lch[0]);
	backup_reconstruct(19,	&volume_Lch[1]);
	backup_reconstruct(20,	&volume_Lch[2]);
	backup_reconstruct(21,	&volume_Lch[3]);
	backup_reconstruct(22,	&volume_Rch[0]);
	backup_reconstruct(23,	&volume_Rch[1]);
	backup_reconstruct(24,	&volume_Rch[2]);
	backup_reconstruct(25,	&volume_Rch[3]);
	backup_reconstruct(26,	&balance[0]);
	backup_reconstruct(27,	&balance[1]);
	backup_reconstruct(28,	&balance[2]);
	backup_reconstruct(29,	&balance[3]);
//+16	
	backup_reconstruct(30,	&input_selMode[0]);		//20150302
	backup_reconstruct(31,	&input_selMode[1]);
	backup_reconstruct(32,	&input_selMode[2]);
	backup_reconstruct(33,	&input_selMode[3]);
//+4	
}
/*******************************************************************************
	RKB LED control
	
	Fan always run with power-on, if fan stop, go to Fan-fault AMP Protection mode....
	
	timer : 125msec
	Led_mode	0 :(SKIP)		: do not action
	         	1 :(STOP)
				23:(power-off)	: (1times)	[ Off(500msec) -> On(500msec) ] or [ 90% dimmed-LED blinking 1 time ]
				4 :(power-on)	: (3times)	[ Off(500msec) -> On(500msec) ]
				5 :(Protection)	: (repeatly)[ Off(250msec) -> On(250msec) ]
				x :(Fan fault)	: (repeatly)[ (5times)[ Off(125msec) -> On(125msec) ]  --->off(1sec) ]
				78:(SSM AMP Off): (repeatly)[ during 4sec(on)+125msec(off) ] or [ 50% dimmed-LED blinking 1 time ]
	Led_count_timer		:	0 -> 1 -> (max)
	Led_count_times 	:	3 -> 0
#define StandbyLED(x)           ( (x==1)? GPIO_SetBits(GPIO_LED, STANDBY_LED) : GPIO_ResetBits(GPIO_LED, STANDBY_LED) );

//(Led_mode) function status
#define	LED_MODE_SKIP				0		//20140409
#define	LED_MODE_STOP				1
#define	LED_MODE_POWER_OFF			2		//20140901 : add LED-off time 500msec -> LED_MODE_POWER_OFF_DIM
#define	LED_MODE_POWER_OFF_DIM		3		//20140901 : 90% dimmed-LEd and blinking 1 time
#define	LED_MODE_POWER_ON			4
#define	LED_MODE_PROTECTION			5
#define	LED_MODE_POWER_ON_MUTEOFF	6		//20131216 : from PowerOn to MuteOff blinking
#define	LED_MODE_POWER_ON_SIG_OFF	7		//20140718 : from No signal(after 10minute) blinking( short time off )
#define	LED_MODE_POWER_ON_SIG_DIM	8		//20140901 : from No signal(after 10minute) add LED-Off time 500msec -> LED_MODE_POWER_ON_SIG_DIM1
#define	LED_MODE_POWER_ON_SIG_DIM1	9		//20140901 : from No signal(after 10minute) 50% dimmed-LED and blinking 1 time
********************************************************************************/
void set_LED_mode(unsigned char mod)
{
	Led_mode = mod;
	Led_count_times = 0;
	Led_count_timer = 0;			
}
//--------------------------------------------------------------------
void Led_control_125msec(void)
{
	switch(Led_mode){
        case LED_MODE_POWER_OFF:
        	//Power Off =>
        	//500msec
        	++Led_count_timer;
        	if (Led_count_timer >= COUNT125M_500MS) {
        		Led_count_timer = 0;
#ifdef LED_DIMMER_ENABLE
				//20140901
				//Led control LED_MODE_POWER_OFF_DIM !!!
				Led_mode = LED_MODE_POWER_OFF_DIM;
				//20140225 : timer => directly on/off
				Led_current_status = ON;
#else
				//20140901
				//Led control STOP!!!!!!!!!
				Led_mode = LED_MODE_STOP;
				StandbyLED(1);
#endif
			}
            break;
		//------------------------------------------------------------            
        case LED_MODE_POWER_OFF_DIM:
        	//Power Off => after 500msec => start Dimming
			//20140225 : timer => directly on/off
			Led_current_status = ON;
            break;
		//------------------------------------------------------------            
        case LED_MODE_POWER_ON:
        	//Power On =>
        	//(3times)500msec+500msec
        	++Led_count_timer;
        	if (Led_count_timer >= COUNT125M_1000MS) {
        		Led_count_timer = 0;
        		if(++Led_count_times >= 3) {
        			//Led control STOP!!!!!!!!!
        			Led_mode = LED_MODE_STOP;
#ifdef LED_DIMMER_ENABLE
					//20140225 : timer => directly on/off
					Led_current_status = ON;
#else
        			StandbyLED(1);
#endif
        		}else{
        			//LED off START
#ifdef LED_DIMMER_ENABLE
					//20140225 : timer => directly on/off
					Led_current_status = OFF;
#else
        			//off
        			StandbyLED(0);
#endif
        		}
			}else if (Led_count_timer == COUNT125M_500MS) {
				//LED on START
#ifdef LED_DIMMER_ENABLE
				//20140225 : timer => directly on/off
				Led_current_status = ON;
#else
				//on
				StandbyLED(1);
#endif
			}else if (Led_count_timer ==1) {
				//LED off
#ifdef LED_DIMMER_ENABLE
				//20140225 : timer => directly on/off
				Led_current_status = OFF;
#else
				StandbyLED(0);
#endif
			}
            break;
		//------------------------------------------------------------            
        case LED_MODE_PROTECTION:
        	//AMP Protection On =>
        	//(repeatly)250msec+250msec
        	++Led_count_timer;
        	if (Led_count_timer >= COUNT125M_500MS) {
        		Led_count_timer = 0;
				//LED off START
#ifdef LED_DIMMER_ENABLE
				//20140225 : timer => directly on/off
				Led_current_status = OFF;
#else
        		//off
				StandbyLED(0);
#endif
			}else if (Led_count_timer == COUNT125M_250MS) {
				//LED on START
#ifdef LED_DIMMER_ENABLE
				//20140225 : timer => directly on/off
				Led_current_status = ON;
#else
				//on
				StandbyLED(1);
#endif
			}
            break;
		//------------------------------------------------------------            
        case LED_MODE_POWER_ON_MUTEOFF:					//20131216
        	//Power On => blinking => to MuteOff
        	//(3times)500msec+500msec
        	++Led_count_timer;
        	if (Led_count_timer >= COUNT125M_1000MS) {
        		Led_count_timer = 0;
        		//? check LED STOP => Mute Off
        		if (mode_PowerStatus == POWER_ON0) {
        			//Led control STOP!!!!!!!!!
        			Led_mode = LED_MODE_STOP;
#ifdef LED_DIMMER_ENABLE
					//20140225 : timer => directly on/off
					Led_current_status = ON;
#else
        			StandbyLED(1);
#endif
        		}else{
        			//LED off START
#ifdef LED_DIMMER_ENABLE
					//20140225 : timer => directly on/off
					Led_current_status = OFF;
#else
        			//off
        			StandbyLED(0);
#endif
        		}
			}else if (Led_count_timer == COUNT125M_500MS) {
				//LED on START
#ifdef LED_DIMMER_ENABLE
				//20140225 : timer => directly on/off
				Led_current_status = ON;
#else
				//on
				StandbyLED(1);
#endif
			}else if (Led_count_timer ==1) {
#ifdef LED_DIMMER_ENABLE
				//20140225 : timer => directly on/off
				Led_current_status = OFF;
#else
				//LED off
				StandbyLED(0);
#endif
			}
            break;
		//------------------------------------------------------------            
        case LED_MODE_POWER_ON_SIG_OFF:					//20140728
        	//No signal (after 10minute) => blinking 
        	//(everytime)4sec(on)+125msec(off)
        	++Led_count_timer;
        	if (Led_count_timer >= COUNT125M_4SEC) {
        		Led_count_timer = 0;
        		//? check LED STOP => Mute Off
				if ( (RKB_mode_PP_MP == 1)&&(RKB_mode_PP_nMP2 == 0)&&(GetSignalSensing_nSW == 0) ) {					
					if (signalSensingAMPStatus == ON) {
        				//Led control STOP!!!!!!!!!
        				Led_mode = LED_MODE_STOP;
#ifdef LED_DIMMER_ENABLE
						//20140225 : timer => directly on/off
						Led_current_status = ON;
#else
        				StandbyLED(1);
#endif
        			}else{
        				//LED off START
#ifdef LED_DIMMER_ENABLE
						//20140225 : timer => directly on/off
						Led_current_status = OFF;
#else
        				//off
        				StandbyLED(0);
#endif
        			}
        		}else{
        			//Led control STOP!!!!!!!!!
        			Led_mode = LED_MODE_STOP;
        		}
			}else if (Led_count_timer == COUNT125M_500MS) {
				//LED on START
#ifdef LED_DIMMER_ENABLE
				//20140225 : timer => directly on/off
				Led_current_status = ON;
#else
				//on
				StandbyLED(1);
#endif
			}else if (Led_count_timer ==1) {
#ifdef LED_DIMMER_ENABLE
				//20140225 : timer => directly on/off
				Led_current_status = OFF;
#else
				//LED off=>0n : 125msec off!
				StandbyLED(1);
#endif
			}
            break;
		//------------------------------------------------------------            
        case LED_MODE_POWER_ON_SIG_DIM:
        	//SSM AMP Off =>
        	//500msec
        	++Led_count_timer;
        	if (Led_count_timer >= COUNT125M_500MS) {
        		Led_count_timer = 0;
				//20140901
				//Led control LED_MODE_POWER_ON_SIG_DIM1 !!!
				Led_mode = LED_MODE_POWER_ON_SIG_DIM1;
				//20140225 : timer => directly on/off
				Led_current_status = ON;
			}
            break;
		//------------------------------------------------------------            
        case LED_MODE_POWER_ON_SIG_DIM1:
        	//Power Off => after 500msec => start Dimming
			//20140225 : timer => directly on/off
			Led_current_status = ON;
            break;
		//------------------------------------------------------------            
        case LED_MODE_STOP:
#ifdef LED_DIMMER_ENABLE
			//20140225 : timer => directly on/off
			Led_current_status = ON;
#endif
            break;
		//------------------------------------------------------------            
        default:
        	//LED stop!
        	//LED On       	
            break;
    }
}
	
/* */

/* end of file */
/******************* (C) COPYRIGHT 2008 Rotel Technology Global **************/

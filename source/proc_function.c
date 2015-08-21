#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "initmcu.h"
#include "main.h"
#include "uart.h"
#include "timer.h"

#include "adckey.h"
#include "adckey.ext"

#include "timer.h"
#include "proc_function.h"
#include "function.mem"
#include "proc_display.h"
#include "proc_display.ext"

#include "ak4114.ext"
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
//----------------------------------------------------------
extern void VFDClearDisplay(void);


#ifdef _RKB_DM8100
Channel_TYPE      RKB_Channel[4];
AK4117_TYPE       AK4117_Data[4];
VOL_CTRL_TYPE     VOL_Data[4];
#endif
const char reconstruct_Table[CONSTRUCT_TABLE_SIZE][4] = { 				//20150820

	0x00,MAX_TEMP_VOLUME,TRIM_INITIAL,	ADDR_CURRENT_KEY0,			// current_key - 2
	0x00,MAX_TEMP_VOLUME,TRIM_INITIAL,	ADDR_CURRENT_KEY1,			// current_key - 3
	0x00,MAX_TEMP_VOLUME,TEMP_INITIAL,	ADDR_CURRENT_KEY2			// current_key - 4
	0x00,MAX_TEMP_VOLUME,TEMP_INITIAL,	ADDR_CURRENT_KEY3			// current_key - 5

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
	  RKB_Channel[i].ak4117_data     = &(AK4117_Data[i]);
	  RKB_Channel[i].vol_data        = &(VOL_Data[i]);
	  RKB_Channel[i].digital_nAnalog = 0;
	  RKB_Channel[i].local_speaker_src = 0;
	  RKB_Channel[i].link_channel_src = 0;
	  //func
	  RKB_Channel[i].p_ak4117_func            =  ak4117_ctrl;
	  RKB_Channel[i].p_link_chanel_src_sel    = njw1112_link_speaker_ctrl;
	  RKB_Channel[i].p_local_speaker_src_sel  = njw1112_local_speaker_ctrl;
	  RKB_Channel[i].p_vol_control            = njw1194_vol_ctrl;
	  RKB_Channel[i].p_digital_nAnalog_sw     = njm2752_ctrl;
	  RKB_Channel[i].p_treb_control           = njw1194_treble_ctrl;
	  RKB_Channel[i].p_bass_control           = njw1194_bass_ctrl;
	  RKB_Channel[i].p_balance_contrl         = njw1194_balance_ctrl;
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
		   	    (fan_status.fan_hs_on_amp_temp):(fan_status.fan_on_smps_temp)
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
			  RKB_Channel[i].digital_nAnalog = (u8)nvr_xGetData(ADDR_AMP_A_INPUTSELMODE+i);
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


#if 0
//----------------------------------------------------------
//----------------------------------------------------------
//----------------------------------------------------------
//----------------------------------------------------------
/*
	call POWER OFF standby toggle key
	call rmc seperate power off	
***********************************************************/
void	Func_PowerOff_PortLow(void)
{
        
  		//AMP power off
		if (RKB_mode_PP_MP == 1) {							//? PP unit : 20131029
	    	SMPS_nPowerOn_PC11(0);		//port=LowActive : AMP off : for PP/MP
	    }else{
	    	SMPS_nPowerOn(0);			//port=LowActive : AMP off
	    }
        		
  		//FAN
		FAN_hs_on(0);
		//--------------------------------------------
		//REC-mute : 201312
		//PP : Low active
		//WS : High active
		if (RKB_mode_PP_MP == 1) {							//? PP unit
			preOut_Mute(0);
		}else{
			preOut_Mute(0);
		}
		//--------------------------------------------
		//NJW1159
		NJW1159_lat(0);
		NJW1159_clock(0);
		NJW1159_data(0);
		//analog(0)/digital(1)
		SEL_CH_A_Digital(0);
		SEL_CH_B_Digital(0);
		SEL_CH_C_Digital(0);
		SEL_CH_D_Digital(0);
		//AK4114
		p_4114_reset(0);
		p_4114_do(0);
		p_4114_clk(0);
		p_4114_cs_A(0);
		p_4114_cs_B(0);
		p_4114_cs_C(0);
		p_4114_cs_D(0);
		//--------------------------------------------
		//eeprom
		//20140226
     	p_i2c_sda(0);
     	p_i2c_scl(0);
		
}
void Amp_On(char on)
{
   //on: 1--amp on, 0--amp off
   amp_on(on); 
}
/*
	AMP Power on/off
	//AMP on delay time 이 필요하지안을 까?
//26(17) SMPS Power On : (Low Active) :
#define SMPS_nPowerOn(x)		( (x==1)? GPIO_SetBits(SMPS_PORT, SMPS_nON) : GPIO_ResetBits(SMPS_PORT, SMPS_nON) );
#define SMPS_nPowerOn_PC11(x)	( (x==1)? GPIO_SetBits(SMPS_PORTC, SMPS_nON_PC11) : GPIO_ResetBits(SMPS_PORTC, SMPS_nON_PC11) );
	SMPS_/on(LOW) -->(2.5sec)--> 12V ON : power 가 너무 늦게올라가기 때문에 power relay 와 같이 올려서 시간을 당긴다.
	12V on 후에 all IC data Tx 한다.

*********************************************************************/
void SMPS_On(void)
{
#ifdef AMP_POWERON_ENABLE
#ifdef _RKB_DM8100
       SMPS_nPowerOn(0);                //port=LowActive: AMP on
#else
    //?Amp Portection skip
    if (mode_amp_protect == OFF) {
		//DelayMs(100);           //20121011
		if (RKB_mode_PP_MP == 1) {							//? PP unit : 20131029
	    	SMPS_nPowerOn_PC11(0);		//port=LowActive : AMP off : for PP/MP
	    }else{
	    	SMPS_nPowerOn(0);			//port=LowActive : AMP on
	    }
	}
#endif
#endif
}
//----------------------------------------------------------
void SMPS_Off(void)
{
#ifdef _RKB_DM8100
        SMPS_nPowerOn(1);
#else
#ifdef AMP_POWERON_ENABLE
		if (RKB_mode_PP_MP == 1) {							//? PP unit : 20131029
	    	SMPS_nPowerOn_PC11(1);		//port=LowActive : AMP off : for PP/MP
	    }else{
	    	SMPS_nPowerOn(1);			//port=LowActive : AMP off
	    }
#endif //AMP_POWERON_ENABLE
#endif //_RKB_DM8100
}
/*
	mute on/off		
	muteOff : f_MainMute = 0
*********************************************************************/
void Mute_AllMuteOff(void)
{
	unsigned char i;

    //?Amp Portection skip
    if (mode_amp_protect == OFF) {
	    f_MainMute = 0;
	    //
	    //20131204 : mute off 시, current volume data 를 clear 함으로 인해 volume 이 천천히 올라가는 현상을 적용!
	    //
	}
	//
}
//---------------------------------------------------------------------
void Mute_AllMuteOn(void)
{
    ////test : 20120927
	//--------------------------------------------
	//REC-mute : ON
	//PP : Low active
	//WS : High active
	#ifndef _RKB_DM8100
	if (RKB_mode_PP_MP == 1) {							//? PP unit
		preOut_Mute(0);
	}else{
		preOut_Mute(1);
	}
	#endif
	//--------------------------------------------
	f_MainMute = 1;
	//--------------------------------------------
	//	20131209 : delete
	//--------------------------------------------
	//NJW1159V_AllMuteOn();

}
//---------------------------------------------------------------------
// 20140224 : 
//	call : main Loop
//		DirectMuteOn[0] = 1;
//		DirectMuteOff[0] = 1;
//---------------------------------------------------------------------
void Check_DirectMuteOnOff(void)
{
	unsigned char i;

	if(mode_PowerStatus == POWER_ON0) {
		if ( (RKB_ModelID == RKB_D850) || (RKB_ModelID == RKB_D8100) ) {
			//
			//check mute On!
			//
            for(i=0;i<4;i++){
				if (DirectMuteOn[i] == 1) {
					DirectMuteOn[i] = 0;
					//
					if ( (mute_mode == MUTE_OFF) && (f_MainMute == 0) ) {      //20130322
						NJW1159V_AmpMuteOn(i);
					}
				}
			}
			//
			//check mute Off!	
			//		
            for(i=0;i<4;i++){
				if (DirectMuteOff[i] == 1) {			
					DirectMuteOff[i] = 0;
					//
					//20140401 : 
					if (timer_MuteOff == 0) {
						if ( (mute_mode == MUTE_OFF) && (f_MainMute == 0) ) {      //20130322
							Mute_SetMuteOffTime(2);
							Mute_CheckMuteOff_100msec();
						}
					}		
				}
			}
		}
	}
}
/*
	standby relay off		
*********************************************************************/
void	StandbyRelay_Off(void)
{
	SMPS_Off();			//SMPS off
	Standby(0);			//stanby power OFF
#ifdef POWER_CONSUMPTION	//20121030
	SetConfigUART2n3ToTelechipsReciva(0);
#endif			
}
/*
	standby relay on :
	
	1. before add delay : 1500msec
	2. relay on
	3. after add delay : 100msec
	
*********************************************************************/
void	StandbyRelay_On(void)
{
	//		
	Standby(1);						//stanby power ON
	//
#ifdef POWER_ON_ADDDELAY_BEFORE_RELAYON		//20140106
	DelayMs(1500);
#endif
#ifdef POWER_CONSUMPTION			//20121030
		DelayMs(100);
		SetConfigUART2n3ToTelechipsReciva(1); //set uart4,5 enable 20150811
#endif			
}
/*
	NJW1159V mute on
*********************************************************************/
//RKB
void NJW1159V_AllMuteOn(void)
{
	unsigned char i;
	//njw1159v mute
	//Set_MainVolume(vol,ampType) => L/R Tx
	for (i=0;i<4;i++) {
		//Set_MainVolume(vol,ampType) => L/R Tx
		Set_MainVolume(0,i);					//mute=0
	}
}
//----------------------------------------------------------
void NJW1159V_AmpMuteOn(unsigned char typ)
{
	//njw1159v mute
	//Set_MainVolume(vol,ampType) => L/R Tx
	Set_MainVolume(0,typ);						//mute=0
}
/*
    //volume power on initial
        // load PwrOn_volume
        AMP_volume = PwrOn_volume;
#define	MAX_MASTER_VOLUME			96		//20100415
#define	VOLUME_INITIAL				45		//20100415
        // load Unity gain
        //checking Unity volume : AMP_volume
        //AUX1, COAX1, OPT1, USB
        
*********************************************************************/
//RKB
void	Level_Initial(unsigned char vol)
{
	unsigned char i;
	for (i=0;i<6;i++) {
		current_key[i] = vol;			//mute=0
	}
}

/*
		BOOT_AC, BOOT_NORMAL => power on	


		20140224 : IIC_SCL(PB6) : now GPIO_Mode_IN_FLOATING ; ==> GPIO_Mode_Out_PP
		So, 이전의 eeprom rading is ERROR!
***********************************************************/
void	PowerOn_StandBy(unsigned char boot)
{
	unsigned char dummy;
	unsigned char i;

	StandbyLED(1);				//20131213 : front LED on
	//20131022 : power off=>on , 가끔 DAC reset 이 안걸리기때문에 미리 올려놓고 아래서 reset 을 함으로 수정
	//20140224 : direct mute off
    for(i=0;i<4;i++){
		DirectMuteOn[i] = 0;
		DirectMuteOff[i] = 0;
	}
	//20140804 : Off [ s/w center ]
	current_SignalSensingMode = 0;
	//20140521
	FAN_HS_mode = FAN_READY;
	FAN_LS_mode = FAN_READY;

	//protection mode clear!!!!
    mode_amp_protect = OFF;   
    Amp_protectionTime = 0;		//AMP.RDY port check : level check time
    count_delay_AMPProtectionCheck = TIME10MSTO5SEC;    //20121012
	//----------------------------------------------------------------	
	// reset delay time : volume data Tx
    //20131219
	time_delay_volume_Tx = 0;     //default(0)									
	//----------------------------------------------------------------
	//20140226 : eeprom port initialize
	iic_Init();
    //==========================================================================
	if(boot == BOOT_NORMAL) {
	    //Boot Normal
		Mute_AllMuteOn();
  		//----------------------------------------------------------------
  		//	mode_AHB_prescaler
  		//	1=36MHz, 2=18MHz, 4=9MHz, 8=4.5MHz
  		// 20140409
  		//----------------------------------------------------------------
		mode_AHB_prescaler = 1;
		//
		//
  		RCC_Configuration(); 		//20140403  
  		GPIO_Configuration(1);			//20121030 : normal(1)<==== option port reading ????
		UART_Configuration();
		//ADC_Configuration(0);		//20110719 : power_consumption(0)
		TIM1_Configuration();		//(250usec)
//20110824
#ifdef TIMER2_ENABLE
		TIM2_Configuration();
#endif
		SYSTICK_Configuration();	//(1msec)
  		//----------------------------------------------------------------
		//----------------------------------------------------------------

		//----------------------------------------------------------------	
		if (RKB_mode_PP_MP == 0) {							//?(pin53:PC12): WS1/2=0, PP=1(add SMPS_sensor_1/2) : 20131024
  	    	GPIO_Configuration(1);      //20121030 : normal(1)
  	    }else{
			if (RKB_mode_PP_nMP2 == 0) {	//20140523 : add signal sensing feature
				GPIO_Configuration_MP2(1);
			}else{
  	    		GPIO_Configuration_PP(1);	//20131024 : normal(1)
  	    	}
  	    }

		//eeprom dummy :
		dummy = nvr_xGetData(ADDR_DUMMYDUMMY);	//page0 setting!!!!!!!!!
		dummy = nvr_xGetData(ADDR_DUMMYDUMMY);	//page0 setting!!!!!!!!!
		dummy = nvr_xGetData(ADDR_DUMMYDUMMY);	//page0 setting!!!!!!!!!
		//----------------------------------------------------------------
		//after PowerOff exec -> (delay time) -> (relay off) -> set delay start power consumption	
	    count_powerOffDelay = 0;				//20120927
		//----------------------------------------------------------------
		//20140104 : before add delay time : 1500msec+100msec
		StandbyRelay_On();		//add 100msec delay time
		//12V Out = On
		//T12VOut(1);
		//----------------------------------------------------------------	
		//SMPS_/on-(2.7sec)->12V on
		SMPS_On();						//20140219
		#ifdef _RKB_DM8100
		if(GetSignalSensing_nSW == 1)
			{
                if(GetSignalSense == 1)   //SS_ON--H: signal in,L: no signal
                	{
                      Amp_On(1);
				    }
				else
					{
                      Amp_On(0);
				    }
		    }
		else
			{
		         Amp_On(1);             
			}
		#endif
		adc_KeyInit();					//20131204
		Key_Init();

		//ADC resetting :
		#ifdef _RKB_DM8100
		ADC_DeInit();
		#else
		ADC_DeInit(ADC1);
		#endif
		//20111214
		UART_Configuration(); 	//power consumption
		
		mute_mode = MUTE_OFF;			//20090804
		//VFD all on flag clear!
		f_initialize = OFF;				//20090619
		//---------------------------------------------------
		//---------------------------------------------------
		//전기공급으로인한 delay
		//DelayMs(10);
	///===================================================================			
	}else{	
	///===================================================================			
		//Boot AC 
		//RDG push power on => reset 여기로 진행
		//RCX reset => power key on 최초에만 여기로 진행 
		Mute_AllMuteOn();

	    count_powerOffDelay = 0;				//20120927
		//----------------------------------------------------------------	
		//20140104 : before add delay time : 1500msec+100msec
		StandbyRelay_On();				//add 100msec delay time
		//12V Out = On
		//T12VOut(1);
		//----------------------------------------------------------------	
		//SMPS_/on-(2.4sec)->12V on
		SMPS_On();						//20140219
		#ifdef _RKB_DM8100
		if(GetSignalSensing_nSW == 1)
			{
                if(GetSignalSense == 1)   //SS_ON--H: signal in,L: no signal
                	{
                      Amp_On(1);
				    }
				else
					{
                      Amp_On(0);
				    }
		    }
		else
			{
		         Amp_On(1);             
			}
		#endif		
		adc_KeyInit();					//20131204
		Key_Init();

		//ADC resetting :
		#ifdef _RKB_DM8100
		ADC_DeInit();
		#else
   		ADC_DeInit(ADC1);
        #endif
		mute_mode = MUTE_OFF;	//20090804
		//VFD all on flag clear!
		//f_initialize = OFF;				//20091120 : f_initialize 건드리면 안됨 ...
		//---------------------------------------------------
		//---------------------------------------------------
		//전기공급으로인한 delay
		//DelayMs(10);
	}
	///===================================================================			
	Set_DisplayBlockCTimer(BLOCKC_NORMAL,0);
	//power sequence Next : 20131217
	Power_SetPowerStatus(POWER_ON0_ING);
	count_PowerOn1Ing = TIME20MSTO1600MS;		//20140107 : 1600msec
	// stby led on
#ifdef LED_DIMMER_ENABLE
	//20140115 : LED dimmer data reading
	LED_Dimmer_value = nvr_xGetData(ADDR_LED_DIMMER);			
#endif
	//LED mode = LED_MODE_POWER_ON
	set_LED_mode(LED_MODE_POWER_ON);
#ifdef LED_DIMMER_ENABLE
	//20140225 : timer => directly on/off
	Led_current_status = OFF;
#endif
	StandbyLED(0);
}
//------------------------------------------------------------------------------
//	after 1600msec
//------------------------------------------------------------------------------
void	PowerOn_exec_1(void)
{
	unsigned char i;

	//20131022 : power off=>on , 가끔 DAC reset 이 안걸리기때문에 미리 올려놓고 아래서 reset 을 함으로 수정
	S_4114_RESET(P_HIGH);		//20131213 : DAC dsd1791 공용 사용
	//20140521
	FAN_HS_mode = FAN_READY;
	FAN_LS_mode = FAN_READY;

	//protection mode clear!!!!
    mode_amp_protect = OFF;   
    Amp_protectionTime = 0;		//AMP.RDY port check : level check time
    count_delay_AMPProtectionCheck = TIME10MSTO5SEC;    //20121012
	//----------------------------------------------------------------	
	// reset delay time : volume data Tx
    //20131219
	time_delay_volume_Tx = 0;     //default(0)									
    //==========================================================================
		//----------------------------------------------------------------	
	    count_powerOffDelay = 0;				//20120927
		//----------------------------------------------------------------	
		if (RKB_mode_PP_MP == 0) {							//?(pin53:PC12): WS1/2=0, PP=1(add SMPS_sensor_1/2) : 20131024
			ADC_Configuration(1);
		}else{
			ADC_Configuration_PP(1);		//20131024
		}
		mute_mode = MUTE_OFF;			//20090804
		//---------------------------------------------------
	///===================================================================			
}
//------------------------------------------------------------------------------
//	after 7000msec
//------------------------------------------------------------------------------
void	PowerOn_exec_DIR(void)
{
	unsigned char i;
	///===================================================================
	//	power sequence : fist call
	//	need delay time : power stable time : ( 1.0sec )
	///===================================================================
	//	power sequence : second call
	//	need delay time : power stable time : ( 1.6sec + 500msec )
	///===================================================================
	//20131022 : power off=>on , 가끔 DAC reset 이 안걸리기때문에 미리 올려놓고 아래서 reset 을 함으로 수정
	S_4114_RESET(P_HIGH);		//20131022 : DAC dsd1791 공용 사용
    //전기공급으로인한 delay add
    //DelayMs(1600+500);
	//---------------------------------------------------
	//20131209 : add mute on : 전기 공급 지연으로 다시한번 volume IC mute on
	Mute_AllMuteOn();
		//20131209 : delete mute Volume data Tx :
		//NJW1159V_AllMuteOn();
	//---------------------------------------------------
    //DIR AK4114 RAM, reset port initialize
	DIR_4114_Reset();			    //4114 reset and DAC reset
    for(i=0;i<4;i++){
        b_4114Input[i] = I_COAX1;         	//RX0=I_COAX1
	    b_4114Mode[i] = AK4114_FREQSET;
	    b_4114Status[i] = 0;        		//initial start , ( INIT_OK == 0xF0 )
		b_ModeStatus[i] = 0x22;				//bit( 0x20 <= f_PllCheckOn ) ( 0x02 <= f_InputChg )
		b_StreamFreq_bk[i] = 7;				//default=7(off) : backup save freq informatiion		
		//20130708 add
		b_PllStatus[i] = PLL_UNLOCK;			//Lock(0),Unlock(1),Checking(2)	//20130708 ( != PLL_UNLOCK )
		b_PllStatus_bk[i] = PLL_CHECKING;		//20130708
		b_StreamKind[i] = ST_CHECKING;			//ST_CHECKING(7)		
		b_PllCheckTime[i] = TIME10MSTO40MS;		//20130104 : 40msec => 500msec => 40msec
	}
	//---------------------------------------------------
	//20140108
	Set_Function();									//RKB : A/D switching : Analog(0) set
}
//------------------------------------------------------------------------------
//	after 120msec
//------------------------------------------------------------------------------
void	PowerOn_exec_2(void)
{
	unsigned char i,j;
	///===================================================================
	//	power sequence : fist call
	//	need delay time : power stable time : ( 1.0sec )
	///===================================================================
	//	power sequence : second call
	//	need delay time : power stable time : ( 1.6sec + 500msec )
	///===================================================================
	//20131022 : power off=>on , 가끔 DAC reset 이 안걸리기때문에 미리 올려놓고 아래서 reset 을 함으로 수정
	//20140106 S_4114_RESET(P_HIGH);		//20131022 : DAC dsd1791 공용 사용
    //전기공급으로인한 delay add
    //DelayMs(1600+500);
	//---------------------------------------------------
	//20131209 : add mute on : 전기 공급 지연으로 다시한번 volume IC mute on
	Mute_AllMuteOn();
		//20131209 : delete mute Volume data Tx :
		//NJW1159V_AllMuteOn();
	//---------------------------------------------------
/* 20140106
    //DIR AK4114 RAM, reset port initialize
	DIR_4114_Reset();			    //4114 reset and DAC reset
    for(i=0;i<4;i++){
        b_4114Input[i] = I_COAX1;         	//RX0=I_COAX1
	    b_4114Mode[i] = AK4114_FREQSET;
	    b_4114Status[i] = 0;        		//initial start , ( INIT_OK == 0xF0 )
		b_ModeStatus[i] = 0x22;				//bit( 0x20 <= f_PllCheckOn ) ( 0x02 <= f_InputChg )
		b_StreamFreq_bk[i] = 7;				//default=7(off) : backup save freq informatiion		
		//20130708 add
		b_PllStatus[i] = PLL_UNLOCK;			//Lock(0),Unlock(1),Checking(2)	//20130708 ( != PLL_UNLOCK )
		b_PllStatus_bk[i] = PLL_CHECKING;		//20130708
		b_StreamKind[i] = ST_CHECKING;			//ST_CHECKING(7)		
		b_PllCheckTime[i] = TIME10MSTO40MS;		//20130104 : 40msec => 500msec => 40msec
	}
*/	
	//---------------------------------------------------
	//Set_Function();									//RKB : A/D switching : Analog(0) set
	b_StreamDisplayDelay = TIME20MSTO1000MS;        //20130521
	//---------------------------------------------------
    //volume power on initial
	Level_Initial(0);			//mute=0	
	//---------------------------------------------------
	//	//Tx(addr,vol)
	//	Set_MainVolume(i,current_key[i]);
	//---------------------------------------------------
	Set_DisplayBlockCTimer(BLOCKC_NORMAL,0);
	//mute off time set!
	//Mute_SetMuteOffTime(COUNT100M_800MS);
	
//#ifdef VOLUME_DATA_TX_x
//20140401
		//850,8100 		: volume Tx
		//D850, D8100 	: AK4117 check (add delay time) A/D switching and then volume Tx
		//power on : volume data Tx : (one time)
    	//power on -> one time Tx
	//--------------------------------------------------------------------------
		//20140404
		#ifdef RS232_VOL_ENABLE_IS_TRIMMIN			//used
			//? All TRIM min(0)
			j=0;
			for (i=0;i<4;i++) {
				if (key.KeyData[i] == 0) ++j;
			}
			//
			if (j == 4) {
				//All TRIM = min(0)!!! 
				//front knob disable!
				front_trimKnob_enable = OFF;
			}else{
				//All TRIM <> min(0)!!! 
				//front knob enable!
				front_trimKnob_enable = ON;
			}
		#endif						
			//20140218
			//(? Front trim and L/R_ch )
			if (front_trimKnob_enable == OFF) {
				//
				//front knob disable!
				//
				//RS232 L/R-ch volume data Tx
				for (i=0;i<4;i++) {
					if (amp_on_off[i] == 1) {
						//? check mute key
						//20140224
						if (ch_muteOn[i] == MUTE_OFF) {
							//mute off ->volume data Tx : 
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
							Func_VolumeLchStatus(i);
							Func_VolumeRchStatus(i);
						}
					}else{
						//mute
						Set_MainVolume(0,i);
					}
				}
				//							
			}else{
				//
				//front knob enable!
				//
				//front trimKnob read and Tx
				for (i=0;i<4;i++) {
					if (amp_on_off[i] == 1) {
						//? check mute key
						//20140224
						if (ch_muteOn[i] == MUTE_OFF) {
							key_target[i] = key.KeyData[i];
							current_key[i] = key_target[i];
							//Set_MainVolume(vol,ampType) => L/R Tx
							Set_MainVolume(current_key[i],i);
							//RS232 Tx
							Func_AmpTrimCurrentStatus(i);
						}
					}else{
						//mute
						Set_MainVolume(0,i);
					}
				}
			}
	//--------------------------------------------------------------------------
		//20140404
		#ifdef RS232_VOL_ENABLE_IS_TRIMMIN			//used
			//? All TRIM min(0)
			j=0;
			for (i=0;i<4;i++) {
				if (key.KeyData[i] == 0) ++j;
			}
			//
			if (j == 4) {
				//All TRIM = min(0)!!! 
				//front knob disable!
				front_trimKnob_enable = OFF;
			}else{
				//All TRIM <> min(0)!!! 
				//front knob enable!
				front_trimKnob_enable = ON;
			}
		#endif						
			//20140218
			//(? Front trim and L/R_ch )
			if (front_trimKnob_enable == OFF) {
				//
				//front knob disable!
				//
				//RS232 L/R-ch volume data Tx
				for (i=0;i<4;i++) {
					if (amp_on_off[i] == 1) {
						//? check mute key
						//20140224
						if (ch_muteOn[i] == MUTE_OFF) {
							//mute off ->volume data Tx : 
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
							Func_VolumeLchStatus(i);
							Func_VolumeRchStatus(i);
						}
					}else{
						//mute
						Set_MainVolume(0,i);
					}
				}
				//							
			}else{
				//
				//front knob enable!
				//
				//front trimKnob read and Tx
				for (i=0;i<4;i++) {
					if (amp_on_off[i] == 1) {
						//? check mute key
						//20140224
						if (ch_muteOn[i] == MUTE_OFF) {
							key_target[i] = key.KeyData[i];
							current_key[i] = key_target[i];
							//Set_MainVolume(vol,ampType) => L/R Tx
							Set_MainVolume(current_key[i],i);
							//RS232 Tx
							Func_AmpTrimCurrentStatus(i);
						}
					}else{
						//mute
						Set_MainVolume(0,i);
					}
				}
			}
	//--------------------------------------------------------------------------
//#endif	
}
//------------------------------------------------------------------------------
//	not used
//------------------------------------------------------------------------------
void	PowerOn_exec_3(void)
{
	unsigned char i;
	
	
}
/* 	RKB 12V trigger off
	RKB use
	power AC on후에 => (check Trigger off ) => standby on 
***********************************************************/
void	Func_PowerStandByOn(void)
{
	unsigned int i,j;		// 20131216

	Mute_AllMuteOn();
	//20131218 : add delay time : need Relay off time
	DelayMs(500);
	//20131209 : mute Volume data Tx :
	NJW1159V_AllMuteOn();

//--------------------------------------
	mode_func = FUNCTION_DISP_OFF;
	Display_PowerAllBufferClear();			//20110715
	//Display_FrontDispBufferClear();
	
	Set_DisplayBlockCTimer(BLOCKC_NORMAL,0);
	
	Power_SetPowerStatus(POWER_OFF);
	//20131219
	count_PowerOn1Ing = 0;
	//main_PowerDownCheckSumSave();			//eeprom writing!

	//VFD all on flag clear!
	f_initialize = OFF;			//20090619

	// stby led off ->on
#ifdef LED_DIMMER_ENABLE
	//20140115 : LED dimmer data reading
	LED_Dimmer_value = nvr_xGetData(ADDR_LED_DIMMER);			
#endif
	//LED mode = LED_MODE_POWER_OFF
	set_LED_mode(LED_MODE_POWER_OFF);
#ifdef LED_DIMMER_ENABLE
	//20140225 : timer => directly on/off
	Led_current_status = OFF;
#endif
	StandbyLED(0);
	
    //20131216 : standby relay off [ AMP_off(), Standby(0) ]-->(delay time)-> All Port Low
    // (3000msec)->(2400msec) ->(0)AllPortLow
	StandbyRelay_Off();			//20131216
	count_powerOffDelay = TIME20MSTO2500MS;			//20140108
	//12V Out = Off
	//T12VOut(0);
#ifdef ASCiiKEY
	Func_PowerStatus();     //20130709
	//20140219
	//all CH status
	Func_AMP_CH_status(4);
#endif

  	for(j=0;j<2;j++){;			
  		for(i=0;i<60000;i++);	
  	}

	//ADC resetting : disable
	ADC_DeInit(ADC1);
	if (RKB_mode_PP_MP == 0) {							//?(pin53:PC12): WS1/2=0, PP=1(add SMPS_sensor_1/2) : 20131024
		ADC_Configuration(0);
	}else{
		ADC_Configuration_PP(0);			//20131024
	}

	//20130710 : stanby 시 port 정리..
	//Func_PowerOff_PortLow();
}


/*
	reset에 의한 power on

	mode_Power : 	power on option.
			0=POWER_DIRECT
			1=POWER_STANDBY
			2=POWER_ALWAYSON
			
	GetT12V_IN		L		H
					|		|_power on
					|_standby on
	
***********************************************************/
void	Func_PowerAcOn(void)
{	
	unsigned int i,j;

	//20100316 : rs232 pressed initialize
	Rs232_pressed_key = 0xff;
	Rs232_pressed_count = 0;
	//20110719 : ASCii pressed initialize
	ASCii_pressed_key = 0xff;
	ASCii_pressed_count = 0;
		
	switch(mode_Power)
	{
		case POWER_ALWAYSON:
		case POWER_DIRECT:
		case POWER_STANDBY:
		default:									
			//------------------------------------------------
			//------------------------------------------------
			//------------------------------------------------			
			mode_Power = POWER_STANDBY;						
#if 0
			//20090820 : RDG1520,RT09 AC on=> power on
			if (GetT12V_IN == 1) {
				trigger_status = 1;				//save!
				f_trigger_in = 1;				//20130621
			}else{
				trigger_status = 0;				//save!
				f_trigger_in = 0;				//20130621
			}
			// power on
			PowerOn_StandBy(BOOT_AC);
#else
			//20140523 : check signal sensing feature
			#ifdef _RKB_DM8100 //ss_mode: H: ss quick mode, L: ss quick mode
			      // power on
			      PowerOn_StandBy(BOOT_AC);
			      //========================================================
			      RS232_TxSkip = 0;			
			      Func_PowerOnCommand_Tx();		        
			      //uart Tx
			      uart1_TxData();			//RS232 TX
			      //========================================================
                  #ifdef ASCiiKEY
			      Func_PowerStatus();     //20131025
			      //20140219
			      //all CH status
			      Func_AMP_CH_status(4);
                  #endif						
			#else
			if ( (RKB_mode_PP_nMP2 == 0)&&(GetSignalSensing_nSW == 0) ) {	//when PowerAC-on
					//all CH on <= signal sensing mode
					//
					signalSensingAMPStatus = ON;
					//reset counter
					signal_sensing_counter = 0;
					//all CH on saving
					for (i=0;i<4;i++)
					{
						//20140819 
						amp_on_off[i] = 1;
						eepromAddr = ADDR_AMP_A_ONOFF+i;					//20140224
						nvr_xPutData(eepromAddr,amp_on_off[i]);	
					}			
					//-> power-on
					PowerOn_StandBy(BOOT_AC);
					//========================================================
					RS232_TxSkip = 0;			
					Func_PowerOnCommand_Tx();		        
					//uart Tx
					uart1_TxData();			//RS232 TX
					//========================================================
#ifdef ASCiiKEY
					Func_PowerStatus();     //20131025
					//20140219
					//all CH status
					Func_AMP_CH_status(4);
#endif
			}else{
				//20140219 : RKB series => check 12VTrigger =>(Low)=>got stnadby on
				if (GetT12V_IN == 1) {
					trigger_status = 1;				//save!
					f_trigger_in = 1;				//20130621
					//? All CH OFF -> skip!
					j=0;
					for (i=0;i<4;i++) {
						if (amp_on_off[i] == 0) ++j;
					}
					if (j == 4) {
						//Now ALL CH OFF!!!!
						//standby on
						//add power-status and AMP ch-status
						Func_PowerStandByOn();	
					}else{
						// power on
						PowerOn_StandBy(BOOT_AC);
						//========================================================
						RS232_TxSkip = 0;			
						Func_PowerOnCommand_Tx();		        
						//uart Tx
						uart1_TxData();			//RS232 TX
						//========================================================
#ifdef ASCiiKEY
						Func_PowerStatus();     //20131025
						//20140219
						//all CH status
						Func_AMP_CH_status(4);
#endif
					}
				}else{
					//SWitch 상단일때 조건 : ( Low )
					//standby on
					//add power-status and AMP ch-status
					Func_PowerStandByOn();
					Power_SetPowerStatus(POWER_OFF);
					trigger_status = 0;				//save!
					f_trigger_in = 0;				//20130621
				}
			}
#endif	//20090820
#endif // RKB_DM8100
//------------------------------------------------
            break;
	}
}



/*
	call POWER OFF standby toggle key
	call rmc seperate power off	
***********************************************************/
void	Func_PowerOff(void)
{
	unsigned int i,j;		// 20060110

	Mute_AllMuteOn();
	//20131218 : add delay time : need Relay off time
	DelayMs(500);
	//20131209 : mute Volume data Tx :
	NJW1159V_AllMuteOn();

//--------------------------------------
	mode_func = FUNCTION_DISP_OFF;	
	Display_PowerAllBufferClear();          //20120927
	//f_tx_disp_buff = 0x03;					//RS232 TX : VFD Line 1~2 : 20130709
	//Display_FrontDispBufferClear();
	
	Set_DisplayBlockCTimer(BLOCKC_NORMAL,0);
	
	Power_SetPowerStatus(POWER_OFF);
	//20131219
	count_PowerOn1Ing = 0;
	//main_PowerDownCheckSumSave();			//eeprom writing!

	//VFD all on flag clear!
	f_initialize = OFF;			//20090619

#ifdef OPTICAL_POWER_ENABLE
	//20140516 : 강제로 off 할 경우, 현재 optical on/off 를 읽어들여 backup한다....
	opticalpower_bk = nOverLoadLowLevelCheck;
	count_opticalPower = 0;
#endif

#ifdef LED_DIMMER_ENABLE
	//20140225 : LED dimmer data reading 
	//LED_Dimmer_value = nvr_xGetData(ADDR_LED_DIMMER);			
#endif

	// stby led off ->on
	//LED mode = LED_MODE_POWER_OFF
	set_LED_mode(LED_MODE_POWER_OFF);
#ifdef LED_DIMMER_ENABLE
	//20140225 : timer => directly on/off
	Led_current_status = OFF;
#endif
	StandbyLED(0);

    //20131216 : standby relay off [ AMP_off(), Standby(0) ]-->(delay time)-> All Port Low
    // (2500msec)->(2400msec) ->(0)AllPortLow
	StandbyRelay_Off();			//20131216
	count_powerOffDelay = TIME20MSTO2500MS;			//20140108
	//12V Out = Off
	//T12VOut(0);
    //    
#ifdef ASCiiKEY
	Func_PowerStatus();     //20130709
#endif
  	for(j=0;j<2;j++){;			
  		for(i=0;i<60000;i++);	
  	}
	//ADC resetting : disable
	ADC_DeInit(ADC1);
	if (RKB_mode_PP_MP == 0) {							//?(pin53:PC12): WS1/2=0, PP=1(add SMPS_sensor_1/2) : 20131024
		ADC_Configuration(0);
	}else{
		ADC_Configuration_PP(0);		//20131024
	}
/*	if (mode_STMPower == STM_POWER_NORMAL) {		//20090818
		//20091015 : stanby 시 port 정리..
		Func_PowerOff_PortLow();
	}*/
}

/*
	call power on standby toggle key
	mode_Power : 	power on option.
			0=POWER_DIRECT
			1=POWER_STANDBY
			2=POWER_ALWAYSON
***********************************************************/
void	Func_PowerOn(void)
{
	switch(mode_Power) {
		case POWER_STANDBY:
		case POWER_DIRECT:
			PowerOn_StandBy(BOOT_NORMAL);
			Display_PowerOffClearTimer();
			break;
		case POWER_STANDBY1:
			mode_Power = POWER_STANDBY;						
			PowerOn_StandBy(BOOT_AC);			//20090917 : POWER_STANDBY1
			Display_PowerOffClearTimer();
			break;
		case POWER_ALWAYSON:			
		default:
			break;
	}
}
/*
	call power(standby) toggle
***********************************************************/
void	Power_toggle(void)			//POWER_STANDBY & POWER_DIRECT
{
	unsigned char i,j;

	switch(mode_PowerStatus) {
		case POWER_ON0:			
			Func_PowerOff();
			break;
		case POWER_OFF:
			//? All CH OFF -> skip!
			j=0;
			for (i=0;i<4;i++) {
				if (amp_on_off[i] == 0) ++j;
			}
			if (j == 4) {
				//Now ALL CH OFF!!!!
				return;
			}
			//goto power-on!
			Func_PowerOn();
			//
			break;
		default:	break;
	}
}
/*
	사용안함.
	call power(AlwaysOn) 
***********************************************************/
void	Power_toggleAlwaysOn(void)		//POWER_ALWAYSON
{
	switch(mode_PowerStatus) {
		case POWER_ON0:
			break;
		case POWER_OFF:
			Func_PowerOn();
			break;
		default:	break;
	}
}
/*
	KEY : power(standby) toggle key
	
	20140523 : check signal sensing feature
	
	mode_Power : 	power on option.
			0=POWER_DIRECT
			1=POWER_STANDBY
			2=POWER_ALWAYSON
***********************************************************/
void 	Func_PowerControl(void)
{
	//? PP & MP2 unit & signal sensing nSW
	//20140523
	if ( (RKB_mode_PP_MP == 1)&&(RKB_mode_PP_nMP2 == 0)&&(GetSignalSensing_nSW == 0) ) return;						

	switch(mode_Power) {
		case POWER_STANDBY:
		case POWER_DIRECT:
		case POWER_STANDBY1:				//20090917
			Power_toggle();
			break;
		case POWER_ALWAYSON:
//			Power_toggleAlwaysOn();
			break;
		default:
			break;
	}
#ifdef ASCiiKEY
    Func_PowerStatus();     //20110317
	//20140219
	if (mode_PowerStatus == OFF) return;	
	//all CH status
	Func_AMP_CH_status(4);
#endif
}
/*
	KEY of rmc/rs232 seperate power on
	
	20140523 : check signal sensing feature

	mode_Power : 	power on option.
			0=POWER_DIRECT
			1=POWER_STANDBY
			2=POWER_ALWAYSON
***********************************************************/
void	Func_SeperatePowerOn(void)
{
	unsigned char i,j;

	//? PP & MP2 unit & signal sensing nSW
	//20140523
	if ( (RKB_mode_PP_MP == 1)&&(RKB_mode_PP_nMP2 == 0)&&(GetSignalSensing_nSW == 0) ) return;						

	switch(mode_Power) {
		case POWER_STANDBY:
		case POWER_DIRECT:
		case POWER_ALWAYSON:
		default:
			switch(mode_PowerStatus) {
				case POWER_OFF:
					//? All CH OFF -> skip!
					j=0;
					for (i=0;i<4;i++) {
						if (amp_on_off[i] == 0) ++j;
					}
					if (j == 4) {
						//Now ALL CH OFF!!!!
					}else{
						PowerOn_StandBy(BOOT_NORMAL);
					}
#ifdef ASCiiKEY
                    Func_PowerStatus();     //20110317
					//20140219
					//all CH status
					Func_AMP_CH_status(4);
#endif
					break;		
				case POWER_ON0:
					break;		
				default:
					break;
			}
			break;
	}
}
/*
	KEY of rmc/rs232 seperate power off	
	
	20140523 : check signal sensing feature

	mode_Power : 	power on option.
			0=POWER_DIRECT
			1=POWER_STANDBY
			2=POWER_ALWAYSON
***********************************************************/
void	Func_SeperatePowerOff(void)
{
	//? PP & MP2 unit & signal sensing nSW
	//20140523
	if ( (RKB_mode_PP_MP == 1)&&(RKB_mode_PP_nMP2 == 0)&&(GetSignalSensing_nSW == 0) ) return;						

	switch(mode_Power) {
		case POWER_STANDBY:
		case POWER_DIRECT:
		case POWER_ALWAYSON:
		default:
			Func_PowerOff();		//20030225
			break;
	}
}
//====================================================================
//====================================================================
/*
	p_trigger_jack 에의한 power on [100msec]

	mode_Power : 	power on option.
			0=POWER_DIRECT
			1=POWER_STANDBY
			2=POWER_ALWAYSON
			
	p_trigger_jack		L		H		H
	p_trigger_in		L		L		H
						|		|		|_power on
						|		|_standby on
	                  	|_standby key

20140703 : signalSensingAMPStatus = ON;
	
***********************************************************/
void	Func_TriggerJackCheck_Power(void)
{
/*

	ENCODER Main VOLUME Chat	[ 2msec timer ]
		
             B_PORTD_YREG [ xxxx **xx ]
             B_PORTD_OLD  [ xxxx **xx ]
             B_PORTD_DATA [ xxxx **xx ]
                                 ||____ f_encoder_vol2 [f_en_mvol_dn]
                                 |_____ f_encoder_vol1 [f_en_mvol_up]
             
             B_PORTC_YREG [ xx** xxxx ]
             B_PORTC_OLD  [ xx** xxxx ]
             B_PORTC_DATA [ xx** xxxx ]
                              ||____ f_encoder_func1 [f_en_func_up]
                              |_____ f_encoder_func2 [f_en_func_dn]

volatile word W_T12V_OLD;
volatile word_flags W_T12V_DATA;
#define	f_trigger_in	W_T12V_DATA.word_bit.bit12
//for 12V trigger input
#define T12V_PORT				GPIOA
#define T12V_IN					GPIO_Pin_12         //71(45) : input
#define GetT12V_IN				GPIO_ReadInputDataBit(T12V_PORT, T12V_IN)
*/
//
	unsigned char i,j;
	unsigned char status;
	word getinput,VolInputTemp;
	
	// check pin12 : RKB : PA12
	getinput = GPIO_ReadInputData(T12V_PORT)&0x1000;	//20130517
	VolInputTemp = getinput & W_T12V_OLD;
	W_T12V_DATA.words = ((getinput ^ W_T12V_OLD) & W_T12V_DATA.words) | VolInputTemp;
	W_T12V_OLD = getinput;

	//
	//unsigned char trigger_status => "1"= port high ??????????
	//RKB : PA12
	
	if (f_trigger_in == 1) {
		status = 1;
	}else{
		status = 0;
	}
	//20130621: need delay time : AC code on 과 의 시간차이로 power_off 를 또 시작하는 문제점이 있습니다.
	//	power on : f_trigger_in = trigger_status = 0/1;
	if (trigger_status != status) {
		trigger_status = status;				//save!
		if (trigger_status == 1) {
			//power on!
			switch(mode_PowerStatus) {
				case POWER_OFF:
					//? All CH OFF -> skip!
					j=0;
					for (i=0;i<4;i++) {
						if (amp_on_off[i] == 0) ++j;
					}
					if (j == 4) {
						//Now ALL CH OFF!!!!
					}else{
						//goto power-on!
						Func_PowerOn();
					}
#ifdef ASCiiKEY
            		Func_PowerStatus();     //20131217
					//20140219
					//all CH status
					Func_AMP_CH_status(4);
#endif
					break;
				case POWER_ON0:			
				case POWER_ON0_ING:		//move 20131217
				case POWER_ON1_ING:
				case POWER_ON2_ING:
				case POWER_ON3_ING:
				case POWER_ON4_ING:		//move 20131216
				case POWER_ON5_ING:		//move 20131216
				default:
					break;
			}
		}else{
			//power off!
			switch(mode_PowerStatus) {
				case POWER_ON0:			
				case POWER_ON0_ING:		//move 20131217
				case POWER_ON1_ING:
				case POWER_ON2_ING:
			    case POWER_ON3_ING:
				case POWER_ON4_ING:		//move 20131216
				case POWER_ON5_ING:		//move 20131216
					//standby on
					Func_PowerOff();
#ifdef ASCiiKEY
            		Func_PowerStatus();     //20131217
					//20140219
					//all CH status
					Func_AMP_CH_status(4);
#endif
					break;
				case POWER_OFF:			
#ifdef ASCiiKEY
            		Func_PowerStatus();     //20131217
					//20140219
					//all CH status
					Func_AMP_CH_status(4);
#endif
				default:
					break;
			}
		}
	}
}
/*
******************************************************************/
unsigned char Check_FrontKeySkip_PowerOff(void)
{
	unsigned char result;
	
	result = FALSE;
	switch(mode_PowerStatus) {
		case POWER_OFF:
			result = TRUE;
			break;
		case POWER_ON0:
		default:
			break;
	}
	return(result);
}
/*
	power on --> call
	
	RKB-D850, RKB-D8100 : ( analog(0) or Digital(1) ) -> analog fix
	RKB-850, RKB-8100 : skip
	
#define SEL_CH_A_Digital(x)		( (x==1)? GPIO_SetBits(CH_AB_SEL_PORT, CH_A_DIGITAL) : GPIO_ResetBits(CH_AB_SEL_PORT, CH_A_DIGITAL) );
#define SEL_CH_B_Digital(x)		( (x==1)? GPIO_SetBits(CH_AB_SEL_PORT, CH_B_DIGITAL) : GPIO_ResetBits(CH_AB_SEL_PORT, CH_B_DIGITAL) );
#define SEL_CH_C_Digital(x)		( (x==1)? GPIO_SetBits(CH_CD_SEL_PORT, CH_C_DIGITAL) : GPIO_ResetBits(CH_CD_SEL_PORT, CH_C_DIGITAL) );
#define SEL_CH_D_Digital(x)		( (x==1)? GPIO_SetBits(CH_CD_SEL_PORT, CH_D_DIGITAL) : GPIO_ResetBits(CH_CD_SEL_PORT, CH_D_DIGITAL) );

******************************************************************/
void 	Set_Function(void)
{
	//850(0),8100(1),D850(2) and D8100(3)
	switch (RKB_ModelID) {
		case RKB_D850:	//D850
		case RKB_D8100:	//D8100
			//
			//( analog(0) or Digital(1) ) -> analog fix
			//
			SEL_CH_A_Digital(0);
			SEL_CH_B_Digital(0);
			SEL_CH_C_Digital(0);
			SEL_CH_D_Digital(0);
			//20130620
			mode_func = FUNCTION_AMP_A;
			break;
		case RKB_850:	//850
		case RKB_8100:	//8100
			//skip
		default:
			break;
	}				
}

/*	RKB : 
	20msec main call
	Auto change Digital and Analog
	//20150303
	//unsigned char input_selMode[4]; 
	INPUT_SEL_AUTO
	INPUT_SEL_DIGITAL
	INPUT_SEL_ANALOG
***************************************************************/
void 	ListenFunctionAutoChange(void)
{
	
	if (mode_PowerStatus < POWER_ON2_ING) return; 				//20140106

	if ( (RKB_ModelID == RKB_D850) || (RKB_ModelID == RKB_D8100) ) {
		//850(0),8100(1),D850(2) and D8100(3)
		//----------------------------------------------------------------
		//	need mute control ???? => volume IC each other 
		//================================================================
	//20150303 : compare AMP A : AUTO / DIGITAL
	if ( (input_selMode[0] == INPUT_SEL_AUTO)||(input_selMode[0] ==	INPUT_SEL_DIGITAL) ) {		
		if (input_selMode[0] ==	INPUT_SEL_DIGITAL) {		
			//change digital=High	
			SEL_CH_A_Digital(1);
		}
		//AMP A :		
		if (b_StreamCheckDelay_A == 0) {
            if (b_PllStatus[0] == PLL_UNLOCK) {					//PLL_LOCK=0 / PLL_UNLOCK=1 / PLL_CHECKING=2
            	if (b_PllStatus_bk[0] != b_PllStatus[0]) {
					b_PllStatus_bk[0] = b_PllStatus[0];
					//20150303 : compare : AUTO
					if (input_selMode[0] ==	INPUT_SEL_AUTO) {		
						//change analog=Low   		
						SEL_CH_A_Digital(0);
					}
					if (RKB_mode_PP_MP == 1) {				
						//20131203 : ? PP-OFF Mode
						//need Tx delay time
						b_FreqTxDelay_A = TIME20MSTO400MS;
					}
					b_StreamCheckDelay_A = TIME20MSTO100MS;        //20130521
					//20130620
					mode_func = FUNCTION_AMP_A;			//(FUNCTION_AMP_A = 1)+0/1/2/3
					//off!
					b_StreamFreq[0] = 7;
				}
			}else if (b_PllStatus[0] == PLL_LOCK) {
            	if (b_PllStatus_bk[0] != b_PllStatus[0]) {
					b_PllStatus_bk[0] = b_PllStatus[0];
					//change digital=High	
					SEL_CH_A_Digital(1);
					if (RKB_mode_PP_MP == 1) {				
						//20131203 : ? PP-OFF Mode
						//need Tx delay time
						b_FreqTxDelay_A = TIME20MSTO400MS;
					}
					//20130821
					if (b_StreamKind[0] == ST_CHECKING) {
						b_StreamCheckDelay_A = 1;		//20130821
					}else{
						b_StreamCheckDelay_A = TIME20MSTO100MS;        //20130521
					}
					//20130620
					mode_func = FUNCTION_AMP_A;			//(FUNCTION_AMP_A = 1)+0/1/2/3
				}
			}
			//
            //current freq Tx? : 20130821
            //
            if (b_StreamFreq[0] != b_StreamFreq_bk[0]) {
                b_StreamFreq_bk[0] = b_StreamFreq[0];		//backup save
				if (RKB_mode_PP_MP == 1) {				
					//20131203 : ? PP-OFF Mode
					//need Tx delay time
					b_FreqTxDelay_A = TIME20MSTO400MS;
				}else{
                	Func_CurrentFreqTx(0);					//Tx : "freq=44.1!"
                }
            }			
		}else{
			--b_StreamCheckDelay_A;
		}
		//----------------------------------------------------------------
		if (RKB_mode_PP_MP == 1) {				
			//20131203 : ? PP-OFF Mode
			//20131202 : Tx only one Tx!
			if (b_FreqTxDelay_A > 0) {
				if (--b_FreqTxDelay_A == 0) {
            		//current freq Tx? :
					Func_CurrentFreqTx(0);				//Tx : "freq=44.1"
				}
			}
		}
	}else{
		//fix analog
		//change analog=Low   		
		SEL_CH_A_Digital(0);
	}
	//================================================================
	//20150303 : compare : AUTO / DIGITAL
	if ( (input_selMode[1] == INPUT_SEL_AUTO)||(input_selMode[1] ==	INPUT_SEL_DIGITAL) ) {		
		if (input_selMode[1] ==	INPUT_SEL_DIGITAL) {		
			//change digital=High	
			SEL_CH_B_Digital(1);
		}
		//AMP B :		
		if (b_StreamCheckDelay_B == 0) {
            if (b_PllStatus[1] == PLL_UNLOCK) {
            	if (b_PllStatus_bk[1] != b_PllStatus[1]) {
					b_PllStatus_bk[1] = b_PllStatus[1];
					//20150303 : compare : AUTO
					if (input_selMode[1] ==	INPUT_SEL_AUTO) {		
						//change analog=Low   	
						SEL_CH_B_Digital(0);
					}
					if (RKB_mode_PP_MP == 1) {				
						//20131203 : ? PP-OFF Mode
						//need Tx delay time
						b_FreqTxDelay_B = TIME20MSTO400MS;
					}
					b_StreamCheckDelay_B = TIME20MSTO100MS;        //20130521
					//20130620
					mode_func = FUNCTION_AMP_B;			//(FUNCTION_AMP_A = 1)+0/1/2/3
					//off!
					b_StreamFreq[1] = 7;
					//Func_CurrentFreqTx(1);			//Tx : "freq=44.1!"
				}
			}else if (b_PllStatus[1] == PLL_LOCK) {
            	if (b_PllStatus_bk[1] != b_PllStatus[1]) {
					b_PllStatus_bk[1] = b_PllStatus[1];
					//change digital=High		
					SEL_CH_B_Digital(1);
					if (RKB_mode_PP_MP == 1) {				
						//20131203 : ? PP-OFF Mode
						//need Tx delay time
						b_FreqTxDelay_B = TIME20MSTO400MS;
					}
					//20130821
					if (b_StreamKind[1] == ST_CHECKING) {
						b_StreamCheckDelay_B = 1;		//20130821
					}else{
						b_StreamCheckDelay_B = TIME20MSTO100MS;        //20130521
					}
					//20130620
					mode_func = FUNCTION_AMP_B;			//(FUNCTION_AMP_A = 1)+0/1/2/3
				}
			}
			//
            //current freq Tx? : 20130821
            //
            if (b_StreamFreq[1] != b_StreamFreq_bk[1]) {
                b_StreamFreq_bk[1] = b_StreamFreq[1];		//backup save
				if (RKB_mode_PP_MP == 1) {				
					//20131203 : ? PP-OFF Mode
					//need Tx delay time
					b_FreqTxDelay_B = TIME20MSTO400MS;
				}else{
                	Func_CurrentFreqTx(1);					//Tx : "freq=44.1!"
                }
            }
		}else{
			--b_StreamCheckDelay_B;
		}
		//----------------------------------------------------------------
		if (RKB_mode_PP_MP == 1) {				
			//20131203 : ? PP-OFF Mode
			//20131202 : Tx only one Tx!
			if (b_FreqTxDelay_B > 0) {
				if (--b_FreqTxDelay_B == 0) {
            		//current freq Tx? :
					Func_CurrentFreqTx(1);				//Tx : "freq=44.1"
				}
			}
		}
	}else{
		//fix analog
		//change analog=Low   		
		SEL_CH_B_Digital(0);
	}
	//================================================================
	//20150303 : compare : AUTO / DIGITAL
	if ( (input_selMode[2] == INPUT_SEL_AUTO)||(input_selMode[2] ==	INPUT_SEL_DIGITAL) ) {		
		if (input_selMode[2] ==	INPUT_SEL_DIGITAL) {		
			//change digital=High	
			SEL_CH_C_Digital(1);
		}
		//AMP C :		
		if (b_StreamCheckDelay_C == 0) {
            if (b_PllStatus[2] == PLL_UNLOCK) {
            	if (b_PllStatus_bk[2] != b_PllStatus[2]) {
					b_PllStatus_bk[2] = b_PllStatus[2];
					//20150303 : compare : AUTO
					if (input_selMode[2] ==	INPUT_SEL_AUTO) {		
						//change analog=Low   	
						SEL_CH_C_Digital(0);
					}
					if (RKB_mode_PP_MP == 1) {				
						//20131203 : ? PP-OFF Mode
						//need Tx delay time
						b_FreqTxDelay_C = TIME20MSTO400MS;
					}
					b_StreamCheckDelay_C = TIME20MSTO100MS;        //20130521
					//20130620
					mode_func = FUNCTION_AMP_C;			//(FUNCTION_AMP_A = 1)+0/1/2/3
					//off!
					b_StreamFreq[2] = 7;
					//Func_CurrentFreqTx(2);			//Tx : "freq=44.1!"
				}
			}else if (b_PllStatus[2] == PLL_LOCK) {
            	if (b_PllStatus_bk[2] != b_PllStatus[2]) {
					b_PllStatus_bk[2] = b_PllStatus[2];
					//change digital=High		
					SEL_CH_C_Digital(1);
					if (RKB_mode_PP_MP == 1) {				
						//20131203 : ? PP-OFF Mode
						//need Tx delay time
						b_FreqTxDelay_C = TIME20MSTO400MS;
					}
					//20130821
					if (b_StreamKind[2] == ST_CHECKING) {
						b_StreamCheckDelay_C = 1;		//20130821
					}else{
						b_StreamCheckDelay_C = TIME20MSTO100MS;        //20130521
					}
					//20130620
					mode_func = FUNCTION_AMP_C;			//(FUNCTION_AMP_A = 1)+0/1/2/3
				}
			}
			//
            //current freq Tx? : 20130821
            //
            if (b_StreamFreq[2] != b_StreamFreq_bk[2]) {
                b_StreamFreq_bk[2] = b_StreamFreq[2];		//backup save
				if (RKB_mode_PP_MP == 1) {				
					//20131203 : ? PP-OFF Mode
					//need Tx delay time
					b_FreqTxDelay_C = TIME20MSTO400MS;
				}else{
                	Func_CurrentFreqTx(2);					//Tx : "freq=44.1!"
                }
            }
		}else{
			--b_StreamCheckDelay_C;
		}
		//----------------------------------------------------------------
		if (RKB_mode_PP_MP == 1) {				
			//20131203 : ? PP-OFF Mode
			//20131202 : Tx only one Tx!
			if (b_FreqTxDelay_C > 0) {
				if (--b_FreqTxDelay_C == 0) {
            		//current freq Tx? :
					Func_CurrentFreqTx(2);				//Tx : "freq=44.1"
				}
			}
		}
	}else{
		//fix analog
		//change analog=Low   		
		SEL_CH_C_Digital(0);
	}
	//================================================================
	//20150303 : compare : AUTO / DIGITAL
	if ( (input_selMode[3] == INPUT_SEL_AUTO)||(input_selMode[3] ==	INPUT_SEL_DIGITAL) ) {		
		if (input_selMode[3] ==	INPUT_SEL_DIGITAL) {		
			//change digital=High	
			SEL_CH_D_Digital(1);
		}
		//AMP D :		
		if (b_StreamCheckDelay_D == 0) {
            if (b_PllStatus[3] == PLL_UNLOCK) {
            	if (b_PllStatus_bk[3] != b_PllStatus[3]) {
					b_PllStatus_bk[3] = b_PllStatus[3];
					//20150303 : compare : AUTO
					if (input_selMode[3] ==	INPUT_SEL_AUTO) {		
						//change analog=Low   	
						SEL_CH_D_Digital(0);
					}
					if (RKB_mode_PP_MP == 1) {				
						//20131203 : ? PP-OFF Mode
						//need Tx delay time
						b_FreqTxDelay_D = TIME20MSTO400MS;
					}
					b_StreamCheckDelay_D = TIME20MSTO100MS;        //20130521
					//20130620
					mode_func = FUNCTION_AMP_D;			//(FUNCTION_AMP_A = 1)+0/1/2/3
					//off!
					b_StreamFreq[3] = 7;
					//Func_CurrentFreqTx(3);			//Tx : "freq=44.1!"
				}
			}else if (b_PllStatus[3] == PLL_LOCK) {
            	if (b_PllStatus_bk[3] != b_PllStatus[3]) {
					b_PllStatus_bk[3] = b_PllStatus[3];
					//change digital=High		
					SEL_CH_D_Digital(1);
					if (RKB_mode_PP_MP == 1) {				
						//20131203 : ? PP-OFF Mode
						//need Tx delay time
						b_FreqTxDelay_D = TIME20MSTO400MS;
					}
					//20130821
					if (b_StreamKind[3] == ST_CHECKING) {
						b_StreamCheckDelay_D = 1;		//20130821
					}else{
						b_StreamCheckDelay_D = TIME20MSTO100MS;		//20130521
					}
					//20130620
					mode_func = FUNCTION_AMP_D;			//(FUNCTION_AMP_A = 1)+0/1/2/3
				}
			}
			//
            //current freq Tx? : 20130821
            //
            if (b_StreamFreq[3] != b_StreamFreq_bk[3]) {
                b_StreamFreq_bk[3] = b_StreamFreq[3];		//backup save
				if (RKB_mode_PP_MP == 1) {				
					//20131203 : ? PP-OFF Mode
					//need Tx delay time
					b_FreqTxDelay_D = TIME20MSTO400MS;
				}else{
                	Func_CurrentFreqTx(3);					//Tx : "freq=44.1!"
                }
            }
		}else{
			--b_StreamCheckDelay_D;
		}
		//----------------------------------------------------------------
		if (RKB_mode_PP_MP == 1) {				
			//20131203 : ? PP-OFF Mode
			//20131202 : Tx only one Tx!
			if (b_FreqTxDelay_D > 0) {
				if (--b_FreqTxDelay_D == 0) {
            		//current freq Tx? :
					Func_CurrentFreqTx(3);				//Tx : "freq=44.1"
				}
			}
		}
	}else{
		//fix analog
		//change analog=Low   		
		SEL_CH_D_Digital(0);
	}
	//================================================================
#ifdef OPTICAL_POWER_ENABLE	//not used
		//================================================================
		//201405116 : add feature
		//
		//	optical_power_mode : POWER_ON0 : 4EA digital off -> goto standby-on!
		//================================================================
		if (optical_power_mode == ON) {
			if(mode_PowerStatus == POWER_ON0) {
            	if ( (b_PllStatus_bk[0] == PLL_UNLOCK)&&(b_PllStatus_bk[1] == PLL_UNLOCK)&&(b_PllStatus_bk[2] == PLL_UNLOCK)&&(b_PllStatus_bk[3] == PLL_UNLOCK) ) 
            	{
            		//
            		// goto standby-on
            		// ( optical sensor reading and save )
            		//
					Func_PowerOff();
					//			
#ifdef ASCiiKEY
					Func_PowerStatus();
#endif		
            	}			
			}			
		}
#endif	//OPTICAL_POWER_ENABLE
		//================================================================
	}
}

/*
 rs232 : FL on/off key 
**********************************************************/
void Func_FLOnOff(void)
{			
	if(b_floff_mode == 1) {
		b_floff_mode = 0;				/* 0 = fl on */
		mode_floff = 0;				/* 0 = flmode on */
		timer_floff = 0;				/* 20010831 */
	}else{
		switch(mode_PowerStatus) {
			case POWER_ON0:			
				b_floff_mode = 1;		/* 1 = FL off! */
				mode_floff = 1;		/* 1 = flmode off */
				break;
			case POWER_ON0_ING:		//move 20131217
			case POWER_ON1_ING:
			case POWER_ON2_ING:
			case POWER_ON3_ING:
			case POWER_ON4_ING:		//move 20131216
			case POWER_ON5_ING:		//move 20131216
			case POWER_OFF:			
			default:
				break;
		}
	}
}
/*
***********************************************************/
void	Power_SetPowerStatus(unsigned char stat)
{
	mode_PowerStatus = stat;
}
/*
***************************************************************/
unsigned char	Power_GetPowerStatus(void)
{
	unsigned char result;
	
	switch(mode_PowerStatus) {
		case POWER_ON0:			
		case POWER_ON0_ING:		//move 20131217
		case POWER_ON1_ING:
		case POWER_ON2_ING:
		case POWER_ON3_ING:
		case POWER_ON4_ING:		//move 20131216
		case POWER_ON5_ING:		//move 20131216
			result = 1;
			break;		
		case POWER_OFF:			
		default:
			result = 0;
			break;
	}
	return(result);
}
/*
***********************************************************/
void	Power_SetPowerMode(unsigned char mode)
{
	mode_Power = mode;
}

//--------------------------------------------------------------------
void	Mute_SetMuteOffTime(unsigned char time)
{
	timer_MuteOff = time;
}
//--------------------------------------------------------------------
//	call : main 100msec
//--------------------------------------------------------------------
void	Mute_CheckMuteOff_100msec(void)
{
	unsigned char i,j;
	
	if(mode_PowerStatus == POWER_ON0) {
		if(timer_MuteOff != 0) {
			if(--timer_MuteOff == 0) {
				switch(mode_PowerStatus) {			
					case POWER_ON0:			
					case POWER_ON0_ING:		//move 20131217
					case POWER_ON1_ING:
					case POWER_ON2_ING:
			        case POWER_ON3_ING:
					case POWER_ON4_ING:		//move 20131216
					case POWER_ON5_ING:		//move 20131216
						Mute_AllMuteOff();							//f_MainMute = 0 flag 만 clear!!!!! => RKB volume Tx
					//20140305
					#ifdef RS232_VOL_ENABLE_IS_TRIMMIN			//used
						//? All TRIM min(0)
						j=0;
						for (i=0;i<4;i++) {
							if (key.KeyData[i] == 0) ++j;
						}
						//
						if (j == 4) {
							//All TRIM = min(0)!!! 
							//front knob disable!
							front_trimKnob_enable = OFF;
						}else{
							//All TRIM <> min(0)!!! 
							//front knob enable!
							front_trimKnob_enable = ON;
						}
					#endif						
						//20140218
						//(? Front trim and L/R_ch )
						if (front_trimKnob_enable == OFF) {
							//
							//front knob disable!
							//
							//RS232 L/R-ch volume data Tx
							for (i=0;i<4;i++) {
								if (amp_on_off[i] == 1) {
									//? check mute key
									//20140224
									if (ch_muteOn[i] == MUTE_OFF) {
										//mute off ->volume data Tx : 
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
			        					Func_VolumeLchStatus(i);
			        					Func_VolumeRchStatus(i);
			        				}
			        			}else{
			        				//mute
									Set_MainVolume(0,i);
			        			}
							}
							//							
						}else{
							//
							//front knob enable!
							//
							//front trimKnob read and Tx
							for (i=0;i<4;i++) {
								if (amp_on_off[i] == 1) {
									//? check mute key
									//20140224
									if (ch_muteOn[i] == MUTE_OFF) {
										key_target[i] = key.KeyData[i];
										current_key[i] = key_target[i];
										//Set_MainVolume(vol,ampType) => L/R Tx
										Set_MainVolume(current_key[i],i);
										//RS232 Tx
										Func_AmpTrimCurrentStatus(i);
									}
								}else{
									//mute
									Set_MainVolume(0,i);
								}
							}
						}
						break;
					case POWER_OFF:
						break;		
					default:
						break;
				}						
			}
		}
//	PutString("Total %\n",f_TotalMute);
//	PutString("MainM=> %\n",f_MainMute);
        //850, 8100
        //20140401 : add 100msec delay
		if(timer_MuteOff == 0) {
			if ( (mute_mode == MUTE_OFF) && (f_MainMute == 0) ) {      //20130322
    			//AMP A/B/C/D/ = mute off!!!!!
    			//key.KeyOk[0] == ON;
    			//key.KeyOk[1] == ON;
    			//key.KeyOk[2] == ON;
    			//key.KeyOk[3] == ON;
				//--------------------------------------------
				//REC-mute : OFf
				//PP : Low active
				//WS : High active
				//MP : AMP-ON 
				//--------------------------------------------
				if (RKB_mode_PP_MP == 1) {							//? PP unit
					preOut_Mute(1);
				}else{
					preOut_Mute(0);
				}
				//--------------------------------------------
				//p_main_mute = 1;	//low active : mute off
			}
		}
    }
}
/*	100msec
	current_key[4]
	current_key[5]
	current_key[6]		//850+8100
	current_key[7]		//8100
	
	fan_hs_on_temp
	fan_hs_off_temp
	smps_fan_on_temp
	smps_fan_off_temp

// FAN_LS_mode
// FAN_HS_mode
#define	FAN_READY					0
#define	FAN_EXEC					1

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
	
//8(3)FAN control
#define FAN_hs_on(x)				( (x==1)? GPIO_SetBits(FAN_PORT, FAN_CTRL) : GPIO_ResetBits(FAN_PORT, FAN_CTRL) );
#define FAN_ls_on(x)				( (x==1)? GPIO_SetBits(FAN_LS_PORTB, FAN_LS_nOFF) : GPIO_ResetBits(FAN_LS_PORTB, FAN_LS_nOFF) );
*********************************************************/
void	FAN_check(void)
{
	switch(mode_PowerStatus) {			
		case POWER_ON0:
			//? Model
			if (RKB_mode_PP_MP == 1) {							//? PP unit
#ifdef SIGNAL_SENSING_ENABLE
				//
				//MP2 & signal sensing mode
				//
				if (RKB_mode_PP_nMP2 == 0) {
					//							
					//20140718
					//MP2 & signal sensing SENSE_nSW
					//
					//20140725 : add compile option
#ifdef RKB_DIGITAL_ANALOG
					if ( (GetSignalSensing_nSW == 0)&&(signalSensingAMPStatus == OFF) ) {
						//----------------------------------------------------------
						// No signal {after 10 minute)
						//----------------------------------------------------------
						//? MP2 unit
						//MP2 : add SMPS_sensor_1/2		
						//20140619 : add FAN on/off feature!
						//20140619 : always 8100/D8100
						//----------------------------------------------------------
						//	High Speed
						//----------------------------------------------------------
						if (FAN_HS_mode == FAN_EXEC) {
							//check HS Low Temp.
							if ( ( (current_key[4] <= fan_hs_off_temp)&&(current_key[5] <= fan_hs_off_temp) )&&( (current_key[6] <= smps_fan_off_temp)&&(current_key[7] <= smps_fan_off_temp) ) )
							{
								FAN_HS_mode = FAN_READY;
								FAN_hs_on(0);
								//Tx status
								Func_FAN_HS_Status();
							}
						} else if (FAN_HS_mode == FAN_READY) {
							//check HS High Temperature
							if ( ( (current_key[4] >= fan_hs_on_temp)||(current_key[5] >= fan_hs_on_temp) ) || ( (current_key[6] >= smps_fan_on_temp)||(current_key[7] >= smps_fan_on_temp) ) )
							{
								//20140703
								FAN_LS_mode = FAN_EXEC;
								FAN_ls_on(1);
								FAN_HS_mode = FAN_EXEC;
								FAN_hs_on(1);
								//Tx status
								Func_FAN_HS_Status();
							}
						}
						//----------------------------------------------------------
						//	Low Speed
						//----------------------------------------------------------
						//20140703
						if (FAN_HS_mode == FAN_READY)
						{
							if (FAN_LS_mode == FAN_EXEC) {
								//check LS Low Temp.
								if ( ( (current_key[4] <= fan_ls_off_temp)&&(current_key[5] <= fan_ls_off_temp) ) )
								{
									FAN_LS_mode = FAN_READY;
									FAN_ls_on(0);
									//Tx status
									Func_FAN_HS_Status();						
								}
							} else if (FAN_LS_mode == FAN_READY) {
								//check LS High Temperature
								if ( (current_key[4] >= fan_ls_on_temp)||(current_key[5] >= fan_ls_on_temp) )
								{
									FAN_LS_mode = FAN_EXEC;
									FAN_ls_on(1);
									//Tx status
									Func_FAN_HS_Status();
								}	
							}
						}
					}else{
						//----------------------------------------------------------
						// Now signal OK
						//----------------------------------------------------------
						//? MP2 unit
						//MP2 : add SMPS_sensor_1/2		
						//20140619 : add FAN on/off feature!
						//20140619 : always 8100/D8100
						//----------------------------------------------------------
						//	High Speed
						//----------------------------------------------------------
						if (FAN_HS_mode == FAN_EXEC) {
							//check HS Low Temp.
							if ( ( (current_key[4] <= fan_hs_off_temp)&&(current_key[5] <= fan_hs_off_temp) )&&( (current_key[6] <= smps_fan_off_temp)&&(current_key[7] <= smps_fan_off_temp) ) )
							{
								FAN_HS_mode = FAN_READY;
								FAN_hs_on(0);
								//Tx status
								Func_FAN_HS_Status();
							}
						} else if (FAN_HS_mode == FAN_READY) {
							//check HS High Temperature
							if ( ( (current_key[4] >= fan_hs_on_temp)||(current_key[5] >= fan_hs_on_temp) ) || ( (current_key[6] >= smps_fan_on_temp)||(current_key[7] >= smps_fan_on_temp) ) )
							{
								//20140703
								FAN_LS_mode = FAN_EXEC;
								FAN_ls_on(1);
								FAN_HS_mode = FAN_EXEC;
								FAN_hs_on(1);
								//Tx status
								Func_FAN_HS_Status();
							}
						}
						//----------------------------------------------------------
						//	Low Speed
						//----------------------------------------------------------
						//20140703
						if (FAN_HS_mode == FAN_READY)
						{
							if (FAN_LS_mode == FAN_EXEC) {
								//check LS Low Temp.
								if ( ( (current_key[4] <= fan_ls_off_temp)&&(current_key[5] <= fan_ls_off_temp) ) )
								{
									FAN_LS_mode = FAN_READY;
									FAN_ls_on(0);
									//Tx status
									Func_FAN_HS_Status();						
								}
							} else if (FAN_LS_mode == FAN_READY) {
								//check LS High Temperature
								if ( (current_key[4] >= fan_ls_on_temp)||(current_key[5] >= fan_ls_on_temp) )
								{
									FAN_LS_mode = FAN_EXEC;
									FAN_ls_on(1);
									//Tx status
									Func_FAN_HS_Status();
								}	
							}
						}
					}
#else //RKB_DIGITAL_ANALOG	
					//===============================================================================
					//add 4 Model Name 	
					//===============================================================================
					if ( (GetSignalSensing_nSW == 0)&&(signalSensingAMPStatus == OFF) ) {
						//----------------------------------------------------------
						// No signal {after 10 minute)
						//----------------------------------------------------------
						//? MP2 unit
						//MP2 : add SMPS_sensor_1/2		
						//20140619 : add FAN on/off feature!
						//20140725
						if ( (RKB_ModelID == RKB_8100) || (RKB_ModelID == RKB_D8100) ) {		
							//!8100 or D8100 [ 00=850, 01=8100, 10=D850, 11=D8100 ]
							//----------------------------------------------------------
							//	High Speed
							//----------------------------------------------------------
							if (FAN_HS_mode == FAN_EXEC) {
								//check HS Low Temp.
								if ( ( (current_key[4] <= fan_hs_off_temp)&&(current_key[5] <= fan_hs_off_temp) )&&( (current_key[6] <= smps_fan_off_temp)&&(current_key[7] <= smps_fan_off_temp) ) )
								{
									FAN_HS_mode = FAN_READY;
									FAN_hs_on(0);
									//Tx status
									Func_FAN_HS_Status();
								}
							} else if (FAN_HS_mode == FAN_READY) {
								//check HS High Temperature
								if ( ( (current_key[4] >= fan_hs_on_temp)||(current_key[5] >= fan_hs_on_temp) ) || ( (current_key[6] >= smps_fan_on_temp)||(current_key[7] >= smps_fan_on_temp) ) )
								{
									//20140703
									FAN_LS_mode = FAN_EXEC;
									FAN_ls_on(1);
									FAN_HS_mode = FAN_EXEC;
									FAN_hs_on(1);
									//Tx status
									Func_FAN_HS_Status();
								}
							}
							//----------------------------------------------------------
							//	Low Speed
							//----------------------------------------------------------
							//20140703
							if (FAN_HS_mode == FAN_READY)
							{
								if (FAN_LS_mode == FAN_EXEC) {
									//check LS Low Temp.
									if ( ( (current_key[4] <= fan_ls_off_temp)&&(current_key[5] <= fan_ls_off_temp) ) )
									{
										FAN_LS_mode = FAN_READY;
										FAN_ls_on(0);
										//Tx status
										Func_FAN_HS_Status();						
									}
								} else if (FAN_LS_mode == FAN_READY) {
									//check LS High Temperature
									if ( (current_key[4] >= fan_ls_on_temp)||(current_key[5] >= fan_ls_on_temp) )
									{
										FAN_LS_mode = FAN_EXEC;
										FAN_ls_on(1);
										//Tx status
										Func_FAN_HS_Status();
									}	
								}
							}
						}else{
							//!850 or D850 [ 00=850, 01=8100, 10=D850, 11=D8100 ]
							//----------------------------------------------------------
							//	High Speed
							//----------------------------------------------------------
							if (FAN_HS_mode == FAN_EXEC) {
								//check HS Low Temp.
								if ( ( (current_key[4] <= fan_hs_off_temp)&&(current_key[5] <= fan_hs_off_temp) )&&( (current_key[6] <= smps_fan_off_temp) ) )
								{
									FAN_HS_mode = FAN_READY;
									FAN_hs_on(0);
									//Tx status
									Func_FAN_HS_Status();
								}
							} else if (FAN_HS_mode == FAN_READY) {
								//check HS High Temperature
								if ( ( (current_key[4] >= fan_hs_on_temp)||(current_key[5] >= fan_hs_on_temp) ) || ( (current_key[6] >= smps_fan_on_temp) ) )
								{
									//20140703
									FAN_LS_mode = FAN_EXEC;
									FAN_ls_on(1);
									FAN_HS_mode = FAN_EXEC;
									FAN_hs_on(1);
									//Tx status
									Func_FAN_HS_Status();
								}
							}
							//----------------------------------------------------------
							//	Low Speed
							//----------------------------------------------------------
							//20140703
							if (FAN_HS_mode == FAN_READY)
							{
								if (FAN_LS_mode == FAN_EXEC) {
									//check LS Low Temp.
									if ( ( (current_key[4] <= fan_ls_off_temp)&&(current_key[5] <= fan_ls_off_temp) ) )
									{
										FAN_LS_mode = FAN_READY;
										FAN_ls_on(0);
										//Tx status
										Func_FAN_HS_Status();						
									}
								} else if (FAN_LS_mode == FAN_READY) {
									//check LS High Temperature
									if ( (current_key[4] >= fan_ls_on_temp)||(current_key[5] >= fan_ls_on_temp) )
									{
										FAN_LS_mode = FAN_EXEC;
										FAN_ls_on(1);
										//Tx status
										Func_FAN_HS_Status();
									}	
								}
							}
						}
					}else{
						//----------------------------------------------------------
						// Now signal OK
						//----------------------------------------------------------
						//? MP2 unit
						//MP2 : add SMPS_sensor_1/2		
						//20140619 : add FAN on/off feature!
						//20140725
						if ( (RKB_ModelID == RKB_8100) || (RKB_ModelID == RKB_D8100) ) {		
							//!8100 or D8100 [ 00=850, 01=8100, 10=D850, 11=D8100 ]
							//----------------------------------------------------------
							//	High Speed
							//----------------------------------------------------------
							if (FAN_HS_mode == FAN_EXEC) {
								//check HS Low Temp.
								if ( ( (current_key[4] <= fan_hs_off_temp)&&(current_key[5] <= fan_hs_off_temp) )&&( (current_key[6] <= smps_fan_off_temp)&&(current_key[7] <= smps_fan_off_temp) ) )
								{
									FAN_HS_mode = FAN_READY;
									FAN_hs_on(0);
									//Tx status
									Func_FAN_HS_Status();
								}
							} else if (FAN_HS_mode == FAN_READY) {
								//check HS High Temperature
								if ( ( (current_key[4] >= fan_hs_on_temp)||(current_key[5] >= fan_hs_on_temp) ) || ( (current_key[6] >= smps_fan_on_temp)||(current_key[7] >= smps_fan_on_temp) ) )
								{
									//20140703
									FAN_LS_mode = FAN_EXEC;
									FAN_ls_on(1);
									FAN_HS_mode = FAN_EXEC;
									FAN_hs_on(1);
									//Tx status
									Func_FAN_HS_Status();
								}
							}
							//----------------------------------------------------------
							//	Low Speed
							//----------------------------------------------------------
							//20140703
							if (FAN_HS_mode == FAN_READY)
							{
								if (FAN_LS_mode == FAN_EXEC) {
									//check LS Low Temp.
									if ( ( (current_key[4] <= fan_ls_off_temp)&&(current_key[5] <= fan_ls_off_temp) ) )
									{
										FAN_LS_mode = FAN_READY;
										FAN_ls_on(0);
										//Tx status
										Func_FAN_HS_Status();						
									}
								} else if (FAN_LS_mode == FAN_READY) {
									//check LS High Temperature
									if ( (current_key[4] >= fan_ls_on_temp)||(current_key[5] >= fan_ls_on_temp) )
									{
										FAN_LS_mode = FAN_EXEC;
										FAN_ls_on(1);
										//Tx status
										Func_FAN_HS_Status();
									}	
								}
							}
						}else{
							//!850 or D850 [ 00=850, 01=8100, 10=D850, 11=D8100 ]
							//----------------------------------------------------------
							//	High Speed
							//----------------------------------------------------------
							if (FAN_HS_mode == FAN_EXEC) {
								//check HS Low Temp.
								if ( ( (current_key[4] <= fan_hs_off_temp)&&(current_key[5] <= fan_hs_off_temp) )&&( (current_key[6] <= smps_fan_off_temp) ) )
								{
									FAN_HS_mode = FAN_READY;
									FAN_hs_on(0);
									//Tx status
									Func_FAN_HS_Status();
								}
							} else if (FAN_HS_mode == FAN_READY) {
								//check HS High Temperature
								if ( ( (current_key[4] >= fan_hs_on_temp)||(current_key[5] >= fan_hs_on_temp) ) || ( (current_key[6] >= smps_fan_on_temp) ) )
								{
									//20140703
									FAN_LS_mode = FAN_EXEC;
									FAN_ls_on(1);
									FAN_HS_mode = FAN_EXEC;
									FAN_hs_on(1);
									//Tx status
									Func_FAN_HS_Status();
								}
							}
							//----------------------------------------------------------
							//	Low Speed
							//----------------------------------------------------------
							//20140703
							if (FAN_HS_mode == FAN_READY)
							{
								if (FAN_LS_mode == FAN_EXEC) {
									//check LS Low Temp.
									if ( ( (current_key[4] <= fan_ls_off_temp)&&(current_key[5] <= fan_ls_off_temp) ) )
									{
										FAN_LS_mode = FAN_READY;
										FAN_ls_on(0);
										//Tx status
										Func_FAN_HS_Status();						
									}
								} else if (FAN_LS_mode == FAN_READY) {
									//check LS High Temperature
									if ( (current_key[4] >= fan_ls_on_temp)||(current_key[5] >= fan_ls_on_temp) )
									{
										FAN_LS_mode = FAN_EXEC;
										FAN_ls_on(1);
										//Tx status
										Func_FAN_HS_Status();
									}	
								}
							}
						}
					}
#endif //RKB_DIGITAL_ANALOG
				}else
#endif
				{
					//==============================================================================
					//PP : add SMPS_sensor_1/2		
					//==============================================================================
					//20140725
					if ( (RKB_ModelID == RKB_8100) || (RKB_ModelID == RKB_D8100) ) {		
						//!8100 or D8100 [ 00=850, 01=8100, 10=D850, 11=D8100 ]
						if (FAN_HS_mode == FAN_EXEC) {
							//check Low Temp.
							if ( ( (current_key[4] <= fan_hs_off_temp)&&(current_key[5] <= fan_hs_off_temp) )&&( (current_key[6] <= smps_fan_off_temp)&&(current_key[7] <= smps_fan_off_temp) ) )
							{
								FAN_HS_mode = FAN_READY;
								FAN_hs_on(0);
								//Tx status
								Func_FAN_HS_Status();						
							}
						} else if (FAN_HS_mode == FAN_READY) {
							//check High Temperature
							if ( ( (current_key[4] >= fan_hs_on_temp)||(current_key[5] >= fan_hs_on_temp) ) || ( (current_key[6] >= smps_fan_on_temp)||(current_key[7] >= smps_fan_on_temp) ) )
							{
								FAN_HS_mode = FAN_EXEC;
								FAN_hs_on(1);
								//Tx status
								Func_FAN_HS_Status();
							}	
						}
					}else{
						//!850 or D850 [ 00=850, 01=8100, 10=D850, 11=D8100 ]
						if (FAN_HS_mode == FAN_EXEC) {
							//check Low Temp.
							if ( ( (current_key[4] <= fan_hs_off_temp)&&(current_key[5] <= fan_hs_off_temp) )&&( (current_key[6] <= smps_fan_off_temp) ) )
							{
								FAN_HS_mode = FAN_READY;
								FAN_hs_on(0);
								//Tx status
								Func_FAN_HS_Status();						
							}
						} else if (FAN_HS_mode == FAN_READY) {
							//check High Temperature
							if ( ( (current_key[4] >= fan_hs_on_temp)||(current_key[5] >= fan_hs_on_temp) ) || ( (current_key[6] >= smps_fan_on_temp) ) )
							{
								FAN_HS_mode = FAN_EXEC;
								FAN_hs_on(1);
								//Tx status
								Func_FAN_HS_Status();
							}	
						}
					}
				}
			}
			break;
		case POWER_OFF:
			break;		
		case POWER_ON0_ING:		//move 20131217
		case POWER_ON1_ING:
		case POWER_ON2_ING:
		case POWER_ON3_ING:
		case POWER_ON4_ING:		//move 20131216
		case POWER_ON5_ING:		//move 20131216
		default:
			break;
	}
}


/*	10msec

	when GetSignalSensing_nSW = ON ,   
	AC code in --> power on [ signalSensingAMPStatus = ON, signal_sensing_counter = 0 ]
	
	signalSensingAMPStatus 		: ON, OFF
	signal_sensing_counter		short

#define	SIGNAL_SENSING_OFF_TIME			10 minutes
#define	SIGNAL_SENSING_ON_TIME			500 msec

#define GetSignalSensing_nSW		GPIO_ReadInputDataBit(SIGNAL_SENSING_PORTA, SIGNAL_SENSING_nSW)
#define RKB_mode_PP_nMP2			GPIO_ReadInputDataBit(SIGNAL_SENSING_PORTA, SIGNAL_SENSING_nMP2)
#define GetSignalSensing_nPort		GPIO_ReadInputDataBit(SIGNAL_SENSING_PORTA, SIGNAL_SENSING_nIN)
#define FAN_ls_on(x)				( (x==1)? GPIO_SetBits(FAN_LS_PORTB, FAN_LS_nOFF) : GPIO_ResetBits(FAN_LS_PORTB, FAN_LS_nOFF) );

	20140523 : need consider : mute on/off, volume-IC control key(volume, balance,,,,)
*********************************************************/
#ifdef SIGNAL_SENSING_ENABLE
void signal_sensingPort_check(void)
{
	unsigned char i,j;

	switch(mode_PowerStatus) {			
		case POWER_ON0:
		case POWER_OFF:		//add : 20140819
			//? Model
			if ( (RKB_mode_PP_MP == 1)&&(RKB_mode_PP_nMP2 == 0) )							//? PP & MP2 unit
			{
				//
				//MP2 & signal sensing SENSE_nSW
				//
				if (GetSignalSensing_nSW == 0) {
					//----------------------------------------------------------
					//signal sensing SW mode ON! [ s/w below-down ]
					//----------------------------------------------------------
					//201400804
					if (current_SignalSensingMode != 1) {
						current_SignalSensingMode = 1;
						//
						// On/Start! Signal Sensing Mode
						//20140819 : all CH on
						//all CH on <= signal sensing mode
						//20140825
						signalSensingAMPStatus = ON;
						//reset counter
						signal_sensing_counter = 0;
						//? All CH OFF -> skip!
						j=0;
						for (i=0;i<4;i++) {
							if (amp_on_off[i] == 0) ++j;
						}
						if (j == 4) {
							//Now ALL CH OFF!!!!
							//all CH on saving
							for (i=0;i<4;i++)
							{
								//20140819 
								amp_on_off[i] = 1;
								eepromAddr = ADDR_AMP_A_ONOFF+i;					//20140224
								nvr_xPutData(eepromAddr,amp_on_off[i]);	
							}										
							//All Power on -> exec.
							//powerOn -> CH on/off checking -> CH-off is muteOn => CH-on is volumeOn
							PowerOn_StandBy(BOOT_NORMAL);
							//
#ifdef ASCiiKEY
							Func_PowerStatus();
							//all CH status
							Func_AMP_CH_status(4);
#endif		
						}else{
							//--------------------------------------------------
							//CH on
							//--------------------------------------------------
							// all mute flag clear! ->
							Func_MuteFlagAllCH_Off();
							//
							for (i=0;i<4;i++) {
								//20140325 : ? each CH power OFF check
								if (amp_on_off[i] == 0) {
									//20140325 : ? 
									amp_on_off[i] = 1;
									eepromAddr = ADDR_AMP_A_ONOFF+i;					//20140224
									nvr_xPutData(eepromAddr,amp_on_off[i]);				
									// volume Tx ->
									//Set_MainVolume(vol,ampType) => L/R Tx
									//20140221
									if (front_trimKnob_enable == OFF) {
										//volume_Lch[4]
										//volume_Rch[4]
										//20140305 : if same Lch & Rch and then have to balance calculation
										if ( volume_Lch[i] != volume_Rch[i] ) {
											//Set_MainVolume(vol,ampType) => L/R Tx
											Set_MainVolumeLeft(volume_Lch[i],i);		//mute off
											Set_MainVolumeRight(volume_Rch[i],i);		//mute off
										}else{
											//Tx Rch & balance									
											Set_MainVolume((unsigned char)volume_Lch[i],(i));
										}
									}else{
										//current_key[4]
										//Set_MainVolume(vol,ampType) => L/R Tx & balance!
										Set_MainVolume((unsigned char)current_key[i],(i));      //=>RKB_function.c
									}	
								}				
							}
#ifdef ASCiiKEY
							Func_PowerStatus();
							//all CH status
							Func_AMP_CH_status(4);
#endif		
						}
						//
#ifdef SIGNAL_SENSING_DEBUG
						//20140730 
						PutChar('\r');
						rs232_printf("SignalSensingMode On!\r",blank);
#endif
					}
					//----------------------------------------------------------
					if (signalSensingAMPStatus == ON) {
						//----------------------------------------------------------------
						//now AMP status-ON mode
						//----------------------------------------------------------------
						//checking for OFF
						//
						if (GetSignalSensing_nPort == 1) {		
							//
							//No signal!!
							//
							if (signal_sensing_counter >= SIGNAL_SENSING_OFF_TIME) {		//? 10 minute/100msec
								//goto OFF!
								signalSensingAMPStatus = OFF;
								//20140703 : 50sec timer (100msec) :
								signalOn_500m_timer = 0;
								//timer Noise counter
								//20140812
								singalOn_noise_counter = 0;
								singalOn_low_counter = 0;
								//reset counter
								signal_sensing_counter = 0;
								//----------------------------------------------
        						//Led control select ( LED_MODE_POWER_ON_SIG_OFF or LED_MODE_POWER_ON_SIG_DIM )
#ifdef LED_DIMMER_ENABLE
        						//20140901
								set_LED_mode(LED_MODE_POWER_ON_SIG_DIM);
								StandbyLED(0);
#else
        						//20140901
								set_LED_mode(LED_MODE_POWER_ON_SIG_OFF);
#endif
								//----------------------------------------------
								//goto AMP off!
								//----------------------------------------------
								Mute_AllMuteOn();
								//----------------------------------------------
#ifdef SIGNAL_SENSING_DEBUG
								//20140612 : string out for test!
								PutChar('\r');
								rs232_printf("SignalSensingMode: AMP Off!\r",blank);
#endif
							}
						}else{
							//
							//Is signal!!
							//reset counter
							//clear 10minutes counter
							signal_sensing_counter = 0;
						}
					}else{
						//----------------------------------------------------------------
						//now AMP status-OFF mode
						//----------------------------------------------------------------
						//checking for ON
						//? Noise counter
						if (singalOn_noise_counter >= 5) {
							//
							//Is signal!!
							//
							//goto ON!
							signalSensingAMPStatus = ON;
							//reset counter
							//20140812
							signalOn_500m_timer = 0;
							singalOn_low_counter = 0;
							singalOn_noise_counter = 0;
							//----------------------------------------------
        					//Led control LED_MODE_STOP!!!!!!!!!
        					//20140718
        					Led_mode = LED_MODE_STOP;
        					Led_count_timer = 0;
							//----------------------------------------------
							//goto AMP on!
							//----------------------------------------------
							Mute_AllMuteOff();
#ifdef SIGNAL_SENSING_DEBUG
							//20140612 : string out for test!
							PutChar('\r');
							rs232_printf("SignalSensingMode: AMP On!\r",blank);    						
#endif
						}
					}
				}else{
					//----------------------------------------------------------
					//signal sensing SW mode OFF! [ s/w center ]
					//----------------------------------------------------------
					//20140804
					if (current_SignalSensingMode != 0) {
						current_SignalSensingMode = 0;
						//
						// Off! Signal Sensing Mode
						//
#ifdef SIGNAL_SENSING_DEBUG
						//20140730 
						PutChar('\r');
						rs232_printf("SignalSensingMode Off!\r",blank);
#endif
					}
					//
					//20140703
					if (signalSensingAMPStatus == OFF) {
							//goto ON!
							signalSensingAMPStatus = ON;
							//reset counter
							//20140812
							signalOn_500m_timer = 0;
							singalOn_low_counter = 0;
							singalOn_noise_counter = 0;
							//----------------------------------------------
        					//Led control LED_MODE_STOP!!!!!!!!!
        					//20140718
        					Led_mode = LED_MODE_STOP;
        					Led_count_timer = 0;
							//----------------------------------------------
							//goto AMP on!
							//----------------------------------------------
							Mute_AllMuteOff();
#ifdef SIGNAL_SENSING_DEBUG
							//20140612 : string out for test!
							PutChar('\r');
							rs232_printf("SignalSensingMode: AMP On!\r",blank);
#endif
					}				
				}
			}
			break;
		case POWER_ON0_ING:		//move 20131217
		case POWER_ON1_ING:
		case POWER_ON2_ING:
		case POWER_ON3_ING:
		case POWER_ON4_ING:		//move 20131216
		case POWER_ON5_ING:		//move 20131216
		default:
			break;
	}
}
#endif

#endif

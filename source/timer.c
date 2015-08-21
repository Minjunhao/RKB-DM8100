/**************** (c)2008 Rotel Technology Global Inc..*************************
PROJECT         : RKB-850,8100,D850,D8100
COMPILER        : IAR Embeded Workbench 5.50
MODULE          : timer.c
VERSION         : V 1.0
USED CHIP       : stm32f10x
CREATION DATE   : /2013
AUTHORS         : /Rotel
DESCRIPTION     :
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "initmcu.h"
#include "main.h"

#include "adckey.h"

#include "timer.h"
#include "function.ext"
#include "proc_function.h"

#include "ak4114.ext"

#ifdef IPOD_INCLUDE
#include "iPod_config.h"
#endif

/*----------------------------------------------------------------------------*/
volatile vu32 TimingDelay = 0;
volatile byte Timer_test = 0;
volatile byte Timer10msR = 0;
volatile byte Timer10ms = 0;
volatile byte Timer20ms = 0;
volatile byte Timer100ms = 0;
volatile byte Timer125ms = 0;       //VFD timer == 6msec
volatile word Timer250ms = 0; 
volatile word Timer500ms = 0; 
volatile byte Timer2ms = 2;
volatile byte Timer250us = 0;

#define _MN_ "TIMER: "

extern byte    u8column;

#ifdef IAP_UPGRADE
extern WORD EEP_Wait;
#endif
/*-----------------------------------------------------------------------------
 Function Name  : timer1_CC_IRQHandler
 Description    : spi1 interrupt handler
 Input          : None
 Output         : None
 Return         : None
 Comments	 	: timer per 50us / audio signal sensing
-----------------------------------------------------------------------------*/
void timer1_UP_IRQHandler(void)
{   
	TIM_ClearITPendingBit(TIM1,TIM_FLAG_Update);

//rmc1
//test
//		++Timer_test;
//		StandbyLED(Timer_test&0x01);
/*
	rmc1_Timer_250usec();
	if(rmc1_status == TRUE) {
		rmc1_process();
	}
//rmcE
	rmcE_Timer_250usec();
	if(rmcE_status == TRUE) {
		rmcE_process();
	}
//rmc2
	rmc2_Timer_250usec();
	if(rmc2_status == TRUE) {
		rmc2_process();
	}
//rmc3
	rmc3_Timer_250usec();
	if(rmc3_status == TRUE) {
		rmc3_process();
	}
//rmc4	
	rmc4_Timer_250usec();
	if(rmc4_status == TRUE) {
		rmc4_process();
	}
*/
#ifdef SIGNAL_SENSING_ENABLE
	//20140703
	//
	// audio signal sensing
	//
	//	counter : 2EA / 500msec
	//	over 100msec : clear counter buffer
	//
	//
	if(mode_PowerStatus == POWER_ON0) 
	{
		if ( (RKB_mode_PP_MP == 1)&&(RKB_mode_PP_nMP2 == 0)&&(GetSignalSensing_nSW == 0) )
		{
			if (signalSensingAMPStatus == OFF)
			{
				//
				//now status-ON mode
				//checking for ON
				//
				if (GetSignalSensing_nPort == 1) {
					//
					// no signal
				}else{
					//
					// yes signal(Noise)
					//
					if (++singalOn_low_counter > SIGNAL_SENSING_ON_TIME) {	//?150usec
						//
						// goto singal(Noise) on counter START!!!
						//
						signalOn_500m_timer = 0;
						singalOn_low_counter = 0;
						++singalOn_noise_counter;
					}
				}
			}
			//
			//	100msec timer check
			//
			if (++signalOn_500m_timer >= 2000) {
				signalOn_500m_timer = 0;
				singalOn_low_counter = 0;
				singalOn_noise_counter = 0;
			}				
		}	
	}	
#endif
}
/*
	signal sensing on condition :
		50usec low pulse : 
		- 2EA / 1sec => on 조건입니다.
		- 1sec 이면 counter clear!!!
----------------------------------------------------------*/
//20110824
#ifdef TIMER2_ENABLE	
void timer2_IRQHandler(void)
{
	unsigned char i;
	 
	TIM_ClearITPendingBit(TIM2,TIM_FLAG_Update);
	//
	//
	//
}
#endif
/*-----------------------------------------------------------------------------
 Function Name  : timer_SysTickHandler
 Description    : call from stm32f10x_it.c each time 1ms
 Input          : None
 Output         : None
 Return         : None
 Comments	 	: ..per 1ms
-----------------------------------------------------------------------------*/
//20140114 : LED dimming table
//const byte TBL_DIMMER_TIMER[8] = { 10,10,8,6,4,3,2,1 };
//20140225 : LED dimming table : 20=MIN(1)
const byte TBL_DIMMER_TIMER[11] = { 20,20,16,12,9,7,5,4,3,2,1 };
//--------------------------------------------------------------------
void timer_SysTickHandler(void)
{
	//can handling timer conuter each time 1ms
	if (TimingDelay != 0) TimingDelay--;

	//count power down : 20130624
	if (GPIO_ReadInputDataBit(POWERDOWN_PORT,POWER_DOWN)) {
		PowerDownCounter = TIME30MS;
	} else {
		if ( PowerDownCounter != 0 ) {
			if (--PowerDownCounter == 0) {		//20140108
				//mute on -> volume mute Tx
				Mute_AllMuteOn();
			}
		}
	}
	//------------------------------------------------------
/*
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
*/
#ifdef LED_DIMMER_ENABLE
	//20140409
	if ( Led_mode != LED_MODE_SKIP ) {
			//20140225 : timer => directly on/off
			if (Led_current_status == ON) {
				//20140114 : LED dimming
				//20140115 : LED_Dimmer_value = 0, Skip
				//20140225 : LED_Dimmer_value => change 20.20,,,,,,, (10step)
				//if (LED_Dimmer_value > 0) {
					//? one period => reset
					if (++LED_dimming_counter >= 20) LED_dimming_counter=0;
					//20140901 : power-off-dimming
					if ( Led_mode == LED_MODE_POWER_OFF_DIM ) {
						//
						//SSM AMP off = 90% dimed-LED on
						////20140901
						//? Fixed-dimmer table [20(dark)..1(bright)]
						// timmer value : 1(dark) -->20(bright)
						if (LED_dimming_counter >= 2) {
							//Off! LED
							StandbyLED(0);
						}else{
							//ON! LED
							StandbyLED(1);
						}
					//20140901 : LED_MODE_POWER_ON_SIG_DIM1
					}else if ( Led_mode == LED_MODE_POWER_ON_SIG_DIM1 ) {
						//
						//SSM AMP off = 50% dimed-LED on
						////20140901
						//? Fixed-dimmer table [20(dark)..1(bright)]
						// timmer value : 1(dark) -->20(bright)
						if (LED_dimming_counter >= 2) {
							//Off! LED
							StandbyLED(0);
						}else{
							//ON! LED
							StandbyLED(1);
						}
					}else{
						//? check dimmer table
						if (LED_dimming_counter >= TBL_DIMMER_TIMER[LED_Dimmer_value] ) {
							//Off! LED
							StandbyLED(0);
						}else{
							//ON! LED
							StandbyLED(1);
						}
					}
				//}
			}else{
				//Off! LED
				StandbyLED(0);		//20140409
			}
	}
#endif
	//------------------------------------------------------
	
	if (++Timer10msR >= 10) {
		Timer10msR = 0;
		//rmc1_CheckFlame_10msec();
	}		
	Timer10ms++;
	Timer20ms++;
	Timer100ms++;
	Timer125ms++;       //6msec use VFD
	Timer250ms++;
	Timer500ms++;
	
    //20121106
	if(mode_PowerStatus >= POWER_ON3_ING) { 				//check from power 3ing
		//----------------------------------------------------------------
/*    	//check Encoder Port : PC5(a),PC4(b)
        if (enc.VolTimerPer1ms<0xff) enc.VolTimerPer1ms++;
        if (--Timer2ms == 0) {
            Timer2ms = 2;
            enc_EncoderChekPer2ms();
            //CMedia Int : 20121214
            CMedia_Int_2ms();
        }
*/        
	    //----------------------------------------------------------------
    	//4114 check 1msec 
        //	if(f_PllCheckEnable == 1){
	    //20100701 : spdif 4114 int0 active => skip Mute
    	//?digital function
		//----------------------------------------------------------------
		//----------------------------------------------------------------
    	//AK4117 A :
		//flag position : 0/1/2/3
		if(Get4114INT0_A != 0){
		    //High -> interrupt occur
		    //20140225
		    if (b_INT0_bk[0] != 1) {
		    	b_INT0_bk[0] = 1; 
			//f_PllCheckEnable=0;
			//test chd 20100701
			//Mute_SetMuteOffTime(COUNT100M_800MS);
			//change function 와 같이 사용하기때문에 비교문 추가함...function mute 를 유지하기위해서
		    //if(timer_MuteOff > 0) {
				//20150303 : compare AMP A : AUTO / DIGITAL
				if ( (input_selMode[0] == INPUT_SEL_AUTO)||(input_selMode[0] ==	INPUT_SEL_DIGITAL) ) {		
					DirectMuteOn[0] = 1;
			    	timer_DirectMuteOff[0] = 95;
			    }
			//}
			//f_StreamRechkReq=1;
			}
		}else{
		    //Low -> no interrupt : 
		    //20140225
		    b_INT0_bk[0] = 0;
		    //
		}
		//----------------------------------------
		if(timer_DirectMuteOff[0] > 0) {
		    if(--timer_DirectMuteOff[0] == 0) {
		        //20140224 : mute off request
		        DirectMuteOff[0] = 1;
		    }
		}
		//----------------------------------------------------------------
		//----------------------------------------------------------------
    	//AK4117 B :
		//flag position : 0/1/2/3
		if(Get4114INT0_B != 0){
		    //High -> interrupt occur
		    //20140225
		    if (b_INT0_bk[1] != 1) {
		    	b_INT0_bk[1] = 1; 
			//f_PllCheckEnable=0;
			//test chd 20100701
			//Mute_SetMuteOffTime(COUNT100M_800MS);
			//change function 와 같이 사용하기때문에 비교문 추가함...function mute 를 유지하기위해서
		    //if(timer_MuteOff > 0) {
				//20150303 : compare : AUTO / DIGITAL
				if ( (input_selMode[1] == INPUT_SEL_AUTO)||(input_selMode[1] ==	INPUT_SEL_DIGITAL) ) {		
					DirectMuteOn[1] = 1;
			    	timer_DirectMuteOff[1] = 95;
			    }
			//}
			//f_StreamRechkReq=1;
			}
		}else{
		    //Low -> no interrupt : 
		    //20140225
			b_INT0_bk[1] = 0;
			// 
		}
		//----------------------------------------
		if(timer_DirectMuteOff[1] > 0) {
		    if(--timer_DirectMuteOff[1] == 0) {
		        //mute off request
		        DirectMuteOff[1] = 1;
		    }
		}
		//----------------------------------------------------------------
		//----------------------------------------------------------------
    	//AK4117 C :
		//flag position : 0/1/2/3
		if(Get4114INT0_C != 0){
		    //High -> interrupt occur
		    //20140225
		    if (b_INT0_bk[2] != 1) {
		    	b_INT0_bk[2] = 1; 
			//f_PllCheckEnable=0;
			//test chd 20100701
			//Mute_SetMuteOffTime(COUNT100M_800MS);
			//change function 와 같이 사용하기때문에 비교문 추가함...function mute 를 유지하기위해서
		    //if(timer_MuteOff > 0) {
				//20150303 : compare : AUTO / DIGITAL
				if ( (input_selMode[2] == INPUT_SEL_AUTO)||(input_selMode[2] ==	INPUT_SEL_DIGITAL) ) {		
					DirectMuteOn[2] = 1;
			    	timer_DirectMuteOff[2] = 95;
			    }
			//}
			//f_StreamRechkReq=1;
			}
		}else{
		    //Low -> no interrupt : 
		    //20140225
			b_INT0_bk[2] = 0;
			// 
		}
		//----------------------------------------
		if(timer_DirectMuteOff[2] > 0) {
		    if(--timer_DirectMuteOff[2] == 0) {
		        //mute off request
		        DirectMuteOff[2] = 1;
		    }
		}
		//----------------------------------------------------------------
		//----------------------------------------------------------------
    	//AK4117 D :
		//flag position : 0/1/2/3
		if(Get4114INT0_D != 0){
		    //High -> interrupt occur
		    //20140225
		    if (b_INT0_bk[3] != 1) {
		    	b_INT0_bk[3] = 1; 
			//f_PllCheckEnable=0;
			//test chd 20100701
			//Mute_SetMuteOffTime(COUNT100M_800MS);
			//change function 와 같이 사용하기때문에 비교문 추가함...function mute 를 유지하기위해서
		    //if(timer_MuteOff > 0) {
				//20150303 : compare : AUTO / DIGITAL
				if ( (input_selMode[3] == INPUT_SEL_AUTO)||(input_selMode[3] ==	INPUT_SEL_DIGITAL) ) {		
					DirectMuteOn[3] = 1;
			    	timer_DirectMuteOff[3] = 95;
			    }
			//}
			//f_StreamRechkReq=1;
			}
		}else{
		    //Low -> no interrupt : 
		    //20140225
			b_INT0_bk[3] = 0;
			// 
		}
		//----------------------------------------
		if(timer_DirectMuteOff[3] > 0) {
		    if(--timer_DirectMuteOff[3] == 0) {
		        //mute off request
		        DirectMuteOff[3] = 1;
		    }
		}
		//----------------------------------------------------------------
    }   //check from POWER_ON3_ING
	//----------------------------------------------------------------
#ifdef IPOD_INCLUDE
	TCLS_iPodTimer();
#endif		

#ifdef IAP_UPGRADE
	if (EEP_Wait!=0)EEP_Wait--;
#endif		
}
/*******************************************************************************
* Function Name  : DelayMs
* Description    : Inserts a delay time.
* Input          : nTime: specifies the delay time length, in milliseconds.
* Output         : None
* Return         : None
*******************************************************************************/
void DelayMs(vu32 mSec)
{
  	TimingDelay = mSec;
	while (TimingDelay != 0);
  	/* Clear SysTick Counter */
  	SysTick_CounterCmd(SysTick_Counter_Clear);
}

/*-----------------------------------------------------------------------------
 Function Name  : timer_Get10ms
 Description    : call from main
 Input          : None
 Output         : None
 Return         : None
 Comments	 	: get 10ms time
-----------------------------------------------------------------------------*/
bool timer_Get10ms(void)
{
	bool ret_val = FALSE;
	if (Timer10ms >= TIME10MS) {
		Timer10ms = 0;
		ret_val = TRUE;
	}
	return ret_val;
}
bool timer_Get20ms(void)
{
	bool ret_val = FALSE;
	if (Timer20ms >= TIME20MS) {
		Timer20ms = 0;
		ret_val = TRUE;
	}
	return ret_val;
}
bool timer_Get100ms(void)
{
	bool ret_val = FALSE;
	if (Timer100ms >= TIME100MS) {
		Timer100ms = 0;
		ret_val = TRUE;
	}
	return ret_val;
}
bool timer_Get125ms(void)
{
	bool ret_val = FALSE;
	if (Timer125ms >= TIME125MS) {
		Timer125ms = 0;
		ret_val = TRUE;
	}
	return ret_val;
}
bool timer_Get250ms(void)
{
	bool ret_val = FALSE;
	if (Timer250ms >= TIME250MS) {		//word
		Timer250ms = 0;
		ret_val = TRUE;
	}
	return ret_val;
}
bool timer_Get500ms(void)
{
	bool ret_val = FALSE;
	if (Timer500ms >= TIME500MS) {		//word
		Timer500ms = 0;
		ret_val = TRUE;
	}
	return ret_val;
}


/* end of file */
/******************* (C) COPYRIGHT 2008 Rotel Technology Global **************/

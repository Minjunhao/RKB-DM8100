/**************** (c)2008 Rotel Technology Global Inc..*************************
PROJECT         : RA1570
COMPILER        : IAR Embeded Workbench 5.50
MODULE          : encode.c
VERSION         : V 1.0
USED CHIP       : stm32f101x
CREATION DATE   : /201206
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
#include "uart.h"
#include "timer.h"

#include "adckey.h"
#include "encode.h"
#include "encode.mem"

/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
#define _MN_ "ENCODER: "
/*-----------------------------------------------------------------------------
 Function Name  : enc_EncoderInit
 Description    : initialize for encoder
 Input          : None
 Output         : None
 Return         : None
 Comments	 	: ..
-----------------------------------------------------------------------------*/
//#ifdef VOLUME_SPEED
void Clear_EncValue(void)
{	
	Timer_UpEncoder_Counter = 0;
	Timer_DownEncoder_Counter = 0;
	Backup_UpEncoder_Counter = 0;
	Backup_DownEncoder_Counter = 0;
}
//#endif
void enc_EncoderInit(void)
{
	enc.VolInputData.words = 0;
	enc.EncoderFlags.words = 0;
/*
	f_enc_volume_key = 0;
	f_enc_volume_ready = 0;
	f_enc_treble_key = 0;
	f_enc_treble_ready = 0;
	f_enc_bass_key = 0;
	f_enc_bass_ready = 0;
    f_enc_volume_low = 0;
    f_enc_treble_low = 0;
    f_enc_bass_low = 0;
	f_enc_token = 0;
*/	
	memset(&enc.EncoderFlags.words,0,sizeof(enc.EncoderFlags.words));
	memset(&enc.VolInputData.words,0,sizeof(enc.VolInputData.words));
	memset(&enc.VolInputOld,0,sizeof(enc.VolInputOld));
	enc.EncoderTokenCounter = TIME10MSTO50MS;

	Clear_EncValue();								//20090811
	//debug((_MN_"encoder Init..\n"));
}
/*-----------------------------------------------------------------------------
 Function Name  : enc_EncoderChekPer2ms
 Description    : check encoder
 Input          : None
 Output         : None
 Return         : None
 Comments	 	: call 2msec timer ->
-----------------------------------------------------------------------------*/
void enc_EncoderUp(void)
{
	if (f_enc_volume_dir != F_ENC_UP) { //?direction up : 20121228
	    //down => up
	    //
	    Timer_DownEncoder_TimeCounter = 0xff;
        enc.VolTimerPer1ms=0xff;
    }
    //
	f_enc_volume_dir = F_ENC_UP;		//direction up
#ifdef VOLUME_SPEED
	//encoder counter :20040821
	++Timer_UpEncoder_Counter;
	//encoder 1msec timer ->timer-value save and clear
	Timer_UpEncoder_TimeCounter = enc.VolTimerPer1ms;
	enc.VolTimerPer1ms = 0;
#endif
}
//--------------------------------------------------------------------
void enc_EncoderDown(void)
{
	if (f_enc_volume_dir != F_ENC_DOWN) { //?direction down : 20121228
	    //up => down
	    //
	    Timer_UpEncoder_TimeCounter = 0xff;
        enc.VolTimerPer1ms=0xff;
    }
    //
	f_enc_volume_dir = F_ENC_DOWN;		//direction down
#ifdef VOLUME_SPEED
	//encoder counter :20040821
	++Timer_DownEncoder_Counter;		//
	//encoder 1msec timer ->timer-value save and clear
	Timer_DownEncoder_TimeCounter = enc.VolTimerPer1ms;
	enc.VolTimerPer1ms = 0;
#endif
}
//-------------------------------------------------------------------------------
void enc_EncoderChekPer2ms(void)
{
	word getinput,VolInputTemp;
	
	// check encoder PC5(a_1),PC4(b_0), PC14(AMP protection)
	getinput = GPIO_ReadInputData(ENC_PORT)&0x0030;
	VolInputTemp = getinput & enc.VolInputOld;
	enc.VolInputData.words = ((getinput ^ enc.VolInputOld) & enc.VolInputData.words) | VolInputTemp;
	enc.VolInputOld = getinput;

//	ENCODER ab
//
//	[11] ==CW==> [01] <==CCW== [00]
//	[00] ==CW==> [10] <==CCW== [11]
//
	if (f_enc_volume_ready == ON) {					//encoder check on?
		if (f_enc_volume_a == 0) {				
			if (f_enc_volume_b == 0) {
			    //ab=00		
				b_encoder_mode = 0x00;					//ab<=00
			}else{
				//ab=01
				f_enc_volume_ready = OFF;				//encoder check OFF!

				if (b_encoder_mode == 0x00) {
					//direction up
					enc_EncoderUp();
				}else{
					//direction down
					enc_EncoderDown();
				}
				f_enc_volume_key = ON;
			}
		}else{
			if (f_enc_volume_b == 1) {
			    //ab=11
				b_encoder_mode = 0x03;					//ab<=11
			}else{
				//ab=10
				f_enc_volume_ready = OFF;				//encoder check OFF!
				if (b_encoder_mode == 0x00) {
					//direction down
					enc_EncoderDown();
				}else{
					//direction up
					enc_EncoderUp();
				}
				f_enc_volume_key = ON;
			}
		}
	}else{
		if (f_enc_volume_a == 1) {			//ab=11
			if (f_enc_volume_b == 1) {		
				f_enc_volume_ready = ON;	//encoder check ON!
			}
		}
/*
		if (f_enc_volume_a == 0) {			//ab=00
			if (f_enc_volume_b == 0) {		
				f_enc_volume_ready = ON;	//encoder check ON!
			}
		}
*/
	}
}

/* end of file */
/******************* (C) COPYRIGHT 2008 Rotel Technology Global **************/

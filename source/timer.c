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
//#include "function.ext"
#include "proc_function.h"

//#include "ak4114.ext"

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
#ifdef _ETHERNET
__IO u32 LocalTime = 0;
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

	#ifdef _ETHERNET
	LocalTime++;
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
  	//SysTick_CounterCmd(SysTick_Counter_Clear);
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

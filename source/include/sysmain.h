/**************** (c)2008 Rotel Technology Global Inc..*************************
PROJECT         : TFT Monitor with HDMI & YPbPr(CbCr) & YC & CVBS input
COMPILER        : IAR Embeded Workbench 4.4.1
MODULE          : sysmain.h
VERSION         : V 1.0
USED CHIP       : stm32f10x
CREATION DATE   : /2008
AUTHORS         : An Kyung Chan / RTG
DESCRIPTION     :
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SYSTEM_MAIN_H__
#define __SYSTEM_MAIN_H__

typedef	struct System_t {
	byte_flags SystemStatus;
} System_t;
extern System_t sys;
#define f_power_on		sys.SystemStatus.byte_bit.bit0
#define f_display_on	sys.SystemStatus.byte_bit.bit1
#define f_initialize	sys.SystemStatus.byte_bit.bit2
#define f_change_input	sys.SystemStatus.byte_bit.bit3
#define f_install_edid	sys.SystemStatus.byte_bit.bit4

/*----------------------------------------------------------------------------*/
void main_SystemInit(void);
void main_Power(byte onoff);
void main_CheckEncoderPer10msec(void);
void main_EncoderVolData(byte vol);
void main_EncoderTrebBassData(byte vol);
void main_PowerDownCheck(void);


#endif 	//__SYSTEM_MAIN_H__


/* end of file */
/******************* (C) COPYRIGHT 2008 Rotel Technology Global **************/

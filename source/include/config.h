/**************** (c)2008 Rotel Technology Global Inc..*************************
PROJECT         : RKB-850,8100,D850,D8100
COMPILER        : IAR Embeded Workbench 5.50
MODULE          : RKB_initmcu.c
VERSION         : V 1.0
USED CHIP       : RKB (stm32f101RC) [T=36pin,C=48pin,R=64pin,V=100pin : 6=32KB/6KB,8=64KB/10KB,B=128KB/16KB,C=256KB/32KB]
CREATION DATE   : /2013
AUTHORS         : /Rotel
DESCRIPTION     :
*******************************************************************************/
#include "stm32f2xx.h"
#include "cortexm3_macro.h"

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CONFIG_H
#define __CONFIG_H

#define _STM32_F2XX_144PIN
/**********************************************************
		Compile Option 
**********************************************************/
//----------------------------------------------------------
//RKB MP1 - re-work board 에서의 수정사항 : 20140619
//	option ID0 : (fix) = High(8100)
//	option ID1 : = Low(Analog), = High(Digital) =>> model name change : RKB Digital/Analog select , 850/D850 에서의 온도 display 가 항상 2개의 sensing mode 로 표현됨.
//
//	
//RKB MP1 re-work & MP2 를 위해서 아래 2개의 option 을 같이 움직여야합니다.
//----------------------------------------------------------
#define 	JTAG_DISABLE                //20140612 : JTAG disable => JTAG normal port!!!!!!
#ifdef JTAG_DISABLE
//#define 	RKB_DIGITAL_ANALOG			//20140725 : signal sensing mode 에서 Digital/Analog compile option 으로 삭제하고  => seperate Model Name ( ModelName, productType, FAN_check 수정 )
#define 	SIGNAL_SENSING_ENABLE		//20140612 : signal sensing mode : FAN Low Speed : enable // and need compile option : JTAG_DISABLE of config.h
#endif
//----------------------------------------------------------
#define 	LINE2_ENABLE                //20120806 : VFD 2x20 => tclinkmain.c
//----------------------------------------------------------
#define		DISP_8BIT_ENABLE			//display data buffer 8bit 사용...

#define		ENCODER_VOLUME_SPEED		//encoder volume speed : 20111020

////#define 	TIMER2_ENABLE			//enable VFD control ( TIMER2 ) : 20110824 <<test>>
#define 	ASCiiKEY					//enable ASCii Key : 20110224

#define     POWER_CONSUMPTION				//110118 for set input mode uart port during standby mode : power consumption

//#define 	UART2_ENABLE     			//UART2 = Telechips
//#define 	UART3_ENABLE     			//UART3 = not used
//#define 	UART4_ENABLE     			//UART4 = RotelLink [57600,n,8,1]
//#define 	UART5_ENABLE     			//UART5 = not used

//#define 	RCX1500						//enable RCX1500
//#define 	RA1570						//enable RA1570-RC1570

#define 	SIMPLE_LINGO				//enable Simple Lingo
//multi-opaque test
#define 	STINGRAY_UTF8_4321				//Stingray UTF8 : 20100511,20100831


/* Define to prevent recursive inclusion -------------------------------------*/

typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;
typedef unsigned char Uint8;
typedef 	u8		byte;
typedef 	u16		word;
typedef 	u8		BOOL;
typedef 	u8 		BYTE;
typedef 	u16 	WORD;
typedef 	u32		DWORD;
typedef		u32		LWORD;

typedef union {
	unsigned short words;
	unsigned char Byte[2];
	struct {
		unsigned short bit0:1;
		unsigned short bit1:1;
		unsigned short bit2:1;
		unsigned short bit3:1;
		unsigned short bit4:1;
		unsigned short bit5:1;
		unsigned short bit6:1;
		unsigned short bit7:1;
		unsigned short bit8:1;
		unsigned short bit9:1;
		unsigned short bit10:1;
		unsigned short bit11:1;
		unsigned short bit12:1;
		unsigned short bit13:1;
		unsigned short bit14:1;
		unsigned short bit15:1;
	}word_bit;
} word_flags;
typedef union {
	unsigned char Byte;
	struct {
		unsigned char bit0:1;
		unsigned char bit1:1;
		unsigned char bit2:1;
		unsigned char bit3:1;
		unsigned char bit4:1;
		unsigned char bit5:1;
		unsigned char bit6:1;
		unsigned char bit7:1;
	}byte_bit;
} byte_flags;

union d_byte {
	unsigned short A;
	unsigned char B[2];
};

typedef union wWORD {
	unsigned int iValue;
	unsigned char bValue[2];
}wWORD;

//----------------------------------------------------------
#define DISPLAYBUFFER_MAX_SIZE 	46
typedef union uTDISP {			
	unsigned char	display_Buffer[DISPLAYBUFFER_MAX_SIZE];
	struct sStingray{
		unsigned char DispBuf[2][20];			
		unsigned char CursorXYPosition[4];			
		unsigned char ScreenState[2];			
	}sStingray;
}uTDISP;
//----------------------------------------------------------
//20120613 : UTF-8
#ifdef STINGRAY_UTF8_4321
#define DISPLAYBUFFER_UTF_MAX_SIZE 	86
typedef union uUTF8DISP {						
	unsigned char	display_Buffer[DISPLAYBUFFER_UTF_MAX_SIZE];
	struct sUtf8Stingray{
		unsigned short iDispBuf[2][20];	        //2x20x2 bypes
		unsigned char CursorXYPosition[4];			
		unsigned char ScreenState[2];			
	}sUtf8Stingray;
}uUTF8DISP;
#endif  //STINGRAY_UTF8_4321
//----------------------------------------------------------

#define STATIONNAME_MAX_SIZE 	40

typedef union uSName {						//20100128
	unsigned char StationName[90][STATIONNAME_MAX_SIZE];
	struct sSName{
		unsigned char IRadioStationName[30][STATIONNAME_MAX_SIZE];			
		unsigned char FMStationName[30][STATIONNAME_MAX_SIZE];			
		unsigned char DABStationName[30][STATIONNAME_MAX_SIZE];			
	}sSName;
}uSName;

typedef union uDSName {						//20100209
	unsigned char DStationName[90][STATIONNAME_MAX_SIZE+3];
}uDSName;

/* ---------------------------------------------------------------------------*/
#define USART1_DR_Base  	0x40013804
#define USART2_DR_Base  	0x40004404
#define ADC1_DR_Address    	((u32)0x4001244C)
#define SPI1_DR_Address  	0x4001300C
#define ADC3_DR_Address     (u32)(0x4001224C)
#define countof(a)   (sizeof(a) / sizeof(*(a)))

/* ---- include port definition header ---------------------------------------*/
#include "RKB_portdef.h"
/* ---------------------------------------------------------------------------*/


#endif /* __CONFIG_H */

/* end of file */
/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/

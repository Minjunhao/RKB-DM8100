/**************** (c)2008 Rotel Technology Global Inc..*************************
PROJECT         : Front Panel for Large VFD
COMPILER        : IAR Embeded Workbench 4.4.1
MODULE          : timer.h
VERSION         : V 1.0
USED CHIP       : stm32f10x
CREATION DATE   : /2008
AUTHORS         : An Kyung Chan / RTG
DESCRIPTION     :
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TIMER_H__
#define __TIMER_H__

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#define TIME10MS		10
#define TIME20MS		20
#define TIME30MS		30
#define TIME100MS		100
#define TIME125MS		125
#define TIME250MS		250
#define TIME500MS		500

#define TIME10MSTO20MS		2
#define TIME10MSTO30MS		3
#define TIME10MSTO40MS		4
#define TIME10MSTO50MS		5
#define TIME10MSTO60MS		6
#define TIME10MSTO70MS		7
#define TIME10MSTO80MS		8
#define TIME10MSTO90MS		9
#define TIME10MSTO100MS		10
#define TIME10MSTO150MS		15
#define TIME10MSTO180MS		18
#define TIME10MSTO200MS		20
#define TIME10MSTO250MS		25
#define TIME10MSTO300MS		30
#define TIME10MSTO400MS		40
#define TIME10MSTO500MS		50
#define TIME10MSTO600MS		60
#define TIME10MSTO700MS		70
#define TIME10MSTO800MS		80
#define TIME10MSTO900MS		90
#define TIME10MSTO1000MS	100
#define TIME10MSTO1500MS	150
#define TIME10MSTO2000MS	200
#define TIME10MSTO3000MS	300
#define TIME10MSTO4000MS	400

#define TIME10MSTO1SEC		100
#define TIME10MSTO2SEC		200
#define TIME10MSTO2700MS	270
#define TIME10MSTO3SEC		300
#define TIME10MSTO4SEC		400
#define TIME10MSTO5SEC		500
#define TIME10MSTO7SEC		700
#define TIME10MSTO20SEC		2000
// 20msec
#define TIME20MSTO20MS		20/20
#define TIME20MSTO40MS		40/20
#define TIME20MSTO60MS		60/20
#define TIME20MSTO80MS		80/20
#define TIME20MSTO100MS		100/20
#define TIME20MSTO120MS		120/20
#define TIME20MSTO140MS		140/20
#define TIME20MSTO160MS		160/20
#define TIME20MSTO180MS		180/20
#define TIME20MSTO200MS		200/20
#define TIME20MSTO240MS		240/20
#define TIME20MSTO260MS		260/20
#define TIME20MSTO300MS		300/20
#define TIME20MSTO340MS		340/20
#define TIME20MSTO400MS		400/20
#define TIME20MSTO500MS		500/20
#define TIME20MSTO600MS		600/20
#define TIME20MSTO700MS		700/20
#define TIME20MSTO800MS		800/20
#define TIME20MSTO1000MS	1000/20
#define TIME20MSTO1200MS	1200/20
#define TIME20MSTO1300MS	1300/20
#define TIME20MSTO1400MS	1400/20
#define TIME20MSTO1500MS	1500/20
#define TIME20MSTO1600MS	1600/20
#define TIME20MSTO1800MS	1800/20
#define TIME20MSTO2000MS	2000/20
#define TIME20MSTO2100MS	2100/20
#define TIME20MSTO2200MS	2200/20
#define TIME20MSTO2300MS	2300/20
#define TIME20MSTO2400MS	2400/20
#define TIME20MSTO2500MS	2500/20
#define TIME20MSTO3000MS	3000/20
#define TIME20MSTO5000MS	5000/20
#define TIME20MSTO6000MS	6000/20
// 100msec ±‚¡ÿ..
#define	COUNT100M_100MS		1
#define	COUNT100M_200MS		2
#define	COUNT100M_300MS		3
#define	COUNT100M_400MS		4
#define	COUNT100M_500MS		5
#define	COUNT100M_600MS		6
#define	COUNT100M_700MS		7
#define	COUNT100M_800MS		8
#define	COUNT100M_900MS		9
#define	COUNT100M_1000MS	10
#define	COUNT100M_1SEC		10
#define	COUNT100M_1_4SEC	14
#define	COUNT100M_1_5SEC	15
#define	COUNT100M_2SEC		20
#define	COUNT100M_2_1SEC	21
#define	COUNT100M_3SEC		30
#define	COUNT100M_4SEC		40
#define	COUNT100M_5SEC		50
#define	COUNT100M_6SEC		60
#define	COUNT100M_7SEC		70
#define	COUNT100M_10SEC		100
#define	COUNT100M_11SEC		110
#define	COUNT100M_12SEC		120
#define	COUNT100M_15SEC		150
#define	COUNT100M_20SEC		200

//125msec
#define	COUNT125M_250MS		2
#define	COUNT125M_500MS		4
#define	COUNT125M_1000MS	8
#define	COUNT125M_1250MS	10
#define	COUNT125M_2000MS	16
#define	COUNT125M_2125MS	17
#define	COUNT125M_3SEC		3000/125
#define	COUNT125M_3_5SEC	3500/125
#define	COUNT125M_4SEC		4000/125
#define	COUNT125M_5SEC		5000/125
#define	COUNT125M_6SEC		6000/125
#define	COUNT125M_7SEC		7000/125
#define	COUNT125M_8SEC		8000/125
#define	COUNT125M_9SEC		9000/125
#define	COUNT125M_10SEC		10000/125
#define	COUNT125M_12SEC		12000/125

//250msec
#define	COUNT250M_1SEC		4
#define	COUNT250M_1_5SEC	6
#define	COUNT250M_3SEC		12
#define	COUNT250M_5SEC		20
#define	COUNT250M_10SEC		40			//20060307
//500msec
#define	COUNT500M_10SEC		20



#define TIME300USTO40MS	134		
#define TIME300USTO30MS	100
#define TIME300USTO25MS	80		
#define TIME300USTO20MS	60		

#define COUNT_ENCCOUNT	TIME10MSTO50MS

extern __IO u32 LocalTime;

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void timer1_UP_IRQHandler(void);
void timer2_IRQHandler(void);
void timer_SysTickHandler(void);
void DelayMs(vu32 mSec);
void main_PowerDown_300us(void);
bool timer_Get10ms(void);
bool timer_Get20ms(void);
bool timer_Get100ms(void);
bool timer_Get125ms(void);
bool timer_Get250ms(void);
bool timer_Get500ms(void);

#endif /* __TIMER_H__ */

/* end of file */
/******************* (C) COPYRIGHT 2008 Rotel Technology Global **************/

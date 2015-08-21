/**************** (c)2008 Rotel Technology Global Inc..*************************
PROJECT         : RDG1520
COMPILER        : IAR Embeded Workbench 4.4.1
MODULE          : adckey.c
VERSION         : V 1.0
USED CHIP       : stm32f101x
CREATION DATE   : SEP 01,2008
AUTHORS         :  / RTG
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

#include "stingray.ext"
#include "dac.h"

/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
#define _MN_ "DAC: "

volatile byte b_work_dummy1;
volatile int b_work_16;

/*
	[ WM8740 DAC ]                                               		   	

	WM8740_nReset(x)		: 22(RSTB)
	WM8740_nML(x)			: 28(ML)
	WM8740_MC(x)			: 27(MC)
	WM8740_MD(x)			: 26(MD)
		
	M0 : 0000 000 LDL AL[7:0]								: Attenuation Left [ 0xff ]
	M1 : 0000 001 LDR AR[7:0]								: Attenuation Right[ 0xff ]
	M2 : 0000 010 0 	000 iw[1:0] OPE DEM MUT    	: iw[1:0]= 01 [ 24bit i2s ]
	M3 : 0000 011 izd SF[1:0] 0 REV SR0 ATC LRP I2S	: I2S=1
	M4 : 0000 110 0 	0 CDD DIFF[1:0] 0000 			: 
		
	M0 [ Left Vol  ] => 0000 0000 1111 1111 => 0x00ff 
	M1 [ Right Vol ] => 0000 0010 1111 1111 => 0x02ff
	
	M2 [ 24bit i2s ] => 0000 0100 0000 1000 => 0x0408 
	M3 [ I2S=1     ] => 0000 0100 0001 1000 => 0x0601
*********************************************************************/

void DAC_Delay(void)
{
	unsigned char i;
	for (i=0;i<3;++i) {};		//2us
}
void DAC_8740Init(void)
{
	WM8740_nReset(1);
	WM8740_nML(1);
	WM8740_MD(0);
	WM8740_MC(0);
}

void DAC_8740_Tx16(unsigned int data)
{
	unsigned char i;

	WM8740_nML(1);
	WM8740_MD(0);
	WM8740_MC(0);

	b_work_16 = data;
	i = 16;
	while(i != 0){
		if (b_work_16 & 0x8000) {			// ? MSB
     		WM8740_MD(1);
     	} else {
     	 	WM8740_MD(0);
     	}
     	 b_work_16 <<= 1;

		if(i == 1) WM8740_nML(0);
		DAC_Delay();
		WM8740_MC(1);
		DAC_Delay();
		WM8740_MC(0);
		if(i == 1) {
			DAC_Delay();
			WM8740_nML(1);
		}
		--i;
	};
}

/*************************************************************************************************/

#ifdef AK4353
/*
	I2C bus control [ AK4353 DAC ]                                               		   	

	p_4353_reset	[ Power Down = Low ]
	p_DACi2c_sda	[ = p_4353_sda ]
	p_DACi2c_scl	[ = p_4353_scl ]
*********************************************************************/

void I2C_Delay(void)
{
	unsigned char i;
//	for (i=0;i<3;++i) {};		//2us
	for (i=0;i< 20;++i) {};		//6us
}
void I2C_4353Init(void)
{
	DACReset(0);
	p_DACi2c_sda(1);
	p_DACi2c_scl(1);
     SetDACSDA;   //input port
	I2C_Delay();	//20060405
	I2C_Delay();
	DACReset(1);
}

void I2C_Start(void)
{
     ResDACSDA;   //output	port
	p_DACi2c_scl(1);
	p_DACi2c_sda(1);
	I2C_Delay();

	p_DACi2c_sda(0);
	I2C_Delay();

	p_DACi2c_scl(0);
	I2C_Delay();
}

void I2C_Stop(void)
{
	p_DACi2c_scl(0);
	I2C_Delay();

     ResDACSDA;   //output	port
	p_DACi2c_sda(0);
	I2C_Delay();

	p_DACi2c_scl(1);
	I2C_Delay();

	p_DACi2c_sda(1);
	I2C_Delay();

     SetDACSDA;   //input port
	I2C_Delay();
	I2C_Delay();
	I2C_Delay();
	I2C_Delay();
	I2C_Delay();
	I2C_Delay();
}


unsigned char I2C_GetAck(void)
{
	unsigned char data;

     SetDACSDA;   //input port

	I2C_Delay();
	p_DACi2c_scl(1);
	I2C_Delay();
	if(GetDACSDA == 0){
     		data = 0x00;
	} else {
    		data = 0x01;
	}
	I2C_Delay();
	p_DACi2c_scl(0);
	I2C_Delay();
	return(data);
}

void I2C_Tx8(unsigned char data)
{
	unsigned char i;

     ResDACSDA;   //output	port

	i = 8;
	b_work_dummy1 = data;

	while(i != 0){
		if (b_work_dummy1 & 0x80) {			// ? MSB
     		p_DACi2c_sda(1);
     	} else {
     	 	p_DACi2c_sda(0);
     	}
     	 b_work_dummy1 <<= 1;

		I2C_Delay();
		p_DACi2c_scl(1);
		I2C_Delay();
		p_DACi2c_scl(0);
		--i;
	};
}

/*
	AK4353 
	
	chip addr = 0x20	0010 /0/c1/c2/Rw/ [R/w=0=write only]
	reg. addr = 0x00	Control 1		[ 0000 0001 ]
		  = 0x01	Control 2
		  = 0x02	Control 3
		  = 0x03	Lch ATT
		  = 0x04	Rch ATT
		  = 0x05	TX
		  = 0x06	Channel Status 1
		  = 0x07	Channel Status 2
*********************************************************************/
void AK4353_SetWriteAll(u8 addr,u8 raddr,u8 data0,u8 data1,u8 data2,u8 data3,u8 data4,u8 data5,u8 data6,u8 data7)
{
	unsigned char dummy;

	I2C_Stop();
	I2C_Start();
	I2C_Tx8(addr);		//addr.		
	dummy = I2C_GetAck();
     
	I2C_Tx8(raddr);		//reg. addr
	dummy = I2C_GetAck();

	I2C_Tx8(data0);		//addr0.Control1 	[0/0/0/0/DIF2/DIF1/DIF0/RSTN]
	dummy = I2C_GetAck();

	I2C_Tx8(data1);		//addr1.Control2 	[0/0/DFS1/DFS0/CKS2/CKS1/CKS0/RSTN]
	dummy = I2C_GetAck();

	I2C_Tx8(data2);		//addr2.Control3 	[PL3/PL2/PL1/PL0/DEM1/DEM0/ATC/SMUTE]
	dummy = I2C_GetAck();

	I2C_Tx8(data3);		//addr3.Lch ATT 	[ATT7/ATT6/ATT5/ATT4/ATT3/ATT2/ATT1/ATT0]
	dummy = I2C_GetAck();
	
	I2C_Tx8(data4);		//addr4.Rch ATT 	[ATT7/ATT6/ATT5/ATT4/ATT3/ATT2/ATT1/ATT0]
	dummy = I2C_GetAck();

	I2C_Tx8(data5);		//addr5.TX	 	[0/0/0/0/0/0/V/TXE]
	dummy = I2C_GetAck();

	I2C_Tx8(data6);		//addr6.Channel Status1 [0/CS29/CS28/CS25/CS24/CS3/CS2/CS1]
	dummy = I2C_GetAck();

	I2C_Tx8(data7);		//addr7.Channel Status2 [CS15/CS14/CS13/CS12/CS11/CS10/CS9/CS8]
	dummy = I2C_GetAck();

	I2C_Stop();
}
void AK4353_SetWriteOne(u8 addr,u8 raddr,u8 dataX)
{
	unsigned char dummy;

	I2C_Stop();
	I2C_Start();
	I2C_Tx8(addr);		//addr.		
	dummy = I2C_GetAck();
     
	I2C_Tx8(raddr);		//reg. addr
	dummy = I2C_GetAck();

	I2C_Tx8(dataX);		//addr x
	dummy = I2C_GetAck();

	I2C_Stop();
}
/*
	AK4353_SetWriteOne(0x20,0x00,0x0b);			//64fs				// Control 1(0000/DIF2/DIF1/DIF0/RSTN)
	//RadioScape
	//AK4353_SetWriteOne(0x20,0x00,0x01);		//16bit >=32fs		// Control 1(0000/DIF2/DIF1/DIF0/RSTN)
	//AK4353_SetWriteOne(0x20,0x01,DabDacVolume_value);		// Control 2(00/DFS1/DFS0/CKS2/CKS1/CKS0/RSTN)
	//AK4353_SetWriteOne(0x20,0x02,DabDacVolume_value);		// Control 3(PL3/PL2/PL1/PL0/DEM1/DEM0/ATC/SMUTE)
	AK4353_SetWriteOne(0x20,0x03,(UNS_8)DabDacVolume_value);			// Lch ATT (ATT 7~0)
	AK4353_SetWriteOne(0x20,0x04,(UNS_8)DabDacVolume_value);			// Rch ATT (ATT 7~0)
	//AK4353_SetWriteOne(0x20,0x05,DabDacVolume_value);		// TX (000000/V/TXE)
	//AK4353_SetWriteOne(0x20,0x06,DabDacVolume_value);		// Channel Status1 (0/CS29/CS28/CS25/CS24/CS3/CS2/CS1)
	//AK4353_SetWriteOne(0x20,0x07,DabDacVolume_value);		// Channel Status1 (CS15/CS14/CS13/CS12/CS11/CS10/CS9/CS8)
*/
#endif /* AK4353 */


/* end of file */
/******************* (C) COPYRIGHT 2008 Rotel Technology Global **************/

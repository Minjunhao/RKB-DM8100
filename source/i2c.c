/**************** (c)2008 Rotel Technology Global Inc..*************************
PROJECT         : TFT Monitor with HDMI & YPbPr(CbCr) & YC & CVBS input
COMPILER        : IAR Embeded Workbench 5.50
MODULE          : i2c.c
VERSION         : V 1.0
USED CHIP       : stm32f101x
CREATION DATE   : JAN 01,2009
AUTHORS         : An Kyung Chan / RTG
DESCRIPTION     : 
*******************************************************************************/
#include "stm32f2xx.h"
#include "config.h"
#include "initmcu.h"
#include "uart.h"
#include "main.h"
//#include "main.h"
#include "timer.h"
#include "i2c.h"


//20140806 WORD=>int
int  EEP_Wait;				//sysTick 1msec Timer counter

byte_flags SysStatus;

#define bNoAck	SysStatus.byte_bit.bit0

#define EEPROM_PBYTE 16
// wait 7 ms
#define EEPROM_PTIME 	Delay_10u(7000/10);
// wait 12usec
#define HoldTime    	Delay_2u(12/2);
// wait 20usec
#define HoldTime1   	Delay_2u(20/2);

//--------------------------------------------------------
//Main : 10usec Timer counter
//--------------------------------------------------------
void Delay_2u(int count)
{ 
	int i,j;
	
	//Main delay     
	for (j=0;j<count;j++) {
		//2us for STM32F207
		for (i=0;i<9;i++) {
			__asm(" nop" );	
		};
	}    	
}
//--------------------------------------------------------
void Delay_10u(int count)
{ 
	int i,j;

	//Main delay
	for (j=0;j<count;j++) {
		//10us for STM32F207
		for (i=0;i<45;i++) {
			__asm(" nop" );	
		};
	}	    	
}
//--------------------------------------------------------
//--------------------------------------------------------
void IIC_Start(void)
{
	ResSDA;
    HoldTime;
	ResSCL;
    HoldTime;
}
//--------------------------------------------------------
void IIC_Stop(void)
{
	SetSDA;
    HoldTime;
	ResSDA;
    HoldTime;
	SetSCL;
    HoldTime;
	SetSDA;
    HoldTime;
}
//*******************************************************************************
//		Initial Software IIC
//*******************************************************************************
void Initial_I2C(void)
{
	EEP_Wait = 0;
	IIC_Stop();
}
//--------------------------------------------------------
void IIC_Send1Byte(BYTE senddata)
{
	BYTE i;

	for(i = 0; i < 8; i++)
	{
        if(senddata & 0x80) {
			SetSDA;
        } else{
			ResSDA;
        }
		senddata <<=1;		
        HoldTime;
		SetSCL;
        while(!GetSCL);
        HoldTime;
		ResSCL;	  
        HoldTime;
	}
	ResSCL;
    HoldTime;
	SetSDA;
    HoldTime;
	SetSCL;
    while(!GetSCL);
    HoldTime;
	if (GetSDA)
		bNoAck = 1;
	else
		bNoAck = 0;
	ResSCL;
    HoldTime;
}
//--------------------------------------------------------
BYTE IIC_Read1Byte(byte noack)
{
	BYTE i,readdata=0;

	for(i = 0; i < 8; i++)
	{
		readdata <<= 1;
		SetSDA;
        HoldTime;
		SetSCL;
        while(!GetSCL);
        HoldTime;
		if(GetSDA) 
		    readdata |= 0x01;
		ResSCL;
        HoldTime;
	}
	if(noack) {
		SetSDA;
	} else {
		ResSDA;
    }
    HoldTime;
	SetSCL;
    while(!GetSCL);
    HoldTime;
	ResSCL;
	return readdata;
}
//--------------------------------------------------------
//	IIC write address
//--------------------------------------------------------
void IICSW_Write(BYTE id,BYTE addr,BYTE *databuf,WORD number)
{
	int i;
	BYTE count = 0;

    do
    {
    	count++;
        // Stop
    	IIC_Stop();
        HoldTime1;
    
        // Start
    	IIC_Start();
    
        // Slave ID	
    	IIC_Send1Byte(id); 
    
    	if (bNoAck) continue;
    
        // Address
    	IIC_Send1Byte(addr);
    
       	if (bNoAck)	continue;
    
        // data
    	for(i = 0; i < number; i++)
    	{
    		IIC_Send1Byte(databuf[i]);
       		if(bNoAck) break;
    	}
    	if (bNoAck) continue;
    
        // Stop
        HoldTime1;
    	IIC_Stop();
    }
    while(bNoAck && count < 10);
}
//*******************************************************************************
//                                      Software IIC Read Function
//*******************************************************************************
void IICSW_Read(BYTE id,BYTE addr,BYTE *databuf,WORD number)
{
	int i;
	BYTE count = 0;

	do
	{
		count++;
		
	    // Stop
		IIC_Stop();
        HoldTime1;
	
	    // Start
		IIC_Start();
	
	    // Slave ID	
		IIC_Send1Byte(id); 
	    if (bNoAck) continue;
	
	    // Address
		IIC_Send1Byte(addr);
	    if (bNoAck) continue;

	    IIC_Stop();  
        HoldTime1;

	    // Start
		IIC_Start();
	
	    // Slave ID 
		IIC_Send1Byte(id | 0x01);
	    if (bNoAck) continue;

		ResSDA;
		ResSCL; 
		
	    // data
		for(i = 0; i < number-1; i++)
		{
			databuf[i] = IIC_Read1Byte(0);
		}
		databuf[i] = IIC_Read1Byte(1);
	    if (bNoAck) continue;
	
	    // Stop
		IIC_Stop();
	}
	while(bNoAck && count < 10);
}
//--------------------------------------------------------
void EEPROM_Write(WORD addr,BYTE *databuf,WORD number)
{
	BYTE id,temp;
	WORD i;
	union W_TEMP1 addr1;

	addr1.valW = addr;
	i = 0;
	
	while( number > 0)
	{
		id = I2C_EEP_ADDR + (addr1.valB[1] << 1);
		temp = EEPROM_PBYTE -(addr1.valB[0]&(EEPROM_PBYTE-1));
		if( temp > number)
			temp = number;

		IICSW_Write(id,addr1.valB[0],&databuf[i],temp);
		
		EEPROM_PTIME;
	
		number -= temp;
		addr1.valW += temp;
		i += temp;
	}
}
//--------------------------------------------------------
void EEPROM_Read(WORD addr,BYTE *databuf,WORD number)
{
	BYTE id;
	union W_TEMP1 addr1;
	
	addr1.valW = addr;
	id = I2C_EEP_ADDR + (addr1.valB[1] << 1);
	IICSW_Read(id,addr1.valB[0],databuf,number);
}


/* end of file */
/******************* (C) COPYRIGHT 2008 Rotel Technology Global *********************/

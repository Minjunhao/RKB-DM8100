/**************** (c)2008 Rotel Technology Global Inc..*************************
PROJECT         : TFT Monitor with HDMI & YPbPr(CbCr) & YC & CVBS input
COMPILER        : IAR Embeded Workbench 5.50
MODULE          : i2c.c
VERSION         : V 1.0
USED CHIP       : stm32f101x
CREATION DATE   : / 2013
AUTHORS         : / Rotel
DESCRIPTION     : 
*******************************************************************************/
#include "stm32f10x_lib.h"
#include "config.h"
#include "initmcu.h"
#include "uart.h"
#include "main.h"
#include "timer.h"
#include "i2c.h"

#define EEPROM_PBYTE 16
//#define EEPROM_PTIME 7     // wait 7 ms
#define EEPROM_PTIME 20000 

#define HoldTime    	Short_Delay(12);
#define HoldTime1   	Short_Delay(15);

WORD  EEP_Wait;
byte_flags SysStatus;

#define bNoAck	SysStatus.byte_bit.bit0

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
//                                      Initial Software IIC
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

		//while(EEP_Wait > 0);
		while(EEP_Wait > 0) {
			Short_Delay(EEP_Wait);
			EEP_Wait = 0;
		}
		IICSW_Write(id,addr1.valB[0],&databuf[i],temp);
		EEP_Wait = EEPROM_PTIME;
	
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
	//while(EEP_Wait > 0);
	while(EEP_Wait > 0) {
		Short_Delay(EEP_Wait);
		EEP_Wait = 0;
	}
	IICSW_Read(id,addr1.valB[0],databuf,number);
}
//--------------------------------------------------------
void Short_Delay(word Time)
{ 
	while(Time --);
}
//--------------------------------------------------------


/* end of file */
/******************* (C) COPYRIGHT 2008 Rotel Technology Global *********************/

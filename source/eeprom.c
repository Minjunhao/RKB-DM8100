/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "config.h"
#include "initmcu.h"
#include "main.h"
//#include "main.h"
#include "uart.h"
#include "timer.h"

#include "eeprom.h"
#include "eeprom.mem"
#include "i2c.h"

/*----------------------------------------------------------------------------*/
#define _MN_ "UART: "
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
void Set_Delay_2u(int count)
{
    int i,j;
     
	for (j=0;j<count;j++) {
		//2us for STM32F207
		for (i=0;i<9;i++) {
			__asm(" nop" );	
		};
	}
}
/*----------------------------------------------------------------------------*/
void iic_Delay(void)
{
	Set_Delay_2u(12/2);		//12usec
}

#if 0
/*-----------------------------------------------------------------------------
 Function Name  : CMedia eeprom_Init
 Description    : initialize for header and tail
 Input          : None
 Output         : None
 Return         : None
 Comments	 	: ..
 
    CM_i2c_sda();
    CM_i2c_scl();
    
    CM_SetSDA;  //input port
    CM_ResSDA;  //output port
    CM_GetSDA;
    CM_SetSCL;  //
    CM_ResSCL;  //
    CM_GetSCL;
-----------------------------------------------------------------------------*/								
void CMiic_Init(void)
{
    CM_i2c_sda(1);
    CM_i2c_scl(1);
    CM_SetSDA;   //input port
}
void CMiic_Start(void)
{
     CM_ResSDA;   //output	port
     CM_i2c_scl(1);
     CM_i2c_sda(1);
     iic_Delay();

     CM_i2c_sda(0);
     iic_Delay();

     CM_i2c_scl(0);
     iic_Delay();
}
void CMiic_Stop(void)
{
     CM_i2c_scl(0);
     iic_Delay();

     CM_ResSDA;   //output	port
     CM_i2c_sda(0);
     iic_Delay();

     CM_i2c_scl(1);
     iic_Delay();

     CM_i2c_sda(1);
     iic_Delay();

     CM_SetSDA;   //input port
     iic_Delay();
}
void CM_StopWrite(void)
{
     CMiic_Stop();
     //Set_Delay_2u(7000/2);	//20111128
}
//  read data-8bit
unsigned char CMiic_getch(void)
{
     unsigned char i;

     CM_SetSDA;   //input port
     iic_Delay();
     b_work_dummy = 0;

     i = 8;
     while(i != 0){
          CM_i2c_scl(1);
          iic_Delay();
          b_work_dummy <<= 1;
			 if (CM_GetSDA == 1) b_work_dummy |= 0x01;
          CM_i2c_scl(0);
          iic_Delay();
          --i;
     };
     return(b_work_dummy);
}
//  read Ack
unsigned char CMiic_getAck(void)
{
     unsigned char data;

     CM_SetSDA;   //input port
     iic_Delay();
     CM_i2c_scl(1);
     iic_Delay();
	  if (CM_GetSDA == 0x00){	//? SDA
          data = 0x00;
     } else {
          data = 0x01;
     }
     iic_Delay();
     CM_i2c_scl(0);
     iic_Delay();
     return(data);
}
//  write data-8bit
void CMiic_putch(unsigned char data)
{
     unsigned char i;

     CM_ResSDA;   //output	port

     i = 8;
     b_work_dummy = data;

     while(i != 0){
		if (b_work_dummy & 0x80) {			// ? MSB
			CM_i2c_sda(1);
		} else {
			CM_i2c_sda(0);
		}
		b_work_dummy <<= 1;
		iic_Delay();
		CM_i2c_scl(1);
		iic_Delay();
		CM_i2c_scl(0);
		iic_Delay();
          --i;
     };
}
//  write Ack=0 (Ack)
void CMiic_putAck(void)
{
     CM_ResSDA;   //output	port

     CM_i2c_sda(0);
     iic_Delay();
     CM_i2c_scl(1);
     iic_Delay();
     CM_i2c_scl(0);
     iic_Delay();
}
//  write Ack=1 (NoAck)
void CMiic_putNoAck(void)
{
     CM_ResSDA;   //output	port

     CM_i2c_sda(1);
     iic_Delay();
     CM_i2c_scl(1);
     iic_Delay();
     CM_i2c_scl(0);
     iic_Delay();
}

//----------------------------------------------------------
// set reg_addr
//----------------------------------------------------------
void CM_SetWrite(unsigned char addr)
{
     unsigned char count, dummy;

     count = 5;
     while(count != 0){
          CMiic_Start();
          CMiic_putch(0x10);        //iic-addr-write command
          if(CMiic_getAck() == 0){  //read Ack
               CMiic_putch(addr);   //write 8it
               dummy = CMiic_getAck();  //read Ack
               return;
          }
          CMiic_Stop();
          --count;
     };
}
//----------------------------------------------------------
//  read start-2
//----------------------------------------------------------
void CM_SetRead(unsigned char addr)
{
     unsigned char dummy;

     CM_SetWrite(addr);         //start, 0x10W+Ack, reg_addr+Ack
     CMiic_Stop();              //stop
//
     CMiic_Start();             //start
     CMiic_putch(0x11);         //0x10R		
     dummy = CMiic_getAck();    //read Ack
}

unsigned char CM_GetData(void)
{
     unsigned char data;

     data = CMiic_getch();
     CMiic_putAck();
     return(data);
}

unsigned char CM_GetData_NoAck(void)
{
     unsigned char data;

     data = CMiic_getch();
     CMiic_putNoAck();
     return(data);
}
//----------------------------------------------------------
//  write start-2
//----------------------------------------------------------
void CM_PutData(unsigned char data)
{
     unsigned char dummy;

     CMiic_putch(data);
     dummy = CMiic_getAck();
}
//--------------------------------------------------------------------
//
//  CMedia reading Start-1
//      2 byte
//      CM_xGetData(0x00, &data1, &data2);  //(r) channels, frequency, bit-depth
//      CM_xGetData(0xfd, &data1, &data2);  //(r/w) interrupt Mask
//      CM_xGetData(0xfe, &data1, &data2);  //(r) interrupt status
//      CM_xGetData(0xff, &data1, &data2);  //(w) HID status
//
//20130111 : 
//84 USB SUSPEND read : 20130111 => High : i2c suspend
//#define GetCmI2cSuspend         GPIO_ReadInputDataBit(CMEDIA_PORT, CMEDIA_i2cSUSPEND)
//
//	read XMOS buffer
//		reg_00 :(ro)  : ****, mm**, *fff, f*** : mm=audio mode, ffff=frequency [default 48K/24bit]
//		reg_01 :(ro)  : Version 01.23 : 0000, 0001, 0010, 0011
//		reg_02 :(0xFC):(ro) : ****, ****, ****, ***,SwUpgrade
//		reg_03 :(0xFD):(r/w): ****, ****, ****, ***,IntMask
//		reg_04 :(0xFE):(r0) : ****, ****, ****,***,Assert
//		reg_05 :(0xFF):(wo) : ****, *,Random,Repeat,MuteRecord, Previous,Next,Stop,Pause, Play,MutePlay,VolDown,VolUp
//--------------------------------------------------------------------
void CM_xGetData(unsigned char addr, unsigned char *data1, unsigned char *data2)
{
     CM_SetRead(addr);
     *data1 = CM_GetData();            //data_L
     *data2 = CM_GetData_NoAck();      //data_H
     CMiic_Stop();
     return;
}
//--------------------------------------------------------------------
//
//  CMedia writing Start-1
//      2 byte
//      CM_xGetData(0x00, &data1, &data2);  //(r) channels, frequency, bit-depth
//      CM_xGetData(0xfd, &data1, &data2);  //(r/w) interrupt Mask
//      CM_xGetData(0xfe, &data1, &data2);  //(r) interrupt status
//      CM_xGetData(0xff, &data1, &data2);  //(w) HID status
//--------------------------------------------------------------------
void CM_xPutData(unsigned char addr, unsigned char data1, unsigned char data2)
{
     CM_SetWrite(addr);
     CM_PutData(data1);
     CM_PutData(data2);
     CM_StopWrite();
}
#endif
/*-----------------------------------------------------------------------------
 Function Name  : eeprom_Init
 Description    : initialize for header and tail
 Input          : None
 Output         : None
 Return         : None
 Comments	 	: ..
-----------------------------------------------------------------------------*/								

void nvr_Init(void)
{
     p_i2c_sda(1);
     p_i2c_scl(1);
     SetSDA;   //input port
}

void iic_Init(void)
{
     p_i2c_sda(1);
     p_i2c_scl(1);
     SetSDA;   //input port
}


void iic_Start(void)
{
     ResSDA;   //output	port
     p_i2c_scl(1);
     p_i2c_sda(1);
     iic_Delay();

     p_i2c_sda(0);
     iic_Delay();

     p_i2c_scl(0);
     iic_Delay();
}

void nvr_Stop(void)
{
     p_i2c_scl(0);
     iic_Delay();

     ResSDA;   //output	port
     p_i2c_sda(0);
     iic_Delay();

     p_i2c_scl(1);
     iic_Delay();

     p_i2c_sda(1);
     iic_Delay();

     SetSDA;   //input port
     iic_Delay();
}

void iic_Stop(void)
{
     p_i2c_scl(0);
     iic_Delay();

     ResSDA;   //output	port
     p_i2c_sda(0);
     iic_Delay();

     p_i2c_scl(1);
     iic_Delay();

     p_i2c_sda(1);
     iic_Delay();

     SetSDA;   //input port
     iic_Delay();
}

void nvr_StopWrite(void)
{
     iic_Stop();
     Set_Delay_2u(7000/2);
}

void iic_StopWrite(void)
{
     iic_Stop();
     Set_Delay_2u(7000/2);
}

unsigned char iic_getch(void)
{
     unsigned char i;

     SetSDA;   //input port
     iic_Delay();
     b_work_dummy = 0;

     i = 8;
     while(i != 0){
          p_i2c_scl(1);
          iic_Delay();
          b_work_dummy <<= 1;
			 if (GetSDA == 1) b_work_dummy |= 0x01;
          p_i2c_scl(0);
          iic_Delay();
          --i;
     };
     return(b_work_dummy);
}

unsigned char iic_getAck(void)
{
     unsigned char data;

     SetSDA;   //input port

     iic_Delay();
     p_i2c_scl(1);
     iic_Delay();
	  if (GetSDA == 0x00){	//? SDA
          data = 0x00;
     } else {
          data = 0x01;
     }
     iic_Delay();
     p_i2c_scl(0);
     iic_Delay();
     return(data);
}

void iic_putch(unsigned char data)
{
     unsigned char i;

     ResSDA;   //output	port

     i = 8;
     b_work_dummy = data;

     while(i != 0){
		if (b_work_dummy & 0x80) {			// ? MSB
     		p_i2c_sda(1);
     	} else {
     		p_i2c_sda(0);
     	}
     	b_work_dummy <<= 1;
		iic_Delay();
		p_i2c_scl(1);
		iic_Delay();
		p_i2c_scl(0);
		iic_Delay();
		--i;
     };
}

void iic_putAck(void)
{
     ResSDA;   //output	port

     p_i2c_sda(0);
     iic_Delay();
     p_i2c_scl(1);
     iic_Delay();
     p_i2c_scl(0);
     iic_Delay();
}

void iic_putNoAck(void)
{
     ResSDA;   //output	port

     p_i2c_sda(1);
     iic_Delay();
     p_i2c_scl(1);
     iic_Delay();
     p_i2c_scl(0);
     iic_Delay();
}

void nvr_SetWrite(unsigned char addr)
{
     unsigned char count, dummy;

     count = 5;
     while(count != 0){
          iic_Start();
          iic_putch(0xa0);    //write command
          if(iic_getAck() == 0){
               iic_putch(addr);
               dummy = iic_getAck();
               return;
          }
          iic_Stop();
          --count;
     };
}


/* chd s */
//page7 : 20060201
void nvr_SetWritePage7(unsigned char addr)
{
     unsigned char count, dummy;

     count = 5;
     while(count != 0){
          iic_Start();
          iic_putch(0xae);    //write command(page7)
          if(iic_getAck() == 0){
               iic_putch(addr);
               dummy = iic_getAck();
               return;
          }
          iic_Stop();
          --count;
     };
}
void nvr_SetReadPage7(unsigned char addr)
{
     unsigned char dummy;

     nvr_SetWritePage7(addr);			//page7
     iic_Stop();
     iic_Start();
     iic_putch(0xaf);		//read page7
     dummy = iic_getAck();
}
//page6: 20060201
void nvr_SetWritePage6(unsigned char addr)
{
     unsigned char count, dummy;

     count = 5;
     while(count != 0){
          iic_Start();
          iic_putch(0xac);    //write command(page6)
          if(iic_getAck() == 0){
               iic_putch(addr);
               dummy = iic_getAck();
               return;
          }
          iic_Stop();
          --count;
     };
}
void nvr_SetReadPage6(unsigned char addr)
{
     unsigned char dummy;

     nvr_SetWritePage6(addr);			//page6
     iic_Stop();
     iic_Start();
     iic_putch(0xad);		//read page6
     dummy = iic_getAck();
}
//page5: 20060201
void nvr_SetWritePage5(unsigned char addr)
{
     unsigned char count, dummy;

     count = 5;
     while(count != 0){
          iic_Start();
          iic_putch(0xaa);    //write command(page5)
          if(iic_getAck() == 0){
               iic_putch(addr);
               dummy = iic_getAck();
               return;
          }
          iic_Stop();
          --count;
     };
}
void nvr_SetReadPage5(unsigned char addr)
{
     unsigned char dummy;

     nvr_SetWritePage5(addr);			//page5
     iic_Stop();
     iic_Start();
     iic_putch(0xab);		//read page5
     dummy = iic_getAck();
}
//page4: 20060201
void nvr_SetWritePage4(unsigned char addr)
{
     unsigned char count, dummy;

     count = 5;
     while(count != 0){
          iic_Start();
          iic_putch(0xa8);    //write command(page4)
          if(iic_getAck() == 0){
               iic_putch(addr);
               dummy = iic_getAck();
               return;
          }
          iic_Stop();
          --count;
     };
}
void nvr_SetReadPage4(unsigned char addr)
{
     unsigned char dummy;

     nvr_SetWritePage4(addr);		//page4
     iic_Stop();
     iic_Start();
     iic_putch(0xa9);		//read page4
     dummy = iic_getAck();
}
//page3: 20060201
void nvr_SetWritePage3(unsigned char addr)
{
     unsigned char count, dummy;

     count = 5;
     while(count != 0){
          iic_Start();
          iic_putch(0xa6);    //write command(page3)
          if(iic_getAck() == 0){
               iic_putch(addr);
               dummy = iic_getAck();
               return;
          }
          iic_Stop();
          --count;
     };
}
void nvr_SetReadPage3(unsigned char addr)
{
     unsigned char dummy;

     nvr_SetWritePage3(addr);
     iic_Stop();
     iic_Start();
     iic_putch(0xa7);		//read page3
     dummy = iic_getAck();
}
//page2
void nvr_SetWritePage2(unsigned char addr)
{
     unsigned char count, dummy;

     count = 5;
     while(count != 0){
          iic_Start();
          iic_putch(0xa4);    //write command(page2)
          if(iic_getAck() == 0){
               iic_putch(addr);
               dummy = iic_getAck();
               return;
          }
          iic_Stop();
          --count;
     };
}
void nvr_SetReadPage2(unsigned char addr)
{
     unsigned char dummy;

     nvr_SetWritePage2(addr);
     iic_Stop();
     iic_Start();
     iic_putch(0xa5);		//read page2
     dummy = iic_getAck();
}
//page1
void nvr_SetWritePage1(unsigned char addr)
{
     unsigned char count, dummy;

     count = 5;
     while(count != 0){
          iic_Start();
          iic_putch(0xa2);    //write command(page1)
          if(iic_getAck() == 0){
               iic_putch(addr);
               dummy = iic_getAck();
               return;
          }
          iic_Stop();
          --count;
     };
}
void nvr_SetReadPage1(unsigned char addr)
{
     unsigned char dummy;

     nvr_SetWritePage1(addr);
     iic_Stop();
     iic_Start();
     iic_putch(0xa3);		//read page 1
     dummy = iic_getAck();
}
//page0
void nvr_SetWritePage0(unsigned char addr)
{
     unsigned char count, dummy;

     count = 5;
     while(count != 0){
          iic_Start();
          iic_putch(0xa0);    //write command(page0)
          if(iic_getAck() == 0){
               iic_putch(addr);
               dummy = iic_getAck();
               return;
          }
          iic_Stop();
          --count;
     };
}
void nvr_SetReadPage0(unsigned char addr)
{
     unsigned char dummy;

     nvr_SetWritePage0(addr);
     iic_Stop();
     iic_Start();
     iic_putch(0xa1);		//read page 0
     dummy = iic_getAck();
}
//
//
//

void nvr_PutDataPage0(void)	/* 256 byte */
{
	unsigned char i,j;

	for(i=0;i<0x10;i++) {

		nvr_SetWrite(i*16);		/* Page 0 */
		for(j=0;j<0x10;++j) {
			nvr_PutData(buffer_eeprom[i*16+j]);
		};
		iic_Stop();
	};
	return;
}
void nvr_PutDataPage1(void)	/* 256 byte */
{
	unsigned char i,j;

	for(i=0;i<0x10;i++) {

		nvr_SetWritePage1(i*16);		/* Page 1 */
		for(j=0;j<0x10;++j) {
			nvr_PutData(buffer_eeprom[i*16+j]);
		};
		iic_Stop();
	};
	return;
}
void nvr_PutDataPage2(void)	/* 256 byte */
{
	unsigned char i,j;

	for(i=0;i<0x10;i++) {

		nvr_SetWritePage2(i*16);		/* Page 2 */
		for(j=0;j<0x10;++j) {
			nvr_PutData(buffer_eeprom[i*16+j]);
		};
		iic_Stop();
	};
	return;
}
void nvr_PutDataPage3(void)	//20060201	:  256 byte
{
	unsigned char i,j;

	for(i=0;i<0x10;i++) {

		nvr_SetWritePage3(i*16);		/* Page 3*/
		for(j=0;j<0x10;++j) {
			nvr_PutData(buffer_eeprom[i*16+j]);
		};
		iic_Stop();
	};
	return;
}
void nvr_PutDataPage4(void)	//20060201	:  256 byte
{
	unsigned char i,j;

	for(i=0;i<0x10;i++) {

		nvr_SetWritePage4(i*16);		/* Page 4*/
		for(j=0;j<0x10;++j) {
			nvr_PutData(buffer_eeprom[i*16+j]);
		};
		iic_Stop();
	};
	return;
}
void nvr_PutDataPage5(void)	//20060201	:  256 byte
{
	unsigned char i,j;

	for(i=0;i<0x10;i++) {

		nvr_SetWritePage5(i*16);		/* Page 5*/
		for(j=0;j<0x10;++j) {
			nvr_PutData(buffer_eeprom[i*16+j]);
		};
		iic_Stop();
	};
	return;
}
void nvr_PutDataPage6(void)	//20060201	:  256 byte
{
	unsigned char i,j;

	for(i=0;i<0x10;i++) {

		nvr_SetWritePage6(i*16);		/* Page 6*/
		for(j=0;j<0x10;++j) {
			nvr_PutData(buffer_eeprom[i*16+j]);
		};
		iic_Stop();
	};
	return;
}
void nvr_PutDataPage7(void)	//20060201	:  256 byte
{
	unsigned char i,j;

	for(i=0;i<0x10;i++) {

		nvr_SetWritePage7(i*16);		/* Page 7*/
		for(j=0;j<0x10;++j) {
			nvr_PutData(buffer_eeprom[i*16+j]);
		};
		iic_Stop();
	};
	return;
}
void nvr_GetDataPage0(void)	/* 256 byte */
{
	unsigned char i;

	nvr_SetRead(0);		/* Page 0 */

	for(i=0;i<0xff;i++) {
		buffer_eeprom[i] = nvr_GetData();
	};
	buffer_eeprom[i] = nvr_GetData_NoAck();

	iic_Stop();
	return;
}
void nvr_GetDataPage1(void)	/* 256 byte */
{
	unsigned char i;

	nvr_SetReadPage1(0);	/* Page 1 */

	for(i=0;i<0xff;i++) {
		buffer_eeprom[i] = nvr_GetData();
	};
	buffer_eeprom[i] = nvr_GetData_NoAck();

	iic_Stop();
	return;
}
void nvr_GetDataPage2(void)	/* 256 byte */
{
	unsigned char i;

	nvr_SetReadPage2(0);	/* Page 2 */

	for(i=0;i<0xff;i++) {
		buffer_eeprom[i] = nvr_GetData();
	};
	buffer_eeprom[i] = nvr_GetData_NoAck();

	iic_Stop();
	return;
}
void nvr_GetDataPage3(void)	/* 256 byte */
{
	unsigned char i;

	nvr_SetReadPage3(0);	/* Page 3 */

	for(i=0;i<0xff;i++) {
		buffer_eeprom[i] = nvr_GetData();
	};
	buffer_eeprom[i] = nvr_GetData_NoAck();

	iic_Stop();
	return;
}
void nvr_GetDataPage4(void)	//20060201	:  256 byte
{
	unsigned char i;

	nvr_SetReadPage4(0);	/* Page 4 */

	for(i=0;i<0xff;i++) {
		buffer_eeprom[i] = nvr_GetData();
	};
	buffer_eeprom[i] = nvr_GetData_NoAck();

	iic_Stop();
	return;
}
void nvr_GetDataPage5(void)	//20060201	: 256 byte
{
	unsigned char i;

	nvr_SetReadPage5(0);	/* Page 5 */

	for(i=0;i<0xff;i++) {
		buffer_eeprom[i] = nvr_GetData();
	};
	buffer_eeprom[i] = nvr_GetData_NoAck();

	iic_Stop();
	return;
}
void nvr_GetDataPage6(void)	//20060201	:  256 byte
{
	unsigned char i;

	nvr_SetReadPage6(0);	/* Page 6 */

	for(i=0;i<0xff;i++) {
		buffer_eeprom[i] = nvr_GetData();
	};
	buffer_eeprom[i] = nvr_GetData_NoAck();

	iic_Stop();
	return;
}
void nvr_GetDataPage7(void)	//20060201	: 256 byte
{
	unsigned char i;

	nvr_SetReadPage7(0);	/* Page 7 */

	for(i=0;i<0xff;i++) {
		buffer_eeprom[i] = nvr_GetData();
	};
	buffer_eeprom[i] = nvr_GetData_NoAck();

	iic_Stop();
	return;
}
/* chd e */



void nvr_SetRead(unsigned char addr)
{
     unsigned char dummy;

     nvr_SetWrite(addr);
     iic_Stop();
     iic_Start();
     iic_putch(0xa1);		
     dummy = iic_getAck();
}

unsigned char nvr_GetData(void)
{
     unsigned char data;

     data = iic_getch();
     iic_putAck();
     return(data);
}

unsigned char nvr_GetData_NoAck(void)
{
     unsigned char data;

     data = iic_getch();
     iic_putNoAck();
     return(data);
}

void nvr_PutData(unsigned char data)
{
     unsigned char dummy;

     iic_putch(data);
     dummy = iic_getAck();
}
//call main
unsigned char nvr_xGetData(unsigned char addr)
{
     unsigned char data;

     nvr_SetRead(addr);
     data = nvr_GetData_NoAck();
     nvr_Stop();
     return(data);
}
//hth
unsigned char nvr_xGetDataPage1(unsigned char addr)
{
     unsigned char data;

     nvr_SetReadPage1(addr);
     data = nvr_GetData_NoAck();
     nvr_Stop();
     return(data);
}
//call main
void nvr_xPutData(unsigned char addr, unsigned char data)
{
     nvr_SetWrite(addr);
     nvr_PutData(data);
     nvr_StopWrite();
}
//hth
void nvr_xPutDataPage1(unsigned char addr, unsigned char data)
{
     nvr_SetWritePage1(addr);
     nvr_PutData(data);
     nvr_StopWrite();
}

unsigned char nvr_GetChksum(unsigned char start_addr, unsigned char end_addr)
{
     unsigned char i, sum, data;


     sum = 0;
     nvr_SetRead(start_addr);

     i = start_addr;
     while(i <= end_addr){
          if(i == end_addr){
               data = nvr_GetData_NoAck();
          } else {
               data = nvr_GetData();
          }
          sum += data;
          ++i;
     };
     iic_Stop();
     return(sum);
}
//chd : 20060203
unsigned char nvr_GetChksumPage0(unsigned char start_addr, unsigned char end_addr)
{
     unsigned char i, sum, data;


     sum = 0;
     nvr_SetReadPage0(start_addr);

     i = start_addr;
     while(i <= end_addr){
          if(i == end_addr){
               data = nvr_GetData_NoAck();
          } else {
               data = nvr_GetData();
          }
          sum += data;
          ++i;
     };
     iic_Stop();
     return(sum);
}
//hth
unsigned char nvr_GetChksumPage1(unsigned char start_addr, unsigned char end_addr)
{
     unsigned char i, sum, data;


     sum = 0;
     nvr_SetReadPage1(start_addr);

     i = start_addr;
     while(i <= end_addr){
          if(i == end_addr){
               data = nvr_GetData_NoAck();
          } else {
               data = nvr_GetData();
          }
          sum += data;
          ++i;
     };
     iic_Stop();
     return(sum);
}
//chd : 20060203
#define EEPROM_ADDR_START     0xb0
#define EEPROM_ADDR_PTY       0xbe
#define EEPROM_ADDR_CHKSUM    0xbf

unsigned char nvr_CheckChksumPage0(void)
{
     unsigned char checksum, checksum1;

     checksum = nvr_GetChksum(EEPROM_ADDR_START, EEPROM_ADDR_PTY);
     nvr_SetRead(EEPROM_ADDR_CHKSUM);
     checksum1 = nvr_GetData_NoAck();
     iic_Stop();

     checksum ^= 0xff;
     if(checksum != checksum1){
          return(0x01);
     } else {
          return(0x00);
     }
}
//hth
unsigned char nvr_CheckChksumPage1(void)
{
     unsigned char checksum, checksum1;

     checksum = nvr_GetChksumPage1(EEPROM_ADDR_START, EEPROM_ADDR_PTY);
     nvr_SetReadPage1(EEPROM_ADDR_CHKSUM);
     checksum1 = nvr_GetData_NoAck();
     iic_Stop();

     checksum ^= 0xff;
     if(checksum != checksum1){
          return(0x01);
     } else {
          return(0x00);
     }
}

void nvr_AdjChksum(void)
{
     unsigned char checksum;

     checksum = nvr_GetChksum(EEPROM_ADDR_START, EEPROM_ADDR_PTY);
     checksum ^= 0xff;

     nvr_SetWrite(EEPROM_ADDR_CHKSUM);
     nvr_PutData(checksum);
     iic_Stop();
}
//hth
void nvr_AdjChksumPage1(void)
{
     unsigned char checksum;

     checksum = nvr_GetChksumPage1(EEPROM_ADDR_START, EEPROM_ADDR_PTY);
     checksum ^= 0xff;

     nvr_SetWritePage1(EEPROM_ADDR_CHKSUM);
     nvr_PutData(checksum);
     iic_Stop();
}
/* end of file */
/******************* (C) COPYRIGHT 2008 Rotel Technology Global **************/

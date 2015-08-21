#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "initmcu.h"
#include "main.h"
#include "uart.h"
#include "timer.h"

#include "timer.h"

#include "stn_module.h"
#include "stn_module.mem"

#include "function.ext"

/*
	TC2004A-03	: 4x20
	TC2002A-03	: 2x20
		
	1/16 duty cycle
	1/5 Bias
	
	5x8 dots
	
	SPLC780D1 : [ 16 COM / 40 SEG ]	
		
	RS=0 : Instruction Register (for write)
	RS=1 : Data Register (for read and write)
		
	R/W=0 : write
	R/W=1 : read
						
	E : a start signal ( for reading or writing data ) : enable port
	
	DB7~0 : data bus
		
	RS	R/W	DB7 ~ DB0
	0	0	: IR write (display clear, etc)
	0	1	: read BF(Busy Flag)(DB7) and AC(address counter)(DB6~DB0) 			
	1	0	: DR write -> Display data RAM and Character generator RAM
	1	1	: DR read <-- Display data RAM and Character generator RAM
		
		BF(busy flag) : [ RS=0, R/W=1 ] : If (DB7==1), busy.
		AC(address counter) : assign address DB6~DB0 로 읽을 수 있다.
		


	=> set character generator RAM address :(aa aaaa)2
		RS	R/W DB7 DB6 DB5 DB4 DB3 DB2 DB1 DB0
		0	0	0	1	a	a	a	a	a	a	
			
	=> set display data RAM address :(aaa aaaa)2
		- one line(N=0) : 0x00 ~ 0x4f (80)
		- two line(N=1) : 0x00 ~ 0x27 (40)
						  0x40 ~ 0x67 (40)	
		RS	R/W DB7 DB6 DB5 DB4 DB3 DB2 DB1 DB0
		0	0	1	a	a	a	a	a	a	a	
	=> set function
		RS	R/W DB7 DB6 DB5 DB4 DB3 DB2 DB1 DB0
		0	0	0	0	1	DL	N	F	x	x
		- DL(Data Length)
			DL=0 : 4bit(DB7~4)
			DL=1 : 8bit(DB7~0)
		- N(Display line)
			N=0 : one-line
			N=1 : two-line
		- F(charater Font)
			F=0 : 5x8 dots character font
			F=1 : 5x10 dots
		
*********************************************************************/


/*0x80 ~ 8f
 {0xFE, 0xF7, 0xBD, 0xC7, 0xE0},  //	track = inverted T
 {0xDE, 0xF7, 0xBD, 0xEF, 0x60},  //	pause
 {0x19, 0xDF, 0xF7, 0x9C, 0x60},  // 	play
 {0x07, 0xFF, 0xFF, 0xFC, 0x00},  //	stop
 {0xFC, 0xFB, 0xDE, 0xCF, 0xE0},  //	chapter = inverted C
 {0xFD, 0x6B, 0x1A, 0xEF, 0xE0},  //	angle = inverted A
 {0xFC, 0x7B, 0xBB, 0xC7, 0xE0},  //	zoom = inverted Z
 {0xFC, 0xDA, 0x5E, 0xCF, 0xE0},  //	group = inverted G
 {0xFE, 0x7B, 0x52, 0xC7, 0xE0},  //	repeat = inverted Repeat
 {0xFC, 0x7B, 0xDE, 0xF7, 0xE0},  //	last = inverted L
 {0xFE, 0x63, 0x18, 0xE7, 0xE0},  //	Dolby D left
 {0xFC, 0xE3, 0x18, 0xCF, 0xE0},  //	Dolby D right
 {0xFD, 0x73, 0x1A, 0xC7, 0xE0},  //	remain = inverted R
 {0xFC, 0x7B, 0xBB, 0xC7, 0xE0},  //	zoom = inverted Z
 {0xfc, 0x77, 0xbd, 0xc7, 0xe0},  //	intro = inverted I
 {0xfe, 0x6f, 0xbe, 0xcf, 0xe0},  //	shuffle = inverted S
//0x90
 {0x00, 0x14, 0xE5, 0x00, 0x00},  //	irregular data (*)
 {0xff, 0xff, 0xff, 0xff, 0xe0},  //	all dot on
//0x92~0x96
 {0xff, 0x7b, 0x1e, 0xc7, 0xe0},  //	inverted F
 {0xfd, 0x6b, 0x58, 0xd7, 0xe0},  //	inverted M
 {0xc7, 0x3d, 0xff, 0x73, 0x00},  //	inverted play (<)
 {0x01, 0x11, 0xf4, 0x10, 0x00},  //	(->)					//20091228
 {0x01, 0x05, 0xf1, 0x10, 0x00}}; //	(<-)
*/
const byte allDotsOn_charPattern[8][8] = {
	//addr=0,1,2,3,4,5,6,7(cursor position)
	{0x1b, 0x1b, 0x1b, 0x1b, 0x1b, 0x1b, 0x1b, 0x00},		//#0 : pause	
	{0x00, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x00, 0x00},		//#1 : stop		
	{0x1f, 0x11, 0x1b, 0x1b, 0x1b, 0x1b, 0x1f, 0x00},		//#2 : title		
	{0x1f, 0x19, 0x17, 0x14, 0x16, 0x19, 0x1f, 0x00},		//#3 : group		
	{0x1f, 0x11, 0x1b, 0x1b, 0x1b, 0x11, 0x1f, 0x00},		//#4 : intro		
	{0x1f, 0x13, 0x11, 0x11, 0x11, 0x13, 0x1f, 0x00},		//#5 : dolby L		
	{0x1f, 0x19, 0x11, 0x11, 0x11, 0x19, 0x1f, 0x00},		//#6 : dolby R		
	{0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f}		//#7 : all dots on
};
const byte char_pattern[8][8] = {
	//addr=0,1,2,3,4,5,6,7(cursor position)
	{0x1b, 0x1b, 0x1b, 0x1b, 0x1b, 0x1b, 0x1b, 0x00},		//#0 : pause	
	{0x00, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x00, 0x00},		//#1 : stop		
	{0x1f, 0x11, 0x1b, 0x1b, 0x1b, 0x1b, 0x1f, 0x00},		//#2 : title		
	{0x1f, 0x19, 0x17, 0x14, 0x16, 0x19, 0x1f, 0x00},		//#3 : group		
	{0x1f, 0x11, 0x1b, 0x1b, 0x1b, 0x11, 0x1f, 0x00},		//#4 : intro		
	{0x1f, 0x13, 0x11, 0x11, 0x11, 0x13, 0x1f, 0x00},		//#5 : dolby L		
	{0x1f, 0x19, 0x11, 0x11, 0x11, 0x19, 0x1f, 0x00},		//#6 : dolby R		
	{0x1f, 0x11, 0x14, 0x15, 0x17, 0x13, 0x1f, 0x00}		//#7 : repeat		
};
const byte TC2002A_CommandTable[11][3] = {
	//RS,R/w,Data
	{0, 0, 0x01},		//clear display		
	{0, 0, 0x02},		//return Home , address = 0x00		
	{0, 0, 0x04},		//entry mode set + DB1(Increment=1/Decrement=0) + DB0(Shift=1)		
	{0, 0, 0x08},		//Display/Cursor/Blinking on/off + DB2(DisplayOn=1) + DB1(CursorOn=1) + DB0(BlinkingOn=1)	
	{0, 0, 0x10},		//shift cussor and display + DB3(ShiftDisplay=1/shiftCursor=0) + DB2(Right=1/Left=0)	
	{0, 0, 0x20},		//function set + DB4(DataLength=1,8bits) + DB3(N=1,2Line) + DB2(Font=0,5x8dots)		
	{0, 0, 0x40},		//set CGRAM address + DB5~DB0
	{0, 0, 0x80},		//set DDRAM address + L1StartAddr(0x00)(+L2StartAddr(0x40))
	{0, 1, 0x00},		//read DB7(BusyFlag=1,busy) + AddressCounter(DB6~DB0)
	{1, 0, 0x00},		//write + Data into RAM (DDRAM, CGRAM) + data[DB7~DB0]
	{1, 1, 0x00}		//read Data from RAM (DDRAM, CGRAM) + data[DB7~DB0]
};

void TC2002A_Delay(void)
{
	unsigned char i;
	for (i=0;i<30;++i) {};		//2us
//	for (i=0;i<3;++i) {};		//2us
}
void TC2002A_PortInit(void)
{
	TC2002A_RS(0);	//Instruction_register=0 / Data_register=1
	TC2002A_RW(0);	//Read=1/write=0
	TC2002A_E(0);	//Enable=1
	SetBusInput;	//data bus port DB7~DB0
	//
}

void TC2002A_Tx(unsigned char key, unsigned char data1, unsigned char data2, unsigned char data3)
{
	unsigned char i;
	char temp;
	short stemp;

	TC2002A_Delay();
	TC2002A_E(0);									//Enable=0
	TC2002A_RS(TC2002A_CommandTable[key][0]);		//RS
	TC2002A_RW(TC2002A_CommandTable[key][1]);		//R/W
	TC2002A_Delay();
	TC2002A_E(1);									//Enable=1
	TC2002A_E(1);									//Enable=1
	//data bus output port DB7~DB0
	ResBusOutput;
	TC2002A_Delay();
	temp = TC2002A_CommandTable[key][2];
	temp |= data1;
	temp |= data2;
	temp |= data3;
	stemp = (short)temp;
	stemp = stemp <<8;
	WriteTC2002A_u16BUS(stemp);		
	TC2002A_Delay();
	TC2002A_Delay();
	TC2002A_E(0);									//Enable=0
}
/*
************************************************************/
//Clear Display
void TC2002A_ClearDisplay(void)
{
	TC2002A_Tx(MODE_CLEAR_DISPLAY, 0, 0, 0);
}
//Return Home
void TC2002A_ReturnHome(void)
{
	TC2002A_Tx(MODE_RETURN_HOME, 0, 0, 0);
}
//Entry Mode Set
void TC2002A_EntryMode_Increment(void)
{
	//Entry Mode Set : increment , shift off
	TC2002A_Tx(MODE_ENTRY_MODESET, MODE_ENTRY_MODESET_INC, 0, 0);
}
void TC2002A_EntryMode_Decrement(void)
{
	//Entry Mode Set : decrement , shift off
	TC2002A_Tx(MODE_ENTRY_MODESET, MODE_ENTRY_MODESET_DEC, 0, 0);
}
//Display Off
void TC2002A_Display_Off(void)
{
	TC2002A_Tx(MODE_DISPLAY_ONOFF, MODE_DISPLAY_ONOFF_OFF, 0, 0);
}
//Display On
void TC2002A_Display_On(void)
{
	TC2002A_Tx(MODE_DISPLAY_ONOFF, MODE_DISPLAY_ONOFF_ON, 0, 0);
}
//Function Set: 8bit, 1Line, 5x8dots
void TC2002A_FunctionSet_8B1L5x8Dot(void)
{
	TC2002A_Tx(MODE_FUNCTION_SET, MODE_FUNCTION_SET_8BIT, MODE_FUNCTION_SET_1LINE, MODE_FUNCTION_SET_5X8DOT);
}
//Function Set: 8bit, 2Line, 5x8dots
void TC2002A_FunctionSet_8B2L5x8Dot(void)
{
	TC2002A_Tx(MODE_FUNCTION_SET, MODE_FUNCTION_SET_8BIT, MODE_FUNCTION_SET_2LINE, MODE_FUNCTION_SET_5X8DOT);
}
/*
//Set DDRAM Address : 
//1 Line [0x00~0x4f] : 1x80 characters
//2 Line [0x00~0x27, 0x40~0x67] : 2x40 characters
//1st Line start address [0x00~0x13] : 1x20 characters
//2nd Line start address [0x14~0x27] : 1x20 characters
//3rd Line start address [0x40~0x53] : 1x20 characters
//4th Line start address [0x54~0x67] : 1x20 characters
***********************************************************/
void TC2002A_SetDDRAM_addr(unsigned char addr)
{
	TC2002A_Tx(MODE_SET_DDRAM_ADDR, addr, 0, 0);
}
/*
//Write Data to RAM
***********************************************************/
void TC2002A_WriteToRAM_data(unsigned char data)
{
	TC2002A_Tx(MODE_WRITE_DATATORAM, data, 0, 0);
}
/*
//Read_BusyFlag_AddressCounter
***********************************************************/
char TC2002A_Read_BF_AC(void)
{
	short data;
	//
	//change input-port data bus port DB7~DB0
	//
	TC2002A_Delay();

	TC2002A_E(0);		//Enable=0
	TC2002A_RS(0);		//RS
	TC2002A_RS(0);		//RS
	TC2002A_RW(1);		//R/W
	TC2002A_RW(1);		//R/W
	TC2002A_Delay();
	TC2002A_E(1);		//Enable=1
	TC2002A_E(1);		//Enable=1
	//
	//read data bus port DB7~DB0 <= PB15~8
	SetBusInput;
	TC2002A_Delay();
	data = (short)GPIO_ReadInputData(GPIOB);	//GetInputTC2002A_u16BUS();
	TC2002A_Delay();
	TC2002A_Delay();
	//
	TC2002A_E(0);		//Enable=0
	TC2002A_E(0);		//Enable=0
	//
	//change output-port data bus port DB7~DB0
	//data bus output port DB7~DB0
	ResBusOutput;
	//
	return( (char)(data>>8) );
}

/*
	character generator RAM : 512 bits
		5x8 dots : 8 characters [ 8 char x 5 x 8 = 512 bits ]
		5x10dots : 4 characters
	character generator ROM : 10880 bits
		5x8 dots : 192 characters
		5x10dots :  64 characters
			
	set CGRAM address + DB5~DB0 [ 01xx xxxx ]
	setCGRAM[01aa aaaa] => 0x40~0x7f [64] => total 8 character pattern
	setDDRAM[1aaa aaaa] => 0x80~0xff [128]
	
	
TC2002A_SetCGRAM_addr(addr);
TC2002A_WriteToRAM_data(data+);

pattern_data	address
	xxx11111	0	<= charater pattern #0
	xxx11111	1
	xxx11111	2
	xxx11111	3
	xxx11111	4
	xxx11111	5
	xxx11111	6
	xxx11111	7	<- cursor position

	xxx11111	8	<= charater pattern #1
	xxx11111	9
	xxx11111	10
	xxx11111	11
	xxx11111	12
	xxx11111	13
	xxx11111	14
	xxx11111	15	<- cursor position
**********************************************************/
void TC2002A_SetCGRAM_addr(unsigned char addr)
{
	TC2002A_Tx(MODE_SET_CGRAM_ADDR, addr, 0, 0);
}
/*
	special characters pattern data Tx
	0x00 ~ 0x07
	0x08 ~ 0x0f
**********************************************************/
void TC2002A_SpecialCharPatternDataTx(void)
{
	unsigned char i,j;
	char data;
	
	//#0x00~0x07 characters pattern 
	TC2002A_SetCGRAM_addr(0x00);
	for (j=0;j<8;j++) {
		for (i=0;i<8;i++) {
			data =0x80;
			while((data & 0x80 ) == 0x80) {
				//Busy Flag ==> 0x80 , busy 
				data = TC2002A_Read_BF_AC();
			}     
			if (f_initialize == ON) {
				TC2002A_WriteToRAM_data(allDotsOn_charPattern[j][i]);	//all dots on
			}else{
				TC2002A_WriteToRAM_data(char_pattern[j][i]);
			}
		}
	}		
/*
	//#0x08~0x0f characters pattern 
	TC2002A_SetCGRAM_addr(0x08);
	for (j=0;j<8;j++) {
		for (i=0;i<8;i++) {
			while((data & 0x80 ) == 0x80) {
				//Busy Flag ==> 0x80 , busy 
				data = TC2002A_Read_BF_AC();
			}     
			TC2002A_WriteToRAM_data(char_pattern[j][i]);
		}
	}*/		
}

/*
	TC2004A : 8bit, 2Line(=>4Line), 5x8dots
	TC2002A : 8bit, 1Line(=>2Line), 5x8dots
	
	- AC on
	- TC2004A_AcOnInit()
	- TC2004A_StartUpCheck(); (1msec)
		- WaitTime > 15msec(>4.5V) : 40msec(>2.7V)
		- TC2002A_FunctionSet_8B2L5x8Dot()

		- wait time > 4.1msec
		- TC2002A_FunctionSet_8B2L5x8Dot()

		- wait time > 100usec
		- TC2002A_FunctionSet_8B2L5x8Dot()

		- wait time > 100usec
		- TC2002A_Initialize()

		- wait time > 100usec
		- TC2002A_SpecialCharPatternDataTx()
	- start up end!!!!		

#define	M_STARTUP					0x00
#define	M_STARTUP1					0x01
#define	M_STARTUP2					0x02
#define	M_STARTUP3					0x03
#define	M_SPECIALCHAR_TX			0x04
#define	M_CONFIGURE_OK				0x05
volatile byte TC2002A_Startup_mode;
volatile byte TC2002A_Tx_time;
************************************************************/
void TC2002A_AcOnInit(void)
{
	TC2002A_PortInit();

	TC2002A_Startup_mode = M_STARTUP;  
	TC2002A_Tx_time = TIME10MSTO40MS;  		//40msec/10ms
}	
void TC2002A_Initialize(void)
{
	//Function Set : 8Bit,2Line,5x8dots
	TC2002A_FunctionSet_8B2L5x8Dot();
	//Display Off
	TC2002A_Display_Off();
	//Clear Display
	TC2002A_ClearDisplay();
	//Entry Mode Set : increment
	TC2002A_EntryMode_Increment();

	//1st Line start address [0x00~0x13]
	TC2002A_SetDDRAM_addr(0x00);
}
/*
1msec
*******************************************************************************/	
void TC2002A_StartUpCheck(void)
{

/*-----------------------------------------------------------------------------*/
	switch(TC2002A_Startup_mode){
		case M_CONFIGURE_OK:
				return;	
			break;
		case M_STARTUP:
			//wait time 40msec ?
			--TC2002A_Tx_time;
			if (TC2002A_Tx_time == 0x0000) {
				//next step
				//Function Set : 8Bit,2Line,5x8dots
				TC2002A_FunctionSet_8B2L5x8Dot();
				
				TC2002A_Startup_mode = M_STARTUP1;
				TC2002A_Tx_time = TIME10MSTO10MS;  		//wait time => 5(4.1)msec/1msec
			}
			break;
/*-----------------------------------------------------------------------------*/
		case M_STARTUP1:
			//wait time 5msec ?
			--TC2002A_Tx_time;
			if (TC2002A_Tx_time == 0x0000) {
				//next step
				//Function Set : 8Bit,2Line,5x8dots
				TC2002A_FunctionSet_8B2L5x8Dot();

				TC2002A_Startup_mode = M_STARTUP2;
				TC2002A_Tx_time = TIME10MSTO10MS;  		//wait time => 1(100u)msec/1msec
			}
			break;
/*-----------------------------------------------------------------------------*/
		case M_STARTUP2:
			//wait time 5msec ?
			--TC2002A_Tx_time;
			if (TC2002A_Tx_time == 0x0000) {
				//next step
				//Function Set : 8Bit,2Line,5x8dots
				TC2002A_FunctionSet_8B2L5x8Dot();

				TC2002A_Startup_mode = M_STARTUP3;
				TC2002A_Tx_time = TIME10MSTO10MS;  		//wait time => 1(100u)msec/1msec
			}
			break;
/*-----------------------------------------------------------------------------*/
		case M_STARTUP3:
			//wait time 5msec ?
			--TC2002A_Tx_time;
			if (TC2002A_Tx_time == 0x0000) {
				//next step
				//
				TC2002A_Initialize();

				TC2002A_Startup_mode = M_SPECIALCHAR_TX;
				TC2002A_Tx_time = TIME10MSTO10MS;  		//wait time => 1(100u)msec/1msec
			}
			break;
/*-----------------------------------------------------------------------------*/
		case M_SPECIALCHAR_TX:
			//wait time 5msec ?
			--TC2002A_Tx_time;
			if (TC2002A_Tx_time == 0x0000) {
				//next step
				//
				TC2002A_SpecialCharPatternDataTx();

				TC2002A_Startup_mode = M_CONFIGURE_OK;
			}
			break;
		default:
			break;
	}
}
/*

//1st Line start address [0x00~0x13] : 1x20 characters
//2nd Line start address [0x40~0x53] : 1x20 characters

**********************************************************/
void TC2002A_DisplayDataTx(void)
{
	unsigned char i,j;
	char data;
	
	//1st Line start address [0x00~0x13]
	TC2002A_SetDDRAM_addr(0x00);
	j=0;
	for (i=0;i<20;i++) {
		data =0x80;
		while((data & 0x80 ) == 0x80) {
			//Busy Flag ==> 0x80 , busy 
			data = TC2002A_Read_BF_AC();
		}     
		TC2002A_WriteToRAM_data(shiftDatIdx[i+20]);
	}		
	//2nd Line start address [0x40~0x53]
	TC2002A_SetDDRAM_addr(0x40);
	j=1;
	for (i=0;i<20;i++) {
		data =0x80;
		while((data & 0x80 ) == 0x80) {
			//Busy Flag ==> 0x80 , busy 
			data = TC2002A_Read_BF_AC();
		}     
		TC2002A_WriteToRAM_data(shiftDatIdx[i]);
	}		
}
/*
20110615 line address 

//1st Line start address [0x00~0x13] : 1x20 characters
//2nd Line start address [0x40~0x53] : 1x20 characters

//3rd Line start address [0x14~0x27] : 1x20 characters
//4th Line start address [0x54~0x67] : 1x20 characters
	
display_buffer[][]	
**********************************************************/
void TC2004A_DisplayDataTx(void)
{
	unsigned char i,j;
	char data;
	
	//1st Line start address [0x00~0x13]
	TC2002A_SetDDRAM_addr(0x00);
	j=0;
	for (i=0;i<20;i++) {
		data =0x80;
		while((data & 0x80 ) == 0x80) {
			//Busy Flag ==> 0x80 , busy 
			data = TC2002A_Read_BF_AC();
		}     
		TC2002A_WriteToRAM_data(shiftDatIdx[i+60]);
	}		
	//2nd Line start address [0x40~0x53]
	TC2002A_SetDDRAM_addr(0x40);
	j=2;
	for (i=0;i<20;i++) {
		data =0x80;
		while((data & 0x80 ) == 0x80) {
			//Busy Flag ==> 0x80 , busy 
			data = TC2002A_Read_BF_AC();
		}     
		TC2002A_WriteToRAM_data(shiftDatIdx[i+40]);
	}		
	//3rd Line start address [0x14~0x27]
	TC2002A_SetDDRAM_addr(0x14);
	j=1;
	for (i=0;i<20;i++) {
		data =0x80;
		while((data & 0x80 ) == 0x80) {
			//Busy Flag ==> 0x80 , busy 
			data = TC2002A_Read_BF_AC();
		}     
		TC2002A_WriteToRAM_data(shiftDatIdx[i+20]);
	}		
	//4th Line start address [0x54~0x67]
	TC2002A_SetDDRAM_addr(0x54);
	j=3;
	for (i=0;i<20;i++) {
		data =0x80;
		while((data & 0x80 ) == 0x80) {
			//Busy Flag ==> 0x80 , busy 
			data = TC2002A_Read_BF_AC();
		}     
		TC2002A_WriteToRAM_data(shiftDatIdx[i]);
	}		
}


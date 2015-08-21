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

#include "proc_display.ext"
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

void TC2002A_Delay100usec(void)
{
	unsigned char i,j;
	for (j=0;j<4;++j) {				//200us
		for (i=0;i<250;++i) {};		//50us
	}
}
void TC2002A_Delay(void)
{
	unsigned char i;
	for (i=0;i<80;++i) {};		//2us
//	for (i=0;i<3;++i) {};		//2us
}
void TC2002A_PortInit(void)
{
	TC2002A_RS(0);	//Instruction_register=0 / Data_register=1
	TC2002A_RW(0);	//Read=1/write=0
	TC2002A_E(0);	//Enable=1
	//SetBusInput;	//data bus port DB7~DB0
	ResBusOutput
}
/*
	power off 시에는 STN 전기 off 이므로 Busy check 불가 -> 무한 loop
********************************************************/
void Read_Busy(void)
{
	char data;
	
	data = 0xff;
	TC2002A_Delay();
	TC2002A_Delay();
	TC2002A_Delay();
	TC2002A_Delay();
	TC2002A_Delay();
	TC2002A_Delay();
/*
	do
	{
		data = TC2002A_Read_BF_AC();
	}while(data&0x80);
*/
}
/*******************************************************/
void TC2002A_Tx( char data, char data1)
{
	unsigned char i;
	char temp;
	short stemp;

	//data bus output port DB7~DB0
	ResBusOutput;
	temp = data;
	temp |= data1;
#ifdef UART1_DEBUG_x
	rs232_printf("Tx:%x \n",temp);
#endif
	stemp = (short)temp;
	stemp = stemp <<8;
	WriteTC2002A_u16BUS(stemp);		

	TC2002A_E(1);									//Enable=0
	TC2002A_Delay();
	TC2002A_Delay();
	TC2002A_E(0);									//Enable=1
}
/*
************************************************************/
//Clear Display
void TC2002A_ClearDisplay(void)
{
	TC2002A_Delay100usec();
	TC2002A_RS(0);		//RS
	TC2002A_RW(0);		//R/W
	TC2002A_Tx(0x01, 0);
}
//Return Home
void TC2002A_ReturnHome(void)
{
	TC2002A_Delay100usec();
	TC2002A_RS(0);		//RS
	TC2002A_RW(0);		//R/W
	TC2002A_Tx(0x02, 0);
}
//Entry Mode Set
void TC2002A_EntryMode_Increment(void)
{
	//Entry Mode Set : increment , shift off
	TC2002A_Delay100usec();
	TC2002A_RS(0);		//RS
	TC2002A_RW(0);		//R/W
	TC2002A_Tx(0x04, 0x02);					//20110811
}
void TC2002A_EntryMode_Decrement(void)
{
	//Entry Mode Set : decrement , shift off
	TC2002A_Delay100usec();
	TC2002A_RS(0);		//RS
	TC2002A_RW(0);		//R/W
	TC2002A_Tx(0x04, 0);
}
//Display Off
void TC2002A_Display_Off(void)
{
	TC2002A_Delay100usec();
	TC2002A_RS(0);		//RS
	TC2002A_RW(0);		//R/W
	TC2002A_Tx(0x08, 0);
}
//Display On, C=1 , B=1
void TC2002A_Display_On(void)
{
	TC2002A_Delay100usec();
	TC2002A_RS(0);		//RS
	TC2002A_RW(0);		//R/W
	TC2002A_Tx(0x08, 0x04);
}
//Shift display to the Right , Cursor follows the display shift
void TC2002A_DisplayShift_Cursor(void)
{
	TC2002A_Delay100usec();
	TC2002A_RS(0);		//RS
	TC2002A_RW(0);		//R/W
	TC2002A_Tx(0x10, 0x0c);
}
//Function Set: 8bit
void TC2002A_FunctionSet_8Bit(void)
{
	TC2002A_Delay100usec();
	TC2002A_RS(0);		//RS
	TC2002A_RW(0);		//R/W
	TC2002A_Tx(0x20, 0x10);
}
//Function Set: 8bit, 1Line, 5x8dots
void TC2002A_FunctionSet_8B1L5x8Dot(void)
{
	TC2002A_Delay100usec();
	TC2002A_RS(0);		//RS
	TC2002A_RW(0);		//R/W
	TC2002A_Tx(0x20, 0x10);
}
//Function Set: 8bit, 2Line, 5x8dots
void TC2002A_FunctionSet_8B2L5x8Dot(void)
{
	TC2002A_Delay100usec();
	TC2002A_RS(0);		//RS
	TC2002A_RW(0);		//R/W
	TC2002A_Tx(0x20, 0x18);		//0x18
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
	TC2002A_Delay100usec();
	TC2002A_RS(0);		//RS
	TC2002A_RW(0);		//R/W
	TC2002A_Tx(0x80, (addr&0x7f));
}
/*
//U+E080 ~ U+E096 ~ U+E09E
***********************************************************/
const byte RotelSpecialCharacter[] = 
{
	STN_invert_T,	//SC_invert_T,			STN_invert_T,	//0x80	
    STN_CD_pause,	//SC_CD_pause,			STN_CD_pause,	//0x81
    0xf6,			//SC_CD_play,			0xf6,			//0x82
    STN_CD_stop,	//SC_CD_stop,			STN_CD_stop,	//0x83
    0xcf,			//SC_invert_C,			0xcf,			//0x84	
    0x41,			//SC_invert_A,			0x41,			//0x85	
    0x5a,			//SC_invert_Z,			0x5a,			//0x86	
    STN_invert_G,	//SC_invert_G,			STN_invert_G,	//0x87	
    STN_invert_RP,	//SC_invert_RP,			STN_invert_RP,	//0x88
    0x4c,			//SC_invert_L,			0x4c,			//0x89	
    STN_invert_Dl,	//SC_invert_Dl,			STN_invert_Dl,	//0x8a
    STN_invert_Dr,	//SC_invert_Dr,			STN_invert_Dr,	//0x8b
    0xce,			//SC_invert_R,			0xce,			//0x8c	
    0x5a,			//SC_invert_Z1,			0x5a,			//0x8d
    STN_invert_I,	//SC_invert_I,			STN_invert_I,	//0x8e	
    0xfe,			//SC_invert_S,			0xfe,			//0x8f	
    0xcd,			//SC_irregular,			0xcd,			//0x90
    STN_allDotOn,	//SC_allDotOn,			STN_allDotOn,	//0x91	
    0xfa,			//SC_invert_F,			0xfa,			//0x92	
    0x4d,			//SC_invert_M,			0x4d,			//0x93	
    0xf7,			//SC_invert_play,		0xf7,			//0x94  
	0xc7,			//SC_arrow_right,	    0xc7,			//0x95
    0xc8			//SC_arrow_left        	0xc8,			//0x96
/*    
    //SC_menuRightArrow,	//0xf6,			//0x97		//add siport font
    //SC_menuLeftArrow,		//0xf7,			//0x98
    //SC_signalStrengthBar,	//STN_CD_stop,	//0x99
    //SC_minSignalIndicator,//0xfc,			//0x9a
    //SC_stationWithSecondary,//0xbc,		//0x9b	
    //SC_secondaryStation,	////0xbb,		//0x9c		
    //SC_favouriteStation,	////0xf5,		//0x9c	
    //SC_alarmBell			////0xd7		//x09e
*/
};		

/*
//Write Data to RAM
***********************************************************/
void TC2002A_WriteToRAM_data(unsigned char data)
{
	TC2002A_Delay100usec();
	TC2002A_RS(1);		//RS
	TC2002A_RW(0);		//R/W
#ifdef STINGRAY_UTF8_4321		//20111124
	//support Unicode => Rotel Special Character [ U+e080 ~ U+e09e ]
	//
	TC2002A_Tx(0x00, data);
#else
	if ((data >= 0x80)&&(data <= 0x96)) {							//? Rotel Special Character
		TC2002A_Tx(0x00, RotelSpecialCharacter[data - 0x80]);		//
	}else{
		TC2002A_Tx(0x00, data);
	}
#endif	
}
/*
//Read Data to RAM
***********************************************************/
char TC2002A_ReadToRAM_data(void)
{
	short data;

	TC2002A_Delay100usec();
	TC2002A_RS(1);		//RS
	TC2002A_RW(1);		//R/W

	//read data bus port DB7~DB0 <= PB15~8
	SetBusInput;
	TC2002A_Delay();

	TC2002A_E(1);		//Enable=0
	TC2002A_Delay();
	TC2002A_Delay();
	TC2002A_Delay();
	//
	data = (short)GPIO_ReadInputData(GPIOB);	//GetInputTC2002A_u16BUS();
	//
	//change output-port data bus port DB7~DB0
	//data bus output port DB7~DB0
	//ResBusOutput;
	//
	TC2002A_E(0);		//Enable=1
	return( (char)(data>>8) );
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
	TC2002A_Delay100usec();
	TC2002A_RS(0);		//RS
	TC2002A_RW(1);		//R/W

	//read data bus port DB7~DB0 <= PB15~8
	SetBusInput;
	TC2002A_Delay();

	TC2002A_E(1);		//Enable=0
	TC2002A_Delay();
	TC2002A_Delay();
	TC2002A_Delay();
	//
	data = (short)GPIO_ReadInputData(GPIOB);	//GetInputTC2002A_u16BUS();
	//
	//change output-port data bus port DB7~DB0
	//data bus output port DB7~DB0
	//ResBusOutput;
	//
	TC2002A_E(0);		//Enable=1
#ifdef UART1_DEBUG_x
	rs232_printf("BF:%x \n",(data>>8));
#endif
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
	TC2002A_Delay100usec();
	TC2002A_RS(0);		//RS
	TC2002A_RW(0);		//R/W
	TC2002A_Tx(0x40, (addr&0x3f));
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
			//Busy Flag ==> 0x80 , busy 
			Read_Busy();
			if (f_initialize == ON) {
				TC2002A_WriteToRAM_data(allDotsOn_charPattern[j][i]);	//all dots on
			}else{
				TC2002A_WriteToRAM_data(char_pattern[j][i]);
			}
		}
	}		
/*
///char. font read test /////////
//	
	//#0x00~0x07 characters pattern 
	TC2002A_SetCGRAM_addr(0x08);			//"STOP"
	j=0;
	for (i=0;i<8;i++) {
		//Busy Flag ==> 0x80 , busy 
		Read_Busy();
		data = TC2002A_ReadToRAM_data();
		test_buf[i] = data;
	}		
#ifdef UART1_DEBUG_x
	rs232_printf("Font: \n",blank);
	for (i=0;i<8;i++) {
		rs232_printf("%x ",test_buf[i]);
	}
	rs232_printf("\n",blank);
#endif
*/
/*
	//#0x08~0x0f characters pattern 
	TC2002A_SetCGRAM_addr(0x08);
	for (j=0;j<8;j++) {
		for (i=0;i<8;i++) {
			//Busy Flag ==> 0x80 , busy 
			Read_Busy();
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
	ResBusOutput;
	TC2002A_Startup_mode = M_STARTUP;  
	TC2002A_Tx_time = TIME10MSTO100MS;  		//100msec/10ms
}	
void TC2002A_Initialize(void)
{
	//Function Set : 8Bit,2Line,5x8dots
	TC2002A_FunctionSet_8B2L5x8Dot();
	//Display Off
	TC2002A_Display_Off();
	DelayMs(1);
	//Clear Display
	TC2002A_ClearDisplay();
	DelayMs(5);
	//Entry Mode Set : increment
	TC2002A_EntryMode_Increment();
	DelayMs(1);
	TC2002A_Display_On();
	//TC2002A_ReturnHome();
	//1st Line start address [0x00~0x13]
	//TC2002A_SetDDRAM_addr(0x00);
}
/*
10msec
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
				//Function Set : 8Bit
				TC2002A_FunctionSet_8B2L5x8Dot();
				
				TC2002A_Startup_mode = M_STARTUP1;
				TC2002A_Tx_time = TIME10MSTO20MS;  		//wait time => 5(4.1)msec/1msec
			}
			break;
/*-----------------------------------------------------------------------------*/
		case M_STARTUP1:
			//wait time 5msec ?
			--TC2002A_Tx_time;
			if (TC2002A_Tx_time == 0x0000) {
				//next step
				//Function Set : 8Bit
				TC2002A_FunctionSet_8B2L5x8Dot();

				TC2002A_Startup_mode = M_STARTUP2;
				TC2002A_Tx_time = TIME10MSTO20MS;  		//wait time => 1(100u)msec/1msec
			}
			break;
/*-----------------------------------------------------------------------------*/
		case M_STARTUP2:
			//wait time 5msec ?
			--TC2002A_Tx_time;
			if (TC2002A_Tx_time == 0x0000) {
				//next step
				//Function Set : 8Bit
				TC2002A_FunctionSet_8B2L5x8Dot();

				TC2002A_Startup_mode = M_STARTUP3;
				TC2002A_Tx_time = TIME10MSTO20MS;  		//wait time => 1(100u)msec/1msec
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
				TC2002A_Tx_time = TIME10MSTO20MS;  		//wait time => 1(100u)msec/1msec
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

//20110906
#ifdef TIM3_PWM_ENABLE
				//PWM start
				TIM3_PWM_Configuration();	//20110721
//	if(Dimmer_TempValue >= 10) {
//		//port Low fix
//	}else{
				TIM3_PWM_CH4_Set(Dimmer_TempValue);
//	}
#endif
				//DAC reseting	: 20110727
#ifdef TIM6_DAC_ENABLE
				TIM_DeInit(TIM6);
				TIM6_Configuration();
#else
#ifdef DAC_ENABLE
				DAC_DeInit();
				DAC_Configuration( (DAc_value.iValue&0x0fff)<<4 );
#endif
#endif

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
	unsigned char i;
	
  if (test_addr == 0) {
	//1 Line start address [0x00~0x4f]
	//2 Line start address [0x00~0x27] [0x40~0x67]
	//1st Line start address [0x00~0x13]
	TC2002A_SetDDRAM_addr(0x00);
	for (i=0;i<20;i++) {
		//Busy Flag ==> 0x80 , busy 
		Read_Busy();
		TC2002A_WriteToRAM_data(shiftDatIdx[i]);
	}		
	//2nd Line start address [0x40~0x53]
	TC2002A_SetDDRAM_addr(0x40);
	for (i=0;i<20;i++) {
		//Busy Flag ==> 0x80 , busy 
		Read_Busy();
		TC2002A_WriteToRAM_data(shiftDatIdx[i+20]);
	}
  }else{
	//1st Line start address [0x00~0x13]
	TC2002A_SetDDRAM_addr(0x00);
	for (i=0;i<20;i++) {
		//Busy Flag ==> 0x80 , busy 
		Read_Busy();
		//TC2002A_WriteToRAM_data('A');
		TC2002A_WriteToRAM_data(test_buf[i]);
	}		
	//2nd Line start address [0x40~0x53]
	TC2002A_SetDDRAM_addr(0x40);
	for (i=0;i<20;i++) {
		//Busy Flag ==> 0x80 , busy 
		Read_Busy();
		//TC2002A_WriteToRAM_data('A');
		TC2002A_WriteToRAM_data(test_buf[i+20]);
	}	  
  }
	/*
	//1st Line start address [0x00~0x13]
	TC2002A_SetDDRAM_addr(0x00);
	j=0;
	for (i=0;i<20;i++) {
		//Busy Flag ==> 0x80 , busy 
		Read_Busy();
		TC2002A_WriteToRAM_data(shiftDatIdx[i+20]);
		//TC2002A_WriteToRAM_data(shiftDatIdx[i+20]);
	}		
	//2nd Line start address [0x40~0x53]
	TC2002A_SetDDRAM_addr(0x40);
	j=1;
	for (i=0;i<20;i++) {
		//Busy Flag ==> 0x80 , busy 
		Read_Busy();
		TC2002A_WriteToRAM_data(shiftDatIdx[i]);
		//TC2002A_WriteToRAM_data(shiftDatIdx[i]);
	}
*/
}
void TC2002A_DisplayDataTx1L(void)
{
	unsigned char i;
	
	//1 Line start address [0x00~0x4f]
	//2 Line start address [0x00~0x27] [0x40~0x67]
	//1st Line start address [0x00~0x13]
	TC2002A_SetDDRAM_addr(0x00);
	for (i=0;i<20;i++) {
		//Busy Flag ==> 0x80 , busy 
		Read_Busy();
		TC2002A_WriteToRAM_data(shiftDatIdx[i]);
	}		
}
void TC2002A_DisplayDataTx2L(void)
{
	unsigned char i;
	
	//1 Line start address [0x00~0x4f]
	//2 Line start address [0x00~0x27] [0x40~0x67]
	//1st Line start address [0x00~0x13]
	//2nd Line start address [0x40~0x53]
	TC2002A_SetDDRAM_addr(0x40);
	for (i=0;i<20;i++) {
		//Busy Flag ==> 0x80 , busy 
		Read_Busy();
		TC2002A_WriteToRAM_data(shiftDatIdx[i+20]);
	}
}
void TC2002A_DisplayDataTx3L(void)
{
	unsigned char i;
	
	//1 Line start address [0x00~0x4f]
	//2 Line start address [0x00~0x27] [0x40~0x67]
	//1st Line start address [0x00~0x13]
	//2nd Line start address [0x40~0x53]
	TC2002A_SetDDRAM_addr(0x14);
	for (i=0;i<20;i++) {
		//Busy Flag ==> 0x80 , busy 
		Read_Busy();
		TC2002A_WriteToRAM_data(shiftDatIdx[i+40]);
	}
}
void TC2002A_DisplayDataTx4L(void)
{
	unsigned char i;
	
	//1 Line start address [0x00~0x4f]
	//2 Line start address [0x00~0x27] [0x40~0x67]
	//1st Line start address [0x00~0x13]
	//2nd Line start address [0x40~0x53]
	TC2002A_SetDDRAM_addr(0x0054);
	for (i=0;i<20;i++) {
		//Busy Flag ==> 0x80 , busy 
		Read_Busy();
		TC2002A_WriteToRAM_data(shiftDatIdx[i+60]);
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
void TC2004A_DisplayDataTx13L(void)
{
	unsigned char i;
	
	//1st Line start address [0x00~0x13]
	//2nd Line start address [0x40~0x53]
	//3rd Line start address [0x14~0x27]
	//4th Line start address [0x54~0x67]
	TC2002A_SetDDRAM_addr(0x00);
	for (i=0;i<20;i++) {
		//Busy Flag ==> 0x80 , busy 
		Read_Busy();
		TC2002A_WriteToRAM_data(shiftDatIdx[i+00]);
	}
	for (i=0;i<20;i++) {
		//Busy Flag ==> 0x80 , busy 
		Read_Busy();
		TC2002A_WriteToRAM_data(shiftDatIdx[i+40]);
	}
}
void TC2004A_DisplayDataTx12L(void)
{
	unsigned char i;
	
	//1st Line start address [0x00~0x13]
	//2nd Line start address [0x40~0x53]
	//3rd Line start address [0x14~0x27]
	//4th Line start address [0x54~0x67]
	TC2002A_SetDDRAM_addr(0x00);
	for (i=0;i<20;i++) {
		//Busy Flag ==> 0x80 , busy 
		Read_Busy();
		TC2002A_WriteToRAM_data(shiftDatIdx[i+00]);
	}
	TC2002A_SetDDRAM_addr(0x40);
	for (i=0;i<20;i++) {
		//Busy Flag ==> 0x80 , busy 
		Read_Busy();
		TC2002A_WriteToRAM_data(shiftDatIdx[i+20]);
	}
}
void TC2004A_DisplayDataTx24L(void)
{
	unsigned char i;
	
	//1st Line start address [0x00~0x13]
	//2nd Line start address [0x40~0x53]
	//3rd Line start address [0x14~0x27]
	//4th Line start address [0x54~0x67]
	TC2002A_SetDDRAM_addr(0x40);
	for (i=0;i<20;i++) {
		//Busy Flag ==> 0x80 , busy 
		Read_Busy();
		TC2002A_WriteToRAM_data(shiftDatIdx[i+20]);
	}
	for (i=0;i<20;i++) {
		//Busy Flag ==> 0x80 , busy 
		Read_Busy();
		TC2002A_WriteToRAM_data(shiftDatIdx[i+60]);
	}
}
void TC2004A_DisplayDataTx34L(void)
{
	unsigned char i;
	
	//1st Line start address [0x00~0x13]
	//2nd Line start address [0x40~0x53]
	//3rd Line start address [0x14~0x27]
	//4th Line start address [0x54~0x67]
	TC2002A_SetDDRAM_addr(0x14);
	for (i=0;i<20;i++) {
		//Busy Flag ==> 0x80 , busy 
		Read_Busy();
		TC2002A_WriteToRAM_data(shiftDatIdx[i+40]);
	}
	TC2002A_SetDDRAM_addr(0x54);
	for (i=0;i<20;i++) {
		//Busy Flag ==> 0x80 , busy 
		Read_Busy();
		TC2002A_WriteToRAM_data(shiftDatIdx[i+60]);
	}
}
//
//	call : Func_PowerOff();
//
void TC2004A_DisplayDataTx(void)
{
	unsigned char i;
	
	//1st Line start address [0x00~0x13]
	TC2002A_SetDDRAM_addr(0x00);
	for (i=0;i<20;i++) {
		//Busy Flag ==> 0x80 , busy 
		Read_Busy();
		TC2002A_WriteToRAM_data(shiftDatIdx[i]);
	}		
	//3rd Line start address [0x14~0x27]
	TC2002A_SetDDRAM_addr(0x14);
	for (i=0;i<20;i++) {
		//Busy Flag ==> 0x80 , busy 
		Read_Busy();
		TC2002A_WriteToRAM_data(shiftDatIdx[i+40]);
	}		
	//2nd Line start address [0x40~0x53]
	TC2002A_SetDDRAM_addr(0x40);
	for (i=0;i<20;i++) {
		//Busy Flag ==> 0x80 , busy 
		Read_Busy();
		TC2002A_WriteToRAM_data(shiftDatIdx[i+20]);
	}		
	//4th Line start address [0x54~0x67]
	TC2002A_SetDDRAM_addr(0x54);
	for (i=0;i<20;i++) {
		//Busy Flag ==> 0x80 , busy 
		Read_Busy();
		TC2002A_WriteToRAM_data(shiftDatIdx[i+60]);
	}		
}
/**********************************************************
20110810
	FTX_DISP_BUFF.display_Buffer[80] == shiftDatIdx[80]
	
	
	1st Line start address [0x00~0x13] : 1x20 characters
	2nd Line start address [0x40~0x53] : 1x20 characters
**********************************************************/
void TC2002A_DisplayDataCompareTx(void)
{
	unsigned char i;
	
	//1st Line start address [0x00~0x13]
	//2nd Line start address [0x40~0x53]
	for (i=0;i<20;i++) {
		if(FTX_DISP_BUFF.display_Buffer[i] != shiftDatIdx[i]) {
			shiftDatIdx[i] = FTX_DISP_BUFF.display_Buffer[i];
			TC2002A_SetDDRAM_addr(i);
			//Busy Flag ==> 0x80 , busy 
			Read_Busy();
			TC2002A_WriteToRAM_data(shiftDatIdx[i]);
		}
	}		
	//2nd Line start address [0x40~0x53]
	for (i=0;i<20;i++) {
		if(FTX_DISP_BUFF.display_Buffer[i+20] != shiftDatIdx[i+20]) {
			shiftDatIdx[i+20] = FTX_DISP_BUFF.display_Buffer[i+20];
			TC2002A_SetDDRAM_addr(i+0x40);
			//Busy Flag ==> 0x80 , busy 
			Read_Busy();
			TC2002A_WriteToRAM_data(shiftDatIdx[i+20]);
		}
	}
}		
/*	RT12
	(125msec) call : main

	FTX_DISP_BUFF.display_Buffer[80] == shiftDatIdx[80]
	FTX_DISP_BUFF_UTF8.sUtf8Stingray.iDispBuf[4][20] == shiftDatIdx[80]

	MAX_UNICODE2TC2004A003 = 118
*********************************************************/
void TC2004A_DisplayDataCompareTx(void)
{
	unsigned char i,j,k,data_f;
	unsigned short data16;
	
#ifdef STINGRAY_UTF8_4321
	//1st Line start address [0x00~0x13]
	//2nd Line start address [0x40~0x53]
	//3rd Line start address [0x14~0x27]
	//4th Line start address [0x54~0x67]
	for(i=0;i<20;i++) {
		data16 = FTX_DISP_BUFF_UTF8.sUtf8Stingray.iDispBuf[0][i];
		if ( (data16 >=0x0020)&&(data16<=0x007e) ) {
			data_f = (unsigned char)data16;
		}else{
			data_f = 0xcd;	//TC2004A : irregular character font
			for(k=0;k<MAX_UNICODE2TC2004A003;k++) {
				if (data16 == Unicode2TC2002A003[k][0]) {
					data_f = (unsigned char)Unicode2TC2002A003[k][1];
					break;
				}
			}
		}
		//compare
		if (data_f != shiftDatIdx[i]) {
			shiftDatIdx[i] = data_f;
			TC2002A_SetDDRAM_addr(i);
			//Busy Flag ==> 0x80 , busy 
			Read_Busy();
			TC2002A_WriteToRAM_data(shiftDatIdx[i]);
		}
	}		
	//2nd Line start address [0x40~0x53]
	//3rd Line start address [0x14~0x27]
	//4th Line start address [0x54~0x67]
	for(i=0;i<20;i++) {
		data16 = FTX_DISP_BUFF_UTF8.sUtf8Stingray.iDispBuf[1][i];
		if ( (data16 >=0x0020)&&(data16<=0x007e) ) {
			data_f = (unsigned char)data16;
		}else{
			data_f = 0xcd;	//TC2004A : irregular character font
			for(k=0;k<MAX_UNICODE2TC2004A003;k++) {
				if (data16 == Unicode2TC2002A003[k][0]) {
					data_f = (unsigned char)Unicode2TC2002A003[k][1];
					break;
				}
			}
		}
		//compare
		if (data_f != shiftDatIdx[i+20]) {
			shiftDatIdx[i+20] = data_f;
			TC2002A_SetDDRAM_addr(i+0x40);
			//Busy Flag ==> 0x80 , busy 
			Read_Busy();
			TC2002A_WriteToRAM_data(shiftDatIdx[i+20]);
		}
	}		
	//3rd Line start address [0x14~0x27]
	//4th Line start address [0x54~0x67]
	for(i=0;i<20;i++) {
		data16 = FTX_DISP_BUFF_UTF8.sUtf8Stingray.iDispBuf[2][i];
		if ( (data16 >=0x0020)&&(data16<=0x007e) ) {
			data_f = (unsigned char)data16;
		}else{
			data_f = 0xcd;	//TC2004A : irregular character font
			for(k=0;k<MAX_UNICODE2TC2004A003;k++) {
				if (data16 == Unicode2TC2002A003[k][0]) {
					data_f = (unsigned char)Unicode2TC2002A003[k][1];
					break;
				}
			}
		}
		//compare
		if (data_f != shiftDatIdx[i+40]) {
			shiftDatIdx[i+40] = data_f;
			TC2002A_SetDDRAM_addr(i+0x14);
			//Busy Flag ==> 0x80 , busy 
			Read_Busy();
			TC2002A_WriteToRAM_data(shiftDatIdx[i+40]);
		}
	}		
	//4th Line start address [0x54~0x67]
	for(i=0;i<20;i++) {
		data16 = FTX_DISP_BUFF_UTF8.sUtf8Stingray.iDispBuf[3][i];
		if ( (data16 >=0x0020)&&(data16<=0x007e) ) {
			data_f = (unsigned char)data16;
		}else{
			data_f = 0xcd;	//TC2004A : irregular character font
			for(k=0;k<MAX_UNICODE2TC2004A003;k++) {
				if (data16 == Unicode2TC2002A003[k][0]) {
					data_f = (unsigned char)Unicode2TC2002A003[k][1];
					break;
				}
			}
		}
		//compare
		if (data_f != shiftDatIdx[i+60]) {
			shiftDatIdx[i+60] = data_f;
			TC2002A_SetDDRAM_addr(i+0x54);
			//Busy Flag ==> 0x80 , busy 
			Read_Busy();
			TC2002A_WriteToRAM_data(shiftDatIdx[i+60]);
		}
	}		

#else	//STINGRAY_UTF8_4321

	//1st Line start address [0x00~0x13]
	//2nd Line start address [0x40~0x53]
	//3rd Line start address [0x14~0x27]
	//4th Line start address [0x54~0x67]
	for (i=0;i<20;i++) {
		if(FTX_DISP_BUFF.display_Buffer[i] != shiftDatIdx[i]) {
			shiftDatIdx[i] = FTX_DISP_BUFF.display_Buffer[i];
			TC2002A_SetDDRAM_addr(i);
			//Busy Flag ==> 0x80 , busy 
			Read_Busy();
			TC2002A_WriteToRAM_data(shiftDatIdx[i]);
		}
	}		
	//2nd Line start address [0x40~0x53]
	//3rd Line start address [0x14~0x27]
	//4th Line start address [0x54~0x67]
	for (i=0;i<20;i++) {
		if(FTX_DISP_BUFF.display_Buffer[i+20] != shiftDatIdx[i+20]) {
			shiftDatIdx[i+20] = FTX_DISP_BUFF.display_Buffer[i+20];
			TC2002A_SetDDRAM_addr(i+0x40);
			//Busy Flag ==> 0x80 , busy 
			Read_Busy();
			TC2002A_WriteToRAM_data(shiftDatIdx[i+20]);
		}
	}		
	//3rd Line start address [0x14~0x27]
	//4th Line start address [0x54~0x67]
	for (i=0;i<20;i++) {
		if(FTX_DISP_BUFF.display_Buffer[i+40] != shiftDatIdx[i+40]) {
			shiftDatIdx[i+40] = FTX_DISP_BUFF.display_Buffer[i+40];
			TC2002A_SetDDRAM_addr(i+0x14);
		//Busy Flag ==> 0x80 , busy 
		Read_Busy();
		TC2002A_WriteToRAM_data(shiftDatIdx[i+40]);
	}		
	}		
	//4th Line start address [0x54~0x67]
	for (i=0;i<20;i++) {
		if(FTX_DISP_BUFF.display_Buffer[i+60] != shiftDatIdx[i+60]) {
			shiftDatIdx[i+60] = FTX_DISP_BUFF.display_Buffer[i+60];
			TC2002A_SetDDRAM_addr(i+0x54);
		//Busy Flag ==> 0x80 , busy 
		Read_Busy();
		TC2002A_WriteToRAM_data(shiftDatIdx[i+60]);
	}		
}
#endif	//STINGRAY_UTF8_4321
}

/**********************************************************/
//
///char. font read test /////////
//	
//	step : 0..7 
/**********************************************************/
void TC2002A_CharacterFontRead(unsigned char step)
{
	unsigned char i,j;
	char data;

	//#0x00~0x07 characters pattern 
	TC2002A_SetCGRAM_addr(step*8);			//"STOP"
	j=0;
	for (i=0;i<8;i++) {
		//Busy Flag ==> 0x80 , busy 
		Read_Busy();
		data = TC2002A_ReadToRAM_data();
		test_buf[i] = data;
	}		
#ifdef UART1_DEBUG_x
	rs232_printf("Font step:%x \n",step);
	for (i=0;i<8;i++) {
		rs232_printf("%x ",test_buf[i]);
	}
	rs232_printf("\n",blank);
#endif
}
/**********************************************************/
//
///DDRam data read test /////////
//	step > step+20
//	
/**********************************************************/
void TC2002A_DDRamDataRead(void)
{
	unsigned char i,j;
	char data;

///read test /////////
//	
	//1st Line start address [0x00~0x13]
	TC2002A_SetDDRAM_addr(0x00);
	for (i=0;i<20;i++) {
		//Busy Flag ==> 0x80 , busy 
		Read_Busy();
		data = TC2002A_ReadToRAM_data();
		test_buf[i] = data;
	}		
	//2nd Line start address [0x40~0x53]
	TC2002A_SetDDRAM_addr(0x40);
	for (i=20;i<40;i++) {
		//Busy Flag ==> 0x80 , busy 
		Read_Busy();
		data = TC2002A_ReadToRAM_data();
		test_buf[i] = data;
	}
#ifdef UART1_DEBUG_x
	rs232_printf("Display: \n",blank);
	for (i=0;i<20;i++) {
		rs232_printf("%x ",test_buf[i]);
	}
	rs232_printf("\n",blank);
	for (i=20;i<40;i++) {
		rs232_printf("%x ",test_buf[i]);
	}
	rs232_printf("\n",blank);
//	rs232_printf("k=>%d %d %d %d \n",ADC_Val[0],ADC_Val[1],ADC_Val[2],ADC_Val[3]);
#endif
}

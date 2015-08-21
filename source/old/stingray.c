/**************** (c)2008 Rotel Technology Global Inc..*************************
PROJECT         : RDG1520
COMPILER        : IAR Embeded Workbench 4.4.1
MODULE          : adckey.c
VERSION         : V 1.0
USED CHIP       : stm32f101x
CREATION DATE   : SEP 01,2008
AUTHORS         : / RTG
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

#include "stingray.mem"
#include "stingray.h"
#include "adckey.h"

#include "function.ext"
#include "proc_function.h"
#include "proc_display.h"
#include "proc_display.ext"

extern unsigned char Asc2Bin(unsigned char ch1,unsigned char ch2);
extern void vfd_processing(void);

/*----------------------------------------------------------------------------*/
//#define MAX_STRING_TEXT_LENGTH		20			//20100129

#ifdef STINGRAY_C
//#define LAST_PRESET_NUMBER		60		//test iRadio+FM
#define LAST_PRESET_NUMBER		90			//iRadio+FM+DAB
#else
#define LAST_PRESET_NUMBER		30			//iRadio
#endif

#define TIME_STINGRAY_PRESET	TIME10MSTO3SEC		//stingray preset time : 20091215

/*----------------------------------------------------------------------------*/
//#define _MN_ "ADC_KEY: "
//initialize Configure 
const byte tbl_Configure[10] = { 
					0x00, 0x04, 0x04, 0x14,		//SetdisplaySize, Length, Rows, Columns	( 4 x 20 )
					0x01, 0x03, 0x06,			//SetAudioconfiguration , Length, SampleRate ( 48K )
					0x03, 0x03, 0x01 };			//SetUIType, Length, UIType ( vertical menu > 2 lines )
//stingray key value
const byte tbl_StingrayKey[] = {
	0x00,		//SK_NULL
	0x19,		//SK_POWER
	0x15,		//SK_QUEUE
	0x12,		//SK_PLAYPAUSE
	0x14,		//SK_MODE
	0x10,		//SK_SKIP_N
	0x13,		//SK_STOP
	0x11,		//SK_SKIP_P
	0x00,		///SK_MEMORY		, reserved
	0x03,		//SK_1
	0x04,		//SK_2
	0x05,		//SK_3
	0x06,		//SK_4
	0x07,		//SK_5
	0x00,		//SK_NULL
	0x00,		//SK_NULL
	0x00,		//SK_NULL
	0x08,		//SK_6
	0x09,		//SK_7
	0x0a,		//SK_8
	0x0b,		//SK_9
	0x0c,		//SK_0
	0x00,		//SK_NULL
	0x00,		//SK_NULL
	0x00,		//SK_NULL
	0x01,		//SK_ENC_N
	0x0f,		//SK_SELECT
	0x02,		//SK_ENC_P
	0x0e,		//SK_BACK
	0x0d,		//SK_REPLY
	0x00,		//SK_NULL
	0x00,		//SK_NULL
	0x00		//SK_NULL
};
//change ASCii code
const byte tbl_ASCii[] = {
	0x30,		//0
	0x31,		//1
	0x32,		//2
	0x33,		//3
	0x34,		//4
	0x35,		//5
	0x36,		//6
	0x37,		//7
	0x38,		//8
	0x39,		//9
	0x41,		//A
	0x42,		//B
	0x43,		//C
	0x44,		//D
	0x45,		//E
	0x46		//F
};
//change ascii code
const byte tbl_ascii[] = {
	0x30,		//0
	0x31,		//1
	0x32,		//2
	0x33,		//3
	0x34,		//4
	0x35,		//5
	0x36,		//6
	0x37,		//7
	0x38,		//8
	0x39,		//9
	0x61,		//a
	0x62,		//b
	0x63,		//c
	0x64,		//d
	0x65,		//e
	0x66		//f
};
//20100201
const char pcStandardCharacters_AaBbCc[] =	"AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz"
											"0123456789"
											"<|=>+-_!?@#$%^&*:;`'\".,\\/[]{}()~ ";
const char pcStandardCharacters[] =			"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
											"abcdefghijklmnopqrstuvwxyz"
											"0123456789"
											"<|=>+-_!?@#$%^&*:;`'\".,\\/[]{}()~ ";
const char pcIPAddressCharacters[] =		"0123456789.";
const char pcCallsignCharacters[] =			"ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const char pcStationNameCharacters[] =		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
											"0123456789"
											"<|=>+-_!?@#$%^&*:;`'\".,\\/[]{}()~ ";
//20100427												/*234567890123456789012345678901234567890*/
const unsigned char Writing_firmware_donot_turnoff[] =	" Writing firmware,   do not turn off !  ";
//const unsigned char UpgradingFirmware[] =	" Upgrading Firmware ";
const unsigned char rotel_UpgradingFirmware[4][20] = {	/*2345678901234567890*/
														"  Writing Firmware  ",
														"  ,do not turn off! ",
														"                    ",
														"                    "};

/* pcStandardCharacters[]=
\\   00000000   414243444546       DC8 41H, 42H, 43H, 44H, 45H, 46H, 47H, 48H
\\              4748        
\\   00000008   494A4B4C4D4E       DC8 49H, 4AH, 4BH, 4CH, 4DH, 4EH, 4FH, 50H
\\              4F50        
\\   00000010   515253545556       DC8 51H, 52H, 53H, 54H, 55H, 56H, 57H, 58H
\\              5758        
\\   00000018   595A61626364       DC8 59H, 5AH, 61H, 62H, 63H, 64H, 65H, 66H
\\              6566        
\\   00000020   6768696A6B6C       DC8 67H, 68H, 69H, 6AH, 6BH, 6CH, 6DH, 6EH
\\              6D6E        
\\   00000028   6F7071727374       DC8 6FH, 70H, 71H, 72H, 73H, 74H, 75H, 76H
\\              7576        
\\   00000030   7778797A3031       DC8 77H, 78H, 79H, 7AH, 30H, 31H, 32H, 33H
\\              3233        
\\   00000038   343536373839       DC8 34H, 35H, 36H, 37H, 38H, 39H, 3CH, 7CH
\\              3C7C        
\\   00000040   3D3E2B2D5F21       DC8 3DH, 3EH, 2BH, 2DH, 5FH, 21H, 3FH, 40H
\\              3F40        
\\   00000048   2324255E262A       DC8 23H, 24H, 25H, 5EH, 26H, 2AH, 3AH, 3BH
\\              3A3B        
\\   00000050   6027222E2C5C       DC8 60H, 27H, 22H, 2EH, 2CH, 5CH, 2FH, 5BH
\\              2F5B        
\\   00000058   5D7B7D28297E       DC8 5DH, 7BH, 7DH, 28H, 29H, 7EH, 20H, 0
\\              2000        
*/   

// stingray command TX
/*-----------------------------------------------------------------------------
	Master --->Slave
	Master <---Slave (ACK)
	
ACK			: CommandID(0x80+?) + SequenceNumber + Status(0:Fail, 1:Pass)

GetStatus	: CheckSum + Length(2) + Command ID(0x01) + SequenceNumber
Status		: CommandID(0x81) + SequenceNumber + status(0:not been configured, 1:has been configured)	

//GetScreen	: CheckSum + Length(2) + Command ID(0x02) + SequenceNumber		//not used serial interface
Screen		: CommandID(0x82) + SequenceNumber + tbl_Screen(n) 
	
const byte tbl_Screen[] = { 
	0x00, 0x04, 0x??, 0x??,			//Icons(0), Length, IconStatus, SignalStrength(0~100)
	0x01, 0x??, 0x00,	0x??,..		//LineContents(1), Length, LineNumber(0), Invert, ScrollEnabled(x), ArrowStatus, Text(UTF-8 format)
	0x01, 0x??, 0x01,	0x??,..		//LineContents(1), Length, LineNumber(1), Invert, ScrollEnabled(x), ArrowStatus, Text(UTF-8 format)
	0x01, 0x??, 0x02,	0x??,..		//LineContents(1), Length, LineNumber(2), Invert, ScrollEnabled(x), ArrowStatus, Text(UTF-8 format)
	0x01, 0x??, 0x03,	0x??,..		//LineContents(1), Length, LineNumber(3), Invert, ScrollEnabled(x), ArrowStatus, Text(UTF-8 format)
	0x02, 0x05, 0x01, 0x00, 0x00 };	//CursorPosition(2), Length, CursorOn, XPosition, YPosition(line number)
};	


Configure	: CheckSum + Length(2) + Command ID(0x03) + SequenceNumber + tbl_Configure(10)
ACK			: CommandID(0x83) + SequenceNumber + status(0:Fail, 1:Pass)	
	
KeyPressed  : CheckSum + Length(2) + Command ID(0x04) + SequenceNumber + keyValue
ACK			: CommandID(0x84) + SequenceNumber + status(0:Fail, 1:Pass)	

KeyReleased	: CheckSum + Length(2) + Command ID(0x05) + SequenceNumber + keyValue
ACK			: CommandID(0x85) + SequenceNumber + status(0:Fail, 1:Pass)	

Sleep		: CheckSum + Length(2) + Command ID(0x06) + SequenceNumber
ACK			: CommandID(0x86) + SequenceNumber + status(0:Fail, 1:Pass)	

DeepSleep	: CheckSum + Length(2) + Command ID(0x07) + SequenceNumber
ACK			: CommandID(0x87) + SequenceNumber + status(0:Fail, 1:Pass)	

CommsError	: CheckSum + Length(2) + Command ID(0x08) + SequenceNumber + LastMessageID + LastMessageSequenceNumber
ACK			: CommandID(0x88) + SequenceNumber + status(0:Fail, 1:Pass)	

GetTimeAndDate	: CheckSum + Length(2) + Command ID(0x09) + SequenceNumber
TimeAndDate 	: CommandID(0x89) + SequenceNumber + Seconds(4) < since 1970, [0]=MSB, [3]=LSB >
	
GetTimeAndDate2	: CheckSum + Length(2) + Command ID(0x0a) + SequenceNumber
TimeAndDate 	: CommandID(0x8a)+SequenceNumber+Year(MSB)+Year(LSB)+Weekday(0~6)+Month(0~11)+Day(1~31)+Hour(0~23)+Minute(0~59)+Seconds(0~59)
	
GetState		: CheckSum + Length(2) + Command ID(0x0d) + SequenceNumber
State			: CommandID(0x8d) + SequenceNumber + Mode + PBState + NetworkState + ScreenState

GetStationName	: CheckSum + Length(2) + Command ID(0x0e) + SequenceNumber + InfoType + PresetNumber <0: currently playing station>
StationName		: CommandID(0x8e) + SequenceNumber + InfoType + PresetNumber + StationName(n) + null(0x00)
	
SetLanguage		: CheckSum + Length(2) + Command ID(0x0f) + SequenceNumber + LanguageString(5) + null(0x00)
ACK				: CommandID(0x8f) + SequenceNumber + status(0:Fail, 1:Pass)	

GetLanguage		: CheckSum + Length(2) + Command ID(0x10) + SequenceNumber
Language		: CommandID(0x90) + SequenceNumber + LanguageString(5) + null(0x00)
	
SaveToPreset	: CheckSum + Length(2) + Command ID(0x11) + PresetNumber
ACK				: CommandID(0x91) + SequenceNumber + status(0:Fail, 1:Pass)	
	
LoadPreset		: CheckSum + Length(2) + Command ID(0x12) + InfoType(0xff) + PresetNumber < 0xff: preset from current mode(IR/FM/DAB) >
ACK				: CommandID(0x92) + SequenceNumber + status(0:Fail, 1:Pass)	
		
-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
tx total : 5byte
-----------------------------------------------------------------------------*/
void Stingray_Tx_GetStatus(void)
{
   unsigned char chksum=0;
   unsigned char i;
   
	Stingray_Tx_mode = CMD_GetStatus;  
	for (i=0;i<5;i++) tx_ST_buf[i] = 0;
	tx_ST_buf[0] = chksum;									// XOR checksum
	tx_ST_buf[1] = 0x00;									// MSB
	tx_ST_buf[2] = 4;										// LSB
	tx_ST_buf[3] = CMD_GetStatus;
	tx_ST_buf[4] = ++Stingray_Tx_mode_SN;					// Sequence Number
//
	for (i=1;i<5;i++) chksum ^=tx_ST_buf[i];
	tx_ST_buf[0] = chksum;									// XOR checksum
//change to ASCII code
	for (i=0;i<5;i++) {
		tx_ASCII_buf[i*2] = tbl_ASCii[((tx_ST_buf[i]>>4) & 0x0f)];			
		tx_ASCII_buf[i*2+1] = tbl_ASCii[(tx_ST_buf[i] & 0x0f)];
	}
	tx_ASCII_buf[5*2] = 0x0d;		// '\r'
//11 byte rs232 Tx->	
#ifdef UART1_DEBUG
	for (i=0;i<(5*2+1);i++) PutChar(tx_ASCII_buf[i]);			//RS232
#endif
	for (i=0;i<(5*2+1);i++) PutChar3(tx_ASCII_buf[i]);			//Stingray
}
/*-----------------------------------------------------------------------------
tx total : (5+4)byte (not supported yet : send any number )
-----------------------------------------------------------------------------*/
void Stingray_Tx_SetConfigOption(void)
{
   unsigned char chksum=0;
   unsigned char i;
   
	Stingray_Tx_mode = CMD_SetConfigOption;  
	for (i=0;i<9;i++) tx_ST_buf[i] = 0;
	tx_ST_buf[0] = chksum;									// XOR checksum
	tx_ST_buf[1] = 0x00;									// MSB
	tx_ST_buf[2] = 8;										// LSB
	tx_ST_buf[3] = CMD_SetConfigOption;
	tx_ST_buf[4] = ++Stingray_Tx_mode_SN;					// Sequence Number
//
	tx_ST_buf[5] = 0x11;									// Option ID MSB
	tx_ST_buf[6] = 0x11;									// Option ID LSB
	tx_ST_buf[7] = 0x03;									// Option Type [ on/off ]
	tx_ST_buf[8] = 0x01;									// Option Data [ on=1   ]

	for (i=1;i<9;i++) chksum ^=tx_ST_buf[i];
	tx_ST_buf[0] = chksum;									// XOR checksum
//change to ASCII code
	for (i=0;i<9;i++) {
		tx_ASCII_buf[i*2] = tbl_ASCii[((tx_ST_buf[i]>>4) & 0x0f)];			
		tx_ASCII_buf[i*2+1] = tbl_ASCii[(tx_ST_buf[i] & 0x0f)];
	}
	tx_ASCII_buf[9*2] = 0x0d;		// '\r'
//19 byte rs232 Tx->	
#ifdef UART1_DEBUG
	for (i=0;i<(9*2+1);i++) PutChar(tx_ASCII_buf[i]);		//RS232
#endif
	for (i=0;i<(9*2+1);i++) PutChar3(tx_ASCII_buf[i]);		//Stingray
}
/*-----------------------------------------------------------------------------
tx total : (5+5+1)byte
-----------------------------------------------------------------------------*/
void Stingray_Tx_SetLanguage(void)
{
   unsigned char chksum=0;
   unsigned char i;
   
	Stingray_Tx_mode = CMD_SetLanguage;  
	for (i=0;i<11;i++) tx_ST_buf[i] = 0;
	tx_ST_buf[0] = chksum;									// XOR checksum
	tx_ST_buf[1] = 0x00;									// MSB
	tx_ST_buf[2] = 10;										// LSB
	tx_ST_buf[3] = CMD_SetLanguage;							// Command ID
	tx_ST_buf[4] = ++Stingray_Tx_mode_SN;					// Sequence Number
//
	tx_ST_buf[5] = 'e';											//
	tx_ST_buf[6] = 'n';											//
	tx_ST_buf[7] = '_';											//
	tx_ST_buf[8] = 'G';											//
	tx_ST_buf[9] = 'B';											//
//	for (i=5;i<10;i++) tx_ST_buf[i] = "en_GB";			// UK
	tx_ST_buf[10] = 0x00;										// null
		
	for (i=1;i<11;i++) chksum ^=tx_ST_buf[i];
	tx_ST_buf[0] = chksum;										// XOR checksum
//change to ASCII code
	for (i=0;i<11;i++) {
		tx_ASCII_buf[i*2] = tbl_ASCii[((tx_ST_buf[i]>>4) & 0x0f)];			
		tx_ASCII_buf[i*2+1] = tbl_ASCii[(tx_ST_buf[i] & 0x0f)];
	}
	tx_ASCII_buf[11*2] = 0x0d;		// '\r'
//23 byte rs232 Tx->	
#ifdef UART1_DEBUG
	for (i=0;i<(11*2+1);i++) PutChar(tx_ASCII_buf[i]);			//RS232
#endif
	for (i=0;i<(11*2+1);i++) PutChar3(tx_ASCII_buf[i]);			//Stingray
}
/*-----------------------------------------------------------------------------
tx total : (5+4+3+3)byte
		checksum + length(2) + command + SN						: 5 bytes
		SetDisplaySize(0x00) + length(1) + Rows + Columns		: 4 bytes
		SetAudioConfiguration(0x01) + length(1) + SampleRateID	: 3 bytes
		SetIType(0x03) + length(1) + UItype						: 3 bytes

-----------------------------------------------------------------------------*/
void Stingray_Tx_Configure(void)
{
   unsigned char chksum=0;
   unsigned char i;
   
	Stingray_Tx_mode = CMD_Configure;  
	for (i=0;i<15;i++) tx_ST_buf[i] = 0;
	tx_ST_buf[0] = chksum;										// XOR checksum
	tx_ST_buf[1] = 0x00;										// MSB
	tx_ST_buf[2] = 14;											// LSB
	tx_ST_buf[3] = CMD_Configure;								// Command ID
	tx_ST_buf[4] = ++Stingray_Tx_mode_SN;						// Sequence Number
//
	for (i=0;i<10;i++) tx_ST_buf[5+i] = tbl_Configure[i];		// configure table
		
	for (i=1;i<15;i++) chksum ^=tx_ST_buf[i];
	tx_ST_buf[0] = chksum;										// XOR checksum
//change to ASCII code
	for (i=0;i<15;i++) {
		tx_ASCII_buf[i*2] = tbl_ASCii[((tx_ST_buf[i]>>4) & 0x0f)];			
		tx_ASCII_buf[i*2+1] = tbl_ASCii[(tx_ST_buf[i] & 0x0f)];
	}
	tx_ASCII_buf[15*2] = 0x0d;		// '\r'
//31 byte rs232 Tx->	
#ifdef UART1_DEBUG
	for (i=0;i<(15*2+1);i++) PutChar(tx_ASCII_buf[i]);			//RS232	
#endif
	for (i=0;i<(15*2+1);i++) PutChar3(tx_ASCII_buf[i]);			//Stingray	
}
/*-----------------------------------------------------------------------------
tx total : (5+1)byte
-----------------------------------------------------------------------------*/
void Stingray_Tx_KeyPressed(unsigned char key_value)
{
   unsigned char chksum=0;
   unsigned char i;
   
	Stingray_Tx_mode = CMD_KeyPressed;  
	for (i=0;i<6;i++) tx_ST_buf[i] = 0;
	tx_ST_buf[0] = chksum;										// XOR checksum
	tx_ST_buf[1] = 0x00;										// MSB
	tx_ST_buf[2] = 5;											// LSB
	tx_ST_buf[3] = CMD_KeyPressed;
	tx_ST_buf[4] = ++Stingray_Tx_mode_SN;						// Sequence Number
//
	tx_ST_buf[5] = key_value;									// Key

	for (i=1;i<6;i++) chksum ^=tx_ST_buf[i];
	tx_ST_buf[0] = chksum;										// XOR checksum
//change to ASCII code
	for (i=0;i<6;i++) {
		tx_ASCII_buf[i*2] = tbl_ASCii[((tx_ST_buf[i]>>4) & 0x0f)];			
		tx_ASCII_buf[i*2+1] = tbl_ASCii[(tx_ST_buf[i] & 0x0f)];
	}
	tx_ASCII_buf[6*2] = 0x0d;		// '\r'
//13 byte rs232 Tx->	
#ifdef UART1_DEBUG
	for (i=0;i<(6*2+1);i++) PutChar(tx_ASCII_buf[i]);			//RS232	
#endif
	for (i=0;i<(6*2+1);i++) PutChar3(tx_ASCII_buf[i]);			//Stingray
	
//	Stingray_Key_OverTime = TIME10MSTO70MS;						//!!! key Tx => 70msec 후에  ACK 예상
}
/*-----------------------------------------------------------------------------
tx total : (5+1)byte
-----------------------------------------------------------------------------*/
void Stingray_Tx_KeyReleased(unsigned char key_value)
{
   unsigned char chksum=0;
   unsigned char i;
   
	Stingray_Tx_mode = CMD_KeyReleased;  
	for (i=0;i<6;i++) tx_ST_buf[i] = 0;
	tx_ST_buf[0] = chksum;										// XOR checksum
	tx_ST_buf[1] = 0x00;										// MSB
	tx_ST_buf[2] = 5;											// LSB
	tx_ST_buf[3] = CMD_KeyReleased;
	tx_ST_buf[4] = ++Stingray_Tx_mode_SN;						// Sequence Number
//
	tx_ST_buf[5] = key_value;									// Key

	for (i=1;i<6;i++) chksum ^=tx_ST_buf[i];
	tx_ST_buf[0] = chksum;										// XOR checksum
//change to ASCII code
	for (i=0;i<6;i++) {
		tx_ASCII_buf[i*2] = tbl_ASCii[((tx_ST_buf[i]>>4) & 0x0f)];			
		tx_ASCII_buf[i*2+1] = tbl_ASCii[(tx_ST_buf[i] & 0x0f)];
	}
	tx_ASCII_buf[6*2] = 0x0d;		// '\r'
//13 byte rs232 Tx->	
#ifdef UART1_DEBUG
	for (i=0;i<(6*2+1);i++) PutChar(tx_ASCII_buf[i]);			//RS232	
#endif
	for (i=0;i<(6*2+1);i++) PutChar3(tx_ASCII_buf[i]);			//Stingray	
}
/*-----------------------------------------------------------------------------
tx total : 5byte
-----------------------------------------------------------------------------*/
void Stingray_Tx_Sleep(void)
{
   unsigned char chksum=0;
   unsigned char i;
   
	Stingray_Tx_mode = CMD_Sleep;  
	for (i=0;i<5;i++) tx_ST_buf[i] = 0;
	tx_ST_buf[0] = chksum;										// XOR checksum
	tx_ST_buf[1] = 0x00;										// MSB
	tx_ST_buf[2] = 4;											// LSB
	tx_ST_buf[3] = CMD_Sleep;
	tx_ST_buf[4] = ++Stingray_Tx_mode_SN;						// Sequence Number
//
	for (i=1;i<5;i++) chksum ^=tx_ST_buf[i];
	tx_ST_buf[0] = chksum;										// XOR checksum
//change to ASCII code
	for (i=0;i<5;i++) {
		tx_ASCII_buf[i*2] = tbl_ASCii[((tx_ST_buf[i]>>4) & 0x0f)];			
		tx_ASCII_buf[i*2+1] = tbl_ASCii[(tx_ST_buf[i] & 0x0f)];
	}
	tx_ASCII_buf[5*2] = 0x0d;		// '\r'
//13 byte rs232 Tx->	
#ifdef UART1_DEBUG
	for (i=0;i<(5*2+1);i++) PutChar(tx_ASCII_buf[i]);			//RS232	
#endif
	for (i=0;i<(5*2+1);i++) PutChar3(tx_ASCII_buf[i]);			//Stingray	
}
/*-----------------------------------------------------------------------------
tx total : 5byte
-----------------------------------------------------------------------------*/
void Stingray_Tx_DeepSleep(void)
{
   unsigned char chksum=0;
   unsigned char i;
   
	Stingray_Tx_mode = CMD_DeepSleep;  
	for (i=0;i<5;i++) tx_ST_buf[i] = 0;
	tx_ST_buf[0] = chksum;										// XOR checksum
	tx_ST_buf[1] = 0x00;										// MSB
	tx_ST_buf[2] = 4;											// LSB
	tx_ST_buf[3] = CMD_DeepSleep;
	tx_ST_buf[4] = ++Stingray_Tx_mode_SN;						// Sequence Number
//
	for (i=1;i<5;i++) chksum ^=tx_ST_buf[i];
	tx_ST_buf[0] = chksum;										// XOR checksum
//change to ASCII code
	for (i=0;i<5;i++) {
		tx_ASCII_buf[i*2] = tbl_ASCii[((tx_ST_buf[i]>>4) & 0x0f)];			
		tx_ASCII_buf[i*2+1] = tbl_ASCii[(tx_ST_buf[i] & 0x0f)];
	}
	tx_ASCII_buf[5*2] = 0x0d;		// '\r'
//13 byte rs232 Tx->	
#ifdef UART1_DEBUG
	for (i=0;i<(5*2+1);i++) PutChar(tx_ASCII_buf[i]);			//RS232	
#endif
	for (i=0;i<(5*2+1);i++) PutChar3(tx_ASCII_buf[i]);			//Stingray	
}
/*-----------------------------------------------------------------------------
tx total : (5+1)byte
-----------------------------------------------------------------------------*/
void Stingray_Tx_SaveToPreset(unsigned char preset_value)
{
   unsigned char chksum=0;
   unsigned char i;
   
	Stingray_Tx_mode = CMD_SaveToPreset;  
	for (i=0;i<6;i++) tx_ST_buf[i] = 0;
	tx_ST_buf[0] = chksum;										// XOR checksum
	tx_ST_buf[1] = 0x00;										// MSB
	tx_ST_buf[2] = 5;											// LSB
	tx_ST_buf[3] = CMD_SaveToPreset;
	tx_ST_buf[4] = ++Stingray_Tx_mode_SN;						// Sequence Number
//
	tx_ST_buf[5] = preset_value;								// Preset

	for (i=1;i<6;i++) chksum ^=tx_ST_buf[i];
	tx_ST_buf[0] = chksum;										// XOR checksum
//change to ASCII code
	for (i=0;i<6;i++) {
		tx_ASCII_buf[i*2] = tbl_ASCii[((tx_ST_buf[i]>>4) & 0x0f)];			
		tx_ASCII_buf[i*2+1] = tbl_ASCii[(tx_ST_buf[i] & 0x0f)];
	}
	tx_ASCII_buf[6*2] = 0x0d;		// '\r'
//13 byte rs232 Tx->	
#ifdef UART1_DEBUG
	for (i=0;i<(6*2+1);i++) PutChar(tx_ASCII_buf[i]);			//RS232	
#endif
	for (i=0;i<(6*2+1);i++) PutChar3(tx_ASCII_buf[i]);			//Stingray	
}
/*-----------------------------------------------------------------------------
tx total : (5+2)byte
-----------------------------------------------------------------------------*/
//20100128
//iRadio(1~30)
//FM(31~60)
//DAB(61~90)
//20100128 : infoType
void Stingray_Tx_LoadPreset(unsigned char infoType,unsigned char preset_value)
{
   unsigned char chksum=0;
   unsigned char i;
   
	Stingray_Tx_mode = CMD_LoadPreset;  
	for (i=0;i<7;i++) tx_ST_buf[i] = 0;
	tx_ST_buf[0] = chksum;										// XOR checksum
	tx_ST_buf[1] = 0x00;										// MSB
	tx_ST_buf[2] = 6;											// LSB
	tx_ST_buf[3] = CMD_LoadPreset;
	tx_ST_buf[4] = ++Stingray_Tx_mode_SN;						// Sequence Number
//
	tx_ST_buf[5] = infoType;									// InfoType(current mode) = 0xff
	tx_ST_buf[6] = preset_value;								// Preset

	for (i=1;i<7;i++) chksum ^=tx_ST_buf[i];
	tx_ST_buf[0] = chksum;										// XOR checksum
//change to ASCII code
	for (i=0;i<7;i++) {
		tx_ASCII_buf[i*2] = tbl_ASCii[((tx_ST_buf[i]>>4) & 0x0f)];			
		tx_ASCII_buf[i*2+1] = tbl_ASCii[(tx_ST_buf[i] & 0x0f)];
	}
	tx_ASCII_buf[7*2] = 0x0d;		// '\r'
//13 byte rs232 Tx->	
#ifdef UART1_DEBUG
	for (i=0;i<(7*2+1);i++) PutChar(tx_ASCII_buf[i]);			//RS232	
#endif
	for (i=0;i<(7*2+1);i++) PutChar3(tx_ASCII_buf[i]);			//Stingray	
}
/*-----------------------------------------------------------------------------
tx total : 5byte
-----------------------------------------------------------------------------*/
void Stingray_Tx_GetState(void)
{
   unsigned char chksum=0;
   unsigned char i;
   
	Stingray_Tx_mode = CMD_GetState;  
	for (i=0;i<5;i++) tx_ST_buf[i] = 0;
	tx_ST_buf[0] = chksum;										// XOR checksum
	tx_ST_buf[1] = 0x00;										// MSB
	tx_ST_buf[2] = 4;											// LSB
	tx_ST_buf[3] = CMD_GetState;
	tx_ST_buf[4] = ++Stingray_Tx_mode_SN;						// Sequence Number
//
	for (i=1;i<5;i++) chksum ^=tx_ST_buf[i];
	tx_ST_buf[0] = chksum;										// XOR checksum
//change to ASCII code
	for (i=0;i<5;i++) {
		tx_ASCII_buf[i*2] = tbl_ASCii[((tx_ST_buf[i]>>4) & 0x0f)];			
		tx_ASCII_buf[i*2+1] = tbl_ASCii[(tx_ST_buf[i] & 0x0f)];
	}
	tx_ASCII_buf[5*2] = 0x0d;		// '\r'
//11 byte rs232 Tx->	
#ifdef UART1_DEBUG
	for (i=0;i<(5*2+1);i++) PutChar(tx_ASCII_buf[i]);			//RS232	
#endif
	for (i=0;i<(5*2+1);i++) PutChar3(tx_ASCII_buf[i]);			//Stingray
}
/*-----------------------------------------------------------------------------
tx total : (5+2)byte
-----------------------------------------------------------------------------*/
void Stingray_Tx_GetStationName(unsigned char infoType, unsigned char preset_value)
{
   unsigned char chksum=0;
   unsigned char i;
   
	Stingray_Tx_mode = CMD_GetStationName;  
	for (i=0;i<7;i++) tx_ST_buf[i] = 0;
	tx_ST_buf[0] = chksum;										// XOR checksum
	tx_ST_buf[1] = 0x00;										// MSB
	tx_ST_buf[2] = 6;											// LSB
	tx_ST_buf[3] = CMD_GetStationName;
	tx_ST_buf[4] = ++Stingray_Tx_mode_SN;						// Sequence Number
//
	tx_ST_buf[5] = infoType;									// InfoType : 0x00:internetRadio , 0x05:FM
	tx_ST_buf[6] = preset_value;								// Preset Number ( 0x00:current playing station )

	for (i=1;i<7;i++) chksum ^=tx_ST_buf[i];
	tx_ST_buf[0] = chksum;										// XOR checksum
//change to ASCII code
	for (i=0;i<7;i++) {
		tx_ASCII_buf[i*2] = tbl_ASCii[((tx_ST_buf[i]>>4) & 0x0f)];			
		tx_ASCII_buf[i*2+1] = tbl_ASCii[(tx_ST_buf[i] & 0x0f)];
	}
	tx_ASCII_buf[7*2] = 0x0d;		// '\r'
//13 byte rs232 Tx->	
#ifdef UART1_DEBUG
	for (i=0;i<(7*2+1);i++) PutChar(tx_ASCII_buf[i]);			//RS232
#endif
	for (i=0;i<(7*2+1);i++) PutChar3(tx_ASCII_buf[i]);			//Stingray
}
/*-----------------------------------------------------------------------------
tx total : 5byte
-----------------------------------------------------------------------------*/
void Stingray_Tx_FactoryReset(void)
{
   unsigned char chksum=0;
   unsigned char i;
   
   Stingray_ACK_status = 0xff;
	Stingray_Tx_mode = CMD_FactoryReset;  
	for (i=0;i<5;i++) tx_ST_buf[i] = 0;
	tx_ST_buf[0] = chksum;										// XOR checksum
	tx_ST_buf[1] = 0x00;										// MSB
	tx_ST_buf[2] = 4;											// LSB
	tx_ST_buf[3] = CMD_FactoryReset;
	tx_ST_buf[4] = ++Stingray_Tx_mode_SN;						// Sequence Number
//
	for (i=1;i<5;i++) chksum ^=tx_ST_buf[i];
	tx_ST_buf[0] = chksum;										// XOR checksum
//change to ASCII code
	for (i=0;i<5;i++) {
		tx_ASCII_buf[i*2] = tbl_ASCii[((tx_ST_buf[i]>>4) & 0x0f)];			
		tx_ASCII_buf[i*2+1] = tbl_ASCii[(tx_ST_buf[i] & 0x0f)];
	}
	tx_ASCII_buf[5*2] = 0x0d;		// '\r'
//11 byte rs232 Tx->	
#ifdef UART1_DEBUG
	for (i=0;i<(5*2+1);i++) PutChar(tx_ASCII_buf[i]);			//RS232
#endif
	for (i=0;i<(5*2+1);i++) PutChar3(tx_ASCII_buf[i]);			//Stingray
}
/*-----------------------------------------------------------------------------
20100129
tx total : 5byte + stringText[n] + null(0x00)
tx count : (4 + n + 1)
-----------------------------------------------------------------------------*/
//20100201
unsigned char stringKeyCheck(void)
{
	unsigned char i,j,k,size;

	if ((mode_DispBlockC==BLOCKC_NORMAL)&&(mode_func == FUNCTION_STINGRAY)) {
		if (uScreen.sStingray.ScreenState[0] == 2) {
			//text entry screen!
			switch (uScreen.sStingray.ScreenState[1]) {
				case 0:
					//pcStandardCharacters[] =	"ABCDEFGHIJKLMNOPQRSTUVWXYZ" 
					//							"abcdefghijklmnopqrstuvwxyz"
					//							"0123456789"
					//							"<|=>+-_!?@#$%^&*:;`'\".,\\/[]{}()~ ";
					for (i=0,size=0; (i<MAX_STRING_TEXT_LENGTH)&&(stringText[i]!='\0'); i++,size++){ }			//MAX=20
					//
					for (i=0;i<size;i++) {
						k = 0;
						for (j=0;(pcStandardCharacters[j]!='\0');j++) {
							if (stringText[i] == pcStandardCharacters[j]) {
								k = 1;
								break;
							}
						}
						if (k == 0) return(FALSE);
					}
					return(TRUE);
					break;	
				case 1:
					//pcIPAddressCharacters[] = "0123456789.";
					for (i=0,size=0; (i<MAX_STRING_TEXT_LENGTH)&&(stringText[i]!='\0'); i++,size++){ }			//MAX=20
					//
					for (i=0;i<size;i++) {
						k = 0;
						for (j=0;(pcIPAddressCharacters[j]!='\0');j++) {
							if (stringText[i] == pcIPAddressCharacters[j]) {
								k = 1;
								break;
							}
						}
						if (k == 0) return(FALSE);
					}
					return(TRUE);
					break;	
				case 2:
					//pcCallsignCharacters[] =	"ABCDEFGHIJKLMNOPQRSTUVWXYZ";
					for (i=0,size=0; (i<MAX_STRING_TEXT_LENGTH)&&(stringText[i]!='\0'); i++,size++){ }			//MAX=20
					//
					for (i=0;i<size;i++) {
						k = 0;
						for (j=0;(pcCallsignCharacters[j]!='\0');j++) {
							if (stringText[i] == pcCallsignCharacters[j]) {
								k = 1;
								break;
							}
						}
						if (k == 0) return(FALSE);
					}
					return(TRUE);
					break;	
				case 3:
					//
					//pcStationNameCharacters[] =	"ABCDEFGHIJKLMNOPQRSTUVWXYZ" 
					//								"0123456789"
					//								"<|=>+-_!?@#$%^&*:;`'\".,\\/[]{}()~ ";
					for (i=0,size=0; (i<MAX_STRING_TEXT_LENGTH)&&(stringText[i]!='\0'); i++,size++){ }			//MAX=20
					//
					for (i=0;i<size;i++) {
						k = 0;
						for (j=0;(pcStationNameCharacters[j]!='\0');j++) {
							if (stringText[i] == pcStationNameCharacters[j]) {
								k = 1;
								break;
							}
						}
						if (k == 0) return(FALSE);
					}
					return(TRUE);
					break;	
				default:
					return(FALSE);
					break;
			}
		}else{
			return(FALSE);
		}
	}else{
		return(FALSE);
	}
}
/*-----------------------------------------------------------------------------*/
void Stingray_Tx_SubmitText(void)
{
	unsigned char chksum=0;
	unsigned char i,size;
   
	if(stringKeyCheck() == TRUE ) {		//20100201

	for (i=0,size=0; (i<MAX_STRING_TEXT_LENGTH)&&(stringText[i]!='\0'); i++,size++){ }			//MAX=20
   
	Stingray_Tx_mode = CMD_SubmitText;  
	for (i=0;i<5;i++) tx_ST_buf[i] = 0;
	tx_ST_buf[0] = chksum;										// XOR checksum
	tx_ST_buf[1] = 0x00;										// MSB
	tx_ST_buf[2] = (4+size+1);									// LSB
	tx_ST_buf[3] = CMD_SubmitText;
	tx_ST_buf[4] = ++Stingray_Tx_mode_SN;						// Sequence Number
//
	for (i=0;i<(size+1);i++) tx_ST_buf[5+i]=stringText[i];		// StringText(n) + null(0x00)
//
	for (i=1;i<(4+size+1+1);i++) chksum ^=tx_ST_buf[i];
	tx_ST_buf[0] = chksum;										// XOR checksum
//change to ASCII code
	for (i=0;i<(4+size+1+1);i++) {
		tx_ASCII_buf[i*2] = tbl_ASCii[((tx_ST_buf[i]>>4) & 0x0f)];			
		tx_ASCII_buf[i*2+1] = tbl_ASCii[(tx_ST_buf[i] & 0x0f)];
	}
	tx_ASCII_buf[(4+size+1+1)*2] = 0x0d;		// '\r'
//n byte rs232 Tx->	
#ifdef UART1_DEBUG
	for (i=0;i<((4+size+1+1)*2+1);i++) PutChar(tx_ASCII_buf[i]);			//RS232	
#endif
	for (i=0;i<((4+size+1+1)*2+1);i++) PutChar3(tx_ASCII_buf[i]);			//Stingray

	}	//20100201
}
/*
20101007 : RotelOpaqueData Tx
	RotelOpaqueData[330]
	tx_OpaqueAscii_buf[1005]
-----------------------------------------------------------------------------*/
void Stingray_Tx_RotelOpaqueData(unsigned short Ssize)
{
#ifdef ASCiiKEY_test		//20110411
	unsigned char chksum=0;
	unsigned char Hsize,Lsize;
	unsigned short i,size;
   
	//?if(stringKeyCheck() == TRUE ) {		//20100201

	//for (i=0,size=0; (i<MAX_STRING_TEXT_LENGTH)&&(stringText[i]!='\0'); i++,size++){ }			//MAX=20
   
	Stingray_Tx_mode = CMD_OpaqueData;  						//(0x18)
	size = Ssize*2+5;									//4+(Ssize*2)+1(null)
	Hsize = (unsigned char)(size>>8)&0x00ff;
	Lsize = (unsigned char)(size)&0x00ff;
	//for (i=0;i<size;i++) tx_ST_buf[i] = 0;
	tx_ST_buf[0] = chksum;										// XOR checksum
	tx_ST_buf[1] = Hsize;										// MSB
	tx_ST_buf[2] = Lsize;										// LSB(4+size+1)
	tx_ST_buf[3] = CMD_OpaqueData;								//(0x18)
	tx_ST_buf[4] = ++Stingray_Tx_mode_SN;						// Sequence Number
//change to ASCII code	//나누어서 보냄..
	for (i=0;i<(Ssize);i++) {
		//tx_ST_buf[5+i]=RotelOpaqueData[i];					// RotelOpaqueData(n)*2 + null(0x00)
		tx_ST_buf[i*2+5] = tbl_ascii[((RotelOpaqueData[i]>>4) & 0x0f)];			
		tx_ST_buf[i*2+5+1] = tbl_ascii[(RotelOpaqueData[i] & 0x0f)];
	}
	tx_ST_buf[4+(Ssize*2)+1] = 0x30;		// null
	tx_ST_buf[4+(Ssize*2)+1+1] = 0x30;		// null
//
	for (i=1;i<(4+Ssize*2+1+1+1);i++) chksum ^=tx_ST_buf[i];
	tx_ST_buf[0] = chksum;										// XOR checksum

//20110408 : 한번만 나누어서 보내기 때문에 바로 move.
//load '\r'
	for (i=0;i<(4+Ssize*2+1+1+1);i++) {
		tx_ASCII_buf[i] = tx_ST_buf[i];			
	}
	tx_ASCII_buf[4+(Ssize*2)+1+1+1] = 0x0d;		// '\r'
//n byte rs232 Tx->	
#ifdef UART1_DEBUG		//20110408 test
	for (i=0;i<(4+(Ssize*2)+1+1+1+1);i++) PutChar(tx_ASCII_buf[i]);			//RS232	
#endif
	for (i=0;i<(4+(Ssize*2)+1+1+1+1);i++) PutChar3(tx_ASCII_buf[i]);		//Stingray

#else	//ASCiiKEY

	unsigned char chksum=0;
	unsigned char Hsize,Lsize;
	unsigned short i,size;
   
	//?if(stringKeyCheck() == TRUE ) {		//20100201

	//for (i=0,size=0; (i<MAX_STRING_TEXT_LENGTH)&&(stringText[i]!='\0'); i++,size++){ }			//MAX=20
   
	Stingray_Tx_mode = CMD_OpaqueData;  						//(0x18)
	size = Ssize*2+5;									//4+(Ssize*2)+1(null)
	Hsize = (unsigned char)(size>>8)&0x00ff;
	Lsize = (unsigned char)(size)&0x00ff;
	//for (i=0;i<size;i++) tx_ST_buf[i] = 0;
	tx_ST_buf[0] = chksum;										// XOR checksum
	tx_ST_buf[1] = Hsize;										// MSB
	tx_ST_buf[2] = Lsize;										// LSB(4+size+1)
	tx_ST_buf[3] = CMD_OpaqueData;								//(0x18)
	tx_ST_buf[4] = ++Stingray_Tx_mode_SN;						// Sequence Number
//change to ASCII code
	for (i=0;i<(Ssize);i++) {
		//tx_ST_buf[5+i]=RotelOpaqueData[i];					// RotelOpaqueData(n)*2 + null(0x00)
		tx_ST_buf[i*2+5] = tbl_ascii[((RotelOpaqueData[i]>>4) & 0x0f)];			
		tx_ST_buf[i*2+5+1] = tbl_ascii[(RotelOpaqueData[i] & 0x0f)];
	}
	tx_ST_buf[4+(Ssize*2)+1] = 0x00;		// null
//
	for (i=1;i<(4+Ssize*2+1+1);i++) chksum ^=tx_ST_buf[i];
	tx_ST_buf[0] = chksum;										// XOR checksum
//change to ASCII code
	for (i=0;i<(4+Ssize*2+1+1);i++) {
		tx_ASCII_buf[i*2] = tbl_ascii[((tx_ST_buf[i]>>4) & 0x0f)];			
		tx_ASCII_buf[i*2+1] = tbl_ascii[(tx_ST_buf[i] & 0x0f)];
	}
	tx_ASCII_buf[(4+Ssize*2+1+1)*2] = 0x0d;		// '\r'
//n byte rs232 Tx->	
#ifdef UART1_DEBUG		//20110314 test
	for (i=0;i<((4+Ssize*2+1+1)*2+1);i++) PutChar(tx_ASCII_buf[i]);			//RS232	
#endif
	for (i=0;i<((4+Ssize*2+1+1)*2+1);i++) PutChar3(tx_ASCII_buf[i]);			//Stingray

	//}	//20100201
#endif	//ASCiiKEY
}
/*-----------------------------------------------------------------------------
	Stingray initialize after reset
	
	byte Startup_mode;
	byte Tx_mode;
	byte Tx_mode_SN;
	byte Tx_time;
	byte Tx_step;
	byte ACK_mode;
	byte ACK_mode_SN;
	byte ACK_status;
	byte ACK_ScreenIconsBuf[4];
	byte ACK_ScreenLine0Buf[STATUS_SIZE];		//line1
	byte ACK_ScreenLine1Buf[STATUS_SIZE];		//line2
	byte ACK_ScreenLine2Buf[STATUS_SIZE];		//line3
	byte ACK_ScreenLine3Buf[STATUS_SIZE];		//line4
	byte ACK_ScreenCursorPosBuf[5];

-----------------------------------------------------------------------------*/
void Stingray_Init(void)
{
   word i,j;
   
	Stingray_Startup_mode = S_STARTUP;  
	Stingray_Tx_mode = 0;  
	Stingray_Tx_mode_SN = 0;  
	Stingray_Tx_time = TIME10MSTO100MS;  		//100msec = reset time
	Stingray_Tx_step = 0;  
	Stingray_ACK_mode = 0;  
	Stingray_ACK_mode_ID = 0;  
	Stingray_ACK_mode_SN = 0;  
	Stingray_ACK_status = 0xff;  
	Stingray_Status = 0xff;  

	Stingray_Key_Value = 0;
	Stingray_Key_OverTime = 0;	

	STRead_mode = STRX232_SYNC;
	STRead_count = 0;
	for (i=0;i<4;i++) {
		for (j=0;j<20;j++) {
			uScreen.sStingray.DispBuf[i][j] = 0x20;
			uAdjCurrent.sStingray.DispBuf[i][j] = 0x20;
			uCurrentScreen.sStingray.DispBuf[i][j] = 0x20;
		}
	}
	for (i=0;i<4;i++) {
		uScreen.sStingray.CursorXYPosition[i] = 0x00;
		uAdjCurrent.sStingray.CursorXYPosition[i] = 0x00;
		uCurrentScreen.sStingray.CursorXYPosition[i] = 0x00;
	}
//20100129 : ScreenState
	for (i=0;i<2;i++) {
		uScreen.sStingray.ScreenState[i] = 0x00;
		uAdjCurrent.sStingray.ScreenState[i] = 0x00;
		uCurrentScreen.sStingray.ScreenState[i] = 0x00;
	}
	UARTRead_count = 0;
	UARTRead_count50 = 0;		//20101213 

	mode_DisplayDAB = DAB_NORMAL;
	DAB_Preset_Reset();   

	Stingray_StateData[0] = 0xff;			//20090618
	count_state_Tx = TIME10MSTO100MS;
	
	//StationName clear
	for (i=0;i<30;i++) {
		//count(size) = 0 : 20100210
		uStingray.sSName.IRadioStationName[i][0] = 0;
		uStingray.sSName.FMStationName[i][0] = 0;
		uStingray.sSName.DABStationName[i][0] =0;
		for (j=1;j<(STATIONNAME_MAX_SIZE);j++) {			//20090617
			uStingray.sSName.IRadioStationName[i][j] = 0x20;
			uStingray.sSName.FMStationName[i][j] = 0x20;
			uStingray.sSName.DABStationName[i][j] =0x20;
		}
	}
	//DStationName clear : 20100210
	for (i=0;i<90;i++) {
		for (j=0;j<3;j++) uDispStingray.DStationName[i][j] = 0;
		for (j=3;j<(STATIONNAME_MAX_SIZE+3);j++) uDispStingray.DStationName[i][j] = 0x20;
	}
	//20100218
	mode_SortMaxPresetList = 0;
	mode_CursorAllPresetList = 0;
	
	mode_stingrayUpgradingFirmware = 0;		//20100426
}

/* 10msec check

Stingray_Startup_mode:

	? f_ACK_Key_OK == ON

**********************************************/
void Stingray_StartUpCheck(void)
{
//   unsigned char nbyte;

//	if (mode_func != FUNCTION_STINGRAY) return;
/*-----------------------------------------------------------------------------*/
	switch(Stingray_Startup_mode){
		case S_CONFIGURE_OK:
				return;	
			break;
		case S_INITIALIZE_FAIL:
				return;	
			break;
		case S_STARTUP:
			//reset time 100msec ---|___|---
			--Stingray_Tx_time;
			if (Stingray_Tx_time == 0x0000) {
				Stingray_Tx_step = 0;  
				//next step
				Stingray_Startup_mode = S_STARTUP1;
				Stingray_Tx_time = TIME10MSTO100MS;  		//reset => 100msec
				STRnRESET(1);
				Stingray_Screen0x82Count = 0;
			}
			break;
/*-----------------------------------------------------------------------------*/
		case S_STARTUP1:
			//Screen data 여러번 receive => start!
			if (Stingray_Screen0x82Count < 5) return;
			//100msec delay
			--Stingray_Tx_time;
			if (Stingray_Tx_time == 0x0000){
				Stingray_Tx_step = 0;  
repeat_Tx_Status:
				//next step
				Stingray_Startup_mode = S_ACKSTATUS;
				//Stingray_Tx_mode = 0;  
				//Stingray_Tx_mode_SN = 0;  
				Stingray_Tx_time = TIME10MSTO500MS;  		//500msec
				Stingray_ACK_mode = 0;  
				Stingray_ACK_mode_ID = 0;  
				Stingray_ACK_mode_SN = 0;  
				Stingray_ACK_status = 0xff;  
			   Stingray_Status = 0xff;  
				//GetStatus Tx
				Stingray_Tx_GetStatus();
			}
			break;
/*-----------------------------------------------------------------------------*/
/*
ACK80 :		Stingray_ACK_mode = STRead_JoinBuf[3];			//ACK.command (0x80 or ID)
			Stingray_ACK_mode_ID = STRead_JoinBuf[5];		//ACK.command ID (0x0e)
			Stingray_ACK_mode_SN = STRead_JoinBuf[6];		//ACK.SN (sequence number)
			Stingray_ACK_status = STRead_JoinBuf[7];		//ACK.Status
Station name(0x8e)
			Stingray_ACK_mode = STRead_JoinBuf[3];			//ACK.command ID (0x8e)
			Stingray_ACK_mode_ID = STRead_JoinBuf[3];		//ACK.command ID (0x8e)
			Stingray_ACK_mode_SN = STRead_JoinBuf[4];		//ACK.SN (sequence number)
*/
		case S_GETSTATUS:
		case S_ACKSTATUS:
			if (f_ACK_Key_OK == ON) {
				f_ACK_Key_OK = OFF;
				if ((Stingray_ACK_mode & 0x80) == 0x80) {
					if (Stingray_Tx_mode == (Stingray_ACK_mode & 0x7f)) {		//?(0x81)
						if (Stingray_Tx_mode_SN == Stingray_ACK_mode_SN) {
							if (Stingray_Status == 0x01){						//? configured
#ifdef PRESET_NAME_READING
								//next step : 20090904
								Stingray_Startup_mode = S_GETSTATE;
								//Stingray_Tx_mode = 0;  
								//Stingray_Tx_mode_SN = 0;  
								Stingray_Tx_time = TIME10MSTO100MS;  		//completed =>100msec
								Stingray_Tx_step = 0;  
								Stingray_ACK_mode = 0;  
								Stingray_ACK_mode_ID = 0;  
								Stingray_ACK_mode_SN = 0;  
								Stingray_ACK_status = 0xff;
			   				Stingray_Status = 0xff;
								Stingray_Tx_GetState();				//20090616
#else
								//configured!!! goto main
								//next step
								Stingray_Startup_mode = S_CONFIGURE_OK;
								//Stingray_Tx_mode = 0;  
								//Stingray_Tx_mode_SN = 0;  
								Stingray_Tx_time = TIME10MSTO500MS;  		//500msec
								Stingray_Tx_step = 0;  
								Stingray_ACK_mode = 0;  
								Stingray_ACK_mode_ID = 0;  
								Stingray_ACK_mode_SN = 0;  
								Stingray_ACK_status = 0xff;
			   				Stingray_Status = 0xff;  
								Stingray_Tx_GetState();				//20090616
#endif
							}else{
								Stingray_Tx_step = 0;  							//Tx count
repeat_Tx_SetConfigOption:
								//not configured
								//next step
								Stingray_Startup_mode = S_ACKCONFIGOPTION;
								//Stingray_Tx_mode = 0;  
								//Stingray_Tx_mode_SN = 0;  
								Stingray_Tx_time = TIME10MSTO500MS;  		//500msec
								Stingray_ACK_mode = 0;  
								Stingray_ACK_mode_ID = 0;  
								Stingray_ACK_mode_SN = 0;  
								Stingray_ACK_status = 0xff;
			   				Stingray_Status = 0xff;  
								//SetConfigOption Tx
								Stingray_Tx_SetConfigOption();
							}
						}
					}
				}
			}else{
				//Stingray_Tx_GetStatus() ==> ACK error
				if(--Stingray_Tx_time == 0x0000) {
					if (++Stingray_Tx_step > 3) goto reboot_StingrayModule;														
					goto repeat_Tx_Status;
				}
			}
			break;
/*-----------------------------------------------------------------------------*/
		case S_GETSTATE:
			//100msec delay
			--Stingray_Tx_time;
			if (Stingray_Tx_time == 0x0000){
				//iRadio StationName reading start!!!!!
				Stingray_StationPresetNumber = 1;			//start =>iRadio(1..30), FM(31..60), DAB(61..90)
				goto iRadio_GetStationNameRead;
			}
			break;
/*-----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------*/
/*
ACK80 :		Stingray_ACK_mode = STRead_JoinBuf[3];			//ACK.command (0x80 or ID)
			Stingray_ACK_mode_ID = STRead_JoinBuf[5];		//ACK.command ID (0x0e)
			Stingray_ACK_mode_SN = STRead_JoinBuf[6];		//ACK.SN (sequence number)
			Stingray_ACK_status = STRead_JoinBuf[7];		//ACK.Status
Station name(0x8e)
			Stingray_ACK_mode = STRead_JoinBuf[3];			//ACK.command ID (0x8e)
			Stingray_ACK_mode_ID = STRead_JoinBuf[3];		//ACK.command ID (0x8e)
			Stingray_ACK_mode_SN = STRead_JoinBuf[4];		//ACK.SN (sequence number)
*/
		case S_STATIONNAME_READ:
			if (f_ACK_Key_OK == ON) {
				f_ACK_Key_OK = OFF;
				if ((Stingray_ACK_mode == 0x80) || (Stingray_ACK_mode == 0x8e)) {
					if (Stingray_Tx_mode == (Stingray_ACK_mode_ID&0x7f)) {		//? CMD_GetStationName(0x0e)
						if (Stingray_Tx_mode_SN == Stingray_ACK_mode_SN) {
							if (++Stingray_StationPresetNumber > LAST_PRESET_NUMBER) {
								Stingray_Startup_mode = S_CONFIGURE_OK;
								//Stingray_Tx_mode = 0;  
								//Stingray_Tx_mode_SN = 0;  
								Stingray_Tx_time = TIME10MSTO100MS;  		//completed =>100msec
								Stingray_Tx_step = 0;  
								Stingray_ACK_mode = 0;  
								Stingray_ACK_mode_ID = 0;  
								Stingray_ACK_mode_SN = 0;  
								Stingray_ACK_status = 0xff;
			   				Stingray_Status = 0xff;
								Stingray_Tx_GetState();				//20090616
								makeSortPresetListTable();			//20100210
							}else{
iRadio_GetStationNameRead:
								//repeat StationName reading
								Stingray_Startup_mode = S_STATIONNAME_READ;
								//Stingray_Tx_mode = 0;  
								//Stingray_Tx_mode_SN = 0;  
								Stingray_Tx_time = TIME10MSTO400MS;  		//read =>400msec
								Stingray_ACK_mode = 0;  
								Stingray_ACK_mode_ID = 0;  
								Stingray_ACK_mode_SN = 0;  
								Stingray_ACK_status = 0xff;
			   				Stingray_Status = 0xff;  
								//GetStationName(iRadio(0)orFM(5)orDAB(0c), PresetNmuber)
#ifdef UART1_DEBUG
		rs232_printf("read %x\n",Stingray_StationPresetNumber);
#endif
								if (Stingray_StationPresetNumber < 31) {
									Stingray_Tx_GetStationName(0x00,Stingray_StationPresetNumber);			//iRadio 1..30
								}else if (Stingray_StationPresetNumber < 61) {
									Stingray_Tx_GetStationName(0x05,Stingray_StationPresetNumber-30);		//FM 1..30
								}else{
									Stingray_Tx_GetStationName(0x0c,Stingray_StationPresetNumber-60);		//DAB 1..30
								}
							}
						}
					}
				}
			}else{
				//Stingray_Tx_GetStationName() ==> ACK error
				if(--Stingray_Tx_time == 0x0000) {
					goto iRadio_GetStationNameRead;
				}
			}
			break;
/*-----------------------------------------------------------------------------*/
		case S_SETCONFIGOPTION:
		case S_ACKCONFIGOPTION:
			if (f_ACK_Key_OK == ON) {
				f_ACK_Key_OK = OFF;
				if ((Stingray_ACK_mode & 0x80) == 0x80) {
					if (Stingray_Tx_mode == Stingray_ACK_mode_ID) {		//? SetconfigOption(0x0b)
						if (Stingray_Tx_mode_SN == Stingray_ACK_mode_SN) {
							if (Stingray_ACK_status == 0x01){
								Stingray_Tx_step = 0;  							//Tx count
repeat_Tx_SetLanguage:
								//Pass!!! 
								//next step
								Stingray_Startup_mode = S_ACKLANGUAGE;
								//Stingray_Tx_mode = 0;  
								//Stingray_Tx_mode_SN = 0;  
								Stingray_Tx_time = TIME10MSTO500MS;  		//500msec
								Stingray_ACK_mode = 0;  
								Stingray_ACK_mode_ID = 0;  
								Stingray_ACK_mode_SN = 0;  
								Stingray_ACK_status = 0xff;
			   				Stingray_Status = 0xff;  
								//SetLanguage Tx
								Stingray_Tx_SetLanguage();
							}
						}
					}
				}
			}else{
				//Stingray_Tx_SetConfigOption() ==> ACK error
				if (--Stingray_Tx_time == 0x0000) {
					if (++Stingray_Tx_step > 3) goto reboot_StingrayModule;														
					goto repeat_Tx_SetConfigOption;
				}
			}
			break;
/*-----------------------------------------------------------------------------*/
		case S_SETLANGUAGE:
		case S_ACKLANGUAGE:
			if (f_ACK_Key_OK == ON) {
				f_ACK_Key_OK = OFF;
				if ((Stingray_ACK_mode & 0x80) == 0x80) {
					if (Stingray_Tx_mode == Stingray_ACK_mode_ID) {				//? SetLanguage(0x0f)
						if (Stingray_Tx_mode_SN == Stingray_ACK_mode_SN) {
							if (Stingray_ACK_status == 0x01){
								Stingray_Tx_step = 0;  							//Tx count
repeat_Tx_Configure:
								//Pass!!! 
								//next step
								Stingray_Startup_mode = S_ACKCONFIGURE;
								//Stingray_Tx_mode = 0;  
								//Stingray_Tx_mode_SN = 0;  
								Stingray_Tx_time = TIME10MSTO500MS;  		//500msec
								Stingray_ACK_mode = 0;  
								Stingray_ACK_mode_ID = 0;  
								Stingray_ACK_mode_SN = 0;  
								Stingray_ACK_status = 0xff;
			   				Stingray_Status = 0xff;  
								//Configure Tx
								Stingray_Tx_Configure();
							}
						}
					}
				}
			}else{
				//Stingray_Tx_SetLanguage() ==> ACK error
				if (--Stingray_Tx_time == 0x0000) {
					if (++Stingray_Tx_step > 3) goto reboot_StingrayModule;														
					goto repeat_Tx_SetLanguage;
				}
			}
			break;
/*-----------------------------------------------------------------------------*/
		case S_TX_CONFIGURE:
		case S_ACKCONFIGURE:
			if (f_ACK_Key_OK == ON) {
				f_ACK_Key_OK = OFF;
				if ((Stingray_ACK_mode & 0x80) == 0x80) {
					if (Stingray_Tx_mode == Stingray_ACK_mode_ID) {				//? Congigure(0x03)
						if (Stingray_Tx_mode_SN == Stingray_ACK_mode_SN) {
							if (Stingray_ACK_status == 0x01){
								//Pass!!! 
								//Txcompleted configure => reboot
								//nReset => Low
reboot_StingrayModule:
#ifdef UART1_DEBUG
		rs232_printf("reBooting\r",blank);
#endif
								STRnRESET(0);
								Stingray_Startup_mode = S_REBOOTING;
								Stingray_Tx_mode = 0;  
								Stingray_Tx_mode_SN = 0;  
								Stingray_Tx_time = TIME10MSTO100MS;  		//reset =>100msec
								Stingray_Tx_step = 0;  
								Stingray_ACK_mode = 0;  
								Stingray_ACK_mode_ID = 0;  
								Stingray_ACK_mode_SN = 0;  
								Stingray_ACK_status = 0xff;
			   				Stingray_Status = 0xff;  
							}
						}
					}
				}
			}else{
				//Stingray_Tx_Configure() ==> ACK error
				if (--Stingray_Tx_time == 0x0000) {
					if (++Stingray_Tx_step > 3) goto reboot_StingrayModule;														
					goto repeat_Tx_Configure;
				}
			}
			break;
/*-----------------------------------------------------------------------------*/
		case S_REBOOTING:
			--Stingray_Tx_time;
			if (Stingray_Tx_time == 0x0000) {
				//nReset => High
				STRnRESET(1);
				Stingray_Init();
			}
			break;
		default:
			break;
	}
}

/*-----------------------------------------------------------------------------
 Function Name	: rs232_GetRxCommand
 Description	: Get rx command from rx buf
 Input			: None
 Output			: None
 Return			: TRUE/FALSE
 Comments		: ..
 	
ACK				: CommandID(0x80+?) + SequenceNumber + Status(0:Fail, 1:Pass)
Stingray_Startup_mode:


//Screen => display data
	byte Startup_mode;
	byte Tx_mode;
	byte Tx_mode_SN;
	byte Tx_time;
	byte Tx_step;
	byte ACK_mode;
	byte ACK_mode_SN;
	byte ACK_status;
	byte ACK_ScreenIconsBuf[4];
	byte ACK_ScreenLine0Buf[STATUS_SIZE];		//line1
	byte ACK_ScreenLine1Buf[STATUS_SIZE];		//line2
	byte ACK_ScreenLine2Buf[STATUS_SIZE];		//line3
	byte ACK_ScreenLine3Buf[STATUS_SIZE];		//line4
	byte ACK_ScreenCursorPosBuf[5];
-----------------------------------------------------------------------------*/
byte Get_HexDigit(unsigned char hex)
{
	if(hex <= 0x39){
		return(hex-0x30);
	}else{
		return(hex-0x37);
	}
}
/*
	Stingray response reading
------------------------------------------------------------------*/
byte uart3_GetRxCommand(void)
{	
	word i,j,count_max;
	unsigned char temp;
	byte command_ok;
	unsigned char Opaque_key_buffer[34];
	int cmd_code;
	    
//	__asm("nop");
	command_ok = FALSE;
	if (rs232_Rx3_Head != rs232_Rx3_Tail) {
		do{
			++rs232_Rx3_Tail;
			if (rs232_Rx3_Tail >= RX3_MAX_SIZE) rs232_Rx3_Tail = 0;
				
			UARTRead_buf[UARTRead_count] = rx3_buf[rs232_Rx3_Tail];

			if (UARTRead_buf[UARTRead_count] == 0x0d)
			{
				if(UARTRead_count < 11) 
				{
					UARTRead_count = 0;
					UARTRead_count50 = 0;				//20101213
					goto UARTRead_Err;
				}
#if 0 //def 0 : chd test : 20090903
				rs232_printf(" UARTRead_count= %x\r",UARTRead_count);
#endif
				//data join start
				i=0;
				j=0;
				do 
				{
					temp = Get_HexDigit(UARTRead_buf[i++]);
					temp = (temp << 4) & 0xf0;
					STRead_JoinBuf[j++] = temp | (Get_HexDigit(UARTRead_buf[i++]) & 0x0f );
				}while(i < UARTRead_count);

				command_ok = TRUE;
				UARTRead_count = 0;
				UARTRead_count50 = 0;				//20101213
			}else{
				if (++UARTRead_count >= 500) 
				{
					UARTRead_count=0;					//? max size = 500
					UARTRead_count50 = 0;			//20101213
					goto UARTRead_Err;
				}
				++UARTRead_count50;
				if ((UARTRead_count%80) == 0 ) UARTRead_count50=0;				
			}
		}while((rs232_Rx3_Head != rs232_Rx3_Tail) && (command_ok == FALSE) && (UARTRead_count50 !=0)); 
#if 0 //def 0 : chd test : 20090903
				rs232_printf(" UARTRead_count= %x %x\r",UARTRead_count, UARTRead_count50);
#endif
	}
//	
//
//
UARTRead_Err:
	if (command_ok == TRUE) 
	{
		//insert VFD display 		
		if(VFD_scanStop == 0)	//20101214 : flicker VFD
		{
			vfd_processing();
		}else{
			VFD_Bk(0);
		}				
		//checksum check
		temp = 0;
		for (i=1;i<j;i++) temp ^= STRead_JoinBuf[i];
		if (STRead_JoinBuf[0] == temp) {
			//OK checksum
			STRead_count = 0;

			if (STRead_JoinBuf[3] == 0x82){				//? Screen==0x82
				STRead_mode = STRX232_SCREENSN;
				if (Stingray_Screen0x82Count < 9) ++Stingray_Screen0x82Count;
			}else if (STRead_JoinBuf[3] == 0x8d){		//? State==0x8d
				STRead_mode = STRX232_STATESN;
			}else if (STRead_JoinBuf[3] == 0x8e){		//? StationName==0x8e
				STRead_mode = STRX232_STATIONNAMESN;
			}else if (STRead_JoinBuf[3] == 0x80){		//? Status==0x80
				STRead_mode = STRX232_ACK80;
			}else if (STRead_JoinBuf[3] == 0x81){		//? GetStatus==0x81
				STRead_mode = STRX232_ACK81;
#ifdef ENABLE_KEY_OPAQUE
			}else if (STRead_JoinBuf[3] == 0x98){		//? KeyOpaqueText==0x98
				STRead_mode = STRX232_KeyOPAQUE;
#endif  //ENABLE_KEY_OPAQUE
			}else{
				STRead_mode = STRX232_ACKERR;			//? 0x80
			}
			Stingray_ACK_mode = STRead_JoinBuf[3];		//ACK.command (0x80 or ID)
//
//
//
			switch (STRead_mode) {
/*------------------------------------------------------------------*/
#ifdef ENABLE_KEY_OPAQUE

#define	rs232_id_rdg1520	0x25			//2010927
#define	rs232_id_rt09		0x26			//2010927
#define	rs232_id_rcx1500	0x62			//2010927
//1. KeyOPAQUE(iPhone->unit) :
//				[chksmu][0x0012][0x98][sn]	
// 										  [fe]                                           
// 											[6][6][6][5]	//5,6,7,8
//											[count]
//											[3][0][3][3]	//9,10,11,12
//											[id]
//											[3][6][3][2]
//											[type]
//											[3][1][3][0]
//											[key]
//											[3][0][3][1]
//											[chksum][null]
//											[3][7][3][6][00]
//1.1 KeyOPAQUE(iPhone->unit) :
//				[chksmu][0x0041][0x98][sn]	
// 										  [fe]                                           
// 											[0][x][6][6][b][0][x][6][5][b]		//7,8,12,13
//											[count]
//											[0][x][3][0][b][0][x][3][3][b]		//17,18,22,23
//											[id]
//											[0][x][3][6][b][0][x][3][2][b]
//											[type]
//											[0][x][3][1][b][0][x][3][0][b]
//											[key]
//											[0][x][3][0][b][0][x][3][1][b]
//											[chksum][null]
//											[0][x][3][7][b][0][x][3][6][00]
//							
//PowerOn 	:	[chksmu][0x0007][0x98][sn][0xfe][0x03][0x62][0x10][0x01][chksum][0x0a]
//PowerOff 	:	[chksmu][0x0007][0x98][sn][0xfe][0x03][0x62][0x10][0x02][chksum][0x0a]
//Vol + 	:	[chksmu][0x0007][0x98][sn][0xfe][0x03][0x62][0x10][0x03][chksum][0x0a]
//Vol - 	:	[chksmu][0x0007][0x98][sn][0xfe][0x03][0x62][0x10][0x04][chksum][0x0a]
//Mute  	:	[chksmu][0x0007][0x98][sn][0xfe][0x03][0x62][0x10][0x05][chksum][0x0a]
//CD    	:	[chksmu][0x0007][0x98][sn][0xfe][0x03][0x62][0x10][0x06][chksum][0x0a]
//Network 	:	[chksmu][0x0007][0x98][sn][0xfe][0x03][0x62][0x10][0x07][chksum][0x0a]
//USB   	:	[chksmu][0x0007][0x98][sn][0xfe][0x03][0x62][0x10][0x08][chksum][0x0a]
//AUX 1 	:	[chksmu][0x0007][0x98][sn][0xfe][0x03][0x62][0x10][0x09][chksum][0x0a]
//AUX 2 	:	[chksmu][0x0007][0x98][sn][0xfe][0x03][0x62][0x10][0x0A][chksum][0x0a]
//FM    	:	[chksmu][0x0007][0x98][sn][0xfe][0x03][0x62][0x10][0x0B][chksum][0x0a]
//DAB   	:	[chksmu][0x0007][0x98][sn][0xfe][0x03][0x62][0x10][0x0C][chksum][0x0a]
//iRadio  	:	[chksmu][0x0007][0x98][sn][0xfe][0x03][0x62][0x10][0x0D][chksum][0x0a]
//Menu  	:
//Memory
//
//Opaque_key_buffer[0] : start[0xfe]
//Opaque_key_buffer[1] : count[0x03]
//Opaque_key_buffer[2] : ID [RCX=0x62, RDG=0x25, RT=0x26  ]
//Opaque_key_buffer[3] : type[0x10]
//Opaque_key_buffer[4] : data
//Opaque_key_buffer[5] : checksum
//
//#ifdef ASCiiKEY

//
//1.1 KeyOPAQUE(iPhone->unit) :
//  [chksum]    - 1byte
//  [0x0041]    - 2bytes
//  [0x98]      - 1byte
//  [sn]        - 1byte
//  [......!]   - (n)bytes
//  [00]        - 1byte
//
//

					case STRX232_KeyOPAQUE:
						OpaqueKey_Type = 0xff;
						OpaqueKey_Value = 0xff;
						//Opaque_key_buffer[0] = Asc2Bin(Asc2Bin(STRead_JoinBuf[7],STRead_JoinBuf[8]),Asc2Bin(STRead_JoinBuf[12],STRead_JoinBuf[13]));		//0xfe
						//Opaque_key_buffer[1] = Asc2Bin(Asc2Bin(STRead_JoinBuf[17],STRead_JoinBuf[18]),Asc2Bin(STRead_JoinBuf[22],STRead_JoinBuf[23]));	//count
						//Opaque_key_buffer[2] = Asc2Bin(Asc2Bin(STRead_JoinBuf[27],STRead_JoinBuf[28]),Asc2Bin(STRead_JoinBuf[32],STRead_JoinBuf[33]));	//ID
						//Opaque_key_buffer[3] = Asc2Bin(Asc2Bin(STRead_JoinBuf[37],STRead_JoinBuf[38]),Asc2Bin(STRead_JoinBuf[42],STRead_JoinBuf[43]));	//type
						//Opaque_key_buffer[4] = Asc2Bin(Asc2Bin(STRead_JoinBuf[47],STRead_JoinBuf[48]),Asc2Bin(STRead_JoinBuf[52],STRead_JoinBuf[53]));	//data
						//Opaque_key_buffer[5] = Asc2Bin(Asc2Bin(STRead_JoinBuf[57],STRead_JoinBuf[58]),Asc2Bin(STRead_JoinBuf[62],STRead_JoinBuf[63]));	//checksum
						//20101028 "0X", space => delete!!!
						//for(i=0;i<6;i++) Opaque_key_buffer[i] = Asc2Bin( Asc2Bin(STRead_JoinBuf[i*10+7],STRead_JoinBuf[i*10+8]),Asc2Bin(STRead_JoinBuf[i*10+12],STRead_JoinBuf[i*10+13]) );
#ifdef ASCiiKEY
						//[!] <= [32][31] :20110408
						for(i=0;(i<32)&&(STRead_JoinBuf[i+5]!='\0');i++) Opaque_key_buffer[i] = STRead_JoinBuf[i+5];
						if (Opaque_key_buffer[i-1] =='!') {
							Opaque_key_buffer[i-1] = 0;
							cmd_code = getCmdID(Opaque_key_buffer);
							if (cmd_code < 0) {
								//key error!
								//rs232_printf("ASCiiKey error!",blank);
							}else{
								if (key_Type == 0x30) {         //?direct volume
		    						//rs232_printf("ASCiiKey=>%x\n",cmd_code);
									OpaqueKey_Type = key_Type;
									OpaqueKey_Value = (unsigned char)cmd_code;				
								}else if ((key_Type == 0x31)||(key_Type == 0x32)||(key_Type == 0x33)) {         //?get iradio/fm/dab preset
		    						//rs232_printf("ASCiiKey=>%x\n",cmd_code);
									OpaqueKey_Type = key_Type;
									OpaqueKey_Value = (unsigned char)cmd_code;				
//20110415
								}else if ((key_Type == 0x41)||(key_Type == 0x42)||(key_Type == 0x43)) {         //?call iradio/fm/dab preset
		    						//rs232_printf("ASCiiKey=>%x\n",cmd_code);
									OpaqueKey_Type = key_Type;
									OpaqueKey_Value = (unsigned char)cmd_code;				
								}else{
		    						//key_Type = 0x10;
									OpaqueKey_Type = key_Type;	//20110408
#ifdef RCX1500
extern const byte AsciiKeyValueTable_RCX[];
		    						OpaqueKey_Value = AsciiKeyValueTable_RCX[(unsigned char)cmd_code];           //20110225
#else
extern const byte AsciiKeyValueTable_RDG[];
		    						OpaqueKey_Value = AsciiKeyValueTable_RDG[(unsigned char)cmd_code];           //20110228
#endif
		    						//rs232_printf("ASCiiKey=>%x\n",cmd_code);
        						}
        					}
						}
#else   //ASCiiKEY
						//[fe] <= [66][65] <= [36][36][36][35]
						for(i=0;i<6;i++) Opaque_key_buffer[i] = Asc2Bin( Asc2Bin(STRead_JoinBuf[i*4+5],STRead_JoinBuf[i*4+6]),Asc2Bin(STRead_JoinBuf[i*4+7],STRead_JoinBuf[i*4+8]) );
#ifdef RCX1500
						//?RCX1500 check
						if ((Opaque_key_buffer[0] == 0xfe) && (Opaque_key_buffer[2] == rs232_id_rcx1500)) {
							OpaqueKey_Type = Opaque_key_buffer[3];
							OpaqueKey_Value = Opaque_key_buffer[4];
						}
#else
						if (mode_RT09 == 0) {
							//?RDG1520 check
							if ((Opaque_key_buffer[0] == 0xfe) && (Opaque_key_buffer[2] == rs232_id_rdg1520)) {
								OpaqueKey_Type = Opaque_key_buffer[3];
								OpaqueKey_Value = Opaque_key_buffer[4];
							}
						}else{
							//?RT-09 check
							if ((Opaque_key_buffer[0] == 0xfe) && (Opaque_key_buffer[2] == rs232_id_rt09)) {
								OpaqueKey_Type = Opaque_key_buffer[3];
								OpaqueKey_Value = Opaque_key_buffer[4];
							}
						}
#endif  //RCX1500
#endif  //ASCiiKEY
						break;
#endif  //ENABLE_KEY_OPAQUE
/*------------------------------------------------------------------*/
/*------------------------------------------------------------------*/
//ACKERR
					case STRX232_ACKERR:
						Stingray_ACK_mode = 0x20;			//ACK.command ID
						Stingray_ACK_mode_SN = 0x20;		//ACK.SN (sequence number)
						Stingray_ACK_status = 0x20;		//ACK.Status
						command_ok = FALSE;
						STRead_mode = STRX232_SYNC;
						STRead_count = 0;
#ifdef UART1_DEBUG
		rs232_printf("ACKerr\r",blank);
//change to ASCII code
		for (i=0;i<(STRead_JoinBuf[2]+1);i++) {
			PutChar(tbl_ASCii[((STRead_JoinBuf[i]>>4) & 0x0f)]);
			PutChar(tbl_ASCii[(STRead_JoinBuf[i] & 0x0f)]);
		}
		rs232_printf("\r",blank);
#endif
						break;
/*------------------------------------------------------------------*/
//ACK(0x80) [chksmu][0x0007][0x80][sn][ID][SN][status]
//ACK(0x80) [chksmu][0x0007][0x80][sn][0e][SN][status]		: GetStationName ACK : Status(0x00)=no-memory
					case STRX232_ACK80:
						Stingray_ACK_mode_ID = STRead_JoinBuf[5];		//ACK.command ID
						Stingray_ACK_mode_SN = STRead_JoinBuf[6];		//ACK.SN (sequence number)
						Stingray_ACK_status = STRead_JoinBuf[7];		//ACK.Status

						command_ok = TRUE;
						STRead_mode = STRX232_SYNC;
						STRead_count = 0;
						//FactoryReset [ 0x14 ] : 20090714
						if ((Stingray_ACK_mode_ID == 0x14)&&(Stingray_ACK_status == 0x01))
						{
							count_factoryDefault = TIME10MSTO3SEC;
							//Stingray_Init();
							//STRnRESET(0);
						}
						
//		PutChar(Stingray_ACK_status);
//		rs232_printf("ACK Status=>%x\n",Stingray_ACK_status);
#ifdef UART1_DEBUG
		rs232_printf("ACK Status0x80\r",blank);
//change to ASCII code
		for (i=0;i<(STRead_JoinBuf[2]+1);i++) {
			PutChar(tbl_ASCii[((STRead_JoinBuf[i]>>4) & 0x0f)]);
			PutChar(tbl_ASCii[(STRead_JoinBuf[i] & 0x0f)]);
		}
		rs232_printf("\r",blank);
#endif
						break;

/*------------------------------------------------------------------*/
//Status(0x81) [chksmu][0x0005][0x81][SN][status]
					case STRX232_ACK81:
						Stingray_ACK_mode_SN = STRead_JoinBuf[4];		//ACK.SN (sequence number)
						Stingray_ACK_status = STRead_JoinBuf[5];		//ACK.Status
						Stingray_Status = STRead_JoinBuf[5];			//ACK.Status

						command_ok = TRUE;
						STRead_mode = STRX232_SYNC;
						STRead_count = 0;
//		PutChar(Stingray_ACK_status);
//		rs232_printf("ACK Status=>%x\n",Stingray_ACK_status);
#ifdef UART1_DEBUG
		rs232_printf("ACK Status0x81\r",blank);
//change to ASCII code
		for (i=0;i<(STRead_JoinBuf[2]+1);i++) {
			PutChar(tbl_ASCii[((STRead_JoinBuf[i]>>4) & 0x0f)]);
			PutChar(tbl_ASCii[(STRead_JoinBuf[i] & 0x0f)]);
		}
		rs232_printf("\r",blank);
#endif
						break;
/*------------------------------------------------------------------*/
//State(0x8d)
					case STRX232_STATESN:
						Stingray_StateData[0] = STRead_JoinBuf[5];	//Mode
						Stingray_StateData[1] = STRead_JoinBuf[6];	//PlaybackState
						Stingray_StateData[2] = STRead_JoinBuf[7];	//NetworkState
						Stingray_StateData[3] = STRead_JoinBuf[8];	//ScreenState

						command_ok = TRUE;
						STRead_mode = STRX232_SYNC;
						STRead_count = 0;
						//20090618 state save!!!!!!!!
						//iRadio(0x00), MMedia(0x01), FM(0x05), DAB(0x0c) check
						if ((Stingray_StateData[0] == 0x00) || (Stingray_StateData[0] == 0x01)|| (Stingray_StateData[0] == 0x05) || (Stingray_StateData[0] == 0x0c))
						{
							if (mode_stingray != Stingray_StateData[0]){
								mode_stingray = Stingray_StateData[0];
								DAC_volume();							//254(FM=245) : 20090709
								Antenna_setting(mode_AntennaAB);				//20090812
#ifdef ASCiiKEY		//20110907
								//20110907 changed current source=>iPhone APP => delete 20111006
								//if(mode_func == FUNCTION_STINGRAY) {
								//	Func_SourceStatus();
								//}
#endif					
							}
						}
#ifdef UART1_DEBUG
		rs232_printf("State\r",blank);
//change to ASCII code
		for (i=0;i<(STRead_JoinBuf[2]+1);i++) {
			PutChar(tbl_ASCii[((STRead_JoinBuf[i]>>4) & 0x0f)]);
			PutChar(tbl_ASCii[(STRead_JoinBuf[i] & 0x0f)]);
		}
		rs232_printf("\r",blank);
#endif
/* 20090616
						//iRadio(0x00), FM(0x05), DAB(0x0c) check
						if ((Stingray_StateData[0] == 0x00) || (Stingray_StateData[0] == 0x05) || (Stingray_StateData[0] == 0x0c)) {
							//memory key & preset key
							if (presetKey_imsi == 0xff) {				//? memory key
								Func_MemoryStart();
								count_memory_state = 0;
							}else{
								Func_STMPresetStart(presetKey_imsi);
								count_preset_state = 0;
							}
							//count_mode_reset = TIME10MSTO20SEC;			// 20sec 뒤에 Stingray_StateData[0] clear!!!
						}
*/
						break;
/*------------------------------------------------------------------*/
//Station name(0x8e)
					case STRX232_STATIONNAMESN:
						Stingray_ACK_mode_ID = STRead_JoinBuf[3];		//ACK.command ID
						Stingray_ACK_mode_SN = STRead_JoinBuf[4];		//ACK.SN (sequence number)
						//Stingray_StationInfo = STRead_JoinBuf[5];				//InfoType : iRadio(0x00), FM(0x05), DAB(0x0c)
						//Stingray_StationPresetNumber = STRead_JoinBuf[6];	//PresetNumber
						if (STRead_JoinBuf[5] == 0x00) {
							//internetRadio (0x00)
							STRead_count = STRead_JoinBuf[2] - 6 - 1;
							//(STATIONNAME_MAX_SIZE=40) 20090617
							if (STRead_count >= STATIONNAME_MAX_SIZE ) {
								STRead_count = STATIONNAME_MAX_SIZE-1;		
								for (i=0;i<STRead_count;i++) uStingray.sSName.IRadioStationName[STRead_JoinBuf[6]-1][i+1] = STRead_JoinBuf[7+i];
							}else{
								for (i=0;i<STRead_count;i++) uStingray.sSName.IRadioStationName[STRead_JoinBuf[6]-1][i+1] = STRead_JoinBuf[7+i];
								for (i=STRead_count;i<(STATIONNAME_MAX_SIZE-1);i++) uStingray.sSName.IRadioStationName[STRead_JoinBuf[6]-1][i+1] = 0x20;
							}
							//size
							uStingray.sSName.IRadioStationName[STRead_JoinBuf[6]-1][0] = STRead_count;
							//1 preset = completed state data
							command_ok = TRUE;
							STRead_mode = STRX232_SYNC;
							STRead_count = 0;
						}else if (STRead_JoinBuf[5] == 0x05) {
							//FM (0x05)
							STRead_count = STRead_JoinBuf[2] - 6 - 1;
							//(STATIONNAME_MAX_SIZE=40) 20090617
							if (STRead_count >= STATIONNAME_MAX_SIZE ) {
								STRead_count = STATIONNAME_MAX_SIZE-1;
								for (i=0;i<STRead_count;i++) uStingray.sSName.FMStationName[STRead_JoinBuf[6]-1][i+1] = STRead_JoinBuf[7+i];
							}else{
								for (i=0;i<STRead_count;i++) uStingray.sSName.FMStationName[STRead_JoinBuf[6]-1][i+1] = STRead_JoinBuf[7+i];
								for (i=STRead_count;i<(STATIONNAME_MAX_SIZE-1);i++) uStingray.sSName.FMStationName[STRead_JoinBuf[6]-1][i+1] = 0x20;
							}
							//size 
							uStingray.sSName.FMStationName[STRead_JoinBuf[6]-1][0] = STRead_count;
							//completed state data
							command_ok = TRUE;
							STRead_mode = STRX232_SYNC;
							STRead_count = 0;
						}else{
							//DAB (0x??)
							STRead_count = STRead_JoinBuf[2] - 6 - 1;
							//(STATIONNAME_MAX_SIZE=40) 20090617
							if (STRead_count >= STATIONNAME_MAX_SIZE ) {
								STRead_count = STATIONNAME_MAX_SIZE-1;		
								for (i=0;i<STRead_count;i++) uStingray.sSName.DABStationName[STRead_JoinBuf[6]-1][i+1] = STRead_JoinBuf[7+i];
							}else{
								for (i=0;i<STRead_count;i++) uStingray.sSName.DABStationName[STRead_JoinBuf[6]-1][i+1] = STRead_JoinBuf[7+i];
								for (i=STRead_count;i<(STATIONNAME_MAX_SIZE-1);i++) uStingray.sSName.DABStationName[STRead_JoinBuf[6]-1][i+1] = 0x20;
							}
							//size
							uStingray.sSName.DABStationName[STRead_JoinBuf[6]-1][0] = STRead_count;
							//completed state data
							command_ok = TRUE;
							STRead_mode = STRX232_SYNC;
							STRead_count = 0;
						}
						makeSortPresetListTable();			//20100210
#ifdef UART1_DEBUG
		rs232_printf("Station\r",blank);
//change to ASCII code
		for (i=0;i<(STRead_JoinBuf[2]+1);i++) {
			PutChar(tbl_ASCii[((STRead_JoinBuf[i]>>4) & 0x0f)]);
			PutChar(tbl_ASCii[(STRead_JoinBuf[i] & 0x0f)]);
		}
		rs232_printf("\r",blank);
#endif
						break;
/*------------------------------------------------------------------*/
//Screen(0x82)
					case STRX232_SCREENSN:
						//read count, max count
						STRead_count = 5;
						count_max = 0;
						if (STRead_JoinBuf[1] > 0) {
							count_max = (int)STRead_JoinBuf[1]*256;
						}
						count_max += (int)STRead_JoinBuf[2];
#if 0 //def 0
				rs232_printf(" ct= %x\r",count_max);
#endif
						//buffer clear
						for (i=0;i<2;i++) Stingray_ACK_ScreenIconsBuf[i] = 0x00;  
						for (i=0;i<4;i++) Stingray_ACK_ScreenCursorPosBuf[i] = 0x00;  
						for (i=0;i<2;i++) Stingray_ACK_ScreenStateBuf[i] = 0x00;  						//20100129
						for (i=0;i<STATUS_SIZE;i++) Stingray_ACK_ScreenLine0Buf[i] = 0x20;  
						for (i=0;i<STATUS_SIZE;i++) Stingray_ACK_ScreenLine1Buf[i] = 0x20;  
						for (i=0;i<STATUS_SIZE;i++) Stingray_ACK_ScreenLine2Buf[i] = 0x20;  
						for (i=0;i<STATUS_SIZE;i++) Stingray_ACK_ScreenLine3Buf[i] = 0x20;  
						
repeat_ScreenRead:
						if (STRead_JoinBuf[STRead_count] == 0x00){										//? Icons(2bytes)
							Stingray_ACK_ScreenIconsBuf[0] = STRead_JoinBuf[STRead_count+2];			//IconStatus
							Stingray_ACK_ScreenIconsBuf[1] = STRead_JoinBuf[STRead_count+3];			//SignalStrength
							STRead_count += 4;
						}else if (STRead_JoinBuf[STRead_count] == 0x02){								//? CursorPosition(3bytes)
							Stingray_ACK_ScreenCursorPosBuf[0] = STRead_JoinBuf[STRead_count+2];		//CursorOn
							Stingray_ACK_ScreenCursorPosBuf[1] = STRead_JoinBuf[STRead_count+3];		//XPosition
							Stingray_ACK_ScreenCursorPosBuf[2] = STRead_JoinBuf[STRead_count+4];		//YPosition
							STRead_count += 5;
						}else if (STRead_JoinBuf[STRead_count] == 0x03){								//? CursorWidth(1byte)
							Stingray_ACK_ScreenCursorPosBuf[3] = STRead_JoinBuf[STRead_count+2];		//CursorWidth
							STRead_count += 3;
//20100129 : ScreenState
						}else if (STRead_JoinBuf[STRead_count] == 0x04){								//? ScreenState(2bytes)
							Stingray_ACK_ScreenStateBuf[0] = STRead_JoinBuf[STRead_count+2];			//State
							Stingray_ACK_ScreenStateBuf[1] = STRead_JoinBuf[STRead_count+3];			//ExtraInfo
							STRead_count += 4;
//
						}else if (STRead_JoinBuf[STRead_count] == 0x01){								//?Lines
							if (STRead_JoinBuf[STRead_count+2] == 0) {				//? 0 line
								for (i=0;i<STRead_JoinBuf[STRead_count+1];i++) Stingray_ACK_ScreenLine0Buf[i] = STRead_JoinBuf[STRead_count+i];
								if (STRead_JoinBuf[STRead_count+1] == 0) goto UARTRead_Err1;
								STRead_count += STRead_JoinBuf[STRead_count+1];
							}else if (STRead_JoinBuf[STRead_count+2] == 1) {		//? 1 line
								for (i=0;i<STRead_JoinBuf[STRead_count+1];i++) Stingray_ACK_ScreenLine1Buf[i] = STRead_JoinBuf[STRead_count+i];
								if (STRead_JoinBuf[STRead_count+1] == 0) goto UARTRead_Err1;
								STRead_count += STRead_JoinBuf[STRead_count+1];
							}else if (STRead_JoinBuf[STRead_count+2] == 2) {		//? 2 line
								for (i=0;i<STRead_JoinBuf[STRead_count+1];i++) Stingray_ACK_ScreenLine2Buf[i] = STRead_JoinBuf[STRead_count+i];
								if (STRead_JoinBuf[STRead_count+1] == 0) goto UARTRead_Err1;
								STRead_count += STRead_JoinBuf[STRead_count+1];
							}else if (STRead_JoinBuf[STRead_count+2] == 3) {		//? 3 line
								for (i=0;i<STRead_JoinBuf[STRead_count+1];i++) Stingray_ACK_ScreenLine3Buf[i] = STRead_JoinBuf[STRead_count+i];
								if (STRead_JoinBuf[STRead_count+1] == 0) goto UARTRead_Err1;
								STRead_count += STRead_JoinBuf[STRead_count+1];
							}
						}else{
							//data error!!!!
UARTRead_Err1:
							command_ok = FALSE;
							STRead_mode = STRX232_SYNC;
							STRead_count = 0;
							break;
						}
						if (STRead_count >= count_max) 
						{
							command_ok = TRUE;
							STRead_mode = STRX232_SYNC;
							STRead_count = 0;
//chd test : 20100430						//
//#ifdef UART1_SCREEN_DEBUG
#ifdef LINE1_UTF8_DEBUG
//Line1 utf8 data check
		for (i=0;i<Stingray_ACK_ScreenLine0Buf[1];i++) rs232_printf("%x",Stingray_ACK_ScreenLine0Buf[i]);
				PutChar('\r');
#endif
#ifdef UART1_SCREEN_DEBUG
		for (i=0;i<Stingray_ACK_ScreenLine0Buf[1];i++) PutChar(Stingray_ACK_ScreenLine0Buf[i]);
				PutChar('\r');
		for (i=0;i<Stingray_ACK_ScreenLine1Buf[1];i++) PutChar(Stingray_ACK_ScreenLine1Buf[i]);
				PutChar('\r');
		for (i=0;i<Stingray_ACK_ScreenLine2Buf[1];i++) PutChar(Stingray_ACK_ScreenLine2Buf[i]);
				PutChar('\r');
		for (i=0;i<Stingray_ACK_ScreenLine3Buf[1];i++) PutChar(Stingray_ACK_ScreenLine3Buf[i]);
				PutChar('\r');
#endif
							//insert VFD display 		
							if(VFD_scanStop == 0)		//20101214 : flicker VFD
							{
								vfd_processing();
							}else{
								VFD_Bk(0);
							}				
						}else{
								goto repeat_ScreenRead;
						}
						break;
					default:
						STRead_mode = STRX232_SYNC;
						break;
            }
         }else{
         	//checksum error!!!!
				command_ok = FALSE;
         }
	}
	return command_ok;
}        



/*-----------------------------------------------------------------------------
 Function Name  : Stingray Screen compare
 Description    : call 10msec
 Input          : None
 Output         : None
 Return         : None
 Comments	 	: ..
-----------------------------------------------------------------------------*/
void Stingray_Normal(void)
{
	unsigned char i,j;

//? f_ACK_Key_OK
	if (Stingray_Startup_mode == S_CONFIGURE_OK){
//
		if (f_ACK_Key_OK == ON) {
			f_ACK_Key_OK = OFF;
			if ((Stingray_ACK_mode & 0x80) == 0x80) {
				if (Stingray_ACK_mode == 0x80) {									//? ACK(0x80)
					if (Stingray_Tx_mode == Stingray_ACK_mode_ID) {
						if (Stingray_Tx_mode_SN == Stingray_ACK_mode_SN) {
							if (Stingray_ACK_status == 0x01){
								//completed!!!
/*								//check key pressed
								if (Stingray_ACK_mode_ID == CMD_KeyPressed) {
									//Stingray_Key_Value == (UP/DOWN/POWER key) 이면 skip!!!
									if ((Stingray_Key_Value == STM_ENC_U) || (Stingray_Key_Value == STM_ENC_D) || (Stingray_Key_Value == STM_POWER)) {
										Stingray_Key_Value = 0;
										Stingray_Key_OverTime = 0;
									}else{
										Stingray_Tx_KeyReleased(Stingray_Key_Value);
									}
								}
								if (Stingray_ACK_mode_ID == CMD_KeyReleased) {
									Stingray_Key_Value = 0;
								}
*/									
							}else{
								//not completed!!!
							}
						}
					}
				}else{
					if (Stingray_Tx_mode == (Stingray_ACK_mode & 0x7f)) {
						if (Stingray_Tx_mode_SN == Stingray_ACK_mode_SN) {
							if (Stingray_ACK_status == 0x01){
								//completed!!!
							}else{
								//not completed!!!
							}
						}
					}
				}
			}
		}
	}
/*	//key release time over check (50msec)
	if (Stingray_Key_OverTime > 0 ) {
		if ((--Stingray_Key_OverTime) == 0 ) {
			if (Stingray_Key_Value != 0 ) {
				Stingray_Key_Value = 0;
			}
		}
	}
*/
}
/*-----------------------------------------------------------------------------
 Function Name  : Stingray Screen compare
 Description    : call 10msec
 Input          : None
 Output         : None
 Return         : None
 Comments	 	: ..

	byte ACK_mode;
	byte ACK_mode_SN;
	byte ACK_status;
	byte ACK_ScreenIconsBuf[4];
	byte ACK_ScreenLine0Buf[STATUS_SIZE];		//line1
	byte ACK_ScreenLine1Buf[STATUS_SIZE];		//line2
	byte ACK_ScreenLine2Buf[STATUS_SIZE];		//line3
	byte ACK_ScreenLine3Buf[STATUS_SIZE];		//line4
	byte ACK_ScreenCursorPosBuf[5];
[UTF-8] :	
	1byte [0x00000000 ~ 0x0000007f	: 0xxx xxxx]
	2byte [0x00000080 ~ 0x000007ff 	: 110x xxyy 10yy yyyy ]
	3byte [0x00000800 ~ 0x0000ffff 	: 1110 xxxx 10xx xxyy 10yy yyyy]
	4byte [0x00010000 ~ 0x001fffff 	: 1111 0zzz 10zz xxxx 10xx xxyy 10yy yyyy]
//?		
	5byte [0x00200000 ~ 0x03ffffff 	: 1111 10xx 10xx xxxx 10xx xxxx 10xx xxyy 10yy yyyy]
	6byte [0x04000000 ~ 0x7fffffff 	: 1111 110x 10xx xxxx 10xx xxxx 10xx xxxx 10xx xxyy 10yy yyyy]
//?		
	not occurs code positions   : U+D800~D+DFFF , U+FFFE , U+FFFF 	
		
//first byte check!!!!!
	<6 bytes> *** & 0xFE == 0xFC
	<5 bytes> *** & 0xFC == 0xF8
	<4 bytes> *** & 0xF8 == 0xF0
	<3 bytes> *** & 0xF0 == 0xE0
	<2 bytes> *** & 0xE0 == 0xC0
			
	in : num[0]num[1]	(MSB --- LSB)
	out: num[1]num[0]	(MSB --- LSB) [0x80~0x7ff]
-----------------------------------------------------------------------------*/
unsigned short make_UTF8Unicode2(unsigned char* num)
{
/*
	unsigned char z,x,y;
	unsigned char zz,xx,yy;
	
		y = num[1];
		x = num[0];
		zz = ((x<<6)&0xc0) | (y&0x3f);	//LSB
		x = num[0];
		num[1] = (x>>2)&0x07;				//MSB
		num[0] = zz;							//LSB
*/	
	unsigned short unicode, unicode1;
	unsigned short p,r;

	p=num[0]%32;		//상위 5bit
	r=num[1]%64;		//하위 6bit
	
	unicode = p;							//---- ---- ---p pppp
	unicode = unicode <<6;		//---- -ppp pp-- ----
	unicode = unicode | r;		//---- ---- --rr rrrr
	return(unicode);					//little-endian(current)
	//unicode1 = unicode;
	//return( ((unicode>>8)&0x00ff)|((unicode1<<8)&0xff00) );				//big-endian
}
//
const byte UnicodeTableE0xx[] = 
{
//	'E','N','D','*','*','*','*','*','<','>' };
	'E','N','D','*','*','*','*','*',0x96,0x95 };		//'<-','->'	//20091228
/*
[UTF-8] :	
	1byte [0x00000000 ~ 0x0000007f	: 0xxx xxxx]
	2byte [0x00000080 ~ 0x000007ff 	: 110x xxyy 10yy yyyy ]
	3byte [0x00000800 ~ 0x0000ffff 	: 1110 xxxx 10xx xxyy 10yy yyyy]
	4byte [0x00010000 ~ 0x001fffff 	: 1111 0zzz 10zz xxxx 10xx xxyy 10yy yyyy]
//?		
	5byte [0x00200000 ~ 0x03ffffff 	: 1111 10xx 10xx xxxx 10xx xxxx 10xx xxyy 10yy yyyy]
	6byte [0x04000000 ~ 0x7fffffff 	: 1111 110x 10xx xxxx 10xx xxxx 10xx xxxx 10xx xxyy 10yy yyyy]
		
	in : num[0]num[1]num[2]	(MSB --- LSB)
	out: num[1]num[0]		(MSB --- LSB) [0x800~0xffff]
*********************************************************************************/
unsigned short make_UTF8Unicode3(unsigned char* num)
{
/*
	unsigned char z,x,y;
	unsigned char zz,xx,yy;

		y = num[2];
		x = num[1];
		z = num[0];
		zz = ((x<<6)&0xc0) | (y&0x3f);			//LSB
		x = num[1];
		num[1] = ((z<<4)&0xf0) | ((x>>2)&0x0f);	//MSB
		num[0] = zz;									//LSB
		num[2] = 0;
*/
	unsigned short unicode, unicode1;
	unsigned short p,q,r;

	p=num[0]%16;		//상위 4bit
	q=num[1]%64;		//중간 6bit
	r=num[2]%64;		//하위 6bit
	
	unicode = p;								//---- ---- ---- pppp
	unicode = unicode <<12;			//pppp ---- ---- ----
	unicode = unicode |(q<<6);	//---- qqqq qq-- ----
	unicode = unicode | r;			//---- ---- --rr rrrr
	return(unicode);						//little-endian(current)
	//unicode1 = unicode;
	//return( ((unicode>>8)&0x00ff)|((unicode1<<8)&0xff00) );				//big-endian
}
unsigned char make_Unicode3(unsigned char* num)
{
	unsigned char z,x,y;
	unsigned char zz,xx,yy;

		y = num[2];
		x = num[1];
		z = num[0];
		zz = ((x<<6)&0xc0) | (y&0x3f);			//LSB
		x = num[1];
		num[1] = ((z<<4)&0xf0) | ((x>>2)&0x0f);	//MSB
		num[0] = zz;									//LSB
		num[2] = 0;

		if(num[1] == 0xe0) {
			if (num[0] < 0x0a) {
				xx = (unsigned char)UnicodeTableE0xx[num[0]];
				return(xx);
			}else{
				return(IRREGULAR_DATA);
			}
		}else{
				return(IRREGULAR_DATA);
		}
}
/*
[UTF-8] :	
	1byte [0x00000000 ~ 0x0000007f	: 0xxx xxxx]
	2byte [0x00000080 ~ 0x000007ff 	: 110x xxyy 10yy yyyy ]
	3byte [0x00000800 ~ 0x0000ffff 	: 1110 xxxx 10xx xxyy 10yy yyyy]
	4byte [0x00010000 ~ 0x001fffff 	: 1111 0zzz 10zz xxxx 10xx xxyy 10yy yyyy]
//?		
	5byte [0x00200000 ~ 0x03ffffff 	: 1111 10xx 10xx xxxx 10xx xxxx 10xx xxyy 10yy yyyy]
	6byte [0x04000000 ~ 0x7fffffff 	: 1111 110x 10xx xxxx 10xx xxxx 10xx xxxx 10xx xxyy 10yy yyyy]
		
	in : num[0]num[1]num[2]num[3]	(MSB --- LSB)
	out: num[2]num[1]num[0]			(MSB --- LSB) [0x10000~0x1fffff]
*********************************************************************************/
void make_Unicode4(unsigned char* num)
{
	unsigned char z,x,y;
	unsigned char zz,xx,yy;
		y = num[3];
		x = num[2];
		z = num[1];
		zz = ((x<<6)&0xc0) | (y&0x3f);				//LSB
		x = num[2];
		xx = ((z<<4)&0xf0) | ((x>>2)&0x0f);			//m
		z = num[1];
		y = num[0];
		num[2] = ((y<<2)&0x1c) | ((z>>4)&0x03);	//MSB
		num[1] = xx;									//m
		num[0] = zz;									//LSB
		num[3] = 0;
}
/******************************************************************************/
void Screen_Compare(void)
{
	unsigned char i,j,k;
	unsigned char num[4];
	short unicode;
	
//line0
	i = 0;
	for(j=1;j<19;j++) {
		//20100830
		if ((Stingray_ACK_ScreenLine0Buf[6+i] & 0xfe) == 0xfc) {
			//6byte
#ifdef STINGRAY_UTF8_4321
			uScreenUTF8Reading.sUtf8Stingray.iDispBuf[0][j] = Unicode_IRREGULAR_DATA;	//int(2Byte)
#endif
			uScreenReading.sStingray.DispBuf[0][j] = IRREGULAR_DATA;
			i += 6;
		}else if ((Stingray_ACK_ScreenLine0Buf[6+i] & 0xfc) == 0xf8) {
			//5byte
#ifdef STINGRAY_UTF8_4321
			uScreenUTF8Reading.sUtf8Stingray.iDispBuf[0][j] = Unicode_IRREGULAR_DATA;	//int(2Byte)
#endif
			uScreenReading.sStingray.DispBuf[0][j] = IRREGULAR_DATA;
			i += 5;
		}else if ((Stingray_ACK_ScreenLine0Buf[6+i] & 0xf8) == 0xf0) {
			//4byte
#ifdef STINGRAY_UTF8_4321
			//for(k=0;k<4;k++) num[k]= Stingray_ACK_ScreenLine0Buf[6+i+k];
			//make_Unicode4(num);
			//uScreenUTF8Reading.sUtf8Stingray.iDispBuf[0][j] = num[1];			//int(2Byte)
			uScreenUTF8Reading.sUtf8Stingray.iDispBuf[0][j] = Unicode_IRREGULAR_DATA;	//int(2Byte)
#endif
			uScreenReading.sStingray.DispBuf[0][j] = IRREGULAR_DATA;
			i += 4;
		}else if ((Stingray_ACK_ScreenLine0Buf[6+i] & 0xf0) == 0xe0) {			
			//3byte
#ifdef STINGRAY_UTF8_4321
			for(k=0;k<3;k++) num[k]= Stingray_ACK_ScreenLine0Buf[6+i+k];
			uScreenUTF8Reading.sUtf8Stingray.iDispBuf[0][j] = make_UTF8Unicode3(num);		//(short)
#endif
			for(k=0;k<3;k++) num[k]= Stingray_ACK_ScreenLine0Buf[6+i+k];
			uScreenReading.sStingray.DispBuf[0][j] = make_Unicode3(num);		//(1byte)
			i += 3;
		}else if ((Stingray_ACK_ScreenLine0Buf[6+i] & 0xe0) == 0xc0) {			
			//2byte
#ifdef STINGRAY_UTF8_4321
			for(k=0;k<2;k++) num[k]= Stingray_ACK_ScreenLine0Buf[6+i+k];
			uScreenUTF8Reading.sUtf8Stingray.iDispBuf[0][j] = make_UTF8Unicode2(num);		//(short)
#endif
			uScreenReading.sStingray.DispBuf[0][j] = IRREGULAR_DATA;
			i += 2;
		}else{
			//1byte [ 0000 ~ 007f ]
#ifdef STINGRAY_UTF8_4321
			unicode = Stingray_ACK_ScreenLine0Buf[6+i];						//LSB
			uScreenUTF8Reading.sUtf8Stingray.iDispBuf[0][j] = unicode;		//(2Byte)
#endif
			//1byte [ 20 ~ 7f ]
			if (Stingray_ACK_ScreenLine0Buf[6+i] < 0x20)			//20100617
			{
				uScreenReading.sStingray.DispBuf[0][j] = ' ';
			}else{
				uScreenReading.sStingray.DispBuf[0][j] = Stingray_ACK_ScreenLine0Buf[6+i];
			}
			i += 1;
		}
	}
	//Arrows check
	if (Stingray_ACK_ScreenLine0Buf[5] == 0x03) {
#ifdef STINGRAY_UTF8_4321
		uScreenUTF8Reading.sUtf8Stingray.iDispBuf[0][0] = '<';		//int(2Byte)
		uScreenUTF8Reading.sUtf8Stingray.iDispBuf[0][19] = '>';		//int(2Byte)
#endif
		uScreenReading.sStingray.DispBuf[0][0] = '<';
		uScreenReading.sStingray.DispBuf[0][19] = '>';
	}else if (Stingray_ACK_ScreenLine0Buf[5] == 0x02) {
#ifdef STINGRAY_UTF8_4321
		uScreenUTF8Reading.sUtf8Stingray.iDispBuf[0][0] = ' ';		//int(2Byte)
		uScreenUTF8Reading.sUtf8Stingray.iDispBuf[0][19] = '>';		//int(2Byte)
#endif
		uScreenReading.sStingray.DispBuf[0][0] = ' ';
		uScreenReading.sStingray.DispBuf[0][19] = '>';
	}else if (Stingray_ACK_ScreenLine0Buf[5] == 0x01) {
#ifdef STINGRAY_UTF8_4321
		uScreenUTF8Reading.sUtf8Stingray.iDispBuf[0][0] = '<';		//int(2Byte)
		uScreenUTF8Reading.sUtf8Stingray.iDispBuf[0][19] = ' ';		//int(2Byte)
#endif
		uScreenReading.sStingray.DispBuf[0][0] = '<';
		uScreenReading.sStingray.DispBuf[0][19] = ' ';
	}else{
#ifdef STINGRAY_UTF8_4321
		uScreenUTF8Reading.sUtf8Stingray.iDispBuf[0][0] = ' ';			//int(2Byte)
		uScreenUTF8Reading.sUtf8Stingray.iDispBuf[0][19] = ' ';			//int(2Byte)
#endif
		uScreenReading.sStingray.DispBuf[0][0] = ' ';
		uScreenReading.sStingray.DispBuf[0][19] = ' ';
	}
	//Invert
#ifdef LINE1_UTF8_DEBUG
//Line1 utf8 data check
		for (i=0;i<20;i++) rs232_printf("%x",uScreenUTF8Reading.sUtf8Stingray.iDispBuf[0][i]);
				PutChar('\r');
		for (i=0;i<20;i++) rs232_printf("%x",uScreenReading.sStingray.DispBuf[0][i]);
				PutChar('\r');
#endif

//line1
	i = 0;
	for(j=1;j<19;j++) {
		//20100830
		if ((Stingray_ACK_ScreenLine1Buf[6+i] & 0xfe) == 0xfc) {
			//6byte
#ifdef STINGRAY_UTF8_4321
			uScreenUTF8Reading.sUtf8Stingray.iDispBuf[1][j] = Unicode_IRREGULAR_DATA;	//int(2Byte)
#endif
			uScreenReading.sStingray.DispBuf[1][j] = IRREGULAR_DATA;
			i += 6;
		}else if ((Stingray_ACK_ScreenLine1Buf[6+i] & 0xfc) == 0xf8) {
			//5byte
#ifdef STINGRAY_UTF8_4321
			uScreenUTF8Reading.sUtf8Stingray.iDispBuf[1][j] = Unicode_IRREGULAR_DATA;	//int(2Byte)
#endif
			uScreenReading.sStingray.DispBuf[1][j] = IRREGULAR_DATA;
			i += 5;
		}else if ((Stingray_ACK_ScreenLine1Buf[6+i] & 0xf8) == 0xf0) {
			//4byte
#ifdef STINGRAY_UTF8_4321
			//for(k=0;k<4;k++) num[k]= Stingray_ACK_ScreenLine1Buf[6+i+k];
			//make_Unicode4(num);
			//uScreenUTF8Reading.sUtf8Stingray.iDispBuf[1][j] = num[0];			//int(2Byte)
			uScreenUTF8Reading.sUtf8Stingray.iDispBuf[1][j] = Unicode_IRREGULAR_DATA;	//int(2Byte)
#endif
			uScreenReading.sStingray.DispBuf[1][j] = IRREGULAR_DATA;
			i += 4;
		}else if ((Stingray_ACK_ScreenLine1Buf[6+i] & 0xf0) == 0xe0) {			
			//3byte
#ifdef STINGRAY_UTF8_4321
			for(k=0;k<3;k++) num[k]= Stingray_ACK_ScreenLine1Buf[6+i+k];
			uScreenUTF8Reading.sUtf8Stingray.iDispBuf[1][j] = make_UTF8Unicode3(num);		//(short)
#endif
			for(k=0;k<3;k++) num[k]= Stingray_ACK_ScreenLine1Buf[6+i+k];
			uScreenReading.sStingray.DispBuf[1][j] = make_Unicode3(num);
			i += 3;
		}else if ((Stingray_ACK_ScreenLine1Buf[6+i] & 0xe0) == 0xc0) {			
			//2byte
#ifdef STINGRAY_UTF8_4321
			for(k=0;k<2;k++) num[k]= Stingray_ACK_ScreenLine1Buf[6+i+k];
			uScreenUTF8Reading.sUtf8Stingray.iDispBuf[1][j] = make_UTF8Unicode2(num);		//(short)
#endif
			uScreenReading.sStingray.DispBuf[1][j] = IRREGULAR_DATA;
			i += 2;
		}else{
			//1byte
#ifdef STINGRAY_UTF8_4321
			unicode = Stingray_ACK_ScreenLine1Buf[6+i];						//LSB
			uScreenUTF8Reading.sUtf8Stingray.iDispBuf[1][j] = unicode;		//int(2Byte)
#endif
			//1byte [ 20 ~ 7f ]
			if (Stingray_ACK_ScreenLine1Buf[6+i] < 0x20)			//20100617
			{
				uScreenReading.sStingray.DispBuf[1][j] = ' ';
			}else{
				uScreenReading.sStingray.DispBuf[1][j] = Stingray_ACK_ScreenLine1Buf[6+i];
			}
			i += 1;
		}
	}
	//Arrows check
	if (Stingray_ACK_ScreenLine1Buf[5] == 0x03) {
#ifdef STINGRAY_UTF8_4321
		uScreenUTF8Reading.sUtf8Stingray.iDispBuf[1][0] = '<';		//int(2Byte)
		uScreenUTF8Reading.sUtf8Stingray.iDispBuf[1][19] = '>';		//int(2Byte)
#endif
		uScreenReading.sStingray.DispBuf[1][0] = '<';
		uScreenReading.sStingray.DispBuf[1][19] = '>';
	}else if (Stingray_ACK_ScreenLine1Buf[5] == 0x02) {
#ifdef STINGRAY_UTF8_4321
		uScreenUTF8Reading.sUtf8Stingray.iDispBuf[1][0] = ' ';		//int(2Byte)
		uScreenUTF8Reading.sUtf8Stingray.iDispBuf[1][19] = '>';		//int(2Byte)
#endif
		uScreenReading.sStingray.DispBuf[1][0] = ' ';
		uScreenReading.sStingray.DispBuf[1][19] = '>';
	}else if (Stingray_ACK_ScreenLine1Buf[5] == 0x01) {
#ifdef STINGRAY_UTF8_4321
		uScreenUTF8Reading.sUtf8Stingray.iDispBuf[1][0] = '<';		//int(2Byte)
		uScreenUTF8Reading.sUtf8Stingray.iDispBuf[1][19] = ' ';		//int(2Byte)
#endif
		uScreenReading.sStingray.DispBuf[1][0] = '<';
		uScreenReading.sStingray.DispBuf[1][19] = ' ';
	}else{
#ifdef STINGRAY_UTF8_4321
		uScreenUTF8Reading.sUtf8Stingray.iDispBuf[1][0] = ' ';			//int(2Byte)
		uScreenUTF8Reading.sUtf8Stingray.iDispBuf[1][19] = ' ';			//int(2Byte)
#endif
		uScreenReading.sStingray.DispBuf[1][0] = ' ';
		uScreenReading.sStingray.DispBuf[1][19] = ' ';
	}
	//Invert
/*
	//cursor : <YES><NO>
	//Cursoron,Xpos(column),Ypos(line),Width
	if(Stingray_ACK_ScreenCursorPosBuf[0] == ON) {			//? cursor on
		if (Stingray_ACK_ScreenCursorPosBuf[2] == 0x01) {	//? 1 Line
			i = Stingray_ACK_ScreenCursorPosBuf[1];			// Xposition(column)
			uScreenReading.sStingray.DispBuf[1][i] = '>';
//			for(j=i;j<19;j++) {
//				if (uScreenReading.sStingray.DispBuf[1][j] == 0x20) {
//					uScreenReading.sStingray.DispBuf[1][j] = '>';
//					break;
//				}
//			}
		}
	}
*/	
//line2
	i = 0;
	for(j=1;j<19;j++) {
		//20100830
		if ((Stingray_ACK_ScreenLine2Buf[6+i] & 0xfe) == 0xfc) {
			//6byte
#ifdef STINGRAY_UTF8_4321
			uScreenUTF8Reading.sUtf8Stingray.iDispBuf[2][j] = Unicode_IRREGULAR_DATA;	//int(2Byte)
#endif
			uScreenReading.sStingray.DispBuf[2][j] = IRREGULAR_DATA;
			i += 6;
		}else if ((Stingray_ACK_ScreenLine2Buf[6+i] & 0xfc) == 0xf8) {
			//5byte
#ifdef STINGRAY_UTF8_4321
			uScreenUTF8Reading.sUtf8Stingray.iDispBuf[2][j] = Unicode_IRREGULAR_DATA;	//int(2Byte)
#endif
			uScreenReading.sStingray.DispBuf[2][j] = IRREGULAR_DATA;
			i += 5;
		}else if ((Stingray_ACK_ScreenLine2Buf[6+i] & 0xf8) == 0xf0) {
			//4byte
#ifdef STINGRAY_UTF8_4321
			//for(k=0;k<4;k++) num[k]= Stingray_ACK_ScreenLine2Buf[6+i+k];
			//make_Unicode4(num);
			//uScreenUTF8Reading.sUtf8Stingray.iDispBuf[2][j] = num[0];			//int(2Byte)
			uScreenUTF8Reading.sUtf8Stingray.iDispBuf[2][j] = Unicode_IRREGULAR_DATA;	//int(2Byte)
#endif
			uScreenReading.sStingray.DispBuf[2][j] = IRREGULAR_DATA;
			i += 4;
		}else if ((Stingray_ACK_ScreenLine2Buf[6+i] & 0xf0) == 0xe0) {			
			//3byte
#ifdef STINGRAY_UTF8_4321
			for(k=0;k<3;k++) num[k]= Stingray_ACK_ScreenLine2Buf[6+i+k];
			uScreenUTF8Reading.sUtf8Stingray.iDispBuf[2][j] = make_UTF8Unicode3(num);		//(short)
#endif
			for(k=0;k<3;k++) num[k]= Stingray_ACK_ScreenLine2Buf[6+i+k];
			uScreenReading.sStingray.DispBuf[2][j] = make_Unicode3(num);
			i += 3;
		}else if ((Stingray_ACK_ScreenLine2Buf[6+i] & 0xe0) == 0xc0) {			
			//2byte
#ifdef STINGRAY_UTF8_4321
			for(k=0;k<2;k++) num[k]= Stingray_ACK_ScreenLine2Buf[6+i+k];
			uScreenUTF8Reading.sUtf8Stingray.iDispBuf[2][j] = make_UTF8Unicode2(num);		//(short)
#endif
			uScreenReading.sStingray.DispBuf[2][j] = IRREGULAR_DATA;
			i += 2;
		}else{
			//1byte
#ifdef STINGRAY_UTF8_4321
			unicode = Stingray_ACK_ScreenLine2Buf[6+i];						//LSB
			uScreenUTF8Reading.sUtf8Stingray.iDispBuf[2][j] = unicode;		//int(2Byte)
#endif
			//1byte [ 20 ~ 7f ]
			if (Stingray_ACK_ScreenLine2Buf[6+i] < 0x20)			//20100617
			{
				uScreenReading.sStingray.DispBuf[2][j] = ' ';
			}else{
				uScreenReading.sStingray.DispBuf[2][j] = Stingray_ACK_ScreenLine2Buf[6+i];
			}
			i += 1;
		}
	}
	//Arrows check
	if (Stingray_ACK_ScreenLine2Buf[5] == 0x03) {
#ifdef STINGRAY_UTF8_4321
		uScreenUTF8Reading.sUtf8Stingray.iDispBuf[2][0] = '<';		//int(2Byte)
		uScreenUTF8Reading.sUtf8Stingray.iDispBuf[2][19] = '>';		//int(2Byte)
#endif
		uScreenReading.sStingray.DispBuf[2][0] = '<';
		uScreenReading.sStingray.DispBuf[2][19] = '>';
	}else if (Stingray_ACK_ScreenLine2Buf[5] == 0x02) {
#ifdef STINGRAY_UTF8_4321
		uScreenUTF8Reading.sUtf8Stingray.iDispBuf[2][0] =  ' ';		//int(2Byte)
		uScreenUTF8Reading.sUtf8Stingray.iDispBuf[2][19] = '>';		//int(2Byte)
#endif
		uScreenReading.sStingray.DispBuf[2][0] = ' ';
		uScreenReading.sStingray.DispBuf[2][19] = '>';
	}else if (Stingray_ACK_ScreenLine2Buf[5] == 0x01) {
#ifdef STINGRAY_UTF8_4321
		uScreenUTF8Reading.sUtf8Stingray.iDispBuf[2][0] = '<';		//int(2Byte)
		uScreenUTF8Reading.sUtf8Stingray.iDispBuf[2][19] = ' ';		//int(2Byte)
#endif
		uScreenReading.sStingray.DispBuf[2][0] = '<';
		uScreenReading.sStingray.DispBuf[2][19] = ' ';
	}else{
#ifdef STINGRAY_UTF8_4321
		uScreenUTF8Reading.sUtf8Stingray.iDispBuf[2][0] = ' ';			//int(2Byte)
		uScreenUTF8Reading.sUtf8Stingray.iDispBuf[2][19] = ' ';			//int(2Byte)
#endif
		uScreenReading.sStingray.DispBuf[2][0] = ' ';
		uScreenReading.sStingray.DispBuf[2][19] = ' ';
	}
	//Invert
//line3
	i = 0;
	for(j=1;j<19;j++) {
		//20100830
		if ((Stingray_ACK_ScreenLine3Buf[6+i] & 0xfe) == 0xfc) {
			//6byte
#ifdef STINGRAY_UTF8_4321
			uScreenUTF8Reading.sUtf8Stingray.iDispBuf[3][j] = Unicode_IRREGULAR_DATA;	//int(2Byte)
#endif
			uScreenReading.sStingray.DispBuf[3][j] = IRREGULAR_DATA;
			i += 6;
		}else if ((Stingray_ACK_ScreenLine3Buf[6+i] & 0xfc) == 0xf8) {
			//5byte
#ifdef STINGRAY_UTF8_4321
			uScreenUTF8Reading.sUtf8Stingray.iDispBuf[3][j] = Unicode_IRREGULAR_DATA;	//int(2Byte)
#endif
			uScreenReading.sStingray.DispBuf[3][j] = IRREGULAR_DATA;
			i += 5;
		}else if ((Stingray_ACK_ScreenLine3Buf[6+i] & 0xf8) == 0xf0) {
			//4byte
#ifdef STINGRAY_UTF8_4321
			//for(k=0;k<4;k++) num[k]= Stingray_ACK_ScreenLine3Buf[6+i+k];
			//make_Unicode4(num);
			//uScreenUTF8Reading.sUtf8Stingray.iDispBuf[3][j] = num[0];			//int(2Byte)
			uScreenUTF8Reading.sUtf8Stingray.iDispBuf[3][j] = Unicode_IRREGULAR_DATA;	//int(2Byte)
#endif
			uScreenReading.sStingray.DispBuf[3][j] = IRREGULAR_DATA;
			i += 4;
		}else if ((Stingray_ACK_ScreenLine3Buf[6+i] & 0xf0) == 0xe0) {			
			//3byte
#ifdef STINGRAY_UTF8_4321
			for(k=0;k<3;k++) num[k]= Stingray_ACK_ScreenLine3Buf[6+i+k];
			uScreenUTF8Reading.sUtf8Stingray.iDispBuf[3][j] = make_UTF8Unicode3(num);		//(short)
#endif
			for(k=0;k<3;k++) num[k]= Stingray_ACK_ScreenLine3Buf[6+i+k];
			uScreenReading.sStingray.DispBuf[3][j] = make_Unicode3(num);
			i += 3;
		}else if ((Stingray_ACK_ScreenLine3Buf[6+i] & 0xe0) == 0xc0) {			
			//2byte
#ifdef STINGRAY_UTF8_4321
			for(k=0;k<2;k++) num[k]= Stingray_ACK_ScreenLine3Buf[6+i+k];
			uScreenUTF8Reading.sUtf8Stingray.iDispBuf[3][j] = make_UTF8Unicode2(num);		//(short)
#endif
			uScreenReading.sStingray.DispBuf[3][j] = IRREGULAR_DATA;
			i += 2;
		}else{
			//1byte
#ifdef STINGRAY_UTF8_4321
			unicode = Stingray_ACK_ScreenLine3Buf[6+i];						//LSB
			uScreenUTF8Reading.sUtf8Stingray.iDispBuf[3][j] = unicode;		//int(2Byte)
#endif
			//1byte [ 20 ~ 7f ]
			if (Stingray_ACK_ScreenLine3Buf[6+i] < 0x20)			//20100617
			{
				uScreenReading.sStingray.DispBuf[3][j] = ' ';
			}else{
				uScreenReading.sStingray.DispBuf[3][j] = Stingray_ACK_ScreenLine3Buf[6+i];
			}
			i += 1;
		}
	}
	//Arrows check
	if (Stingray_ACK_ScreenLine3Buf[5] == 0x03) {
#ifdef STINGRAY_UTF8_4321
		uScreenUTF8Reading.sUtf8Stingray.iDispBuf[3][0] = '<';		//int(2Byte)
		uScreenUTF8Reading.sUtf8Stingray.iDispBuf[3][19] = '>';		//int(2Byte)
#endif
		uScreenReading.sStingray.DispBuf[3][0] = '<';
		uScreenReading.sStingray.DispBuf[3][19] = '>';
	}else if (Stingray_ACK_ScreenLine3Buf[5] == 0x02) {
#ifdef STINGRAY_UTF8_4321
		uScreenUTF8Reading.sUtf8Stingray.iDispBuf[3][0] = ' ';		//int(2Byte)
		uScreenUTF8Reading.sUtf8Stingray.iDispBuf[3][19] = '>';		//int(2Byte)
#endif
		uScreenReading.sStingray.DispBuf[3][0] = ' ';
		uScreenReading.sStingray.DispBuf[3][19] = '>';
	}else if (Stingray_ACK_ScreenLine3Buf[5] == 0x01) {
#ifdef STINGRAY_UTF8_4321
		uScreenUTF8Reading.sUtf8Stingray.iDispBuf[3][0] = '<';		//int(2Byte)
		uScreenUTF8Reading.sUtf8Stingray.iDispBuf[3][19] = ' ';		//int(2Byte)
#endif
		uScreenReading.sStingray.DispBuf[3][0] = '<';
		uScreenReading.sStingray.DispBuf[3][19] = ' ';
	}else{
#ifdef STINGRAY_UTF8_4321
		uScreenUTF8Reading.sUtf8Stingray.iDispBuf[3][0] = ' ';			//int(2Byte)
		uScreenUTF8Reading.sUtf8Stingray.iDispBuf[3][19] = ' ';			//int(2Byte)
#endif
		uScreenReading.sStingray.DispBuf[3][0] = ' ';
		uScreenReading.sStingray.DispBuf[3][19] = ' ';
	}
	//Invert
	//Cursoron,Xpos(column),Ypos(line),Width
#ifdef STINGRAY_UTF8_4321
	uScreenUTF8Reading.sUtf8Stingray.CursorXYPosition[0] = Stingray_ACK_ScreenCursorPosBuf[0];
	uScreenUTF8Reading.sUtf8Stingray.CursorXYPosition[1] = Stingray_ACK_ScreenCursorPosBuf[1];
	uScreenUTF8Reading.sUtf8Stingray.CursorXYPosition[2] = Stingray_ACK_ScreenCursorPosBuf[2];
	uScreenUTF8Reading.sUtf8Stingray.CursorXYPosition[3] = Stingray_ACK_ScreenCursorPosBuf[3];
#endif
	uScreenReading.sStingray.CursorXYPosition[0] = Stingray_ACK_ScreenCursorPosBuf[0];
	uScreenReading.sStingray.CursorXYPosition[1] = Stingray_ACK_ScreenCursorPosBuf[1];
	uScreenReading.sStingray.CursorXYPosition[2] = Stingray_ACK_ScreenCursorPosBuf[2];
	uScreenReading.sStingray.CursorXYPosition[3] = Stingray_ACK_ScreenCursorPosBuf[3];
	//20100129
	//ScreenState(2bytes: State,ExtraInfo)
#ifdef STINGRAY_UTF8_4321
	uScreenUTF8Reading.sUtf8Stingray.ScreenState[0] = Stingray_ACK_ScreenStateBuf[0];
	uScreenUTF8Reading.sUtf8Stingray.ScreenState[1] = Stingray_ACK_ScreenStateBuf[1];
#endif
	uScreenReading.sStingray.ScreenState[0] = Stingray_ACK_ScreenStateBuf[0];
	uScreenReading.sStingray.ScreenState[1] = Stingray_ACK_ScreenStateBuf[1];
	//buffer compare
//	f_Screen_Key_OK = ON;			//chd test
#ifdef STINGRAY_UTF8_4321
	f_Screen_Key_OK = OFF;
	if(memcmp((void*)uScreenUTF8.display_Buffer,(void*)uScreenUTF8Reading.display_Buffer,166) != 0){
		memcpy((void*)uScreenUTF8.display_Buffer,(void*)uScreenUTF8Reading.display_Buffer,166);
		f_Screen_Key_OK = ON;
//chd test : 20100511						//
#ifdef UART1_SCREENUTF8_DEBUG
		for (i=0;i<20;i++) PutChar(uScreenUTF8Reading.sUtf8Stingray.iDispBuf[0][i]);
				PutChar('\r');
		for (i=0;i<20;i++) PutChar(uScreenUTF8Reading.sUtf8Stingray.iDispBuf[1][i]);
				PutChar('\r');
		for (i=0;i<20;i++) PutChar(uScreenUTF8Reading.sUtf8Stingray.iDispBuf[2][i]);
				PutChar('\r');
		for (i=0;i<20;i++) PutChar(uScreenUTF8Reading.sUtf8Stingray.iDispBuf[3][i]);
				PutChar('\r');
#endif
	}
#endif
	f_Screen_Key_OK = OFF;
	if(memcmp((void*)uScreen.display_Buffer,(void*)uScreenReading.display_Buffer,DISPLAYBUFFER_MAX_SIZE) != 0){
		memcpy((void*)uScreen.display_Buffer,(void*)uScreenReading.display_Buffer,DISPLAYBUFFER_MAX_SIZE);
		f_Screen_Key_OK = ON;
	}
}
/*-----------------------------------------------------------------------------
 Function Name  : Stingray RS232 check
 Description    : call 10msec
 Input          : None
 Output         : screen_OK, f_ACK_Key_OK
 Return         : None
 Comments	 	: ..

ACK80 :		Stingray_ACK_mode = STRead_JoinBuf[3];			//ACK.command (0x80 or ID)
			Stingray_ACK_mode_ID = STRead_JoinBuf[5];		//ACK.command ID (0x0e)
			Stingray_ACK_mode_SN = STRead_JoinBuf[6];		//ACK.SN (sequence number)
			Stingray_ACK_status = STRead_JoinBuf[7];		//ACK.Status
Station name(0x8e) :
			Stingray_ACK_mode = STRead_JoinBuf[3];			//ACK.command ID (0x8e)
			Stingray_ACK_mode_ID = STRead_JoinBuf[3];		//ACK.command ID (0x8e)
			Stingray_ACK_mode_SN = STRead_JoinBuf[4];		//ACK.SN (sequence number)
-----------------------------------------------------------------------------*/
void Stingray_Uart3_check(void)
{
   unsigned char i;
   
	if (uart3_GetRxCommand() == TRUE) {
		if ((Stingray_ACK_mode & 0x80) == 0x80) {
			if (Stingray_ACK_mode == 0x82) {							//? Screen
				Screen_Compare();										//? f_Screen_Key_OK
			}else if (Stingray_ACK_mode == 0x80) {						//? ACK80
				if (Stingray_Tx_mode == Stingray_ACK_mode_ID) {
					if (Stingray_Tx_mode_SN == Stingray_ACK_mode_SN) {
						//ACK_Key_OK!
						f_ACK_Key_OK = ON;
					}
				}
			}else {
				if (Stingray_Tx_mode == (Stingray_ACK_mode & 0x7f)) {
					if (Stingray_Tx_mode_SN == Stingray_ACK_mode_SN) {
						//ACK_Key_OK!
						f_ACK_Key_OK = ON;
					}
				}
			}
		}
	}
}
//
// Preset
//
void	DAB_99_display_blinking(unsigned char preset_num)
{
	unsigned	char i;
	unsigned	char val_10,val_1;
	
	// "__" + flag"memory" off or "  " + flag"memory" toggle 
	for(i=0;i<20;i++)	DispBuf_Preset[i] =	' ';
	DispBuf_Preset[2] = 'P';
	DispBuf_Preset[3] = 'r';
	DispBuf_Preset[4] = 'e';
	DispBuf_Preset[5] = 's';
	DispBuf_Preset[6] = 'e';
	DispBuf_Preset[7] = 't';
	DispBuf_Preset[8] = ' ';
	if (f_DAB_preset_500msec == 0) {
		// "Preset --"
		DispBuf_Preset[9] = '_';
		DispBuf_Preset[10] = '_';
	}else{
		if (preset_num == 0x20){				//20091223
			DispBuf_Preset[9] = ' ';			//preset HIGH
			DispBuf_Preset[10] = ' ';			//preset Low		: 20091223
		}else if (preset_num == 0){
			DispBuf_Preset[9] = ' ';			//preset HIGH
			DispBuf_Preset[10] = '0';			//preset Low		: 20091223
		}else{
			val_10 = preset_num /10;
			val_1 = preset_num - val_10*10;
			if (val_10 == 0 ){
				DispBuf_Preset[9] = ' ';			//preset HIGH
			}else{
				DispBuf_Preset[9] = val_10 + 0x30;	//preset HIGH
			}
			DispBuf_Preset[10] = val_1 + 0x30;		//preset Low
		}
	}
}
void	DAB_99_display(unsigned char preset_num)
{
	unsigned	char i;
	unsigned	char val_10,val_1;

	if((preset_num == 0x20) || (preset_num == 0)){		//20091223	
		// "Preset --"
		for(i=0;i<20;i++)	DispBuf_Preset[i] =	' ';
		DispBuf_Preset[2] = 'P';
		DispBuf_Preset[3] = 'r';
		DispBuf_Preset[4] = 'e';
		DispBuf_Preset[5] = 's';
		DispBuf_Preset[6] = 'e';
		DispBuf_Preset[7] = 't';
		DispBuf_Preset[8] = ' ';
		DispBuf_Preset[9] = '_';
		DispBuf_Preset[10] = '_';
	}else{	
		// "Preset 99"
		for(i=0;i<20;i++)	DispBuf_Preset[i] =	' ';
		val_10 = preset_num/10;
		val_1 = preset_num - val_10*10;
		DispBuf_Preset[2] = 'P';
		DispBuf_Preset[3] = 'r';
		DispBuf_Preset[4] = 'e';
		DispBuf_Preset[5] = 's';
		DispBuf_Preset[6] = 'e';
		DispBuf_Preset[7] = 't';
		DispBuf_Preset[8] = ' ';
		if (val_10 == 0 ){
			if(preset_num < 4){			
				DispBuf_Preset[9] = '0';			//preset HIGH		: 20091223
			}else{
				DispBuf_Preset[9] = ' ';			//preset HIGH		: 20091223
			}
		}else{
			DispBuf_Preset[9] = val_10 + 0x30;	//preset HIGH
		}
		DispBuf_Preset[10] = val_1 + 0x30;		//preset Low
	}
}
//20091216
//	//StationName clear
//	for (i=0;i<30;i++) {
//		for (j=0;j<(STATIONNAME_MAX_SIZE);j++) {			//20090617
//			uStingray.sSName.IRadioStationName[i][j] = 0x20;
//			uStingray.sSName.FMStationName[i][j] = 0x20;
//			uStingray.sSName.DABStationName[i][j] =0x20;
//		}
//	}
//
void PresetEmptyCheck(unsigned char preset_num)
{
	unsigned	char i,j;

	if((preset_num > 0) && (preset_num <= 30)) {			
		switch(mode_func) {
			case FUNCTION_STINGRAY:
				i = 0;
				if (mode_stingray == FUNCTION_ST_iRADIO) {
					for (j=1;j<(STATIONNAME_MAX_SIZE);j++) {
						if (uStingray.sSName.IRadioStationName[preset_num-1][j] == 0x20) ++i;
					}
				}else if (mode_stingray == FUNCTION_ST_FM) {
					for (j=1;j<(STATIONNAME_MAX_SIZE);j++) {
						if (uStingray.sSName.FMStationName[preset_num-1][j] == 0x20) ++i;
					}
				}else if (mode_stingray == FUNCTION_ST_DAB) {
					for (j=1;j<(STATIONNAME_MAX_SIZE);j++) {
						if (uStingray.sSName.DABStationName[preset_num-1][j] == 0x20) ++i;
					}
				}
				if (i == STATIONNAME_MAX_SIZE-1) {		//20100210
					// DAB 99 [ Empty ]
					DispBuf_Preset[12] = '[';			//[
					DispBuf_Preset[13] = 'E';			//E
					DispBuf_Preset[14] = 'm';			//m
					DispBuf_Preset[15] = 'p';			//p
					DispBuf_Preset[16] = 't';			//t
					DispBuf_Preset[17] = 'y';			//y
					DispBuf_Preset[18] = ']';			//]
				}						
				break;	
			default:
				break;
		}
	}
}
//
void DAB_Preset_Reset(void)
{
//Memory/Preset reset
	f_DAB_preset_500msec = 0;
	timer_500msec = TIME10MSTO500MS;
	mode_preset = PRESET_END;
	count_DisplayDAB_Preset = 0;
	DABfl_Memory = 0;						
}
//
void DAB_Preset_Start(void)
{

	count_DisplayDAB_Preset = 0;
}

//
//	Preset 1~9,0
//
void Func_STMPreset(unsigned char num)
{
	unsigned	char i;
	
	presetKey_imsi = num;

	switch(mode_func) {
		case FUNCTION_AUX:
		case FUNCTION_AUX2:						//20090708
		case FUNCTION_CD:						//20090708
//			Set_DisplayBlockCTimer(BLOCKC_NORMAL,0);	
			break;	
//20090616
		case FUNCTION_STINGRAY:
			//iRadio(0x00), FM(0x05), DAB(0x0c) check
			if ((Stingray_StateData[0] == 0x00) || (Stingray_StateData[0] == 0x05) || (Stingray_StateData[0] == 0x0c)) {
				Func_STMPresetStart(presetKey_imsi);
			}else if (Stingray_StateData[0] == 0x01) {	//Media player
//			}else if (Stingray_StateData[0] == 0xff) {	//Mode
			}else{
				//100msec => GetState();
				count_preset_state = TIME10MSTO200MS;
				Stingray_Tx_GetState();				//Stingray_StateData[0]=mode:0x00=iRadio, 0x01=MedaiPlayer, 0x05=FM, 0x0c=DAB
			}
			break;	

		case FUNCTION_IPOD:		
		case FUNCTION_DISP_OFF:		
		default:
			break;
	}
}
//
void Func_STMPresetStart(unsigned char num)
{
	unsigned	char i;

	switch(mode_func) {
		case FUNCTION_AUX:
		case FUNCTION_AUX2:						//20090708
		case FUNCTION_CD:						//20090708
			break;	

		case FUNCTION_STINGRAY:
			switch(mode_preset){
				case MEMORY_START:
					if ( num < 4 ) {
						//0,1,2,3
						dab_preset_new = num;
						mode_preset = MEMORY_PRESET2Ready;
						//memory/preset 5sec count re-start!!!!
						timer_500msec = TIME10MSTO500MS;
						count_DisplayDAB_Preset = TIME_STINGRAY_PRESET;		//20091215
					}else{
						//4~9 : memory go!
						dab_preset_new = num;
						goto memory_go_49;
					}
					break;
				case MEMORY_PRESET2Ready:
					//Max 30 check
					if (((dab_preset_new*10)+ num ) < 31 ) {
						dab_preset_new = (dab_preset_new*10)+ num;
memory_go_49:
						if((dab_preset_new > 0) && (dab_preset_new < 31)){		//20091223
							dab_preset = dab_preset_new;
							//SavetoPreset!!!
							Stingray_Tx_SaveToPreset(dab_preset);
							DAB_99_display(dab_preset);			
							mode_preset = MEMORY_ENDing;
							count_DisplayDAB_Preset = TIME10MSTO1500MS;
							DABfl_Memory = 0;
						}else{
							timer_500msec = TIME10MSTO500MS;
							count_DisplayDAB_Preset = TIME_STINGRAY_PRESET;		//20091215
						}
					}else{
						timer_500msec = TIME10MSTO500MS;
						count_DisplayDAB_Preset = TIME_STINGRAY_PRESET;			//20091215
					}
					break;
				case PRESET_PRESET2Ready:
					//Max 30 check
					if (((dab_preset_new*10)+ num ) < 31 ) {
						dab_preset_new = (dab_preset_new*10)+ num;
preset_go_49:
						if((dab_preset_new > 0) && (dab_preset_new < 31)){		//20091223
							dab_preset = dab_preset_new;
							//LoadPreset start!	//20100128 current mode
							Stingray_Tx_LoadPreset(0xff,dab_preset);
							DAB_99_display(dab_preset);			
							//preset empty checking : 20091216
							PresetEmptyCheck(dab_preset);
							mode_preset = PRESET_ENDing;
							count_DisplayDAB_Preset = TIME10MSTO1500MS;
						}else{
							timer_500msec = TIME10MSTO500MS;
							count_DisplayDAB_Preset = TIME_STINGRAY_PRESET;		//20091215
						}
					}else{
						timer_500msec = TIME10MSTO500MS;
						count_DisplayDAB_Preset = TIME_STINGRAY_PRESET;			//20091215
					}
					break;
				case MEMORY_ENDing:
				case PRESET_ENDing:
				case PRESET_EMPTY:
				case PRESET_END:
				default:
					//Preset Starting
					//display mode :
					mode_DisplayDAB = DAB_PRESET;
					if ( num < 4 ) {
						//0,1,2,3
						dab_preset_new = num;
						mode_preset = PRESET_PRESET2Ready;
						//memory/preset 5sec count re-start!!!!
						timer_500msec = TIME10MSTO500MS;
						count_DisplayDAB_Preset = TIME_STINGRAY_PRESET;			//20091215	
					}else{
						//4~9 : memory go!
						dab_preset_new = num;
						goto preset_go_49;
					}
					break;
			}
			break;
			
		case FUNCTION_IPOD:		
		case FUNCTION_DISP_OFF:		
		default:
			break;
	}
}
//
void RFunc_TunerPreset1(void)
{
	unsigned	char i;

	i=STM_1;
	Func_STMPreset(i);	
}
void RFunc_TunerPreset2(void)
{
	unsigned	char i;

	i=STM_2;
	Func_STMPreset(i);	
}
void RFunc_TunerPreset3(void)
{
	unsigned	char i;

	i=STM_3;
	Func_STMPreset(i);	
}
void RFunc_TunerPreset4(void)
{
	unsigned	char i;

	i=STM_4;
	Func_STMPreset(i);	
}
void RFunc_TunerPreset5(void)
{
	unsigned	char i;

	i=STM_5;
	Func_STMPreset(i);	
}
void RFunc_TunerPreset6(void)
{
	unsigned	char i;

	i=STM_6;
	Func_STMPreset(i);	
}
void RFunc_TunerPreset7(void)
{
	unsigned	char i;

	i=STM_7;
	Func_STMPreset(i);	
}
void RFunc_TunerPreset8(void)
{
	unsigned	char i;

	i=STM_8;
	Func_STMPreset(i);	
}
void RFunc_TunerPreset9(void)
{
	unsigned	char i;

	i=STM_9;
	Func_STMPreset(i);	
}
void RFunc_TunerPreset0(void)
{
	unsigned	char i;

	i=STM_0;
	Func_STMPreset(i);	
}

//
// preset counter ending
//
void DAB_Preset5sec_counter_10msec(void)
{
	unsigned char ret;

	if (mode_func != FUNCTION_STINGRAY) return;

	switch(Stingray_Startup_mode){
		case S_CONFIGURE_OK:
			if(count_DisplayDAB_Preset != 0) {
				--count_DisplayDAB_Preset;
				if(count_DisplayDAB_Preset == 0) {
					// 5sec count end!!
					// next status check
					// memory x and 1~9 
					// preset 1~9
					switch(mode_preset){
						case MEMORY_START:
							mode_preset = MEMORY_END;
							DABfl_Memory = 0;
							//display mode :
							mode_DisplayDAB = DAB_NORMAL;
							break;
						case MEMORY_PRESET2Ready:
							//memory start!!!
							if((dab_preset_new > 0) && (dab_preset_new < 31)){		//20091223
								dab_preset = dab_preset_new;
								//iRadio(0x00), FM(0x05), DAB(0x0c) check
								if ((Stingray_StateData[0] == 0x00) || (Stingray_StateData[0] == 0x05) || (Stingray_StateData[0] == 0x0c)) {
									//SavetoPreset!!!
									Stingray_Tx_SaveToPreset(dab_preset);
								}
							}
							DAB_99_display(dab_preset);			
							mode_preset = MEMORY_ENDing;
							count_DisplayDAB_Preset = TIME10MSTO1500MS;
							DABfl_Memory = 0;
							break;
						case PRESET_PRESET2Ready:
							//preset read start!
							if((dab_preset_new > 0) && (dab_preset_new < 31)){		//20091223
								dab_preset = dab_preset_new;
								//iRadio(0x00), FM(0x05), DAB(0x0c) check
								if ((Stingray_StateData[0] == 0x00) || (Stingray_StateData[0] == 0x05) || (Stingray_StateData[0] == 0x0c)) {
									//LoadPreset start! : 20100128 current mode
									Stingray_Tx_LoadPreset(0xff,dab_preset);					
								}
							}
							DAB_99_display(dab_preset);			
							//preset empty checking : 20091216
							PresetEmptyCheck(dab_preset);
							mode_preset = PRESET_ENDing;
							count_DisplayDAB_Preset = TIME10MSTO1500MS;
							break;
						case MEMORY_ENDing:
							//GetStationName() call				//20090617
							Stingray_Tx_GetStationName(Stingray_StateData[0],dab_preset);
							
							dab_CurrentPreset = dab_preset;
							mode_preset = MEMORY_END;
//							count_DisplayDAB_back = 1;			//20060213 : mode_DisaplayDAB 복원.
							DABfl_Memory = 0;
							//display mode :
							mode_DisplayDAB = DAB_NORMAL;
							break;
						case PRESET_ENDing:
							dab_CurrentPreset = dab_preset;
							mode_preset = PRESET_END;
//							count_DisplayDAB_back = 1;			//20060213 : mode_DisaplayDAB 복원.
							//display mode :
							mode_DisplayDAB = DAB_NORMAL;
							break;
						case PRESET_EMPTY:
							mode_preset = PRESET_END;
//							count_DisplayDAB_back = 1;			//20060213 : mode_DisaplayDAB 복원.
							//display mode :
							mode_DisplayDAB = DAB_NORMAL;
							break;
						case PRESET_END:
//							count_DisplayDAB_back = 1;			//20060213 : mode_DisaplayDAB 복원.
							//display mode :
							mode_DisplayDAB = DAB_NORMAL;
							break;
						//20110602
						case PRESET_UpDownENDing:	
							mode_preset = PRESET_END;
							//display mode :
							mode_DisplayDAB = DAB_NORMAL;
							break;
						default:
							break;
					}
				}
			}
			break;
		default:
			break;
	}
}
void Func_MemoryStart(void)
{
	// memory start!
	dab_preset_new = 0x20;				//20091223
	mode_preset = MEMORY_START;
	f_DAB_preset_500msec = 0;
       
	timer_500msec = TIME10MSTO500MS;
	count_DisplayDAB_Preset = TIME_STINGRAY_PRESET;			//20091215
	//display mode :
	mode_DisplayDAB = DAB_PRESET;
}
void Func_TunerMemory(void)
{
	unsigned	char i;

	switch(mode_func) {
		case FUNCTION_AUX:
		case FUNCTION_AUX2:						//20090708
		case FUNCTION_CD:						//20090708
//			Set_DisplayBlockCTimer(BLOCKC_NORMAL,0);	
			break;	
//20090616
		case FUNCTION_STINGRAY:
			//iRadio(0x00), FM(0x05), DAB(0x0c) check
			if ((Stingray_StateData[0] == 0x00) || (Stingray_StateData[0] == 0x05) || (Stingray_StateData[0] == 0x0c)) {
				Func_MemoryStart();
			}else if (Stingray_StateData[0] == 0x01) {	//Media player
//			}else if (Stingray_StateData[0] == 0xff) {	//Mode
			}else{
				//100msec => GetState();
				count_memory_state = TIME10MSTO200MS;
				presetKey_imsi = 0xff;			//memory!!!
				Stingray_Tx_GetState();			//Stingray_StateData[0]=mode:0x00=iRadio, 0x01=MedaiPlayer, 0x05=FM, 0x0c=DAB
			}
			break;	

		case FUNCTION_IPOD:		
		case FUNCTION_DISP_OFF:		
		default:
			break;
	}
}
//
// Stingray module
//	DAB display [ 10msec ]
// 
void	Display_STM(void)
{
	unsigned	char i,j;
	unsigned	char val_10,val_1;				

	//VFD_FontSize = 0x02;				//2 Line small character

	//rs232_printf("mode_DisplayDAB=>%x\n",mode_DisplayDAB);
	switch(mode_DisplayDAB){
		case DAB_POWER_ON:
			break;
		case DAB_SID_SEARCH:			//20060420
			break;
		case DAB_NORMAL:
#ifdef STINGRAY_UTF8_4321
			memcpy((void*)uAdjCurrentUTF8.display_Buffer,(void*)uScreenUTF8.display_Buffer,166);			//2010915
#endif
			memcpy((void*)uAdjCurrent.display_Buffer,(void*)uScreen.display_Buffer,DISPLAYBUFFER_MAX_SIZE);
			break;
		case DAB_NOSERVICE:
			break;
		case DAB_PRESET:					//memory or preset start!!!
			switch(mode_preset){
				case MEMORY_START:
				case MEMORY_PRESET2Ready:
					DAB_99_display_blinking(dab_preset_new);
					//Memory indication blinking
					if (f_DAB_preset_500msec == 0) {
						DABfl_Memory = 1;
						DispBuf_Preset[0] = 'M';
					}else{
						DABfl_Memory = 0;
						DispBuf_Preset[0] = ' ';
					}
					break;					
				case MEMORY_ENDing:
					// " DAB 99"
					DAB_99_display(dab_preset_new);
					//Memory setting
					DABfl_Memory = 1;
					DispBuf_Preset[0] = 'M';
					break;
				case PRESET_PRESET2Ready:
					DAB_99_display_blinking(dab_preset_new);
					break;
				case PRESET_ENDing:
					// " DAB 99"
					DAB_99_display(dab_preset_new);
					//preset empty checking : 20091216
					PresetEmptyCheck(dab_preset_new);
					break;
				case PRESET_EMPTY:
					//DAB99 [ Empty ]
					for(i=0;i<20;i++)	DispBuf_Preset[i] =	' ';
					DispBuf_Preset[2] = 'P';
					DispBuf_Preset[3] = 'r';
					DispBuf_Preset[4] = 'e';
					DispBuf_Preset[5] = 's';
					DispBuf_Preset[6] = 'e';
					DispBuf_Preset[7] = 't';
					DispBuf_Preset[8] = ' ';
         
					val_10 = dab_preset_new /10;
					val_1 = dab_preset_new - val_10*10;
					if (val_10 == 0 ){
						if(dab_preset_new < 4){			
							DispBuf_Preset[9] = '0';			//preset HIGH		: 20091223
						}else{
							DispBuf_Preset[9] = ' ';			//preset HIGH		: 20091223
						}
					}else{
						DispBuf_Preset[9] = val_10 + 0x30;	//preset HIGH
					}
					DispBuf_Preset[10] = val_1 + 0x30;		//preset Low
         
					// DAB 99 [ Empty ]
					DispBuf_Preset[12] = '[';			//[
					DispBuf_Preset[13] = 'E';			//E
					DispBuf_Preset[14] = 'm';			//m
					DispBuf_Preset[15] = 'p';			//p
					DispBuf_Preset[16] = 't';			//t
					DispBuf_Preset[17] = 'y';			//y
					DispBuf_Preset[18] = ']';			//]
					break;
				//20110602
				case PRESET_UpDownENDing:	
					break;
				case PRESET_END:
				default:
					break;
			}
			//
#ifdef STINGRAY_UTF8_4321
			memcpy((void*)uAdjCurrentUTF8.display_Buffer,(void*)uScreenUTF8.display_Buffer,166);			//2010915
			for (i=0;i<20;i++) uAdjCurrentUTF8.sUtf8Stingray.iDispBuf[3][i] = (unsigned short)DispBuf_Preset[i];
#endif
			memcpy((void*)uAdjCurrent.display_Buffer,(void*)uScreen.display_Buffer,DISPLAYBUFFER_MAX_SIZE);
			for (i=0;i<20;i++) uAdjCurrent.sStingray.DispBuf[3][i] = DispBuf_Preset[i];
			break;
		case DAB_PRESET_TRACKING:				// preset start!!!
			break;
		default:
			break;
	}
	//20100427 : checking (40characters) " Writing firmware,   do not turn off !  " 
	if(memcmp((void*)Writing_firmware_donot_turnoff,(void*)uAdjCurrent.display_Buffer,40) == 0){
		//stingray Upgrading Firmware !!!
		mode_stingrayUpgradingFirmware = 1;		//20100426
		
		//for(j=0;j<4;j++) {
		//	for(i=0;i<20;i++) uAdjCurrent.sStingray.DispBuf[j][i] = rotel_UpgradingFirmware[j][i];
		//}
	}else{
		//if (mode_stingrayUpgradingFirmware == 1) {		//20100426
			//completed UpgardingFirmware
		//}
		mode_stingrayUpgradingFirmware = 0;		//20100426
	}
	//
#ifdef STINGRAY_UTF8_4321
	if(memcmp((void*)uCurrentScreenUTF8.display_Buffer,(void*)uAdjCurrentUTF8.display_Buffer,166) != 0){
		memcpy((void*)uCurrentScreenUTF8.display_Buffer,(void*)uAdjCurrentUTF8.display_Buffer,166);

#ifdef UART1_DEBUG_x
				PutChar('\r');
				rs232_printf("!!!!!+++++!!!!!+++++\r",blank);
				for (i=0;i<4;i++) {
//					rs232_printf("L ",blank);
					for (j=0;j<20;j++) {
						PutChar(uCurrentScreenUTF8.sUtf8Stingray.iDispBuf[i][j]);			//uart 1 : temp!!!!!!!!!!!!!!!!!!!!
					}
					PutChar('\r');
				}
#endif
	}
#endif	//STINGRAY_UTF8_4321
	//
	if(memcmp((void*)uCurrentScreen.display_Buffer,(void*)uAdjCurrent.display_Buffer,DISPLAYBUFFER_MAX_SIZE) != 0){
		memcpy((void*)uCurrentScreen.display_Buffer,(void*)uAdjCurrent.display_Buffer,DISPLAYBUFFER_MAX_SIZE);

#ifdef UART1_DEBUG_x
				PutChar('\r');
				rs232_printf("!!!!!+++++!!!!!+++++\r",blank);
				for (i=0;i<4;i++) {
//					rs232_printf("L ",blank);
					for (j=0;j<20;j++) {
						PutChar(uCurrentScreen.sStingray.DispBuf[i][j]);			//uart 1 : temp!!!!!!!!!!!!!!!!!!!!
					}
					PutChar('\r');
				}
#endif
//screen uart1 end
	}
}
/*-----------------------------------------------------------------------------
	Preset blinking => ~ flag
-----------------------------------------------------------------------------*/
void	Check_PresetBlinking_10msec(void)
{
	if (Stingray_Startup_mode != S_CONFIGURE_OK) return;
		
	//500msec timer -> ~flag
	if(--timer_500msec == 0){
		timer_500msec = TIME10MSTO500MS;

		//~f_DAB_preset_500msec;
		if (f_DAB_preset_500msec ==0) {
			f_DAB_preset_500msec = 1;
		}else{
			f_DAB_preset_500msec = 0;
		}
	}
	//
	//memory key =>100msec=>mode check
	if (count_memory_state > 0x00) {
		if (--count_memory_state == 0x00) {
			//20sec=>mode reset
			//count_mode_reset = TIME10MSTO20SEC;			//20090616
			//iRadio(0x00), FM(0x05), DAB(0x0c) check
			if ((Stingray_StateData[0] == 0x00) || (Stingray_StateData[0] == 0x05) || (Stingray_StateData[0] == 0x0c)) {
				Func_MemoryStart();
			}
		}
	}			
	//preset key =>100msec=>mode check
	if (count_preset_state > 0x00) {
		if (--count_preset_state == 0x00) {
			//20sec=>mode reset
			//count_mode_reset = TIME10MSTO20SEC;			//20090616
			//iRadio(0x00), FM(0x05), DAB(0x0c) check
			if ((Stingray_StateData[0] == 0x00) || (Stingray_StateData[0] == 0x05) || (Stingray_StateData[0] == 0x0c)) {
				Func_STMPresetStart(presetKey_imsi);
			}
		}
	}			
//20090616
//20110809 : GetState() 주기적으로 해야할 것으로 생각됨...
	//state =>50msec=>Tx
	if (count_state_Tx > 0x00) {
		if (--count_state_Tx == 0x00) {
			Stingray_Tx_GetState();
		}
	}else{
		if (mode_func == FUNCTION_STINGRAY) {
			//iRadio(0x00), MMedia(0x01), FM(0x05), DAB(0x0c), MPlayer(0x01)
			//20110809
			count_state_Tx = TIME10MSTO100MS;
			if (mode_stingray != Stingray_StateData[0]) count_state_Tx = 1;
		}	
	}
/* 20090616
	//mode =>20sec=>reset
	if (count_mode_reset > 0x00) {
		if (--count_mode_reset == 0x00) {
			Stingray_StateData[0] = 0xff;
		}
	}
*/	//
}
/*-----------------------------------------------------------------------------
 Function Name  : Stingray main
 Description    : call 10msec
 Input          : None
 Output         : None
 Return         : None
 Comments	 	: ..
-----------------------------------------------------------------------------*/
void Stingray_Main(void)
{
	Check_PresetBlinking_10msec();		//Preset blinking 500msec
	Stingray_Uart3_check();				//UART3
	Stingray_StartUpCheck();			//initialize , booting
	Stingray_Normal();   

	DAB_Preset5sec_counter_10msec();	//Preset couner ending
	Display_STM();
	
	if (count_factoryDefault != 0) {	//20090714
		if (--count_factoryDefault == 0) {
			//factoryDefault start!
			Stingray_Init();
			STRnRESET(0);
		}
	}
}

/*-----------------------------------------------------------------------------
 Function Name  : Stingray key input
 Description    : call 
 Input          : None
 Output         : None
 Return         : None
 Comments	 	: ..
#define STM_NULL			0x00
#define STM_ENC_U			0x01
#define STM_ENC_D			0x02
#define STM_1				0x03
#define STM_2				0x04
#define STM_3				0x05
#define STM_4				0x06
#define STM_5				0x07
#define STM_6				0x08
#define STM_7				0x09
#define STM_8				0x0a
#define STM_9				0x0b
#define STM_0				0x0c
#define STM_REPLY			0x0d
#define STM_BACK			0x0e
#define STM_SELECT			0x0f
#define STM_SKIP_N			0x10
#define STM_SKIP_P			0x11
#define STM_PLAYPAUSE		0x12
#define STM_STOP			0x13
#define STM_MODE			0x14
#define STM_QUEUE			0x15
#define STM_VOLUME_U		0x16
#define STM_VOLUME_D		0x17
#define STM_ALARM			0x18
#define STM_POWER			0x19
//
#define STM_FM				0x1a
#define STM_IRADIO			0x1b
#define STM_MEDIA			0x1c
#define STM_LINEIN			0x1d
#define STM_MEDIA_PLAY		0x1e
#define STM_MEDIA_PAUSE		0x1f
#define STM_MEDIA_SHUFFLE 	0x20
#define STM_MEDIA_REPEAT	0x21
#define STM_MENU_TOP		0x22
#define STM_DAB				0x23
#define STM_SOURCE			0x24
//
#define STM_LAST			0x27
//
#define STM_MEMORY			0x7e

//#define SK_NULL			0x7f
-----------------------------------------------------------------------------*/
void Stingray_KeyInput(unsigned char data)
{
		if (Stingray_Startup_mode == S_CONFIGURE_OK){
			//key UP/DOWN/POWER
			if ((data == STM_ENC_U) || (data == STM_ENC_D) || (data == STM_POWER)) {
				Stingray_Key_Value = data;
				Stingray_Tx_KeyPressed(Stingray_Key_Value);
				
				//20100331 : SK_UP,SK_DOWN ( long key(key_Pressed,key_Released)적용 check => Stingray 에서 무시하는 것으로판단.)
				//if ((data == STM_ENC_U) || (data == STM_ENC_D)) {
				//}else{
					Stingray_Tx_KeyReleased(Stingray_Key_Value);
					Stingray_Key_Value = 0;
				//}
			} else {
				//key PRESET1~9
				if ((data >= STM_1) && (data <= STM_9)) {
					Func_STMPreset(data-2);
				//key PRESET0
				}else if (data == STM_0) {
					Func_STMPreset(0);
				//key MEMORY
				}else if (data == STM_MEMORY) {
					Func_TunerMemory();
				}else {	
					//key release 대기중?
//					if (Stingray_Key_Value == 0 ) {
						if ((data == STM_NULL) || (data >= STM_LAST)) return;
						Stingray_Key_Value = data;
						Stingray_Tx_KeyPressed(Stingray_Key_Value);
//chd
						if ((data == STM_STOP)) return;						//20091222 : Stop KeyRelease => main Tx
						if ((data == STM_SELECT)) return;					//20100310 : Select KeyRelease => main Tx
						Stingray_Tx_KeyReleased(Stingray_Key_Value);
						Stingray_Key_Value = 0;

//					}
				}
			}
			//STM_MEMORY
			//STM_NULL
		}		
}
/*
20100210
	make sort preset list table

	uDispStingray.DStationName[90][3+40] : [infoType]+[number]+[status]+[count(size)]+[39]
	mode_SortMaxPresetList
	uStingray.StationName[90][40]

-----------------------------------------------------------------------------*/
void makeSortPresetListTable(void)
{
	unsigned	char i,j,k;
	
	//DStationName clear : 20100210
	for (i=0;i<90;i++) {
		for (j=0;j<3;j++) uDispStingray.DStationName[i][j] = 0;
		for (j=3;j<(STATIONNAME_MAX_SIZE+3);j++) uDispStingray.DStationName[i][j] = 0x20;
	}

	for (i=0,k=0;i<90;i++)
	{
		if (uStingray.StationName[i][0] > 0) {
			//It's list!
			//mode[0x0/0x05/0x0c]
			if(i < 30) {
				//iRadio
				uDispStingray.DStationName[k][0] = 0;		//(iRadio)
				//number[0..29]
				uDispStingray.DStationName[k][1] = i;
			}else if(i < 60) {
				//FM
				uDispStingray.DStationName[k][0] = 5;		//(FM)
				//number[0..29]
				uDispStingray.DStationName[k][1] = i-30;
			}else{
				//DAB
				uDispStingray.DStationName[k][0] = 0x0c;	//(DAB)
				//number[0..29]
				uDispStingray.DStationName[k][1] = i-60;
			}
			//status[0/1]
			uDispStingray.DStationName[k][2] = 1;
			//count(size),name[39]
			for (j=0;j<STATIONNAME_MAX_SIZE;j++) uDispStingray.DStationName[k][j+3] = uStingray.StationName[i][j];

			//count
			++k;
		}
	}
	mode_SortMaxPresetList = k;	
}
/*
20110602
-----------------------------------------------------------------------------
 Function Name  : preset up/down
 Description    : call 
 Input          : None
 Output         : None
 Return         : None
 Comments	 	: ..
-----------------------------------------------------------------------------*/
void Func_Preset_Up(void)
{
	unsigned char pst;
	
	switch(mode_func) {
		case FUNCTION_AUX:
		case FUNCTION_AUX2:						//20090708
		case FUNCTION_CD:						//20090708
//			Set_DisplayBlockCTimer(BLOCKC_NORMAL,0);	
			break;	
//20090616
		case FUNCTION_STINGRAY:
			//iRadio(0x00), FM(0x05), DAB(0x0c) check
			if ((Stingray_StateData[0] == 0x00) || (Stingray_StateData[0] == 0x05) || (Stingray_StateData[0] == 0x0c)) {
				//preset up!!!
				pst = dab_preset;
				++pst;
				if((pst == 0) || (pst > 30)){
					pst = 1;
				}
				dab_preset = pst;
				//
				//LoadPreset start!	//20100128 current mode
				Stingray_Tx_LoadPreset(0xff,dab_preset);
				DAB_99_display(dab_preset);			
				//preset empty checking : 20091216
				PresetEmptyCheck(dab_preset);
				mode_preset = PRESET_UpDownENDing;
				count_DisplayDAB_Preset = TIME10MSTO1500MS;
				//display mode :
				mode_DisplayDAB = DAB_PRESET;

			}else if (Stingray_StateData[0] == 0x01) {	//Media player
//			}else if (Stingray_StateData[0] == 0xff) {	//Mode
			}else{
				//100msec => GetState();
				count_preset_state = TIME10MSTO200MS;
				//Stingray_StateData[0]=mode:0x00=iRadio, 0x01=MedaiPlayer, 0x05=FM, 0x0c=DAB
				Stingray_Tx_GetState();
			}
			break;	

		case FUNCTION_IPOD:		
		case FUNCTION_DISP_OFF:		
		default:
			break;
	}
}
/*
-----------------------------------------------------------------------*/
void Func_Preset_Down(void)
{
	unsigned char pst;
	
	switch(mode_func) {
		case FUNCTION_AUX:
		case FUNCTION_AUX2:						//20090708
		case FUNCTION_CD:						//20090708
//			Set_DisplayBlockCTimer(BLOCKC_NORMAL,0);	
			break;	
//20090616
		case FUNCTION_STINGRAY:
			//iRadio(0x00), FM(0x05), DAB(0x0c) check
			if ((Stingray_StateData[0] == 0x00) || (Stingray_StateData[0] == 0x05) || (Stingray_StateData[0] == 0x0c)) {
				//preset up!!!
				pst = dab_preset;
				--pst;
				if((pst == 0) || (pst > 30)){
					pst = 30;
				}
				dab_preset = pst;
				//
				//LoadPreset start!	//20100128 current mode
				Stingray_Tx_LoadPreset(0xff,dab_preset);
				DAB_99_display(dab_preset);			
				//preset empty checking : 20091216
				PresetEmptyCheck(dab_preset);
				mode_preset = PRESET_UpDownENDing;
				count_DisplayDAB_Preset = TIME10MSTO1500MS;
				//display mode :
				mode_DisplayDAB = DAB_PRESET;

			}else if (Stingray_StateData[0] == 0x01) {	//Media player
//			}else if (Stingray_StateData[0] == 0xff) {	//Mode
			}else{
				//100msec => GetState();
				count_preset_state = TIME10MSTO200MS;
				//Stingray_StateData[0]=mode:0x00=iRadio, 0x01=MedaiPlayer, 0x05=FM, 0x0c=DAB
				Stingray_Tx_GetState();
			}
			break;	

		case FUNCTION_IPOD:		
		case FUNCTION_DISP_OFF:		
		default:
			break;
	}
}
/*
20110922 add remote Preset up/down repeat key 
-----------------------------------------------------------------------*/
void rFunc_Preset_Up(void)
{
	unsigned char pst;
	
//20110922
	if(count_rmcRepeatKey < 1) {
presetUp_repeat_go:

	switch(mode_func) {
		case FUNCTION_AUX:
		case FUNCTION_AUX2:						//20090708
		case FUNCTION_CD:						//20090708
//			Set_DisplayBlockCTimer(BLOCKC_NORMAL,0);	
			break;	
//20090616
		case FUNCTION_STINGRAY:
			//iRadio(0x00), FM(0x05), DAB(0x0c) check
			if ((Stingray_StateData[0] == 0x00) || (Stingray_StateData[0] == 0x05) || (Stingray_StateData[0] == 0x0c)) {
				//preset up!!!
				pst = dab_preset;
				++pst;
				if((pst == 0) || (pst > 30)){
					pst = 1;
				}
				dab_preset = pst;
				//
				//LoadPreset start!	//20100128 current mode
				Stingray_Tx_LoadPreset(0xff,dab_preset);
				DAB_99_display(dab_preset);			
				//preset empty checking : 20091216
				PresetEmptyCheck(dab_preset);
				mode_preset = PRESET_UpDownENDing;
				count_DisplayDAB_Preset = TIME10MSTO1500MS;
				//display mode :
				mode_DisplayDAB = DAB_PRESET;

			}else if (Stingray_StateData[0] == 0x01) {	//Media player
//			}else if (Stingray_StateData[0] == 0xff) {	//Mode
			}else{
				//100msec => GetState();
				count_preset_state = TIME10MSTO200MS;
				//Stingray_StateData[0]=mode:0x00=iRadio, 0x01=MedaiPlayer, 0x05=FM, 0x0c=DAB
				Stingray_Tx_GetState();
			}
			break;	

		case FUNCTION_IPOD:		
		case FUNCTION_DISP_OFF:		
		default:
			break;
	}

//20110922
	}else{
		//remocon repeat key check
		if(count_rmcRepeatKey >= 5) {			//20110922
			count_rmcRepeatKey = 3;
			//repeat key go
			goto presetUp_repeat_go;
		}
	}

}
/*
-----------------------------------------------------------------------*/
void rFunc_Preset_Down(void)
{
	unsigned char pst;
	
//20110922
	if(count_rmcRepeatKey < 1) {
presetDown_repeat_go:

	switch(mode_func) {
		case FUNCTION_AUX:
		case FUNCTION_AUX2:						//20090708
		case FUNCTION_CD:						//20090708
//			Set_DisplayBlockCTimer(BLOCKC_NORMAL,0);	
			break;	
//20090616
		case FUNCTION_STINGRAY:
			//iRadio(0x00), FM(0x05), DAB(0x0c) check
			if ((Stingray_StateData[0] == 0x00) || (Stingray_StateData[0] == 0x05) || (Stingray_StateData[0] == 0x0c)) {
				//preset up!!!
				pst = dab_preset;
				--pst;
				if((pst == 0) || (pst > 30)){
					pst = 30;
				}
				dab_preset = pst;
				//
				//LoadPreset start!	//20100128 current mode
				Stingray_Tx_LoadPreset(0xff,dab_preset);
				DAB_99_display(dab_preset);			
				//preset empty checking : 20091216
				PresetEmptyCheck(dab_preset);
				mode_preset = PRESET_UpDownENDing;
				count_DisplayDAB_Preset = TIME10MSTO1500MS;
				//display mode :
				mode_DisplayDAB = DAB_PRESET;

			}else if (Stingray_StateData[0] == 0x01) {	//Media player
//			}else if (Stingray_StateData[0] == 0xff) {	//Mode
			}else{
				//100msec => GetState();
				count_preset_state = TIME10MSTO200MS;
				//Stingray_StateData[0]=mode:0x00=iRadio, 0x01=MedaiPlayer, 0x05=FM, 0x0c=DAB
				Stingray_Tx_GetState();
			}
			break;	

		case FUNCTION_IPOD:		
		case FUNCTION_DISP_OFF:		
		default:
			break;
	}
//20110922
	}else{
		//remocon repeat key check
		if(count_rmcRepeatKey >= 5) {			//20110922
			count_rmcRepeatKey = 3;
			//repeat key go
			goto presetDown_repeat_go;
		}
	}

}
//
//
//		Stintray A(w/o thumb-USB)	: RSP1580
//		Stintray C(w/o thumb-USB)	: RDG1520 , RCX1500
//		Stintray C(w/ thumb-USB)	: RT-09
//
//		initial 					: STRnRESET(0);				//stingray module reset port = low
//									Stingray_Init();
//
//		main(10msec) 				: Stingray_Main();
//		key input 					: Stingray_KeyInput(data);
//		output 						: uCurrentScreen.display_Buffer[DISPLAYBUFFER_MAX_SIZE]

//									typedef union uTDISP {			
//										unsigned char	display_Buffer[DISPLAYBUFFER_MAX_SIZE];
//										struct sStingray{
//											unsigned char DispBuf[4][20];			
//											unsigned char CursorXYPosition[4];			
//											unsigned char ScreenState[2];			
//										}sStingray;
//									}uTDISP;
//
//									CursorOn 	: uCurrentScreen.display_Buffer[80] : ON(0x01)
//									XPosition	: uCurrentScreen.display_Buffer[81]	: column(0~19) (? + 1)
//									YPosition	: uCurrentScreen.display_Buffer[82]	: line(0~3)			
//									CursorWidth	: uCurrentScreen.display_Buffer[83]	: width size			
//
//									State		: uCurrentScreen.display_Buffer[84]	: menu/palyback/entryScreen(0~3)			
//									ExtraInfo	: uCurrentScreen.display_Buffer[85]	: standard/IPAddress/callSign/stationName characters(0~3)			
//
//		Factory Reset 				: Stingray_Tx_FactoryReset(); 
//									==> wait for [f_ACK_Key_OK = ON] 
//									==> [ Stingray_ACK_status = 0x01 : FactoryReset completed ]
//									==> STRnRESET(0);				//stingray module reset port = low
//									==> Stingray_Init();
//
//
//

/* end of file */
/******************* (C) COPYRIGHT 2008 Rotel Technology Global **************/

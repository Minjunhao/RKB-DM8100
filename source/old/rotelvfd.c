/*****************************************************************************
******************************************************************************
**                                                                          **
**  Copyright (c) 2006 Rotel Technology Global.                             **
**  All rights reserved.                                                    **
**                                                                          **
**  The computer program contained herein contains proprietary information  **
**  which is the property of Videon Central, Inc.  The program may be used  **
**  and/or copied only with the written permission of Videon Central, Inc.  **
**  or in accordance with the terms and conditions stipulated in the        **
**  agreement/contract under which the programs have been supplied.         **
**                                                                          **
******************************************************************************
*****************************************************************************/
/**
 * @file
 *    rotelvfd.c
 *
 * Controls the Vacuum Flourescent Display on the Rotel DVD player,
 * sends button pressed data to the Cheers. Receives unsolicited
 * data from the Cheers, and sets up the VFD accordingly.
 *
 * When looking over this code keep in mind that the architecture
 * of the PIC chips don't allow a pointer to a constant. Therefore
 * I have to keep an array of offsets to the bitmap for the display
 * instead of pointers into the bitmaps.
 * This code has to trade some readibility for effeciency.
 * Most of this code is compiler specific. It is not ever going to
 * be transportable
 *
 *
 *
*/
//-----------------------------------------------------------------------------
//  Included header files
#include "config.h"
#include "initmcu.h"
#include "uart.h"
#include "main.h"
#include "rotelvfd.h"
//#include "charDef.h"					// Character map
//#include	"display.h"					// display specific defines
#include "function.ext"					//20091224
#include "proc_function.h"				//20091224

//-----------------------------------------------------------------------------
#define _MN_ "VFD: "
//-----------------------------------------------------------------------------
//typedef unsigned char Uint8;
byte    u8column;
Uint8   u8datPos = 0;
Uint8 	u8dim,dim_back = 0;				//for display dim function
Uint8 	shiftDatIdx[40];				//[4x20] rowD->C->B->A and [2x20] rowD->A : shiftDatIdx[1..40]
Uint8 	u8MsgLen = 0;
Uint8   fSecretButtonPress = 0;

void 	sendDataVFD(Uint8 u8colNum);
Uint8 	getBitmapOffset( Uint8 u8ArrayOffset );

// Only 255 elements allowed in an array, had to split into two.
BYTE const charAasciiFont[126][5] = {	/* 5x7 ASCII character font */
 {0x00, 0x00, 0x00, 0x00, 0x00},  // 0x20 space                       
 {0x20, 0x08, 0x42, 0x10, 0x80},  // 0x21 !                           
 {0x00, 0x00, 0x05, 0x29, 0x40},  // 0x22 "                           
 {0x52, 0xBE, 0xAF, 0xA9, 0x40},  // 0x23 #                           
 {0x23, 0xE8, 0xE2, 0xF8, 0x80},  // 0x24 $                           
 {0xC6, 0x44, 0x44, 0x4C, 0x60},  // 0x25 %                           
 {0xB2, 0x6A, 0x22, 0xA4, 0xC0},  // 0x26 &                           
 {0x00, 0x00, 0x01, 0x10, 0xC0},  // 0x27 '                           
 {0x41, 0x04, 0x21, 0x11, 0x00},  // 0x28 (                           
 {0x11, 0x10, 0x84, 0x10, 0x40},  // 0x29 )                           
 {0x01, 0x2A, 0xEA, 0x90, 0x00},  // 0x2a *                           
 {0x01, 0x09, 0xF2, 0x10, 0x00},  // 0x2b +                           
 {0x11, 0x0C, 0x00, 0x00, 0x00},  // 0x2c ,                           
 {0x00, 0x01, 0xF0, 0x00, 0x00},  // 0x2d -                           
 {0x31, 0x80, 0x00, 0x00, 0x00},  // 0x2e .                           
 {0x00, 0x44, 0x44, 0x40, 0x00},  // 0x2f /                           
 {0x74, 0x67, 0x5C, 0xC5, 0xC0},  // 0x30 0                           
 {0x71, 0x08, 0x42, 0x18, 0x80},  // 0x31 1                           
 {0xF8, 0x88, 0x88, 0x45, 0xC0},  // 0x32 2                           
 {0x74, 0x60, 0x82, 0x23, 0xE0},  // 0x33 3                           
 {0x42, 0x3E, 0x95, 0x31, 0x00},  // 0x34 4                           
 {0x74, 0x61, 0x07, 0x87, 0xE0},  // 0x35 5                           
 {0x74, 0x62, 0xF0, 0x89, 0x80},  // 0x36 6                           
 {0x10, 0x84, 0x44, 0x43, 0xE0},  // 0x37 7                           
 {0x74, 0x62, 0xE8, 0xC5, 0xC0},  // 0x38 8                           
 {0x32, 0x21, 0xE8, 0xC5, 0xC0},  // 0x39 9                           
 {0x01, 0x8C, 0x03, 0x18, 0x00},  // 0x3a :                           
 {0x11, 0x0C, 0x03, 0x18, 0x00},  // 0x3b ;                           
 {0x41, 0x04, 0x11, 0x11, 0x00},  // 0x3c <                           
 {0x00, 0x3E, 0x0F, 0x80, 0x00},  // 0x3d =                           
 {0x11, 0x11, 0x04, 0x10, 0x40},  // 0x3e >                           
 {0x20, 0x08, 0x88, 0x45, 0xC0},  // 0x3f ?                           
 {0x75, 0x6B, 0x68, 0x45, 0xC0},  // 0x40 @                           
 {0x8C, 0x7F, 0x18, 0xC5, 0xC0},  // 0x41 A                           
 {0x7C, 0x62, 0xF8, 0xC5, 0xF0},  // 0x42 B                           
 {0x74, 0x42, 0x10, 0xC5, 0xC0},  // 0x43 C                           
 {0x3A, 0x63, 0x18, 0xA4, 0xE0},  // 0x44 D                           
 {0xF8, 0x42, 0xF0, 0x87, 0xE0},  // 0x45 E                           
 {0x08, 0x42, 0xF0, 0x87, 0xE0},  // 0x46 F                           
 {0x74, 0x63, 0x90, 0x85, 0xC0},  // 0x47 G                           
 {0x8C, 0x63, 0xF8, 0xC6, 0x20},  // 0x48 H                           
 {0x71, 0x08, 0x42, 0x11, 0xC0},  // 0x49 I                           
 {0x11, 0x4A, 0x42, 0x13, 0xE0},  // 0x4a J                           
 {0x8A, 0x4A, 0x32, 0xA6, 0x20},  // 0x4b K                           
 {0xF8, 0x42, 0x10, 0x84, 0x20},  // 0x4c L                           
 {0x8C, 0x63, 0x5A, 0xEE, 0x20},  // 0x4d M                           
 {0x8C, 0x73, 0x59, 0xC6, 0x20},  // 0x4e N                           
 {0x74, 0x63, 0x18, 0xC5, 0xC0},  // 0x4f O                           
 {0x08, 0x42, 0xF8, 0xC5, 0xE0},  // 0x50 P                           
 {0x83, 0xA3, 0x18, 0xC5, 0xC0},  // 0x51 Q                           
 {0x8A, 0x4A, 0xF8, 0xC5, 0xE0},  // 0x52 R                           
 {0x7C, 0x20, 0xE0, 0x87, 0xC0},  // 0x53 S                           
 {0x21, 0x08, 0x42, 0x13, 0xE0},  // 0x54 T                           
 {0x74, 0x63, 0x18, 0xC6, 0x20},  // 0x55 U                           
 {0x22, 0xA3, 0x18, 0xC6, 0x20},  // 0x56 V                           
 {0x55, 0x6B, 0x58, 0xC6, 0x20},  // 0x57 W                           
 {0x8C, 0x54, 0x45, 0x46, 0x20},  // 0x58 X                           
 {0x21, 0x08, 0x45, 0x46, 0x20},  // 0x59 Y                           
 {0xF8, 0x44, 0x44, 0x43, 0xE0},  // 0x5a Z                           
 {0x70, 0x84, 0x21, 0x09, 0xC0},  // 0x5b [                           
 {0x04, 0x10, 0x41, 0x04, 0x00},  // 0x5c (\)                         
 {0x72, 0x10, 0x84, 0x21, 0xC0},  // 0x5d ]                           
 {0x00, 0x00, 0x08, 0xA8, 0x80},  // 0x5e ^                           
 {0xF8, 0x00, 0x00, 0x00, 0x00},  // 0x5f _                           
 {0x00, 0x00, 0x04, 0x10, 0x40},  // 0x60 `                           
 {0xF4, 0x7D, 0x07, 0x00, 0x00},  // 0x61 a                           
 {0x7C, 0x63, 0x36, 0x84, 0x20},  // 0x62 b                           
 {0x70, 0x42, 0x17, 0x00, 0x00},  // 0x63 c                           
 {0xF4, 0x63, 0x9B, 0x42, 0x00},  // 0x64 d                           
 {0x70, 0x7F, 0x17, 0x00, 0x00},  // 0x65 e                           
 {0x10, 0x84, 0x71, 0x28, 0x80},  // 0x66 f                           
 {0x74, 0x3D, 0x18, 0xF8, 0x00},  // 0x67 g                           
 {0x8C, 0x63, 0x36, 0x84, 0x20},  // 0x68 h                           
 {0x71, 0x08, 0x43, 0x00, 0x80},  // 0x69 i                           
 {0x64, 0xA1, 0x00, 0x40, 0x00},  // 0x6a j                           
 {0x49, 0x46, 0x54, 0x84, 0x00},  // 0x6b k                           
 {0x21, 0x08, 0x42, 0x10, 0x80},  // 0x6c l                           
 {0x8C, 0x6B, 0x55, 0x00, 0x00},  // 0x6d m                           
 {0x8C, 0x63, 0x36, 0x80, 0x00},  // 0x6e n                           
 {0x74, 0x63, 0x17, 0x00, 0x00},  // 0x6f o                           
 {0x08, 0x5F, 0x18, 0xBC, 0x00},  // 0x70 p                           
 {0x84, 0x3D, 0x18, 0xF8, 0x00},  // 0x71 q                           
 {0x08, 0x42, 0x36, 0x80, 0x00},  // 0x72 r                           
 {0x7C, 0x1C, 0x17, 0x00, 0x00},  // 0x73 s                           
 {0x64, 0x84, 0x23, 0x88, 0x40},  // 0x74 t                           
 {0x74, 0x63, 0x18, 0x80, 0x00},  // 0x75 u                           
 {0x22, 0xA3, 0x18, 0x80, 0x00},  // 0x76 v                           
 {0x55, 0x6B, 0x18, 0x80, 0x00},  // 0x77 w                           
 {0x8A, 0x88, 0xA8, 0x80, 0x00},  // 0x78 x                           
 {0x21, 0x08, 0xA8, 0x80, 0x00},  // 0x79 y                           
 {0xF8, 0x88, 0x8F, 0x80, 0x00},  // 0x7a z                           
 {0x41, 0x08, 0x22, 0x11, 0x00},  // 0x7b {                           
 {0x01, 0x08, 0x42, 0x10, 0x00},  // 0x7c |                           
 {0x11, 0x08, 0x82, 0x10, 0x40},  // 0x7d }                           
 {0x00, 0x11, 0x51, 0x00, 0x00},  // 0x7e ~                           
 {0x00, 0x00, 0x00, 0x00, 0x00},  // 0x7f space                       
//0x80 ~ 8f
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
/*-----------------------------------------------------------------------------
ROUTINE NAME : VFD_Init(void)
INPUT        :
OUTPUT       :
DESCRIPTION  : vfd all clear

COMMENTS     : ..
-----------------------------------------------------------------------------*/
void VFDClearDisplay(void)
{
	unsigned char i;
	for (i=0;i<40;i++) {
		shiftDatIdx[i] = ' ';
	}
}
void VFD_Init(void)
{
	VFD_scanStop = 1;				//20091224
    count_delayScanVFD = 0;	        //20121011
	VFDClearDisplay();
	for( u8column = 0; u8column < MAX_COLS; u8column++ ) {
	    sendDataVFD(u8column);
	}
}
/******************************************************************************
 * vfd_processing Function.
 *  The first 20 bits are for the proper column.
 *	Next three bits are underscore data for row B of that column.
 *	Next 35 bits are character data for row B of that column.
 *	Next three bits are underscore data for row A for that column.
 *	Next 35 bits are character data for row A of that column.
 *  Then the data is latched.
 *
 * @param
 *    u8colNum - current colum number
 *
 * @retval
 *    None
 ******************************************************************************/
void vfd_processing(void)
{
	for( u8column = 0; u8column < MAX_COLS; u8column++ )
	{
		sendDataVFD(u8column);
	} //for()
    //20091224 power off -> VFD stop	
    if(Power_GetPowerStatus() == 0) VFD_scanStop = 1;
}
/******************************************************************************
 * sendData Function.
 *  The first 20 bits are for the proper column.
 *	Next three bits are underscore data for row B of that column.
 *	Next 35 bits are character data for row B of that column.
 *	Next three bits are underscore data for row A for that column.
 *	Next 35 bits are character data for row A of that column.
 *  Then the data is latched.
 *
 * @param
 *    u8colNum - current colum number
 *
 * @retval
 *    None
 ******************************************************************************/
void sendDataVFD(Uint8 u8colNum)
{
	Uint8 u8Inc = 0, u8Inc_1 = 0, u8Count = 0, u8ByteCount = 0, u8offset, u8ByteToParse;
	// fCharNum is a flag meaning this is the second time through loop
//	bool fCharNum = 0;
	Uint8 u8CharNum = 0, i;

//dimmer
	if( u8dim <= DIM_DAFAULT )
	{
		// turn display on full brightness
		if(VFD_scanStop == 0) {
		    VFD_Bk(0);
		}
		//vfd_delay;
	}
//G1 .. G20
	for(u8Inc=0; u8Inc < MAX_COLS; u8Inc++ )/* Turn on proper column	*/
	{
		if( u8Inc == u8colNum )
		{
			VFD_Data(1);
		} else {
			VFD_Data(0);
		}
		//vfd_delay;
		VFD_Clock(1);
		//vfd_delay;
		VFD_Clock(0);
		//vfd_delay;
	}
//colNum ==> Row D ==> font table pointer
	u8offset = getBitmapOffset( u8colNum );		//charAasciiFont[111][5]
start_sending_characterD:
//Null *3
	VFD_Data(0);
	for (i=0;i<3;i++) {
		//vfd_delay;
		VFD_Clock(1);
		//vfd_delay;
		VFD_Clock(0);
		//vfd_delay;
	}
	//Row D,C,B,A
	//5 - number of bytes needed for each char
	for( u8Inc = 0; u8Inc < NUM_CHAR_BYTES; u8Inc++ )
	{
		//[4x20] u8dim is 4 to 21. u8ByteCount is 1 to 20. If u8dim == 21( off )
		//[2x20] u8dim is 4 to 11. u8ByteCount is 1 to 10. If u8dim == 11( off )
		// display will never turn on.
//dimmer
		if( u8dim == ++u8ByteCount )
		{
			// turn display on
		if(VFD_scanStop == 0) {
		    VFD_Bk(0);
		}
		}
		//20 - offset in array for row A data
//font table(0..4) => font data reading
//		if( shiftDatIdx[u8colNum] < C_0_CHARS )
//		{
//			u8ByteToParse = c_0[u8offset++];
//		} else {
//			u8ByteToParse = c_1[u8offset++];
//		}
		u8ByteToParse = charAasciiFont[u8offset][u8Inc];
//font data(7..0) --> sending (MAX:35)
		for( u8Inc_1 = 7; u8Inc_1 < 255; u8Inc_1-- )// going to scan bit 7 to bit 0
		{
			//35 - number of bits in a char
			if( u8Count++ == NUM_CHAR_BITS )
			{	//0 or 1 - B or A char
				//35 bits for row A All done
				u8Count = 0;							//reset for row C
				u8CharNum = 1;						   //20 - offset in array for row C data
				u8offset = getBitmapOffset( u8colNum + 20);		//(colNum + 1*20 )
#ifdef ROTEL_VFD_2x20
				goto start_sending_characterA;		//[2x20]
#else
				goto start_sending_characterC;		//[4x20]
#endif
			}
			if( bit_test(u8ByteToParse,u8Inc_1 ) )
			{
				VFD_Data(1);
			} else {
				VFD_Data(0);
			}
		//vfd_delay;
			VFD_Clock(1);
		//vfd_delay;
			VFD_Clock(0);
		//vfd_delay;
		}
	}
/********************************************************************/
start_sending_characterC:
//Null *3
	VFD_Data(0);
	for (i=0;i<3;i++) {
		//vfd_delay;
		VFD_Clock(1);
		//vfd_delay;
		VFD_Clock(0);
		//vfd_delay;
	}
	//Row D,C,B,A
	//5 - number of bytes needed for each char
	for( u8Inc = 0; u8Inc < NUM_CHAR_BYTES; u8Inc++ )
	{
		//u8dim is 4 to 21. u8ByteCount is 1 to 20. If u8dim == 21( off )
		// display will never turn on.
//dimmer
		if( u8dim == ++u8ByteCount )
		{
			// turn display on
		if(VFD_scanStop == 0) {
		    VFD_Bk(0);
		}
		}
		//20 - offset in array for row A data
//font table(0..4) => font data reading
//		if( shiftDatIdx[u8colNum] < C_0_CHARS )
//		{
//			u8ByteToParse = c_0[u8offset++];
//		} else {
//			u8ByteToParse = c_1[u8offset++];
//		}
		u8ByteToParse = charAasciiFont[u8offset][u8Inc];
//font data(7..0) --> sending (MAX:35)
		for( u8Inc_1 = 7; u8Inc_1 < 255; u8Inc_1-- )// going to scan bit 7 to bit 0
		{
			//35 - number of bits in a char
			if( u8Count++ == NUM_CHAR_BITS )
			{	//0 or 1 - B or A char
				u8Count = 0;							//reset for row B
				u8CharNum = 2;						   //20 - offset in array for row B data
				u8offset = getBitmapOffset( u8colNum + 40);		//(colNum + n*20 )
				goto start_sending_characterB;
			}
			if( bit_test(u8ByteToParse,u8Inc_1 ) )
			{
				VFD_Data(1);
			} else {
				VFD_Data(0);
			}
		//vfd_delay;
			VFD_Clock(1);
		//vfd_delay;
			VFD_Clock(0);
		//vfd_delay;
		}
	}
/********************************************************************/
start_sending_characterB:
//Null *23
	VFD_Data(0);
	for (i=0;i<23;i++) {
		//vfd_delay;
		VFD_Clock(1);
		//vfd_delay;
		VFD_Clock(0);
		//vfd_delay;
	}
	//Row D,C,B,A
	//5 - number of bytes needed for each char
	for( u8Inc = 0; u8Inc < NUM_CHAR_BYTES; u8Inc++ )
	{
		//u8dim is 4 to 21. u8ByteCount is 1 to 20. If u8dim == 21( off )
		// display will never turn on.
//dimmer
		if( u8dim == ++u8ByteCount )
		{
			// turn display on
		if(VFD_scanStop == 0) {
		    VFD_Bk(0);
		}
		}
		//20 - offset in array for row A data
//font table(0..4) => font data reading
//		if( shiftDatIdx[u8colNum] < C_0_CHARS )
//		{
//			u8ByteToParse = c_0[u8offset++];
//		} else {
//			u8ByteToParse = c_1[u8offset++];
//		}
		u8ByteToParse = charAasciiFont[u8offset][u8Inc];
//font data(7..0) --> sending (MAX:35)
		for( u8Inc_1 = 7; u8Inc_1 < 255; u8Inc_1-- )// going to scan bit 7 to bit 0
		{
			//35 - number of bits in a char
			if( u8Count++ == NUM_CHAR_BITS )
			{	//0 or 1 - B or A char
				u8Count = 0;							//reset for row A
				u8CharNum = 3;						   //20 - offset in array for row A data
				u8offset = getBitmapOffset( u8colNum + 60);		//(colNum + n*20 )
				goto start_sending_characterA;
			}
			if( bit_test(u8ByteToParse,u8Inc_1 ) )
			{
				VFD_Data(1);
			} else {
				VFD_Data(0);
			}
		//vfd_delay;
			VFD_Clock(1);
		//vfd_delay;
			VFD_Clock(0);
		//vfd_delay;
		}
	}
/********************************************************************/
start_sending_characterA:
//Null *3
	VFD_Data(0);
	for (i=0;i<3;i++) {
		//vfd_delay;
		VFD_Clock(1);
		//vfd_delay;
		VFD_Clock(0);
		//vfd_delay;
	}
	//Row D,C,B,A
	//5 - number of bytes needed for each char
	for( u8Inc = 0; u8Inc < NUM_CHAR_BYTES; u8Inc++ )
	{
		//[4x20] u8dim is 4 to 21. u8ByteCount is 1 to 20. If u8dim == 21( off )
		//[2x20] u8dim is 4 to 11. u8ByteCount is 1 to 10. If u8dim == 11( off )
		// display will never turn on.
//dimmer
		if( u8dim == ++u8ByteCount )
		{
			// turn display on
		if(VFD_scanStop == 0) {
		    VFD_Bk(0);
		}
		}
		//20 - offset in array for row A data
//font table(0..4) => font data reading
//		if( shiftDatIdx[u8colNum] < C_0_CHARS )
//		{
//			u8ByteToParse = c_0[u8offset++];
//		} else {
//			u8ByteToParse = c_1[u8offset++];
//		}
		u8ByteToParse = charAasciiFont[u8offset][u8Inc];
//font data(7..0) --> sending (MAX:35)
		for( u8Inc_1 = 7; u8Inc_1 < 255; u8Inc_1-- )// going to scan bit 7 to bit 0
		{
			//35 - number of bits in a char
			if( u8Count++ == NUM_CHAR_BITS )
			{	//0 or 1 - B or A char
				goto stop_sending_character;
			}
			if( bit_test(u8ByteToParse,u8Inc_1 ) )
			{
				VFD_Data(1);
			} else {
				VFD_Data(0);
			}
		//vfd_delay;
			VFD_Clock(1);
		//vfd_delay;
			VFD_Clock(0);
		//vfd_delay;
		}
	}
stop_sending_character:
	VFD_Bk(1);			// turn display off
		vfd_delay;
	VFD_Latch(1);		// latch the data - start
		vfd_delay;
  	VFD_Latch(0);		// latch the data - end
		vfd_delay;
	VFD_Data(0);
		//vfd_delay;
}
/********************************************************************************
 * Init Function.
 *
 * set_tris_x sets up the direction a particular pin.
 * A 1 sets the pin up to be an input.
 * A 0 sets the pin up to be an output.
 * This is done one port at a time.
 *
 * @param
 *    None
 *
 * @retval
 *    None
 *******************************************************************************/
Uint8 getBitmapOffset( Uint8 u8ArrayOffset )
{
	Uint8 u8Offset;
/*
	 // ASCII values 32 to 82 are in C_0[], 83 to 122 are in C_1[]
	 // To save RAM, there are no bounds checking - must be tested thoroughly
	if( shiftDatIdx[u8ArrayOffset] < C_0_CHARS ) // bitmap is in C_0[]
	{
		u8Offset = ( ( shiftDatIdx[u8ArrayOffset] - ASCII_TO_C0_OFFSET ) * NUM_CHAR_BYTES );
	}
	else // bitmap is in C_1[]
	{
		u8Offset = ( ( shiftDatIdx[u8ArrayOffset] - ASCII_TO_C1_OFFSET ) * NUM_CHAR_BYTES );
	}
*/
//20100617
	if ((shiftDatIdx[u8ArrayOffset] < 0x20) || (shiftDatIdx[u8ArrayOffset] > 0x96))
	{
		return( 0x00 );			//blank ( 0x20 - 0x20 )
	}else{
		return( ( shiftDatIdx[u8ArrayOffset] - 0x20 ) );
	}
}


//-------------------------------------------------------------------------------
/* end of file */

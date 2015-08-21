/*****************************************************************************
******************************************************************************
**                                                                          **
**  Copyright (c) 2005 Videon Central, Inc.                                 **
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
 *    rotelvfd.h
 *
 * Header file to accompany rotelvfd.c
 *
 *
 * @author
 *    Chuck Etzweiler
 *
  */
#ifndef _ROTELVFD_H_
#define _ROTELVFD_H_


// version information
/* msb set to indicate version - lower 7 bits is the version number x10	*/
/* Example A = 1.0	*/
#define VER_NO 				0x0A

// bitmap stuff
#define MAX_COLS       	   20 	// maximum number of columns
#define MAX_CHARS      	   40 	// maximum number of characters     //20120807
#define MAX_ROWS       	    2  	// maximum number of rows
#define ROW_B		   	    0
#define ROW_A		   	    1
#define NUM_CHAR_BYTES 	    5  	// number of bytes in a character
#define NUM_CHAR_BITS  	   35 	// number of bits in a character
#define NUM_BYTE_BITS  	    8  	// number of bits in a byte
#define ARRAY_A_OFFSET	   20 	// offset between B info. and A info.
#define ASCII_TO_C0_OFFSET 32
#define ASCII_TO_C1_OFFSET 83
#define	C_0_CHARS  	   	   83 	// number of char bitmaps in C_0[]

#define WELCOME_DELAY		9	// how long to keep welcome display up
#define DIM_DAFAULT			3
#define DIPLAY_OFF			21

// this is the position of the offsets in the array
// example: C0_B is Column 0 row B
#define C0_B   0
#define C1_B   1
#define C2_B   2
#define C3_B   3
#define C4_B   4
#define C5_B   5
#define C6_B   6
#define C7_B   7
#define C8_B   8
#define C9_B   9
#define C10_B 10
#define C11_B 11
#define C12_B 12
#define C13_B 13
#define C14_B 14
#define C15_B 15
#define C16_B 16
#define C17_B 17
#define C18_B 18
#define C19_B 19

#define C0_A  20
#define C1_A  21
#define C2_A  22
#define C3_A  23
#define C4_A  24
#define C5_A  25
#define C6_A  26
#define C7_A  27
#define C8_A  28
#define C9_A  29
#define C10_A 30
#define C11_A 31
#define C12_A 32
#define C13_A 33
#define C14_A 34
#define C15_A 35
#define C16_A 36
#define C17_A 37
#define C18_A 38
#define C19_A 39

/*-----------------------------------------------------------------*/
//#define BK					P9L.7
//#define CLK					P9H.1	//P9.9
//#define SI					P9H.0	//P9.8
//#define LAT					P9H.5	//P9.13

#define output_high(port)	port = 1
#define output_low(port)	port = 0
#define bit_test(a,b) 		(((a>>b)&0x01)==1)
//#define vfd_delay				for(i=0;i<10;i++) {}
#define vfd_delay				__asm(" nop" )
/*-----------------------------------------------------------------*/
// special character
#define SC_invert_T		0x80	//title
#define SC_CD_pause		0x81	//CD pause
#define SC_CD_play		0x82	//CD play
#define SC_CD_stop		0x83	//CD stop
#define SC_invert_C		0x84	//chapter
#define SC_invert_A		0x85	//angle
#define SC_invert_Z		0x86	//zoom
#define SC_invert_G		0x87	//group
#define SC_invert_RP	0x88	//repeat
#define SC_invert_L		0x89	//last
#define SC_invert_Dl	0x8a	//Dolby DD(left)
#define SC_invert_Dr	0x8b	//Dolby DD(right)
#define SC_invert_R		0x8c	//remain
#define SC_invert_Z1	0x8d	//zoom						//????
#define SC_invert_I		0x8e	//intro scan
#define SC_invert_S		0x8f	//shuffle, random

#define SC_irregular	0x90	//irregular character *
#define SC_allDotOn		0x91	//
#define SC_invert_F		0x92	//File
#define SC_invert_M		0x93	//Music
#define SC_invert_play	0x94	//invert PLAY(<)
/*-----------------------------------------------------------------*/

 /* this is the array that stores the ascii chars to be displayed.
    the first 20 bytes are the data for row B.
    the next 20 bytes are the data for row A. */
extern Uint8 shiftDatIdx[MAX_CHARS];
extern Uint8 u8dim,dim_back;		// for display dim function
// variable to store the length of data that is coming in over rs232
extern Uint8 u8MsgLen;
extern bool	fGotFirstPacket;		// true after first unsol. message

void VFDClearDisplay(void);
void VFD_Init(void);
void vfd_processing(void);
void sendDataVFD(Uint8 u8colNum);


#endif	//_ROTELVFD_H_


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
 *    stn_module.h
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

//#define	ROTEL_VFD_2x20		//[4x20] or [2x20] and dimmerRange[1..20] or [1..10] => dimmer Table 변경해세요..

/* TC2002A key attribute */
#define	MODE_CLEAR_DISPLAY			0x00
#define	MODE_RETURN_HOME			0x01

#define	MODE_ENTRY_MODESET			0x02
#define	MODE_ENTRY_MODESET_DEC			0x00
#define	MODE_ENTRY_MODESET_INC			0x02

#define	MODE_DISPLAY_ONOFF			0x03
#define	MODE_DISPLAY_ONOFF_ON			0x04
#define	MODE_DISPLAY_ONOFF_OFF			0x00

#define MODE_CURSOR_DISPLAYSHIFT	0x04

#define	MODE_FUNCTION_SET			0x05
#define	MODE_FUNCTION_SET_8BIT			0x10
#define	MODE_FUNCTION_SET_1LINE			0x00
#define	MODE_FUNCTION_SET_2LINE			0x08
#define	MODE_FUNCTION_SET_5X8DOT		0x00

#define	MODE_SET_CGRAM_ADDR			0x06
#define	MODE_SET_DDRAM_ADDR			0x07
#define	MODE_READ_BF_AC				0x08
#define	MODE_WRITE_DATATORAM		0x09
#define	MODE_READ_DATAFROMRAM		0x0a

// STN : special character table
#define STN_allDotOn		0x07	//all dots on
#define STN_CD_pause		0x08	//CD pause
#define STN_CD_stop			0x09	//CD stop
#define STN_invert_T		0x0a	//title
#define STN_invert_G		0x0b	//group
#define STN_invert_I		0x0c	//intro
#define STN_invert_Dl		0x0d	//Dolby DD(left)
#define STN_invert_Dr		0x0e	//Dolby DD(right)
#define STN_invert_RP		0x0f	//repeat
//---------------------------------------------------------
// RotelSpecialCharacter [ 0x80~0x96 ]
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
#define SC_arrow_right	0x95	//->
#define SC_arrow_left	0x96	//<-
//---------------------------------------------------------
#define	M_STARTUP					0x00
#define	M_STARTUP1					0x01
#define	M_STARTUP2					0x02
#define	M_STARTUP3					0x03
#define	M_SPECIALCHAR_TX			0x04
#define	M_CONFIGURE_OK				0x05
#define	M_STANDBY_ON				0x06

#endif	//_ROTELVFD_H_

void TC2002A_AcOnInit(void);
void TC2002A_StartUpCheck(void);
void TC2002A_DisplayDataTx(void);
void TC2002A_Tx( char data, char data1);
char TC2002A_ReadToRAM_data(void);
void TC2002A_CharacterFontRead(unsigned char step);
char TC2002A_Read_BF_AC(void);
void TC2002A_DDRamDataRead(void);
void TC2002A_DisplayDataTx1L(void);
void TC2002A_DisplayDataTx2L(void);
void TC2002A_DisplayDataTx3L(void);
void TC2002A_DisplayDataTx4L(void);

void TC2004A_DisplayDataTx(void);
void TC2002A_PortInit(void);
void TC2002A_ReturnHome(void);

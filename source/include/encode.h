/**************** (c)2008 Rotel Technology Global Inc..*************************
PROJECT         : Front Panel for Large VFD
COMPILER        : IAR Embeded Workbench 4.4.1
MODULE          : encode.h
VERSION         : V 1.0
USED CHIP       : stm32f10x
CREATION DATE   : /2008
AUTHORS         : An Kyung Chan / RTG
DESCRIPTION     :
*******************************************************************************/

#ifndef __ENCODE_H__
#define __ENCODE_H__

#define F_ENC_UP              1
#define F_ENC_DOWN            0
#define F_ON                  1
#define F_OFF                 0


/*----------------------------------------------------------------------------*/
void enc_EncoderInit(void);
void enc_EncoderChekPer2ms(void);
void Clear_EncValue(void);

#endif	//__ENCODE_H__

/* end of file */
/******************* (C) COPYRIGHT 2008 Rotel Technology Global **************/

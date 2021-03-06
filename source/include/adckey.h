/**************** (c)2012 Rotel ************************************************
PROJECT         : RKB
COMPILER        : IAR Embeded Workbench 5.50
MODULE          : adc.h
VERSION         : V 1.0
USED CHIP       : stm32f10x
CREATION DATE   : /2013
AUTHORS         : /Rotel
DESCRIPTION     :
*******************************************************************************/

#ifndef __ADC_H__
#define __ADC_H__

typedef enum {
	SCANNOS=4,
	CHNOS=4,            //4+4 Line
	NODES=4             //7 key
} ChNo_s;
typedef enum {
	KM_READY,
	KM_HOLD,
	KM_KEYOFF
}Kmode_s;

typedef	struct Key_t{
	byte KeyScan[SCANNOS];
	byte KeyImsi[SCANNOS];
	byte KeyBuf[SCANNOS];
	byte KeyData[SCANNOS];
	byte KeyHoldTime[SCANNOS];
	int  KeyRepeatTime[SCANNOS];
	byte KeyMulti;
	byte KeyMode[SCANNOS];
	byte KeyOk[SCANNOS];
	byte_flags KeyStatus1;
	byte_flags KeyStatus2;
	byte_flags KeyStatus3;
	byte_flags KeyStatus4;
	//byte_flags KeyStatus5;
	//byte_flags KeyStatus6;
} Key_t;

#define f_KeyRefresh1	key.KeyStatus1.byte_bit.bit1
#define f_KeyRepeatOk1	key.KeyStatus1.byte_bit.bit2
#define f_KeyRepeatly1	key.KeyStatus1.byte_bit.bit3
#define f_KeyReleased1	key.KeyStatus1.byte_bit.bit4
//
#define f_KeyRefresh2	key.KeyStatus2.byte_bit.bit1
#define f_KeyRepeatOk2	key.KeyStatus2.byte_bit.bit2
#define f_KeyRepeatly2	key.KeyStatus2.byte_bit.bit3
#define f_KeyReleased2	key.KeyStatus2.byte_bit.bit4
//
#define f_KeyRefresh3	key.KeyStatus3.byte_bit.bit1
#define f_KeyRepeatOk3	key.KeyStatus3.byte_bit.bit2
#define f_KeyRepeatly3	key.KeyStatus3.byte_bit.bit3
#define f_KeyReleased3	key.KeyStatus3.byte_bit.bit4
//
#define f_KeyRefresh4	key.KeyStatus4.byte_bit.bit1
#define f_KeyRepeatOk4	key.KeyStatus4.byte_bit.bit2
#define f_KeyRepeatly4	key.KeyStatus4.byte_bit.bit3
#define f_KeyReleased4	key.KeyStatus4.byte_bit.bit4
//
#define f_KeyRefresh5	key.KeyStatus5.byte_bit.bit1
#define f_KeyRepeatOk5	key.KeyStatus5.byte_bit.bit2
#define f_KeyRepeatly5	key.KeyStatus5.byte_bit.bit3
#define f_KeyReleased5	key.KeyStatus5.byte_bit.bit4
//
#define f_KeyRefresh6	key.KeyStatus6.byte_bit.bit1
#define f_KeyRepeatOk6	key.KeyStatus6.byte_bit.bit2
#define f_KeyRepeatly6	key.KeyStatus6.byte_bit.bit3
#define f_KeyReleased6	key.KeyStatus6.byte_bit.bit4
//




#define FRONT_REPEAT          0x10
#define FRONT_LONG            0x20
#define FRONT_SPECIAL         0x30

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
#define STM_POWERON			0x25
#define STM_POWEROFF		0x26
//
#define STM_FM_SEEK_UP		0x27			//20090702 : new
#define STM_FM_SEEK_DOWN	0x28
#define STM_FM_STEP_UP		0x29
#define STM_FM_STEP_DOWN	0x2a
//
#define STM_LAST			0x2b
//
#define STM_MEMORY			0x7e

extern unsigned char current_key[4];
extern vu16 ADC_Val[4];
extern vu16 current_ADC_Val[4];
/*----------------------------------------------------------------------------*/
void adc_KeyRestart(unsigned char i);							//20130527
void adc_KeyInit(void);
void adc_DMA1_Channel1_IRQHandler(void);
void adc_KeyBoardPer10ms(void);
//void adc_KeyRead(void);
//void adc_KeyMake(void);
void adc_KeyRead(unsigned char Lines);
void adc_KeyMake(unsigned char Lines);
void mode_temperature_check(void);
void temperature_read(void);

void DMA2_Stream1_IRQHandler(void);	

#endif	//__ADC_H__

/* end of file */
/******************* (C) COPYRIGHT 2008 Rotel Technology Global **************/

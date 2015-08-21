/**************** (c)2012 Rotel ************************************************
PROJECT         : RA1570
COMPILER        : IAR Embeded Workbench 5.50
MODULE          : main.c
VERSION         : V 1.0
USED CHIP       : stm32f101VC
CREATION DATE   : /2012
AUTHORS         : /Rotel
DESCRIPTION     :
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "initmcu.h"
#include "uart.h"
#include "main.h"
#include "timer.h"
#include "adckey.h"
#include "adckey.ext"
#include "encode.h"
#include "encode.ext"
#include "eeprom.h"


#include "proc_function.h"
#include "dac.h"

#include "function.ext"
#include "remocon.h"
#include "remocon.ext"

#include "proc_display.ext"
#include "proc_display.h"

#include "ak4114.ext"
#include "ak4114.h"

#include "rotelvfd.h"

#include "RA_function.h"

#ifdef IPOD_INCLUDE
#include "iPod_config.h"
#endif

/*******************************************************************************
		switch(mode_func){
            //------------------------------------------------------------
            case FUNCTION_IPOD:
                break;
            case FUNCTION_CD:
                break;
            case FUNCTION_COAX1:
            case FUNCTION_COAX2:
            case FUNCTION_OPT1:
            case FUNCTION_OPT2:
            case FUNCTION_PHONO:
            case FUNCTION_balXLR:
            case FUNCTION_TUNER:
            case FUNCTION_AUX1:
            case FUNCTION_AUX2:
            case FUNCTION_PCUSB:
                break;
            case FUNCTION_DISP_OFF:	
            default:
                break;
		}
*******************************************************************************/
#ifdef skip_skip
/* 	
*************************************************/
void 	AK4104_init(void)
{
	AK4104_nReset(0);
	AK4104_cs(0);			//사용안함.
	AK4104_clock(0);		//사용안함.
	AK4104_data(0);			//사용안함.

	AK4104_nReset(1);
}
#endif
#ifdef skip_skip
/*

sw1,2,3,4,5,6,7,8,Rch,Lch,c0,c1,c2,c3

sw1=> DAC output => 1000 0000
sw2=> Aux analog => 0100 0000
c0,1,2,3 => 0000 (TC9162)

(14bit)
data _||||||||_||_||||_
ck   _||||||||_||_||||_
stb  _________________|_

          (0.5usec)
_________|----------|_________
(0.5usec)            (0.5usec)
*************************************************/
void TC9162_delay(void)
{
	unsigned char i;
//	for (i=0;i<1;++i) {};		//1us
	for (i=0;i<3;++i) {};		//2us
//	for (i=0;i< 20;++i) {};		//6us
}

void TC9162_Tx(unsigned char t_data)
{
	unsigned char i;
	
	TC9162_STB(0);
	TC9162_Clock(0);
	TC9162_Data(0);

	for(i=0;i<8;i++){
		TC9162_Clock(0);
		TC9162_delay();
		if((t_data & (0x80 >> i)) == 0){
			TC9162_Data(0);
		} else {
			TC9162_Data(1);
		}
		TC9162_delay();
		TC9162_Clock(1);
		TC9162_delay();
	}
	TC9162_Clock(0);

//Rch,Lch,0000 (6bit)
	t_data = 0xc0;			
	for(i=0;i<6;i++){
		TC9162_Clock(0);
		TC9162_delay();
		if((t_data & (0x80 >> i)) == 0){
			TC9162_Data(0);
		} else {
			TC9162_Data(1);
		}
		TC9162_delay();
		TC9162_Clock(1);
		TC9162_delay();
	}
	TC9162_Clock(0);
	TC9162_delay();
	TC9162_STB(1);
	TC9162_delay();
	TC9162_STB(0);
	TC9162_delay();
}
#endif
#ifdef skip_skip
/*
[MSB first]
L1,2,3,4,5,6,7,0		: Left CH volume data
R1,2,3,4,5,6,7,0		: Right CH volume data
A,B,C,0					: A=HP, B=AMP, C=PreAMP
S0,S1,00				: S0:S1 (up=01)(down=10)
0000					: dummy
CS1,2,3,4 => 0001		: CS1=CS2=Low

[LSB first]
0,L7~L1					: Left CH volume data
0,R7~R1					: Right CH volume data
00,S1,S0,0,C,B,A		: A=HP, B=AMP, C=PreAMP
CS4~1,0000				: CS4:CS3=10,CS2:CS1=00 + dummy

(32 bit)
data _||||||||_||||||||_||||||||_||||||||_
ck   _||||||||_||||||||_||||||||_||||||||_
stb  ____________________________________|_

          (0.5usec)
_________|----------|_________
(0.5usec)            (0.5usec)
*************************************************/
void TC9482_delay(void)
{
	unsigned char i;
//	for (i=0;i<1;++i) {};		//1us
	for (i=0;i<3;++i) {};		//2us
//	for (i=0;i< 20;++i) {};		//6us
}
void TC9482_8Tx_LSB(unsigned char t_data)
{
	unsigned char i;

	for(i=0;i<8;i++){
		TC9482_Clock(0);
		TC9482_delay();
		if((t_data & (0x01 << i)) == 0){
			TC9482_Data(0);
		} else {
			TC9482_Data(1);
		}
		TC9482_delay();
		TC9482_Clock(1);
		TC9482_delay();
	}
	TC9482_Clock(0);
}
#endif
#ifdef skip_skip
/*
	HP_volume[96..0]		
	AMP_volume[96..0]
	PreAMP_volume[96..0]

	HP_CH			= 0000 0001
	AMP_CH			= 0000 0010
	PreAMP_CH		= 0000 0100

	directionUp		= 0010 0000
	directionDown	= 0001 0000

	display(96)	: 96 - (volume) => 96-96 	=>0 	=>0db
	display(95)	: 96 - (volume) => 96-95 	=>1 	=>-1db
	display(1)	: 96 - (volume) => 96-1 	=>95 	=>-95db
	display(0)	: 96 - (volume) => 96-0 	=>96 	=>mute
*************************************************/
void TC9482_Tx(unsigned char v_data, unsigned char ch, unsigned char direction)
{
	TC9482_STB(0);
	TC9482_Clock(0);
	TC9482_Data(0);

	TC9482_8Tx_LSB(96-v_data);
	TC9482_8Tx_LSB(96-v_data);
	TC9482_8Tx_LSB(ch | direction);
	TC9482_8Tx_LSB(0x80);

	TC9482_delay();
	TC9482_STB(1);
	TC9482_delay();
	TC9482_STB(0);
	TC9482_delay();
}
#endif
//==============================================================================
/*
RA RS232 key
	AMP_volume[0 => 96]
*********************************************************/
void	Func_VolUp(void)
{
	switch(mode_PowerStatus) {
		case POWER_ON0:			
			if (AMP_volume < MAX_MASTER_VOLUME) {
				++AMP_volume;
			}
			if (AMP_volume > MAX_MASTER_VOLUME) {
				AMP_volume = MAX_MASTER_VOLUME;			//Max
			}
			
			Set_MainVolume();

			if(mute_mode == MUTE_ON) {
				mute_mode = MUTE_OFF;
			 	Mute_AllMuteOff();
#ifdef ASCiiKEY
				Func_MuteStatus(mute_mode);  //20130316
#endif
			}
            //volume display : temp / main
            //20120906
            //if ( (mode_DispBlockC == BLOCKC_NORMAL) || (mode_DispBlockC == BLOCKC_SETUP) ) {
            //20121128
            if (mode_DispBlockC == BLOCKC_SETUP) {
            }else{
			    mode_setup = 0;
			    Set_DisplayBlockCTimer(BLOCKC_TEMP_VOLUME,COUNT100M_5SEC);
			}

#ifdef ASCiiKEY		//20110408
		    Func_VolumeCurrentStatus();
#endif	
			break;
		case POWER_ON1_ING:
		case POWER_ON2_ING:
		case POWER_ON3_ING:
		case POWER_OFF:			
		default:
			break;
	}
}
/*
RA RS232 key
	AMP_volume[0 <= 96]
*********************************************************/
void	Func_VolDown(void)
{
	switch(mode_PowerStatus) {
		case POWER_ON0:			
			if (AMP_volume > 0) {
				--AMP_volume;
			}
			if (AMP_volume > MAX_MASTER_VOLUME) {
				AMP_volume = 0;			//Min
			}
			
			Set_MainVolume();

			if(mute_mode == MUTE_ON) {
				mute_mode = MUTE_OFF;
			 	Mute_AllMuteOff();
#ifdef ASCiiKEY
				Func_MuteStatus(mute_mode);  //20130316
#endif
			}
            //volume display : temp / main
            //20120906
            //if ( (mode_DispBlockC == BLOCKC_NORMAL) || (mode_DispBlockC == BLOCKC_SETUP) ) {
            //20121128
            if (mode_DispBlockC == BLOCKC_SETUP) {
            }else{
			    mode_setup = 0;
			    Set_DisplayBlockCTimer(BLOCKC_TEMP_VOLUME,COUNT100M_5SEC);
			}

#ifdef ASCiiKEY		//20110408
		    Func_VolumeCurrentStatus();
#endif	
			break;
		case POWER_ON1_ING:
		case POWER_ON2_ING:
		case POWER_ON3_ING:
		case POWER_OFF:			
		default:
			break;
	}
}
/*
RA
*********************************************************/
void	Func_Mute(void)
{
		switch(mode_func){
            case FUNCTION_IPOD:
            case FUNCTION_CD:
            case FUNCTION_COAX1:
            case FUNCTION_COAX2:
            case FUNCTION_OPT1:
            case FUNCTION_OPT2:
            case FUNCTION_PHONO:
            case FUNCTION_balXLR:
            case FUNCTION_TUNER:
            case FUNCTION_AUX1:
            case FUNCTION_AUX2:
            case FUNCTION_PCUSB:
				switch(mute_mode){
					case MUTE_ON:
					    mute_mode = MUTE_OFF;
					    Set_DisplayBlockCTimer(BLOCKC_NORMAL,0);
			 		    Mute_AllMuteOff();
						break;
					case MUTE_OFF:	
					    mode_setup = 0;					//20091117
					    mute_mode = MUTE_ON;
					    Set_DisplayBlockCTimer(BLOCKC_NORMAL,0);
					    b1_CommTimer_250msec = 0;		//blinking reset
					    Mute_AllMuteOn();
						break;
					default:
						break;
			    }
#ifdef ASCiiKEY
                    Func_MuteStatus(mute_mode);  //20110329
#endif
                break;
            case FUNCTION_DISP_OFF:	
            default:
                break;
		}
}
/*
RA
*********************************************************/
void	Func_MuteOn(void)
{
		switch(mode_func){
            case FUNCTION_IPOD:
            case FUNCTION_CD:
            case FUNCTION_COAX1:
            case FUNCTION_COAX2:
            case FUNCTION_OPT1:
            case FUNCTION_OPT2:
            case FUNCTION_PHONO:
            case FUNCTION_balXLR:
            case FUNCTION_TUNER:
            case FUNCTION_AUX1:
            case FUNCTION_AUX2:
            case FUNCTION_PCUSB:
				switch(mute_mode){
					case MUTE_ON:
						break;
					case MUTE_OFF:	
						mode_setup = 0;					//20091117
						mute_mode = MUTE_ON;
						Set_DisplayBlockCTimer(BLOCKC_NORMAL,0);
						b1_CommTimer_250msec = 0;		//blinking reset
						Mute_AllMuteOn();
						break;
					default:
						break;
				}
#ifdef ASCiiKEY
                    Func_MuteStatus(mute_mode);  //20110329
#endif
                break;
            case FUNCTION_DISP_OFF:	
            default:
                break;
		}
}
/*
RCX
*********************************************************/
void	Func_MuteOff(void)
{
		switch(mode_func){
            case FUNCTION_IPOD:
            case FUNCTION_CD:
            case FUNCTION_COAX1:
            case FUNCTION_COAX2:
            case FUNCTION_OPT1:
            case FUNCTION_OPT2:
            case FUNCTION_PHONO:
            case FUNCTION_balXLR:
            case FUNCTION_TUNER:
            case FUNCTION_AUX1:
            case FUNCTION_AUX2:
            case FUNCTION_PCUSB:
				switch(mute_mode){
					case MUTE_ON:
						mute_mode = MUTE_OFF;
						Set_DisplayBlockCTimer(BLOCKC_NORMAL,0);
				 		Mute_AllMuteOff();
						break;
					case MUTE_OFF:	
						break;
					default:
						break;
				}
#ifdef ASCiiKEY
                    Func_MuteStatus(mute_mode);  //20110329
#endif
                break;
            case FUNCTION_DISP_OFF:	
            default:
                break;
		}
}
//
/*******************************************************************************
    AMP_volume      [0 ~ 96]
    mode_bypass = mode_TempBypass       [0,1]
	bass_volume = bass_TempVolume       [0 ~ 10 ~ 20]	
	treble_volume = treble_TempVolume   [0 ~ 10 ~ 20]	
	balance_volume = balance_TempVolume [0 ~ 15 ~ 30]	

*******************************************************************************/
/*
    range : L15 ~ R15
    step : 0.5dB
*******************************************************************************/
const char balanceLevelTable15[12][15] = {                      //20121101 : suda simple table
	{1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,14,16,0},       //0?
	{1, 2, 3, 4, 5, 6, 8,10,12,16,20,24,28,32,0},       //1?
	{1, 2, 3, 4, 5, 6, 8,10,12,16,20,24,28,32,0},       //2?
    {1, 2, 3, 4, 5, 6, 8,10,12,16,20,24,28,32,0},       //3?
    {1, 2, 3, 4, 5, 6, 8,10,12,16,20,24,28,32,0},       //4?
    {1, 2, 3, 4, 5, 6, 8,10,12,16,20,24,28,32,0},       //5?
    {1, 2, 3, 4, 5, 6, 8,10,12,16,20,24,28,32,0},       //6?
    {1, 2, 3, 4, 5, 6, 8,10,12,16,20,24,28,32,0},       //7?
    {1, 2, 3, 4, 5, 6, 8,10,12,16,20,24,28,32,0},       //8?
    {1, 2, 3, 4, 5, 6, 8,10,12,16,20,24,28,32,0},       //9?
    {1, 2, 3, 4, 5, 6, 8,10,12,16,20,24,28,32,0},       //10?
    {1, 2, 3, 4, 5, 6, 8,10,12,16,20,24,28,32,0}};      //11?

const char balanceLevelTable151[12][15] = {                      //20121101
	{1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,14,16,0},      //0?
	{1, 2, 3, 4, 5, 6, 8,10,12,16,20,24,28,32,0},      //1?
	{1, 2, 3, 4, 5, 6, 8,10,12,16,20,28,36,44,0},      //2?
    {1, 2, 3, 4, 5, 6, 8,10,12,16,20,28,46,60,0},      //3?
    {1, 2, 3, 4, 5, 6, 8,10,12,16,20,28,48,76,0},      //4?
    {1, 2, 3, 4, 5, 6, 8,10,12,16,32,52,72,92,0},      //5?
    {1, 2, 3, 4, 5, 6, 8,10,12,16,32,60,88,108,0},     //6?
    {1, 2, 3, 4, 5, 6, 8,10,12,20,40,64,96,124,0},     //7?
    {1, 2, 3, 4, 5, 6, 8,12,20,40,64,96,120,144,0},    //8?
    {1, 2, 3, 4, 5, 6, 8,12,24,48,68,96,136,168,0},    //9?
    {1, 2, 3, 4, 5, 6, 8,12,24,48,68,108,136,168,0},   //10?
    {1, 2, 3, 4, 5, 6, 8,12,24,48,68,108,136,168,0}};  //11?
/*
const char balanceLevelTable15[12][15] = {
	{1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 8,0},		//0?		
	{1, 1, 2, 2, 3, 3, 4, 4, 6, 8,10,12,14,16,0},		//1?		
	{1, 1, 2, 2, 3, 4, 5, 6, 8,10,12,14,18,22,0},		//2?		
	{1, 1, 2, 2, 3, 4, 5, 6, 8,10,12,16,23,30,0},		//3?		
	{1, 1, 2, 2, 3, 4, 5, 6, 8,10,12,16,28,38,0},		//4?		
	{1, 1, 2, 2, 3, 4, 5, 6, 8,10,16,26,36,46,0},		//5?		
	{1, 1, 2, 2, 3, 4, 5, 6, 8,10,18,30,44,54,0},		//6?		
	{1, 1, 2, 2, 3, 4, 5, 6, 8,10,20,32,48,62,0},		//7?		
	{1, 1, 2, 2, 3, 4, 6, 8,14,20,32,48,60,72,0},		//8?		
	{1, 1, 2, 2, 3, 4, 6, 8,14,24,34,48,68,84,0},		//9?
	{1, 1, 2, 2, 3, 4, 6, 8,14,24,34,54,68,84,0},		//10?
	{1, 1, 2, 2, 3, 4, 6, 8,14,24,34,54,68,84,0}};		//11? */
/*
    range : L17 ~ R17
    step : 0.5dB
*******************************************************************************/
const byte balanceLevelTable17[12][17] = {
	{1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,0},		//0?		
	{1, 2, 3, 4, 5, 6, 7, 8,10,12,14,16,20,24,28,32,0},		//1?		
	{1, 2, 3, 4, 6, 8,10,12,16,20,24,28,32,36,40,44,0},		//2?		
	{1, 2, 3, 4, 6, 8,10,12,16,20,24,28,32,36,48,60,0},		//3?		
	{1, 2, 3, 4, 6, 8,10,12,16,20,24,28,40,52,64,76,0},		//4?		
	{1, 2, 3, 4, 6, 8,10,12,16,20,24,28,40,52,72,92,0},		//5?		
	{1, 2, 3, 4, 6, 8,10,12,16,20,32,44,56,68,88,108,0},	//6?		
	{1, 2, 3, 4, 6, 8,10,12,16,20,32,44,64,84,104,124,0},	//7?		
	{1, 2, 3, 4, 6, 8,12,16,28,40,52,64,84,104,124,144,0},	//8?		
	{1, 2, 3, 4, 6, 8,12,16,28,48,68,88,108,128,148,168,0},	//9?
	{1, 2, 3, 4, 6, 8,12,16,28,48,68,88,108,128,148,168,0},	//10?
	{1, 2, 3, 4, 6, 8,12,16,28,48,68,88,108,128,148,168,0}};//11?
//----------------------------------------------------------------------------------------
const char tbl_RmcVolSpeedValue[23] = { 0,1,0,1,0,1,0,1,0,2,0,2,0,2,0,2,0,2,0,3,0,3,3 };	//20070307
//----------------------------------------------------------------------------------------
unsigned char Get_RmcVol_Adj(unsigned char data)
{
	return(tbl_RmcVolSpeedValue[data]);
}
//----------------------------------------------------------------------------------------
const char tbl_VolSpeedValue[3] = { 1,2,3 };
unsigned char get_vol_speed(void)
{
	return(tbl_VolSpeedValue[0]);
}
/*

20121031 repair
    out : volume Tx data
    

******************************************************************************************/
void	Balance_channel_calculate(unsigned char vol, unsigned char *L_ch, unsigned char *R_ch)
{
	unsigned char total,step,balance_leavl_max;
	unsigned char vol2;
	
	//
    //make volume data : (vol*2)-(balance) : 20121031
	//

#ifdef RA1570		//20121102
    vol2 = vol*2;
    balance_leavl_max = 30;
	step = 10;	//1db step
#else
    balance_leavl_max = 34;
	step = 10*2;	//0.5db step
#endif
    //
	//balance channel calculate
	//
	if (vol == 0) {
		//mute
		*L_ch = vol;
		*R_ch = vol;
	}else{
		if (balance_TempVolume == (balance_leavl_max/2) ) {
			//center
			*L_ch = vol2;
			*R_ch = vol2;
		}else if (balance_TempVolume > (balance_leavl_max/2) ) {
			//Left_channel
			total = balanceLevelTable15[vol/step][balance_TempVolume - (balance_leavl_max/2) - 1];          //Table refer
			if (total > 0) {
				if ( vol2 >= total ) {
					*L_ch = vol2 - total;
				}else{
					*L_ch = 0;  //mute
				}
			}else{
				*L_ch = 0;  //mute
			}
			*R_ch = vol2;
		}else{
			//Right_channel
			total = balanceLevelTable15[vol/step][ (balance_leavl_max/2) - balance_TempVolume - 1];         //Table refer
			if (total > 0) {
				if ( vol2 >= total ) {
					*R_ch = vol2 - total;
				}else{
					*R_ch = 0;  //mute
				}
			}else{
				*R_ch = 0;  //mute
			}
			*L_ch = vol2;
		}
	}
}
/*
//(mode_func) function status
#define	FUNCTION_DISP_OFF			0
#define	FUNCTION_COAX1     			1
#define	FUNCTION_COAX2     			2
#define	FUNCTION_OPT1     			3
#define	FUNCTION_OPT2     			4
#define	FUNCTION_PCUSB				5
#define	FUNCTION_IPOD				6
#define	FUNCTION_PHONO  			7
#define	FUNCTION_CD       			8
#define	FUNCTION_balXLR  			9
#define	FUNCTION_TUNER  			10
#define	FUNCTION_AUX1				11
#define	FUNCTION_AUX2				12
*/
//==============================================================================
void	Set_MainVolume(void)
{
	unsigned char vol, L_ch, R_ch;

    //----------------------------------------------------------------
    //checking Unity volume : AMP_volume
/*    //AUX1, COAX1, OPT1, PCUSB, iPod
    if ( (mode_func == FUNCTION_AUX1)||(mode_func == FUNCTION_COAX1)||(mode_func == FUNCTION_OPT1)||(mode_func == FUNCTION_PCUSB)||(mode_func == FUNCTION_IPOD) ) {
        if ( unity_volume[mode_func] == 0 ) {
            //unity volume disable
            vol = AMP_volume;
        }else{
            //enable unity volume ==> do not adjust VolumeData
            vol = unity_volume[mode_func];
        }
    }else{
        //remain function volume
        vol = AMP_volume;
    }*/
    //----------------------------------------------------------------
    //20120926
    vol = AMP_volume;
	//volume 0db, main mute on
	//
	if(vol == 0) {
		Mute_AllMuteOn();		/*=>io_sub */
	}else{
	    //when volume < 6 , goto mute on . (RCX1500)
		//if(vol < 6) {     //20121101 : direct volume 에서 mute 안풀리는 현상 발생....
			if(mute_mode == MUTE_OFF) {
				if(timer_MuteOff == 0) {
					Mute_SetMuteOffTime(1);		/*=>sub*/
				}
			}
		//}
	}
    //
    //make volume data : (vol*2)-(balance)
    //
	//balance ?
	Balance_channel_calculate(vol, &L_ch, &R_ch);
	//
#ifdef IC_DEBUG_x
	//rs232_printf("L_ch=> %x , R_ch=> %x\n",L_ch,R_ch);
    rs232_printf("VOLUME DATA ==================>\r",blank);
	rs232_printf("L_ch=> %x\n",L_ch);
	rs232_printf("R_ch=> %x\n",R_ch);
	rs232_printf("Vol => %x\n",vol);
#endif
    //
    //volume data *2    //(0.5dB*2)
    //
    //if (L_ch != 0) L_ch = L_ch*2;
    //if (R_ch != 0) R_ch = R_ch*2;
    //
    //volume data Tx
    //20121011
    //s_PGA2310_Tx16( R_ch, L_ch );
    s_PGA2310_Tx16( L_ch, R_ch );
}
//==============================================================================
unsigned char Get_UpVol_Adj(void)
{
	
#ifdef IC_DEBUG_x
		rs232_printf("adjUp=> %d",Adj_UpEncoder_Counter);
		rs232_printf("BadjUp=> %d\n",Backup_UpEncoder_TimeCounter);
#endif
	if (Adj_UpEncoder_Counter == 0) {			//20040825
		return(0);
	}
	if (Backup_UpEncoder_TimeCounter == 0xff) {
		return(1);
	}
	if (Adj_UpEncoder_Counter < 3) {						//20090819 : 1/2/3/4/5 => 1/1/2/2/3
		if (Backup_UpEncoder_TimeCounter > 40) {            //20121012 RA1570 : 1/1/2/2/3
			return((Adj_UpEncoder_Counter * 1));
		}else if (Backup_UpEncoder_TimeCounter > 30) {
			return((Adj_UpEncoder_Counter * 1));			//20090819 : 
		}else if (Backup_UpEncoder_TimeCounter > 20) {
			return((Adj_UpEncoder_Counter * 2));
		}else if (Backup_UpEncoder_TimeCounter > 10) {
			return((Adj_UpEncoder_Counter * 2));
		}else {
			return((Adj_UpEncoder_Counter * 3));
		}
	}else if (Adj_UpEncoder_Counter < 5) {	
		// 3/4
		if (Backup_UpEncoder_TimeCounter > 36) {
			return((Adj_UpEncoder_Counter * 1));
		}else if (Backup_UpEncoder_TimeCounter > 26) {
			return((Adj_UpEncoder_Counter * 2));
		}else {
			return((Adj_UpEncoder_Counter * 3));
		}
	}else if (Adj_UpEncoder_Counter < 7) {	
		// 5/6 
		if (Backup_UpEncoder_TimeCounter > 36) {
			return((Adj_UpEncoder_Counter * 1));
		}else if (Backup_UpEncoder_TimeCounter > 26) {
			return((Adj_UpEncoder_Counter * 2));
		}else if (Backup_UpEncoder_TimeCounter > 16) {
			return((Adj_UpEncoder_Counter * 3));
		}else {
			return((Adj_UpEncoder_Counter * 4));
		}
	}else{
		// 7/8/9
		if (Backup_UpEncoder_TimeCounter > 38) {
			return((Adj_UpEncoder_Counter * 1));
		}else if (Backup_UpEncoder_TimeCounter > 28) {
			return((Adj_UpEncoder_Counter * 2));
		}else if (Backup_UpEncoder_TimeCounter > 18) {
			return((Adj_UpEncoder_Counter * 3));
		}else {
			return((Adj_UpEncoder_Counter * 4));
		}
	}
}
//==============================================================================
unsigned char Get_DownVol_Adj(void)
{
#ifdef IC_DEBUG_x
		rs232_printf("adjDw=> %d",Adj_DownEncoder_Counter);
		rs232_printf("BadjDw=> %d\n",Backup_DownEncoder_TimeCounter);
#endif
	if (Adj_DownEncoder_Counter == 0) {			//20040825
		return(0);
	}
	if (Backup_DownEncoder_TimeCounter == 0xff) {
		return(1);
	}
	if (Adj_DownEncoder_Counter < 3) {						//20090819 : 1/2/3/4/5 => 1/1/2/2/3
		if (Backup_DownEncoder_TimeCounter > 40) {          //20121012 RA1570 : 1/1/2/2/3
			return((Adj_DownEncoder_Counter * 1));
		}else if (Backup_DownEncoder_TimeCounter > 30) {
			return((Adj_DownEncoder_Counter * 1));
		}else if (Backup_DownEncoder_TimeCounter > 20) {
			return((Adj_DownEncoder_Counter * 2));
		}else if (Backup_DownEncoder_TimeCounter > 10) {
			return((Adj_DownEncoder_Counter * 2));
		}else {
			return((Adj_DownEncoder_Counter * 3));
		}
	}else if (Adj_DownEncoder_Counter < 5) {	
		// 3/4
		if (Backup_DownEncoder_TimeCounter > 36) {
			return((Adj_DownEncoder_Counter * 1));
		}else if (Backup_DownEncoder_TimeCounter > 26) {
			return((Adj_DownEncoder_Counter * 2));
		}else {
			return((Adj_DownEncoder_Counter * 3));
		}
	}else if (Adj_DownEncoder_Counter < 7) {	
		// 5/6
		if (Backup_DownEncoder_TimeCounter > 36) {
			return((Adj_DownEncoder_Counter * 1));
		}else if (Backup_DownEncoder_TimeCounter > 26) {
			return((Adj_DownEncoder_Counter * 2));
		}else if (Backup_DownEncoder_TimeCounter > 16) {
			return((Adj_DownEncoder_Counter * 3));
		}else {
			return((Adj_DownEncoder_Counter * 4));
		}
	}else{
		// 7/8/9
		if (Backup_DownEncoder_TimeCounter > 38) {
			return((Adj_DownEncoder_Counter * 1));
		}else if (Backup_DownEncoder_TimeCounter > 28) {
			return((Adj_DownEncoder_Counter * 2));
		}else if (Backup_DownEncoder_TimeCounter > 18) {
			return((Adj_DownEncoder_Counter * 3));
		}else {
			return((Adj_DownEncoder_Counter * 4));
		}
	}
}
/*
    front volume encoder up/down
    
        ? setup > SETUP_PWRONVOLUME , PwrOn_volume
        Func_PwrOnVolume_UpEnd();
        Func_PwrOnVolume_DownEnd();
*******************************************************************************/
void 	Func_EncoderVolumeUp(void)
{
	char total;
	
            //----------------------------------------------
	        //enable setup power on volume
	        if (mode_setup == SETUP_PWRONVOLUME) {
                // load PwrOn_volume
                AMP_volume = PwrOn_volume;
                goto start_EncoderVolUp;
            }     
            //----------------------------------------------
	        //enable setup AUX1 Unity gain adjust
	        if ( (mode_func == FUNCTION_AUX1)&&(mode_setup == SETUP_UNITY_GAIN_AUX1) ) {
                // load Unity gain 
			    AMP_volume = unity_volume[mode_func];
                goto start_EncoderVolUp;
            }	            
            //----------------------------------------------
	        //enable setup COAX1 Unity gain adjust
	        if ( (mode_func == FUNCTION_COAX1)&&(mode_setup == SETUP_UNITY_GAIN_COAX1) ) {
                // load Unity gain 
			    AMP_volume = unity_volume[mode_func];
                goto start_EncoderVolUp;
            }	            
            //----------------------------------------------
	        //enable setup OPT1 Unity gain adjust
	        if ( (mode_func == FUNCTION_OPT1)&&(mode_setup == SETUP_UNITY_GAIN_OPT1) ) {
                // load Unity gain 
			    AMP_volume = unity_volume[mode_func];
                goto start_EncoderVolUp;
            }	            
            //----------------------------------------------
	        //enable setup PCUSB Unity gain adjust : 20121210
	        if ( (mode_func == FUNCTION_PCUSB)&&(mode_setup == SETUP_UNITY_GAIN_PCUSB) ) {
                // load Unity gain 
			    AMP_volume = unity_volume[mode_func];
                goto start_EncoderVolUp;
            }	            
            //----------------------------------------------
	        //enable setup USB Unity gain adjust
	        if ( (mode_func == FUNCTION_IPOD)&&(mode_setup == SETUP_UNITY_GAIN_USB) ) {
                // load Unity gain 
			    AMP_volume = unity_volume[mode_func];
                goto start_EncoderVolUp;
            }	            
            //----------------------------------------------
            //AUX1, COAX1, OPT1, IPOD unity gain function disable check : 20121210
            if ( (mode_func == FUNCTION_AUX1)||(mode_func == FUNCTION_COAX1)||(mode_func == FUNCTION_OPT1)||(mode_func == FUNCTION_PCUSB)||(mode_func == FUNCTION_IPOD) ) {
                if ( unity_volume[mode_func] == 0 ) {
                    //unity volume disable =>goto volume control
                }else{
                    //enable unity volume ==> do not adjust VolumeData
                    //exit
                    //if MUTE == ON, MUTE=OFF
			        if(mute_mode == MUTE_ON) {
				        mute_mode = MUTE_OFF;
			 	        Mute_AllMuteOff();
#ifdef ASCiiKEY
				        Func_MuteStatus(mute_mode);  //20130316
#endif
			        }
                    return;
                }
            }
            //----------------------------------------------
start_EncoderVolUp:
            //----------------------------------------------
#ifdef VOLUME_SPEED
			if (volspeed_enable == 1) {
				total = AMP_volume + Get_UpVol_Adj();		//20040826
			} else {
				total = AMP_volume + get_vol_speed();
			}
#else
			total = AMP_volume + get_vol_speed();
#endif
			if(total > MAX_MASTER_VOLUME) {
				total = MAX_MASTER_VOLUME;
			}
			AMP_volume = total;

#ifdef IC_DEBUG_x
	rs232_printf("Evol.=>%x\n",AMP_volume);
#endif
			Set_MainVolume();

			if(mute_mode == MUTE_ON) {
				mute_mode = MUTE_OFF;
			 	Mute_AllMuteOff();
#ifdef ASCiiKEY
				Func_MuteStatus(mute_mode);  //20130316
#endif
			}
            //volume display : temp / main
            //20120906
            //if ( (mode_DispBlockC == BLOCKC_NORMAL) || (mode_DispBlockC == BLOCKC_SETUP) ) {
            //20121128
            if (mode_DispBlockC == BLOCKC_SETUP) {
            }else{
			    mode_setup = 0;
			    Set_DisplayBlockCTimer(BLOCKC_TEMP_VOLUME,COUNT100M_5SEC);
			}
#ifdef ASCiiKEY		//20110408
		    Func_VolumeCurrentStatus();
#endif	
            //----------------------------------------------
	        //enable setup power on volume
            if (mode_setup == SETUP_PWRONVOLUME) {
	            //setup power on volume
                // save PwrOn_volume
                PwrOn_volume = AMP_volume;
                Set_DisplayBlockCTimer(BLOCKC_SETUP,COUNT100M_10SEC);	
	            nvr_xPutData(ADDR_PWRON_VOLUME,PwrOn_volume);
            }     
            //----------------------------------------------
	        //enable setup AUX1 Unity gain adjust
	        if ( (mode_func == FUNCTION_AUX1)&&(mode_setup == SETUP_UNITY_GAIN_AUX1) ) {
                // save Unity gain 
			    unity_volume[mode_func] = AMP_volume;
                
                Set_DisplayBlockCTimer(BLOCKC_SETUP,COUNT100M_10SEC);
                nvr_xPutData((ADDR_UNITY_COAX1+mode_func-1),unity_volume[mode_func]);
                return;     //20121219
            }	            
            //----------------------------------------------
	        //enable setup COAX1 Unity gain adjust
	        if ( (mode_func == FUNCTION_COAX1)&&(mode_setup == SETUP_UNITY_GAIN_COAX1) ) {
                // save Unity gain 
			    unity_volume[mode_func] = AMP_volume;
                
                Set_DisplayBlockCTimer(BLOCKC_SETUP,COUNT100M_10SEC);
                nvr_xPutData((ADDR_UNITY_COAX1+mode_func-1),unity_volume[mode_func]);
                return;     //20121219
            }	            
            //----------------------------------------------
	        //enable setup OPT1 Unity gain adjust
	        if ( (mode_func == FUNCTION_OPT1)&&(mode_setup == SETUP_UNITY_GAIN_OPT1) ) {
                // save Unity gain 
			    unity_volume[mode_func] = AMP_volume;
                
                Set_DisplayBlockCTimer(BLOCKC_SETUP,COUNT100M_10SEC);
                nvr_xPutData((ADDR_UNITY_COAX1+mode_func-1),unity_volume[mode_func]);
                return;     //20121219
            }	            
            //----------------------------------------------
	        //enable setup PCUSB Unity gain adjust : 20121210
	        if ( (mode_func == FUNCTION_PCUSB)&&(mode_setup == SETUP_UNITY_GAIN_PCUSB) ) {
                // save Unity gain 
			    unity_volume[mode_func] = AMP_volume;
                
                Set_DisplayBlockCTimer(BLOCKC_SETUP,COUNT100M_10SEC);
                nvr_xPutData((ADDR_UNITY_COAX1+mode_func-1),unity_volume[mode_func]);
                return;     //20121219
            }	            
            //----------------------------------------------
	        //enable setup USB Unity gain adjust
	        if ( (mode_func == FUNCTION_IPOD)&&(mode_setup == SETUP_UNITY_GAIN_USB) ) {
                // save Unity gain 
			    unity_volume[mode_func] = AMP_volume;
                
                Set_DisplayBlockCTimer(BLOCKC_SETUP,COUNT100M_10SEC);
                nvr_xPutData((ADDR_UNITY_COAX1+mode_func-1),unity_volume[mode_func]);
                return;     //20121219
            }	            
            //----------------------------------------------
            //AUX1, COAX1, OPT1, PCUSB, IPOD unity gain function disable check : 20121219
            if ( (mode_func == FUNCTION_AUX1)||(mode_func == FUNCTION_COAX1)||(mode_func == FUNCTION_OPT1)||(mode_func == FUNCTION_PCUSB)||(mode_func == FUNCTION_IPOD) ) {
                if ( unity_volume[mode_func] == 0 ) {
                    //unity volume disable : normal save
                }else{
                    //enable unity volume ==> do not save
                    //exit
                    return;
                }
            }
            //save : 20121219
            AMP_volume_NoUnity = AMP_volume;
            //----------------------------------------------
}
//==============================================================================
void 	Func_EncoderVolumeDown(void)
{ 
	char total;

            //----------------------------------------------
	        //enable setup power on volume
	        if (mode_setup == SETUP_PWRONVOLUME) {
                // load PwrOn_volume
                AMP_volume = PwrOn_volume;
                goto start_EncoderVolDown;
            }     
            //----------------------------------------------
	        //enable setup AUX1 Unity gain adjust
	        if ( (mode_func == FUNCTION_AUX1)&&(mode_setup == SETUP_UNITY_GAIN_AUX1) ) {
                // load Unity gain 
			    AMP_volume = unity_volume[mode_func];
                goto start_EncoderVolDown;
            }	            
            //----------------------------------------------
	        //enable setup COAX1 Unity gain adjust
	        if ( (mode_func == FUNCTION_COAX1)&&(mode_setup == SETUP_UNITY_GAIN_COAX1) ) {
                // load Unity gain 
			    AMP_volume = unity_volume[mode_func];
                goto start_EncoderVolDown;
            }	            
            //----------------------------------------------
	        //enable setup OPT1 Unity gain adjust
	        if ( (mode_func == FUNCTION_OPT1)&&(mode_setup == SETUP_UNITY_GAIN_OPT1) ) {
                // load Unity gain 
			    AMP_volume = unity_volume[mode_func];
                goto start_EncoderVolDown;
            }	            
            //----------------------------------------------
	        //enable setup PCUSB Unity gain adjust : 20121210
	        if ( (mode_func == FUNCTION_PCUSB)&&(mode_setup == SETUP_UNITY_GAIN_PCUSB) ) {
                // load Unity gain 
			    AMP_volume = unity_volume[mode_func];
                goto start_EncoderVolDown;
            }	            
            //----------------------------------------------
	        //enable setup USB Unity gain adjust
	        if ( (mode_func == FUNCTION_IPOD)&&(mode_setup == SETUP_UNITY_GAIN_USB) ) {
                // load Unity gain 
			    AMP_volume = unity_volume[mode_func];
                goto start_EncoderVolDown;
            }	            
            //----------------------------------------------
            //AUX1, COAX1, OPT1, IPOD unity gain function disable check : 20121210
            if ( (mode_func == FUNCTION_AUX1)||(mode_func == FUNCTION_COAX1)||(mode_func == FUNCTION_OPT1)||(mode_func == FUNCTION_PCUSB)||(mode_func == FUNCTION_IPOD) ) {
                if ( unity_volume[mode_func] == 0 ) {
                    //unity volume disable =>goto volume control
                }else{
                    //enable unity volume ==> do not adjust VolumeData
                    //exit
                    //if MUTE == ON, MUTE=OFF
			        if(mute_mode == MUTE_ON) {
				        mute_mode = MUTE_OFF;
			 	        Mute_AllMuteOff();
#ifdef ASCiiKEY
				        Func_MuteStatus(mute_mode);  //20130316
#endif
			        }
                    return;
                }
            }
            //----------------------------------------------
start_EncoderVolDown:
            //----------------------------------------------
#ifdef VOLUME_SPEED
			if (volspeed_enable == 1) {
				if ( AMP_volume >= Get_DownVol_Adj() ) {		
					total = AMP_volume - Get_DownVol_Adj();		//20040826
				}else{
					total = 0;
				}
			} else {
				if ( AMP_volume >= get_vol_speed() ) {
					total = AMP_volume - get_vol_speed();
				}else{
					total = 0;
				}
			}
#else
			if ( AMP_volume >= get_vol_speed() ) {
				total = AMP_volume - get_vol_speed();
			}else{
				total = 0;
			}
#endif
			AMP_volume = total;

#ifdef IC_DEBUG_x
	rs232_printf("Evol.=>%x\n",AMP_volume);
#endif
			Set_MainVolume();

			if(mute_mode == MUTE_ON) {
				mute_mode = MUTE_OFF;
			 	Mute_AllMuteOff();
#ifdef ASCiiKEY
				Func_MuteStatus(mute_mode);  //20130316
#endif
			}
            //volume display : temp / main
            //20120906
            //if ( (mode_DispBlockC == BLOCKC_NORMAL) || (mode_DispBlockC == BLOCKC_SETUP) ) {
            //20121128
            if (mode_DispBlockC == BLOCKC_SETUP) {
            }else{
			    mode_setup = 0;
			    Set_DisplayBlockCTimer(BLOCKC_TEMP_VOLUME,COUNT100M_5SEC);
			}
#ifdef ASCiiKEY		//20110408
		    Func_VolumeCurrentStatus();
#endif	
            //----------------------------------------------
	        //enable setup power on volume
            if (mode_setup == SETUP_PWRONVOLUME) {
	            //setup power on volume
                // save PwrOn_volume
                PwrOn_volume = AMP_volume;
                Set_DisplayBlockCTimer(BLOCKC_SETUP,COUNT100M_10SEC);	
	            nvr_xPutData(ADDR_PWRON_VOLUME,PwrOn_volume);
            }     
            //----------------------------------------------
	        //enable setup AUX1 Unity gain adjust
	        if ( (mode_func == FUNCTION_AUX1)&&(mode_setup == SETUP_UNITY_GAIN_AUX1) ) {
                // save Unity gain 
			    unity_volume[mode_func] = AMP_volume;
                
                Set_DisplayBlockCTimer(BLOCKC_SETUP,COUNT100M_10SEC);
                nvr_xPutData((ADDR_UNITY_COAX1+mode_func-1),unity_volume[mode_func]);
                return;     //20121219
            }	            
            //----------------------------------------------
	        //enable setup COAX1 Unity gain adjust
	        if ( (mode_func == FUNCTION_COAX1)&&(mode_setup == SETUP_UNITY_GAIN_COAX1) ) {
                // save Unity gain 
			    unity_volume[mode_func] = AMP_volume;
                
                Set_DisplayBlockCTimer(BLOCKC_SETUP,COUNT100M_10SEC);
                nvr_xPutData((ADDR_UNITY_COAX1+mode_func-1),unity_volume[mode_func]);
                return;     //20121219
            }	            
            //----------------------------------------------
	        //enable setup OPT1 Unity gain adjust
	        if ( (mode_func == FUNCTION_OPT1)&&(mode_setup == SETUP_UNITY_GAIN_OPT1) ) {
                // save Unity gain 
			    unity_volume[mode_func] = AMP_volume;
                
                Set_DisplayBlockCTimer(BLOCKC_SETUP,COUNT100M_10SEC);
                nvr_xPutData((ADDR_UNITY_COAX1+mode_func-1),unity_volume[mode_func]);
                return;     //20121219
            }	            
            //----------------------------------------------
	        //enable setup PCUSB Unity gain adjust : 20121210
	        if ( (mode_func == FUNCTION_PCUSB)&&(mode_setup == SETUP_UNITY_GAIN_PCUSB) ) {
                // save Unity gain 
			    unity_volume[mode_func] = AMP_volume;
                
                Set_DisplayBlockCTimer(BLOCKC_SETUP,COUNT100M_10SEC);
                nvr_xPutData((ADDR_UNITY_COAX1+mode_func-1),unity_volume[mode_func]);
                return;     //20121219
            }	            
            //----------------------------------------------
	        //enable setup USB Unity gain adjust
	        if ( (mode_func == FUNCTION_IPOD)&&(mode_setup == SETUP_UNITY_GAIN_USB) ) {
                // save Unity gain 
			    unity_volume[mode_func] = AMP_volume;
                
                Set_DisplayBlockCTimer(BLOCKC_SETUP,COUNT100M_10SEC);
                nvr_xPutData((ADDR_UNITY_COAX1+mode_func-1),unity_volume[mode_func]);
                return;     //20121219
            }	            
            //----------------------------------------------
            //AUX1, COAX1, OPT1, PCUSB, IPOD unity gain function disable check : 20121219
            if ( (mode_func == FUNCTION_AUX1)||(mode_func == FUNCTION_COAX1)||(mode_func == FUNCTION_OPT1)||(mode_func == FUNCTION_PCUSB)||(mode_func == FUNCTION_IPOD) ) {
                if ( unity_volume[mode_func] == 0 ) {
                    //unity volume disable : normal save
                }else{
                    //enable unity volume ==> do not save
                    //exit
                    return;
                }
            }
            //save : 20121219
            AMP_volume_NoUnity = AMP_volume;
            //----------------------------------------------
            //----------------------------------------------
}
//==============================================================================
void 	Func_RmcVolumeUp(void)
{
	char total;

            //----------------------------------------------
	        //enable setup power on volume
	        if (mode_setup == SETUP_PWRONVOLUME) {
                // load PwrOn_volume
                AMP_volume = PwrOn_volume;
                goto start_remoconVolUp;
            }     
            //----------------------------------------------
	        //enable setup AUX1 Unity gain adjust
	        if ( (mode_func == FUNCTION_AUX1)&&(mode_setup == SETUP_UNITY_GAIN_AUX1) ) {
                // load Unity gain 
			    AMP_volume = unity_volume[mode_func];
                goto start_remoconVolUp;
            }	            
            //----------------------------------------------
	        //enable setup COAX1 Unity gain adjust
	        if ( (mode_func == FUNCTION_COAX1)&&(mode_setup == SETUP_UNITY_GAIN_COAX1) ) {
                // load Unity gain 
			    AMP_volume = unity_volume[mode_func];
                goto start_remoconVolUp;
            }	            
            //----------------------------------------------
	        //enable setup OPT1 Unity gain adjust
	        if ( (mode_func == FUNCTION_OPT1)&&(mode_setup == SETUP_UNITY_GAIN_OPT1) ) {
                // load Unity gain 
			    AMP_volume = unity_volume[mode_func];
                goto start_remoconVolUp;
            }	            
            //----------------------------------------------
	        //enable setup PCUSB Unity gain adjust : 20121210
	        if ( (mode_func == FUNCTION_PCUSB)&&(mode_setup == SETUP_UNITY_GAIN_PCUSB) ) {
                // load Unity gain 
			    AMP_volume = unity_volume[mode_func];
                goto start_remoconVolUp;
            }	            
            //----------------------------------------------
	        //enable setup USB Unity gain adjust
	        if ( (mode_func == FUNCTION_IPOD)&&(mode_setup == SETUP_UNITY_GAIN_USB) ) {
                // load Unity gain 
			    AMP_volume = unity_volume[mode_func];
                goto start_remoconVolUp;
            }	            
            //----------------------------------------------
            //AUX1, COAX1, OPT1, IPOD unity gain function disable check
            if ( (mode_func == FUNCTION_AUX1)||(mode_func == FUNCTION_COAX1)||(mode_func == FUNCTION_OPT1)||(mode_func == FUNCTION_PCUSB)||(mode_func == FUNCTION_IPOD) ) {
                if ( unity_volume[mode_func] == 0 ) {
                    //unity volume disable =>goto volume control
                }else{
                    //enable unity volume ==> do not adjust VolumeData
                    //exit
                    //if MUTE == ON, MUTE=OFF
			        if(mute_mode == MUTE_ON) {
				        mute_mode = MUTE_OFF;
			 	        Mute_AllMuteOff();
#ifdef ASCiiKEY
				        Func_MuteStatus(mute_mode);  //20130316
#endif
			        }
                    return;
                }
            }
            //----------------------------------------------
start_remoconVolUp:
            //----------------------------------------------

	if (f1_rmc1_longkey == 1) {					//20090812
		if (rmc1_longkey_counter < 21) {		//20070307
			++rmc1_longkey_counter;
		}
		total = AMP_volume + Get_RmcVol_Adj(rmc1_longkey_counter);
	}else{
		rmc1_longkey_counter = 0;			//20040827
		total = AMP_volume + get_vol_speed();
	}
	if(total > MAX_MASTER_VOLUME) {
		total = MAX_MASTER_VOLUME;
	}
	AMP_volume = total;

#ifdef IC_DEBUG_x
	rs232_printf("Rvol.=>%x\n",AMP_volume);
#endif
	Set_MainVolume();

	if(mute_mode == MUTE_ON) {
		mute_mode = MUTE_OFF;
	 	Mute_AllMuteOff();
#ifdef ASCiiKEY
		Func_MuteStatus(mute_mode);  //20130316
#endif
	}
            //volume display : temp / main
            //20120906
            //if ( (mode_DispBlockC == BLOCKC_NORMAL) || (mode_DispBlockC == BLOCKC_SETUP) ) {
            //20121128
            if (mode_DispBlockC == BLOCKC_SETUP) {
            }else{
	            mode_setup = 0;
	            Set_DisplayBlockCTimer(BLOCKC_TEMP_VOLUME,COUNT100M_5SEC);
			}
#ifdef ASCiiKEY		//20110408
		    Func_VolumeCurrentStatus();
#endif	
            //----------------------------------------------
	        //enable setup power on volume
            if (mode_setup == SETUP_PWRONVOLUME) {
	            //setup power on volume
                // save PwrOn_volume
                PwrOn_volume = AMP_volume;
                Set_DisplayBlockCTimer(BLOCKC_SETUP,COUNT100M_10SEC);	
	            nvr_xPutData(ADDR_PWRON_VOLUME,PwrOn_volume);
            }     
            //----------------------------------------------
            //----------------------------------------------
	        //enable setup AUX1 Unity gain adjust
	        if ( (mode_func == FUNCTION_AUX1)&&(mode_setup == SETUP_UNITY_GAIN_AUX1) ) {
                // save Unity gain 
			    unity_volume[mode_func] = AMP_volume;
                
                Set_DisplayBlockCTimer(BLOCKC_SETUP,COUNT100M_10SEC);
                nvr_xPutData((ADDR_UNITY_COAX1+mode_func-1),unity_volume[mode_func]);
                return;     //20121219
            }	            
            //----------------------------------------------
	        //enable setup COAX1 Unity gain adjust
	        if ( (mode_func == FUNCTION_COAX1)&&(mode_setup == SETUP_UNITY_GAIN_COAX1) ) {
                // save Unity gain 
			    unity_volume[mode_func] = AMP_volume;
                
                Set_DisplayBlockCTimer(BLOCKC_SETUP,COUNT100M_10SEC);
                nvr_xPutData((ADDR_UNITY_COAX1+mode_func-1),unity_volume[mode_func]);
                return;     //20121219
            }	            
            //----------------------------------------------
	        //enable setup OPT1 Unity gain adjust
	        if ( (mode_func == FUNCTION_OPT1)&&(mode_setup == SETUP_UNITY_GAIN_OPT1) ) {
                // save Unity gain 
			    unity_volume[mode_func] = AMP_volume;
                
                Set_DisplayBlockCTimer(BLOCKC_SETUP,COUNT100M_10SEC);
                nvr_xPutData((ADDR_UNITY_COAX1+mode_func-1),unity_volume[mode_func]);
                return;     //20121219
            }	            
            //----------------------------------------------
	        //enable setup PCUSB Unity gain adjust : 20121210
	        if ( (mode_func == FUNCTION_PCUSB)&&(mode_setup == SETUP_UNITY_GAIN_PCUSB) ) {
                // save Unity gain 
			    unity_volume[mode_func] = AMP_volume;
                
                Set_DisplayBlockCTimer(BLOCKC_SETUP,COUNT100M_10SEC);
                nvr_xPutData((ADDR_UNITY_COAX1+mode_func-1),unity_volume[mode_func]);
                return;     //20121219
            }	            
            //----------------------------------------------
	        //enable setup USB Unity gain adjust
	        if ( (mode_func == FUNCTION_IPOD)&&(mode_setup == SETUP_UNITY_GAIN_USB) ) {
                // save Unity gain 
			    unity_volume[mode_func] = AMP_volume;
                
                Set_DisplayBlockCTimer(BLOCKC_SETUP,COUNT100M_10SEC);
                nvr_xPutData((ADDR_UNITY_COAX1+mode_func-1),unity_volume[mode_func]);
                return;     //20121219
           }	            
            //----------------------------------------------
            //AUX1, COAX1, OPT1, PCUSB, IPOD unity gain function disable check : 20121219
            if ( (mode_func == FUNCTION_AUX1)||(mode_func == FUNCTION_COAX1)||(mode_func == FUNCTION_OPT1)||(mode_func == FUNCTION_PCUSB)||(mode_func == FUNCTION_IPOD) ) {
                if ( unity_volume[mode_func] == 0 ) {
                    //unity volume disable : normal save
                }else{
                    //enable unity volume ==> do not save
                    //exit
                    return;
                }
            }
            //save : 20121219
            AMP_volume_NoUnity = AMP_volume;
            //----------------------------------------------
            //----------------------------------------------
}
//==============================================================================
void 	Func_RmcVolumeDown(void)
{
	char total;

            //----------------------------------------------
	        //enable setup power on volume
	        if (mode_setup == SETUP_PWRONVOLUME) {
                // load PwrOn_volume
                AMP_volume = PwrOn_volume;
                goto start_remoconVolDown;
            }     
            //----------------------------------------------
	        //enable setup AUX1 Unity gain adjust
	        if ( (mode_func == FUNCTION_AUX1)&&(mode_setup == SETUP_UNITY_GAIN_AUX1) ) {
                // load Unity gain 
			    AMP_volume = unity_volume[mode_func];
                goto start_remoconVolDown;
            }	            
            //----------------------------------------------
	        //enable setup COAX1 Unity gain adjust
	        if ( (mode_func == FUNCTION_COAX1)&&(mode_setup == SETUP_UNITY_GAIN_COAX1) ) {
                // load Unity gain 
			    AMP_volume = unity_volume[mode_func];
                goto start_remoconVolDown;
            }	            
            //----------------------------------------------
	        //enable setup OPT1 Unity gain adjust
	        if ( (mode_func == FUNCTION_OPT1)&&(mode_setup == SETUP_UNITY_GAIN_OPT1) ) {
                // load Unity gain 
			    AMP_volume = unity_volume[mode_func];
                goto start_remoconVolDown;
            }	            
            //----------------------------------------------
	        //enable setup PCUSB Unity gain adjust : 20121210
	        if ( (mode_func == FUNCTION_PCUSB)&&(mode_setup == SETUP_UNITY_GAIN_PCUSB) ) {
                // load Unity gain 
			    AMP_volume = unity_volume[mode_func];
                goto start_remoconVolDown;
            }	            
            //----------------------------------------------
	        //enable setup USB Unity gain adjust
	        if ( (mode_func == FUNCTION_IPOD)&&(mode_setup == SETUP_UNITY_GAIN_USB) ) {
                // load Unity gain 
			    AMP_volume = unity_volume[mode_func];
                goto start_remoconVolDown;
            }	            
            //----------------------------------------------
            //AUX1, COAX1, OPT1, IPOD unity gain function disable check : 20121210
            if ( (mode_func == FUNCTION_AUX1)||(mode_func == FUNCTION_COAX1)||(mode_func == FUNCTION_OPT1)||(mode_func == FUNCTION_PCUSB)||(mode_func == FUNCTION_IPOD) ) {
                if ( unity_volume[mode_func] == 0 ) {
                    //unity volume disable =>goto volume control
                }else{
                    //enable unity volume ==> do not adjust VolumeData
                    //exit
                    //if MUTE == ON, MUTE=OFF
			        if(mute_mode == MUTE_ON) {
				        mute_mode = MUTE_OFF;
			 	        Mute_AllMuteOff();
#ifdef ASCiiKEY
				        Func_MuteStatus(mute_mode);  //20130316
#endif
			        }
                    return;
                }
            }
            //----------------------------------------------
start_remoconVolDown:
            //----------------------------------------------

	if (f1_rmc1_longkey == 1) {					//20090812
		if (rmc1_longkey_counter < 21) {		//20070307
			++rmc1_longkey_counter;
		}
		if ( AMP_volume >= Get_RmcVol_Adj(rmc1_longkey_counter) ) {
			total = AMP_volume - Get_RmcVol_Adj(rmc1_longkey_counter);
		}else{
			total = 0;
		}
	}else{
		rmc1_longkey_counter = 0;			//20040827
		if ( AMP_volume >= get_vol_speed() ) {
			total = AMP_volume - get_vol_speed();
		}else{
			total = 0;
		}
	}
	AMP_volume = total;

#ifdef IC_DEBUG_x
	rs232_printf("Rvol.=>%x\n",AMP_volume);
#endif
	Set_MainVolume();

	if(mute_mode == MUTE_ON) {
		mute_mode = MUTE_OFF;
	 	Mute_AllMuteOff();
#ifdef ASCiiKEY
		Func_MuteStatus(mute_mode);  //20130316
#endif
	}
            //volume display : temp / main
            //20120906
            //if ( (mode_DispBlockC == BLOCKC_NORMAL) || (mode_DispBlockC == BLOCKC_SETUP) ) {
            //20121128
            if (mode_DispBlockC == BLOCKC_SETUP) {
            }else{
	            mode_setup = 0;
	            Set_DisplayBlockCTimer(BLOCKC_TEMP_VOLUME,COUNT100M_5SEC);
	        }
#ifdef ASCiiKEY		//20110408
		    Func_VolumeCurrentStatus();
#endif	
            //----------------------------------------------
	        //enable setup power on volume
            if (mode_setup == SETUP_PWRONVOLUME) {
	            //setup power on volume
                // save PwrOn_volume
                PwrOn_volume = AMP_volume;
                Set_DisplayBlockCTimer(BLOCKC_SETUP,COUNT100M_10SEC);	
	            nvr_xPutData(ADDR_PWRON_VOLUME,PwrOn_volume);
            }     
            //----------------------------------------------
	        //enable setup AUX1 Unity gain adjust
	        if ( (mode_func == FUNCTION_AUX1)&&(mode_setup == SETUP_UNITY_GAIN_AUX1) ) {
                // save Unity gain 
			    unity_volume[mode_func] = AMP_volume;
                
                Set_DisplayBlockCTimer(BLOCKC_SETUP,COUNT100M_10SEC);
                nvr_xPutData((ADDR_UNITY_COAX1+mode_func-1),unity_volume[mode_func]);
                return;     //20121219
            }	            
            //----------------------------------------------
	        //enable setup COAX1 Unity gain adjust
	        if ( (mode_func == FUNCTION_COAX1)&&(mode_setup == SETUP_UNITY_GAIN_COAX1) ) {
                // save Unity gain 
			    unity_volume[mode_func] = AMP_volume;
                
                Set_DisplayBlockCTimer(BLOCKC_SETUP,COUNT100M_10SEC);
                nvr_xPutData((ADDR_UNITY_COAX1+mode_func-1),unity_volume[mode_func]);
                return;     //20121219
            }	            
            //----------------------------------------------
	        //enable setup OPT1 Unity gain adjust
	        if ( (mode_func == FUNCTION_OPT1)&&(mode_setup == SETUP_UNITY_GAIN_OPT1) ) {
                // save Unity gain 
			    unity_volume[mode_func] = AMP_volume;
                
                Set_DisplayBlockCTimer(BLOCKC_SETUP,COUNT100M_10SEC);
                nvr_xPutData((ADDR_UNITY_COAX1+mode_func-1),unity_volume[mode_func]);
                return;     //20121219
            }	            
            //----------------------------------------------
	        //enable setup PCUSB Unity gain adjust : 20121210
	        if ( (mode_func == FUNCTION_PCUSB)&&(mode_setup == SETUP_UNITY_GAIN_PCUSB) ) {
                // save Unity gain 
			    unity_volume[mode_func] = AMP_volume;
                
                Set_DisplayBlockCTimer(BLOCKC_SETUP,COUNT100M_10SEC);
                nvr_xPutData((ADDR_UNITY_COAX1+mode_func-1),unity_volume[mode_func]);
                return;     //20121219
            }	            
            //----------------------------------------------
	        //enable setup USB Unity gain adjust
	        if ( (mode_func == FUNCTION_IPOD)&&(mode_setup == SETUP_UNITY_GAIN_USB) ) {
                // save Unity gain 
			    unity_volume[mode_func] = AMP_volume;
                
                Set_DisplayBlockCTimer(BLOCKC_SETUP,COUNT100M_10SEC);
                nvr_xPutData((ADDR_UNITY_COAX1+mode_func-1),unity_volume[mode_func]);
                return;     //20121219
            }	            
            //----------------------------------------------
            //AUX1, COAX1, OPT1, PCUSB, IPOD unity gain function disable check : 20121219
            if ( (mode_func == FUNCTION_AUX1)||(mode_func == FUNCTION_COAX1)||(mode_func == FUNCTION_OPT1)||(mode_func == FUNCTION_PCUSB)||(mode_func == FUNCTION_IPOD) ) {
                if ( unity_volume[mode_func] == 0 ) {
                    //unity volume disable : normal save
                }else{
                    //enable unity volume ==> do not save
                    //exit
                    return;
                }
            }
            //save : 20121219
            AMP_volume_NoUnity = AMP_volume;
            //----------------------------------------------
            //----------------------------------------------
}
//20091215
/**********************************************************
	tc9482 : 0 -- 96dB
**********************************************************/
//20091215 : Main volume direct key
void	proc_KeyRs232_MZ_VOL(unsigned char key)
{
	//main active?
	//main mute?
	//max volume?
	//volume range[min ... max]
	char total;

            //----------------------------------------------
	        //enable setup power on volume
	        if (mode_setup == SETUP_PWRONVOLUME) {
                // load PwrOn_volume
                AMP_volume = PwrOn_volume;
                goto start_directVolume;
            }     
            //----------------------------------------------
	        //enable setup AUX1 Unity gain adjust
	        if ( (mode_func == FUNCTION_AUX1)&&(mode_setup == SETUP_UNITY_GAIN_AUX1) ) {
                // load Unity gain 
			    AMP_volume = unity_volume[mode_func];
                goto start_directVolume;
            }	            
            //----------------------------------------------
	        //enable setup COAX1 Unity gain adjust
	        if ( (mode_func == FUNCTION_COAX1)&&(mode_setup == SETUP_UNITY_GAIN_COAX1) ) {
                // load Unity gain 
			    AMP_volume = unity_volume[mode_func];
                goto start_directVolume;
            }	            
            //----------------------------------------------
	        //enable setup OPT1 Unity gain adjust
	        if ( (mode_func == FUNCTION_OPT1)&&(mode_setup == SETUP_UNITY_GAIN_OPT1) ) {
                // load Unity gain 
			    AMP_volume = unity_volume[mode_func];
                goto start_directVolume;
            }	            
            //----------------------------------------------
	        //enable setup PCUSB Unity gain adjust : 20121210
	        if ( (mode_func == FUNCTION_PCUSB)&&(mode_setup == SETUP_UNITY_GAIN_PCUSB) ) {
                // load Unity gain 
			    AMP_volume = unity_volume[mode_func];
                goto start_directVolume;
            }	            
            //----------------------------------------------
	        //enable setup USB Unity gain adjust
	        if ( (mode_func == FUNCTION_IPOD)&&(mode_setup == SETUP_UNITY_GAIN_USB) ) {
                // load Unity gain 
			    AMP_volume = unity_volume[mode_func];
                goto start_directVolume;
            }	            
            //----------------------------------------------
            //AUX1, COAX1, OPT1, IPOD unity gain function disable check : 20121210
            if ( (mode_func == FUNCTION_AUX1)||(mode_func == FUNCTION_COAX1)||(mode_func == FUNCTION_OPT1)||(mode_func == FUNCTION_PCUSB)||(mode_func == FUNCTION_IPOD) ) {
                if ( unity_volume[mode_func] == 0 ) {
                    //unity volume disable =>goto volume control
                }else{
                    //enable unity volume ==> do not adjust VolumeData
                    //exit
                    //if MUTE == ON, MUTE=OFF
			        if(mute_mode == MUTE_ON) {
				        mute_mode = MUTE_OFF;
			 	        Mute_AllMuteOff();
#ifdef ASCiiKEY
				        Func_MuteStatus(mute_mode);  //20130316
#endif
			        }
                    return;
                }
            }
            //----------------------------------------------
start_directVolume:
            //----------------------------------------------
	total = key;
	if(total > MAX_MASTER_VOLUME) {
		total = MAX_MASTER_VOLUME;
	}
	AMP_volume = total;

#ifdef IC_DEBUG_x
	rs232_printf("Rvol.=>%x\n",AMP_volume);
#endif
	Set_MainVolume();

	if(mute_mode == MUTE_ON) {
		mute_mode = MUTE_OFF;
	 	Mute_AllMuteOff();
#ifdef ASCiiKEY
		Func_MuteStatus(mute_mode);  //20130316
#endif
	}
            //volume display : temp / main
            //20120906
            //if ( (mode_DispBlockC == BLOCKC_NORMAL) || (mode_DispBlockC == BLOCKC_SETUP) ) {
            //20121128
            if (mode_DispBlockC == BLOCKC_SETUP) {
            }else{
	            mode_setup = 0;
	            Set_DisplayBlockCTimer(BLOCKC_TEMP_VOLUME,COUNT100M_5SEC);
	        }
#ifdef ASCiiKEY
    Func_VolumeCurrentStatus();     //20110317
#endif	
            //----------------------------------------------
	        //enable setup power on volume
            if (mode_setup == SETUP_PWRONVOLUME) {
	            //setup power on volume
                // save PwrOn_volume
                PwrOn_volume = AMP_volume;
                Set_DisplayBlockCTimer(BLOCKC_SETUP,COUNT100M_10SEC);	
	            nvr_xPutData(ADDR_PWRON_VOLUME,PwrOn_volume);
            }     
            //----------------------------------------------
	        //enable setup AUX1 Unity gain adjust
	        if ( (mode_func == FUNCTION_AUX1)&&(mode_setup == SETUP_UNITY_GAIN_AUX1) ) {
                // save Unity gain 
			    unity_volume[mode_func] = AMP_volume;
                
                Set_DisplayBlockCTimer(BLOCKC_SETUP,COUNT100M_10SEC);
                nvr_xPutData((ADDR_UNITY_COAX1+mode_func-1),unity_volume[mode_func]);
                return;     //20121219
            }	            
            //----------------------------------------------
	        //enable setup COAX1 Unity gain adjust
	        if ( (mode_func == FUNCTION_COAX1)&&(mode_setup == SETUP_UNITY_GAIN_COAX1) ) {
                // save Unity gain 
			    unity_volume[mode_func] = AMP_volume;
                
                Set_DisplayBlockCTimer(BLOCKC_SETUP,COUNT100M_10SEC);
                nvr_xPutData((ADDR_UNITY_COAX1+mode_func-1),unity_volume[mode_func]);
                return;     //20121219
            }	            
            //----------------------------------------------
	        //enable setup OPT1 Unity gain adjust
	        if ( (mode_func == FUNCTION_OPT1)&&(mode_setup == SETUP_UNITY_GAIN_OPT1) ) {
                // save Unity gain 
			    unity_volume[mode_func] = AMP_volume;
                
                Set_DisplayBlockCTimer(BLOCKC_SETUP,COUNT100M_10SEC);
                nvr_xPutData((ADDR_UNITY_COAX1+mode_func-1),unity_volume[mode_func]);
                return;     //20121219
            }	            
            //----------------------------------------------
	        //enable setup PCUSB Unity gain adjust : 20121210
	        if ( (mode_func == FUNCTION_PCUSB)&&(mode_setup == SETUP_UNITY_GAIN_PCUSB) ) {
                // save Unity gain 
			    unity_volume[mode_func] = AMP_volume;
                
                Set_DisplayBlockCTimer(BLOCKC_SETUP,COUNT100M_10SEC);
                nvr_xPutData((ADDR_UNITY_COAX1+mode_func-1),unity_volume[mode_func]);
                return;     //20121219
            }	            
            //----------------------------------------------
	        //enable setup USB Unity gain adjust
	        if ( (mode_func == FUNCTION_IPOD)&&(mode_setup == SETUP_UNITY_GAIN_USB) ) {
                // save Unity gain 
			    unity_volume[mode_func] = AMP_volume;
                
                Set_DisplayBlockCTimer(BLOCKC_SETUP,COUNT100M_10SEC);
                nvr_xPutData((ADDR_UNITY_COAX1+mode_func-1),unity_volume[mode_func]);
                return;     //20121219
            }	            
            //----------------------------------------------
            //AUX1, COAX1, OPT1, PCUSB, IPOD unity gain function disable check : 20121219
            if ( (mode_func == FUNCTION_AUX1)||(mode_func == FUNCTION_COAX1)||(mode_func == FUNCTION_OPT1)||(mode_func == FUNCTION_PCUSB)||(mode_func == FUNCTION_IPOD) ) {
                if ( unity_volume[mode_func] == 0 ) {
                    //unity volume disable : normal save
                }else{
                    //enable unity volume ==> do not save
                    //exit
                    return;
                }
            }
            //save : 20121219
            AMP_volume_NoUnity = AMP_volume;
            //----------------------------------------------
            //----------------------------------------------
}
/*******************************************************************************
NJW1194V 

LATCH	--|_______________________|----
CLK		---|_|-|_|-			  -|_|-----	[ falling edge,	Low(2usec) and High(2usec) ]
DATA	--D15-D14-------------D0-------	[ MSB(D15) ] 

VOLUME CONTROL DATA	:
0000 0000 0000 0000		VOL	1
0000 0000 0001 0000		VOL	2

0000 0000 xxxx xxxx		mute(0)
0000 0001 xxxx xxxx		+31.5dB
0000 0010 xxxx xxxx		+31.0dB
0000 0011 xxxx xxxx		+30.5dB

0100 0000 xxxx xxxx		0dB

1111 1110 xxxx xxxx		-95.0dB
1111 1111 xxxx xxxx		Mute(default)

INPUT SELECTOR :
000	000	00 0010	0000	MUTE(default)
000	001	00 0010	0000	INPUT1
000	010	00 0010	0000	INPUT2
000	011	00 0010	0000	INPUT3
000	100	00 0010	0000	INPUT4

TREBLE :
TC/B(1),data(4),TSW(1),00, 0011	0000

BASS :
TC/B(1),data(4),TSW(1),00, 0100	0000

-tone data(8bit)
0 000 0	000	(default: 0db, tone_off)

0 101 0	100	(-10dB)
0 100 1	100
0 100 0	100
0 011 1	100
0 011 0	100
0 010 1	100
0 010 0	100
0 001 1	100
0 001 0	100
0 000 1	100
0 000 0	100	(0dB)
1 000 1	100
1 001 0	100
1 001 1	100
1 010 0	100
1 010 1	100
1 011 0	100
1 011 1	100
1 100 0	100
1 100 1	100
1 101 0	100	(+10dB)


//42~45	NJW1194	tone
#define	NJW1194_lh(x)			( (x==1)? GPIO_SetBits(NJW1194_PORT, NJW1194_LH) : GPIO_ResetBits(NJW1194_PORT,	NJW1194_LH)	);
#define	NJW1194_data(x)			( (x==1)? GPIO_SetBits(NJW1194_PORT, NJW1194_DA) : GPIO_ResetBits(NJW1194_PORT,	NJW1194_DA)	);
#define	NJW1194_clock(x)		( (x==1)? GPIO_SetBits(NJW1194_PORT, NJW1194_CK) : GPIO_ResetBits(NJW1194_PORT,	NJW1194_CK)	);
********************************************************************************/
//------------------------------------------------------------------------------
//TYPE
//volume 1			0x00
//volume 2			0x10
//input	select		0x20
//treble			0x30
//bass				0x40
//DATA+TYPE
//volume 1          (0db)		0x4000
//volume 2          (0dB)		0x4010
//input	select      input1		0x0420
//treble			table       0xtt30
//bass				table       0xtt40
//------------------------------------------------------------------------------
const unsigned char TBL_NJW1194_TONE_LEVEL[21]	= {	
		0x54,0x4c,0x44,0x3c,0x34,0x2c,0x24,0x1c,0x14,0x0c,		//-10(dB),-9,-8,,-1
		0x04,													//0(dB)
		0x8c,0x94,0x9c,0xa4,0xac,0xb4,0xbc,0xc4,0xcc,0xd4		//+1(db),+2,+3,,+10
};
//------------------------------------------------------------------------------
void sDelay(word Time)
{ 
	while(Time --);
}
void s_NJW1194PortInit(void)
{
	NJW1194_lh(1);
	NJW1194_data(0);
	NJW1194_clock(0);
	NJW1194_bypass(1);			//bypass off is port High
}
//------------------------------------------------------------------------------
void s_NUW1194_Tx16(unsigned char t_data, unsigned char	t_type)
{
	unsigned char i;

	NJW1194_data(0);
	NJW1194_clock(1);
	NJW1194_lh(0);
    sDelay(10);
	//"data"	
	for(i=0;i<8;i++){
		if((t_data & (0x80 >> i)) == 0){	  //from MSB of	data
			NJW1194_data(0);
		} else {
			NJW1194_data(1);
		}
		sDelay(10);
		NJW1194_clock(1);
		sDelay(10);
		NJW1194_clock(0);
        sDelay(10);
	}
	//"type"
	for(i=0;i<8;i++){
		if((t_type & (0x80 >> i)) == 0){	  //from MSB of	type
			NJW1194_data(0);
		} else {
			NJW1194_data(1);
		}
		sDelay(10);
		NJW1194_clock(1);
		sDelay(10);
		NJW1194_clock(0);
        sDelay(10);
	}
	NJW1194_clock(1);
    sDelay(10);
	NJW1194_lh(1);
    sDelay(10);
}
//==============================================================================
/*******************************************************************************
    AMP_volume      [0 ~ 96]
    mode_bypass = mode_TempBypass       [0,1]
	bass_volume = bass_TempVolume       [0 ~ 10 ~ 20]	
	treble_volume = treble_TempVolume   [0 ~ 10 ~ 20]	
	balance_volume = balance_TempVolume [0 ~ 15 ~ 30]	
//TYPE
//volume 1			0x00
//volume 2			0x10
//input	select		0x20
//treble			0x30
//bass				0x40
//DATA+TYPE
//volume 1          (0db)		0x4000
//volume 2          (0dB)		0x4010
//input	select      input1		0x0420
//treble			table       0xtt30
//bass				table       0xtt40

*******************************************************************************/
void	Set_ToneInitialVolumeTx(void)
{
        //bypass
        NJW1194_bypass(mode_TempBypass);
	    //s_NUW1194_Tx16(data,type);
	    //volume 1 = 0dB
	    s_NUW1194_Tx16( 0x40, 0x00 );
		DelayMs(1);
	    //volume 2 = 0dB
	    s_NUW1194_Tx16( 0x40, 0x10 );
		DelayMs(1);
	    //input 1
	    s_NUW1194_Tx16( 0x04, 0x20 );
		DelayMs(1);
	    //treble_TempVolume
	    s_NUW1194_Tx16( TBL_NJW1194_TONE_LEVEL[treble_TempVolume], 0x30 );
		DelayMs(1);
	    //bass_TempVolume
	    s_NUW1194_Tx16( TBL_NJW1194_TONE_LEVEL[bass_TempVolume], 0x40 );
}
//------------------------------------------------------------------------------
void	Set_ToneVolume(void)
{
	//volume 0db, main mute on
	if(mode_TempBypass == OFF) {
	    //s_NUW1194_Tx16(data,type);
	    //treble_TempVolume
	    s_NUW1194_Tx16( TBL_NJW1194_TONE_LEVEL[treble_TempVolume], 0x30 );
	    //bass_TempVolume
	    s_NUW1194_Tx16( TBL_NJW1194_TONE_LEVEL[bass_TempVolume], 0x40 );
	}
}

/*******************************************************************************
PGA2310, PGA2311 [ (+31.5dB) --	0 -- (-95.5dB) -- sw-Mute ]

/CS		--|_______________________|----
CLK		___|-|_|-|			__|-|______	[ rising edge, High(2usec) and Low(2usec) ]
SDI		--R7-R6----R0-L7-----L0--------	[ MSB(R7) ]	

/MUTE	mute function [gain	value =	0x00]


	AMP_volume [00 ~ 96] /(1dB)
	=> Tx_data (00 ~ 96) * 2 => [ 0 ~ 0xc0 ]
	
	AMP_volume [0x00 ~ 0xc0	~ 0xff]	/(0.5dB)
	=> Tx_data [0x00 ~ 0xc0 ]


	Hex		db_disp		Display		data
	FF		+31.5		+31.5		1111 1111
									
	c0		0.0			MAX			1100 0000
	bf		-0.5		95.5		1011 1111
										
	2		-95.0		1.0			0000 0010
	1		-95.5		0.5			0000 0001
	0		Mute		MIN			0000 0000 (sw-mute)
	
//38~41	PGA2310	volume
#define	PGA2310_mute(x)			( (x==1)? GPIO_SetBits(PGA2310_PORT, PGA2310_MUTE) : GPIO_ResetBits(PGA2310_PORT, PGA2310_MUTE)	);
#define	PGA2310_cs(x)			( (x==1)? GPIO_SetBits(PGA2310_PORT, PGA2310_CS) : GPIO_ResetBits(PGA2310_PORT,	PGA2310_CS)	);
#define	PGA2310_data(x)			( (x==1)? GPIO_SetBits(PGA2310_PORT, PGA2310_DA) : GPIO_ResetBits(PGA2310_PORT,	PGA2310_DA)	);
#define	PGA2310_clock(x)		( (x==1)? GPIO_SetBits(PGA2310_PORT, PGA2310_CK) : GPIO_ResetBits(PGA2310_PORT,	PGA2310_CK)	);
********************************************************************************/
//------------------------------------------------------------------------------
void s_PGA2310PortInit(void)
{
	PGA2310_cs(1);
	PGA2310_data(0);
	PGA2310_clock(0);
	PGA2310_mute(0);		//mute(	Active low )
}
//------------------------------------------------------------------------------
void s_PGA2310_Tx16(unsigned char l_data, unsigned char	r_data)
{
	unsigned char i;

	PGA2310_data(0);
	PGA2310_clock(0);
	PGA2310_cs(0);
	PGA2310_cs(0);
	PGA2310_cs(0);
	PGA2310_cs(0);
	//"r_data"	
	for(i=0;i<8;i++){
		if((r_data & (0x80 >> i)) == 0){	  //from MSB of	data
			PGA2310_data(0);
			PGA2310_data(0);
			PGA2310_data(0);
		} else {
			PGA2310_data(1);
			PGA2310_data(1);
			PGA2310_data(1);
		}
		PGA2310_clock(1);
		PGA2310_clock(1);
		PGA2310_clock(1);
		PGA2310_clock(0);
		PGA2310_clock(0);
		PGA2310_clock(0);
	}
	//"l_data"	
	for(i=0;i<8;i++){
		if((l_data & (0x80 >> i)) == 0){	  //from MSB of	data
			PGA2310_data(0);
			PGA2310_data(0);
			PGA2310_data(0);
		} else {
			PGA2310_data(1);
			PGA2310_data(1);
			PGA2310_data(1);
		}
		PGA2310_clock(1);
		PGA2310_clock(1);
		PGA2310_clock(1);
		PGA2310_clock(0);
		PGA2310_clock(0);
		PGA2310_clock(0);
	}
	PGA2310_clock(0);
	PGA2310_clock(0);
	PGA2310_cs(1);
	PGA2310_cs(1);
	PGA2310_cs(1);
	PGA2310_cs(1);
}



/* 
	Display	Volume

//PGA2310
//TC9482

	AMP_volume [0 ~	96]	
	
*********************************************12345678901234567890**************************/
void volumeCalculate(unsigned char vol,	unsigned char *sign, unsigned char *val_100, unsigned char *val_10,	unsigned char *val_1)
{
	unsigned int temp_vol;
	
#ifdef RC_DSP				//20110421
	//TC9482 1db step
	temp_vol = (unsigned int) vol;
	if (temp_vol  >= MAX_MASTER_VOLUME)	{	//>=96
		//0.0 or plus dB
		//VOL:0.0
		*sign =	0;							//sign=0 , ' '
		*val_100 = 0;
		*val_10	= 0;
		*val_1 = 0;
	}else{
		//minus	dB
		temp_vol = MAX_MASTER_VOLUME - temp_vol;
		//VOL:-xx.x
		*sign =	2;							//sign=2 , '-'
		*val_100 = (unsigned char)((temp_vol)/100);
		*val_10	= (unsigned	char)((temp_vol	- *val_100*100)/10);
		*val_1 = (unsigned char)((temp_vol - *val_100*100 -	*val_10*10));
	}
#else
	//zetex	0.5db step
	temp_vol = (unsigned int) vol;
	if (temp_vol  >= 0x00c0) {
		//0.0 or plus dB
		temp_vol -=	0x00c0;
		if (temp_vol ==	0) {
			//VOL:0.0
			*sign =	0;							//sign=0 , ' '
			*val_100 = 0;
			*val_10	= 0;
			*val_1 = 0;
		}else{
			//*5
			temp_vol *=5;
			//VOL:+xx.x
			*sign =	1;							//sign=1 , '+'
			*val_100 = (unsigned char)((temp_vol)/100);
			*val_10	= (unsigned	char)((temp_vol	- *val_100*100)/10);
			*val_1 = (unsigned char)((temp_vol - *val_100*100 -	*val_10*10));
		}
	}else{
		//minus	dB
		temp_vol = 0x00c0 -	temp_vol;
		//*5
		temp_vol *=5;
		//VOL:-xx.x
		*sign =	2;							//sign=2 , '-'
		*val_100 = (unsigned char)((temp_vol)/100);
		*val_10	= (unsigned	char)((temp_vol	- *val_100*100)/10);
		*val_1 = (unsigned char)((temp_vol - *val_100*100 -	*val_10*10));
	}
#endif
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
	
/*
RA1570
//(mode_func) function status
#define	FUNCTION_DISP_OFF			0
#define	FUNCTION_COAX1     			1
#define	FUNCTION_COAX2     			2
#define	FUNCTION_OPT1     			3
#define	FUNCTION_OPT2     			4
#define	FUNCTION_PCUSB				5
#define	FUNCTION_IPOD				6
#define	FUNCTION_PHONO  			7
#define	FUNCTION_CD       			8
#define	FUNCTION_balXLR  			9
#define	FUNCTION_TUNER  			10
#define	FUNCTION_AUX1				11
#define	FUNCTION_AUX2				12


//Rotel CDP, RCD
#define ROTEL_CDP_INITIAL	0   	//20120618
#define ROTEL_CDP_ACD       0   	//20120618
#define ROTEL_CDP_COAX1     1   	//20120618
#define ROTEL_CDP_COAX2     2   	//20120618
*********************************************************/
void	Function_RotelLinkRCD(void)
{
    switch(rotel_cdp){
        case ROTEL_CDP_ACD:
            Function_SelectStart(FUNCTION_CD);
            break;
        case ROTEL_CDP_COAX1:
            Function_SelectStart(FUNCTION_COAX1);
            break;
        case ROTEL_CDP_COAX2:
            Function_SelectStart(FUNCTION_COAX2);
            break;
        default:
            break;
    }
}
//------------------------------------------------------------------------------
/*  RC function Toggle

    RC_functionToggleBackup :
                bit0:0 AUX1
                bit0:1 AUX2
            bit1:0 OPT1
            bit1:1 OPT2
        bit2:0 COAX1
        bit2:1 COAX2
//
	case SK_AUX1_2:			Func_Aux12Toggle();         break;      //20130301
	case SK_OPT1_2:			Func_Opt12Toggle();         break;      //Opt1 <->Opt2
	case SK_COAX1_2			Func_Coax12Toggle();        break;      //Coax1 <->Coax2
	case SK_AUX1:			Function_SelectStart(FUNCTION_AUX1);    break;
	case SK_AUX2:			Function_SelectStart(FUNCTION_AUX2);    break;

	case SK_COAX1:		    Function_SelectStart(FUNCTION_COAX1);   break;
	case SK_COAX2:		    Function_SelectStart(FUNCTION_COAX2);   break;
	case SK_OPT1:		    Function_SelectStart(FUNCTION_OPT1);    break;
	case SK_OPT2:		    Function_SelectStart(FUNCTION_OPT2);    break;
*******************************************************************************/
void 	Func_Aux12Toggle(void)
{
	
    if (RA_rc_1570 == 1) return;        //20130301
        
    // bit0 = 0 : AUX1
    // bot0 = 1 : AUX2        
    if ( (RC_functionToggleBackup & 0x01) == 0x00 )
    {
        //start AUX1
        //
        if(Power_GetPowerStatus() == 0) 				//?power off(0)
        {
            //call last backup function
            Function_SelectStart(FUNCTION_AUX1);
        }else{
            //?current AUX1
            if (mode_func == FUNCTION_AUX1) {
                //AUX1 => AUX2
                //RC_functionToggleBackup |= 0x01;
	            //nvr_xPutData(ADDR_FUNCTION_TOGGLE,RC_functionToggleBackup);
                //goto AUX2
                Function_SelectStart(FUNCTION_AUX2);
            }else{
                //=> AUX1
                Function_SelectStart(FUNCTION_AUX1);
            }
        }
    }else{
        //start AUX2
        //
        if(Power_GetPowerStatus() == 0) 				//?power off(0)
        {
            //call last backup function
            Function_SelectStart(FUNCTION_AUX2);
        }else{
            //?current AUX2
            if (mode_func == FUNCTION_AUX2) {
                //AUX1 <= AUX2
                //RC_functionToggleBackup &= 0xfe;
	            //nvr_xPutData(ADDR_FUNCTION_TOGGLE,RC_functionToggleBackup);
                //goto AUX1
                Function_SelectStart(FUNCTION_AUX1);
            }else{
                //=> AUX2
                Function_SelectStart(FUNCTION_AUX2);
            }
        }
    }      
}        
void 	Func_Opt12Toggle(void)
{
	
    if (RA_rc_1570 == 1) return;        //20130301
    // bit1 = 0 : OPT1
    // bot1 = 1 : OPT2        
    if ( (RC_functionToggleBackup & 0x02) == 0x00 )
    {
        //start OPT1
        //
        if(Power_GetPowerStatus() == 0) 				//?power off(0)
        {
            //call last backup function
            Function_SelectStart(FUNCTION_OPT1);
        }else{
            //?current OPT1
            if (mode_func == FUNCTION_OPT1) {
                //OPT1 => OPT2
                //RC_functionToggleBackup |= 0x02;
	            //nvr_xPutData(ADDR_FUNCTION_TOGGLE,RC_functionToggleBackup);
                //goto OPT2
                Function_SelectStart(FUNCTION_OPT2);
            }else{
                //=> OPT1
                Function_SelectStart(FUNCTION_OPT1);
            }
        }
    }else{
        //start OPT2
        //
        if(Power_GetPowerStatus() == 0) 				//?power off(0)
        {
            //call last backup function
            Function_SelectStart(FUNCTION_OPT2);
        }else{
            //?current OPT2
            if (mode_func == FUNCTION_OPT2) {
                //OPT1 <= OPT2
                //RC_functionToggleBackup &= 0xfd;
	            //nvr_xPutData(ADDR_FUNCTION_TOGGLE,RC_functionToggleBackup);
                //goto OPT1
                Function_SelectStart(FUNCTION_OPT1);
            }else{
                //=> OPT2
                Function_SelectStart(FUNCTION_OPT2);
            }
        }
    }      
}        
void 	Func_Coax12Toggle(void)
{
	
    if (RA_rc_1570 == 1) return;        //20130301
    // bit2 = 0 : COAX1
    // bot2 = 1 : COAX2        
    if ( (RC_functionToggleBackup & 0x04) == 0x00 )
    {
        //start COAX1
        //
        if(Power_GetPowerStatus() == 0) 				//?power off(0)
        {
            //call last backup function
            Function_SelectStart(FUNCTION_COAX1);
        }else{
            //?current COAX1
            if (mode_func == FUNCTION_COAX1) {
                //COAX1 => COAX2
                //RC_functionToggleBackup |= 0x04;
	            //nvr_xPutData(ADDR_FUNCTION_TOGGLE,RC_functionToggleBackup);
                //goto COAX2
                Function_SelectStart(FUNCTION_COAX2);
            }else{
                //=> COAX1
                Function_SelectStart(FUNCTION_COAX1);
            }
        }
    }else{
        //start COAX2
        //
        if(Power_GetPowerStatus() == 0) 				//?power off(0)
        {
            //call last backup function
            Function_SelectStart(FUNCTION_COAX2);
        }else{
            //?current COAX2
            if (mode_func == FUNCTION_COAX2) {
                //COAX1 <= COAX2
                //RC_functionToggleBackup &= 0xfb;
	            //nvr_xPutData(ADDR_FUNCTION_TOGGLE,RC_functionToggleBackup);
                //goto COAX1
                Function_SelectStart(FUNCTION_COAX1);
            }else{
                //=> COAX2
                Function_SelectStart(FUNCTION_COAX2);
            }
        }
    }      
}        
	    
/*******************************************************************************
    Function keys start!!!!!!!!!!!!1
    RA
    unity gain => tx volume data [AUX1, COAX1, OPT1, PC-USB, USB ]
*******************************************************************************/
void	Function_SelectStart(unsigned char new_func)
{
	    Setup_Reset();					//setup menu reset

		//test chd 20100701
		a_Work[6] = 0;
		a_Work[7] = 0;
		//20130131
        //b_StreamFreq_bk[typ] = 7;         //default=7(off) : backup save freq informatiion
        //20090820
		if(Power_GetPowerStatus() == 0) 				//?power off(0)
		{
			//power on =>
			mode_func_bk = new_func;
            //power commnad Tx?
            power_txMode = 0;
			Func_SeperatePowerOn();
#ifdef ASCiiKEY		//20110408
				Func_SourceStatus();
                Func_GetCurrentFreq();  //20130131
#endif	
		}else{
            //------------------------------------
		    if (mode_func == new_func) {
		        //same function
				Set_DisplayBlockCTimer(BLOCKC_NORMAL,0);	//20090827
#ifdef ASCiiKEY		//20110408
				Func_SourceStatus();
                Func_GetCurrentFreq();  //20130131
#endif	
            //------------------------------------
            }else{
            //------------------------------------
/*            //test VFD display => all skip
                Mute_AllMuteOn();	                        //20120926	
                mode_func = new_func;
				mode_func_bk = mode_func;
		        nvr_xPutData(ADDR_MODE_FUNC_BK,mode_func_bk);
	            //mute off time set!
	            Mute_SetMuteOffTime(COUNT100M_800MS);
				return;
*/
            //------------------------------------
                //other function
                Mute_AllMuteOn();	                        //20120926	
                if (mode_func == FUNCTION_IPOD) {
                    //before, if function = IPOD, iPOD STOP!!!!!!!		
#ifdef IPOD_INCLUDE
				    TCLS_iPodSetFunction(iPod_OFF);
		            if (TInfo.mPlayPause >= TCL_PBSTATE_PLAY) {
#ifdef SIMPLE_REMOTE_LINGO								//20100316
                        //Pause
				        Main_TCLS_NaviMain(KEY_IPOD_PAUSE);
				        //Stop
				        //Main_TCLS_NaviMain(KEY_IPOD_STOP);
#else
                        //Pause
                        TCLS_NaviOnButtonPause();
                        //Stop
				        //TCLS_NaviOnButtonStop();
#endif
                    }
#endif
                }
                //------------------------------------
                //save NEW function
				mode_func = new_func;
				mode_func_bk = mode_func;
		        nvr_xPutData(ADDR_MODE_FUNC_BK,mode_func_bk);
				//?toggle function backup : 20130301
				//RC1570
            if (RA_rc_1570 == 0) {
                if (mode_func == FUNCTION_OPT2) {
                    //OPT2 : bit1=1
                    RC_functionToggleBackup |= 0x02;
                    count_toggleFuncDataSave = TIME20MSTO100MS;
	                //nvr_xPutData(ADDR_FUNCTION_TOGGLE,RC_functionToggleBackup);
                }else if (mode_func == FUNCTION_OPT1) {
                    //OPT1 : bit1=0
                    RC_functionToggleBackup &= 0xfd;
                    count_toggleFuncDataSave = TIME20MSTO100MS;
	                //nvr_xPutData(ADDR_FUNCTION_TOGGLE,RC_functionToggleBackup);
                }else if (mode_func == FUNCTION_COAX2) {
                    //COAX2 : bit2=1
                    RC_functionToggleBackup |= 0x04;
                    count_toggleFuncDataSave = TIME20MSTO100MS;
	                //nvr_xPutData(ADDR_FUNCTION_TOGGLE,RC_functionToggleBackup);
                }else if (mode_func == FUNCTION_COAX1) {
                    //COAX1 : bit2=0
                    RC_functionToggleBackup &= 0xfb;
                    count_toggleFuncDataSave = TIME20MSTO100MS;
	                //nvr_xPutData(ADDR_FUNCTION_TOGGLE,RC_functionToggleBackup);
                }else if (mode_func == FUNCTION_AUX2) {
                    //AUX2 : bit0=1
                    RC_functionToggleBackup |= 0x01;
                    count_toggleFuncDataSave = TIME20MSTO100MS;
	                //nvr_xPutData(ADDR_FUNCTION_TOGGLE,RC_functionToggleBackup);
                }else if (mode_func == FUNCTION_AUX1) {
                    //AUX1 : bit0=0
                    RC_functionToggleBackup &= 0xfe;
                    count_toggleFuncDataSave = TIME20MSTO100MS;
	                //nvr_xPutData(ADDR_FUNCTION_TOGGLE,RC_functionToggleBackup);
                }
            }
				//Display_AllBufferClear();				//20090616
                //change NEW function
				ListenFunctionChange();
				//PC-USB freq display delay : 
				//20130121
                if( mode_func == FUNCTION_PCUSB )
                {
                    // suspend High => count reset 1000msec [ irregular data display skip !!!! ]
                    CM_count_suspendLow = 0;
				}
				//stream display delay : 
				//?digital function : 20121031
                if( (mode_func == FUNCTION_COAX1)||(mode_func == FUNCTION_COAX2)||(mode_func == FUNCTION_OPT1)||(mode_func == FUNCTION_OPT2)||(mode_func == FUNCTION_PCUSB)||(mode_func == FUNCTION_IPOD) )
                //if( (mode_func == FUNCTION_COAX1)||(mode_func == FUNCTION_COAX2)||(mode_func == FUNCTION_OPT1)||(mode_func == FUNCTION_OPT2) )
                {
					b_StreamDisplayDelay = TIME20MSTO1000MS;        //20121012
				}
                //20121214
                //load???
                if ( (mode_func == FUNCTION_AUX1)||(mode_func == FUNCTION_COAX1)||(mode_func == FUNCTION_OPT1)||(mode_func == FUNCTION_PCUSB)||(mode_func == FUNCTION_IPOD) ) {
                    //? unity volume disable
                    if ( unity_volume[mode_func] == 0 ) {
                        //unity volume disable
                        AMP_volume = AMP_volume_NoUnity;             //re-load
                    }else{
                        //enable unity volume ==> do not adjust VolumeData
                        AMP_volume = unity_volume[mode_func];
                    }
                }else{
                    //remain function volume
                    //if (AMP_volume_NoUnity != AMP_volume) {
                        AMP_volume = AMP_volume_NoUnity;             //re-load
                    //}
                }
                //
                Set_MainVolume();                       //Unity gain check?

#ifdef IPOD_INCLUDE  
                if(mode_func == FUNCTION_IPOD) { 
					TCLS_iPodSetFunction(iPod_ON);
					TCLS_iPodInitialize();
				}
#endif

#ifdef ASCiiKEY		//20110408
				Func_SourceStatus();
                Func_GetCurrentFreq();  //20130131
#endif
            }					
            //------------------------------------
		}
}
/*
#define	ON							1
#define	OFF							0
#define	SPEAKER_A			        0			//20120619
#define	SPEAKER_B			        1			//20120619
#define	SPEAKER_INITIAL			    0			//20120619
#define	SPEAKER_ALLOFF			    0			//20120619
#define	SPEAKER_A_ON			    1			//20120619
#define	SPEAKER_B_ON			    2			//20120619
#define	SPEAKER_AB_ON			    3			//20120619
*******************************************************************************/
void	set_SPEAKER_Function(unsigned char spk_type, unsigned char spk_data)
{
    //spk_type : 0 => SPEAKER_A
    //spk_type : 1 => SPEAKER_B
    //spk_data : 0 => OFF
    //spk_data : 1 => ON
    if (spk_type == SPEAKER_A) {
        //speaker A
        if (spk_data == OFF) {
            //SPEAKER A = off
            switch(mode_speakerAB)
            {
                case SPEAKER_A_ON:
	                mode_speakerAB = SPEAKER_ALLOFF;
                    break;                    
                case SPEAKER_AB_ON:
	                mode_speakerAB = SPEAKER_B_ON;
                    break;    
                case SPEAKER_B_ON:
                case SPEAKER_ALLOFF:
                default:
                    break;
            }
        }else{
            //SPEAKER A = on
            switch(mode_speakerAB)
            {
                case SPEAKER_B_ON:
	                mode_speakerAB = SPEAKER_AB_ON;
                    break;                    
                case SPEAKER_ALLOFF:
	                mode_speakerAB = SPEAKER_A_ON;
                    break;    
                case SPEAKER_A_ON:
                case SPEAKER_AB_ON:
                default:
                    break;
            }
        }
    }else{
        //speaker B
        if (spk_data == OFF) {
            //SPEAKER B = off
            switch(mode_speakerAB)
            {
                case SPEAKER_B_ON:
	                mode_speakerAB = SPEAKER_ALLOFF;
                    break;                    
                case SPEAKER_AB_ON:
	                mode_speakerAB = SPEAKER_A_ON;
                    break;    
                case SPEAKER_A_ON:
                case SPEAKER_ALLOFF:
                default:
                    break;
            }
        }else{
            //SPEAKER B = on
            switch(mode_speakerAB)
            {
                case SPEAKER_A_ON:
	                mode_speakerAB = SPEAKER_AB_ON;
                    break;                    
                case SPEAKER_ALLOFF:
	                mode_speakerAB = SPEAKER_B_ON;
                    break;    
                case SPEAKER_B_ON:
                case SPEAKER_AB_ON:
                default:
                    break;
            }
        }
    }
}
//------------------------------------------------------------------------------
void	set_SPEAKER_Relay(unsigned char spk_type)
{
    //?Amp Portection skip
    //if (mode_amp_protect == 0) {
    	if (RA_rc_1570 == 0) return;        //20120924

       //setting Speaker Relay Port
        switch(spk_type)
        {
            case SPEAKER_A_ON:
                Speaker_B(0);
                Speaker_A(1);
                break;                    
            case SPEAKER_B_ON:
                Speaker_A(0);
                Speaker_B(1);
                break;    
            case SPEAKER_AB_ON:
                Speaker_A(1);
                Speaker_B(1);
                break;    
            case SPEAKER_ALLOFF:
            default:
                Speaker_A(0);
                Speaker_B(0);
                break;    
        }
    //}   
}
//------------------------------------------------------------------------------
const char str_speaker_AIs[2][14+1] = {     //20120619
    			"speaker_a=off!",
				"speaker_a=on!"
};
const char str_speaker_BIs[2][14+1] = {     //20120619
    			"speaker_b=off!",
				"speaker_b=on!"
};
const char str_speakerIs[4][12+1] = {      //20120619
				"speaker=off!",
    			"speaker=a!",
				"speaker=b!",
				"speaker=a_b!"
};
//------------------------------------------------------------------------------
void 	Func_SpeakerStatus(void)
{
	unsigned short i,m;
	
    if (RA_rc_1570 == 0) return;        //20120924

    m=0;
    if (mode_speakerAB < 4) {
        for(i=0;(i<20)&&(str_speakerIs[mode_speakerAB][i]!='\0');i++) RotelOpaqueData[m++] = (unsigned char)str_speakerIs[mode_speakerAB][i];
    }else{
        return;
    }

	//Reciva OpaqueText Tx
	RotelOpaqueData[m] = 0x00;		//null(0x00)
    //------------------------------------------------------
//#ifdef ADD_ID_ROTELLINK
    if ( (RL_version == 1)||(RL_version == 2) ) {
        //20120829
        //RA-1570 : "11:"
        //"11:" + RotelOpaqueData(m);
        addID_RoteOpaqueData(1,&m);
    }
    //------------------------------------------------------
	//
	//RS232 Tx start
	//
//#ifdef RS232_SKIP	//20110114 test
	for(i=0;i<m;i++) {								
		PutChar(RotelOpaqueData[i]); 
	}				
//#endif
	//
	//Opaque Tx start			
	//
	if (Stingray_Startup_mode == S_CONFIGURE_OK){		//20101027
		Stingray_Tx_RotelOpaqueData(m);                 //RA_proc_display.c
	}
}
/*******************************************************************************
    key speaker A

	mode_speakerAB = SPEAKER_INITIAL;						////20120619
	nvr_xPutData(ADDR_SPEAKER_AB,mode_speakerAB);
*******************************************************************************/
void 	Func_speaker_A(void)
{
    	if (RA_rc_1570 == 0) return;        //20120924

        switch(mode_speakerAB)
        {
            case SPEAKER_A_ON:
                set_SPEAKER_Function( SPEAKER_A, OFF );     //set mode_speakerAB
                Func_SpeakerStatus();                       //Tx
                set_SPEAKER_Relay(mode_speakerAB);          //set Relay port
                break;                    
            case SPEAKER_AB_ON:
                set_SPEAKER_Function( SPEAKER_A, OFF );     //set mode_speakerAB
                Func_SpeakerStatus();                       //Tx
                set_SPEAKER_Relay(mode_speakerAB);          //set Relay port
                break;    
            case SPEAKER_B_ON:
            case SPEAKER_ALLOFF:
                set_SPEAKER_Function( SPEAKER_A, ON );      //set mode_speakerAB
                Func_SpeakerStatus();                       //Tx
                set_SPEAKER_Relay(mode_speakerAB);          //set Relay port
                break;    
            default:
                break;
        }    
	    nvr_xPutData(ADDR_SPEAKER_AB,mode_speakerAB);
}
/*******************************************************************************
    key speaker B

	mode_speakerAB = SPEAKER_INITIAL;						////20120619
	nvr_xPutData(ADDR_SPEAKER_AB,mode_speakerAB);
*******************************************************************************/
void 	Func_speaker_B(void)
{
    	if (RA_rc_1570 == 0) return;        //20120924

        switch(mode_speakerAB)
        {
            case SPEAKER_B_ON:
                set_SPEAKER_Function( SPEAKER_B, OFF );     //set mode_speakerAB
                Func_SpeakerStatus();                       //Tx
                set_SPEAKER_Relay(mode_speakerAB);          //set Relay port
                break;                    
            case SPEAKER_AB_ON:
                set_SPEAKER_Function( SPEAKER_B, OFF );     //set mode_speakerAB
                Func_SpeakerStatus();                       //Tx
                set_SPEAKER_Relay(mode_speakerAB);          //set Relay port
                break;                    
            case SPEAKER_A_ON:
            case SPEAKER_ALLOFF:
                set_SPEAKER_Function( SPEAKER_B, ON );      //set mode_speakerAB
                Func_SpeakerStatus();                       //Tx
                set_SPEAKER_Relay(mode_speakerAB);          //set Relay port
                break;                    
            default:
                break;
        }    
	    nvr_xPutData(ADDR_SPEAKER_AB,mode_speakerAB);
}
/*******************************************************************************
    key Bypass : Temp Tone bypass on/off

	mode_bypass = 0;			// bypass On(1), off(0)
	mode_TempBypass = 0;
	nvr_xPutData(ADDR_BYPASS,mode_bypass);		//20090616
*******************************************************************************/
void 	Func_bypass(void)
{
	Setup_Reset();					//setup menu reset
	Mute_AllMuteOn();       //20121010
	Mute_SetMuteOffTime(COUNT100M_800MS);
    DelayMs(2);                     //20121130
        switch(mode_TempBypass)
        {
            case ON:
                mode_TempBypass = OFF;
                NJW1194_bypass(OFF);
                //display mode
				Set_DisplayBlockCTimer(BLOCKC_BYPASS,COUNT100M_5SEC);
				//Tx
				Func_GetCurrentBypass();
                break;                    
            case OFF:
                mode_TempBypass = ON;
                NJW1194_bypass(ON);
                //display mode
				Set_DisplayBlockCTimer(BLOCKC_BYPASS,COUNT100M_5SEC);	
				//Tx
				Func_GetCurrentBypass();
                break;                    
            default:
                break;
        }    
}
//--------------------------------------------------------------------
void 	Func_DirectBypassSet(unsigned char type)
{
	Setup_Reset();					//setup menu reset
	Mute_AllMuteOn();       //20121010
	Mute_SetMuteOffTime(COUNT100M_800MS);
    DelayMs(2);                     //20121130
        mode_TempBypass = type;
        //exec bypass
        NJW1194_bypass(type);
        Set_DisplayBlockCTimer(BLOCKC_BYPASS,COUNT100M_5SEC);	
        //Tx
        Func_GetCurrentBypass();
}
/*******************************************************************************
    key Bass : Temp Bass display on/off

	bass_volume = TONE_VOLUME_INITIAL;
	bass_TempVolume = TONE_VOLUME_INITIAL;
	nvr_xPutData(ADDR_BASS,bass_volume);	    //[0~10~20]	
	
	mode_DispBlockC == BLOCKC_BASS;

*******************************************************************************/
void 	Func_bass(void)
{
	Setup_Reset();					//setup menu reset
	//20121206 : if temp_bypass==on, skip
    if (mode_TempBypass == OFF) {
        if (mode_DispBlockC == BLOCKC_BASS) {
            Set_DisplayBlockCTimer(BLOCKC_NORMAL,0);
        }else{
            //Tx
            Func_GetCurrentBass();
            Set_DisplayBlockCTimer(BLOCKC_BASS,COUNT100M_5SEC);	
        }
    b1_CommTimer_250msec = 0;		//blinking reset : 20121206
    }
}
//--------------------------------------------------------------------
void 	Func_DirectBassSet(unsigned char bas)
{
	Setup_Reset();					//setup menu reset
	//20121206 : if temp_bypass==on, skip
    if (mode_TempBypass == OFF) {
        bass_TempVolume = bas;
        Set_DisplayBlockCTimer(BLOCKC_BASS,COUNT100M_5SEC);	
        Set_ToneVolume();
        //Tx
        Func_GetCurrentBass();
    b1_CommTimer_250msec = 0;		//blinking reset : 20121206
    }
}
//--------------------------------------------------------------------
void 	Func_BassUp(void)
{
        if (bass_TempVolume < 20) {
            ++bass_TempVolume;
        }
        Set_DisplayBlockCTimer(BLOCKC_BASS,COUNT100M_5SEC);
        Set_ToneVolume();
        //Tx
        Func_GetCurrentBass();        
    b1_CommTimer_250msec = 0;		//blinking reset : 20121206
}
//--------------------------------------------------------------------
void 	Func_BassDown(void)
{
        if (bass_TempVolume > 0) {
            --bass_TempVolume;
        }
        Set_DisplayBlockCTimer(BLOCKC_BASS,COUNT100M_5SEC);
        Set_ToneVolume();
        //Tx
        Func_GetCurrentBass();        
    b1_CommTimer_250msec = 0;		//blinking reset : 20121206
}
/*******************************************************************************
    key Treble : Temp Treble display on/off

	treble_volume = TONE_VOLUME_INITIAL;
	treble_TempVolume = TONE_VOLUME_INITIAL;
	nvr_xPutData(ADDR_TREBLE,treble_volume);    //[0~10~20]			
	
	mode_DispBlockC == BLOCKC_TREBLE;

*******************************************************************************/
void 	Func_treble(void)
{
	Setup_Reset();					//setup menu reset
	//20121206 : if temp_bypass==on, skip
    if (mode_TempBypass == OFF) {
        if (mode_DispBlockC == BLOCKC_TREBLE) {
            Set_DisplayBlockCTimer(BLOCKC_NORMAL,0);
        }else{
            Set_DisplayBlockCTimer(BLOCKC_TREBLE,COUNT100M_5SEC);	
        }
    b1_CommTimer_250msec = 0;		//blinking reset : 20121206
    }
}
//--------------------------------------------------------------------
void 	Func_DirectTrebleSet(unsigned char trb)
{
	Setup_Reset();					//setup menu reset
	//20121206 : if temp_bypass==on, skip
    if (mode_TempBypass == OFF) {
        treble_TempVolume = trb;
        Set_DisplayBlockCTimer(BLOCKC_TREBLE,COUNT100M_5SEC);	
        Set_ToneVolume();
        //Tx
        Func_GetCurrentTreble();
    b1_CommTimer_250msec = 0;		//blinking reset : 20121206
    }
}
//--------------------------------------------------------------------
void 	Func_TrebleUp(void)
{
        if (treble_TempVolume < 20) {
            ++treble_TempVolume;
        }
        Set_DisplayBlockCTimer(BLOCKC_TREBLE,COUNT100M_5SEC);
        Set_ToneVolume();
        //Tx
        Func_GetCurrentTreble();        
    b1_CommTimer_250msec = 0;		//blinking reset : 20121206
}
//--------------------------------------------------------------------
void 	Func_TrebleDown(void)
{
        if (treble_TempVolume > 0) {
            --treble_TempVolume;
        }
        Set_DisplayBlockCTimer(BLOCKC_TREBLE,COUNT100M_5SEC);
        Set_ToneVolume();
        //Tx
        Func_GetCurrentTreble();        
    b1_CommTimer_250msec = 0;		//blinking reset : 20121206
}
/*******************************************************************************
    key Balance : Temp Balance display on/off

	balance_volume = BALANCE_VOLUME_INITIAL;
	balance_TempVolume = BALANCE_VOLUME_INITIAL;
	nvr_xPutData(ADDR_BALANCE,balance_volume);  //[0~15~30]			
	
	mode_DispBlockC == BLOCKC_BALANCE;

*******************************************************************************/
void 	Func_balance(void)
{
	Setup_Reset();					//setup menu reset
        if (mode_DispBlockC == BLOCKC_BALANCE) {
            Set_DisplayBlockCTimer(BLOCKC_NORMAL,0);
        }else{
            Set_DisplayBlockCTimer(BLOCKC_BALANCE,COUNT100M_5SEC);	
        }
}
//--------------------------------------------------------------------
void 	Func_DirectBalanceSet(unsigned char bal)
{
	Setup_Reset();					//setup menu reset
        balance_TempVolume = bal;
        Set_DisplayBlockCTimer(BLOCKC_BALANCE,COUNT100M_5SEC);	
        Set_MainVolume();
}
//--------------------------------------------------------------------
void 	Func_BalanceRight(void)
{
        if (balance_TempVolume < 30) {
            ++balance_TempVolume;
        }
        Set_DisplayBlockCTimer(BLOCKC_BALANCE,COUNT100M_5SEC);
        Set_MainVolume();
        //Tx
        Func_GetCurrentBalance();        
}
//--------------------------------------------------------------------
void 	Func_BalanceLeft(void)
{
        if (balance_TempVolume > 0) {
            --balance_TempVolume;
        }
        Set_DisplayBlockCTimer(BLOCKC_BALANCE,COUNT100M_5SEC);
        Set_MainVolume();
        //Tx
        Func_GetCurrentBalance();        
}
	
/**/



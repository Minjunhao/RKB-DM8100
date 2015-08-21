/**************** (c)2012 Rotel ************************************************
PROJECT         : RKB-850,8100,D850,D8100
COMPILER        : IAR Embeded Workbench 5.50
MODULE          : main.c
VERSION         : V 1.0
USED CHIP       : stm32f101RC [R=64pin / 256KB/32KB]
CREATION DATE   : /2013
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

#include "eeprom.h"
#include "i2c.h"

#include "proc_function.h"

#include "function.ext"

#include "proc_display.ext"
#include "proc_display.h"

#include "ak4114.ext"
#include "ak4114.h"


#include "RKB_function.h"

#ifdef IPOD_INCLUDE
#include "iPod_config.h"
#endif

/*******************************************************************************
		switch(mode_func){
            //------------------------------------------------------------
            case FUNCTION_AMP_A:
            case FUNCTION_AMP_B:
            case FUNCTION_AMP_C:
            case FUNCTION_AMP_D:
                break;
            case FUNCTION_DISP_OFF:	
            default:
                break;
		}
*******************************************************************************/
//==============================================================================
/*
#define	FUNCTION_DISP_OFF			0
#define	FUNCTION_AMP_A				1
#define	FUNCTION_AMP_B				2
#define	FUNCTION_AMP_C				3
#define	FUNCTION_AMP_D				4
*********************************************************/
/*
	RKB mute toggle
//volatile byte amp_on_off[4];
//volatile byte volume_Lch[4];
//volatile byte volume_Rch[4];
//volatile byte balance[4];
//volatile byte ch_muteOn[4];
//unsigned char ch_muteOn_volValue[4];
//volatile byte front_trimKnob_enable;
	
*********************************************************/
void	Func_MuteFlagAllCH_Off(void)
{
	unsigned char i;
	
	for (i=0;i<4;i++) ch_muteOn[i] = MUTE_OFF;
}
//----------------------------------------------------------
void	Func_MuteFlag_CH_Off(unsigned char ampType)
{	
	ch_muteOn[ampType] = MUTE_OFF;
}
/*
	20140604 : singnal sense_nSW
************************************************************/
void	Func_Mute(unsigned char ampType)
{
	unsigned char i;

	if(Power_GetPowerStatus() == 0) return;		//?power off(0), skip
	//? PP & MP2 unit & signal sensing nSW
	//20140819 deleted
	//if ( (RKB_mode_PP_MP == 1)&&(RKB_mode_PP_nMP2 == 0)&&(GetSignalSensing_nSW == 0) ) return;	
		
	if (ampType > 3) {
		//All-ch mute toggle
		for (i=0;i<4;i++) {
			//20140325 : ? each CH power check
			if (amp_on_off[i] == 1 ) {
				if (ch_muteOn[i] == MUTE_ON) {
					//mute off ->volume data Tx
    				//AMP = mute off!!!!!
    				Func_MuteOff(i);
				}else{
					//mute on ->volume mute data Tx
    				//AMP = mute on!!!!!
    				Func_MuteOn(i);
				}
			}
		}
	}else{
		//20140325
		//? curretn-ch power on
		if (amp_on_off[ampType] == 1 ) {	//?amp_on_off[]==on				
			//current-ch mute toggle
			if (ch_muteOn[ampType] == MUTE_ON) {
				//mute off ->volume data Tx
    			//AMP = mute off!!!!!
    			Func_MuteOff(ampType);
			}else{
				//mute on ->volume mute data Tx
    			//AMP = mute on!!!!!
    			Func_MuteOn(ampType);
			}
		}
	}
}
/*
	RKB mute on
//unsigned char ch_muteOn_volValue[4];
	20140604 : singnal sense_nSW
*********************************************************/
void	Func_MuteOn(unsigned char ampType)
{
	unsigned char i;

	if(Power_GetPowerStatus() == 0) return;		//?power off(0), skip
	//? PP & MP2 unit & signal sensing nSW
	//20140819 deleted
	//if ( (RKB_mode_PP_MP == 1)&&(RKB_mode_PP_nMP2 == 0)&&(GetSignalSensing_nSW == 0) ) return;	
		
	if (ampType > 3) {
		//All-ch mute on
		for (i=0;i<4;i++) {
			//20140325 : ? each CH power check
			if (amp_on_off[i] == 1 ) {
				ch_muteOn[i] = MUTE_ON;
				//20140226 : for func_volume2dB_cha(,,)
				if (front_trimKnob_enable == ON) {
					ch_muteOn_volValue[i] = key_target[i];
				}			
				//Set_MainVolume(vol,ampType) => L/R Tx
				Set_MainVolume(0,i);					//mute=0
#ifdef ASCiiKEY
				Func_MuteStatus(ch_muteOn[i],i);
#endif
			}
		}
	}else{
		//20140325
		//? curretn-ch power on
		if (amp_on_off[ampType] == 1 ) {	//?amp_on_off[]==on				
			//current-ch mute on
			if (ch_muteOn[ampType] == MUTE_OFF) {
				//mute on ->volume mute data Tx
    			//AMP = mute on!!!!!
				ch_muteOn[ampType] = MUTE_ON;
				//20140226 : for func_volume2dB_cha(,,)
				if (front_trimKnob_enable == ON) {
					ch_muteOn_volValue[ampType] = key_target[ampType];
				}
				//Set_MainVolume(vol,ampType) => L/R Tx
				Set_MainVolume(0,ampType);					//mute=0
			}
#ifdef ASCiiKEY
			//20140401
			Func_MuteStatus(ch_muteOn[ampType],ampType);  //20140213
			//RS232_TxSkip = mode_DisplayUpdate;				//20140220	
#endif				
		}
	}
}
/*
	RKB mute off
	
	20140306 : review balance ?
	
	20140604 : singnal sense_nSW
*********************************************************/
void	Func_MuteOff(unsigned char ampType)
{
	unsigned char i;

	if(Power_GetPowerStatus() == 0) return;		//?power off(0), skip
	//? PP & MP2 unit & signal sensing nSW
	//20140819 deleted
	//if ( (RKB_mode_PP_MP == 1)&&(RKB_mode_PP_nMP2 == 0)&&(GetSignalSensing_nSW == 0) ) return;	
		
	if (ampType > 3) {
		//All-ch mute off
		for (i=0;i<4;i++) {
			//20140325 : ? each CH power check
			if (amp_on_off[i] == 1 ) {
				ch_muteOn[i] = MUTE_OFF;
				//20140221
				if (front_trimKnob_enable == OFF) {
					//20140306 : if same Lch & Rch and then have to balance calculation
					if ( volume_Lch[i] != volume_Rch[i] ) {
						//balance = center!
						//volume_Lch[4]
						//volume_Rch[4]
						//Set_MainVolume(vol,ampType) => L/R Tx
						Set_MainVolumeLeft(volume_Lch[i],i);		//mute off
						Set_MainVolumeRight(volume_Rch[i],i);		//mute off
					}else{
						//(Lch = Rch) calculate balance value!
						//20140305
						Set_MainVolume(volume_Lch[i],i);
					}
				}else{
					//current_key[4]
					//Set_MainVolume(vol,ampType) => L/R Tx
					//mute off : balance value!
					Set_MainVolume(current_key[i],i);
				}
#ifdef ASCiiKEY
				Func_MuteStatus(ch_muteOn[i],i);
#endif
			}
		}
	}else{
		//20140325
		//? curretn-ch power on
		if (amp_on_off[ampType] == 1 ) {	//?amp_on_off[]==on				
			//current-ch mute off
			if (ch_muteOn[ampType] == MUTE_ON) {
				ch_muteOn[ampType] = MUTE_OFF;
				//mute off ->volume data Tx
    			//AMP = mute off!!!!!
				//20140221
				if (front_trimKnob_enable == OFF) {
					//20140306 : if same Lch & Rch and then have to balance calculation
					if ( volume_Lch[ampType] != volume_Rch[ampType] ) {
						//balance = center!
						//volume_Lch[4]
						//volume_Rch[4]
						//Set_MainVolume(vol,ampType) => L/R Tx
						Set_MainVolumeLeft(volume_Lch[ampType],ampType);		//mute off
						Set_MainVolumeRight(volume_Rch[ampType],ampType);		//mute off
					}else{
						//(Lch = Rch) calculate balance value!
						//20140305
						Set_MainVolume(volume_Lch[ampType],ampType);
					}
				}else{
					//current_key[4]
					//Set_MainVolume(vol,ampType) => L/R Tx
					//mute off : balance value!
					Set_MainVolume(current_key[ampType],ampType);
				}
			}
#ifdef ASCiiKEY
			//20140401
			Func_MuteStatus(ch_muteOn[ampType],ampType);  //20140213
			//RS232_TxSkip = mode_DisplayUpdate;				//20140220	
#endif
		}
	}
}
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
/*

	balance[4]
	
	vol >= (bal_data) : vol - (bal-data) => L(R)_ch
	vol < (bal_data)  : 0(mute) => L(R)_ch
    
******************************************************************************************/
void	Balance_channel_calculate(unsigned char ampType, unsigned char vol, unsigned char *L_ch, unsigned char *R_ch)
{
	unsigned char total,step,balance_level_max;
	

    balance_level_max = 30;
	step = 10;		//1db step
    //
	//balance channel calculate
	//
	if (vol == 0) {
		//mute
		*L_ch = vol;
		*R_ch = vol;
	}else{
		if (balance[ampType] == (balance_level_max/2) ) {
			//center
			*L_ch = vol;
			*R_ch = vol;
		}else if (balance[ampType] > (balance_level_max/2) ) {
			//Left_channel
			total = balanceLevelTable15[vol/step][balance[ampType] - (balance_level_max/2) - 1];          //Table refer
			if (total > 0) {
				if ( vol >= total ) {
					*L_ch = vol - total;
				}else{
					*L_ch = 0;  //mute
				}
			}else{
				*L_ch = 0;  //mute
			}
			*R_ch = vol;
		}else{
			//Right_channel
			total = balanceLevelTable15[vol/step][ (balance_level_max/2) - balance[ampType] - 1];         //Table refer
			if (total > 0) {
				if ( vol >= total ) {
					*R_ch = vol - total;
				}else{
					*R_ch = 0;  //mute
				}
			}else{
				*R_ch = 0;  //mute
			}
			*L_ch = vol;
		}
	}
}

/*
	NJW1159
	20140213 : add balance calculate
	
//volatile byte amp_on_off[4];
//volatile byte volume_Lch[4];
//volatile byte volume_Rch[4];
//volatile byte balance[4];
//volatile byte ch_muteOn[4];
//volatile byte front_trimKnob_enable;

	ampType : 0/1/2/3
********************************************************************************/
void	Set_MainVolume(unsigned char vol, unsigned char ampType)
{
	unsigned char adjL_ch, adjR_ch;
	unsigned char L_ch, R_ch;

    //----------------------------------------------------------------
    if(vol > 96) vol = 96;
    //----------------------------------------------------------------
	//balance ?
	Balance_channel_calculate(ampType, vol, &L_ch, &R_ch);
    //----------------------------------------------------------------
	//adjust if (vol==0) , vol data mute on(0x7F)
	//		 else (96-vol) , vol data 
    //----------------------------------------------------------------
	if(L_ch == 0) {				//? MUTE
		adjL_ch = 0x7f;			//njw1159
	}else{
		adjL_ch = 96 - L_ch;
	}
	//
	if(R_ch == 0) {				//? MUTE
		adjR_ch = 0x7f;			//njw1159
	}else{
		adjR_ch = 96 - R_ch;
	}
#ifdef UART1_DEBUG_x
    //rs232_printf("\rVOLUME DATA =>\r",blank);
	//rs232_printf("amp =>%d\n",ampType);
	//rs232_printf("Vol =>%d\n",vol);
	//rs232_printf("L_ch=> %d\n",L_ch);
	//rs232_printf("R_ch=> %d\n",R_ch);
	rs232_printf("\nAMP:Vol:Lch:Rch: =>%d %d %d %d\n",ampType, vol, adjL_ch, adjR_ch);
#endif
    //
    //
    //volume data Tx
    //20140213
    s_NJW1159_Tx16(adjL_ch, (0x00|ampType) );		//Left+ampType(0/1/2/3)
    s_NJW1159_Tx16(adjR_ch, (0x10|ampType) );		//Right+ampType(0/1/2/3)
}
//--------------------------------------------------------------------
//	RKB new
//		if (front_trimKnob_enable == ON) skip!!!!!
//
//--------------------------------------------------------------------
void	Set_MainVolumeLeft(unsigned char L_ch, unsigned char ampType)
{
    //----------------------------------------------------------------
    //	balance[ampType] => BALANCE_VOLUME_INITIAL setting
    if(L_ch > 96) L_ch = 96;
    //----------------------------------------------------------------
	//L_ch=0, L_ch data mute on
	//
	if(L_ch == 0) {				//? MUTE
		L_ch = 0x7f;			//njw1159
	}else{
		__asm(" nop" );			//20140210
		L_ch = 96 - L_ch;
	}

#ifdef UART1_DEBUG_x
    //rs232_printf("\rVOLUME DATA =>\r",blank);
	//rs232_printf("amp =>%d\n",ampType);
	//rs232_printf("Vol =>%d\n",vol);
	//rs232_printf("L_ch => %x\n",L_ch);
	//rs232_printf("R_ch => %x\n",R_ch);
	rs232_printf("\nAMP:Lch: =>%d %d\n",ampType, L_ch);
#endif
    //
    //
    //volume data Tx
    //20140213
    s_NJW1159_Tx16(L_ch, (0x00|ampType) );		//Left+ampType(0/1/2/3)
}
//--------------------------------------------------------------------
//--------------------------------------------------------------------
void	Set_MainVolumeRight(unsigned char R_ch, unsigned char ampType)
{
    //----------------------------------------------------------------
    //	balance[ampType] => BALANCE_VOLUME_INITIAL setting
    if(R_ch > 96) R_ch = 96;
    //----------------------------------------------------------------
	//R_ch=0, R_ch data mute on
	//
	if(R_ch == 0) {				//? MUTE
		R_ch = 0x7f;			//njw1159
	}else{
		__asm(" nop" );			//20140210
		R_ch = 96 - R_ch;
	}

#ifdef UART1_DEBUG_x
    //rs232_printf("\rVOLUME DATA =>\r",blank);
	//rs232_printf("amp =>%d\n",ampType);
	//rs232_printf("Vol =>%d\n",vol);
	//rs232_printf("L_ch => %x\n",L_ch);
	//rs232_printf("R_ch => %x\n",R_ch);
	rs232_printf("\nAMP:Rch: =>%d %d\n",ampType,R_ch);
#endif
    //
    //
    //volume data Tx
    //20140213
    s_NJW1159_Tx16(R_ch, (0x10|ampType) );		//Right+ampType(0/1/2/3)
}

//==============================================================================
////////////////////////////////////////////////////////////////////////////////////////////////////
//		RKB-D850
////////////////////////////////////////////////////////////////////////////////////////////////////
/*******************************************************************************
NJW1159V : Volume IC [ 0 ~ -95dB/1dBstep , MUTE ]

LATCH	--|_______________________|----
CLK		---|_|-|_|-			  -|_|-----	[ falling edge,	Low(2usec) and High(2usec) ]
DATA	--D15-D14-------------D0-------	[ MSB(D15) ] 

VOLUME DATA		: [ D15~D9, D8=0] 
Select Address	: [ D7/6/5/4 ]		: Left=0000 / Right=0001 /
Chip Address	: [ D3/2/1/0 ]		: ch_A=0000 / ch_B=0001 / ch_C=0010 / ch_D=0011 /

0000 0000 0000 0000		VOL	1(L)
0000 0000 0001 0000		VOL	2(R)

0000 000x 0000 0000		ch_A Left
0000 000x 0001 0000		ch_A Right
0000 000x 0000 0001		ch_B Left
0000 000x 0001 0001		ch_B Right
0000 000x 0000 0010		ch_C Left
0000 000x 0001 0010		ch_C Right
0000 000x 0000 0011		ch_D Left
0000 000x 0001 0011		ch_D Right

<volume data>
0000 000x xxxx xxxx		0dB
0000 001x xxxx xxxx		-1dB
0000 010x xxxx xxxx		-2dB

1011 111x xxxx xxxx		-95.0dB			: (0x3f=95)

1111 1111 xxxx xxxx		Mute(default)	: (0x7f)

/MUTE	mute function [gain	value =	0x7f]


	current_key [00 ~ 96] /(1dB)
	=> Tx_data (96 - volume) => [ 0x5f ~ 0x00 ]
	

Dec	Hex		db_disp		Display		data
96	60 		0.0			MAX			0000 0000
95	5f		-1			95			0000 0001
										
1	1		-95			1			0101 1111
0	0		Mute		MIN			0111 1111(0x7f)

//30~32(21~23) NJW1159 volume IC
#define NJW1159_lat(x)			( (x==1)? GPIO_SetBits(NJW1159_PORT, NJW1159_LAT) : GPIO_ResetBits(NJW1159_PORT, NJW1159_LAT) );
#define NJW1159_clock(x)		( (x==1)? GPIO_SetBits(NJW1159_PORT, NJW1159_CLK) : GPIO_ResetBits(NJW1159_PORT, NJW1159_CLK) );
#define NJW1159_data(x)			( (x==1)? GPIO_SetBits(NJW1159_PORT, NJW1159_DAT) : GPIO_ResetBits(NJW1159_PORT, NJW1159_DAT) );
********************************************************************************/
//------------------------------------------------------------------------------
//TYPE
//CH_A_LEFT			0x00
//CH_A_RIGHT		0x10
//CH_B_LEFT			0x01
//CH_B_RIGHT		0x11
//CH_C_LEFT			0x02
//CH_C_RIGHT		0x12
//CH_D_LEFT			0x03
//CH_D_RIGHT		0x13

//DATA => ( (shift left <<1) | 0x01 )
//(0db)				0x00
//(-1dB)			0x01
//(-95dB)			0x5f(95)

//(mute)			0x7f
//------------------------------------------------------------------------------
void sDelay(word Time)
{ 
	while(Time --);
}
//------------------------------------------------------------------------------
void s_NJW1159PortInit(void)
{
	NJW1159_lat(1);
	NJW1159_data(0);
	NJW1159_clock(1);
}
//------------------------------------------------------------------------------
//20140210 : add delay time
//------------------------------------------------------------------------------
void s_NJW1159_Tx16(unsigned char t_data, unsigned char	t_type)
{
	unsigned char i;

	NJW1159_data(0);
	NJW1159_clock(1);
	NJW1159_lat(0);
    sDelay(10+1);
	//adjust "data" << 1 or 0x01
	t_data = ( t_data <<1 ) | 0x01;
	for(i=0;i<8;i++){
		if((t_data & (0x80 >> i)) == 0){	  //from MSB of	data
			NJW1159_data(0);
		} else {
			NJW1159_data(1);
		}
		sDelay(10+2);
		NJW1159_clock(0);
		sDelay(10+2);
		NJW1159_clock(1);
        sDelay(10+2);
	}
	//"type"
	for(i=0;i<8;i++){
		if((t_type & (0x80 >> i)) == 0){	  //from MSB of	type
			NJW1159_data(0);
		} else {
			NJW1159_data(1);
		}
		sDelay(10+2);
		NJW1159_clock(0);
		sDelay(10+2);
		NJW1159_clock(1);
        sDelay(10+2);
	}
	NJW1159_clock(1);
    sDelay(10+2);
	NJW1159_lat(1);
    sDelay(10+10);
}
//------------------------------------------------------------------------------
//volatile byte amp_on_off[4];
//volatile byte volume_Lch[4];
//volatile byte volume_Rch[4];
//volatile byte balance[4];
//volatile byte ch_muteOn[4];
//volatile byte front_trimKnob_enable;
//	balance[ampType]
//	check AMP on/off	20140409
//	check mute on/off	20140409
//------------------------------------------------------------------------------
void 	func_DirectBalanceSet(unsigned char bal, unsigned char ampType)
{

	unsigned char i;

	if(Power_GetPowerStatus() > 0) {		//?power off(0), skip
		//
		//Now unit-power on => 
		if (ampType > 3) {
			//20140401 :
			if (front_trimKnob_enable == OFF) {
				//volume_Lch[4]
				//volume_Rch[4]
				//All CH balance setting
				for (i=0;i<4;i++) {
					//20140409 : AMP on/off ?
					if (amp_on_off[i] == 1 ) {		//?amp_on_off[]==on
						//20140409 : mute ?
						if (ch_muteOn[i] == MUTE_OFF) {
							//? Lch == Rch
							if ( volume_Lch[i] == volume_Rch[i] ) {
								balance[i] = bal;
								eepromAddr = ADDR_A_BAL+i;					//20140224
								nvr_xPutData(eepromAddr,balance[i]);
								//20140221
								//volume_Lch[4]
								//volume_Rch[4]
								//Set_MainVolume(vol,ampType) => L/R Tx
								Set_MainVolume(volume_Rch[i],i);
                                //Tx status: 20140213
                                Func_GetCurrentBalance(i);
                            }else{
                            	// Can not change balance , so display current vol level
								Func_VolumeLchStatus((i));
								Func_VolumeRchStatus((i));
                            }
                        }else{
                        	//mute on
#ifdef ASCiiKEY
							Func_MuteStatus(ch_muteOn[i],i);
#endif
                        }
                    }else{
                    	//AMP off!
						//AMP CH status
						Func_AMP_CH_status(i);                  	
                    }
				} 
			}else{
				//current_key[4]
				//All CH balance setting
				for (i=0;i<4;i++) {
					//20140409 : AMP on/off ?
					if (amp_on_off[i] == 1 ) {		//?amp_on_off[]==on
						//20140409 : mute ?
						if (ch_muteOn[i] == MUTE_OFF) {
							balance[i] = bal;
							eepromAddr = ADDR_A_BAL+i;					//20140224
							nvr_xPutData(eepromAddr,balance[i]);
							//20140221
							//current_key[4]
                            //Set_MainVolume(vol,ampType) => L/R Tx
							Set_MainVolume(current_key[i],i);
                            //Tx status: 20140213
                            Func_GetCurrentBalance(i);
                        }else{
                        	//mute on
#ifdef ASCiiKEY
							Func_MuteStatus(ch_muteOn[i],i);
#endif
                        }
                    }else{
                    	//AMP off!
						//AMP CH status
						Func_AMP_CH_status(i);                  	
                    }
				} 
			}
		}else{
			//20140401 :
			if (front_trimKnob_enable == OFF) {
				//20140409 : AMP on/off ?
				if (amp_on_off[ampType] == 1 ) {		//?amp_on_off[]==on
					//20140409 : mute ?
					if (ch_muteOn[ampType] == MUTE_OFF) {
						//		
						//? Lch == Rch
						if ( volume_Lch[ampType] == volume_Rch[ampType] ) {
							balance[ampType] = bal;
							eepromAddr = ADDR_A_BAL+ampType;					//20140224
							nvr_xPutData(eepromAddr,balance[ampType]);
							//20140221
							//volume_Lch[4]
							//volume_Rch[4]
                            //Set_MainVolume(vol,ampType) => L/R Tx
							Set_MainVolume(volume_Rch[ampType],ampType);
                            //Tx status: 20140213
                            Func_GetCurrentBalance(ampType);
                        }else{
                        	// Can not change balance , so display current vol level
							Func_VolumeLchStatus((ampType));
							Func_VolumeRchStatus((ampType));
                        }
                    }else{
                    	//mute on
#ifdef ASCiiKEY
						Func_MuteStatus(ch_muteOn[ampType],ampType);
#endif
                    }
                }else{
					//AMP off!
					//AMP CH status
					Func_AMP_CH_status(ampType);                  	
                }
            }else{
				//20140409 : AMP on/off ?
				if (amp_on_off[ampType] == 1 ) {		//?amp_on_off[]==on
					//20140409 : mute ?
					if (ch_muteOn[ampType] == MUTE_OFF) {
						//		
						//current_key[4]
						balance[ampType] = bal;
						eepromAddr = ADDR_A_BAL+ampType;					//20140224
						nvr_xPutData(eepromAddr,balance[ampType]);
						//20140221
						//current_key[4]
                        //Set_MainVolume(vol,ampType) => L/R Tx
						Set_MainVolume(current_key[ampType],ampType);
                        //Tx status: 20140213
                        Func_GetCurrentBalance(ampType);
                    }else{
                    	//mute on
#ifdef ASCiiKEY
						Func_MuteStatus(ch_muteOn[ampType],ampType);
#endif
                    }
                }else{
					//AMP off!
					//AMP CH status
					Func_AMP_CH_status(ampType);                  	
                }
            }
        }
    }
}
//--------------------------------------------------------------------
//20140408
//20140409 : check amp on/off & mute on/off
void 	Func_BalanceRight(unsigned char ampType)
{
	unsigned char i;

	if(Power_GetPowerStatus() > 0) {		//?power off(0), skip
		//
		//Now unit-power on => 
		if (ampType > 3) {
			//20140401 :
			if (front_trimKnob_enable == OFF) {
				//volume_Lch[4]
				//volume_Rch[4]
				//All CH balance setting
				for (i=0;i<4;i++) {
					//20140409 : AMP on/off ?
					if (amp_on_off[i] == 1 ) {		//?amp_on_off[]==on
						//20140409 : mute ?
						if (ch_muteOn[i] == MUTE_OFF) {
							//? Lch == Rch
							if ( volume_Lch[i] == volume_Rch[i] ) {
								//
								//
								//balance[i] = bal;
								if (balance[i] < 30) {
									++balance[i];			//balance right
									//
									//
									eepromAddr = ADDR_A_BAL+i;					//20140224
									nvr_xPutData(eepromAddr,balance[i]);
									//20140221
									//volume_Lch[4]
									//volume_Rch[4]
									//Set_MainVolume(vol,ampType) => L/R Tx
									Set_MainVolume(volume_Rch[i],i);
								}
                                //Tx status: 20140213
                                Func_GetCurrentBalance(i);
                        	}else{
                        		// Can not change balance , so display current vol level
								Func_VolumeLchStatus((i));
								Func_VolumeRchStatus((i));
                        	}
                        }else{
                        	//mute on
#ifdef ASCiiKEY
							Func_MuteStatus(ch_muteOn[i],i);
#endif
                        }
                    }else{
                    	//AMP off!
						//AMP CH status
						Func_AMP_CH_status(i);                  	
                    }
				} 
			}else{
				//current_key[4]
				//All CH balance setting
				for (i=0;i<4;i++) {
					//20140409 : AMP on/off ?
					if (amp_on_off[i] == 1 ) {		//?amp_on_off[]==on
						//20140409 : mute ?
						if (ch_muteOn[i] == MUTE_OFF) {
							//
							//balance[i] = bal;
							if (balance[i] < 30) {
								++balance[i];			//balance right
								//
								//
								eepromAddr = ADDR_A_BAL+i;					//20140224
								nvr_xPutData(eepromAddr,balance[i]);
								//20140221
								//current_key[4]
                            	//Set_MainVolume(vol,ampType) => L/R Tx
								Set_MainVolume(current_key[i],i);
							}
                            //Tx status: 20140213
                            Func_GetCurrentBalance(i);
                        }else{
                        	//mute on
#ifdef ASCiiKEY
							Func_MuteStatus(ch_muteOn[i],i);
#endif
                        }
                    }else{
                    	//AMP off!
 						//AMP CH status
						Func_AMP_CH_status(i);                  	
                    }
				} 
			}
		}else{
			//20140401 :
			if (front_trimKnob_enable == OFF) {
				//20140409 : AMP on/off ?
				if (amp_on_off[ampType] == 1 ) {		//?amp_on_off[]==on
					//20140409 : mute ?
					if (ch_muteOn[ampType] == MUTE_OFF) {
						//		
						//? Lch == Rch
						if ( volume_Lch[ampType] == volume_Rch[ampType] ) {
							//
							//
							//balance[ampType] = bal;
							if (balance[ampType] < 30) {
								++balance[ampType];			//balance right
								//
								//
								eepromAddr = ADDR_A_BAL+ampType;					//20140224
								nvr_xPutData(eepromAddr,balance[ampType]);
								//20140221
								//volume_Lch[4]
								//volume_Rch[4]
                            	//Set_MainVolume(vol,ampType) => L/R Tx
								Set_MainVolume(volume_Rch[ampType],ampType);
							}
                            //Tx status: 20140213
                            Func_GetCurrentBalance(ampType);
                    	}else{
                    		// Can not change balance , so display current vol level
							Func_VolumeLchStatus((ampType));
							Func_VolumeRchStatus((ampType));
                    	}
                    }else{
                    	//mute on
#ifdef ASCiiKEY
						Func_MuteStatus(ch_muteOn[ampType],ampType);
#endif
                    }
                }else{
					//AMP off!
					//AMP CH status
					Func_AMP_CH_status(ampType);                  	
                }
            }else{
				//20140409 : AMP on/off ?
				if (amp_on_off[ampType] == 1 ) {		//?amp_on_off[]==on
					//20140409 : mute ?
					if (ch_muteOn[ampType] == MUTE_OFF) {
						//		
						//current_key[4]
						//
						//balance[ampType] = bal;
						if (balance[ampType] < 30) {
							++balance[ampType];			//balance right
							//
							//
							eepromAddr = ADDR_A_BAL+ampType;					//20140224
							nvr_xPutData(eepromAddr,balance[ampType]);
							//20140221
							//current_key[4]
                        	//Set_MainVolume(vol,ampType) => L/R Tx
							Set_MainVolume(current_key[ampType],ampType);
						}
                        //Tx status: 20140213
                        Func_GetCurrentBalance(ampType);
                    }else{
                    	//mute on
#ifdef ASCiiKEY
						Func_MuteStatus(ch_muteOn[ampType],ampType);
#endif
                    }
                }else{
					//AMP off!
					//AMP CH status
					Func_AMP_CH_status(ampType);                  	
                }
            }
        }
    }
}
//--------------------------------------------------------------------
//20140408
//20140409 : check amp on/off & mute on/off
void 	Func_BalanceLeft(unsigned char ampType)
{
	unsigned char i;

	if(Power_GetPowerStatus() > 0) {		//?power off(0), skip
		//
		//Now unit-power on => 
		if (ampType > 3) {
			//20140401 :
			if (front_trimKnob_enable == OFF) {
				//volume_Lch[4]
				//volume_Rch[4]
				//All CH balance setting
				for (i=0;i<4;i++) {
					//20140409 : AMP on/off ?
					if (amp_on_off[i] == 1 ) {		//?amp_on_off[]==on
						//20140409 : mute ?
						if (ch_muteOn[i] == MUTE_OFF) {
							//? Lch == Rch
							if ( volume_Lch[i] == volume_Rch[i] ) {
								//
								//balance[i] = bal;
								if (balance[i] > 0) {
									--balance[i];			//balance left
									//
									//
									eepromAddr = ADDR_A_BAL+i;					//20140224
									nvr_xPutData(eepromAddr,balance[i]);
									//20140221
									//volume_Lch[4]
									//volume_Rch[4]
									//Set_MainVolume(vol,ampType) => L/R Tx
									Set_MainVolume(volume_Rch[i],i);
								}
                                //Tx status: 20140213
                                Func_GetCurrentBalance(i);
                            }else{
                            	// Can not change balance , so display current vol level
								Func_VolumeLchStatus((i));
								Func_VolumeRchStatus((i));
                            }
                        }else{
                        	//mute on
#ifdef ASCiiKEY
							Func_MuteStatus(ch_muteOn[i],i);
#endif
                        }
                    }else{
                    	//AMP off!
						//AMP CH status
						Func_AMP_CH_status(i);                  	
                    }
				} 
			}else{
				//current_key[4]
				//All CH balance setting
				for (i=0;i<4;i++) {
					//20140409 : AMP on/off ?
					if (amp_on_off[i] == 1 ) {		//?amp_on_off[]==on
						//20140409 : mute ?
						if (ch_muteOn[i] == MUTE_OFF) {
							//
							//balance[i] = bal;
							if (balance[i] > 0) {
								--balance[i];			//balance left
								//
								//
								eepromAddr = ADDR_A_BAL+i;					//20140224
								nvr_xPutData(eepromAddr,balance[i]);
								//20140221
								//current_key[4]
                            	//Set_MainVolume(vol,ampType) => L/R Tx
								Set_MainVolume(current_key[i],i);
							}
                            //Tx status: 20140213
                            Func_GetCurrentBalance(i);
                        }else{
                        	//mute on
#ifdef ASCiiKEY
							Func_MuteStatus(ch_muteOn[i],i);
#endif
                        }
                    }else{
                    	//AMP off!
						//AMP CH status
						Func_AMP_CH_status(i);                  	
                    }
				} 
			}
		}else{
			//20140401 :
			if (front_trimKnob_enable == OFF) {
				//20140409 : AMP on/off ?
				if (amp_on_off[ampType] == 1 ) {		//?amp_on_off[]==on
					//20140409 : mute ?
					if (ch_muteOn[ampType] == MUTE_OFF) {
						//		
						//? Lch == Rch
						if ( volume_Lch[ampType] == volume_Rch[ampType] ) {
							//
							//
							//balance[ampType] = bal;
							if (balance[ampType] > 0) {
								--balance[ampType];			//balance left
								//
								//
								eepromAddr = ADDR_A_BAL+ampType;					//20140224
								nvr_xPutData(eepromAddr,balance[ampType]);
								//20140221
								//volume_Lch[4]
								//volume_Rch[4]
                            	//Set_MainVolume(vol,ampType) => L/R Tx
								Set_MainVolume(volume_Rch[ampType],ampType);
							}
                            //Tx status: 20140213
                            Func_GetCurrentBalance(ampType);
                        }else{
                        	// Can not change balance , so display current vol level
							Func_VolumeLchStatus((ampType));
							Func_VolumeRchStatus((ampType));
                        }
                    }else{
                    	//mute on
#ifdef ASCiiKEY
						Func_MuteStatus(ch_muteOn[ampType],ampType);
#endif
                    }
                }else{
					//AMP off!
					//AMP CH status
					Func_AMP_CH_status(ampType);                  	
                }
            }else{
				//20140409 : AMP on/off ?
				if (amp_on_off[ampType] == 1 ) {		//?amp_on_off[]==on
					//20140409 : mute ?
					if (ch_muteOn[ampType] == MUTE_OFF) {
						//		
						//current_key[4]
						//
						//balance[ampType] = bal;
						if (balance[ampType] > 0) {
							--balance[ampType];			//balance left
							//
							//
							eepromAddr = ADDR_A_BAL+ampType;					//20140224
							nvr_xPutData(eepromAddr,balance[ampType]);
							//20140221
							//current_key[4]
                        	//Set_MainVolume(vol,ampType) => L/R Tx
							Set_MainVolume(current_key[ampType],ampType);
						}
                        //Tx status: 20140213
                        Func_GetCurrentBalance(ampType);
                    }else{
                    	//mute on
#ifdef ASCiiKEY
						Func_MuteStatus(ch_muteOn[ampType],ampType);
#endif
                    }
                }else{
					//AMP off!
					//AMP CH status
					Func_AMP_CH_status(ampType);                  	
                }
            }
        }
    }
}




//20140304
/*
	RS232 key
	volume_Lch[4] : [0 => 96]
	volume_Rch[4] : [0 => 96]
*********************************************************/
void	Func_VolUp(unsigned char ampType)
{
	unsigned char i;

	if(Power_GetPowerStatus() > 0) {		//?power off(0), skip

		if (ampType > 3) {
			//All-ch vol up
			if (front_trimKnob_enable == OFF) {
				//
				for(i=0;i<4;i++){
					if (amp_on_off[i] == 1 ) {		//?amp_on_off[]==on
						//? compare (L_ch)&(R_ch) > Max(96)
						if ( (volume_Lch[i] < MAX_MASTER_VOLUME) && (volume_Rch[i] < MAX_MASTER_VOLUME) ) {
							volume_Lch[i] += 1;
							eepromAddr = ADDR_A_VOL_LCH+i;					//20140224
							nvr_xPutData(eepromAddr,volume_Lch[i]);										
							volume_Rch[i] += 1;
							eepromAddr = ADDR_A_VOL_RCH+i;					//20140224
							nvr_xPutData(eepromAddr,volume_Rch[i]);
							//Tx L/R ch									
							//20140306 : if same Lch & Rch and then have to balance calculation
							if ( volume_Lch[i] != volume_Rch[i] ) {
								Set_MainVolumeLeft((unsigned char)volume_Lch[i],(i));      //=>RKB_function.c
								Set_MainVolumeRight((unsigned char)volume_Rch[i],(i));      //=>RKB_function.c
							}else{
								//Tx L/R ch								
								//(Lch = Rch) calculate balance value!
								Set_MainVolume((unsigned char)volume_Lch[i],(i));
							}
						}
		    			//rs232_printf("ASCiiKey=>%x\n",cmd_code);
		    			Func_VolumeLchStatus((i));
		    			Func_VolumeRchStatus((i));
		    			//20140401 : mute flag off
		    			//
						ch_muteOn[i] = MUTE_OFF;	    			
		    		}
				}
			}		        	
		}else{		        	
			//ch vol up
			if (front_trimKnob_enable == OFF) {
		    	if (amp_on_off[ampType] == 1 ) {	//?amp_on_off[]==on
					//? compare (L_ch)&(R_ch) > Max(96)
					if ( (volume_Lch[ampType] < MAX_MASTER_VOLUME) && (volume_Rch[ampType] < MAX_MASTER_VOLUME) ) {
						volume_Lch[ampType] += 1;
						eepromAddr = ADDR_A_VOL_LCH+ampType;					//20140224
						nvr_xPutData(eepromAddr,volume_Lch[ampType]);										
						volume_Rch[ampType] += 1;
						eepromAddr = ADDR_A_VOL_RCH+ampType;					//20140224
						nvr_xPutData(eepromAddr,volume_Rch[ampType]);
						//Tx L/R ch									
						//20140403 : if same Lch & Rch and then have to balance calculation
						if ( volume_Lch[ampType] != volume_Rch[ampType] ) {
							Set_MainVolumeLeft((unsigned char)volume_Lch[ampType],(ampType));      //=>RKB_function.c
							Set_MainVolumeRight((unsigned char)volume_Rch[ampType],(ampType));      //=>RKB_function.c
						}else{
							//Tx L/R ch								
							//(Lch = Rch) calculate balance value!
							Set_MainVolume((unsigned char)volume_Lch[ampType],(ampType));
						}
					}
		    		//rs232_printf("ASCiiKey=>%x\n",cmd_code);
		    		Func_VolumeLchStatus((ampType));
		    		Func_VolumeRchStatus((ampType));
		    		//20140401 : mute flag off
		    		//
					ch_muteOn[ampType] = MUTE_OFF;	    			
		    	}
		    }
		}
	}
}
/*
	RS232 key
	volume_Lch[4] : [0 => 96]
	volume_Rch[4] : [0 => 96]
*********************************************************/
void	Func_VolDown(unsigned char ampType)
{
	unsigned char i;

	if(Power_GetPowerStatus() > 0) {		//?power off(0), skip

		if (ampType > 3) {
			//All-ch vol down
			if (front_trimKnob_enable == OFF) {
				for(i=0;i<4;i++){
					if (amp_on_off[i] == 1 ) {		//?amp_on_off[]==on
						//? compare (L_ch)&(R_ch) > 0
						if ( (volume_Lch[i] > 0) && (volume_Rch[i] > 0) ) {
							volume_Lch[i] -= 1;
							eepromAddr = ADDR_A_VOL_LCH+i;					//20140224
							nvr_xPutData(eepromAddr,volume_Lch[i]);										
							volume_Rch[i] -= 1;
							eepromAddr = ADDR_A_VOL_RCH+i;					//20140224
							nvr_xPutData(eepromAddr,volume_Rch[i]);
							//Tx L/R ch									
							//20140306 : if same Lch & Rch and then have to balance calculation
							if ( volume_Lch[i] != volume_Rch[i] ) {
								Set_MainVolumeLeft((unsigned char)volume_Lch[i],(i));      //=>RKB_function.c
								Set_MainVolumeRight((unsigned char)volume_Rch[i],(i));      //=>RKB_function.c
							}else{
								//Tx L/R ch								
								//(Lch = Rch) calculate balance value!
								Set_MainVolume((unsigned char)volume_Lch[i],(i));
							}
						}
		    			//rs232_printf("ASCiiKey=>%x\n",cmd_code);
		    			Func_VolumeLchStatus((i));
		    			Func_VolumeRchStatus((i));
		    			//20140401 : mute flag off
		    			//
						ch_muteOn[i] = MUTE_OFF;	    			
		    		}
				}
			}		        	
		}else{		        	
			//ch vol down
			if (front_trimKnob_enable == OFF) {
		    	if (amp_on_off[ampType] == 1 ) {	//?amp_on_off[]==on
					//? compare (L_ch)&(R_ch) > 0
					if ( (volume_Lch[ampType] > 0) && (volume_Rch[ampType] > 0) ) {
						volume_Lch[ampType] -= 1;
						eepromAddr = ADDR_A_VOL_LCH+ampType;					//20140224
						nvr_xPutData(eepromAddr,volume_Lch[ampType]);										
						volume_Rch[ampType] -= 1;
						eepromAddr = ADDR_A_VOL_RCH+ampType;					//20140224
						nvr_xPutData(eepromAddr,volume_Rch[ampType]);
						//Tx L/R ch									
						//20140403 : if same Lch & Rch and then have to balance calculation
						if ( volume_Lch[ampType] != volume_Rch[ampType] ) {
							Set_MainVolumeLeft((unsigned char)volume_Lch[ampType],(ampType));      //=>RKB_function.c
							Set_MainVolumeRight((unsigned char)volume_Rch[ampType],(ampType));      //=>RKB_function.c
						}else{
							//Tx L/R ch								
							//(Lch = Rch) calculate balance value!
							Set_MainVolume((unsigned char)volume_Lch[ampType],(ampType));
						}
					}
		    		//rs232_printf("ASCiiKey=>%x\n",cmd_code);
		    		Func_VolumeLchStatus((ampType));
		    		Func_VolumeRchStatus((ampType));
		    		//20140401 : mute flag off
		    		//
					ch_muteOn[ampType] = MUTE_OFF;	    			
		    	}
		    }
		}
	}
}
/*
	RS232 key
	volume_Lch[4] : [0 => 96]
	volume_Rch[4] : [0 => 96]
	
	unbalance!
*********************************************************/
void	Func_VolLeftChUp(unsigned char ampType)
{
	unsigned char i;

	if(Power_GetPowerStatus() > 0) {		//?power off(0), skip

		if (ampType > 3) {
			//All-Lch vol up
			if (front_trimKnob_enable == OFF) {
				//
				for(i=0;i<4;i++){
					if (amp_on_off[i] == 1 ) {		//?amp_on_off[]==on
						//? compare (L_ch) > Max(96)
						if (volume_Lch[i] < MAX_MASTER_VOLUME) {
							volume_Lch[i] += 1;
							eepromAddr = ADDR_A_VOL_LCH+i;					//20140224
							nvr_xPutData(eepromAddr,volume_Lch[i]);										
							//volume_Rch[i] += 1;
							//eepromAddr = ADDR_A_VOL_RCH+i;					//20140224
							//nvr_xPutData(eepromAddr,volume_Rch[i]);
							//reset balance value
							balance[i] = BALANCE_VOLUME_INITIAL;
							eepromAddr = ADDR_A_BAL+i;					//20140224
							nvr_xPutData(eepromAddr,balance[i]);
							//Tx L/R ch	& unbalance!							
							Set_MainVolumeLeft((unsigned char)volume_Lch[i],(i));      //=>RKB_function.c
							Set_MainVolumeRight((unsigned char)volume_Rch[i],(i));      //=>RKB_function.c
						}
		    			//rs232_printf("ASCiiKey=>%x\n",cmd_code);
		    			Func_VolumeLchStatus((i));
		    			//Func_VolumeRchStatus((i));
		    			//20140401 : mute flag off
		    			//
						ch_muteOn[i] = MUTE_OFF;	    			
		    		}
				}
			}		        	
		}else{		        	
			//ch vol up
			if (front_trimKnob_enable == OFF) {
		    	if (amp_on_off[ampType] == 1 ) {	//?amp_on_off[]==on
					//? compare (L_ch) > Max(96)
					if (volume_Lch[ampType] < MAX_MASTER_VOLUME) {
						volume_Lch[ampType] += 1;
						eepromAddr = ADDR_A_VOL_LCH+ampType;					//20140224
						nvr_xPutData(eepromAddr,volume_Lch[ampType]);										
						//volume_Rch[ampType] += 1;
						//eepromAddr = ADDR_A_VOL_RCH+ampType;					//20140224
						//nvr_xPutData(eepromAddr,volume_Rch[ampType]);
						//reset balance value
						balance[ampType] = BALANCE_VOLUME_INITIAL;
						eepromAddr = ADDR_A_BAL+ampType;					//20140224
						nvr_xPutData(eepromAddr,balance[ampType]);
						//Tx L/R ch	& unbalance							
						Set_MainVolumeLeft((unsigned char)volume_Lch[ampType],(ampType));      //=>RKB_function.c
						Set_MainVolumeRight((unsigned char)volume_Rch[ampType],(ampType));      //=>RKB_function.c
					}
		    		//rs232_printf("ASCiiKey=>%x\n",cmd_code);
		    		Func_VolumeLchStatus((ampType));
		    		//Func_VolumeRchStatus((ampType));
		    		//20140401 : mute flag off
		    		//
					ch_muteOn[ampType] = MUTE_OFF;	    			
		    	}
		    }
		}
	}
}
/*
	RS232 key
	volume_Lch[4] : [0 => 96]
	volume_Rch[4] : [0 => 96]
	
	unbalance !
*********************************************************/
void	Func_VolLeftChDown(unsigned char ampType)
{
	unsigned char i;

	if(Power_GetPowerStatus() > 0) {		//?power off(0), skip

		if (ampType > 3) {
			//All-ch vol down
			if (front_trimKnob_enable == OFF) {
				//
				for(i=0;i<4;i++){
					if (amp_on_off[i] == 1 ) {		//?amp_on_off[]==on
						//? compare (L_ch) > 0
						if (volume_Lch[i] > 0) {
							volume_Lch[i] -= 1;
							eepromAddr = ADDR_A_VOL_LCH+i;					//20140224
							nvr_xPutData(eepromAddr,volume_Lch[i]);										
							//volume_Rch[i] -= 1;
							//eepromAddr = ADDR_A_VOL_RCH+i;					//20140224
							//nvr_xPutData(eepromAddr,volume_Rch[i]);
							//reset balance value
							balance[i] = BALANCE_VOLUME_INITIAL;
							eepromAddr = ADDR_A_BAL+i;					//20140224
							nvr_xPutData(eepromAddr,balance[i]);
							//Tx L/R ch	& unbalance										
							Set_MainVolumeLeft((unsigned char)volume_Lch[i],(i));      //=>RKB_function.c
							Set_MainVolumeRight((unsigned char)volume_Rch[i],(i));      //=>RKB_function.c
						}
		    			//rs232_printf("ASCiiKey=>%x\n",cmd_code);
		    			Func_VolumeLchStatus((i));
		    			//Func_VolumeRchStatus((i));
		    			//20140401 : mute flag off
		    			//
						ch_muteOn[i] = MUTE_OFF;	    			
		    		}
				}
			}		        	
		}else{		        	
			//ch vol down
			if (front_trimKnob_enable == OFF) {
		    	if (amp_on_off[ampType] == 1 ) {	//?amp_on_off[]==on
					//? compare (L_ch) > 0
					if (volume_Lch[ampType] > 0) {
						volume_Lch[ampType] -= 1;
						eepromAddr = ADDR_A_VOL_LCH+ampType;					//20140224
						nvr_xPutData(eepromAddr,volume_Lch[ampType]);										
						//volume_Rch[ampType] -= 1;
						//eepromAddr = ADDR_A_VOL_RCH+ampType;					//20140224
						//nvr_xPutData(eepromAddr,volume_Rch[ampType]);
						//reset balance value
						balance[ampType] = BALANCE_VOLUME_INITIAL;
						eepromAddr = ADDR_A_BAL+ampType;					//20140224
						nvr_xPutData(eepromAddr,balance[ampType]);
						//Tx L/R ch	& unbalance										
						Set_MainVolumeLeft((unsigned char)volume_Lch[ampType],(ampType));      //=>RKB_function.c
						Set_MainVolumeRight((unsigned char)volume_Rch[ampType],(ampType));      //=>RKB_function.c
					}
		    		//rs232_printf("ASCiiKey=>%x\n",cmd_code);
		    		Func_VolumeLchStatus((ampType));
		    		//Func_VolumeRchStatus((ampType));
		    		//20140401 : mute flag off
		    		//
					ch_muteOn[ampType] = MUTE_OFF;	    			
		    	}
		    }
		}
	}
}
/*
	RS232 key
	volume_Lch[4] : [0 => 96]
	volume_Rch[4] : [0 => 96]
*********************************************************/
void	Func_VolRightChUp(unsigned char ampType)
{
	unsigned char i;

	if(Power_GetPowerStatus() > 0) {		//?power off(0), skip

		if (ampType > 3) {
			//All-Lch vol up
			if (front_trimKnob_enable == OFF) {
				//
				for(i=0;i<4;i++){
					if (amp_on_off[i] == 1 ) {		//?amp_on_off[]==on
						//? compare (R_ch) > Max(96)
						if (volume_Rch[i] < MAX_MASTER_VOLUME) {
							//volume_Lch[i] += 1;
							//eepromAddr = ADDR_A_VOL_LCH+i;					//20140224
							//nvr_xPutData(eepromAddr,volume_Lch[i]);										
							volume_Rch[i] += 1;
							eepromAddr = ADDR_A_VOL_RCH+i;					//20140224
							nvr_xPutData(eepromAddr,volume_Rch[i]);
							//reset balance value
							balance[i] = BALANCE_VOLUME_INITIAL;
							eepromAddr = ADDR_A_BAL+i;					//20140224
							nvr_xPutData(eepromAddr,balance[i]);
							//Tx L/R ch	& unbalance										
							Set_MainVolumeLeft((unsigned char)volume_Lch[i],(i));      //=>RKB_function.c
							Set_MainVolumeRight((unsigned char)volume_Rch[i],(i));      //=>RKB_function.c
						}
		    			//rs232_printf("ASCiiKey=>%x\n",cmd_code);
		    			//Func_VolumeLchStatus((i));
		    			Func_VolumeRchStatus((i));
		    			//20140401 : mute flag off
		    			//
						ch_muteOn[i] = MUTE_OFF;	    			
		    		}
				}
			}		        	
		}else{		        	
			//ch vol up
			if (front_trimKnob_enable == OFF) {
		    	if (amp_on_off[ampType] == 1 ) {	//?amp_on_off[]==on
					//? compare (R_ch) > Max(96)
					if (volume_Rch[ampType] < MAX_MASTER_VOLUME) {
						//volume_Lch[ampType] += 1;
						//eepromAddr = ADDR_A_VOL_LCH+ampType;					//20140224
						//nvr_xPutData(eepromAddr,volume_Lch[ampType]);										
						volume_Rch[ampType] += 1;
						eepromAddr = ADDR_A_VOL_RCH+ampType;					//20140224
						nvr_xPutData(eepromAddr,volume_Rch[ampType]);
						//reset balance value
						balance[ampType] = BALANCE_VOLUME_INITIAL;
						eepromAddr = ADDR_A_BAL+ampType;					//20140224
						nvr_xPutData(eepromAddr,balance[ampType]);
						//Tx L/R ch	& unbalance										
						Set_MainVolumeLeft((unsigned char)volume_Lch[ampType],(ampType));      //=>RKB_function.c
						Set_MainVolumeRight((unsigned char)volume_Rch[ampType],(ampType));      //=>RKB_function.c
					}
		    		//rs232_printf("ASCiiKey=>%x\n",cmd_code);
		    		//Func_VolumeLchStatus((ampType));
		    		Func_VolumeRchStatus((ampType));
		    		//20140401 : mute flag off
		    		//
					ch_muteOn[ampType] = MUTE_OFF;	    			
		    	}
		    }
		}
	}
}
/*
	RS232 key
	volume_Lch[4] : [0 => 96]
	volume_Rch[4] : [0 => 96]
*********************************************************/
void	Func_VolRightChDown(unsigned char ampType)
{
	unsigned char i;

	if(Power_GetPowerStatus() > 0) {		//?power off(0), skip

		if (ampType > 3) {
			//All-ch vol down
			if (front_trimKnob_enable == OFF) {
				//
				for(i=0;i<4;i++){
					if (amp_on_off[i] == 1 ) {		//?amp_on_off[]==on
						//? compare (R_ch) > 0
						if (volume_Rch[i] > 0) {
							//volume_Lch[i] -= 1;
							//eepromAddr = ADDR_A_VOL_LCH+i;					//20140224
							//nvr_xPutData(eepromAddr,volume_Lch[i]);										
							volume_Rch[i] -= 1;
							eepromAddr = ADDR_A_VOL_RCH+i;					//20140224
							nvr_xPutData(eepromAddr,volume_Rch[i]);
							//reset balance value
							balance[i] = BALANCE_VOLUME_INITIAL;
							eepromAddr = ADDR_A_BAL+i;					//20140224
							nvr_xPutData(eepromAddr,balance[i]);
							//Tx L/R ch	& unbalance										
							Set_MainVolumeLeft((unsigned char)volume_Lch[i],(i));      //=>RKB_function.c
							Set_MainVolumeRight((unsigned char)volume_Rch[i],(i));      //=>RKB_function.c
						}
		    			//rs232_printf("ASCiiKey=>%x\n",cmd_code);
		    			//Func_VolumeLchStatus((i));
		    			Func_VolumeRchStatus((i));
		    			//20140401 : mute flag off
		    			//
						ch_muteOn[i] = MUTE_OFF;	    			
		    		}
				}
			}		        	
		}else{		        	
			//ch vol down
			if (front_trimKnob_enable == OFF) {
		    	if (amp_on_off[ampType] == 1 ) {	//?amp_on_off[]==on
					//? compare (R_ch) > 0
					if (volume_Rch[ampType] > 0) {
						//volume_Lch[ampType] -= 1;
						//eepromAddr = ADDR_A_VOL_LCH+ampType;					//20140224
						//nvr_xPutData(eepromAddr,volume_Lch[ampType]);										
						volume_Rch[ampType] -= 1;
						eepromAddr = ADDR_A_VOL_RCH+ampType;					//20140224
						nvr_xPutData(eepromAddr,volume_Rch[ampType]);
						//reset balance value
						balance[ampType] = BALANCE_VOLUME_INITIAL;
						eepromAddr = ADDR_A_BAL+ampType;					//20140224
						nvr_xPutData(eepromAddr,balance[ampType]);
						//Tx L/R ch	& unbalance										
						Set_MainVolumeLeft((unsigned char)volume_Lch[ampType],(ampType));      //=>RKB_function.c
						Set_MainVolumeRight((unsigned char)volume_Rch[ampType],(ampType));      //=>RKB_function.c
					}
		    		//rs232_printf("ASCiiKey=>%x\n",cmd_code);
		    		//Func_VolumeLchStatus((ampType));
		    		Func_VolumeRchStatus((ampType));
		    		//20140401 : mute flag off
		    		//
					ch_muteOn[ampType] = MUTE_OFF;	    			
		    	}
		    }
		}
	}
}
	
/**/



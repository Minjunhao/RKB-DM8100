#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "initmcu.h"
#include "main.h"
#include "uart.h"
#include "timer.h"

#include "adckey.h"
#include "adckey.ext"

#include "proc_function.h"
#include "function.ext"

#include "proc_display.mem"
#include "proc_display.h"
#include "RKB_function.h"

#include "eeprom.h"
#include "i2c.h"

#include "ak4114.h"
#include "ak4114.ext"


#ifdef IPOD_INCLUDE
#include "iPod_config.h"
#endif

/*

	- display	routine
	- program	start	: 
	- revision history
	
	20150302 : V2.44 : add commands : get_input_sel_mode! , input_sel_auto! , input_sel_digital! , input_sel_analog!
					response : 0A:input_sel_mode=auto! , 0A:input_sel_mode=digital! , 0A:input_sel_mode=analog! 
*********************************************************************/

//char sw_version[] = "v1.0.0-090522"__DATE__;
//20140619
char str_RKB_MP1_SWver_date[] =	"V2.4.4-150303";      
char str_RKB_MP1_SWver[] =		"V2.4.4 V1";
char str_RKB_MP1_HWver[13] =	"RKB_v100     ";
//20140619
char str_RKB_MP2_SWver_date[] =	"V2.4.4-150303";      
char str_RKB_MP2_SWver[] =		"V2.4.4 V2";
char str_RKB_MP2_HWver[13] =	"RKB_v200     ";

//VFD
Uint8 	shiftDatIdx[40];				//[4x20] rowD->C->B->A and [2x20] rowD->A : shiftDatIdx[1..40]
//extern Uint8 	shiftDatIdx[40];							//row D -> row C -> row B -> row A
extern Uint8 u8dim;

extern const byte tbl_ASCii[];
//extern char	str_HWver[13];
//extern char	str_SWver[];
extern void __iar_program_start( void );		//20110608
extern const char rxString_source[][17+1];      //20120327


/*
		<Stingray display buffer>
		
		stingray module  ===> make ===> uScreenReading
		uScreen <> uScreenReading
		
		adding Preset/Memory ===> uScreen ===> uAdjCurrent
		uCurrentScreen <> uAdjCurrent
*/
		
/*

*********************************************************************/
void	Set_DisplayBlockCTimer(unsigned char block,unsigned char display_time)
{

	mode_DispBlockC =	block;

	blockc_DisplayTimer	= display_time;
	timer_Scroll =	0;
	counter_Scroll	= 0;
}
/*
	display Model Name
*********************************************************************/
const unsigned char str_RKB850[2][20] = {				/*2345678901234567890*/
														"    ROTEL RKB-850   ",
														"                    "};
//--------------------------------------------------------------------												
const unsigned char str_RKB8100[2][20] = {				/*2345678901234567890*/
														"   ROTEL RKB-8100   ",
														"                    "};
//--------------------------------------------------------------------												
const unsigned char str_RKBD850[2][20] = {				/*2345678901234567890*/
														"   ROTEL RKB-D850   ",
														"                    "};
//--------------------------------------------------------------------												
const unsigned char str_RKBD8100[2][20] = {				/*2345678901234567890*/
														"   ROTEL RKB-D8100  ",
														"                    "};
//--------------------------------------------------------------------		
//20140619										
const unsigned char str_RKBDigital[2][20] = {			/*2345678901234567890*/
														"       ROTEL        ",
														"   RKB-D850/D8100   "};
//--------------------------------------------------------------------												
const unsigned char str_RKBAnalog[2][20] = {			/*2345678901234567890*/
														"       ROTEL        ",
														"    RKB-850/8100    "};
//--------------------------------------------------------------------												
void	Display_modelName(void)
{
	unsigned	char i,j;
	
#ifdef STINGRAY_UTF8_4321
	//==========================================================================
	//? Model
	//20140619 => display Digital / Analog
	//20140725 : add compile option
	//==========================================================================
#ifdef RKB_DIGITAL_ANALOG
	//==========================================================================
    //RKB Model : Digital / Analog
 	//==========================================================================
    switch (RKB_ModelID)
    {
		case RKB_850:	//850
		case RKB_8100:	//8100
			for(j=0;j<2;j++) {
				for(i=0;i<20;i++)	TDISP_BUFF_UTF8.sUtf8Stingray.iDispBuf[j][i] = (unsigned short)str_RKBAnalog[j][i];
			}
			break;
		case RKB_D850:	//D850
		case RKB_D8100:	//D8100
			for(j=0;j<2;j++) {
				for(i=0;i<20;i++)	TDISP_BUFF_UTF8.sUtf8Stingray.iDispBuf[j][i] = (unsigned short)str_RKBDigital[j][i];
			}
			break;
		default:											
			break;
	}
#else	//RKB_DIGITAL_ANALOG
	//==========================================================================
    //RKB Model-ID:PB9,PB8: [ 00=850, 01=8100, 10=D850, 11=D8100 ]
	//==========================================================================
    switch (RKB_ModelID)
    {
		case RKB_850:	//850
			for(j=0;j<2;j++) {
				for(i=0;i<20;i++)	TDISP_BUFF_UTF8.sUtf8Stingray.iDispBuf[j][i] = (unsigned short)str_RKB850[j][i];
			}
			break;
		case RKB_8100:	//8100
			for(j=0;j<2;j++) {
				for(i=0;i<20;i++)	TDISP_BUFF_UTF8.sUtf8Stingray.iDispBuf[j][i] = (unsigned short)str_RKB8100[j][i];
			}
			break;
		case RKB_D850:	//D850
			for(j=0;j<2;j++) {
				for(i=0;i<20;i++)	TDISP_BUFF_UTF8.sUtf8Stingray.iDispBuf[j][i] = (unsigned short)str_RKBD850[j][i];
			}
			break;
		case RKB_D8100:	//D8100
			for(j=0;j<2;j++) {
				for(i=0;i<20;i++)	TDISP_BUFF_UTF8.sUtf8Stingray.iDispBuf[j][i] = (unsigned short)str_RKBD8100[j][i];
			}
			break;
		default:											
			break;
	}
#endif	//RKB_DIGITAL_ANALOG
#endif
	//	
	//
#ifdef DISP_8BIT_ENABLE
	//==========================================================================
	//? Model
	//20140619 => display Digital / Analog
	//20140725 : add compile option
	//==========================================================================
#ifdef RKB_DIGITAL_ANALOG
	//==========================================================================
    //RKB Model : Digital / Analog
 	//==========================================================================
    switch (RKB_ModelID)
    {
		case RKB_850:	//850
		case RKB_8100:	//8100
			for(j=0;j<2;j++) {
				for(i=0;i<20;i++)	TDISP_BUFF.sStingray.DispBuf[j][i] = str_RKBAnalog[j][i];
			}
			break;
		case RKB_D850:	//D850
		case RKB_D8100:	//D8100
			for(j=0;j<2;j++) {
				for(i=0;i<20;i++)	TDISP_BUFF.sStingray.DispBuf[j][i] = str_RKBDigital[j][i];
			}
			break;
		default:											
			break;
	}
#else	//RKB_DIGITAL_ANALOG	
	//==========================================================================
    //RKB Model-ID:PB9,PB8: [ 00=850, 01=8100, 10=D850, 11=D8100 ]
 	//==========================================================================
   switch (RKB_ModelID) {
		case RKB_850:	//850
			for(j=0;j<2;j++) {
				for(i=0;i<20;i++)	TDISP_BUFF.sStingray.DispBuf[j][i] = str_RKB850[j][i];
			}
			break;
		case RKB_8100:	//8100
			for(j=0;j<2;j++) {
				for(i=0;i<20;i++)	TDISP_BUFF.sStingray.DispBuf[j][i] = str_RKB8100[j][i];
			}
			break;
		case RKB_D850:	//D850
			for(j=0;j<2;j++) {
				for(i=0;i<20;i++)	TDISP_BUFF.sStingray.DispBuf[j][i] = str_RKBD850[j][i];
			}
			break;
		case RKB_D8100:	//D8100
			for(j=0;j<2;j++) {
				for(i=0;i<20;i++)	TDISP_BUFF.sStingray.DispBuf[j][i] = str_RKBD8100[j][i];
			}
			break;
		default:											
			break;
	}
#endif	//RKB_DIGITAL_ANALOG
#endif
}
/*

			[ TDISP_TEMP_BUFF ] <== [ TDISP_BUFF ]
			<-- f_initialize = ALLDOTON_DATA
			
			mute-on >(1500msec):relay-on(100msec)->(1600msec)
			
			POWER_ON0_ING 	: (1600msec)
							: ADC enable+PowerOn_exec_1()
			POWER_ON1_ING 	: check Max.time (800msec)
							: AMP./RDY == High(700msec) + 120msec
			POWER_ON2_ING 	: DIR reset & initialize / 850,8100(D850,D8100) : Volume Tx
			POWER_ON4_ING 	: check Max.time (500msec)
							: AMP./RDY == Low, 
								=> set 500msec(800msec) time of REC_mute off
								=> set 2000msec time of amp protection check start
			POWER_ON5_ING 	: 500msec(800msec) + mute-off
							: 2000msec + protection checking
							
*********************************************************************/
void	Display_UpdateDisplayBuffer_20msec(void)		//20060222
{
	unsigned	char i;

		//chd	s:
		if (mode_PowerStatus == POWER_OFF) return;          //20121109

		switch(mode_PowerStatus)	{
			//----------------------------------------------------------------------------
			case	POWER_ON0:
				Display_Block();
				break;
			//----------------------------------------------------------------------------
			case	POWER_ON0_ING:                      
				//->(delay : 1600msec)	-> AMP./RDY=High(500msec,700msec)
				//						-> AMP.12V=High->
				if (--count_PowerOn1Ing == 0) {
                	//-----------------------------------------------
                	//ADC enable
                	PowerOn_exec_1();			
                	//-----------------------------------------------
                	//power sequence Next : 
					mode_PowerStatus = POWER_ON1_ING;
					count_PowerOn1Ing = TIME20MSTO700MS;	//20140325 : 800MS
					//----------------------------------------------------------------	
					//protection mode clear!!!!
    				mode_amp_protect = OFF;   
    				Amp_protectionTime = 0;		//level check time
    				count_delay_AMPProtectionCheck = TIME10MSTO5SEC;    //20131217
					//----------------------------------------------------------------	
				}
				break;
			//----------------------------------------------------------------------------
			case	POWER_ON1_ING:                      
				//sequence Power step1 : 
				//------------------------------------------------------------
				// for PP/MP
				//------------------------------------------------------------
				//	AMP.RDY port check -> Port-Noise 대책이 필요함...
				//	new port AMP./RDY for PP/MP
				//
				// 	check AMP.RDY port High Level ???????????????
				//	=> (after 300msec) DIR / volume Tx
				//------------------------------------------------------------
				if (RKB_mode_PP_MP == 1) {						
					//? PP
					if (Amp_nReadyPortC10Protection == 1) {   	
			    		if (++Amp_protectionTime >= TIME20MSTO300MS) {	//20140325 : 700MS
                			//for Next Step!
							mode_PowerStatus = POWER_ON2_ING;
							count_PowerOn1Ing = TIME20MSTO300MS;		//20140325 : 120MS
							Amp_protectionTime = 0;
							//20140106
							PowerOn_exec_DIR();
							//20140211
							s_NJW1159PortInit();
							break;
						}
					}else{
						Amp_protectionTime = 0;								//20131216
					}
				}else{
					//? W/S
					if (Amp_nReadyPortProtection == 1) {   	
			    		if (++Amp_protectionTime >= TIME20MSTO300MS) {	//20140325 : 700MS
                			//for Next Step!
							mode_PowerStatus = POWER_ON2_ING;
							count_PowerOn1Ing = TIME20MSTO300MS;		//20140325 : 120MS
							Amp_protectionTime = 0;
							//20140106
							PowerOn_exec_DIR();
							//20140211
							s_NJW1159PortInit();
							break;
						}
					}else{
						Amp_protectionTime = 0;								//20131216
					}
				}
				//------------------------------------------
				//->(delay : 800msec) 
				if (--count_PowerOn1Ing == 0) {
                	//for Next Step!
					mode_PowerStatus = POWER_ON2_ING;
					count_PowerOn1Ing = TIME20MSTO20MS;					//20140325 : 60MS
					Amp_protectionTime = 0;
					//20140106
					PowerOn_exec_DIR();
					//20140211
					s_NJW1159PortInit();
				}
				break;
			//----------------------------------------------------------------------------
			case	POWER_ON2_ING:
				//12V OK!                 
				if (--count_PowerOn1Ing == 0) {

					Display_PowerAllBufferClear();          //20120927
                	//-----------------------------------------------
					Display_modelName();
                	//-----------------------------------------------
					//VFD_Init();                             //VFD_scanStop=1;
                	VFD_scanStop = 1;				//20120919
                	count_delayScanVFD = 0;	        //20121011
                	//-----------------------------------------------
                	/// 850,8100(D850,D8100) : Volume Tx
                	PowerOn_exec_2();			
                	//-----------------------------------------------
                	//for Next Step! => POWER_ON4_ING
					mode_PowerStatus = POWER_ON4_ING;
					count_PowerOn1Ing = TIME20MSTO700MS;
                	//-----------------------------------------------
					//protection mode clear!!!!
    				mode_amp_protect = OFF;   
    				Amp_protectionTime = 0;		//level check time
    				count_delay_AMPProtectionCheck = TIME10MSTO5SEC;    //20131217
                	//-----------------------------------------------
				}
				break;
			//----------------------------------------------------------------------------
			case	POWER_ON3_ING:     		//RKB not used                 
				//-> (delay : 200msec)
				if (--count_PowerOn1Ing == 0) {

                	VFD_scanStop = 1;				//20120919
                	count_delayScanVFD = 0;	        //20121011
                	//-----------------------------------------------
               		//PowerOn_exec_3();			
                	//-----------------------------------------------
                	//for Next Step!
					mode_PowerStatus = POWER_ON4_ING;
					count_PowerOn1Ing = TIME20MSTO700MS;
				}
				break;
			//----------------------------------------------------------------------------
			case	POWER_ON4_ING:                      
				//------------------------------------------------------------
				// for PP/MP
				//------------------------------------------------------------
				//	AMP.RDY port = LOW check
				//	new port AMP./RDY for PP/MP
				//
				// 	check AMP.RDY port Low Level 
				//	=> next step start : check start AMP protection : Mute off
				//------------------------------------------------------------
				if ( (RKB_mode_PP_MP == 1)&&(Amp_nReadyPortC10Protection == 0) ) {   	
                	//-----------------------------------------------
                	//for Next Step!
					mode_PowerStatus = POWER_ON5_ING;
					count_PowerOn1Ing = 0;
					//count_PowerOn1Ing = TIME20MSTO100MS;
					break;
				}
				if ( (RKB_mode_PP_MP == 0)&&(Amp_nReadyPortProtection == 0) ) {   	
                	//-----------------------------------------------
                	//for Next Step!
					mode_PowerStatus = POWER_ON5_ING;
					count_PowerOn1Ing = 0;
					//count_PowerOn1Ing = TIME20MSTO100MS;
					break;
				}
				//--------------------------------------------------------------
				//->(check Max.time : 500msec) 
				if (--count_PowerOn1Ing == 0) {
                	//-----------------------------------------------
                	//for Next Step!
					mode_PowerStatus = POWER_ON5_ING;
					count_PowerOn1Ing = 0;
					//count_PowerOn1Ing = TIME20MSTO40MS;	//s/w 11C : 200msec=>400msec
				}
			
				break;
			//----------------------------------------------------------------------------
			case	POWER_ON5_ING:                      
				//AMP./RDY=Low -> (20msec)	->after 800msec(900msec), start Mute off
				//							->after 2000msec, start AMP Protection check
					//20140404
					if ( (RKB_ModelID == RKB_850) || (RKB_ModelID == RKB_8100) ) {
						//Mute_AllMuteOff();
						//call main 100msec -> proc_function.c
						Mute_SetMuteOffTime(COUNT100M_500MS);		//20140325 : 100m -> 500msec
					}else{
						//Mute_AllMuteOff();
						//call main 100msec -> proc_function.c
						Mute_SetMuteOffTime(COUNT100M_800MS);		//20140325 : 500m -> 800msec
					}
                	//-----------------------------------------------
                	//for Next Step!
					mode_PowerStatus = POWER_ON0;
					count_PowerOn1Ing = 0;
                	//-----------------------------------------------
					//start checking AMP Protection 
					//protection mode clear!!!!
    				mode_amp_protect = OFF;   
    				Amp_protectionTime = 0;		//level check time
                	count_delay_AMPProtectionCheck = TIME10MSTO2000MS;
                	//-----------------------------------------------
                	VFD_scanStop = 1;				        //20121011
                	count_delayScanVFD = TIME20MSTO200MS;	//20121011
				break;
			//----------------------------------------------------------------------------
			//----------------------------------------------------------------------------
			case	POWER_OFF:
				//Display_PowerAllBufferClear();                      //20120927 => 20121109 delete
				//Display_BufferClear();							//TDISP_BUFF
				break;
			default:
				break;
		}

		//b_floff_mode	> mode_floff
		if	(mode_floff	==	0)	{
			if (f_initialize == ON) {
				for(i=0;i<40;i++) shiftDatIdx[i] = ALLDOTON_DATA;
			}else{
#ifdef STINGRAY_UTF8_4321
				for(i = 0;i<DISPLAYBUFFER_UTF_MAX_SIZE;i++) TDISP_TEMP_BUFF_UTF8.display_Buffer[i] = TDISP_BUFF_UTF8.display_Buffer[i];
#endif  ///
//
#ifdef DISP_8BIT_ENABLE
				for(i = 0;i<DISPLAYBUFFER_MAX_SIZE;i++) TDISP_TEMP_BUFF.display_Buffer[i] = TDISP_BUFF.display_Buffer[i];
#endif  ///
				//test chd 20100701
				//TDISP_TEMP_BUFF.display_Buffer[76] = tbl_ASCii[((a_Work[6]>>4) & 0x0f)];
				//TDISP_TEMP_BUFF.display_Buffer[77] = tbl_ASCii[(a_Work[6] & 0x0f)];
				//TDISP_TEMP_BUFF.display_Buffer[78] = tbl_ASCii[((a_Work[7]>>4) & 0x0f)];
				//TDISP_TEMP_BUFF.display_Buffer[79] = tbl_ASCii[(a_Work[7] & 0x0f)];
			}
		}else{
			//	FL	off!
			Display_BufferClear();							//TDISP_BUFF
		}
}
/*
		VFD 
*********************************************************************/
void	Display_FrontDispBufferClear(void)
{
	unsigned	char i,j;

#ifdef STINGRAY_UTF8_4321
	for(j=0;j<2;j++)
	{
		for(i=0;i<20;i++) FTX_DISP_BUFF_UTF8.sUtf8Stingray.iDispBuf[j][i] = (unsigned short)' ';		//2bytes
	}
#endif
#ifdef DISP_8BIT_ENABLE
	for(i = 0;i<(DISPLAYBUFFER_MAX_SIZE);i++) FTX_DISP_BUFF.display_Buffer[i] = ' ';		//bytes
#endif
}
/*
		RS232
*********************************************************************/
void	Display_RS232DispBufferClear(void)
{
	unsigned	char i,j;

#ifdef STINGRAY_UTF8_4321
	for(j=0;j<2;j++)
	{
		for(i=0;i<20;i++) TTX_DISP_BUFF_UTF8.sUtf8Stingray.iDispBuf[j][i] = (unsigned short)' ';		//2bytes
	}
#endif
#ifdef DISP_8BIT_ENABLE
	for(i = 0;i<(DISPLAYBUFFER_MAX_SIZE);i++) TTX_DISP_BUFF.display_Buffer[i] = ' ';		//bytes
#endif
}
/*
*********************************************************************/
void	Display_TempBufferClear(void)
{
	unsigned	char i,j;

#ifdef STINGRAY_UTF8_4321
	for(j=0;j<2;j++)
	{
		for(i=0;i<20;i++) TDISP_TEMP_BUFF_UTF8.sUtf8Stingray.iDispBuf[j][i] = (unsigned short)' ';		//2bytes
	}
#endif
#ifdef DISP_8BIT_ENABLE
	for(i = 0;i<(DISPLAYBUFFER_MAX_SIZE);i++) TDISP_TEMP_BUFF.display_Buffer[i] = ' ';		//bytes
#endif
}
/*
*********************************************************************/
void	Display_BufferClear(void)
{
	unsigned	char i,j;

#ifdef STINGRAY_UTF8_4321
	for(j=0;j<2;j++)
	{
		for(i=0;i<20;i++) TDISP_BUFF_UTF8.sUtf8Stingray.iDispBuf[j][i] = (unsigned short)' ';		//2bytes
	}
#endif
#ifdef DISP_8BIT_ENABLE
	for(i = 0;i<(DISPLAYBUFFER_MAX_SIZE);i++) TDISP_BUFF.display_Buffer[i] = ' ';		//bytes
#endif
}
/*
*********************************************************************/
void	Display_AllBufferClear(void)
{
    //20120927 : PowerAllBufferClear 과 비교해서 참조할 것..
	//VFDClearDisplay();								//shiftDatIdx
	//Display_TempBufferClear();						//TDISP_TEMP_BUFF
	//Display_FrontDispBufferClear();					//FTX_DISP_BUFF				
	//Display_RS232DispBufferClear();					//TTX_DISP_BUFF
	Display_BufferClear();							//TDISP_BUFF
}
/*
*********************************************************************/
void VFDClearDisplay(void)
{
	unsigned char i;
	for (i=0;i<40;i++) {
		shiftDatIdx[i] = ' ';
	}
}
/*
	AC	power	On시 call
사용함.
*********************************************************************/
void	Display_PowerAllBufferClear(void)
{
	VFDClearDisplay();								//shiftDatIdx
	Display_TempBufferClear();						//TDISP_TEMP_BUFF
	Display_FrontDispBufferClear();					//FTX_DISP_BUFF				
	Display_RS232DispBufferClear();					//TTX_DISP_BUFF
	Display_BufferClear();							//TDISP_BUFF
}
/*
	AC	power	On시 call

	- display	buffer를	모두 0으로	clear
사용안함...
*********************************************************************/
void    Display_Initial(void){

	Display_BufferClear();							//TDISP_BUFF
	Display_TempBufferClear();
	//	display buffer초기화.
	mode_DispBlockC =	BLOCKC_NORMAL;
	Display_Block();
}




/*
	display AUX
*********************************************************************/
/*
const unsigned char str_AutoFreq[7][6] = {	
											"  48K ",
											" 44.1K",
											"  32K ",
											"  96K ",
											" 88.2K",
											" 192K ",
											"176.4K"};
*/
const unsigned char str_AutoFreq[7][6] = {	/*2345678901234567890*/
											"48K   ",
											"44.1K ",
											"32K   ",
											"96K   ",
											"88.2K ",
											"192K  ",
											"176.4K"};
const unsigned char str_Stream[7][6] = {	/*2345678901234567890*/
											"AC3   ",
											"dts   ",
											"PCM   ",
											"HDCD  ",
											"MP3   ",
											"MPEG  ",
											"Analog"};

const unsigned char str_Empty[4][20] = {	/*2345678901234567890*/
											"                    ",
											"      [empty]       ",
											"                    ",
											"                    "};

/*
	Display AMP Protection
RKB
********************************************************************/
const char tbl_AmpProtectionDisplayData[] = {  
	/*---12345678901234567890-*/
	    "   AMP PROTECTION   "}; 

void Display_AmpProtection(void)
{
	unsigned char i;

#ifdef STINGRAY_UTF8_4321
            for(i=0;i<20;i++) TDISP_BUFF_UTF8.sUtf8Stingray.iDispBuf[0][i] = (unsigned short)tbl_AmpProtectionDisplayData[i];
		    //2nd Line clear
			for(i=0;i<20;i++) TDISP_BUFF_UTF8.sUtf8Stingray.iDispBuf[1][i] = (unsigned short)' ';
#endif
#ifdef DISP_8BIT_ENABLE
	        for(i=0;i<20;i++) TDISP_BUFF.sStingray.DispBuf[0][i] = tbl_AmpProtectionDisplayData[i];
            //2nd Line clear
            for(i=0;i<20;i++) TDISP_BUFF.sStingray.DispBuf[1][i] = ' ';
#endif
}


/*
	display RKB Function

	typ : 0 : AMP-A
	typ : 1 : AMP-B
	typ : 2 : AMP-C
	typ : 3 : AMP-D
*********************************************************************/
const unsigned char str_RKB_Function[5][20] = {    
		/*2345678901234567890*/
        "  AMP-A      VOL    ",
        "  AMP-B      VOL    ",
        "  AMP-C      VOL    ",
        "  AMP-D      VOL    ",
        "                    "};
/******************************************************************************
    //-----------------------------------------------------------------
    //<freq> <Temp.>
    //-----------------------------------------------------------------
******************************************************************************/
void Display_RKB_Function_1LineFunction(unsigned char typ)
{
	unsigned char i;

    //-----------------------------------------------------------------
    //function name, vol
    //-----------------------------------------------------------------
#ifdef STINGRAY_UTF8_4321
		for(i=0;i<20;i++) {
		    TDISP_BUFF_UTF8.sUtf8Stingray.iDispBuf[0][i] = (unsigned short)str_RKB_Function[typ][i];   	//set function name
		    TDISP_BUFF_UTF8.sUtf8Stingray.iDispBuf[1][i] = (unsigned short)str_RKB_Function[4][i];		//clear Line2
		}
#endif
#ifdef DISP_8BIT_ENABLE
		for(i=0;i<20;i++) {
		    TDISP_BUFF.sStingray.DispBuf[0][i] = str_RKB_Function[typ][i];		//set function name
		    TDISP_BUFF.sStingray.DispBuf[1][i] = str_RKB_Function[4][i];		//clear Line2
		}
#endif
        //
        //volume : 20121206
        Display_RKB_Function_1LineVolume(typ);            
}
//------------------------------------------------------------------------------
void Display_RKB_Function_1LineVolume(unsigned char typ)
{
    //-----------------------------------------------------------------
    //volume
    //?mute
    //
    //**12345678901234567890*/
    //*"             VOL MAX",
    //*"             VOL MIN",
    //*"             VOL 45 ",
    //*"              MUTE  ",
    //-----------------------------------------------------------------
	unsigned char vol;
	unsigned short val_10,val_1;

	if(mute_mode == MUTE_ON) {
	    //mute display
		if((b1_CommTimer_250msec & 0x02) == 0x00){			//? 500msec
#ifdef STINGRAY_UTF8_4321
		    TDISP_BUFF_UTF8.sUtf8Stingray.iDispBuf[0][13] = (unsigned short)' ';
		    TDISP_BUFF_UTF8.sUtf8Stingray.iDispBuf[0][14] = (unsigned short)'M';
		    TDISP_BUFF_UTF8.sUtf8Stingray.iDispBuf[0][15] = (unsigned short)'U';
		    TDISP_BUFF_UTF8.sUtf8Stingray.iDispBuf[0][16] = (unsigned short)'T';
		    TDISP_BUFF_UTF8.sUtf8Stingray.iDispBuf[0][17] = (unsigned short)'E';
		    TDISP_BUFF_UTF8.sUtf8Stingray.iDispBuf[0][18] = (unsigned short)' ';
		    TDISP_BUFF_UTF8.sUtf8Stingray.iDispBuf[0][19] = (unsigned short)' ';
#endif
#ifdef DISP_8BIT_ENABLE
		    TDISP_BUFF.sStingray.DispBuf[0][13] = ' ';
		    TDISP_BUFF.sStingray.DispBuf[0][14] = 'M';
		    TDISP_BUFF.sStingray.DispBuf[0][15] = 'U';
		    TDISP_BUFF.sStingray.DispBuf[0][16] = 'T';
		    TDISP_BUFF.sStingray.DispBuf[0][17] = 'E';
		    TDISP_BUFF.sStingray.DispBuf[0][18] = ' ';
		    TDISP_BUFF.sStingray.DispBuf[0][19] = ' ';
#endif
		}else{
#ifdef STINGRAY_UTF8_4321
		    TDISP_BUFF_UTF8.sUtf8Stingray.iDispBuf[0][13] = (unsigned short)' ';
		    TDISP_BUFF_UTF8.sUtf8Stingray.iDispBuf[0][14] = (unsigned short)' ';
		    TDISP_BUFF_UTF8.sUtf8Stingray.iDispBuf[0][15] = (unsigned short)' ';
		    TDISP_BUFF_UTF8.sUtf8Stingray.iDispBuf[0][16] = (unsigned short)' ';
		    TDISP_BUFF_UTF8.sUtf8Stingray.iDispBuf[0][17] = (unsigned short)' ';
		    TDISP_BUFF_UTF8.sUtf8Stingray.iDispBuf[0][18] = (unsigned short)' ';
		    TDISP_BUFF_UTF8.sUtf8Stingray.iDispBuf[0][19] = (unsigned short)' ';
#endif
#ifdef DISP_8BIT_ENABLE
		    TDISP_BUFF.sStingray.DispBuf[0][13] = ' ';
		    TDISP_BUFF.sStingray.DispBuf[0][14] = ' ';
		    TDISP_BUFF.sStingray.DispBuf[0][15] = ' ';
		    TDISP_BUFF.sStingray.DispBuf[0][16] = ' ';
		    TDISP_BUFF.sStingray.DispBuf[0][17] = ' ';
		    TDISP_BUFF.sStingray.DispBuf[0][18] = ' ';
		    TDISP_BUFF.sStingray.DispBuf[0][19] = ' ';
#endif
		}
	}else{
	    //
        //"VOL"
        //
#ifdef STINGRAY_UTF8_4321
		TDISP_BUFF_UTF8.sUtf8Stingray.iDispBuf[0][13] = (unsigned short)'V';
		TDISP_BUFF_UTF8.sUtf8Stingray.iDispBuf[0][14] = (unsigned short)'O';
		TDISP_BUFF_UTF8.sUtf8Stingray.iDispBuf[0][15] = (unsigned short)'L';
		TDISP_BUFF_UTF8.sUtf8Stingray.iDispBuf[0][16] = (unsigned short)' ';
#endif
#ifdef DISP_8BIT_ENABLE
		TDISP_BUFF.sStingray.DispBuf[0][13] = 'V';
		TDISP_BUFF.sStingray.DispBuf[0][14] = 'O';
		TDISP_BUFF.sStingray.DispBuf[0][15] = 'L';
		TDISP_BUFF.sStingray.DispBuf[0][16] = ' ';
#endif

        //----------------------------------------
	    //volume data display
        //----------------------------------------
        vol = current_key[typ];					//RKB volume
	    //
	    //
	    if (vol == MAX_MASTER_VOLUME) {				//?96
		//MAX
#ifdef STINGRAY_UTF8_4321
		    TDISP_BUFF_UTF8.sUtf8Stingray.iDispBuf[0][17] = (unsigned short)'M';
		    TDISP_BUFF_UTF8.sUtf8Stingray.iDispBuf[0][18] = (unsigned short)'A';
		    TDISP_BUFF_UTF8.sUtf8Stingray.iDispBuf[0][19] = (unsigned short)'X';
#endif
#ifdef DISP_8BIT_ENABLE
		    TDISP_BUFF.sStingray.DispBuf[0][17] = 'M';
		    TDISP_BUFF.sStingray.DispBuf[0][18] = 'A';
		    TDISP_BUFF.sStingray.DispBuf[0][19] = 'X';
#endif
	    }else if (vol == 0) {
		//MIN
#ifdef STINGRAY_UTF8_4321
		    TDISP_BUFF_UTF8.sUtf8Stingray.iDispBuf[0][17] = (unsigned short)'M';
		    TDISP_BUFF_UTF8.sUtf8Stingray.iDispBuf[0][18] = (unsigned short)'I';
		    TDISP_BUFF_UTF8.sUtf8Stingray.iDispBuf[0][19] = (unsigned short)'N';
#endif
#ifdef DISP_8BIT_ENABLE
		    TDISP_BUFF.sStingray.DispBuf[0][17] = 'M';
		    TDISP_BUFF.sStingray.DispBuf[0][18] = 'I';
		    TDISP_BUFF.sStingray.DispBuf[0][19] = 'N';
#endif
	    }else{
		    val_10 = (vol)/10;
		    val_1 = (vol - val_10*10 );
#ifdef STINGRAY_UTF8_4321
		    TDISP_BUFF_UTF8.sUtf8Stingray.iDispBuf[0][17] = (unsigned short)val_10 + 0x30;
		    TDISP_BUFF_UTF8.sUtf8Stingray.iDispBuf[0][18] = (unsigned short)val_1 + 0x30;
		    TDISP_BUFF_UTF8.sUtf8Stingray.iDispBuf[0][19] = (unsigned short)' ';
#endif
#ifdef DISP_8BIT_ENABLE
		    TDISP_BUFF.sStingray.DispBuf[0][17] = (unsigned char)val_10 + 0x30;
		    TDISP_BUFF.sStingray.DispBuf[0][18] = (unsigned char)val_1 + 0x30;
		    TDISP_BUFF.sStingray.DispBuf[0][19] = ' ';
#endif
	    }
	}
}
//------------------------------------------------------------------------------
void Display_RKB_Function_2LineTemperature(unsigned char typ)
{
    //-----------------------------------------------------------------
    //Temp. : current_key[4] = 0..99
    //
	/**12345678901234567890**/
	/*"   44.1K  xxxx  xxxx"*/
    //-----------------------------------------------------------------
	unsigned char val;
	unsigned short val16;
	unsigned short val_1000,val_100,val_10,val_1;

	if ( typ == 0 ) {
	    //
        //Temp. = typ = 0
		//current_key[4]		//Temp. value saving
		//current_key[5]		//Temp. value saving
		//
        //ADC Value = typ = 1
		//current_ADC_Val[4] = ADC_Val[4];			//16bit ADC value saving
		//current_ADC_Val[5] = ADC_Val[5];			//16bit ADC value saving
        //
        //----------------------------------------
	    //temp. current_key[4],current_key[5] display - {2 digit]
        //----------------------------------------
        val = current_key[4];					//8bit current_key[4]
	    //
		val_10 = (val)/10;
		val_1 = (val - val_10*10);
#ifdef STINGRAY_UTF8_4321
		TDISP_BUFF_UTF8.sUtf8Stingray.iDispBuf[1][10] = (unsigned short)' ';
		TDISP_BUFF_UTF8.sUtf8Stingray.iDispBuf[1][11] = (unsigned short)' ';
		TDISP_BUFF_UTF8.sUtf8Stingray.iDispBuf[1][12] = (unsigned short)val_10 + 0x30;
		TDISP_BUFF_UTF8.sUtf8Stingray.iDispBuf[1][13] = (unsigned short)val_1 + 0x30;
#endif
#ifdef DISP_8BIT_ENABLE
		TDISP_BUFF.sStingray.DispBuf[1][10] = ' ';
		TDISP_BUFF.sStingray.DispBuf[1][11] = ' ';
		TDISP_BUFF.sStingray.DispBuf[1][12] = (unsigned char)val_10 + 0x30;
		TDISP_BUFF.sStingray.DispBuf[1][13] = (unsigned char)val_1 + 0x30;
#endif
        //----------------------------------------
	    //temp. current_key[4],current_key[5] display - {2 digit]
        //----------------------------------------
        val = current_key[5];					//8bit current_key[5]
	    //
		val_10 = (val)/10;
		val_1 = (val - val_10*10);
#ifdef STINGRAY_UTF8_4321
		TDISP_BUFF_UTF8.sUtf8Stingray.iDispBuf[1][16] = (unsigned short)' ';
		TDISP_BUFF_UTF8.sUtf8Stingray.iDispBuf[1][17] = (unsigned short)' ';
		TDISP_BUFF_UTF8.sUtf8Stingray.iDispBuf[1][18] = (unsigned short)val_10 + 0x30;
		TDISP_BUFF_UTF8.sUtf8Stingray.iDispBuf[1][19] = (unsigned short)val_1 + 0x30;
#endif
#ifdef DISP_8BIT_ENABLE
		TDISP_BUFF.sStingray.DispBuf[1][16] = ' ';
		TDISP_BUFF.sStingray.DispBuf[1][17] = ' ';
		TDISP_BUFF.sStingray.DispBuf[1][18] = (unsigned char)val_10 + 0x30;
		TDISP_BUFF.sStingray.DispBuf[1][19] = (unsigned char)val_1 + 0x30;
#endif
	}else{
		//--------------------------------------------------------------------------------
        //ADC Value = typ = 1
		//current_ADC_Val[4] = ADC_Val[4];			//16bit ADC value saving
		//current_ADC_Val[5] = ADC_Val[5];			//16bit ADC value saving
        //
        //----------------------------------------
	    //ADC current_ADC_Val[4],current_ADC_Val[5] display - {4 digit]
        //----------------------------------------
        val16 = current_ADC_Val[4];					//16bit current_ADC_Val[4]
	    //
		val_1000 = (val16)/1000;
		val_100 = (val16 - val_1000*1000 )/100;
		val_10 = (val16 - (val_1000*1000+val_100*100)/10 );
		val_1 = (val16 - (val_1000*1000+val_100*100+val_10*10) );
#ifdef STINGRAY_UTF8_4321
		TDISP_BUFF_UTF8.sUtf8Stingray.iDispBuf[1][10] = (unsigned short)val_1000 + 0x30;
		TDISP_BUFF_UTF8.sUtf8Stingray.iDispBuf[1][11] = (unsigned short)val_100 + 0x30;
		TDISP_BUFF_UTF8.sUtf8Stingray.iDispBuf[1][12] = (unsigned short)val_10 + 0x30;
		TDISP_BUFF_UTF8.sUtf8Stingray.iDispBuf[1][13] = (unsigned short)val_1 + 0x30;
#endif
#ifdef DISP_8BIT_ENABLE
		TDISP_BUFF.sStingray.DispBuf[1][10] = (unsigned char)val_1000 + 0x30;
		TDISP_BUFF.sStingray.DispBuf[1][11] = (unsigned char)val_100 + 0x30;
		TDISP_BUFF.sStingray.DispBuf[1][12] = (unsigned char)val_10 + 0x30;
		TDISP_BUFF.sStingray.DispBuf[1][13] = (unsigned char)val_1 + 0x30;
#endif
        //----------------------------------------
	    //ADC current_ADC_Val[4],current_ADC_Val[5] display - {4 digit]
        //----------------------------------------
        val16 = current_ADC_Val[5];					//16bit current_ADC_Val[5]
	    //
		val_1000 = (val16)/1000;
		val_100 = (val16 - val_1000*1000 )/100;
		val_10 = (val16 - (val_1000*1000+val_100*100)/10 );
		val_1 = (val16 - (val_1000*1000+val_100*100+val_10*10) );
#ifdef STINGRAY_UTF8_4321
		TDISP_BUFF_UTF8.sUtf8Stingray.iDispBuf[1][16] = (unsigned short)val_1000 + 0x30;
		TDISP_BUFF_UTF8.sUtf8Stingray.iDispBuf[1][17] = (unsigned short)val_100 + 0x30;
		TDISP_BUFF_UTF8.sUtf8Stingray.iDispBuf[1][18] = (unsigned short)val_10 + 0x30;
		TDISP_BUFF_UTF8.sUtf8Stingray.iDispBuf[1][19] = (unsigned short)val_1 + 0x30;
#endif
#ifdef DISP_8BIT_ENABLE
		TDISP_BUFF.sStingray.DispBuf[1][16] = (unsigned char)val_1000 + 0x30;
		TDISP_BUFF.sStingray.DispBuf[1][17] = (unsigned char)val_100 + 0x30;
		TDISP_BUFF.sStingray.DispBuf[1][18] = (unsigned char)val_10 + 0x30;
		TDISP_BUFF.sStingray.DispBuf[1][19] = (unsigned char)val_1 + 0x30;
#endif
	}
}

/*
	typ : 0 : AMP-A
	typ : 1 : AMP-B
	typ : 2 : AMP-C
	typ : 3 : AMP-D
******************************************************************************/
void Display_RKB_Function(unsigned char typ)
{
	unsigned char i;

			//1st line
			Display_RKB_Function_1LineFunction(typ);   
	         
            //2nd line
            //if (b_PllStatus[typ] == PLL_UNLOCK), display TEMP.
            //<freq><tempA tempB> display
            /**12345678901234567890**/
            /*"   44.1K  xxxx  xxxx"*/
	        if (b_StreamDisplayDelay == 0) {
	            //20130131
                if (b_PllStatus[typ] == PLL_UNLOCK) {
                    if (b_StreamFreq_bk[typ] != 7) {
                        b_StreamFreq_bk[typ] = 7;
                        Func_CurrentFreqTx(typ);		//Tx : "freq=44.1!"
                    }
                	goto goto_temp_display;              
                }
                //current freq Tx? : 20130131
                if (b_StreamFreq[typ] != b_StreamFreq_bk[typ]) {
                    b_StreamFreq_bk[typ] = b_StreamFreq[typ];         //backup save
                        Func_CurrentFreqTx(typ);		//Tx : "freq=44.1!"
                }
				//------------------------------------------------------------------------	                
		        if (b_StreamKind[typ] == ST_A_PCM) {
#ifdef STINGRAY_UTF8_4321
			        //StreamFreq
			        if (b_StreamFreq[typ] < 7) {
				        for(i=0;i<6;i++) TDISP_BUFF_UTF8.sUtf8Stingray.iDispBuf[1][i+3] = (unsigned short)str_AutoFreq[b_StreamFreq[typ]][i];
			        }
			        //StreamType or TEMP-A/TEMP-B
			        for(i=0;i<6;i++) TDISP_BUFF_UTF8.sUtf8Stingray.iDispBuf[1][i+13] = (unsigned short)str_Stream[b_StreamKind[typ]][i];
#endif
#ifdef DISP_8BIT_ENABLE
			        //StreamFreq
			        if (b_StreamFreq[typ] < 7) {
				        for(i=0;i<6;i++) TDISP_BUFF.sStingray.DispBuf[1][i+3] = str_AutoFreq[b_StreamFreq[typ]][i];
			        }
			        //StreamType or TEMP-A/TEMP-B
			        for(i=0;i<6;i++) TDISP_BUFF.sStingray.DispBuf[1][i+13] = str_Stream[b_StreamKind[typ]][i];
#endif
		        }
				//------------------------------------------------------------------------
				// TEMP-A/TEMP-B display               
goto_temp_display:
				////Temp data [ 000 .. 100 ]
				//Display_RKB_Function_2LineTemperature(0);	
				////Temp ADC data [ 0000 .. 4095 ]
				Display_RKB_Function_2LineTemperature(1);	
				//
				//------------------------------------------------------------------------	                
	        }else{
		        --b_StreamDisplayDelay;
	        }
}

/*
	Display off
*********************************************************************/
void		Display_Off(void)
{
	VFDClearDisplay();
	Display_FrontDispBufferClear();					//FTX_DISP_BUFF				
}

/*
	block	 display : 20msec
//(mode_func) :
	- if (Power off and Protection),  mode_func = FUNCTION_DISP_OFF
	- if (change Trim value),  mode_func = change AMP-?
	- if (change Freq value),  mode_func = change AMP-?
#define	FUNCTION_DISP_OFF			0
#define	FUNCTION_AMP_A				1
#define	FUNCTION_AMP_B				2
#define	FUNCTION_AMP_C				3
#define	FUNCTION_AMP_D				4
*********************************************************************/
void		Display_Block(void)
{

        //AMP protection display
        if (mode_amp_protect == ON) {
            Display_AmpProtection();
            return;
        }
        //------------------------------------------------------------
		switch(mode_DispBlockC){
			case BLOCKC_NORMAL:
				if(mode_func == FUNCTION_DISP_OFF) {
					Display_Off();
				}else{
					//chd degugging
					//Display_RKB_Function(mode_func-1);	//AMP-A = 0  => 삭제로 인해 b_StreamKind 를 잘 고려해야함..    
				}
				break;
			case BLOCKC_VERSION_DISPLAY:
				Display_VersionDisplay();                  
				break;
			default:
				break;
		}
}

/*
*********************************************************************/
void	Display_PowerOffClearTimer(void)
{
		blockc_DisplayTimer	= 0;
		mode_DispBlockC =	BLOCKC_NORMAL;
		timer_Scroll =	0;
		counter_Scroll	= 0;
}



/*
compare  uTDISP TDISP_TEMP_BUFF < == uTDISP TDISP_BUFF
			uTDISP TTX_DISP_BUFF ==> rs232 ==> PC			
			uTDISP FTX_DISP_BUFF ==>TX,RX  ==> Front-Micom			


		FTX_DISP_BUFF.display_Buffer[DISPLAYBUFFER_MAX_SIZE] : front
		TDISP_TEMP_BUFF.display_Buffer
*********************************************************************/


/*
	display block c의 임시 디스플레이 모니터.
*********************************************************************/
void	Display_BlockCModeCheck_100msec(void)
{
	
	if(blockc_DisplayTimer != 0) {
		if(--blockc_DisplayTimer == 0){
			if(mode_DispBlockC != BLOCKC_NORMAL) {
				//buffer clear!!!
				////test : 20120927
				//Display_BufferClear();							//TDISP_BUFF
			}
            mode_DispBlockC = BLOCKC_NORMAL;
            mode_setup = 0;							// setup start cancel
		}
	}
}


/* RKB
	Block > Setup version
*********************************************12345678901234567890**************************/
const char str_SetupMenu_mainVersion1[20] = "   MAIN :           ";		
//----------------------------------------------------------
void	Display_VersionDisplay(void)
{
	unsigned char i;

#ifdef STINGRAY_UTF8_4321
    //main
	for(i=0;i<20;i++) TDISP_BUFF_UTF8.sUtf8Stingray.iDispBuf[0][i] = (unsigned short)str_SetupMenu_mainVersion1[i];
	//20140619
	//? Model
	if ( (RKB_mode_PP_MP == 1)&&(RKB_mode_PP_nMP2 == 0) ) 
	{
		//? MP2 unit : V2
		//20140819 : display Digital / Analog V2
	    for(i=0;i<9;i++) TDISP_BUFF_UTF8.sUtf8Stingray.iDispBuf[0][i+10] = (unsigned short)str_RKB_MP2_SWver[i];	//9 chars
	}else if ( (RKB_mode_PP_MP == 1)&&(RKB_mode_PP_nMP2 == 1) )	{
		//? PP/MP1 unit : V1
		//20140819 : display Digital / Analog V1
	    for(i=0;i<9;i++) TDISP_BUFF_UTF8.sUtf8Stingray.iDispBuf[0][i+10] = (unsigned short)str_RKB_MP1_SWver[i];	//9 chars
	}
#endif	//STINGRAY_UTF8_4321
//----------------------------------------------------------------------------------------
#ifdef DISP_8BIT_ENABLE
    //main
	for(i=0;i<20;i++) TDISP_BUFF.sStingray.DispBuf[0][i] = str_SetupMenu_mainVersion1[i];
	//20140619
	//? Model
	if ( (RKB_mode_PP_MP == 1)&&(RKB_mode_PP_nMP2 == 0) ) 
	{
		//? MP2 unit : V2
		//20140819 : display Digital / Analog V2
    	for(i=0;i<9;i++) TDISP_BUFF.sStingray.DispBuf[0][i+10] = str_RKB_MP2_SWver[i];		//9 chars
	}else if ( (RKB_mode_PP_MP == 1)&&(RKB_mode_PP_nMP2 == 1) )	{
		//? PP/MP1 unit : V1
		//20140819 : display Digital / Analog
    	for(i=0;i<9;i++) TDISP_BUFF.sStingray.DispBuf[0][i+10] = str_RKB_MP1_SWver[i];		//9 chars
	}
#endif	//DISP_8BIT_ENABLE
}



//------------------------------------------------------------------------------
//
//
//
//
//
//
//volatile byte RotelOpaqueData[50];			//20111201
//
//	PutChar(RotelOpaqueData[i]); 
//	Stingray_Startup_mode <== S_CONFIGURE_OK
//	Stingray_Tx_RotelOpaqueData(m);
/*
    add ID + RotelOpaqueData
    
	(mode,AMPtype,&m)
	mode=0 : 00:
	mode=2 : 0A: or 0B: or 0C: or 0D:

    AMPtype=0: "0A:" + RotelOpaqueData[m]
    AMPtype=1: "0B:" + RotelOpaqueData[m]
    AMPtype=2: "0C:" + RotelOpaqueData[m]
    AMPtype=3: "0D:" + RotelOpaqueData[m]       
    
    "0A:freq=192!"
    "0B:freq=192!"
    "0C:freq=192!"
    "0D:freq=192!"
-----------------------------------------------------------------------------*/
const unsigned char str_modelID_ampType[4][3] = {
	"0A:",
	"0B:",
	"0C:",
	"0D:"};
//----------------------------------------------------------------------------------------
void addID_RoteOpaqueData(unsigned char mode, unsigned char AMPtype, unsigned short* m)
{
	unsigned short i,j;
	
	if ( mode == 0 ) {
		//skip
		return;
/*		//00:xxxxxxx
    	for(i=0,j=*m;i<=*m;i++,j--) {
       	RotelOpaqueData[j+3] = RotelOpaqueData[j];
    	}

        RotelOpaqueData[0] = '0';
        RotelOpaqueData[1] = '0';
        RotelOpaqueData[2] = ':';
    	*m = *m + 3;
*/
	}else{
		//0A:xxxxxx
		//0B:xxxxxx
		//0C:xxxxxx
		//0D:xxxxxx
    	for(i=0,j=*m;i<=*m;i++,j--) {
       	RotelOpaqueData[j+3] = RotelOpaqueData[j];
    	}
        //
    	for(i=0;i<3;i++) {
           	RotelOpaqueData[i] = str_modelID_ampType[AMPtype][i];
        }
    	*m = *m + 3;
    }
}
/*
    RotelOpaqueData Tx => RotelLink-UART4
    
    RotelOpaqueData[m]
    
//--------------------------------------------------------------------------
	RotelOpaqueData[200]
-----------------------------------------------------------------------------*/
void Stingray_Tx_RotelOpaqueData(unsigned short Ssize)
{

	unsigned short i;

	if (Stingray_Startup_mode == S_CONFIGURE_OK) {		//20130516
		
#ifdef UART1_DEBUG_x 		//: 나중에 막아야 함..
		for(i=0;i<Ssize;i++) {								
			PutChar(RotelOpaqueData[i]); 
		}				
#endif
#ifdef UART4_ENABLE
    	//UART4 Tx
		for (i=0;i<Ssize;i++) PutChar4(RotelOpaqueData[i]);		//RotelLink-UART4
#endif
	}
}

/*
    memcpy((void*)TTX_DISP_BUFF_UTF8.display_Buffer,(void*)TDISP_TEMP_BUFF_UTF8.display_Buffer,166);
    memcpy((void*)TTX_DISP_BUFF.display_Buffer,(void*)TDISP_TEMP_BUFF.display_Buffer,DISPLAYBUFFER_MAX_SIZE);

    display=###,text
    
************************************************/
const char str_display[] = "display=";
//--------------------------------------------------------------------
void 	Func_DisplayAll(void)
{

	unsigned char UTF8_buffer[4],UTFcount;
	unsigned short stemp;
	unsigned short m;
	unsigned short i,j,k;
	unsigned char val_100,val_10,val_1;

	
//----------------------------------------------------------	
//
//  display=###,text      
		m=4+8;
//
//RS232 Tx[UTF8]
//
		//start
		//UTF-8 은 가변이므로 ...

        //20120214 temp volume display 고려함..
		//20120214 : Uart45RotelLinkDisplayData_Tx
		//20120214 : Func_DisplayAll,Func_Display1234

		//make RotelUTF8 from Unicode all
		for (j=0;j<2;++j) {									//20101101 : 80 character [Unicode]
			for (i=0;i<20;++i) {							//20101101 : 80 character [Unicode]
#ifdef STINGRAY_UTF8_4321
	    		stemp = TTX_DISP_BUFF_UTF8.sUtf8Stingray.iDispBuf[j][i];
#else
	    		stemp = (unsigned short)TTX_DISP_BUFF.sStingray.DispBuf[j][i];  //20110330
#endif
#ifdef MAKE_UNICODE
				RotelOpaqueData[m++] = (unsigned char)((stemp>>8)&0x00ff);
				RotelOpaqueData[m++] = (unsigned char)(stemp&0x00ff);
#else
				makeRotelUTF8(stemp, UTF8_buffer, &UTFcount);
				for (k=0;k<UTFcount;k++) {
					RotelOpaqueData[m++] = UTF8_buffer[k];
				}
#endif
			}
		}
		//Reciva OpaqueText Tx
		RotelOpaqueData[m] = 0x00;		//null(0x00)
//
//
//
	//(m-4) => decimal MMM,
	val_100 = ((m-4-8)/100);
	val_10 = ((m-4-8) - val_100*100)/10;
	val_1 = ((m-4-8) - val_100*100 - val_10*10 );
	RotelOpaqueData[0+8] = (unsigned char)val_100 + 0x30;;
	RotelOpaqueData[1+8] = (unsigned char)val_10 + 0x30;;
	RotelOpaqueData[2+8] = (unsigned char)val_1 + 0x30;;
	RotelOpaqueData[3+8] = ',';

    for(i=0;(i<10)&&(str_display[i]!='\0');i++) RotelOpaqueData[i]=str_display[i];  //chd 20110322
    //
    //------------------------------------------------------
	//(mode,AMPtype,&m)
	//mode=0 : skip
	//mode=2 : 0A: or 0B: or 0C: or 0D:
	//
    //AMPtype=0: "0A:" + RotelOpaqueData[m]
    //AMPtype=1: "0B:" + RotelOpaqueData[m]
    //AMPtype=2: "0C:" + RotelOpaqueData[m]
    //AMPtype=3: "0D:" + RotelOpaqueData[m]       
    //------------------------------------------------------
	//addID_RoteOpaqueData(0,0,&m);
    //------------------------------------------------------
	//
	//RS232 Tx start
	//
//#ifdef RS232_SKIP	//20110114 test
	//20131212
	if (RS232_TxSkip == 0) {
		rs232_printf("\n");						//20131025
		for(i=0;i<m;i++) {								
			PutChar(RotelOpaqueData[i]);
		}
	}				
#ifdef DISPLAY_AUTOMANUAL_ALL_COMMAND	//20140402
	//20131212 : mode_DisplayUpdate = (manual = 1) = RS232_TxSkip;
	RS232_TxSkip = mode_DisplayUpdate;		//20131212
#else	
	RS232_TxSkip = 0;
#endif	
//#endif
	//
	//Opaque Tx start			
	//
	Stingray_Tx_RotelOpaqueData(m);                 //RotelOpaqueData => UART4
}
/*
    display1=##,text
    display2=##,text
    display3=##,text
    display4=##,text
************************************************/
const char str_display1234[4][9] = {
    "display1=",
    "display2=",
    "display3=",
    "display4=" };
//--------------------------------------------------------------------
void 	Func_Display1234(unsigned char j)
{
	unsigned char UTF8_buffer[4],UTFcount;
	unsigned short stemp;
	unsigned short m;
	unsigned short i,k;


//----------------------------------------------------------	
//  display1=##,text      
		m=3+9;
//
//RS232 Tx[UTF8]
//
		//start
		//UTF-8 은 가변이므로 ...

        //20120214 temp volume display 고려함..
		//20120214 : Uart45RotelLinkDisplayData_Tx
		//20120214 : Func_DisplayAll,Func_Display1234
    		
		//make RotelUTF8 from Unicode all
		for (i=0;i<20;++i) {							//20101101 : 80 character [Unicode]
#ifdef STINGRAY_UTF8_4321
    		stemp = TTX_DISP_BUFF_UTF8.sUtf8Stingray.iDispBuf[j][i];
#else
			stemp = (unsigned short)TTX_DISP_BUFF.sStingray.DispBuf[j][i];  //20110330
#endif
#ifdef MAKE_UNICODE
			RotelOpaqueData[m++] = (unsigned char)((stemp>>8)&0x00ff);
			RotelOpaqueData[m++] = (unsigned char)(stemp&0x00ff);
#else
			makeRotelUTF8(stemp, UTF8_buffer, &UTFcount);
			for (k=0;k<UTFcount;k++) {
				RotelOpaqueData[m++] = UTF8_buffer[k];
			}
#endif
		}
		//Reciva OpaqueText Tx
		RotelOpaqueData[m] = 0x00;		//null(0x00)
//	
//
//
	//(m-3) => decimal MM,
	RotelOpaqueData[0+9] = (unsigned char)((m-3-9)/10 + 0x30);
	RotelOpaqueData[1+9] = (unsigned char)((m-3-9)%10 + 0x30);
	RotelOpaqueData[2+9] = ',';
	
    for(i=0;i<9;i++) RotelOpaqueData[i]=str_display1234[j][i];  //chd 20110322					
	
    //------------------------------------------------------
    //------------------------------------------------------
	//(mode,AMPtype,&m)
	//mode=0 : skip
	//mode=2 : 0A: or 0B: or 0C: or 0D:
	//
    //AMPtype=0: "0A:" + RotelOpaqueData[m]
    //AMPtype=1: "0B:" + RotelOpaqueData[m]
    //AMPtype=2: "0C:" + RotelOpaqueData[m]
    //AMPtype=3: "0D:" + RotelOpaqueData[m]       
    //------------------------------------------------------
    //------------------------------------------------------
	//addID_RoteOpaqueData(1,0,&m);
    //------------------------------------------------------
	//
	//RS232 Tx start
	//
//#ifdef RS232_SKIP	//20110114 test
	//20131212
	if (RS232_TxSkip == 0) {
		rs232_printf("\n");						//20131025
		for(i=0;i<m;i++) {								
			PutChar(RotelOpaqueData[i]);
		}
	}				
#ifdef DISPLAY_AUTOMANUAL_ALL_COMMAND	//20140402
	//20131212 : mode_DisplayUpdate = (manual = 1) = RS232_TxSkip;
	RS232_TxSkip = mode_DisplayUpdate;		//20131212
#else	
	RS232_TxSkip = 0;
#endif	
//#endif
	//
	//Opaque Tx start			
	//
	Stingray_Tx_RotelOpaqueData(m);                 //RotelOpaqueData => UART4
}
/*	RKB
    product_type
************************************************/
const char str_productType_rkb850[] =	"product_type=07,RKB-850";
const char str_productType_rkb8100[] =	"product_type=08,RKB-8100";
const char str_productType_rkbd850[] =	"product_type=08,RKB-D850";
const char str_productType_rkbd8100[] =	"product_type=09,RKB-D8100";
//--------------------------------------------------------------------
//20140619
const char str_productType_rkbDigital[] =	"product_type=14,RKB-D850/D8100";
const char str_productType_rkbAnalog[] =	"product_type=12,RKB-850/8100";
//--------------------------------------------------------------------
//20140619
const char str_productType_rkbDigitalV2[] =	"product_type=17,RKB-D850/D8100 V2";
const char str_productType_rkbAnalogV2[] =	"product_type=15,RKB-850/8100 V2";
//--------------------------------------------------------------------
//20140725
const char str_productType_rkb850V2[] =		"product_type=10,RKB-850 V2";
const char str_productType_rkb8100V2[] =	"product_type=11,RKB-8100 V2";
const char str_productType_rkbd850V2[] =	"product_type=11,RKB-D850 V2";
const char str_productType_rkbd8100v2[] =	"product_type=12,RKB-D8100 V2";
//--------------------------------------------------------------------
//--------------------------------------------------------------------
void 	Func_ProductType(void)
{
	unsigned short i,m;   //20120309

    m=0;
	//? Model
	if ( (RKB_mode_PP_MP == 1)&&(RKB_mode_PP_nMP2 == 0) ) 
	{
	//20140725 : add compile option
#ifdef RKB_DIGITAL_ANALOG
		//
		//? MP2 unit : V2
		//20140619 : display Digital / Analog V2
		if ( (RKB_ModelID == RKB_850)||(RKB_ModelID == RKB_8100) ) {
            	for(i=0;(i<34)&&(str_productType_rkbAnalogV2[i]!='\0');i++) RotelOpaqueData[m++] = (unsigned char)str_productType_rkbAnalogV2[i];		//850/8100 V2
    	}else{
            	for(i=0;(i<34)&&(str_productType_rkbDigitalV2[i]!='\0');i++) RotelOpaqueData[m++] = (unsigned char)str_productType_rkbDigitalV2[i];	//D850/D8100 V2
    	}
#else	//RKB_DIGITAL_ANALOG
		//
		//seperate model name V2
		//20140725
		if (RKB_ModelID == RKB_850) {
			    for(i=0;(i<30)&&(str_productType_rkb850V2[i]!='\0');i++) RotelOpaqueData[m++] = (unsigned char)str_productType_rkb850V2[i];	//
		}else if (RKB_ModelID == RKB_8100) {
			    for(i=0;(i<30)&&(str_productType_rkb8100V2[i]!='\0');i++) RotelOpaqueData[m++] = (unsigned char)str_productType_rkb8100V2[i];	//
		}else if (RKB_ModelID == RKB_D850) {
			    for(i=0;(i<30)&&(str_productType_rkbd850V2[i]!='\0');i++) RotelOpaqueData[m++] = (unsigned char)str_productType_rkbd850V2[i];	//
        }else{
                for(i=0;(i<30)&&(str_productType_rkbd8100v2[i]!='\0');i++) RotelOpaqueData[m++] = (unsigned char)str_productType_rkbd8100v2[i];	//
        }		
#endif	//RKB_DIGITAL_ANALOG
	}else if ( (RKB_mode_PP_MP == 1)&&(RKB_mode_PP_nMP2 == 1) )	{
		//
		//? PP/MP1 unit
		//20140725
		if (RKB_ModelID == RKB_850) {
			    for(i=0;(i<30)&&(str_productType_rkb850[i]!='\0');i++) RotelOpaqueData[m++] = (unsigned char)str_productType_rkb850[i];	//
		}else if (RKB_ModelID == RKB_8100) {
			    for(i=0;(i<30)&&(str_productType_rkb8100[i]!='\0');i++) RotelOpaqueData[m++] = (unsigned char)str_productType_rkb8100[i];	//
		}else if (RKB_ModelID == RKB_D850) {
			    for(i=0;(i<30)&&(str_productType_rkbd850[i]!='\0');i++) RotelOpaqueData[m++] = (unsigned char)str_productType_rkbd850[i];	//
        }else{
                for(i=0;(i<30)&&(str_productType_rkbd8100[i]!='\0');i++) RotelOpaqueData[m++] = (unsigned char)str_productType_rkbd8100[i];	//
        }
    }
	//Reciva OpaqueText Tx
	RotelOpaqueData[m] = 0x00;		//null(0x00)

    //------------------------------------------------------
    //------------------------------------------------------
	//(mode,AMPtype,&m)
	//mode=0 : skip
	//mode=2 : 0A: or 0B: or 0C: or 0D:
	//
    //AMPtype=0: "0A:" + RotelOpaqueData[m]
    //AMPtype=1: "0B:" + RotelOpaqueData[m]
    //AMPtype=2: "0C:" + RotelOpaqueData[m]
    //AMPtype=3: "0D:" + RotelOpaqueData[m]       
    //------------------------------------------------------
    //------------------------------------------------------
	//addID_RoteOpaqueData(1,0,&m);
    //------------------------------------------------------
    //------------------------------------------------------
	//
	//RS232 Tx start
	//
//#ifdef RS232_SKIP	//20110114 test
	//20131212
	if (RS232_TxSkip == 0) {
		//rs232_printf("\n");						//20131025
		for(i=0;i<m;i++) {								
			PutChar(RotelOpaqueData[i]);
		}
	}				
#ifdef DISPLAY_AUTOMANUAL_ALL_COMMAND	//20140402
	//20131212 : mode_DisplayUpdate = (manual = 1) = RS232_TxSkip;
	RS232_TxSkip = mode_DisplayUpdate;		//20131212
#else	
	RS232_TxSkip = 0;
#endif	
//#endif
	//
	//Opaque Tx start			
	//
	Stingray_Tx_RotelOpaqueData(m);                 //RotelOpaqueData => UART4
}

/*	RKB
    product_version=09,v2.4.0 Vx
************************************************/
const char str_productVersion[] =	"product_version=09,";
//--------------------------------------------------------------------
void 	Func_ProductVersion(void)
{
	unsigned short i,m;   //20120309


    m=0;
	for(i=0;(i<20)&&(str_productVersion[i]!='\0');i++) RotelOpaqueData[m++] = (unsigned char)str_productVersion[i];	//

	//20140619
	//? Model
	if ( (RKB_mode_PP_MP == 1)&&(RKB_mode_PP_nMP2 == 0) ) 
	{
		//? MP2 unit : V2
		//20140819 : display Digital / Analog V2
		for(i=0;i<9;i++) RotelOpaqueData[m++] = (unsigned char)str_RKB_MP2_SWver[i];	//V2.x.x V2
	}else if ( (RKB_mode_PP_MP == 1)&&(RKB_mode_PP_nMP2 == 1) )	{
		//? PP/MP1 unit : V1
		//20140819 : display Digital / Analog V1
		for(i=0;i<9;i++) RotelOpaqueData[m++] = (unsigned char)str_RKB_MP1_SWver[i];	//V1.x.x V1
	}
	//Reciva OpaqueText Tx
	RotelOpaqueData[m] = 0x00;		//null(0x00)

    //------------------------------------------------------
    //------------------------------------------------------
	//(mode,AMPtype,&m)
	//mode=0 : skip
	//mode=2 : 0A: or 0B: or 0C: or 0D:
	//
    //AMPtype=0: "0A:" + RotelOpaqueData[m]
    //AMPtype=1: "0B:" + RotelOpaqueData[m]
    //AMPtype=2: "0C:" + RotelOpaqueData[m]
    //AMPtype=3: "0D:" + RotelOpaqueData[m]       
    //------------------------------------------------------
    //------------------------------------------------------
	//addID_RoteOpaqueData(1,0,&m);
    //------------------------------------------------------
    //------------------------------------------------------
	//
	//RS232 Tx start
	//
//#ifdef RS232_SKIP	//20110114 test
	//20131212
	if (RS232_TxSkip == 0) {
		for(i=0;i<m;i++) {								
			PutChar(RotelOpaqueData[i]);
		}
	}				
#ifdef DISPLAY_AUTOMANUAL_ALL_COMMAND	//20140402
	//20131212 : mode_DisplayUpdate = (manual = 1) = RS232_TxSkip;
	RS232_TxSkip = mode_DisplayUpdate;		//20131212
#else	
	RS232_TxSkip = 0;
#endif	
//#endif
	//
	//Opaque Tx start			
	//
	Stingray_Tx_RotelOpaqueData(m);                 //RotelOpaqueData => UART4
}
/*
	//20131212 : add to RKB 

    display_update=auto!
    display_update=manual!	(1) => All RS232 data skip!!!!
************************************************/
void 	Func_SetDisplayUpdate(unsigned char mode)
{
	mode_DisplayUpdate = mode;
	//20131212 : manual = 1 = RS232_TxSkip;
	// this command always : = 0 
	RS232_TxSkip = 0;			
	nvr_xPutData(ADDR_DISPLAYUPDATE,mode_DisplayUpdate);

	Func_DisplayUpdateStatus();   
}
//---------------------------------------------------------------------
//20130805 : move to out-position of function file
const char str_displayUpdateAuto[] = 	"display_update=auto!";
const char str_displayUpdateManual[] = 	"display_update=manual!";
//---------------------------------------------------------------------
/*
	//20131212 : add to RKB 

    display_update=auto!
    display_update=manual!
    mode_DisplayUpdate == DISPLAY_AUTO(0)
************************************************/
void 	Func_DisplayUpdateStatus(void)
{
	unsigned short i,m;   //20120309


    m=0;
    if (mode_DisplayUpdate == DISPLAY_AUTO) {
        //Auto
	    for(i=0;(i<30)&&(str_displayUpdateAuto[i]!='\0');i++) RotelOpaqueData[m++] = (unsigned char)str_displayUpdateAuto[i];	//20110323
    }else{
        //Manual
	    for(i=0;(i<30)&&(str_displayUpdateManual[i]!='\0');i++) RotelOpaqueData[m++] = (unsigned char)str_displayUpdateManual[i];	//20110323
    }
	//Reciva OpaqueText Tx
	RotelOpaqueData[m] = 0x00;		//null(0x00)

    //------------------------------------------------------
    //------------------------------------------------------
	//(mode,AMPtype,&m)
	//mode=0 : skip
	//mode=2 : 0A: or 0B: or 0C: or 0D:
	//
    //AMPtype=0: "0A:" + RotelOpaqueData[m]
    //AMPtype=1: "0B:" + RotelOpaqueData[m]
    //AMPtype=2: "0C:" + RotelOpaqueData[m]
    //AMPtype=3: "0D:" + RotelOpaqueData[m]       
    //------------------------------------------------------
    //------------------------------------------------------
	//addID_RoteOpaqueData(1,0,&m);
    //------------------------------------------------------
    //------------------------------------------------------
	//
	//RS232 Tx start
	//
//#ifdef RS232_SKIP
	//20131212
	//if (RS232_TxSkip == 0) {
		rs232_printf("\n");						//20131025
		for(i=0;i<m;i++) {								
			PutChar(RotelOpaqueData[i]);
		}
	//}				
#ifdef DISPLAY_AUTOMANUAL_ALL_COMMAND	//20140402
	//20131212 : mode_DisplayUpdate = (manual = 1) = RS232_TxSkip;
	RS232_TxSkip = mode_DisplayUpdate;		//20131212
#else	
	RS232_TxSkip = 0;
#endif	
//#endif
    //------------------------------------------------------
    //------------------------------------------------------
	//
	//Opaque Tx start			
	//
	Stingray_Tx_RotelOpaqueData(m);                 //RotelOpaqueData => UART4
}


/*	RKB
    amp_trim=30! <= current_key[typ] : [0..96]
    amp_trim=min!
    amp_trim=max!
************************************************/
//RKB
void 	Func_GetCurrentTrim(unsigned char ampType)
{
	if (ampType > 3) {
		//All A/B/C/D/
		Func_AmpTrimCurrentStatus(0);
		Func_AmpTrimCurrentStatus(1);
		Func_AmpTrimCurrentStatus(2);
		Func_AmpTrimCurrentStatus(3);
	}else{
		Func_AmpTrimCurrentStatus(ampType);
		//
#ifdef DISPLAY_AUTOMANUAL_ALL_COMMAND	//20140402
		//20131212 : mode_DisplayUpdate = (manual = 1) = RS232_TxSkip;
		RS232_TxSkip = mode_DisplayUpdate;		//20131212
#else	
		RS232_TxSkip = 0;
#endif	
	}
}
//--------------------------------------------------------------------
//	when change Freq, call
//	typ = 0/1/2/3	; A/B/C/D - CH
//--------------------------------------------------------------------
const char str_ampTrimCurr[] =	"amp_trim=  !";
const char str_ampTrimMax[] =	"amp_trim=max!";
const char str_ampTrimMin[] =	"amp_trim=min!";
//--------------------------------------------------------------------
//	front_trimKnob_enable => ON :	current_key[i]
//	front_trimKnob_enable => OFF : 	key_target[i]
//--------------------------------------------------------------------
void 	Func_AmpTrimCurrentStatus(unsigned char typ)
{
	unsigned short i,m;   //20120309
	unsigned char val_10,val_1;
	unsigned char adjVol;

		if (front_trimKnob_enable == ON) {
        	//on!
        	adjVol = current_key[typ];
    	}else{
    		//OFF! front knob disable => 
        	adjVol = key_target[typ];
    	}
    	//
    	//
        m=0;
		if (adjVol == MAX_MASTER_VOLUME) {				//?96
            //max!
		    for(i=0;i<13;i++) RotelOpaqueData[m++] = (unsigned char)str_ampTrimMax[i];	//
		}else if (adjVol == 0) {
		    //min!
		    for(i=0;i<13;i++) RotelOpaqueData[m++] = (unsigned char)str_ampTrimMin[i];	//
		}else{
		    //##!
		    for(i=0;i<12;i++) RotelOpaqueData[m++] = (unsigned char)str_ampTrimCurr[i];	//
        
			val_10 = (adjVol)/10;
			val_1 = (adjVol - val_10*10 );
		    	    
        	RotelOpaqueData[9] = (unsigned char)val_10 + 0x30;;
		    RotelOpaqueData[10] = (unsigned char)val_1 + 0x30;;
        }
		//Reciva OpaqueText Tx
		RotelOpaqueData[m] = 0x00;		//null(0x00)
        //------------------------------------------------------
        //------------------------------------------------------
		//(mode,AMPtype,&m)
		//mode=0 : skip
		//mode=2 : 0A: or 0B: or 0C: or 0D:
		//
        //AMPtype=0: "0A:" + RotelOpaqueData[m]
        //AMPtype=1: "0B:" + RotelOpaqueData[m]
        //AMPtype=2: "0C:" + RotelOpaqueData[m]
        //AMPtype=3: "0D:" + RotelOpaqueData[m]       
        //------------------------------------------------------
        //------------------------------------------------------
		addID_RoteOpaqueData(2,typ,&m);
        //------------------------------------------------------
		//
		//RS232 Tx start
		//
//#ifdef RS232_SKIP	//20110114 test
		//20131212
		if (RS232_TxSkip == 0) {
			rs232_printf("\n");						//20131025
			for(i=0;i<m;i++) {								
				PutChar(RotelOpaqueData[i]);
			}
		}				
		//20131212 : mode_DisplayUpdate = (manual = 1) = RS232_TxSkip;
		// 4 EA 의 AMP 를 표시해햐히기 때문에.... 비교문 추가함...
		if (typ == 3) {
#ifdef DISPLAY_AUTOMANUAL_ALL_COMMAND	//20140402
			//20131212 : mode_DisplayUpdate = (manual = 1) = RS232_TxSkip;
			RS232_TxSkip = mode_DisplayUpdate;		//20131212
#else	
			RS232_TxSkip = 0;
#endif	
		}		
//#endif
		//
		//Opaque Tx start			
		//
		Stingray_Tx_RotelOpaqueData(m);                 //RotelOpaqueData => UART4
}

/*
    power=on!
    power=off!
    power=standby!
    
    //RS232, Rotel Link 처리에서 power_txMode 를 정해야한다.
    //power_txMode==0, status Tx
    //power_txMode==1, command Tx

    power_on!
    power_off!
    power_toggle!

	if(Power_GetPowerStatus() == 0) 				//?power off(0)
	
	//20140826
	Signal Sensing Mode :
	
************************************************/
const char str_powerOn[] =        "power=on!";
const char str_powerStandby[] =   "power=standby!";
//20140826
const char str_SSM_PowerOn[] =    "power=SignalSensingMode!";
//
const char str_commandPowerOn[] =    "power_on!";
const char str_commandPowerOff[] =   "power_off!";
//--------------------------------------------------------------------
void 	Func_PowerStatus(void)
{
	unsigned short i,m;   //20120309

    m=0;
	if(Power_GetPowerStatus() == 0) {			
        if (power_txMode == 0) {
			//add 20140826
			if ( (RKB_mode_PP_MP == 1)&&(RKB_mode_PP_nMP2 == 0)&&(GetSignalSensing_nSW == 0) ) {
				//SSMode :
				for(i=0;(i<25)&&(str_SSM_PowerOn[i]!='\0');i++) RotelOpaqueData[m++] = (unsigned char)str_SSM_PowerOn[i];	//
			}else{
	        	//?power status : power=standby!
            	for(i=0;(i<20)&&(str_powerStandby[i]!='\0');i++) RotelOpaqueData[m++] = (unsigned char)str_powerStandby[i];	//
            }
        }else{
            power_txMode = 0;
	        //?power command : power_off!
            for(i=0;(i<20)&&(str_commandPowerOff[i]!='\0');i++) RotelOpaqueData[m++] = (unsigned char)str_commandPowerOff[i];	//
        }
	}else{
        if (power_txMode == 0) {
			//add 20140826
			if ( (RKB_mode_PP_MP == 1)&&(RKB_mode_PP_nMP2 == 0)&&(GetSignalSensing_nSW == 0) ) {
				//SSMode :
				for(i=0;(i<25)&&(str_SSM_PowerOn[i]!='\0');i++) RotelOpaqueData[m++] = (unsigned char)str_SSM_PowerOn[i];	//
			}else{
	        	//?power status : power=on!
            	for(i=0;(i<20)&&(str_powerOn[i]!='\0');i++) RotelOpaqueData[m++] = (unsigned char)str_powerOn[i];	//
            }
        }else{
            power_txMode = 0;
	        //?power command : power_on!
            for(i=0;(i<20)&&(str_commandPowerOn[i]!='\0');i++) RotelOpaqueData[m++] = (unsigned char)str_commandPowerOn[i];	//
        }
	}
	//Reciva OpaqueText Tx
	RotelOpaqueData[m] = 0x00;		//null(0x00)
    //------------------------------------------------------
    //------------------------------------------------------
	//(mode,AMPtype,&m)
	//mode=0 : skip
	//mode=2 : 0A: or 0B: or 0C: or 0D:
	//
    //AMPtype=0: "0A:" + RotelOpaqueData[m]
    //AMPtype=1: "0B:" + RotelOpaqueData[m]
    //AMPtype=2: "0C:" + RotelOpaqueData[m]
    //AMPtype=3: "0D:" + RotelOpaqueData[m]       
    //------------------------------------------------------
    //------------------------------------------------------
	//addID_RoteOpaqueData(1,0,&m);
    //------------------------------------------------------
	//
	//RS232 Tx start
	//
//#ifdef RS232_SKIP	//20110114 test
	//20131212
	if (RS232_TxSkip == 0) {
		rs232_printf("\n");						//20131025
		for(i=0;i<m;i++) {								
			PutChar(RotelOpaqueData[i]);
		}
	}				
#ifdef DISPLAY_AUTOMANUAL_ALL_COMMAND	//20140402
	//20131212 : mode_DisplayUpdate = (manual = 1) = RS232_TxSkip;
	RS232_TxSkip = mode_DisplayUpdate;		//20131212
#else	
	RS232_TxSkip = 0;
#endif	
//#endif
	//
	//Opaque Tx start			
	//
	Stingray_Tx_RotelOpaqueData(m);                 //RotelOpaqueData => UART4
}
//------------------------------------------------
//20140107
void 	Func_PowerOnCommand_Tx(void)
{
	unsigned short i,m;

        m=0;
        //?power command : power_on!
		for(i=0;(i<20)&&(str_commandPowerOn[i]!='\0');i++) RotelOpaqueData[m++] = (unsigned char)str_commandPowerOn[i];	//

		//Reciva OpaqueText Tx
		RotelOpaqueData[m] = 0x00;		//null(0x00)
        //------------------------------------------------------
		//
		//RS232 Tx start
		//
//#ifdef RS232_SKIP	//20110114 test
	//20131212
	if (RS232_TxSkip == 0) {
		rs232_printf("\n");						//20131025
		for(i=0;i<m;i++) {								
			PutChar(RotelOpaqueData[i]);
		}
	}				
#ifdef DISPLAY_AUTOMANUAL_ALL_COMMAND	//20140402
	//20131212 : mode_DisplayUpdate = (manual = 1) = RS232_TxSkip;
	RS232_TxSkip = mode_DisplayUpdate;		//20131212
#else	
	RS232_TxSkip = 0;
#endif	
//#endif
        //------------------------------------------------------
        //20140108 : power command add id skip! 
/*/    #ifdef ADD_ID_ROTELLINK
    if ( (RL_version == 1)||(RL_version == 2) ) {
            //20120829
            //RA-1570 : "11:"
            //"11:" + RotelOpaqueData(m);
            addID_RoteOpaqueData(1,&m);
    }
*/    
        //------------------------------------------------------
		//
		//Opaque Tx start			
		//
		Stingray_Tx_RotelOpaqueData(m);                 //RotelOpaqueData => Stingray
}
//------------------------------------------------
//20140107
void 	Func_PowerOffCommand_Tx(void)
{
	unsigned short i,m;

        m=0;
        //?power command : power_off!
		for(i=0;(i<20)&&(str_commandPowerOff[i]!='\0');i++) RotelOpaqueData[m++] = (unsigned char)str_commandPowerOff[i];	//

		//Reciva OpaqueText Tx
		RotelOpaqueData[m] = 0x00;		//null(0x00)
        //------------------------------------------------------
		//
		//RS232 Tx start
		//
//#ifdef RS232_SKIP	//20110114 test
	//20131212
	if (RS232_TxSkip == 0) {
		rs232_printf("\n");						//20131025
		for(i=0;i<m;i++) {								
			PutChar(RotelOpaqueData[i]);
		}
	}				
#ifdef DISPLAY_AUTOMANUAL_ALL_COMMAND	//20140402
	//20131212 : mode_DisplayUpdate = (manual = 1) = RS232_TxSkip;
	RS232_TxSkip = mode_DisplayUpdate;		//20131212
#else	
	RS232_TxSkip = 0;
#endif	
//#endif
        //------------------------------------------------------
        //20140108 : power command add id skip! 
/*/    #ifdef ADD_ID_ROTELLINK
    if ( (RL_version == 1)||(RL_version == 2) ) {
            //20120829
            //RA-1570 : "11:"
            //"11:" + RotelOpaqueData(m);
            addID_RoteOpaqueData(1,&m);
    }
*/    
        //------------------------------------------------------
		//
		//Opaque Tx start			
		//
		Stingray_Tx_RotelOpaqueData(m);                 //RotelOpaqueData => Stingray
}

/*
    mute=off!
    mute=on!
************************************************/
const char str_muteType[2][9+1] = { 
									"mute=off!",
									"mute=on!"};
//--------------------------------------------------------------------
void 	Func_MuteStatus(unsigned char type, unsigned char ampType)
{
	unsigned short i,m;   //20120309


    m=0;
    for(i=0;(i<9)&&(str_muteType[type][i]!='\0');i++) RotelOpaqueData[m++] = (unsigned char)str_muteType[type][i];	//

	//Reciva OpaqueText Tx
	RotelOpaqueData[m] = 0x00;		//null(0x00)
    //------------------------------------------------------
    //------------------------------------------------------
	//(mode,AMPtype,&m)
	//mode=0 : skip
	//mode=2 : 0A: or 0B: or 0C: or 0D:
	//
    //AMPtype=0: "0A:" + RotelOpaqueData[m]
    //AMPtype=1: "0B:" + RotelOpaqueData[m]
    //AMPtype=2: "0C:" + RotelOpaqueData[m]
    //AMPtype=3: "0D:" + RotelOpaqueData[m]       
    //------------------------------------------------------
    //------------------------------------------------------
	addID_RoteOpaqueData(2,ampType,&m);
    //------------------------------------------------------
	//
	//RS232 Tx start
	//
//#ifdef RS232_SKIP	//20110114 test
	//20131212
	if (RS232_TxSkip == 0) {
		rs232_printf("\n");						//20131025
		for(i=0;i<m;i++) {								
			PutChar(RotelOpaqueData[i]);
		}
	}				
	//20131212 : mode_DisplayUpdate = (manual = 1) = RS232_TxSkip;
	// 4 EA 의 AMP 를 표시해햐히기 때문에.... 비교문 추가함...
	if (ampType == 3) {
#ifdef DISPLAY_AUTOMANUAL_ALL_COMMAND	//20140402
		//20131212 : mode_DisplayUpdate = (manual = 1) = RS232_TxSkip;
		RS232_TxSkip = mode_DisplayUpdate;		//20131212
#else	
		RS232_TxSkip = 0;
#endif	
	}	
//#endif
	//
	//Opaque Tx start			
	//
	Stingray_Tx_RotelOpaqueData(m);                 //RotelOpaqueData => UART4
}

/*	RKB D850 and D8100
	RS232 key SK_GET_FREQ
************************************************/
const char str_FreqIs[8][11+1] = {			/*2345678901234567890*/
											"freq=48!",
											"freq=44.1!",
											"freq=32!",
											"freq=96!",
											"freq=88.2!",
											"freq=192!",
											"freq=176.4!",
											"freq=off!"				//add 20131210							
											};
//--------------------------------------------------------------------
const char str_FreqIs_analog[] =   			"freq=off!";			//20131210
//RKB main key call
//--------------------------------------------------------------------
void 	Func_GetCurrentFreq(unsigned char ampType)
{

	if(Power_GetPowerStatus() > 0) {		//?power off(0), skip
		//
		if ( (RKB_ModelID == RKB_D850) || (RKB_ModelID == RKB_D8100) ) {
			//Now unit-power on => 
			if (ampType > 3) {
				//AMP A/B/C/D -CH
				//20140325 : ? each CH power check
				if (amp_on_off[0] == 1 ) Func_CurrentFreqTx(0);	//A-CH
				if (amp_on_off[1] == 1 ) Func_CurrentFreqTx(1);	//B-CH
				if (amp_on_off[2] == 1 ) Func_CurrentFreqTx(2);	//C-CH
				if (amp_on_off[3] == 1 ) Func_CurrentFreqTx(3);	//D-CH
			}else{
				//20140325
				//? curretn-ch power on
				if (amp_on_off[ampType] == 1 ) {	//?amp_on_off[]==on		
					Func_CurrentFreqTx(ampType);	
					//
#ifdef DISPLAY_AUTOMANUAL_ALL_COMMAND	//20140402
					//20131212 : mode_DisplayUpdate = (manual = 1) = RS232_TxSkip;
					RS232_TxSkip = mode_DisplayUpdate;		//20131212
#else	
					RS232_TxSkip = 0;
#endif	
				}		
			}
		}
	}			
}
//--------------------------------------------------------------------
//	when change Freq, call
//	typ = 0/1/2/3	; A/B/C/D - CH
//--------------------------------------------------------------------
void 	Func_CurrentFreqTx(unsigned char typ)
{
	unsigned short i,m;   //20120313

	if (b_PllStatus[typ] == PLL_UNLOCK) goto goto_freq_off;
	if (b_StreamKind[typ] != ST_A_PCM) goto goto_freq_off;

	//StreamFreq
	if (b_StreamFreq[typ] >= 8) return;				//20131127

	m=0;
	for(i=0;(i<16)&&(str_FreqIs[b_StreamFreq[typ]][i]!='\0');i++) RotelOpaqueData[m++] = (unsigned char)str_FreqIs[b_StreamFreq[typ]][i];	//
	goto goto_freq_display;
//------------------------------------------------------------------------------    
goto_freq_off:
        m=0;
	    for(i=0;(i<16)&&(str_FreqIs_analog[i]!='\0');i++) RotelOpaqueData[m++] = (unsigned char)str_FreqIs_analog[i];	//
//------------------------------------------------------------------------------    
goto_freq_display:
	//Reciva OpaqueText Tx
	RotelOpaqueData[m] = 0x00;		//null(0x00)
    //------------------------------------------------------
    //------------------------------------------------------
	//(mode,AMPtype,&m)
	//mode=0 : skip
	//mode=2 : 0A: or 0B: or 0C: or 0D:
	//
    //AMPtype=0: "0A:" + RotelOpaqueData[m]
    //AMPtype=1: "0B:" + RotelOpaqueData[m]
    //AMPtype=2: "0C:" + RotelOpaqueData[m]
    //AMPtype=3: "0D:" + RotelOpaqueData[m]       
    //------------------------------------------------------
    //------------------------------------------------------
	addID_RoteOpaqueData(2,typ,&m);
    //------------------------------------------------------
	//
	//RS232 Tx start
	//
//#ifdef RS232_SKIP	//20110114 test
	//20131212
	if (RS232_TxSkip == 0) {
		rs232_printf("\n");						//20131025
		for(i=0;i<m;i++) {								
			PutChar(RotelOpaqueData[i]);
		}
	}				
	//20131212 : mode_DisplayUpdate = (manual = 1) = RS232_TxSkip;
	// 4 EA 의 AMP 를 표시해햐히기 때문에.... 비교문 추가함...
	if (typ == 3) {
#ifdef DISPLAY_AUTOMANUAL_ALL_COMMAND	//20140402
		//20131212 : mode_DisplayUpdate = (manual = 1) = RS232_TxSkip;
		RS232_TxSkip = mode_DisplayUpdate;		//20131212
#else	
		RS232_TxSkip = 0;
#endif	
	}	
//#endif
	//
	//Opaque Tx start			
	//
	Stingray_Tx_RotelOpaqueData(m);                 //RotelOpaqueData => UART4
}
/*	RKB 
	RS232 key SK_TEMPERATURE
	
	WS1/2 : 		xx,xx
	PP/MP : (850) 	xx,xx,xx
			(8100) 	xx,xx,xx,xx
			
	typ = 0 : current_key[] : xx,xx,xx(,xx)
	typ = 1 : current_ADC_Val[] : xxxx,xxxx,xxxx(,xxxx)
************************************************/
//RKB
const char str_ampTemp2Digit[] = "temperature=  ,  !";
const char str_ampTemp4Digit[] = "temperature=    ,    !";
//RKB D850
const char str_ampTemp2Digit3[] = "temperature=  ,  ,  !";
const char str_ampTemp4Digit3[] = "temperature=    ,    ,    !";
//RKB D8100
const char str_ampTemp2Digit4[] = "temperature=  ,  ,  ,  !";
const char str_ampTemp4Digit4[] = "temperature=    ,    ,    ,    !";
//--------------------------------------------------------------------
void 	Func_AmpTemperatureCurrentStatus(unsigned char typ)
{
	unsigned short i,m;   //20120309
	unsigned char val;
	unsigned short val16;
	unsigned short val_1000,val_100,val_10,val_1;

	if(mode_PowerStatus < POWER_ON3_ING) return; 				//20131216
	//test
	//return; 	//20130708 :
	//
	//20140403 : display_updatae=manual 하에서도 get_temperature! 는 진행해야한다.
	//
	//
	if (RKB_mode_PP_MP == 0) {
		//--------------------------------------------------
		//? WS1/2
		//--------------------------------------------------
		//Temp. = typ = 0
		//current_key[5]		//Temp. value saving =>AMP A/B HeatSink sensor : Right
		//current_key[4]		//Temp. value saving =>AMP C/D HeatSink sensor : Left
		//
        //ADC Value = typ = 1
		//current_ADC_Val[5] = ADC_Val[5];			//16bit ADC value saving
		//current_ADC_Val[4] = ADC_Val[4];			//16bit ADC value saving
        //
		if ( typ == 0 ) {
			//2,2 digit
			m=0;
			for(i=0;(i<23)&&(str_ampTemp2Digit[i]!='\0');i++) RotelOpaqueData[m++] = (unsigned char)str_ampTemp2Digit[i];	//
            //----------------------------------------
		    //temp. current_key[5],current_key[4] display - {2 digit]
            //----------------------------------------
            val = current_key[5];					//8bit current_key[5] : Right : AMP_A/B
		    //
			val_10 = (val)/10;
			val_1 = (val - val_10*10);
		    	    
        	RotelOpaqueData[12] = (unsigned char)val_10 + 0x30;;
		    RotelOpaqueData[13] = (unsigned char)val_1 + 0x30;;
			RotelOpaqueData[14] = (unsigned char)',';
			//
            val = current_key[4];					//8bit current_key[4] : Left : AMP_C/D
		    //
			val_10 = (val)/10;
			val_1 = (val - val_10*10);
		    	    
        	RotelOpaqueData[15] = (unsigned char)val_10 + 0x30;;
		    RotelOpaqueData[16] = (unsigned char)val_1 + 0x30;;
        }else{
			//4,4 digit
			m=0;
			for(i=0;(i<23)&&(str_ampTemp4Digit[i]!='\0');i++) RotelOpaqueData[m++] = (unsigned char)str_ampTemp4Digit[i];	//
            //----------------------------------------
		    //ADC current_ADC_Val[4],current_ADC_Val[5] display - {4 digit]
            //----------------------------------------
            val16 = current_ADC_Val[5];					//16bit current_ADC_Val[5]
		    //
			val_1000 = (val16)/1000;
			val_100 = (val16 - val_1000*1000 )/100;
			val_10 = (val16 - (val_1000*1000+val_100*100))/10;
			val_1 = (val16 - (val_1000*1000+val_100*100+val_10*10));
		    //
        	RotelOpaqueData[12] = (unsigned char)val_1000 + 0x30;;
		    RotelOpaqueData[13] = (unsigned char)val_100 + 0x30;;
        	RotelOpaqueData[14] = (unsigned char)val_10 + 0x30;;
		    RotelOpaqueData[15] = (unsigned char)val_1 + 0x30;;
			RotelOpaqueData[16] = (unsigned char)',';
			//
            val16 = current_ADC_Val[4];					//16bit current_ADC_Val[4]
		    //
			val_1000 = (val16)/1000;
			val_100 = (val16 - val_1000*1000 )/100;
			val_10 = (val16 - (val_1000*1000+val_100*100))/10;
			val_1 = (val16 - (val_1000*1000+val_100*100+val_10*10));
		    //
        	RotelOpaqueData[17] = (unsigned char)val_1000 + 0x30;;
		    RotelOpaqueData[18] = (unsigned char)val_100 + 0x30;;
        	RotelOpaqueData[19] = (unsigned char)val_10 + 0x30;;
		    RotelOpaqueData[20] = (unsigned char)val_1 + 0x30;;
			//
		}
	}else{
		//
		//	PP/MP
		//
#ifdef SIGNAL_SENSING_ENABLE
	//20140725 : add compile option
#ifdef RKB_DIGITAL_ANALOG
		//--------------------------------------------------------
		//20140619 : always 8100/D8100
		//--------------------------------------------------------
		//Temp. = typ = 0
		//current_key[5]		//Temp. value saving =>AMP A/B HeatSink sensor : Right
		//current_key[4]		//Temp. value saving =>AMP C/D HeatSink sensor : Left
		//current_key[6]		//Temp. value saving =>SMPS1 HeatSink sensor
		//current_key[7]		//Temp. value saving =>SMPS2 HeatSink sensor
		//
        //ADC Value = typ = 1
		//current_ADC_Val[5] = ADC_Val[5];			//16bit ADC value saving
		//current_ADC_Val[4] = ADC_Val[4];			//16bit ADC value saving
		//current_ADC_Val[6] = ADC_Val[6];			//16bit ADC value saving
		//current_ADC_Val[7] = ADC_Val[7];			//16bit ADC value saving
        //
		if ( typ == 0 ) {
			//2,2,2,2 digit
			m=0;
			for(i=0;(i<25)&&(str_ampTemp2Digit4[i]!='\0');i++) RotelOpaqueData[m++] = (unsigned char)str_ampTemp2Digit4[i];	//
            //----------------------------------------------------
		    //temp. current_key[4],current_key[5],current_key[6],current_key[7] display - [2 digit]
            //----------------------------------------------------
            val = current_key[5];					//8bit current_key[5]
		    //
			val_10 = (val)/10;
			val_1 = (val - val_10*10);
		    	    
        	RotelOpaqueData[12] = (unsigned char)val_10 + 0x30;;
		    RotelOpaqueData[13] = (unsigned char)val_1 + 0x30;;
			RotelOpaqueData[14] = (unsigned char)',';
            //----------------------------------------------------
            val = current_key[4];					//8bit current_key[4]
		    //
			val_10 = (val)/10;
			val_1 = (val - val_10*10);
		    	    
        	RotelOpaqueData[15] = (unsigned char)val_10 + 0x30;;
		    RotelOpaqueData[16] = (unsigned char)val_1 + 0x30;;
			RotelOpaqueData[17] = (unsigned char)',';
            //----------------------------------------------------
            val = current_key[6];					//8bit current_key[6]
		    //
			val_10 = (val)/10;
			val_1 = (val - val_10*10);
		    	    
        	RotelOpaqueData[18] = (unsigned char)val_10 + 0x30;;
		    RotelOpaqueData[19] = (unsigned char)val_1 + 0x30;;
			RotelOpaqueData[20] = (unsigned char)',';
			//----------------------------------------------------
            val = current_key[7];					//8bit current_key[7]
		    //
			val_10 = (val)/10;
			val_1 = (val - val_10*10);
		    	    
        	RotelOpaqueData[21] = (unsigned char)val_10 + 0x30;;
		    RotelOpaqueData[22] = (unsigned char)val_1 + 0x30;;
        }else{
			//4,4,4,4 digit
			m=0;
			for(i=0;(i<33)&&(str_ampTemp4Digit4[i]!='\0');i++) RotelOpaqueData[m++] = (unsigned char)str_ampTemp4Digit4[i];	//
			//----------------------------------------------------
		    //temp. current_key[4],current_key[5],current_key[6],current_key[7] display - [4 digit]
			//----------------------------------------------------
            val16 = current_ADC_Val[5];					//16bit current_ADC_Val[5]
		    //
			val_1000 = (val16)/1000;
			val_100 = (val16 - val_1000*1000 )/100;
			val_10 = (val16 - (val_1000*1000+val_100*100))/10;
			val_1 = (val16 - (val_1000*1000+val_100*100+val_10*10));
		    //
        	RotelOpaqueData[12] = (unsigned char)val_1000 + 0x30;;
		    RotelOpaqueData[13] = (unsigned char)val_100 + 0x30;;
        	RotelOpaqueData[14] = (unsigned char)val_10 + 0x30;;
		    RotelOpaqueData[15] = (unsigned char)val_1 + 0x30;;
			RotelOpaqueData[16] = (unsigned char)',';
			//----------------------------------------------------
            val16 = current_ADC_Val[4];					//16bit current_ADC_Val[4]
		    //
			val_1000 = (val16)/1000;
			val_100 = (val16 - val_1000*1000 )/100;
			val_10 = (val16 - (val_1000*1000+val_100*100))/10;
			val_1 = (val16 - (val_1000*1000+val_100*100+val_10*10));
		    //
        	RotelOpaqueData[17] = (unsigned char)val_1000 + 0x30;;
		    RotelOpaqueData[18] = (unsigned char)val_100 + 0x30;;
        	RotelOpaqueData[19] = (unsigned char)val_10 + 0x30;;
		    RotelOpaqueData[20] = (unsigned char)val_1 + 0x30;;
			RotelOpaqueData[21] = (unsigned char)',';
			//----------------------------------------------------
            val16 = current_ADC_Val[6];					//16bit current_ADC_Val[6]
		    //
			val_1000 = (val16)/1000;
			val_100 = (val16 - val_1000*1000 )/100;
			val_10 = (val16 - (val_1000*1000+val_100*100))/10;
			val_1 = (val16 - (val_1000*1000+val_100*100+val_10*10));
		    //
        	RotelOpaqueData[22] = (unsigned char)val_1000 + 0x30;;
		    RotelOpaqueData[23] = (unsigned char)val_100 + 0x30;;
        	RotelOpaqueData[24] = (unsigned char)val_10 + 0x30;;
		    RotelOpaqueData[25] = (unsigned char)val_1 + 0x30;;
			RotelOpaqueData[26] = (unsigned char)',';
			//----------------------------------------------------
            val16 = current_ADC_Val[7];					//16bit current_ADC_Val[7]
		    //
			val_1000 = (val16)/1000;
			val_100 = (val16 - val_1000*1000 )/100;
			val_10 = (val16 - (val_1000*1000+val_100*100))/10;
			val_1 = (val16 - (val_1000*1000+val_100*100+val_10*10));
		    //
        	RotelOpaqueData[27] = (unsigned char)val_1000 + 0x30;;
		    RotelOpaqueData[28] = (unsigned char)val_100 + 0x30;;
        	RotelOpaqueData[29] = (unsigned char)val_10 + 0x30;;
		    RotelOpaqueData[30] = (unsigned char)val_1 + 0x30;;
			//
		}
#else //RKB_DIGITAL_ANALOG
		//--------------------------------------------------
		//PP : add SMPS_sensor_1/2		
		//--------------------------------------------------
		if ( (RKB_ModelID == RKB_8100) || (RKB_ModelID == RKB_D8100) ) {		//?8100 or D8100 [ 00=850, 01=8100, 10=D850, 11=D8100 ]
			//--------------------------------------------------------
			//!8100 or D8100 [ 00=850, 01=8100, 10=D850, 11=D8100 ]
			//--------------------------------------------------------
			//Temp. = typ = 0
			//current_key[5]		//Temp. value saving =>AMP A/B HeatSink sensor : Right
			//current_key[4]		//Temp. value saving =>AMP C/D HeatSink sensor : Left
			//current_key[6]		//Temp. value saving =>SMPS1 HeatSink sensor
			//current_key[7]		//Temp. value saving =>SMPS2 HeatSink sensor
			//
            //ADC Value = typ = 1
			//current_ADC_Val[5] = ADC_Val[5];			//16bit ADC value saving
			//current_ADC_Val[4] = ADC_Val[4];			//16bit ADC value saving
			//current_ADC_Val[6] = ADC_Val[6];			//16bit ADC value saving
			//current_ADC_Val[7] = ADC_Val[7];			//16bit ADC value saving
            //
			if ( typ == 0 ) {
				//2,2,2,2 digit
				m=0;
				for(i=0;(i<25)&&(str_ampTemp2Digit4[i]!='\0');i++) RotelOpaqueData[m++] = (unsigned char)str_ampTemp2Digit4[i];	//
                //----------------------------------------------------
			    //temp. current_key[4],current_key[5],current_key[6],current_key[7] display - [2 digit]
                //----------------------------------------------------
                val = current_key[5];					//8bit current_key[5]
			    //
				val_10 = (val)/10;
				val_1 = (val - val_10*10);
			    	    
            	RotelOpaqueData[12] = (unsigned char)val_10 + 0x30;;
			    RotelOpaqueData[13] = (unsigned char)val_1 + 0x30;;
				RotelOpaqueData[14] = (unsigned char)',';
                //----------------------------------------------------
                val = current_key[4];					//8bit current_key[4]
			    //
				val_10 = (val)/10;
				val_1 = (val - val_10*10);
			    	    
            	RotelOpaqueData[15] = (unsigned char)val_10 + 0x30;;
			    RotelOpaqueData[16] = (unsigned char)val_1 + 0x30;;
				RotelOpaqueData[17] = (unsigned char)',';
                //----------------------------------------------------
                val = current_key[6];					//8bit current_key[6]
			    //
				val_10 = (val)/10;
				val_1 = (val - val_10*10);
			    	    
            	RotelOpaqueData[18] = (unsigned char)val_10 + 0x30;;
			    RotelOpaqueData[19] = (unsigned char)val_1 + 0x30;;
				RotelOpaqueData[20] = (unsigned char)',';
				//----------------------------------------------------
                val = current_key[7];					//8bit current_key[7]
			    //
				val_10 = (val)/10;
				val_1 = (val - val_10*10);
			    	    
            	RotelOpaqueData[21] = (unsigned char)val_10 + 0x30;;
			    RotelOpaqueData[22] = (unsigned char)val_1 + 0x30;;
            }else{
				//4,4,4,4 digit
				m=0;
				for(i=0;(i<33)&&(str_ampTemp4Digit4[i]!='\0');i++) RotelOpaqueData[m++] = (unsigned char)str_ampTemp4Digit4[i];	//
				//----------------------------------------------------
			    //temp. current_key[4],current_key[5],current_key[6],current_key[7] display - [4 digit]
				//----------------------------------------------------
                val16 = current_ADC_Val[5];					//16bit current_ADC_Val[5]
			    //
				val_1000 = (val16)/1000;
				val_100 = (val16 - val_1000*1000 )/100;
				val_10 = (val16 - (val_1000*1000+val_100*100))/10;
				val_1 = (val16 - (val_1000*1000+val_100*100+val_10*10));
			    //
            	RotelOpaqueData[12] = (unsigned char)val_1000 + 0x30;;
			    RotelOpaqueData[13] = (unsigned char)val_100 + 0x30;;
            	RotelOpaqueData[14] = (unsigned char)val_10 + 0x30;;
			    RotelOpaqueData[15] = (unsigned char)val_1 + 0x30;;
				RotelOpaqueData[16] = (unsigned char)',';
				//----------------------------------------------------
                val16 = current_ADC_Val[4];					//16bit current_ADC_Val[4]
			    //
				val_1000 = (val16)/1000;
				val_100 = (val16 - val_1000*1000 )/100;
				val_10 = (val16 - (val_1000*1000+val_100*100))/10;
				val_1 = (val16 - (val_1000*1000+val_100*100+val_10*10));
			    //
            	RotelOpaqueData[17] = (unsigned char)val_1000 + 0x30;;
			    RotelOpaqueData[18] = (unsigned char)val_100 + 0x30;;
            	RotelOpaqueData[19] = (unsigned char)val_10 + 0x30;;
			    RotelOpaqueData[20] = (unsigned char)val_1 + 0x30;;
				RotelOpaqueData[21] = (unsigned char)',';
				//----------------------------------------------------
                val16 = current_ADC_Val[6];					//16bit current_ADC_Val[6]
			    //
				val_1000 = (val16)/1000;
				val_100 = (val16 - val_1000*1000 )/100;
				val_10 = (val16 - (val_1000*1000+val_100*100))/10;
				val_1 = (val16 - (val_1000*1000+val_100*100+val_10*10));
			    //
            	RotelOpaqueData[22] = (unsigned char)val_1000 + 0x30;;
			    RotelOpaqueData[23] = (unsigned char)val_100 + 0x30;;
            	RotelOpaqueData[24] = (unsigned char)val_10 + 0x30;;
			    RotelOpaqueData[25] = (unsigned char)val_1 + 0x30;;
				RotelOpaqueData[26] = (unsigned char)',';
				//----------------------------------------------------
                val16 = current_ADC_Val[7];					//16bit current_ADC_Val[7]
			    //
				val_1000 = (val16)/1000;
				val_100 = (val16 - val_1000*1000 )/100;
				val_10 = (val16 - (val_1000*1000+val_100*100))/10;
				val_1 = (val16 - (val_1000*1000+val_100*100+val_10*10));
			    //
            	RotelOpaqueData[27] = (unsigned char)val_1000 + 0x30;;
			    RotelOpaqueData[28] = (unsigned char)val_100 + 0x30;;
            	RotelOpaqueData[29] = (unsigned char)val_10 + 0x30;;
			    RotelOpaqueData[30] = (unsigned char)val_1 + 0x30;;
				//
			}
		}else{	
			//--------------------------------------------------------
			//!850 or D850 [ 00=850, 01=8100, 10=D850, 11=D8100 ]
			//--------------------------------------------------------
			//Temp. = typ = 0
			//current_key[5]		//Temp. value saving =>AMP A/B HeatSink sensor : Right
			//current_key[4]		//Temp. value saving =>AMP C/D HeatSink sensor : Left
			//current_key[6]		//Temp. value saving =>SMPS1 HeatSink sensor
			//
            //ADC Value = typ = 1
			//current_ADC_Val[5] = ADC_Val[5];			//16bit ADC value saving
			//current_ADC_Val[4] = ADC_Val[4];			//16bit ADC value saving
			//current_ADC_Val[6] = ADC_Val[6];			//16bit ADC value saving
            //
			if ( typ == 0 ) {
				//2,2,2 digit
				m=0;
				for(i=0;(i<23)&&(str_ampTemp2Digit3[i]!='\0');i++) RotelOpaqueData[m++] = (unsigned char)str_ampTemp2Digit3[i];	//
				//----------------------------------------------------
			    //temp. current_key[4],current_key[5],current_key[6] display - [2 digit]
				//----------------------------------------------------
                val = current_key[5];					//8bit current_key[5]
			    //
				val_10 = (val)/10;
				val_1 = (val - val_10*10);
			    	    
            	RotelOpaqueData[12] = (unsigned char)val_10 + 0x30;;
			    RotelOpaqueData[13] = (unsigned char)val_1 + 0x30;;
				RotelOpaqueData[14] = (unsigned char)',';
				//----------------------------------------------------
                val = current_key[4];					//8bit current_key[4]
			    //
				val_10 = (val)/10;
				val_1 = (val - val_10*10);
			    	    
            	RotelOpaqueData[15] = (unsigned char)val_10 + 0x30;;
			    RotelOpaqueData[16] = (unsigned char)val_1 + 0x30;;
				RotelOpaqueData[17] = (unsigned char)',';
				//----------------------------------------------------
                val = current_key[6];					//8bit current_key[6]
			    //
				val_10 = (val)/10;
				val_1 = (val - val_10*10);
			    	    
            	RotelOpaqueData[18] = (unsigned char)val_10 + 0x30;;
			    RotelOpaqueData[19] = (unsigned char)val_1 + 0x30;;
            }else{
				//4,4,4 digit
				m=0;
				for(i=0;(i<28)&&(str_ampTemp4Digit3[i]!='\0');i++) RotelOpaqueData[m++] = (unsigned char)str_ampTemp4Digit3[i];	//
				//----------------------------------------------------
			    //temp. current_key[4],current_key[5],current_key[6] display - [2 digit]
				//----------------------------------------------------
                val16 = current_ADC_Val[5];					//16bit current_ADC_Val[5]
			    //
				val_1000 = (val16)/1000;
				val_100 = (val16 - val_1000*1000 )/100;
				val_10 = (val16 - (val_1000*1000+val_100*100))/10;
				val_1 = (val16 - (val_1000*1000+val_100*100+val_10*10));
			    //
            	RotelOpaqueData[12] = (unsigned char)val_1000 + 0x30;;
			    RotelOpaqueData[13] = (unsigned char)val_100 + 0x30;;
            	RotelOpaqueData[14] = (unsigned char)val_10 + 0x30;;
			    RotelOpaqueData[15] = (unsigned char)val_1 + 0x30;;
				RotelOpaqueData[16] = (unsigned char)',';
				//----------------------------------------------------
                val16 = current_ADC_Val[4];					//16bit current_ADC_Val[4]
			    //
				val_1000 = (val16)/1000;
				val_100 = (val16 - val_1000*1000 )/100;
				val_10 = (val16 - (val_1000*1000+val_100*100))/10;
				val_1 = (val16 - (val_1000*1000+val_100*100+val_10*10));
			    //
            	RotelOpaqueData[17] = (unsigned char)val_1000 + 0x30;;
			    RotelOpaqueData[18] = (unsigned char)val_100 + 0x30;;
            	RotelOpaqueData[19] = (unsigned char)val_10 + 0x30;;
			    RotelOpaqueData[20] = (unsigned char)val_1 + 0x30;;
				RotelOpaqueData[21] = (unsigned char)',';
				//----------------------------------------------------
                val16 = current_ADC_Val[6];					//16bit current_ADC_Val[6]
			    //
				val_1000 = (val16)/1000;
				val_100 = (val16 - val_1000*1000 )/100;
				val_10 = (val16 - (val_1000*1000+val_100*100))/10;
				val_1 = (val16 - (val_1000*1000+val_100*100+val_10*10));
			    //
            	RotelOpaqueData[22] = (unsigned char)val_1000 + 0x30;;
			    RotelOpaqueData[23] = (unsigned char)val_100 + 0x30;;
            	RotelOpaqueData[24] = (unsigned char)val_10 + 0x30;;
			    RotelOpaqueData[25] = (unsigned char)val_1 + 0x30;;
				//
			}
		}
#endif //RKB_DIGITAL_ANALOG
//--------------------------------------------------------------------		
#else //SIGNAL_SENSING_ENABLE	
//--------------------------------------------------------------------		
		//--------------------------------------------------
		//PP : add SMPS_sensor_1/2		
		//--------------------------------------------------
		if ( (RKB_ModelID == RKB_8100) || (RKB_ModelID == RKB_D8100) ) {		//?8100 or D8100 [ 00=850, 01=8100, 10=D850, 11=D8100 ]
			//--------------------------------------------------------
			//!8100 or D8100 [ 00=850, 01=8100, 10=D850, 11=D8100 ]
			//--------------------------------------------------------
			//Temp. = typ = 0
			//current_key[5]		//Temp. value saving =>AMP A/B HeatSink sensor : Right
			//current_key[4]		//Temp. value saving =>AMP C/D HeatSink sensor : Left
			//current_key[6]		//Temp. value saving =>SMPS1 HeatSink sensor
			//current_key[7]		//Temp. value saving =>SMPS2 HeatSink sensor
			//
            //ADC Value = typ = 1
			//current_ADC_Val[5] = ADC_Val[5];			//16bit ADC value saving
			//current_ADC_Val[4] = ADC_Val[4];			//16bit ADC value saving
			//current_ADC_Val[6] = ADC_Val[6];			//16bit ADC value saving
			//current_ADC_Val[7] = ADC_Val[7];			//16bit ADC value saving
            //
			if ( typ == 0 ) {
				//2,2,2,2 digit
				m=0;
				for(i=0;(i<25)&&(str_ampTemp2Digit4[i]!='\0');i++) RotelOpaqueData[m++] = (unsigned char)str_ampTemp2Digit4[i];	//
                //----------------------------------------------------
			    //temp. current_key[4],current_key[5],current_key[6],current_key[7] display - [2 digit]
                //----------------------------------------------------
                val = current_key[5];					//8bit current_key[5]
			    //
				val_10 = (val)/10;
				val_1 = (val - val_10*10);
			    	    
            	RotelOpaqueData[12] = (unsigned char)val_10 + 0x30;;
			    RotelOpaqueData[13] = (unsigned char)val_1 + 0x30;;
				RotelOpaqueData[14] = (unsigned char)',';
                //----------------------------------------------------
                val = current_key[4];					//8bit current_key[4]
			    //
				val_10 = (val)/10;
				val_1 = (val - val_10*10);
			    	    
            	RotelOpaqueData[15] = (unsigned char)val_10 + 0x30;;
			    RotelOpaqueData[16] = (unsigned char)val_1 + 0x30;;
				RotelOpaqueData[17] = (unsigned char)',';
                //----------------------------------------------------
                val = current_key[6];					//8bit current_key[6]
			    //
				val_10 = (val)/10;
				val_1 = (val - val_10*10);
			    	    
            	RotelOpaqueData[18] = (unsigned char)val_10 + 0x30;;
			    RotelOpaqueData[19] = (unsigned char)val_1 + 0x30;;
				RotelOpaqueData[20] = (unsigned char)',';
				//----------------------------------------------------
                val = current_key[7];					//8bit current_key[7]
			    //
				val_10 = (val)/10;
				val_1 = (val - val_10*10);
			    	    
            	RotelOpaqueData[21] = (unsigned char)val_10 + 0x30;;
			    RotelOpaqueData[22] = (unsigned char)val_1 + 0x30;;
            }else{
				//4,4,4,4 digit
				m=0;
				for(i=0;(i<33)&&(str_ampTemp4Digit4[i]!='\0');i++) RotelOpaqueData[m++] = (unsigned char)str_ampTemp4Digit4[i];	//
				//----------------------------------------------------
			    //temp. current_key[4],current_key[5],current_key[6],current_key[7] display - [4 digit]
				//----------------------------------------------------
                val16 = current_ADC_Val[5];					//16bit current_ADC_Val[5]
			    //
				val_1000 = (val16)/1000;
				val_100 = (val16 - val_1000*1000 )/100;
				val_10 = (val16 - (val_1000*1000+val_100*100))/10;
				val_1 = (val16 - (val_1000*1000+val_100*100+val_10*10));
			    //
            	RotelOpaqueData[12] = (unsigned char)val_1000 + 0x30;;
			    RotelOpaqueData[13] = (unsigned char)val_100 + 0x30;;
            	RotelOpaqueData[14] = (unsigned char)val_10 + 0x30;;
			    RotelOpaqueData[15] = (unsigned char)val_1 + 0x30;;
				RotelOpaqueData[16] = (unsigned char)',';
				//----------------------------------------------------
                val16 = current_ADC_Val[4];					//16bit current_ADC_Val[4]
			    //
				val_1000 = (val16)/1000;
				val_100 = (val16 - val_1000*1000 )/100;
				val_10 = (val16 - (val_1000*1000+val_100*100))/10;
				val_1 = (val16 - (val_1000*1000+val_100*100+val_10*10));
			    //
            	RotelOpaqueData[17] = (unsigned char)val_1000 + 0x30;;
			    RotelOpaqueData[18] = (unsigned char)val_100 + 0x30;;
            	RotelOpaqueData[19] = (unsigned char)val_10 + 0x30;;
			    RotelOpaqueData[20] = (unsigned char)val_1 + 0x30;;
				RotelOpaqueData[21] = (unsigned char)',';
				//----------------------------------------------------
                val16 = current_ADC_Val[6];					//16bit current_ADC_Val[6]
			    //
				val_1000 = (val16)/1000;
				val_100 = (val16 - val_1000*1000 )/100;
				val_10 = (val16 - (val_1000*1000+val_100*100))/10;
				val_1 = (val16 - (val_1000*1000+val_100*100+val_10*10));
			    //
            	RotelOpaqueData[22] = (unsigned char)val_1000 + 0x30;;
			    RotelOpaqueData[23] = (unsigned char)val_100 + 0x30;;
            	RotelOpaqueData[24] = (unsigned char)val_10 + 0x30;;
			    RotelOpaqueData[25] = (unsigned char)val_1 + 0x30;;
				RotelOpaqueData[26] = (unsigned char)',';
				//----------------------------------------------------
                val16 = current_ADC_Val[7];					//16bit current_ADC_Val[7]
			    //
				val_1000 = (val16)/1000;
				val_100 = (val16 - val_1000*1000 )/100;
				val_10 = (val16 - (val_1000*1000+val_100*100))/10;
				val_1 = (val16 - (val_1000*1000+val_100*100+val_10*10));
			    //
            	RotelOpaqueData[27] = (unsigned char)val_1000 + 0x30;;
			    RotelOpaqueData[28] = (unsigned char)val_100 + 0x30;;
            	RotelOpaqueData[29] = (unsigned char)val_10 + 0x30;;
			    RotelOpaqueData[30] = (unsigned char)val_1 + 0x30;;
				//
			}
		}else{	
			//--------------------------------------------------------
			//!850 or D850 [ 00=850, 01=8100, 10=D850, 11=D8100 ]
			//--------------------------------------------------------
			//Temp. = typ = 0
			//current_key[5]		//Temp. value saving =>AMP A/B HeatSink sensor : Right
			//current_key[4]		//Temp. value saving =>AMP C/D HeatSink sensor : Left
			//current_key[6]		//Temp. value saving =>SMPS1 HeatSink sensor
			//
            //ADC Value = typ = 1
			//current_ADC_Val[5] = ADC_Val[5];			//16bit ADC value saving
			//current_ADC_Val[4] = ADC_Val[4];			//16bit ADC value saving
			//current_ADC_Val[6] = ADC_Val[6];			//16bit ADC value saving
            //
			if ( typ == 0 ) {
				//2,2,2 digit
				m=0;
				for(i=0;(i<23)&&(str_ampTemp2Digit3[i]!='\0');i++) RotelOpaqueData[m++] = (unsigned char)str_ampTemp2Digit3[i];	//
				//----------------------------------------------------
			    //temp. current_key[4],current_key[5],current_key[6] display - [2 digit]
				//----------------------------------------------------
                val = current_key[5];					//8bit current_key[5]
			    //
				val_10 = (val)/10;
				val_1 = (val - val_10*10);
			    	    
            	RotelOpaqueData[12] = (unsigned char)val_10 + 0x30;;
			    RotelOpaqueData[13] = (unsigned char)val_1 + 0x30;;
				RotelOpaqueData[14] = (unsigned char)',';
				//----------------------------------------------------
                val = current_key[4];					//8bit current_key[4]
			    //
				val_10 = (val)/10;
				val_1 = (val - val_10*10);
			    	    
            	RotelOpaqueData[15] = (unsigned char)val_10 + 0x30;;
			    RotelOpaqueData[16] = (unsigned char)val_1 + 0x30;;
				RotelOpaqueData[17] = (unsigned char)',';
				//----------------------------------------------------
                val = current_key[6];					//8bit current_key[6]
			    //
				val_10 = (val)/10;
				val_1 = (val - val_10*10);
			    	    
            	RotelOpaqueData[18] = (unsigned char)val_10 + 0x30;;
			    RotelOpaqueData[19] = (unsigned char)val_1 + 0x30;;
            }else{
				//4,4,4 digit
				m=0;
				for(i=0;(i<28)&&(str_ampTemp4Digit3[i]!='\0');i++) RotelOpaqueData[m++] = (unsigned char)str_ampTemp4Digit3[i];	//
				//----------------------------------------------------
			    //temp. current_key[4],current_key[5],current_key[6] display - [2 digit]
				//----------------------------------------------------
                val16 = current_ADC_Val[5];					//16bit current_ADC_Val[5]
			    //
				val_1000 = (val16)/1000;
				val_100 = (val16 - val_1000*1000 )/100;
				val_10 = (val16 - (val_1000*1000+val_100*100))/10;
				val_1 = (val16 - (val_1000*1000+val_100*100+val_10*10));
			    //
            	RotelOpaqueData[12] = (unsigned char)val_1000 + 0x30;;
			    RotelOpaqueData[13] = (unsigned char)val_100 + 0x30;;
            	RotelOpaqueData[14] = (unsigned char)val_10 + 0x30;;
			    RotelOpaqueData[15] = (unsigned char)val_1 + 0x30;;
				RotelOpaqueData[16] = (unsigned char)',';
				//----------------------------------------------------
                val16 = current_ADC_Val[4];					//16bit current_ADC_Val[4]
			    //
				val_1000 = (val16)/1000;
				val_100 = (val16 - val_1000*1000 )/100;
				val_10 = (val16 - (val_1000*1000+val_100*100))/10;
				val_1 = (val16 - (val_1000*1000+val_100*100+val_10*10));
			    //
            	RotelOpaqueData[17] = (unsigned char)val_1000 + 0x30;;
			    RotelOpaqueData[18] = (unsigned char)val_100 + 0x30;;
            	RotelOpaqueData[19] = (unsigned char)val_10 + 0x30;;
			    RotelOpaqueData[20] = (unsigned char)val_1 + 0x30;;
				RotelOpaqueData[21] = (unsigned char)',';
				//----------------------------------------------------
                val16 = current_ADC_Val[6];					//16bit current_ADC_Val[6]
			    //
				val_1000 = (val16)/1000;
				val_100 = (val16 - val_1000*1000 )/100;
				val_10 = (val16 - (val_1000*1000+val_100*100))/10;
				val_1 = (val16 - (val_1000*1000+val_100*100+val_10*10));
			    //
            	RotelOpaqueData[22] = (unsigned char)val_1000 + 0x30;;
			    RotelOpaqueData[23] = (unsigned char)val_100 + 0x30;;
            	RotelOpaqueData[24] = (unsigned char)val_10 + 0x30;;
			    RotelOpaqueData[25] = (unsigned char)val_1 + 0x30;;
				//
			}
		}
//--------------------------------------------------------------------		
#endif	//SIGNAL_SENSING_ENABLE
//--------------------------------------------------------------------		
	}
	//
	//Reciva OpaqueText Tx
	RotelOpaqueData[m] = 0x00;		//null(0x00)
    //------------------------------------------------------
    //------------------------------------------------------
	//(mode,AMPtype,&m)
	//mode=0 : skip
	//mode=2 : 0A: or 0B: or 0C: or 0D:
	//
    //AMPtype=0: "0A:" + RotelOpaqueData[m]
    //AMPtype=1: "0B:" + RotelOpaqueData[m]
    //AMPtype=2: "0C:" + RotelOpaqueData[m]
    //AMPtype=3: "0D:" + RotelOpaqueData[m]       
    //------------------------------------------------------
    //------------------------------------------------------
	//addID_RoteOpaqueData(1,0,&m);
    //------------------------------------------------------
	//
	//RS232 Tx start
	//
//#ifdef RS232_SKIP	//20110114 test
	//20131212
	if (RS232_TxSkip == 0) {
		rs232_printf("\n");						//20131025
		for(i=0;i<m;i++) {								
			PutChar(RotelOpaqueData[i]);
		}
	}				
#ifdef DISPLAY_AUTOMANUAL_ALL_COMMAND	//20140402
	//20131212 : mode_DisplayUpdate = (manual = 1) = RS232_TxSkip;
	RS232_TxSkip = mode_DisplayUpdate;		//20131212
#else	
	RS232_TxSkip = 0;
#endif	
//#endif
	//
	//Opaque Tx start			
	//
	Stingray_Tx_RotelOpaqueData(m);                 //RotelOpaqueData => UART4
}
/*
    	fan=normal!
    	fan=high!
20140521
	signal sensing mode :
    	fan=off!
    	fan=low!
    	fan=high!
    	
// FAN_LS_mode
// FAN_HS_mode
#define	FAN_READY					0
#define	FAN_EXEC					1
************************************************/
const char str_fanType[2][9+1] = { 
									"fan=low!",
									"fan=high!"};
//--------------------------------------------------------------------
const char str_fan_speed_Type[3][9+1] = { 
									"fan=off!",
									"fan=low!",
									"fan=high!"};
//--------------------------------------------------------------------
void 	Func_FAN_HS_Status(void)
{
	unsigned short i,m;   //20120309
	unsigned char data;
	

	if(mode_PowerStatus < POWER_ON0) return; 				//20130710

#ifdef SIGNAL_SENSING_ENABLE
	//20140523 : MP2 option :
	if (RKB_mode_PP_nMP2 == 0)
	{
		if (FAN_HS_mode == 1) {
			data= 2;
		}else{
			data = FAN_LS_mode;
		}
		//
	    m=0;
	    for(i=0;(i<12)&&(str_fan_speed_Type[data][i]!='\0');i++) RotelOpaqueData[m++] = (unsigned char)str_fan_speed_Type[data][i];	//
	}else
#endif
	{
	    m=0;
	    for(i=0;(i<12)&&(str_fanType[FAN_HS_mode][i]!='\0');i++) RotelOpaqueData[m++] = (unsigned char)str_fanType[FAN_HS_mode][i];	//
	}
	//Reciva OpaqueText Tx
	RotelOpaqueData[m] = 0x00;		//null(0x00)
    //------------------------------------------------------
    //------------------------------------------------------
	//(mode,AMPtype,&m)
	//mode=0 : skip
	//mode=2 : 0A: or 0B: or 0C: or 0D:
	//
    //AMPtype=0: "0A:" + RotelOpaqueData[m]
    //AMPtype=1: "0B:" + RotelOpaqueData[m]
    //AMPtype=2: "0C:" + RotelOpaqueData[m]
    //AMPtype=3: "0D:" + RotelOpaqueData[m]       
    //------------------------------------------------------
    //------------------------------------------------------
	//addID_RoteOpaqueData(1,0,&m);
    //------------------------------------------------------
	//
	//RS232 Tx start
	//
//#ifdef RS232_SKIP	//20110114 test
	//20131212
	if (RS232_TxSkip == 0) {
		rs232_printf("\n");						//20131025
		for(i=0;i<m;i++) {								
			PutChar(RotelOpaqueData[i]);
		}
	}				
#ifdef DISPLAY_AUTOMANUAL_ALL_COMMAND	//20140402
	//20131212 : mode_DisplayUpdate = (manual = 1) = RS232_TxSkip;
	RS232_TxSkip = mode_DisplayUpdate;		//20131212
#else	
	RS232_TxSkip = 0;
#endif	
//#endif
	//
	//Opaque Tx start			
	//
	Stingray_Tx_RotelOpaqueData(m);                 //RotelOpaqueData => UART4
}
/*	20140521
    fan_hs_on=xx!
************************************************/
const char str_fanHsOnTemp[] = "fan_hs_on=  !";
//--------------------------------------------------------------------
void 	Func_FanHsOnStatus(void)
{
	unsigned short i,m;   //20120309
	unsigned short val_10,val_1,val;

	if(mode_PowerStatus < POWER_ON0) return; 				//20130710
	//
	//load fan_on_value;
	//
	val = fan_hs_on_temp;
	//
	//2,2 digit
	m=0;
	for(i=0;(i<15)&&(str_fanHsOnTemp[i]!='\0');i++) RotelOpaqueData[m++] = (unsigned char)str_fanHsOnTemp[i];	//
	//
	val_10 = (val)/10;
	val_1 = (val - val_10*10);
		    
    RotelOpaqueData[10] = (unsigned char)val_10 + 0x30;;
	RotelOpaqueData[11] = (unsigned char)val_1 + 0x30;;

	//Reciva OpaqueText Tx
	RotelOpaqueData[m] = 0x00;		//null(0x00)
    //------------------------------------------------------
    //------------------------------------------------------
    //------------------------------------------------------
	//addID_RoteOpaqueData(1,0,&m);
    //------------------------------------------------------
	//
	//RS232 Tx start
	//
//#ifdef RS232_SKIP	//20110114 test
	//20131212
	if (RS232_TxSkip == 0) {
		rs232_printf("\n");						//20131025
		for(i=0;i<m;i++) {								
			PutChar(RotelOpaqueData[i]);
		}
	}				
#ifdef DISPLAY_AUTOMANUAL_ALL_COMMAND	//20140402
	//20131212 : mode_DisplayUpdate = (manual = 1) = RS232_TxSkip;
	RS232_TxSkip = mode_DisplayUpdate;		//20131212
#else	
	RS232_TxSkip = 0;
#endif	
//#endif
	//
	//Opaque Tx start			
	//
	Stingray_Tx_RotelOpaqueData(m);                 //RotelOpaqueData => UART4
}
/*	20140521
    fan_hs_off=xx!
************************************************/
const char str_fanHsOffTemp[] = "fan_hs_off=  !";
//--------------------------------------------------------------------
void 	Func_FanHsOffStatus(void)
{
	unsigned short i,m;   //20120309
	unsigned short val_10,val_1,val;

	if(mode_PowerStatus < POWER_ON0) return; 				//20130710
	//
	//load fan_on_value;
	//
	val = fan_hs_off_temp;
	//
	//2,2 digit
	m=0;
	for(i=0;(i<15)&&(str_fanHsOffTemp[i]!='\0');i++) RotelOpaqueData[m++] = (unsigned char)str_fanHsOffTemp[i];	//
	//
	val_10 = (val)/10;
	val_1 = (val - val_10*10);
		    
    RotelOpaqueData[11] = (unsigned char)val_10 + 0x30;;
	RotelOpaqueData[12] = (unsigned char)val_1 + 0x30;;

	//Reciva OpaqueText Tx
	RotelOpaqueData[m] = 0x00;		//null(0x00)
    //------------------------------------------------------
    //------------------------------------------------------
    //------------------------------------------------------
	//addID_RoteOpaqueData(1,0,&m);
    //------------------------------------------------------
	//
	//RS232 Tx start
	//
//#ifdef RS232_SKIP	//20110114 test
	//20131212
	if (RS232_TxSkip == 0) {
		rs232_printf("\n");						//20131025
		for(i=0;i<m;i++) {								
			PutChar(RotelOpaqueData[i]);
		}
	}				
#ifdef DISPLAY_AUTOMANUAL_ALL_COMMAND	//20140402
	//20131212 : mode_DisplayUpdate = (manual = 1) = RS232_TxSkip;
	RS232_TxSkip = mode_DisplayUpdate;		//20131212
#else	
	RS232_TxSkip = 0;
#endif	
//#endif
	//
	//Opaque Tx start			
	//
	Stingray_Tx_RotelOpaqueData(m);                 //RotelOpaqueData => UART4
}
/*	RKB : 20140521
	RS232 key 
************************************************/
void 	proc_KeyRs232_FanHsOnSet(unsigned char val)
{

	if(mode_PowerStatus < POWER_ON3_ING) return; 				//20130708
	//
	//save to EEPROM
	//
	fan_hs_on_temp = val;
	nvr_xPutData(ADDR_FAN_HS_ON_TEMP,fan_hs_on_temp);
	// compare fan_hs_off_temp
	if (fan_hs_on_temp < (fan_hs_off_temp+2) ) {
		fan_hs_off_temp = fan_hs_on_temp-2;
		nvr_xPutData(ADDR_FAN_HS_OFF_TEMP,fan_hs_off_temp);
		//20140325
		Func_FanHsOffStatus();
	}		
	// compare Shutdown temp => (fan_on_temp+1)
	if ( shutdown_temp <= fan_hs_on_temp ) {
		shutdown_temp = fan_hs_on_temp + 1;
		nvr_xPutData(ADDR_SHUTDOWN_TEMP,shutdown_temp);
		//20140325
		Func_getShutdownTemp();
	}
	//
	//2,2 digit
	//
	Func_FanHsOnStatus();
}
/*	RKB : 20140521
	RS232 key 
************************************************/
void 	proc_KeyRs232_FanHsOffSet(unsigned char val)
{

	if(mode_PowerStatus < POWER_ON3_ING) return; 				//20130708
	//	
	//save to EEPROM
	//
	fan_hs_off_temp = val;
	// compare fan_off_temp
	if (fan_hs_on_temp < (fan_hs_off_temp+2) ) {
		fan_hs_off_temp = fan_hs_on_temp-2;
	}		
	nvr_xPutData(ADDR_FAN_HS_OFF_TEMP,fan_hs_off_temp);
	//
	//2,2 digit
	//
	Func_FanHsOffStatus();
}
//====================================================================
/*	20140521
    fan_ls_on=xx!
************************************************/
const char str_fanLsOnTemp[] = "fan_ls_on=  !";
//--------------------------------------------------------------------
void 	Func_FanLsOnStatus(void)
{
	unsigned short i,m;   //20120309
	unsigned short val_10,val_1,val;

	if(mode_PowerStatus < POWER_ON0) return; 				//20130710
	//? PP & MP2 unit & signal sensing nSW
	//20140523
  if ( (RKB_mode_PP_MP == 1)&&(RKB_mode_PP_nMP2 == 0)&&(GetSignalSensing_nSW == 0) ) {							
	//
	//load fan_on_value;
	//
	val = fan_ls_on_temp;
	//
	//2,2 digit
	m=0;
	for(i=0;(i<15)&&(str_fanLsOnTemp[i]!='\0');i++) RotelOpaqueData[m++] = (unsigned char)str_fanLsOnTemp[i];	//
	//
	val_10 = (val)/10;
	val_1 = (val - val_10*10);
		    
    RotelOpaqueData[10] = (unsigned char)val_10 + 0x30;;
	RotelOpaqueData[11] = (unsigned char)val_1 + 0x30;;

	//Reciva OpaqueText Tx
	RotelOpaqueData[m] = 0x00;		//null(0x00)
    //------------------------------------------------------
    //------------------------------------------------------
    //------------------------------------------------------
	//addID_RoteOpaqueData(1,0,&m);
    //------------------------------------------------------
	//
	//RS232 Tx start
	//
//#ifdef RS232_SKIP	//20110114 test
	//20131212
	if (RS232_TxSkip == 0) {
		rs232_printf("\n");						//20131025
		for(i=0;i<m;i++) {								
			PutChar(RotelOpaqueData[i]);
		}
	}				
#ifdef DISPLAY_AUTOMANUAL_ALL_COMMAND	//20140402
	//20131212 : mode_DisplayUpdate = (manual = 1) = RS232_TxSkip;
	RS232_TxSkip = mode_DisplayUpdate;		//20131212
#else	
	RS232_TxSkip = 0;
#endif	
//#endif
	//
	//Opaque Tx start			
	//
	Stingray_Tx_RotelOpaqueData(m);                 //RotelOpaqueData => UART4
	//20140523
  }
}
/*	20140521
    fan_ls_off=xx!
************************************************/
const char str_fanLsOffTemp[] = "fan_ls_off=  !";
//--------------------------------------------------------------------
void 	Func_FanLsOffStatus(void)
{
	unsigned short i,m;   //20120309
	unsigned short val_10,val_1,val;

	if(mode_PowerStatus < POWER_ON0) return; 				//20130710
	//? PP & MP2 unit & signal sensing nSW
	//20140523
  if ( (RKB_mode_PP_MP == 1)&&(RKB_mode_PP_nMP2 == 0)&&(GetSignalSensing_nSW == 0) ) {							
	//
	//load fan_on_value;
	//
	val = fan_ls_off_temp;
	//
	//2,2 digit
	m=0;
	for(i=0;(i<15)&&(str_fanLsOffTemp[i]!='\0');i++) RotelOpaqueData[m++] = (unsigned char)str_fanLsOffTemp[i];	//
	//
	val_10 = (val)/10;
	val_1 = (val - val_10*10);
		    
    RotelOpaqueData[11] = (unsigned char)val_10 + 0x30;;
	RotelOpaqueData[12] = (unsigned char)val_1 + 0x30;;

	//Reciva OpaqueText Tx
	RotelOpaqueData[m] = 0x00;		//null(0x00)
    //------------------------------------------------------
    //------------------------------------------------------
    //------------------------------------------------------
	//addID_RoteOpaqueData(1,0,&m);
    //------------------------------------------------------
	//
	//RS232 Tx start
	//
//#ifdef RS232_SKIP	//20110114 test
	//20131212
	if (RS232_TxSkip == 0) {
		rs232_printf("\n");						//20131025
		for(i=0;i<m;i++) {								
			PutChar(RotelOpaqueData[i]);
		}
	}				
#ifdef DISPLAY_AUTOMANUAL_ALL_COMMAND	//20140402
	//20131212 : mode_DisplayUpdate = (manual = 1) = RS232_TxSkip;
	RS232_TxSkip = mode_DisplayUpdate;		//20131212
#else	
	RS232_TxSkip = 0;
#endif	
//#endif
	//
	//Opaque Tx start			
	//
	Stingray_Tx_RotelOpaqueData(m);                 //RotelOpaqueData => UART4
	//20140523
  }
}
/*	RKB : 20140521
	RS232 key 
************************************************/
void 	proc_KeyRs232_FanLsOnSet(unsigned char val)
{

	if(mode_PowerStatus < POWER_ON3_ING) return; 				//20130708
	//? PP & MP2 unit & signal sensing nSW
	//20140523
  if ( (RKB_mode_PP_MP == 1)&&(RKB_mode_PP_nMP2 == 0)&&(GetSignalSensing_nSW == 0) ) {							
	//
	//save to EEPROM
	//
	fan_ls_on_temp = val;
	nvr_xPutData(ADDR_FAN_LS_ON_TEMP,fan_ls_on_temp);
	// compare fan_ls_off_temp
	if (fan_ls_on_temp < (fan_ls_off_temp+2) ) {
		fan_ls_off_temp = fan_ls_on_temp-2;
		nvr_xPutData(ADDR_FAN_LS_OFF_TEMP,fan_ls_off_temp);
		//20140325
		Func_FanLsOffStatus();
	}		
	// compare Shutdown temp => (fan_on_temp+1)
	if ( shutdown_temp <= fan_ls_on_temp ) {
		shutdown_temp = fan_ls_on_temp + 1;
		nvr_xPutData(ADDR_SHUTDOWN_TEMP,shutdown_temp);
		//20140325
		Func_getShutdownTemp();
	}
	//
	//2,2 digit
	//
	Func_FanLsOnStatus();
	//20140523
  }
}
/*	RKB : 20140521
	RS232 key 
************************************************/
void 	proc_KeyRs232_FanLsOffSet(unsigned char val)
{

	if(mode_PowerStatus < POWER_ON3_ING) return; 				//20130708
	//? PP & MP2 unit & signal sensing nSW
	//20140523
  if ( (RKB_mode_PP_MP == 1)&&(RKB_mode_PP_nMP2 == 0)&&(GetSignalSensing_nSW == 0) ) {							
	//	
	//save to EEPROM
	//
	fan_ls_off_temp = val;
	// compare fan_ls_off_temp
	if (fan_ls_on_temp < (fan_ls_off_temp+2) ) {
		fan_ls_off_temp = fan_ls_on_temp-2;
	}		
	nvr_xPutData(ADDR_FAN_LS_OFF_TEMP,fan_ls_off_temp);
	//
	//2,2 digit
	//
	Func_FanLsOffStatus();
	//20140523
  }
}
//====================================================================

/*	20140311
    smps_fan_on=xx!
************************************************/
const char str_smpsFanOnTemp[] = "smps_fan_on=  !";
//--------------------------------------------------------------------
void 	Func_SMPSFanOnStatus(void)
{
	unsigned short i,m;   //20120309
	unsigned short val_10,val_1,val;

	if(mode_PowerStatus < POWER_ON0) return; 				//20130710
	//
	//load fan_on_value;
	//
	val = smps_fan_on_temp;
	//
	//2,2 digit
	m=0;
	for(i=0;(i<17)&&(str_smpsFanOnTemp[i]!='\0');i++) RotelOpaqueData[m++] = (unsigned char)str_smpsFanOnTemp[i];	//
	//
	val_10 = (val)/10;
	val_1 = (val - val_10*10);
		    
    RotelOpaqueData[12] = (unsigned char)val_10 + 0x30;;
	RotelOpaqueData[13] = (unsigned char)val_1 + 0x30;;

	//Reciva OpaqueText Tx
	RotelOpaqueData[m] = 0x00;		//null(0x00)
    //------------------------------------------------------
    //------------------------------------------------------
    //------------------------------------------------------
	//addID_RoteOpaqueData(1,0,&m);
    //------------------------------------------------------
	//
	//RS232 Tx start
	//
//#ifdef RS232_SKIP	//20110114 test
	//20131212
	if (RS232_TxSkip == 0) {
		rs232_printf("\n");						//20131025
		for(i=0;i<m;i++) {								
			PutChar(RotelOpaqueData[i]);
		}
	}				
#ifdef DISPLAY_AUTOMANUAL_ALL_COMMAND	//20140402
	//20131212 : mode_DisplayUpdate = (manual = 1) = RS232_TxSkip;
	RS232_TxSkip = mode_DisplayUpdate;		//20131212
#else	
	RS232_TxSkip = 0;
#endif	
//#endif
	//
	//Opaque Tx start			
	//
	Stingray_Tx_RotelOpaqueData(m);                 //RotelOpaqueData => UART4
}

/*	20140311
    smps_fan_off=xx!
************************************************/
const char str_smpsFanOffTemp[] = "smps_fan_off=  !";
//--------------------------------------------------------------------
void 	Func_SMPSFanOffStatus(void)
{
	unsigned short i,m;   //20120309
	unsigned short val_10,val_1,val;

	if(mode_PowerStatus < POWER_ON0) return; 				//20130710
	//
	//load smps_fan_off value;
	//
	val = smps_fan_off_temp;
	//
	//2,2 digit
	m=0;
	for(i=0;(i<17)&&(str_smpsFanOffTemp[i]!='\0');i++) RotelOpaqueData[m++] = (unsigned char)str_smpsFanOffTemp[i];	//
	//
	val_10 = (val)/10;
	val_1 = (val - val_10*10);
		    
    RotelOpaqueData[13] = (unsigned char)val_10 + 0x30;;
	RotelOpaqueData[14] = (unsigned char)val_1 + 0x30;;

	//Reciva OpaqueText Tx
	RotelOpaqueData[m] = 0x00;		//null(0x00)
    //------------------------------------------------------
    //------------------------------------------------------
    //------------------------------------------------------
	//addID_RoteOpaqueData(1,0,&m);
    //------------------------------------------------------
	//
	//RS232 Tx start
	//
//#ifdef RS232_SKIP	//20110114 test
	//20131212
	if (RS232_TxSkip == 0) {
		rs232_printf("\n");						//20131025
		for(i=0;i<m;i++) {								
			PutChar(RotelOpaqueData[i]);
		}
	}				
#ifdef DISPLAY_AUTOMANUAL_ALL_COMMAND	//20140402
	//20131212 : mode_DisplayUpdate = (manual = 1) = RS232_TxSkip;
	RS232_TxSkip = mode_DisplayUpdate;		//20131212
#else	
	RS232_TxSkip = 0;
#endif	
//#endif
	//
	//Opaque Tx start			
	//
	Stingray_Tx_RotelOpaqueData(m);                 //RotelOpaqueData => UART4
}
/*	RKB : 20140325
	RS232 key 
************************************************/
void 	proc_KeyRs232_SMPSFanOnSet(unsigned char val)
{

	if(mode_PowerStatus < POWER_ON3_ING) return; 				//20130708
	//
	//save to EEPROM
	//
	smps_fan_on_temp = val;
	nvr_xPutData(ADDR_SMPS_FAN_ON_TEMP,smps_fan_on_temp);
	// compare fan_off_temp
	if (smps_fan_on_temp < (smps_fan_off_temp+2) ) {
		smps_fan_off_temp = smps_fan_on_temp-2;
		nvr_xPutData(ADDR_SMPS_FAN_OFF_TEMP,smps_fan_off_temp);
		//20140325
		Func_SMPSFanOffStatus();
	}		
	// compare Shutdown temp => (smps_fan_on_temp+1)
	if ( shutdown_temp <= smps_fan_on_temp ) {
		shutdown_temp = smps_fan_on_temp + 1;
		nvr_xPutData(ADDR_SHUTDOWN_TEMP,shutdown_temp);
	}
	//
	//2,2 digit
	//
	Func_SMPSFanOnStatus();
}

/*	RKB : 20140325
	RS232 key 
************************************************/
void 	proc_KeyRs232_SMPSFanOffSet(unsigned char val)
{

	if(mode_PowerStatus < POWER_ON3_ING) return; 				//20130708
	//test

	//save to EEPROM
	//
	smps_fan_off_temp = val;
	// compare smps_fan_off_temp
	if (smps_fan_on_temp < (smps_fan_off_temp+2) ) {
		smps_fan_off_temp = smps_fan_on_temp-2;
	}		
	nvr_xPutData(ADDR_SMPS_FAN_OFF_TEMP,smps_fan_off_temp);
	//
	//2,2 digit
	//
	Func_SMPSFanOffStatus();
}
//

/*
    amp=normal!
    amp=protection!
// AMP  mode
mode_amp_protect = ON
************************************************/
const char str_ampType[2][15+1] = { 
									"amp=normal!",
									"amp=protection!"};
//--------------------------------------------------------------------
void 	Func_AMPStatus(void)
{
	unsigned short i,m;   //20120708


	//if(mode_PowerStatus < POWER_ON0) return; 				//20130710

    m=0;
    for(i=0;(i<16)&&(str_ampType[mode_amp_protect][i]!='\0');i++) RotelOpaqueData[m++] = (unsigned char)str_ampType[mode_amp_protect][i];	//

	//Reciva OpaqueText Tx
	RotelOpaqueData[m] = 0x00;		//null(0x00)
    //------------------------------------------------------
    //------------------------------------------------------
    //------------------------------------------------------
    //------------------------------------------------------
	//addID_RoteOpaqueData(1,0,&m);
    //------------------------------------------------------
	//
	//RS232 Tx start
	//
//#ifdef RS232_SKIP	//20110114 test
	//20131212
	if (RS232_TxSkip == 0) {
		rs232_printf("\n");						//20131025
		for(i=0;i<m;i++) {								
			PutChar(RotelOpaqueData[i]);
		}
	}				
#ifdef DISPLAY_AUTOMANUAL_ALL_COMMAND	//20140402
	//20131212 : mode_DisplayUpdate = (manual = 1) = RS232_TxSkip;
	RS232_TxSkip = mode_DisplayUpdate;		//20131212
#else	
	RS232_TxSkip = 0;
#endif	
//#endif
	//
	//Opaque Tx start			
	//
	Stingray_Tx_RotelOpaqueData(m);                 //RotelOpaqueData => UART4
}
/*
    mod=0 : amp
    mod=1 : overload
    mod=2 : temperature
    mod=3 : SMPS Heat Sink temperature
************************************************/
const char str_protectionType[4][28+1] = { 
											"protection=amp!",
											"protection=overload!",
											"protection=temperature!",				//20131008
											"protection=smps_temperature!"};		//20131024
//--------------------------------------------------------------------
void 	Func_ProtectionStatus(unsigned char mod)
{
	unsigned short i,m;


	//if(mode_PowerStatus < POWER_ON0) return; 				//20130710

    m=0;
    for(i=0;(i<29)&&(str_protectionType[mod][i]!='\0');i++) RotelOpaqueData[m++] = (unsigned char)str_protectionType[mod][i];	//

	//Reciva OpaqueText Tx
	RotelOpaqueData[m] = 0x00;		//null(0x00)
    //------------------------------------------------------
    //------------------------------------------------------
    //------------------------------------------------------
    //------------------------------------------------------
	//addID_RoteOpaqueData(1,0,&m);
    //------------------------------------------------------
	//
	//RS232 Tx start
	//
//#ifdef RS232_SKIP	//20110114 test
	//20131212
	if (RS232_TxSkip == 0) {
		rs232_printf("\n");						//20131025
		for(i=0;i<m;i++) {								
			PutChar(RotelOpaqueData[i]);
		}
	}				
#ifdef DISPLAY_AUTOMANUAL_ALL_COMMAND	//20140402
	//20131212 : mode_DisplayUpdate = (manual = 1) = RS232_TxSkip;
	RS232_TxSkip = mode_DisplayUpdate;		//20131212
#else	
	RS232_TxSkip = 0;
#endif	
//#endif
	//
	//Opaque Tx start			
	//
	Stingray_Tx_RotelOpaqueData(m);                 //RotelOpaqueData => UART4
}
/*	RKB : 20140325
	RS232 key 
	
	shutdown_temp [31..86]
************************************************/
void 	proc_KeyRs232_ShutdownSet(unsigned char val)
{

	if(mode_PowerStatus < POWER_ON3_ING) return; 				//20130708
	//
	//save to EEPROM
	//
	shutdown_temp = val;
	// compare Shutdown temp => (fan_hs_on_temp+1)
	if ( shutdown_temp <= fan_hs_on_temp ) {
		shutdown_temp = fan_hs_on_temp + 1;
	}
	nvr_xPutData(ADDR_SHUTDOWN_TEMP,shutdown_temp);
	//
	//2,2 digit
	//
	Func_getShutdownTemp();
}
/*	20140325
    shutdown_temp=xx!
************************************************/
const char str_shutdownTemp[] = "shutdown_temp=  !";
//--------------------------------------------------------------------
void 	Func_getShutdownTemp(void)
{
	unsigned short i,m;   //20120309
	unsigned short val_10,val_1,val;

	if(mode_PowerStatus < POWER_ON0) return; 				//20130710
	//
	//load shutdown_temp;
	//
	val = shutdown_temp;
	//
	//2,2 digit
	m=0;
	for(i=0;(i<18)&&(str_shutdownTemp[i]!='\0');i++) RotelOpaqueData[m++] = (unsigned char)str_shutdownTemp[i];	//
	//
	val_10 = (val)/10;
	val_1 = (val - val_10*10);
		    
    RotelOpaqueData[14] = (unsigned char)val_10 + 0x30;;
	RotelOpaqueData[15] = (unsigned char)val_1 + 0x30;;

	//Reciva OpaqueText Tx
	RotelOpaqueData[m] = 0x00;		//null(0x00)
    //------------------------------------------------------
    //------------------------------------------------------
    //------------------------------------------------------
	//addID_RoteOpaqueData(1,0,&m);
    //------------------------------------------------------
	//
	//RS232 Tx start
	//
//#ifdef RS232_SKIP	//20110114 test
	//20131212
	if (RS232_TxSkip == 0) {
		rs232_printf("\n");						//20131025
		for(i=0;i<m;i++) {								
			PutChar(RotelOpaqueData[i]);
		}
	}				
#ifdef DISPLAY_AUTOMANUAL_ALL_COMMAND	//20140402
	//20131212 : mode_DisplayUpdate = (manual = 1) = RS232_TxSkip;
	RS232_TxSkip = mode_DisplayUpdate;		//20131212
#else	
	RS232_TxSkip = 0;
#endif	
//#endif
	//
	//Opaque Tx start			
	//
	Stingray_Tx_RotelOpaqueData(m);                 //RotelOpaqueData => UART4
}
/*	RKB : 20140325
	RS232 key 
	
	SMPS_shutdown_temp [ 31..99]
************************************************/
void 	proc_KeyRs232_SmpsShutdownSet(unsigned char val)
{

	if(mode_PowerStatus < POWER_ON3_ING) return; 				//20130708
	//
	//save to EEPROM
	//
	SMPS_shutdown_temp = val;
	nvr_xPutData(ADDR_SMPS_SHUTDOWN_TEMP,SMPS_shutdown_temp);
	//
	//2 digit
	//
	Func_getSmpsShutdownTemp();
}
/*	20131024
    smps_shutdown_temp=xx!
    
	SMPS_shutdown_temp [ 31..99]
************************************************/
const char str_SmpsShutdownTemp[] = "smps_shutdown_temp=  !";
//--------------------------------------------------------------------
void 	Func_getSmpsShutdownTemp(void)
{
	unsigned short i,m;   //20120309
	unsigned short val_10,val_1,val;

	if(mode_PowerStatus < POWER_ON0) return; 				//20130710
	//
	//load shutdown_temp;
	//
	val = SMPS_shutdown_temp;
	//
	//2 digit
	m=0;
	for(i=0;(i<23)&&(str_SmpsShutdownTemp[i]!='\0');i++) RotelOpaqueData[m++] = (unsigned char)str_SmpsShutdownTemp[i];	//
	//
	val_10 = (val)/10;
	val_1 = (val - val_10*10);
		    
    RotelOpaqueData[19] = (unsigned char)val_10 + 0x30;;
	RotelOpaqueData[20] = (unsigned char)val_1 + 0x30;;

	//Reciva OpaqueText Tx
	RotelOpaqueData[m] = 0x00;		//null(0x00)
    //------------------------------------------------------
    //------------------------------------------------------
    //------------------------------------------------------
	//addID_RoteOpaqueData(1,0,&m);
    //------------------------------------------------------
	//
	//RS232 Tx start
	//
//#ifdef RS232_SKIP	//20110114 test
	//20131212
	if (RS232_TxSkip == 0) {
		rs232_printf("\n");						//20131025
		for(i=0;i<m;i++) {								
			PutChar(RotelOpaqueData[i]);
		}
	}				
#ifdef DISPLAY_AUTOMANUAL_ALL_COMMAND	//20140402
	//20131212 : mode_DisplayUpdate = (manual = 1) = RS232_TxSkip;
	RS232_TxSkip = mode_DisplayUpdate;		//20131212
#else	
	RS232_TxSkip = 0;
#endif	
//#endif
	//
	//Opaque Tx start			
	//
	Stingray_Tx_RotelOpaqueData(m);                 //RotelOpaqueData => UART4
}
/*
	//20131218 : add to RKB 

	// delay time volume data Tx [0 .. 30]
	time_delay_volume_Tx = 0;     //default(0)									
	nvr_xPutData(ADDR_DELAY_VOLUME_TX,time_delay_volume_Tx);
************************************************/
void 	Func_SetDelayTimeVolumeDataTx(unsigned char val)
{
	time_delay_volume_Tx = val;
	nvr_xPutData(ADDR_DELAY_VOLUME_TX,time_delay_volume_Tx);
	
	Func_getDelayTimeVolumeDataTx();

}
/*	20131218

	// delay time volume data Tx [0 .. 30]
	time_delay_volume_Tx = 0;     //default(0)									
	nvr_xPutData(ADDR_DELAY_VOLUME_TX,time_delay_volume_Tx);
************************************************/
const char str_DelayTimeVolumeDataTx[] = "delay_time_volume_tx=  !";
//--------------------------------------------------------------------
void 	Func_getDelayTimeVolumeDataTx(void)
{
	unsigned short i,m;   //20120309
	unsigned short val_10,val_1,val;

//#ifdef 1	//20130708 test
	if(mode_PowerStatus < POWER_ON0) return; 				//20130710

	//load time_delay_volume_Tx;
	//
	val = time_delay_volume_Tx;
	//
	//2 digit
	m=0;
	for(i=0;(i<25)&&(str_DelayTimeVolumeDataTx[i]!='\0');i++) RotelOpaqueData[m++] = (unsigned char)str_DelayTimeVolumeDataTx[i];	//
	//
	val_10 = (val)/10;
	val_1 = (val - val_10*10);
		    
    RotelOpaqueData[21] = (unsigned char)val_10 + 0x30;;
	RotelOpaqueData[22] = (unsigned char)val_1 + 0x30;;

	//Reciva OpaqueText Tx
	RotelOpaqueData[m] = 0x00;		//null(0x00)
    //------------------------------------------------------
    //------------------------------------------------------
    //------------------------------------------------------
	//addID_RoteOpaqueData(1,0,&m);
    //------------------------------------------------------
	//
	//RS232 Tx start
	//
//#ifdef RS232_SKIP	//20110114 test
	//20131212
	if (RS232_TxSkip == 0) {
		rs232_printf("\n");						//20131025
		for(i=0;i<m;i++) {								
			PutChar(RotelOpaqueData[i]);
		}
	}				
#ifdef DISPLAY_AUTOMANUAL_ALL_COMMAND	//20140402
	//20131212 : mode_DisplayUpdate = (manual = 1) = RS232_TxSkip;
	RS232_TxSkip = mode_DisplayUpdate;		//20131212
#else	
	RS232_TxSkip = 0;
#endif	
//#endif
	//
	//Opaque Tx start			
	//
	Stingray_Tx_RotelOpaqueData(m);                 //RotelOpaqueData => UART4
}
//20140213
/*
	get_balance
	balance=###!
	balance[ampType]
************************************************/
const char str_balanceIs_list[31][11+1] = {
			"balance=L15","balance=L14","balance=L13","balance=L12","balance=L11","balance=L10","balance=L09","balance=L08",
			"balance=L07","balance=L06","balance=L05","balance=L04","balance=L03","balance=L02","balance=L01","balance=000",
			"balance=R01","balance_R02","balance=R03","balance=R04","balance=R05","balance=R06","balance=R07","balance=R08",
			"balance=R09","balance=R10","balance=R11","balance=R12","balance=R13","balance=R14","balance=R15"
};
//---------------------------------------------------------------------
void 	Func_GetCurrentBalanceTx(unsigned char ampType)
{
	unsigned short i,m;   //20120313

    m=0;
    for(i=0;(i<12)&&(str_balanceIs_list[balance[ampType]][i]!='\0');i++) RotelOpaqueData[m++] = (unsigned char)str_balanceIs_list[balance[ampType]][i];
	
    RotelOpaqueData[m++] = '!';
    
	//Reciva OpaqueText Tx
	RotelOpaqueData[m] = 0x00;		//null(0x00)
    //------------------------------------------------------
    //------------------------------------------------------
	//(mode,ampType,&m)
	//mode=0 : skip
	//mode=2 : 0A: or 0B: or 0C: or 0D:
	//
    //ampType=0: "0A:" + RotelOpaqueData[m]
    //ampType=1: "0B:" + RotelOpaqueData[m]
    //ampType=2: "0C:" + RotelOpaqueData[m]
    //ampType=3: "0D:" + RotelOpaqueData[m]       
    //------------------------------------------------------
	addID_RoteOpaqueData(2,ampType,&m);
    //------------------------------------------------------
	//
	//RS232 Tx start
	//
//#ifdef RS232_SKIP	//20110114 test
	//20131212
	if (RS232_TxSkip == 0) {
		rs232_printf("\n");						//20131025
		for(i=0;i<m;i++) {								
			PutChar(RotelOpaqueData[i]);
		}
	}				
	//20131212 : mode_DisplayUpdate = (manual = 1) = RS232_TxSkip;
	if (ampType == 3) {
#ifdef DISPLAY_AUTOMANUAL_ALL_COMMAND	//20140402
		//20131212 : mode_DisplayUpdate = (manual = 1) = RS232_TxSkip;
		RS232_TxSkip = mode_DisplayUpdate;		//20131212
#else	
		RS232_TxSkip = 0;
#endif	
	}	
//#endif
	//
	//Opaque Tx start			
	//
	Stingray_Tx_RotelOpaqueData(m);                 //RotelOpaqueData => UART4
}
//--------------------------------------------------------------------
void 	Func_GetCurrentBalance(unsigned char ampType)
{

	if(Power_GetPowerStatus() == 0) return;		//?power off(0), skip
	if (ampType > 3) {
		//All-ch status Tx
		//20140325 : ? each CH power check
		if (amp_on_off[0] == 1 ) Func_GetCurrentBalanceTx(0);	//A-CH
		if (amp_on_off[1] == 1 ) Func_GetCurrentBalanceTx(1);	//B-CH
		if (amp_on_off[2] == 1 ) Func_GetCurrentBalanceTx(2);	//C-CH
		if (amp_on_off[3] == 1 ) Func_GetCurrentBalanceTx(3);	//D-CH		
	}else{
		//20140325
		//? curretn-ch power on
		if (amp_on_off[ampType] == 1 ) {	//?amp_on_off[]==on		
			//current-ch status Tx
			Func_GetCurrentBalanceTx(ampType);
#ifdef DISPLAY_AUTOMANUAL_ALL_COMMAND	//20140402
			//20131212 : mode_DisplayUpdate = (manual = 1) = RS232_TxSkip;
			RS232_TxSkip = mode_DisplayUpdate;		//20131212
#else	
			RS232_TxSkip = 0;
#endif	
		}
	}	
}
/*
    volume_max=87!
    volume_min=0!
    volume_step=0.5!
************************************************/
const char str_volumeMax2[] =	"volume_max=  !";
//---------------------------------------------------------------------
void 	Func_GetVolumeMax(void)
{
	unsigned short i,m;   //20120309
	unsigned char val_10,val_1;

    m=0;
	for(i=0;(i<20)&&(str_volumeMax2[i]!='\0');i++) RotelOpaqueData[m++] = (unsigned char)str_volumeMax2[i];	//

	val_10 = (MAX_MASTER_VOLUME)/10;
	val_1 = (MAX_MASTER_VOLUME - val_10*10 );
	    	    
    RotelOpaqueData[11] = (unsigned char)val_10 + 0x30;;
	RotelOpaqueData[12] = (unsigned char)val_1 + 0x30;;
	
	//Reciva OpaqueText Tx
	RotelOpaqueData[m] = 0x00;		//null(0x00)
    //------------------------------------------------------
    //------------------------------------------------------
    //------------------------------------------------------
	//(mode,ampType,&m)
	//mode=0 : skip
	//mode=2 : 0A: or 0B: or 0C: or 0D:
	//
    //ampType=0: "0A:" + RotelOpaqueData[m]
    //ampType=1: "0B:" + RotelOpaqueData[m]
    //ampType=2: "0C:" + RotelOpaqueData[m]
    //ampType=3: "0D:" + RotelOpaqueData[m]       
    //------------------------------------------------------
	//addID_RoteOpaqueData(2,ampType,&m);
    //------------------------------------------------------
	//
	//RS232 Tx start
	//
//#ifdef RS232_SKIP	//20110114 test
	//20131212
	if (RS232_TxSkip == 0) {
		rs232_printf("\n");						//20131025
		for(i=0;i<m;i++) {								
			PutChar(RotelOpaqueData[i]);
		}
	}				
#ifdef DISPLAY_AUTOMANUAL_ALL_COMMAND	//20140402
	//20131212 : mode_DisplayUpdate = (manual = 1) = RS232_TxSkip;
	RS232_TxSkip = mode_DisplayUpdate;		//20131212
#else	
	RS232_TxSkip = 0;
#endif	
//#endif
	//
	//Opaque Tx start			
	//
	Stingray_Tx_RotelOpaqueData(m);                 //RotelOpaqueData => UART4
}
/*

************************************************/
const char str_volumeMinValue[] = "volume_min=0!";
//---------------------------------------------------------------------
void 	Func_GetVolumeMin(void)
{
	unsigned short i,m;   //20120309

    m=0;
	for(i=0;(i<20)&&(str_volumeMinValue[i]!='\0');i++) RotelOpaqueData[m++] = (unsigned char)str_volumeMinValue[i];	//

	//Reciva OpaqueText Tx
	RotelOpaqueData[m] = 0x00;		//null(0x00)
    //------------------------------------------------------
    //------------------------------------------------------
    //------------------------------------------------------
	//(mode,ampType,&m)
	//mode=0 : skip
	//mode=2 : 0A: or 0B: or 0C: or 0D:
	//
    //ampType=0: "0A:" + RotelOpaqueData[m]
    //ampType=1: "0B:" + RotelOpaqueData[m]
    //ampType=2: "0C:" + RotelOpaqueData[m]
    //ampType=3: "0D:" + RotelOpaqueData[m]       
    //------------------------------------------------------
	//addID_RoteOpaqueData(2,ampType,&m);
    //------------------------------------------------------
	//
	//RS232 Tx start
	//
//#ifdef RS232_SKIP	//20110114 test
	//20131212
	if (RS232_TxSkip == 0) {
		rs232_printf("\n");						//20131025
		for(i=0;i<m;i++) {								
			PutChar(RotelOpaqueData[i]);
		}
	}				
#ifdef DISPLAY_AUTOMANUAL_ALL_COMMAND	//20140402
	//20131212 : mode_DisplayUpdate = (manual = 1) = RS232_TxSkip;
	RS232_TxSkip = mode_DisplayUpdate;		//20131212
#else	
	RS232_TxSkip = 0;
#endif	
//#endif
	//
	//Opaque Tx start			
	//
	Stingray_Tx_RotelOpaqueData(m);                 //RotelOpaqueData => UART4
}
/*
    volume=30!
    volume=max!	=> volume_00!
    volume=min! => volume_96!
    
//volatile byte volume_Lch[4];
//volatile byte volume_Rch[4];
************************************************/
const char str_volumeLchCurr[] ="volume_l_=  !";
const char str_volumeLchMax[] =	"volume_l_=max!";
const char str_volumeLchMin[] =	"volume_l_=min!";
//---------------------------------------------------------------------
void 	Func_VolumeLchStatusTx(unsigned char L_vol, unsigned char ampType)
{
	unsigned short i,m;   //20120309
	unsigned char val_10,val_1;

    m=0;

#ifdef DISPLAY_MAX_MIN_ENABLE						//20140304
	if (L_vol == MAX_MASTER_VOLUME) {				//?96
        //max!
	    for(i=0;i<14;i++) RotelOpaqueData[m++] = (unsigned char)str_volumeLchMax[i];	//
	}else if (L_vol == 0) {
	    //min!
	    for(i=0;i<14;i++) RotelOpaqueData[m++] = (unsigned char)str_volumeLchMin[i];	//
	}else
#endif
	{
	    //##!
	    for(i=0;i<13;i++) RotelOpaqueData[m++] = (unsigned char)str_volumeLchCurr[i];	//

		val_10 = (L_vol)/10;
		val_1 = (L_vol - val_10*10 );
	    	    
    	RotelOpaqueData[10] = (unsigned char)val_10 + 0x30;;
	    RotelOpaqueData[11] = (unsigned char)val_1 + 0x30;;
    }
	//Reciva OpaqueText Tx
	RotelOpaqueData[m] = 0x00;		//null(0x00)
    //------------------------------------------------------
    //------------------------------------------------------
    //------------------------------------------------------
	//(mode,ampType,&m)
	//mode=0 : skip
	//mode=2 : 0A: or 0B: or 0C: or 0D:
	//
    //ampType=0: "0A:" + RotelOpaqueData[m]
    //ampType=1: "0B:" + RotelOpaqueData[m]
    //ampType=2: "0C:" + RotelOpaqueData[m]
    //ampType=3: "0D:" + RotelOpaqueData[m]       
    //------------------------------------------------------
	addID_RoteOpaqueData(2,ampType,&m);
    //------------------------------------------------------
	//
	//RS232 Tx start
	//
//#ifdef RS232_SKIP	//20110114 test
	//20131212
	if (RS232_TxSkip == 0) {
		rs232_printf("\n");						//20131025
		for(i=0;i<m;i++) {								
			PutChar(RotelOpaqueData[i]);
		}
	}				
	//20131212 : mode_DisplayUpdate = (manual = 1) = RS232_TxSkip;
	if (ampType == 3) {
#ifdef DISPLAY_AUTOMANUAL_ALL_COMMAND	//20140402
		//20131212 : mode_DisplayUpdate = (manual = 1) = RS232_TxSkip;
		RS232_TxSkip = mode_DisplayUpdate;		//20131212
#else	
		RS232_TxSkip = 0;
#endif	
	}	
//#endif
	//
	//Opaque Tx start			
	//
	Stingray_Tx_RotelOpaqueData(m);                 //RotelOpaqueData => UART4
}
//--------------------------------------------------------------------
//	KEY : SK_GET_VOLUME ,SK_GET_VOLUME_L, SK_GET_VOLUME_R
//	if 
//--------------------------------------------------------------------
void 	Func_VolumeLchStatus(unsigned char ampType)
{

	if(Power_GetPowerStatus() == 0) return;		//?power off(0), skip
	if (ampType > 3) {
		//All-ch status Tx
		//20140221
		if (front_trimKnob_enable == OFF) {
			//volume_Lch[4]
			//20140325 : ? each CH power check
			if (amp_on_off[0] == 1 ) Func_VolumeLchStatusTx(volume_Lch[0],0);	//A-CH
			if (amp_on_off[1] == 1 ) Func_VolumeLchStatusTx(volume_Lch[1],1);	//B-CH
			if (amp_on_off[2] == 1 ) Func_VolumeLchStatusTx(volume_Lch[2],2);	//C-CH
			if (amp_on_off[3] == 1 ) Func_VolumeLchStatusTx(volume_Lch[3],3);	//D-CH		
		}else{
			//current_key[4]
			//20140325 : ? each CH power check
			if (amp_on_off[0] == 1 ) Func_VolumeLchStatusTx(current_key[0],0);	//A-CH
			if (amp_on_off[1] == 1 ) Func_VolumeLchStatusTx(current_key[1],1);	//B-CH
			if (amp_on_off[2] == 1 ) Func_VolumeLchStatusTx(current_key[2],2);	//C-CH
			if (amp_on_off[3] == 1 ) Func_VolumeLchStatusTx(current_key[3],3);	//D-CH		
		}			
	}else{
		//20140325
		//? curretn-ch power on
		if (amp_on_off[ampType] == 1 ) {	//?amp_on_off[]==on		
			//current-ch status Tx
			//20140221
			if (front_trimKnob_enable == OFF) {
				//volume_Lch[4]
				Func_VolumeLchStatusTx(volume_Lch[ampType],ampType);
			}else{
				//current_key[4]
				Func_VolumeLchStatusTx(current_key[ampType],ampType);
			}
			//
			//
			//
#ifdef DISPLAY_AUTOMANUAL_ALL_COMMAND	//20140402
			//20131212 : mode_DisplayUpdate = (manual = 1) = RS232_TxSkip;
			RS232_TxSkip = mode_DisplayUpdate;		//20131212
#else	
			RS232_TxSkip = 0;
#endif	
		}	
	}	
}
//---------------------------------------------------------------------
const char str_volumeRchCurr[] ="volume_r_=  !";
const char str_volumeRchMax[] =	"volume_r_=max!";
const char str_volumeRchMin[] =	"volume_r_=min!";
//---------------------------------------------------------------------
void 	Func_VolumeRchStatusTx(unsigned char R_vol, unsigned char ampType)
{
	unsigned short i,m;   //20120309
	unsigned char val_10,val_1;

    m=0;

#ifdef DISPLAY_MAX_MIN_ENABLE						//20140304
	if (R_vol == MAX_MASTER_VOLUME) {				//?96
        //max!
	    for(i=0;i<14;i++) RotelOpaqueData[m++] = (unsigned char)str_volumeRchMax[i];	//
	}else if (R_vol == 0) {
	    //min!
	    for(i=0;i<14;i++) RotelOpaqueData[m++] = (unsigned char)str_volumeRchMin[i];	//
	}else
#endif
	{
	    //##!
	    for(i=0;i<13;i++) RotelOpaqueData[m++] = (unsigned char)str_volumeRchCurr[i];	//

		val_10 = (R_vol)/10;
		val_1 = (R_vol - val_10*10 );
	    	    
    	RotelOpaqueData[10] = (unsigned char)val_10 + 0x30;;
	    RotelOpaqueData[11] = (unsigned char)val_1 + 0x30;;
    }
	//Reciva OpaqueText Tx
	RotelOpaqueData[m] = 0x00;		//null(0x00)
    //------------------------------------------------------
    //------------------------------------------------------
    //------------------------------------------------------
	//(mode,ampType,&m)
	//mode=0 : skip
	//mode=2 : 0A: or 0B: or 0C: or 0D:
	//
    //ampType=0: "0A:" + RotelOpaqueData[m]
    //ampType=1: "0B:" + RotelOpaqueData[m]
    //ampType=2: "0C:" + RotelOpaqueData[m]
    //ampType=3: "0D:" + RotelOpaqueData[m]       
    //------------------------------------------------------
	addID_RoteOpaqueData(2,ampType,&m);
    //------------------------------------------------------
	//
	//RS232 Tx start
	//
//#ifdef RS232_SKIP	//20110114 test
	//20131212
	if (RS232_TxSkip == 0) {
		rs232_printf("\n");						//20131025
		for(i=0;i<m;i++) {								
			PutChar(RotelOpaqueData[i]);
		}
	}				
	//20131212 : mode_DisplayUpdate = (manual = 1) = RS232_TxSkip;
	if (ampType == 3) {
#ifdef DISPLAY_AUTOMANUAL_ALL_COMMAND	//20140402
		//20131212 : mode_DisplayUpdate = (manual = 1) = RS232_TxSkip;
		RS232_TxSkip = mode_DisplayUpdate;		//20131212
#else	
		RS232_TxSkip = 0;
#endif	
	}	
//#endif
	//
	//Opaque Tx start			
	//
	Stingray_Tx_RotelOpaqueData(m);                 //RotelOpaqueData => UART4
}
//--------------------------------------------------------------------
//	KEY : SK_GET_VOLUME ,SK_GET_VOLUME_L, SK_GET_VOLUME_R
//	if 
//--------------------------------------------------------------------
void 	Func_VolumeRchStatus(unsigned char ampType)
{

	if(Power_GetPowerStatus() == 0) return;		//?power off(0), skip
	if (ampType > 3) {
		//All-ch status Tx
		//20140221
		if (front_trimKnob_enable == OFF) {
			//volume_Rch[4]
			//20140325 : ? each CH power check
			if (amp_on_off[0] == 1 ) Func_VolumeRchStatusTx(volume_Rch[0],0);	//A-CH
			if (amp_on_off[1] == 1 ) Func_VolumeRchStatusTx(volume_Rch[1],1);	//B-CH
			if (amp_on_off[2] == 1 ) Func_VolumeRchStatusTx(volume_Rch[2],2);	//C-CH
			if (amp_on_off[3] == 1 ) Func_VolumeRchStatusTx(volume_Rch[3],3);	//D-CH		
		}else{
			//current_key[4]
			//20140325 : ? each CH power check
			if (amp_on_off[0] == 1 ) Func_VolumeRchStatusTx(current_key[0],0);	//A-CH
			if (amp_on_off[1] == 1 ) Func_VolumeRchStatusTx(current_key[1],1);	//B-CH
			if (amp_on_off[2] == 1 ) Func_VolumeRchStatusTx(current_key[2],2);	//C-CH
			if (amp_on_off[3] == 1 ) Func_VolumeRchStatusTx(current_key[3],3);	//D-CH		
		}			
	}else{
		//20140325
		//? curretn-ch power on
		if (amp_on_off[ampType] == 1 ) {	//?amp_on_off[]==on		
			//current-ch status Tx
			//20140221
			if (front_trimKnob_enable == OFF) {
				//volume_Rch[4]
				Func_VolumeRchStatusTx(volume_Rch[ampType],ampType);
			}else{
				//current_key[4]
				Func_VolumeRchStatusTx(current_key[ampType],ampType);
			}
			//
			//
			//
#ifdef DISPLAY_AUTOMANUAL_ALL_COMMAND	//20140402
			//20131212 : mode_DisplayUpdate = (manual = 1) = RS232_TxSkip;
			RS232_TxSkip = mode_DisplayUpdate;		//20131212
#else	
			RS232_TxSkip = 0;
#endif	
		}		
	}	
}
//--------------------------------------------------------------------
//	KEY : SK_GET_VOLUME ,SK_GET_VOLUME_L, SK_GET_VOLUME_R
//	if 
//--------------------------------------------------------------------
void 	Func_VolumeLRStatus(unsigned char ampType)
{
	unsigned char i;

	if (ampType > 3) {
		//All-ch status Tx
		for (i=0;i<4;i++) {
			Func_VolumeLchStatus(i);			
			Func_VolumeRchStatus(i);
		}
	}else{
		//? current-ch
		Func_VolumeLchStatus(ampType);				
		Func_VolumeRchStatus(ampType);
	}
}
//---------------------------------------------------------------------
const char str_ampCH_on[] =	 "ch_power=on!";
const char str_ampCH_off[] = "ch_power=off!";
//volatile byte amp_on_off[4];
//volatile byte volume_Lch[4];
//volatile byte volume_Rch[4];
//volatile byte balance[4];
//volatile byte ch_muteOn[4];
//volatile byte front_trimKnob_enable;
//---------------------------------------------------------------------
void 	Func_AMP_CH_statusTx(unsigned char ampType)
{
	unsigned short i,m;

    m=0;
	if (amp_on_off[ampType] == 1) {
        //on!
	    for(i=0;i<12;i++) RotelOpaqueData[m++] = (unsigned char)str_ampCH_on[i];	//
	}else{
	    //off!
	    for(i=0;i<13;i++) RotelOpaqueData[m++] = (unsigned char)str_ampCH_off[i];	//
    }
	//Reciva OpaqueText Tx
	RotelOpaqueData[m] = 0x00;		//null(0x00)
    //------------------------------------------------------
    //------------------------------------------------------
    //------------------------------------------------------
	//(mode,ampType,&m)
	//mode=0 : skip
	//mode=2 : 0A: or 0B: or 0C: or 0D:
	//
    //ampType=0: "0A:" + RotelOpaqueData[m]
    //ampType=1: "0B:" + RotelOpaqueData[m]
    //ampType=2: "0C:" + RotelOpaqueData[m]
    //ampType=3: "0D:" + RotelOpaqueData[m]       
    //------------------------------------------------------
	addID_RoteOpaqueData(2,ampType,&m);
    //------------------------------------------------------
	//
	//RS232 Tx start
	//
//#ifdef RS232_SKIP	//20110114 test
	//20131212
	if (RS232_TxSkip == 0) {
		rs232_printf("\n");						//20131025
		for(i=0;i<m;i++) {								
			PutChar(RotelOpaqueData[i]);
		}
	}				
	//20131212 : mode_DisplayUpdate = (manual = 1) = RS232_TxSkip;
	if (ampType == 3) {
#ifdef DISPLAY_AUTOMANUAL_ALL_COMMAND	//20140402
		//20131212 : mode_DisplayUpdate = (manual = 1) = RS232_TxSkip;
		RS232_TxSkip = mode_DisplayUpdate;		//20131212
#else	
		RS232_TxSkip = 0;
#endif	
	}	
//#endif
	//
	//Opaque Tx start			
	//
	Stingray_Tx_RotelOpaqueData(m);                 //RotelOpaqueData => UART4
}
//---------------------------------------------------------------------
//20140826
const char str_SSM_amp_on[] =	"ch_power=amp_on!";
const char str_SSM_amp_off[] =	"ch_power=amp_off!";
//---------------------------------------------------------------------
void 	Func_SSM_AMP_statusTx(unsigned char type)
{
	unsigned short i,m;

    m=0;
	if (type == 1) {
        //on!
	    for(i=0;i<17;i++) RotelOpaqueData[m++] = (unsigned char)str_SSM_amp_on[i];	//
	}else{
	    //off!
	    for(i=0;i<18;i++) RotelOpaqueData[m++] = (unsigned char)str_SSM_amp_off[i];	//
    }
	//Reciva OpaqueText Tx
	RotelOpaqueData[m] = 0x00;		//null(0x00)
    //------------------------------------------------------
    //------------------------------------------------------
    //------------------------------------------------------
	//(mode,ampType,&m)
	//mode=0 : skip
	//mode=2 : 0A: or 0B: or 0C: or 0D:
	//
    //ampType=0: "0A:" + RotelOpaqueData[m]
    //ampType=1: "0B:" + RotelOpaqueData[m]
    //ampType=2: "0C:" + RotelOpaqueData[m]
    //ampType=3: "0D:" + RotelOpaqueData[m]       
    //------------------------------------------------------
	//addID_RoteOpaqueData(2,ampType,&m);
    //------------------------------------------------------
	//
	//RS232 Tx start
	//
//#ifdef RS232_SKIP	//20110114 test
	//20131212
	if (RS232_TxSkip == 0) {
		rs232_printf("\n");						//20131025
		for(i=0;i<m;i++) {								
			PutChar(RotelOpaqueData[i]);
		}
	}				
#ifdef DISPLAY_AUTOMANUAL_ALL_COMMAND	//20140402
	//20131212 : mode_DisplayUpdate = (manual = 1) = RS232_TxSkip;
	RS232_TxSkip = mode_DisplayUpdate;		//20131212
#else	
	RS232_TxSkip = 0;
#endif	
//#endif
	//
	//Opaque Tx start			
	//
	Stingray_Tx_RotelOpaqueData(m);                 //RotelOpaqueData => UART4
}

/*
	20140826 : 
		when Signal Sensing Mode,
			if (signalSensingAMPStatus == on)  then "ch_power=amp_on!" 
			if (signalSensingAMPStatus == off) then "ch_power=amp_off!"
**********************************************************************/		
void 	Func_AMP_CH_status(unsigned char ampType)
{
	unsigned char i;
#ifdef _RKB_DM8100
	if ((GetSignalSensing_nSW == 0) ) {
		//signal sense mode
		Func_SSM_AMP_statusTx(signalSensingAMPStatus);
	}else{
		//
		//Normal Mode :	
		//
		if (ampType > 3) {
			for (i=0;i<4;i++) Func_AMP_CH_statusTx(i);				
		}else{
			//current-ch status Tx
			Func_AMP_CH_statusTx(ampType);

#ifdef DISPLAY_AUTOMANUAL_ALL_COMMAND	//20140402
			//20131212 : mode_DisplayUpdate = (manual = 1) = RS232_TxSkip;
			RS232_TxSkip = mode_DisplayUpdate;		//20131212
#else	
			RS232_TxSkip = 0;
#endif	
		}
	}
#else

	//if(Power_GetPowerStatus() == 0) return;		//?power off(0), skip

	//? PP & MP2 unit & signal sensing nSW
	//add 20140826
	if ( (RKB_mode_PP_MP == 1)&&(RKB_mode_PP_nMP2 == 0)&&(GetSignalSensing_nSW == 0) ) {
		//
		//Signal Sensing Mode :
		//add 20140826
		Func_SSM_AMP_statusTx(signalSensingAMPStatus);
	}else{
		//
		//Normal Mode :	
		//
		if (ampType > 3) {
			//All-ch status Tx
			for (i=0;i<4;i++) Func_AMP_CH_statusTx(i);				
		}else{
			//current-ch status Tx
			Func_AMP_CH_statusTx(ampType);
			//
			//
			//
#ifdef DISPLAY_AUTOMANUAL_ALL_COMMAND	//20140402
			//20131212 : mode_DisplayUpdate = (manual = 1) = RS232_TxSkip;
			RS232_TxSkip = mode_DisplayUpdate;		//20131212
#else	
			RS232_TxSkip = 0;
#endif	
		}
	}
#endif
}
//--------------------------------------------------------------------
//	KEY : 
//		check All CH On ?
//	
//	20140523 : check signal sensing feature
//
//--------------------------------------------------------------------
void 	Func_AMP_CH_on(unsigned char ampType)
{
	unsigned char i;

	//? PP & MP2 unit & signal sensing nSW
	//20140819 enable
	if ( (RKB_mode_PP_MP == 1)&&(RKB_mode_PP_nMP2 == 0)&&(GetSignalSensing_nSW == 0) ) return;	

	Func_MuteFlagAllCH_Off();
	//--------------------------------------------------
	if(Power_GetPowerStatus() == 0) {		//?power off(0),
		//--------------------------------------------------
		//Now unit-power off => power-on exec.
		//--------------------------------------------------
		if (ampType > 3) {
			//--------------------------------------------------
			//All CH on
			//--------------------------------------------------
			for (i=0;i<4;i++) {
				amp_on_off[i] = 1;
				eepromAddr = ADDR_AMP_A_ONOFF+i;					//20140224
				nvr_xPutData(eepromAddr,amp_on_off[i]);				
			} 			
		}else{
			//--------------------------------------------------
			// this CH on
			//--------------------------------------------------
			amp_on_off[ampType] = 1;
			eepromAddr = ADDR_AMP_A_ONOFF+ampType;					//20140224
			nvr_xPutData(eepromAddr,amp_on_off[ampType]);
		}
		//All Power on -> exec.
		//powerOn -> CH on/off checking -> CH-off is muteOn => CH-on is volumeOn
		PowerOn_StandBy(BOOT_NORMAL);
		//
#ifdef ASCiiKEY
		Func_PowerStatus();
		//all CH status
		Func_AMP_CH_status(4);
#endif		
		//--------------------------------------------------
	}else{
		//--------------------------------------------------
		//Now unit-power on => 
		//--------------------------------------------------
		if (ampType > 3) {
			//--------------------------------------------------
			//All CH on
			//--------------------------------------------------
			// all mute flag clear! ->
			Func_MuteFlagAllCH_Off();
			//
			for (i=0;i<4;i++) {
				//20140325 : ? each CH power OFF check
				if (amp_on_off[i] == 0) {
					//20140325 : ? 
					amp_on_off[i] = 1;
					eepromAddr = ADDR_AMP_A_ONOFF+i;					//20140224
					nvr_xPutData(eepromAddr,amp_on_off[i]);				
					// volume Tx ->
					//Set_MainVolume(vol,ampType) => L/R Tx
					//20140221
					if (front_trimKnob_enable == OFF) {
						//volume_Lch[4]
						//volume_Rch[4]
						//20140305 : if same Lch & Rch and then have to balance calculation
						if ( volume_Lch[i] != volume_Rch[i] ) {
							//Set_MainVolume(vol,ampType) => L/R Tx
							Set_MainVolumeLeft(volume_Lch[i],i);		//mute off
							Set_MainVolumeRight(volume_Rch[i],i);		//mute off
						}else{
							//Tx Rch & balance									
							Set_MainVolume((unsigned char)volume_Lch[i],(i));
						}
					}else{
						//current_key[4]
						//Set_MainVolume(vol,ampType) => L/R Tx & balance!
						Set_MainVolume((unsigned char)current_key[i],(i));      //=>RKB_function.c
					}	
				}				
			}
			//all CH status
			Func_AMP_CH_status(4);
			//--------------------------------------------------
		}else{
			//--------------------------------------------------
			// ? this CH ON
			//--------------------------------------------------
			if (amp_on_off[ampType] == 0) {
				// this CH off->on
				// mute flag clear! ->
				Func_MuteFlag_CH_Off(ampType);			
				// amp on -> 
				amp_on_off[ampType] = 1;
				eepromAddr = ADDR_AMP_A_ONOFF+ampType;					//20140224
				nvr_xPutData(eepromAddr,amp_on_off[ampType]);
				// volume Tx ->
				//20140221
				if (front_trimKnob_enable == OFF) {
					//volume_Lch[4]
					//volume_Rch[4]
					//20140404 : if same Lch & Rch and then have to balance calculation
					if ( volume_Lch[ampType] != volume_Rch[ampType] ) {
						//Set_MainVolume(vol,ampType) => L/R Tx
						Set_MainVolumeLeft(volume_Lch[ampType],ampType);		//mute off
						Set_MainVolumeRight(volume_Rch[ampType],ampType);		//mute off
					}else{
						//Tx Rch & balance									
						Set_MainVolume((unsigned char)volume_Lch[ampType],(ampType));
					}
				}else{
					//current_key[4]
					//Set_MainVolume(vol,ampType) => L/R Tx & balance!
					Set_MainVolume((unsigned char)current_key[ampType],(ampType));      //=>RKB_function.c
				}
			}
			//this ch status
			Func_AMP_CH_status(ampType);
		}			
		//--------------------------------------------------
	}
	//--------------------------------------------------
}
//--------------------------------------------------------------------
//	KEY : 
//		check All CH OFF ?
//	
//	20140523 : check signal sensing feature
//
//--------------------------------------------------------------------
void 	Func_AMP_CH_off(unsigned char ampType)
{
	unsigned char i,j;

	//? PP & MP2 unit & signal sensing nSW
	//20140819 enable
	if ( (RKB_mode_PP_MP == 1)&&(RKB_mode_PP_nMP2 == 0)&&(GetSignalSensing_nSW == 0) ) return;	

	if(mode_PowerStatus >= POWER_ON0) {		//?POWER_ON0(8) , ?skip : 20140325
		//
		//Now unit-power on => 
		if (ampType > 3) {
			//--------------------------------------------------
			//All CH off -> goto standby-on
			//--------------------------------------------------
			// all mute flag clear! ->
			Func_MuteFlagAllCH_Off();
			//
			for (i=0;i<4;i++) {
				amp_on_off[i] = 0;
				eepromAddr = ADDR_AMP_A_ONOFF+i;					//20140224
				nvr_xPutData(eepromAddr,amp_on_off[i]);				
			}
			// volume Tx ->
			//Set_MainVolume(vol,ampType) => L/R Tx
			for (i=0;i<4;i++) {
				//Set_MainVolume(vol,ampType) => L/R Tx
				Set_MainVolume(0,i);						//mute=0
			}
			//
			//goto standby-on
			Func_PowerOff();
			//			
#ifdef ASCiiKEY
			Func_PowerStatus();
			//all CH status
			Func_AMP_CH_status(4);
#endif		
			//
		}else{
			//--------------------------------------------------
			//this CH off -> volume mute-on
			//--------------------------------------------------
			if (amp_on_off[ampType] == 1) {
				// this CH on->off
				amp_on_off[ampType] = 0;
				eepromAddr = ADDR_AMP_A_ONOFF+ampType;					//20140224
				nvr_xPutData(eepromAddr,amp_on_off[ampType]);
				//		
				//? all ch-off check ->goto standby
				j=0;
				for (i=0;i<4;i++) {
					if (amp_on_off[i] == 0) ++j;
				}
				if (j == 4) {
					// all mute flag clear! ->
					Func_MuteFlagAllCH_Off();
					// volume Tx ->
					//Set_MainVolume(vol,ampType) => L/R Tx
					for (i=0;i<4;i++) {
						//Set_MainVolume(vol,ampType) => L/R Tx
						Set_MainVolume(0,i);						//mute=0
					}			
					//
					//goto standby-on
					//
					Func_PowerOff();
#ifdef ASCiiKEY
					Func_PowerStatus();
					//all CH status
					Func_AMP_CH_status(4);
#endif		
					//
				}else{
					// mute flag clear! ->
					Func_MuteFlag_CH_Off(ampType);
					//Set_MainVolume(vol,ampType) => L/R Tx
					Set_MainVolume(0,ampType);						//mute=0			
					//this ch status
					Func_AMP_CH_status(ampType);
				}			
			}	
		}	
	}
}
//---------------------------------------------------------------------
const char str_frontTrimEnable[] =	 "front_trim=enable!";
const char str_frontTrimDisable[] =	 "front_trim=disable!";
//volatile byte amp_on_off[4];
//volatile byte volume_Lch[4];
//volatile byte volume_Rch[4];
//volatile byte balance[4];
//volatile byte ch_muteOn[4];
//volatile byte front_trimKnob_enable;
//---------------------------------------------------------------------
void 	Func_frontTrimStatus(void)
{
	unsigned short i,m;

    m=0;
	if (front_trimKnob_enable == ON) {
        //on!
	    for(i=0;i<18;i++) RotelOpaqueData[m++] = (unsigned char)str_frontTrimEnable[i];		//
	}else{
	    //off!
	    for(i=0;i<19;i++) RotelOpaqueData[m++] = (unsigned char)str_frontTrimDisable[i];	//
    }
	//Reciva OpaqueText Tx
	RotelOpaqueData[m] = 0x00;		//null(0x00)
    //------------------------------------------------------
    //------------------------------------------------------
    //------------------------------------------------------
	//(mode,ampType,&m)
	//mode=0 : skip
	//mode=2 : 0A: or 0B: or 0C: or 0D:
	//
    //ampType=0: "0A:" + RotelOpaqueData[m]
    //ampType=1: "0B:" + RotelOpaqueData[m]
    //ampType=2: "0C:" + RotelOpaqueData[m]
    //ampType=3: "0D:" + RotelOpaqueData[m]       
    //------------------------------------------------------
	//addID_RoteOpaqueData(2,ampType,&m);
    //------------------------------------------------------
	//
	//RS232 Tx start
	//
//#ifdef RS232_SKIP	//20110114 test
	//20131212
	if (RS232_TxSkip == 0) {
		rs232_printf("\n");						//20131025
		for(i=0;i<m;i++) {								
			PutChar(RotelOpaqueData[i]);
		}
	}				
#ifdef DISPLAY_AUTOMANUAL_ALL_COMMAND	//20140402
	//20131212 : mode_DisplayUpdate = (manual = 1) = RS232_TxSkip;
	RS232_TxSkip = mode_DisplayUpdate;		//20131212
#else	
	RS232_TxSkip = 0;
#endif	
//#endif
	//
	//Opaque Tx start			
	//
	Stingray_Tx_RotelOpaqueData(m);                 //RotelOpaqueData => UART4
}
//--------------------------------------------------------------------
//	volume All CH re-setting!
//--------------------------------------------------------------------
void 	Func_frontTrim_enable(void)
{
	unsigned char i;

	if(Power_GetPowerStatus() == 0) {		//?power off(0),
	}else{
		if (front_trimKnob_enable == OFF) {
			//
			//front knob enable!
			//    
			front_trimKnob_enable = ON;
			nvr_xPutData(ADDR_FRONT_TRIM_ENABLE,front_trimKnob_enable);
			//status front knob enable
			Func_frontTrimStatus();
			//
			// volume All CH re-setting ==> key [ flag key.KeyOk[] = ON ]
			//
			for (i=0;i<4;i++) {
				key.KeyOk[i] = ON;
				if ( volume_Lch[i] >= volume_Rch[i] ) {
					current_key[i] = volume_Rch[i];
				}else{
					current_key[i] = volume_Lch[i];
				}
				//current value re-setting ==> and then search value!!!!
				//Set_MainVolume(vol,ampType) => L/R Tx
				Set_MainVolume(current_key[i],i);
			}
		}else{
			//status
			Func_frontTrimStatus();
		}
	}
}
//--------------------------------------------------------------------
//	1st : balance All CH , if ( Lch != Rch ) , and then default setting
//	2nd : volume All CH re-setting!
//--------------------------------------------------------------------
void 	Func_frontTrim_disable(void)
{
	unsigned char i;

	if(Power_GetPowerStatus() == 0) {		//?power off(0),
	}else{
		if (front_trimKnob_enable == ON) {
			//
			//front knob disable!
			//  
			front_trimKnob_enable = OFF;
			nvr_xPutData(ADDR_FRONT_TRIM_ENABLE,front_trimKnob_enable);
			//status front knob disable
			Func_frontTrimStatus();
			//
			// balance All CH , if ( Lch != Rch ) , and then default setting
			// 20140305
			for (i=0;i<4;i++) {
				if ( volume_Lch[i] != volume_Rch[i] ) {
			    	balance[i] = BALANCE_VOLUME_INITIAL;	//15
					eepromAddr = ADDR_A_BAL+i;					//20140224
					nvr_xPutData(eepromAddr,balance[i]);
				}
			}
			//
			// volume All CH re-setting
			//RS232 L/R-ch volume data Tx
			//
			for (i=0;i<4;i++) {
				if (amp_on_off[i] == 1) {
					//mute off ->volume data Tx
					//20140305 : if same Lch & Rch and then have to balance calculation
					if ( volume_Lch[i] != volume_Rch[i] ) {
            			Set_MainVolumeLeft((unsigned char)volume_Lch[i],(i));
            			Set_MainVolumeRight((unsigned char)volume_Rch[i],(i));
            		}else{
						//(Lch = Rch) calculate balance value!
						//20140305
						Set_MainVolume(volume_Lch[i],i);
            		}
					//rs232_printf("ASCiiKey=>%x\n",cmd_code);
				}else{
					//mute
					Set_MainVolume(0,i);
				}
				Func_VolumeLchStatus((i));
				Func_VolumeRchStatus((i));
			}
		}else{
			//status
			Func_frontTrimStatus();
		}
	}
}
//
#ifdef LED_DIMMER_ENABLE
//---------------------------------------------------------------------
const char str_ledDimmerCurr[] =	"led_dimmer= !";
const char str_ledDimmerMax[] =		"led_dimmer=max!";
const char str_ledDimmerMin[] =		"led_dimmer=min!";
//--------------------------------------------------------------------
//	LED_Dimmer_value (internal)
//	[ 1(bright=MIN),,,,10(dark=MAX) ]
//	LED_Dimmer display 
//	[ 0(bright=MIN),,,,9(dark=MAX) ]
//--------------------------------------------------------------------
//20140225 : LED dimming display table
//const byte TBL_DIMMER_TIMER[11] = { 20,20,16,12,9,7,5,4,3,2,1 };
const byte TBL_DIMMER_DISPLAY[11] = { 0,0,1,2,3,4,5,6,7,8,9 };
//--------------------------------------------------------------------
//--------------------------------------------------------------------
void 	Func_ledDimmerStatus(void)
{
	unsigned short i,m;
	//unsigned char val_10,val_1;
	unsigned char val;
	
	if (LED_Dimmer_value == 0) return;
	
    m=0;
#ifdef LED_DIMMER_MAX_ENABLE
	if (LED_Dimmer_value == LED_DIMMER_MIN) {
		//bright! =MIN=max!
		for(i=0;(i<16)&&(str_ledDimmerMax[i]!='\0');i++) RotelOpaqueData[m++] = (unsigned char)str_ledDimmerMax[i];	//
	}else if (LED_Dimmer_value == LED_DIMMER_MAX) {
		//dark!  =MAX=min!
		for(i=0;(i<16)&&(str_ledDimmerMin[i]!='\0');i++) RotelOpaqueData[m++] = (unsigned char)str_ledDimmerMin[i];	//
	}else{
		//led_dimmer=#!
		for(i=0;(i<13)&&(str_ledDimmerCurr[i]!='\0');i++) RotelOpaqueData[m++] = (unsigned char)str_ledDimmerCurr[i];
		val = TBL_DIMMER_DISPLAY[LED_Dimmer_value];
		//val_10 = (val)/10;
		//val_1 = (val - val_10*10 );
			    
        //RotelOpaqueData[11] = (unsigned char)val_10 + 0x30;;
		//RotelOpaqueData[12] = (unsigned char)val_1 + 0x30;;
		RotelOpaqueData[11] = (unsigned char)val + 0x30;;
	}	
#else
	//led_dimmer=#!
	for(i=0;(i<13)&&(str_ledDimmerCurr[i]!='\0');i++) RotelOpaqueData[m++] = (unsigned char)str_ledDimmerCurr[i];
	val = TBL_DIMMER_DISPLAY[LED_Dimmer_value];
	//val_10 = (val)/10;
	//val_1 = (val - val_10*10 );
		    
    //RotelOpaqueData[11] = (unsigned char)val_10 + 0x30;;
	//RotelOpaqueData[12] = (unsigned char)val_1 + 0x30;;
	RotelOpaqueData[11] = (unsigned char)val + 0x30;;
#endif
	//Reciva OpaqueText Tx
	RotelOpaqueData[m] = 0x00;		//null(0x00)
    //------------------------------------------------------
    //------------------------------------------------------
    //------------------------------------------------------
	//(mode,ampType,&m)
	//mode=0 : skip
	//mode=2 : 0A: or 0B: or 0C: or 0D:
	//
    //ampType=0: "0A:" + RotelOpaqueData[m]
    //ampType=1: "0B:" + RotelOpaqueData[m]
    //ampType=2: "0C:" + RotelOpaqueData[m]
    //ampType=3: "0D:" + RotelOpaqueData[m]       
    //------------------------------------------------------
	//addID_RoteOpaqueData(2,ampType,&m);
    //------------------------------------------------------
	//
	//RS232 Tx start
	//
//#ifdef RS232_SKIP	//20110114 test
	//20131212
	if (RS232_TxSkip == 0) {
		rs232_printf("\n");						//20131025
		for(i=0;i<m;i++) {								
			PutChar(RotelOpaqueData[i]);
		}
	}				
#ifdef DISPLAY_AUTOMANUAL_ALL_COMMAND	//20140402
	//20131212 : mode_DisplayUpdate = (manual = 1) = RS232_TxSkip;
	RS232_TxSkip = mode_DisplayUpdate;		//20131212
#else	
	RS232_TxSkip = 0;
#endif	
//#endif
	//
	//Opaque Tx start			
	//
	Stingray_Tx_RotelOpaqueData(m);                 //RotelOpaqueData => UART4
}
//----------------------------------------------------------
//	20140114 : LED_Dimmer_value[1(bright=MIN)..10(dark=MAX)]
//----------------------------------------------------------
void 	Func_LED_Dimmer_set(unsigned char val)
{
	LED_Dimmer_value = val;		
	if ( (val >= LED_DIMMER_MIN) && (val <= LED_DIMMER_MAX) ) {
		nvr_xPutData(ADDR_LED_DIMMER,LED_Dimmer_value);
		//Tx status
		Func_ledDimmerStatus();
	}	
}
#endif	//LED_DIMMER_ENABLE
//
//
#ifdef	OPTICAL_POWER_ENABLE
//---------------------------------------------------------------------
const char str_opticalPowerEnable[] =	 "optical_power=enable!";
const char str_opticalPowerDisable[] =	 "optical_power=disable!";
//	check digital model
//	optical_power_mode
//---------------------------------------------------------------------
void 	Func_opticalPowerModeStatus(void)
{
	unsigned short i,m;

	if ( (RKB_ModelID == RKB_D850) || (RKB_ModelID == RKB_D8100) ) {
		
    m=0;
	if (optical_power_mode == ON) {
        //on!
	    for(i=0;i<18;i++) RotelOpaqueData[m++] = (unsigned char)str_opticalPowerEnable[i];		//
	}else{
	    //off!
	    for(i=0;i<19;i++) RotelOpaqueData[m++] = (unsigned char)str_opticalPowerDisable[i];	//
    }
	//Reciva OpaqueText Tx
	RotelOpaqueData[m] = 0x00;		//null(0x00)
    //------------------------------------------------------
    //------------------------------------------------------
    //------------------------------------------------------
	//(mode,ampType,&m)
	//mode=0 : skip
	//mode=2 : 0A: or 0B: or 0C: or 0D:
	//
    //ampType=0: "0A:" + RotelOpaqueData[m]
    //ampType=1: "0B:" + RotelOpaqueData[m]
    //ampType=2: "0C:" + RotelOpaqueData[m]
    //ampType=3: "0D:" + RotelOpaqueData[m]       
    //------------------------------------------------------
	//addID_RoteOpaqueData(2,ampType,&m);
    //------------------------------------------------------
	//
	//RS232 Tx start
	//
//#ifdef RS232_SKIP	//20110114 test
	//20131212
	if (RS232_TxSkip == 0) {
		rs232_printf("\n");						//20131025
		for(i=0;i<m;i++) {								
			PutChar(RotelOpaqueData[i]);
		}
	}				
#ifdef DISPLAY_AUTOMANUAL_ALL_COMMAND	//20140402
	//20131212 : mode_DisplayUpdate = (manual = 1) = RS232_TxSkip;
	RS232_TxSkip = mode_DisplayUpdate;		//20131212
#else	
	RS232_TxSkip = 0;
#endif	
//#endif
	//
	//Opaque Tx start			
	//
	Stingray_Tx_RotelOpaqueData(m);                 //RotelOpaqueData => UART4
	
	}
}
//--------------------------------------------------------------------
//	check digital model
//--------------------------------------------------------------------
void 	Func_opticalPower_enable(void)
{
	unsigned char i;

	if(Power_GetPowerStatus() == 0) {		//?power off(0),
	}else{
		//digital Model ?
		if ( (RKB_ModelID == RKB_D850) || (RKB_ModelID == RKB_D8100) ) {
			if (optical_power_mode == OFF) {
				//
				//optical_power_mode!
				//    
				optical_power_mode = ON;
				nvr_xPutData(optical_power_mode,ADDR_OPTI_POWER_MODE);			
				//status 
				Func_opticalPowerModeStatus();
			}else{
				//status
				Func_opticalPowerModeStatus();
			}
		}
	}
}
//--------------------------------------------------------------------
//	check digital model
//--------------------------------------------------------------------
void 	Func_opticalPower_disable(void)
{
	unsigned char i;

	if(Power_GetPowerStatus() == 0) {		//?power off(0),
	}else{
		//digital Model ?
		if ( (RKB_ModelID == RKB_D850) || (RKB_ModelID == RKB_D8100) ) {
			if (optical_power_mode == ON) {
				//
				//optical_power_mode!
				//  
				optical_power_mode = OFF;
				nvr_xPutData(optical_power_mode,ADDR_OPTI_POWER_MODE);			
				//status
				Func_opticalPowerModeStatus();
			}else{
				//status
				Func_opticalPowerModeStatus();
			}
		}
	}
}
#endif	//OPTICAL_POWER_ENABLE
//20150302
/*
//unsigned char input_selMode[4];
************************************************/
const char str_inputSelModeType[3][23+1] = { 
											"input_sel_mode=auto!",
											"input_sel_mode=digital!",
											"input_sel_mode=analog!"};
//--------------------------------------------------------------------
void 	Func_inputSelStatusTx(unsigned char ampType)
{
	unsigned short i,m;

    m=0;
    for(i=0;(i<25)&&(str_inputSelModeType[input_selMode[ampType]][i]!='\0');i++) RotelOpaqueData[m++] = (unsigned char)str_inputSelModeType[input_selMode[ampType]][i];	//
	//Reciva OpaqueText Tx
	RotelOpaqueData[m] = 0x00;		//null(0x00)
    //------------------------------------------------------
    //------------------------------------------------------
    //------------------------------------------------------
	//(mode,ampType,&m)
	//mode=0 : skip
	//mode=2 : 0A: or 0B: or 0C: or 0D:
	//
    //ampType=0: "0A:" + RotelOpaqueData[m]
    //ampType=1: "0B:" + RotelOpaqueData[m]
    //ampType=2: "0C:" + RotelOpaqueData[m]
    //ampType=3: "0D:" + RotelOpaqueData[m]       
    //------------------------------------------------------
	addID_RoteOpaqueData(2,ampType,&m);
    //------------------------------------------------------
	//
	//RS232 Tx start
	//
//#ifdef RS232_SKIP	//20110114 test
	//20131212
	if (RS232_TxSkip == 0) {
		rs232_printf("\n");						//20131025
		for(i=0;i<m;i++) {								
			PutChar(RotelOpaqueData[i]);
		}
	}				
	//20131212 : mode_DisplayUpdate = (manual = 1) = RS232_TxSkip;
	if (ampType == 3) {
#ifdef DISPLAY_AUTOMANUAL_ALL_COMMAND	//20140402
		//20131212 : mode_DisplayUpdate = (manual = 1) = RS232_TxSkip;
		RS232_TxSkip = mode_DisplayUpdate;		//20131212
#else	
		RS232_TxSkip = 0;
#endif	
	}	
//#endif
	//
	//Opaque Tx start			
	//
	Stingray_Tx_RotelOpaqueData(m);                 //RotelOpaqueData => UART4
}
//--------------------------------------------------------------------
//--------------------------------------------------------------------
void 	Func_inputSelStatus(unsigned char ampType)
{
	unsigned char i;

	//if(Power_GetPowerStatus() == 0) return;		//?power off(0), skip
	//20150303
	if ( (RKB_ModelID == RKB_D850) || (RKB_ModelID == RKB_D8100) ) {
		//
		//Normal Mode :	
		//
		if (ampType > 3) {
			//All-ch status Tx
			for (i=0;i<4;i++) Func_inputSelStatusTx(i);				
		}else{
			//current-ch status Tx
			Func_inputSelStatusTx(ampType);
			//
			//
			//
#ifdef DISPLAY_AUTOMANUAL_ALL_COMMAND	//20140402
			//20131212 : mode_DisplayUpdate = (manual = 1) = RS232_TxSkip;
			RS232_TxSkip = mode_DisplayUpdate;		//20131212
#else	
			RS232_TxSkip = 0;
#endif	
		}
	}
}
//--------------------------------------------------------------------
//unsigned char input_selMode[4];
//--------------------------------------------------------------------
void 	Func_inputSel_auto(unsigned char ampType)
{
	unsigned char i;

	//20150303
	if ( (RKB_ModelID == RKB_D850) || (RKB_ModelID == RKB_D8100) ) {
		if (ampType > 3) {
			//--------------------------------------------------
			//All CH set : INPUT_SEL_AUTO
			//--------------------------------------------------
			for (i=0;i<4;i++) {
				input_selMode[i] = INPUT_SEL_AUTO;
				eepromAddr = ADDR_AMP_A_INPUTSELMODE+i;
				nvr_xPutData(eepromAddr,input_selMode[i]);				
#ifdef ASCiiKEY
				//all CH status
				Func_inputSelStatus(i);
#endif		
			} 			
		}else{
			//--------------------------------------------------
			// this CH set : INPUT_SEL_AUTO
			//--------------------------------------------------
			input_selMode[ampType] = INPUT_SEL_AUTO;
			eepromAddr = ADDR_AMP_A_INPUTSELMODE+ampType;
			nvr_xPutData(eepromAddr,input_selMode[ampType]);

#ifdef ASCiiKEY
			//this CH status
			Func_inputSelStatus(ampType);
#endif		
		}
	}
}
//
//--------------------------------------------------------------------
//unsigned char input_selMode[4];
//--------------------------------------------------------------------
void 	Func_inputSel_digital(unsigned char ampType)
{
	unsigned char i;

	//20150303
	if ( (RKB_ModelID == RKB_D850) || (RKB_ModelID == RKB_D8100) ) {
		if (ampType > 3) {
			//--------------------------------------------------
			//All CH set : INPUT_SEL_DIGITAL
			//--------------------------------------------------
			for (i=0;i<4;i++) {
				input_selMode[i] = INPUT_SEL_DIGITAL;
				eepromAddr = ADDR_AMP_A_INPUTSELMODE+i;
				nvr_xPutData(eepromAddr,input_selMode[i]);				
#ifdef ASCiiKEY
				//all CH status
				Func_inputSelStatus(i);
#endif		
			} 			
		}else{
			//--------------------------------------------------
			// this CH set : INPUT_SEL_DIGITAL
			//--------------------------------------------------
			input_selMode[ampType] = INPUT_SEL_DIGITAL;
			eepromAddr = ADDR_AMP_A_INPUTSELMODE+ampType;
			nvr_xPutData(eepromAddr,input_selMode[ampType]);

#ifdef ASCiiKEY
			//this CH status
			Func_inputSelStatus(ampType);
#endif		
		}
	}
}
//
//--------------------------------------------------------------------
//unsigned char input_selMode[4];
//--------------------------------------------------------------------
void 	Func_inputSel_analog(unsigned char ampType)
{
	unsigned char i;

	//20150303
	if ( (RKB_ModelID == RKB_D850) || (RKB_ModelID == RKB_D8100) ) {
		if (ampType > 3) {
			//--------------------------------------------------
			//All CH set : INPUT_SEL_ANALOG
			//--------------------------------------------------
			for (i=0;i<4;i++) {
				input_selMode[i] = INPUT_SEL_ANALOG;
				eepromAddr = ADDR_AMP_A_INPUTSELMODE+i;
				nvr_xPutData(eepromAddr,input_selMode[i]);				
#ifdef ASCiiKEY
				//all CH status
				Func_inputSelStatus(i);
#endif		
			} 			
		}else{
			//--------------------------------------------------
			// this CH set : INPUT_SEL_ANALOG
			//--------------------------------------------------
			input_selMode[ampType] = INPUT_SEL_ANALOG;
			eepromAddr = ADDR_AMP_A_INPUTSELMODE+ampType;
			nvr_xPutData(eepromAddr,input_selMode[ampType]);

#ifdef ASCiiKEY
			//this CH status
			Func_inputSelStatus(ampType);
#endif		
		}
	}
}

/*	 */


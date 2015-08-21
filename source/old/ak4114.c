/**************** (c)2012 Rotel ************************************************
PROJECT         : RKB-850,8100,D850,D8100
COMPILER        : IAR Embeded Workbench 5.50
MODULE          : adckey.c
VERSION         : V 1.0
USED CHIP       : stm32f101x
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
#include "main.h"
#include "uart.h"
#include "timer.h"

#include "ak4114.h"
#include "ak4114.mem"

#include "adckey.h"
#include "proc_function.h"
#include "function.ext"

#define 	DIR_MULTI_ENABLE			//2013418 : enable DIR AK4117 *4ea
#define 	AK4117_ENABLE				//2013418 : enable AK4117

/*
#define I_COAX1			0x00		
#define I_ANALOG		0x06        //analog in , 0x07=GND
#define I_NULL			0x7f
********************************************************************************/
typedef const unsigned char CUNC_8;
//RA1570 : coax1(rx0), coax2(rx1), opt1(rx2), opt2(rx3), PC_USB(rx4), USB(rx5), analog(rx6,7=GND)
CUNC_8 TBL_4114_INPUTSEL[8]   		= { 0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07 };
CUNC_8 TBL_4114_OUTPUTSEL[8]  		= { 0x88,0x99,0xaa,0xbb,0xcc,0xdd,0xee,0xff };
//RKB- : (rx0=coax),(rx1=GND)
CUNC_8 TBL_4117_INPUTSEL_RKB[2]     = { 0x05,0x85 };

/* jyj_pop */
#define DIR_ANALOG		0
#define DIR_DIGITAL		1
/* jyj_neo6 */
#define Freq_Level0			0			//init
#define Freq_Level1			1			//48kHz
#define Freq_Level2			2			//96kHz
#define Freq_Level3			3			//192kHz
#define ReadChk_DelayTime	2			//jyj_stmnone2

//------------------------------------------------------------------------------
void S_4114_RESET(unsigned char i)
{
    if(i == P_HIGH){
        p_4114_reset(1);
    } else {
        p_4114_reset(0);
    }
}
//add 4117----------------------------------------------------------------------------
void S_4114_CS(unsigned char typ, unsigned char i)
{
	switch(typ){		//type : 0/1/2/3
#ifdef DIR_MULTI_ENABLE
        case 3:
            if(i == P_HIGH){
                p_4114_cs_D(1);
            } else {
                p_4114_cs_D(0);
            }
            break;
        case 2:
            if(i == P_HIGH){
                p_4114_cs_C(1);
            } else {
                p_4114_cs_C(0);
            }
            break;
        case 1:
            if(i == P_HIGH){
                p_4114_cs_B(1);
            } else {
                p_4114_cs_B(0);
            }
            break;
#endif
        case 0:
        default:
            if(i == P_HIGH){
#ifdef DIR_MULTI_ENABLE
                p_4114_cs_A(1);
#else                
                p_4114_cs(1);
#endif
            } else {
#ifdef DIR_MULTI_ENABLE
                p_4114_cs_A(0);
#else                
                p_4114_cs(0);
#endif
            }
            break;
    }
}
//******************************************************************************
void s_SmallWait(unsigned char s_data)
{
    unsigned short s_speeddata;
     
    s_speeddata = s_data * 20;
    do {
        --s_speeddata;
        //32M PLL
		__asm(" nop" );
		__asm(" nop" );
		__asm(" nop" );
    } while(s_speeddata > 0);
}
//******************************************************************************
//call power on
void DIR_4114_Reset(void)
{
    S_4114_RESET(P_LOW);		//jyj_reset : DAC dsd1791 공용 사용
	s_SmallWait(10+10+10);		//20131022
	S_4114_RESET(P_HIGH);		//jyj_reset : DAC dsd1791 공용 사용
}
//******************************************************************************
//add 4117 (사용?)
void s_CommPortInit(unsigned char typ)
{
	S_4114_RESET(P_LOW);      //chd : delete : 20130408
	S_4114_CS(typ,P_HIGH);
	p_4114_do(1);
	p_4114_clk(0);
}
//******************************************************************************
void s_Ak4114_Write_Byte(unsigned char t_data)
{
    unsigned char i;

    for(i=0;i<8;i++){
        p_4114_clk(0);
        s_SmallWait(2);	//jyj_Nec
        if((t_data & (0x80 >> i)) == 0){
            p_4114_do(0);
        } else {
            p_4114_do(1);
        }
        s_SmallWait(2);	//jyj_Nec
        p_4114_clk(1);
        s_SmallWait(4);	//jyj_Nec
    }
    p_4114_clk(0);
}
/*********************************************************************
    c1,c0,W/r,A4,A3,A2,A1,A0 -- D7,D6,D5,D4,D3,D2,D1,D0
    
    chipAddress : c1=c0=0
    Write(1), /read(0)
    Address : A4~A0
    data : D7~D0
    
    typ : 4EA AK4117

*********************************************************************/
//add 4117
void s_Ak4114MsgTrans(unsigned char typ)
{
//	rs232_printf("4114=>%x %x\n",a_Work[0],a_Work[1]);
	S_4114_CS(typ,P_LOW);
    s_Ak4114_Write_Byte(a_Work[0]);
    s_Ak4114_Write_Byte(a_Work[1]);
	S_4114_CS(typ,P_HIGH);
}

//******************************************************************************
//  in ::  typ : 4EA AK4117 , s_data
//  out a_Work[2]
//  test if(s_data == 0x07) a_Work[7] <= a_Work[2]
//******************************************************************************
//add 4117
void s_Ak4114MsgRead(unsigned char typ, unsigned char s_data)
{
    unsigned char i;

	S_4114_CS(typ,P_LOW);

    s_Ak4114_Write_Byte(s_data);

    a_Work[2] = 0;

    for(i=0;i<8;i++){
        p_4114_clk(1);
        s_SmallWait(2);
        if(Get4114di == 1){
            a_Work[2] = a_Work[2] | (0x01 << (7-i));
        }
        s_SmallWait(2);	//jyj_Nec
        p_4114_clk(0);
        s_SmallWait(4);	//jyj_Nec
    }
	S_4114_CS(typ,P_HIGH);

    //test chd 20100701
    //if (s_data == 0x06) a_Work[6] = a_Work[2];
    //if (s_data == 0x07) a_Work[7] = a_Work[2];
     	
}
//******************************************************************************
//0x22, 0x23 register setting
//add 4117-done
void DIR_InOutSelect(unsigned char typ)	//jyj_merge3
{
	if (b_4114Input[typ] == I_ANALOG) {
#ifdef AK4117_ENABLE
        //All Analog mode(GND)
        a_Work[0] = 0x22;
        a_Work[1] = TBL_4117_INPUTSEL_RKB[1];                               //(rx1/i2s,24bit) => GND=rx1
        s_Ak4114MsgTrans(typ);
#else
        //All Analog mode(GND)
        a_Work[0] = 0x23;
        a_Work[1] = TBL_4114_INPUTSEL[7];                                   //(rx7)
        s_Ak4114MsgTrans(typ);
//
        a_Work[0] = 0x22;
        a_Work[1] = TBL_4114_OUTPUTSEL[7];
        a_Work[1] = a_Work[1] & 0x77;	//Digi out disable
        s_Ak4114MsgTrans(typ);
#endif        
	} else {
#ifdef AK4117_ENABLE
        //Rx0
        a_Work[0] = 0x22;
        a_Work[1] = TBL_4117_INPUTSEL_RKB[0];                               //(rx0/i2s,24bit)
        s_Ak4114MsgTrans(typ);
#else
		//coax1(rx0)~USB(rx5)
		a_Work[0] = 0x23;
     	a_Work[1] = TBL_4114_INPUTSEL[b_4114Input[typ]];
     	s_Ak4114MsgTrans(typ);
//
     	a_Work[0] = 0x22;
     	a_Work[1] = TBL_4114_OUTPUTSEL[b_4114Input[typ]];
		a_Work[1] = a_Work[1] & 0x77;	//Digi out disable
     	s_Ak4114MsgTrans(typ);
#endif     	
	}
}
//******************************************************************************
//add 4117-done
void s_DeemphasisAuto(unsigned char typ)
{
    if (b_4114Input[typ] == I_ANALOG) {
#ifdef AK4117_ENABLE
		a_Work[0] = 0x23;       //INT0 Mask
        a_Work[1] = 0x3f;       //default
        s_Ak4114MsgTrans(typ);
        a_Work[0] = 0x22;       //Format
        a_Work[1] = 0x85;       //Rx1=GND, 24bit I2S		//20140611
        s_Ak4114MsgTrans(typ);
#else
        a_Work[0] = 0x24;       //INT0 Mask
        a_Work[1] = 0xee;       //default
        s_Ak4114MsgTrans(typ);
        a_Work[0] = 0x21;       //Format
        a_Work[1] = 0x7a;       /* 24bit I2S / LRCK=I */
        s_Ak4114MsgTrans(typ);
#endif     	
	}else{
#ifdef AK4117_ENABLE
        //opti, coax
        a_Work[0] = 0x23;       //INT0 Mask
        a_Work[1] = 0x1f;       //Unlck,,PAR, Auto
        s_Ak4114MsgTrans(typ);
        a_Work[0] = 0x22;       //Format
        a_Work[1] = 0x05;       //Rx0, 24bit I2S
        s_Ak4114MsgTrans(typ);
        //20130523
        a_Work[0] = 0x23;       //INT0 Mask
        a_Work[1] = 0x1f;       //Unlck,,PAR, Auto
        s_Ak4114MsgTrans(typ);
#else
        //opti, coax
        a_Work[0] = 0x24;		//INT0 Mask
        a_Work[1] = 0xa6;		//Auto,Unlck,DtsCd,PAR
        s_Ak4114MsgTrans(typ);
        a_Work[0] = 0x21;       //Format
        a_Work[1] = 0x5a;       /* 24bit I2S / LRCK=O*/
        s_Ak4114MsgTrans(typ);
        //20130523
        a_Work[0] = 0x24;       //INT0 Mask
        a_Work[1] = 0xa6;		//Auto,Unlck,DtsCd,PAR
        s_Ak4114MsgTrans(typ);
#endif     	
    }
}
//******************************************************************************
//add 4117-done
unsigned char s_FrequencyRead(unsigned char typ, unsigned char i_mode)
{
    unsigned char i_freq, i_status;

    //digital freq read
#ifdef AK4117_ENABLE
	if (RKB_mode_PP_MP == 0) {
		//				
		// WS1/2 : 20131129
		//
		// port PC10 : Low = AK4117 X'tal MODE [ External clock mode ]
		// port PC10 : High = AK4117 OFF MODE
		if ( Amp_nReadyPortC10Protection == 0 ) {   	//20131129 ???? delete X'tal ????
			//------------------------------
			// AK4117 X'tal mode [ External clock mode ]
			//------------------------------
			//address 06H:[ 0,DTSCD,NPCM,PEM,FS3/2/1/0 ]
			//------------------------------
			s_Ak4114MsgRead(typ, 0x06);              
			//0,DTSCD,NPCM,PEM,FS3/2/1/0
			a_Work[2] = ( a_Work[2] << 4 )&0xf0;
		}else{
			//20131129
			//------------------------------
			// AK4117 OFF mode ( delete X'tal )
			//------------------------------
			//address 0BH:[ 0,0,0,0,FS3/2/1/0 ]
			//------------------------------
			s_Ak4114MsgRead(typ, 0x0b);              
			//0,0,0,0,FS3/2/1/0
			a_Work[2] = ( a_Work[2] << 4 )&0xf0;
		}
	}else{
		//
		//PP/MP
		//
#ifdef AK4117_Xtal_ENABLE			//20140611	
		//------------------------------
		// AK4117 X'tal mode
		//------------------------------
		//address 06H:[ 0,DTSCD,NPCM,PEM,FS3/2/1/0 ]
		//------------------------------
		s_Ak4114MsgRead(typ, 0x06);              
		//0,DTSCD,NPCM,PEM,FS3/2/1/0
		a_Work[2] = ( a_Work[2] << 4 )&0xf0;
#else
		//20131128 : RKB PP : delete X'tal(OFF MODE) so XTL1:0="11"
		//( use channel status : consumer mode : address 0BH:Byte3.bit3.2.1.0 )           
		//xxxx 0000 : 44.1
		//xxxx 0010 : 48
		//xxxx 0011 : 32
		//xxxx 1000 : 88.2
		//xxxx 1010 : 96
		//xxxx 1100 : 176.4 => change to **** 0100
		//xxxx 1110 : 192   => change to **** 0110
		//------------------------------
		s_Ak4114MsgRead(typ, 0x0b);              
		//0,0,0,0,FS3/2/1/0
		a_Work[2] = ( a_Work[2] << 4 )&0xf0;
#endif	//AK4117_Xtal_ENABLE
	}
#else
    s_Ak4114MsgRead(typ, 0x07);              //test : if(s_data == 0x07) a_Work[7] <= a_Work[2]							
#endif
	//------------------------------------------------------
	//	check sampling frequency
	//------------------------------------------------------
    switch(a_Work[2] & 0xf0){						//FS3~0
        case 0x00:
            i_freq = ST_441;
            i_status = DIR_READ_OK;
            //rs232_printf("i_freq = 44.1 ");
             break;
        case 0x30:
            i_freq = ST_32;
            i_status = DIR_READ_OK;
            break;
        case 0x80:
            i_freq = ST_882;
            i_status = DIR_READ_OK;
            //rs232_printf("i_freq = 88.2 ");
            break;
        case 0xa0:
            i_freq = ST_96;
            i_status = DIR_READ_OK;
            //rs232_printf("i_freq = 96 ");
             break;
        case 0xc0:
            i_freq = ST_1764;
            i_status = DIR_READ_OK;
            //rs232_printf("i_freq = 176.4 ");
             break;
        case 0xe0:
            i_freq = ST_192;
            i_status = DIR_READ_OK;
            //rs232_printf("i_freq = 192 ");
            break;
        case 0x20:
            i_freq = ST_48;
            i_status = DIR_READ_OK;
            //rs232_printf("i_freq = 48 ");
            break;
        case 0x40:
        	//20131129 : AK4117 : OFF Mode
            i_freq = ST_1764;
            i_status = DIR_READ_OK;
            //rs232_printf("i_freq = 176.4 ");
             break;
        case 0x60:
        	//20131129 : AK4117 : OFF Mode
            i_freq = ST_192;
            i_status = DIR_READ_OK;
            //rs232_printf("i_freq = 192 ");
            break;
        default:
            i_freq = ST_48;
            i_status = DIR_READ_ERR;
            break;
    }
    if(i_mode == MODE_NORMAL){
        b_NorStreamFreq[typ] = i_freq;
#ifdef IC_DEBUG_x
        rs232_printf("Freq1 = %x ",typ, b_NorStreamFreq[typ]);		//jyj_merge3
#endif
    } else {
        b_StreamFreq[typ] = i_freq;
#ifdef IC_DEBUG_x
        rs232_printf("Freq2 = %x ",typ, b_StreamFreq[typ]);			//jyj_merge3
#endif
    }

     return(i_status);
}
//******************************************************************************
//analog 0x20(AK4117:0x21) register setting!
//add 4117-done
void s_AnalogClkModeTrans(unsigned char typ)		//jyj add
{
#ifdef AK4117_ENABLE
	if (RKB_mode_PP_MP == 0) {				//? WS1/2 : 20131128
		//
		// WS1/2 : 20131129
		//
		// port PC10 : Low = AK4117 X'tal MODE [ External clock mode ]
		// port PC10 : High = AK4117 OFF MODE
		if ( Amp_nReadyPortC10Protection == 0 ) {   	//20131129 ???? delete X'tal ????
			//------------------------------
			// AK4117 X'tal mode [ External clock mode ]
			//------------------------------
			//000 EXCK XTL1 XTL0 PWN RSTN           
			//Exck=1 : External clock mode
			//XTL[1:0]=10(24.576MHz)
			//------------------------------
			a_Work[0] = 0x20;
			//000 EXCK XTL1 XTL0 PWN RSTN           //Exck=1 : External clock mode
			a_Work[1] = 0x1b;						//XTL[1:0]=00(11.2896MHz)   //XTL[1:0]=01(12.288MHz)
			s_Ak4114MsgTrans(typ);                  //XTL[1:0]=10(24.576MHz)    //XTL[1:0]=11(use channel status)
			//------------------------------
			a_Work[0] = 0x21;
			//1010 10xx                             //CM[1:0]=11(DAUX)
			//1010 xx11 							//XCKS[1:0]
			a_Work[1] = 0xab;						//XCKS[1:0]=10(48K), 512fs
			s_Ak4114MsgTrans(typ);
            
			b_CurrDIRMode[typ] = DIR_ANALOG;		//jyj_pop    
		}else{
			//20131129
			//------------------------------
			// AK4117 OFF mode ( delete X'tal )
			//------------------------------
			//000 EXCK XTL1 XTL0 PWN RSTN           
			//Exck=1 : External clock mode
			//XTL[1:0]=11(use channel status)
			//------------------------------
			a_Work[0] = 0x20;
			//000 EXCK XTL1 XTL0 PWN RSTN           //Exck=1 : External clock mode
			a_Work[1] = 0x1f;						//XTL[1:0]=00(11.2896MHz)   //XTL[1:0]=01(12.288MHz)
			s_Ak4114MsgTrans(typ);                  //XTL[1:0]=10(24.576MHz)    //XTL[1:0]=11(use channel status)
			//------------------------------
			a_Work[0] = 0x21;
			//1010 10xx                             //CM[1:0]=11(DAUX)
			//1010 xx11 							//XCKS[1:0]
			a_Work[1] = 0xab;						//XCKS[1:0]=10(48K), 512fs
			s_Ak4114MsgTrans(typ);
            
			b_CurrDIRMode[typ] = DIR_ANALOG;		//jyj_pop     
		} 
	}else{
		//
		//PP/MP
		//
#ifdef AK4117_Xtal_ENABLE			//20140611	
		//------------------------------
		// AK4117 X'tal mode
		//------------------------------
		//000 EXCK XTL1 XTL0 PWN RSTN           
		//Exck=0 : X'tal mode
		//XTL[1:0]=10(24.576MHz)
		//------------------------------
		a_Work[0] = 0x20;
		//000 EXCK XTL1 XTL0 PWN RSTN           //Exck=0 : X'tal mode
		a_Work[1] = 0x0b;						//XTL[1:0]=00(11.2896MHz)   //XTL[1:0]=01(12.288MHz)
		s_Ak4114MsgTrans(typ);                  //XTL[1:0]=10(24.576MHz)    //XTL[1:0]=11(use channel status)
		//------------------------------
		a_Work[0] = 0x21;
		//1010 10xx                             //CM[1:0]=11(DAUX)
		//1010 xx11 							//XCKS[1:0]
		a_Work[1] = 0xab;						//XCKS[1:0]=10(48K), 512fs
		s_Ak4114MsgTrans(typ);
        
		b_CurrDIRMode[typ] = DIR_ANALOG;		//jyj_pop    
#else
		//------------------------------
		//20131128 : RKB PP : delete X'tal(OFF mode) so XTL1:0="11"
		//------------------------------
		//000 EXCK XTL1 XTL0 PWN RSTN           
		//Exck=1 : External clock mode
		//XTL[1:0]=11(use channel status)
		//------------------------------
		a_Work[0] = 0x20;
		//000 EXCK XTL1 XTL0 PWN RSTN           //Exck=1 : External clock mode
		a_Work[1] = 0x1f;						//XTL[1:0]=00(11.2896MHz)   //XTL[1:0]=01(12.288MHz)
		s_Ak4114MsgTrans(typ);                  //XTL[1:0]=10(24.576MHz)    //XTL[1:0]=11(use channel status)
		//------------------------------
		a_Work[0] = 0x21;
		//1010 10xx                             //CM[1:0]=11(DAUX)
		//1010 xx11 							//XCKS[1:0]
		a_Work[1] = 0xab;						//XCKS[1:0]=10(48K), 512fs
		s_Ak4114MsgTrans(typ);
        
		b_CurrDIRMode[typ] = DIR_ANALOG;		//jyj_pop     
#endif	//AK4117_Xtal_ENABLE
	}
#else
    a_Work[0] = 0x20;
	//01xx 0011 							//CM[1:0]=11(DAUX)
	//0100 xx11 							//OCKS[1:0]
	a_Work[1] = 0x7b;						//OCKS[1:0]=10(48K), 512fs
	s_Ak4114MsgTrans(typ);

	b_CurrDIRMode[typ] = DIR_ANALOG;		//jyj_pop
#endif
}
//******************************************************************************
//digital 0x20(AK4117:0x21) register setting!
//add 4117-done
void s_ClockModeTrans(unsigned char typ)
{     
#ifdef AK4117_ENABLE
	//------------------------------------------------------
	//	AK4117
	//------------------------------------------------------
	//	address : 0x20
	//------------------------------------------------------
	if (RKB_mode_PP_MP == 0) {				//? WS1/2 : 20131128
		//
		// WS1/2 : 20131129
		//
		// port PC10 : Low = AK4117 X'tal MODE [ External clock mode ]
		// port PC10 : High = AK4117 OFF MODE
		if ( Amp_nReadyPortC10Protection == 0 ) {   	//20131129 ???? delete X'tal ????
			//------------------------------
			// AK4117 X'tal mode [ External clock mode ]
			//------------------------------
			//000 EXCK XTL1 XTL0 PWN RSTN           
			//Exck=1 : External clock mode
			//XTL[1:0]=10(24.576MHz)
			//------------------------------
			a_Work[0] = 0x20;
			//000 EXCK XTL1 XTL0 PWN RSTN           //Exck=1 : External clock mode
			a_Work[1] = 0x1b;						//XTL[1:0]=00(11.2896MHz)   //XTL[1:0]=01(12.288MHz)
			s_Ak4114MsgTrans(typ);                  //XTL[1:0]=10(24.576MHz)    //XTL[1:0]=11(use channel status)     
		}else{
			//20131129
			//------------------------------
			// AK4117 OFF mode ( delete X'tal )
			//------------------------------
			//000 EXCK XTL1 XTL0 PWN RSTN           
			//Exck=1 : External clock mode
			//XTL[1:0]=11(use channel status)
			//------------------------------
			a_Work[0] = 0x20;
			//000 EXCK XTL1 XTL0 PWN RSTN           //Exck=1 : External clock mode
			a_Work[1] = 0x1f;						//XTL[1:0]=00(11.2896MHz)   //XTL[1:0]=01(12.288MHz)
			s_Ak4114MsgTrans(typ);                  //XTL[1:0]=10(24.576MHz)    //XTL[1:0]=11(use channel status)
		}
	}else{
		//
		//PP/MP
		//
#ifdef AK4117_Xtal_ENABLE			//20140611	
		//------------------------------
		// AK4117 X'tal mode
		//------------------------------
		//000 EXCK XTL1 XTL0 PWN RSTN           
		//Exck=0 : X'tal mode
		//XTL[1:0]=10(24.576MHz)
		//------------------------------
		a_Work[0] = 0x20;
		//000 EXCK XTL1 XTL0 PWN RSTN           //Exck=0 : X'tal mode
		a_Work[1] = 0x0b;						//XTL[1:0]=00(11.2896MHz)   //XTL[1:0]=01(12.288MHz)
		s_Ak4114MsgTrans(typ);                  //XTL[1:0]=10(24.576MHz)    //XTL[1:0]=11(use channel status)     
#else
		//------------------------------
		//20131128 : RKB PP : delete X'tal(OFF mode) so XTL1:0="11"
		//------------------------------
		//000 EXCK XTL1 XTL0 PWN RSTN           
		//Exck=1 : External clock mode
		//XTL[1:0]=11(use channel status)
		//------------------------------
		a_Work[0] = 0x20;
		//000 EXCK XTL1 XTL0 PWN RSTN           //Exck=1 : External clock mode
		a_Work[1] = 0x1f;						//XTL[1:0]=00(11.2896MHz)   //XTL[1:0]=01(12.288MHz)
		s_Ak4114MsgTrans(typ);                  //XTL[1:0]=10(24.576MHz)    //XTL[1:0]=11(use channel status)
#endif	//AK4117_Xtal_ENABLE
	}
	//------------------------------------------------------
	//	address : 0x21 (use PLL Mode)
	//
	//  LP = 0 : LowPower : Normal Mode
	//	PCKS[1:0] = 10	: 176.4 ~ 192K : MasterClockFreq at Pll Mode
	//	PCKS[1:0] = 01	: 88.2 ~ 96K
	//	PCKS[1:0] = 00	: 32 ~ 48K
	//
	//	XCKS[1:0] = 01(Default)	: MasterClockFreq at X'tal Mode
	//	CM[1:0] = 00(Default) : MasterClockOperationMode : RX
	//------------------------------------------------------
    a_Work[0] = 0x21;
	switch(b_StreamFreq[typ]){		//I_HDMII2S일때에도 SPDIF는 DIR의 Fs를 참조해야함.
        case ST_1764:
        case ST_192:
            //1010 01xx 				//CM[1:0]=00(RX)
            //0xx0 0100 				//PCKS[1:0]
            a_Work[1] = 0x44;			//PCKS[1:0]=10 (192K)
            break;
        case ST_96:
        case ST_882:
            a_Work[1] = 0x24;		    //PCKS[1:0]=01 (96K)
            break;
        case ST_48:
        case ST_441:
        case ST_32:
        default:
            a_Work[1] = 0x04;			//PCKS[1:0]=00 (48K)
            break;
    }
    s_Ak4114MsgTrans(typ);
    b_CurrDIRMode[typ] = DIR_DIGITAL;		//jyj_pop
	//------------------------------------------------------
#else
	//------------------------------------------------------
	//	AK4114
	//------------------------------------------------------
	//	address : 0x20
	//------------------------------------------------------
    a_Work[0] = 0x20;
	switch(b_StreamFreq[typ]){		//I_HDMII2S일때에도 SPDIF는 DIR의 Fs를 참조해야함.
        case ST_1764:
        case ST_192:
            //01xx 0011 				//CM[1:0]=00(RX)
            //0100 xx11 				//OCKS[1:0]
            a_Work[1] = 0x4f;			//OCKS[1:0]=11 (192K)
            break;
        case ST_96:
        case ST_882:
            a_Work[1] = 0x43;		    //OCKS[1:0]=00 (96K)
            break;
        case ST_48:
        case ST_441:
        case ST_32:
        default:
            a_Work[1] = 0x4b;			//OCKS[1:0]=10 (48K)
            break;
    }
    s_Ak4114MsgTrans(typ);
    b_CurrDIRMode[typ] = DIR_DIGITAL;		//jyj_pop
	//------------------------------------------------------
#endif
}
//******************************************************************************
//add 4117-done
void s_StreamKindChk(unsigned char typ)
{
#ifdef AK4117_ENABLE
    s_Ak4114MsgRead(typ, 0x0d);         //test : if(s_data == 0x07) a_Work[7] <= a_Work[2]	
#else
    s_Ak4114MsgRead(typ, 0x12);         //test : if(s_data == 0x07) a_Work[7] <= a_Work[2]	
#endif
    switch(a_Work[2] & 0x1f){								//PC4~0
        case 1:
            b_StreamKind[typ] = ST_A_AC3;
            b_AutoStreamKind[typ] = ST_A_AC3;	//jyj_merge4
            break;
        case 0:
        case 3:
            b_StreamKind[typ] = ST_A_OUTOFPCM;
            b_AutoStreamKind[typ] = ST_A_OUTOFPCM;	//jyj_merge4
            break;
        case 4:
        case 6:
        case 8:
        case 9:
            b_StreamKind[typ] = ST_A_MPEG;
            b_AutoStreamKind[typ] = ST_A_MPEG;	//jyj_merge4
             break;
        case 5:
            b_StreamKind[typ] = ST_A_MP3;
            b_AutoStreamKind[typ] = ST_A_MP3;	//jyj_merge4
            break;
        case 11:
        case 12:
        case 13:
            b_StreamKind[typ] = ST_A_DTS;
            b_AutoStreamKind[typ] = ST_A_DTS;	//jyj_merge4
            break;
        default:
            b_StreamKind[typ] = ST_A_NONA;
            b_AutoStreamKind[typ] = ST_A_NONA;	//jyj_merge4
            break;
    }
}
//******************************************************************************
//function (analog , digital)
//add 4117
void DIR_DspSetup(unsigned char typ)	//jyj_merge3
{

    s_DeemphasisAuto(typ);	//0x24, 0x21 register setting
	DIR_InOutSelect(typ);	//0x22, 0x23 register setting

		s_ClockModeTrans(typ);					//0x20 register setting!
		b_PllStatus[typ] = PLL_UNLOCK;			//20130708 : PLL_CHECKING;
		b_PllStatus_bk[typ] = PLL_CHECKING;		//20130708
		b_StreamKind[typ] = ST_CHECKING;		//UNLCK =>checking : chd 20130708			
		b_PllCheckTime[typ] = TIME10MSTO40MS;
		//f_PllCheckOn=1;						//bit5
		b_ModeStatus[typ] |=0x20;
}
//******************************************************************************
//10msec call => (timer 40msec)
//add 4117-done
void s_PllLockCheck(unsigned char typ)
{

	//if(f_PllCheckOn == 0) return;			//?bit5
	if(b_ModeStatus[typ]&0x20 == 0) return;			//?digital / analog
	if(b_PllCheckTime[typ] != 0) return;			//?40msec

	if (b_4114Input[typ] == I_ANALOG) {
		//Analog mode
		b_PllStatus[typ] = PLL_LOCK;
		b_PllCheckTime[typ] = TIME10MSTO40MS;
		return;
	}
    //------------------------------------------------------
	//digital ( opt1 & coax1 )
	if ( (typ == 0) && (Get4114INT0_A !=0) ) goto goto_exec_int0_high;
	if ( (typ == 1) && (Get4114INT0_B !=0) ) goto goto_exec_int0_high;
	if ( (typ == 2) && (Get4114INT0_C !=0) ) goto goto_exec_int0_high;
	if ( (typ == 3) && (Get4114INT0_D !=0) ) {
	//if(Get4114INT0 != 0){                   
goto_exec_int0_high:
	    //occur int0 = High
	    //status0 read
#ifdef AK4117_ENABLE
		s_Ak4114MsgRead(typ, 0x05);             //reg5=>(UNLCK, PAR, AUTO, V, AUDION, STC, CINT, QINT)
		//test chd 20100702                     //reg6=>(0, DTSCD, NPCM, PEM, FS3/2/1/0 )
		a_Work[6] = a_Work[2];
#else
		s_Ak4114MsgRead(typ, 0x06);             //reg6=>(QINT, AUTO, CINT, UNLCK, DTSCD, PEM, AUDION, PAR)
		//test chd 20100702
		a_Work[6] = a_Work[2];
#endif

#ifdef IC_DEBUG_x
	rs232_printf("4117(0x05)=>%d,%x\n",typ,a_Work[2]);
#endif
#ifdef AK4117_ENABLE
		if((a_Work[2] & 0x80) != 0)				//? UNLCK=1
#else
		if((a_Work[2] & 0x10) != 0)				//? UNLCK=1
#endif
        {
			//test chd 20100701
			//Mute_AllMuteOn();
			//Mute_SetMuteOffTime(COUNT100M_800MS);
			b_StreamKind[typ] = ST_CHECKING;			//UNLCK =>checking : chd 20090807	
			//				
			switch(b_PllStatus[typ]){
				case PLL_LOCK:
					b_PllStatus[typ] = PLL_CHECKING;
	                //status0 read
#ifdef AK4117_ENABLE
		            s_Ak4114MsgRead(typ, 0x05);             //reg5=>(UNLCK, PAR, AUTO, V, AUDION, STC, CINT, QINT)
#else
					s_Ak4114MsgRead(typ, 0x06);                  //test : if(s_data == 0x07) a_Work[7] <= a_Work[2]	
#endif
					b_PllCheckTime[typ] = TIME10MSTO40MS;
					break;
				case PLL_CHECKING:
					b_PllStatus[typ] = PLL_UNLOCK;
	                //status0 read
#ifdef AK4117_ENABLE
		            s_Ak4114MsgRead(typ, 0x05);             //reg5=>(UNLCK, PAR, AUTO, V, AUDION, STC, CINT, QINT)
#else
					s_Ak4114MsgRead(typ, 0x06);                  //test : if(s_data == 0x07) a_Work[7] <= a_Work[2]	
#endif
					b_PllCheckTime[typ] = TIME10MSTO40MS;
					break;
				case PLL_UNLOCK:
	                //status0 read
#ifdef AK4117_ENABLE
		            s_Ak4114MsgRead(typ, 0x05);             //reg5=>(UNLCK, PAR, AUTO, V, AUDION, STC, CINT, QINT)
#else
					s_Ak4114MsgRead(typ, 0x06);                  //test : if(s_data == 0x07) a_Work[7] <= a_Work[2]	
#endif
					b_PllCheckTime[typ] = TIME10MSTO40MS;
					break;
			}
		}
	} else {
goto_exec_int0_low:
		b_PllStatus[typ] = PLL_LOCK;
		b_PllCheckTime[typ] = TIME10MSTO40MS;
	}
}
//******************************************************************************
//main call
//add 4117
void Chk_DIRFreq(unsigned char typ)	//jyj_inchg
{
	unsigned char Temp_AutoFreq;

	//I_ANALOG,I_HDMII2S,PLL_UNLOCK 모두 아닌 경우
	Temp_AutoFreq = s_FrequencyRead(typ, MODE_NORMAL);   //read 0x07
	if (Temp_AutoFreq == DIR_READ_OK) {
    	if (b_StreamFreq[typ] != b_NorStreamFreq[typ]) {	//pcm이 아닌 경우 b_StreamFreq사용
    		b_StreamFreq[typ] = b_NorStreamFreq[typ];
    		s_ClockModeTrans(typ);
    	}
    	
    }
}
//******************************************************************************
//		//b_ModeStatus[i].byte_bit.bit1;		
//		b_ModeStatus[i] = 0x02;				//f_InputChg=1 (bit1)
//add 4117
void s_AssignDigitalCheck(unsigned char typ)
{
	if(b_4114Status[typ] != INIT_OK) return;

	//if(f_InputChg == 1){					//? bit1
	if( (b_ModeStatus[typ]&0x02) == 0x02){
		DIR_InOutSelect(typ);	//0x22, 0x23 register setting
		//f_InputChg = 0;		//bit1
		b_ModeStatus[typ] &=0xfd;
	}
}
//******************************************************************************
//call(10msec) (s_4114_Exec > AK4114_MONITOR)
//add 4117-done
void s_StreamCheck(unsigned char typ) 
{
    s_Ak4114MsgRead(typ, 0x06);             //AK4114:reg6=>(QINT, AUTO, CINT, UNLCK, DTSCD, PEM, AUDION, PAR)
    //test chd 20100702                     //AK4117:reg6=>(0, DTSCD, NPCM, PEM, FS3/2/1/0 )
    a_Work[6] = a_Work[2];                  //AK4117:reg5=>(UNLCK, PAR, AUTO, V, AUDION, STC, CINT, QINT)

#ifdef IC_DEBUG_x
	rs232_printf("[0x06][stream]=>%x %x\n",a_Work[2],b_StreamKind[typ]);
#endif

#ifdef AK4117_ENABLE
	if((a_Work[2] & 0x20) != 0)				//?NPCM
#else
	if((a_Work[2] & 0x40) != 0)				//?Auto=1(<= NonPCM)
#endif
    {
        //Non-PCM AUTO Detect!!!!    		
		//Mute_AllMuteOn();
		//Mute_SetMuteOffTime(COUNT100M_800MS);

//20090806
#ifdef AK4117_ENABLE
	    if((a_Work[2] & 0x40) == 0)            //?DTSCD
#else
		if((a_Work[2] & 0x08) == 0)            //?DTSCD
#endif
        {
		    //DTS-CD no detect!!!
		    //
		    //read DIR_stream (0x12)
			s_StreamKindChk(typ);              //<<read 0x12>> , a_Work[2] & 0x1f , [PC4/3/2/1/0]
			switch(b_StreamKind[typ]){
				case ST_A_DTS:
					//b_AutoMode = M_AUTODTS;
					b_StreamKind[typ] = ST_A_DTS;
					break;
				case ST_A_MP3:
					//b_AutoMode = M_AUTOMP3;
					break;
				case ST_A_MPEG:
					//b_AutoMode = M_AUTOMPEG;
					break;
				case ST_A_OUTOFPCM:
					//b_SubmodeTimer = T100M;
					return;
				case ST_A_AC3:
				default:
					//b_AutoMode = M_AUTODOLBY;
					break;
			}
		} else {
		    //DTS-CD detect!!!!
			//b_AutoMode = M_AUTODTS;
			b_StreamKind[typ] = ST_A_DTS;
			b_AutoStreamKind[typ] = ST_A_DTS;		//jyj_merge4
		}
	} else {
	    //PCM!!!!
#ifdef AK4117_ENABLE
	    if((a_Work[2] & 0x40) == 0)         //?DTSCD
#else
		if((a_Work[2] & 0x08) == 0)			//?noDetect ?DTSCD
#endif
        {	    
		    //DTS-CD no detect!!!
			//b_AutoMode = M_AUTOHDCD;
			b_StreamKind[typ] = ST_A_PCM;			//chd
			//b_AutoStreamKind[typ] = ST_A_HDCD;		//jyj_merge4
		} else {
		    //DTS-CD detect!!!!
			//b_AutoMode = M_AUTODTS;
			b_StreamKind[typ] = ST_A_DTS;
			b_AutoStreamKind[typ] = ST_A_DTS;		//jyj_merge4
		}
	}
}
//******************************************************************************
//add 4117
void s_DspTimeSet(unsigned char typ) 
{
	if (b_PllCheckTime[typ] > 0) --b_PllCheckTime[typ];
}
//******************************************************************************
//add 4117
void s_NextModeSet(unsigned char typ) 
{
	++b_4114Mode[typ];
}
/*
call main 10msec

// STREAM STATUS
#define PLL_LOCK              0
#define PLL_UNLOCK            1
#define PLL_CHECKING          2

//SYSTEM MODE
#define AK4114_INIT			0x00
#define AK4114_FREQSET		0x01
#define AK4114_MUTECHK		0x02
#define AK4114_MONITOR		0x03

#define INIT_OK			0xf0

typ : 4EA AK4117
**********************************************************************/
//add 4117
void s_4114_Exec(unsigned char typ)
{
	
    if (mode_PowerStatus >= POWER_ON2_ING) {			//20140108

		s_DspTimeSet(typ);		    //--timer  (40msec) 
		s_AssignDigitalCheck(typ);  //?f_InputChg==1 이면 DIR_DspSetup(typ) call
		
		switch(b_4114Mode[typ]){
			case AK4114_INIT:
				//ak4114 reset 및 init //결국 사용안함......
				s_CommPortInit(typ);
				s_NextModeSet(typ);
				break;
			case AK4114_FREQSET:
			    //AK4114 결국 여기서부터 시작함니다.....
				DIR_DspSetup(typ);		//func, 0x22,0x23 setting
				s_NextModeSet(typ);
				break;
			case AK4114_MUTECHK:
				s_NextModeSet(typ);
				break;
			case AK4114_MONITOR:
				if(b_4114Status[typ] != INIT_OK){
					b_4114Status[typ] = INIT_OK;		//INIT_OK = 0xF0
				}
				if (b_4114Input[typ] == I_COAX1) 	//20130708
				{
					if(b_PllStatus[typ] != PLL_UNLOCK) {		//Lock(0),Unlock(1),Checking(2)	//20130708 ( != PLL_UNLOCK )
						Chk_DIRFreq(typ);					//read => 0x27 => freq read => clock setting
						s_StreamCheck(typ);				//read => 0x26 => noPCM => mute
					}
				}
				break;
            default:
				break;
		}
        //(40msec, call)	
		s_PllLockCheck(typ);	//int0 =>(High) => read = 0x26  : 40msec
		//	Chk_StreamChg();	//stream이 chg인지 check한 뒤 필요한 것들 실행.
		//	StreamInfoBackup();	//stream backup
	}
}

/**/

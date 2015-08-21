/**************** (c)2012 Rotel Inc..*******************************************
PROJECT         : RA1570 : 2ch AMP, 2x20 VFD, RS232, Telechips, RotelLink, (AirPlay)
COMPILER        : IAR Embeded Workbench 5.50
MODULE          : RA_portdef.h
VERSION         : V 1.0
USED CHIP       : RA1570(stm32f101VC) [V=100pin, C=256KB/32KB]
CREATION DATE   : /2012
AUTHORS         : /Rotel
DESCRIPTION     :
*******************************************************************************/
#ifndef __PORT_DEF__
#define __PORT_DEF__

#define PORTE_PORT              GPIOE
#define Edummy002               GPIO_Pin_2          //1
#define Edummy003               GPIO_Pin_3          //2

//for project option port : internal pull-up!
#define PROJ_PORT               GPIOE
#define PROJ_RA_RC              GPIO_Pin_4          //3,RA(high),RC(low)    //not used : 20121221

//for Speaker A/B port
#define SPEAKER_PORT            GPIOE
#define SPEAKER_a               GPIO_Pin_5          //4
#define SPEAKER_b               GPIO_Pin_6          //5

//6(1),VBAT

//for power-down
#define POWERDOWN_PORT          GPIOC
#define POWER_DOWN              GPIO_Pin_13         //7(2)  

//for AMP protection
#define PROTECTION_PORT         GPIOC
#define AMP_PROTECT             GPIO_Pin_14         //8(3)

//for rmc in port
#define RMC_PORT                GPIOC
#define p_Rmc_Ext               GPIO_Pin_15         //9(4)

//10,VSS-5
//11,VDD-5

//12(5),OSC-in(4MHz)
//13(6),OSC-out
//14(7),NRST

//for ADC port
#define ADC_PORT                GPIOC
#define ADC_IN0                 GPIO_Pin_0          //15(8) //ADC10
#define ADC_IN1                 GPIO_Pin_1          //16(9) //ADC11
#define ADC_IN2                 GPIO_Pin_2          //17(10)//ADC12
//#define ADC_IN3               GPIO_Pin_3          //18(11)

//19(12),VSSA
//20,VREF-
//21,VREF+
//22(13),VDDA

//for front LED
#define GPIO_LED                GPIOA    
#define STANDBY_LED             GPIO_Pin_0          //23(14)

//for front Micom Reset
#define FRONT_RMC               GPIOA    
#define p_Rmc                   GPIO_Pin_1          //24(15)

//for UARTx (UART2->Telechips)
//#define GPIOx                 GPIOA
//#define RCC_APB2Periph_GPIOx  RCC_APB2Periph_GPIOA
#define GPIO_Tx2Pin             GPIO_Pin_2          //25(16)
#define GPIO_Rx2Pin             GPIO_Pin_3          //26(17)
    
//27(18),VAA-4
//28(19),VDD-4

//for vfd port
#define VFD_PORT                GPIOA
#define VFD_CLOCK               GPIO_Pin_4          //29(20)
#define VFD_DATA                GPIO_Pin_5          //30(21)    
#define VFD_LAT                 GPIO_Pin_6          //31(22)
#define VFD_BK                  GPIO_Pin_7          //32(23) : Blanking(1), DisplayOn(0)

//for encoder port
#define ENC_PORT                GPIOC
#define ENC_0                   GPIO_Pin_4          //33(24)
#define ENC_1                   GPIO_Pin_5          //34(25)

//for CM6631 XRST reset port
#define CM6631_PORT             GPIOB
#define CM6631_RESET            GPIO_Pin_0          //35(26) : CM6631A reset [ High -> Low ]

//for AMP power on(H)/off(L) port
#define AMP_PORT                GPIOB
#define AMP_POWER               GPIO_Pin_1          //36(27) : High Active

//37(28),BOOT1=Low(GPIOB,GPIO_Pin_2)

//for volume IC
#define PGA2310_PORT            GPIOE
#define PGA2310_MUTE            GPIO_Pin_7          //38
#define PGA2310_CS              GPIO_Pin_8          //39
#define PGA2310_DA              GPIO_Pin_9          //40
#define PGA2310_CK              GPIO_Pin_10         //41
//for tone IC
#define NJW1194_PORT            GPIOE
#define NJW1194_LH              GPIO_Pin_11         //42
#define NJW1194_DA              GPIO_Pin_12         //43
#define NJW1194_CK              GPIO_Pin_13         //44
#define NJW1194_BYPASS          GPIO_Pin_14         //45

#define Edummy015               GPIO_Pin_15         //46
//#define nHP_MUTE              GPIO_Pin_14         //45
//#define nHP_inCHECK           GPIO_Pin_15         //46 : input

//for UARTx (UART3->notUsed)
//#define GPIOx                 GPIOB
//#define RCC_APB2Periph_GPIOx  RCC_APB2Periph_GPIOB
//#define GPIO_Tx3Pin             GPIO_Pin_10         //47(29)
//#define GPIO_Rx3Pin             GPIO_Pin_11         //48(30)

//49(31),VSS-1
//50(32),VDD-1

//for iPod port
#define iPOD_PORT               GPIOB
#define iPOD_RESET              GPIO_Pin_12         //51(33),(Low active)

//for source select portb
#define SOURCE_RELAY_PORTB      GPIOB
#define RELAY_PHONO             GPIO_Pin_13         //52(34)
#define RELAY_CD                GPIO_Pin_14         //53(35)
#define RELAY_BALANCED          GPIO_Pin_15         //54(36)

//for source select portd
#define SOURCE_RELAY_PORTD      GPIOD
#define RELAY_TUNER             GPIO_Pin_8          //55
#define RELAY_AUX1              GPIO_Pin_9          //56
#define RELAY_AUX2              GPIO_Pin_10         //57
#define RELAY_DAC               GPIO_Pin_11         //58

//for 4114 switching port
#define AK4114_PORTD            GPIOD
#define p_4114_Int1             GPIO_Pin_12         //59
#define p_4114_Int0             GPIO_Pin_13         //60
#define p_4114_CS               GPIO_Pin_14         //61
#define p_4114_CLK              GPIO_Pin_15         //62

//for 4114 switching port
#define AK4114_PORTC            GPIOC
#define p_4114_DO               GPIO_Pin_6          //63(37)
#define p_4114_DI               GPIO_Pin_7          //64(38)
#define p_4114_RESET            GPIO_Pin_8          //65(39)

//for DAC port
#define DAC_PORTC               GPIOC
#define WM8740_RESET            GPIO_Pin_9          //66(40)
//for DAC port
#define DAC_PORTA               GPIOA
#define WM8740_ML               GPIO_Pin_8          //67(41)

//for UARTx (UART1)
//#define GPIOx                 GPIOA
//#define RCC_APB2Periph_GPIOx  RCC_APB2Periph_GPIOA
#define GPIO_Tx1Pin             GPIO_Pin_9          //68(42)
#define GPIO_Rx1Pin             GPIO_Pin_10         //69(43)

//for main mute port(HighActive)
#define MAIN_PORT               GPIOA
#define MAIN_MUTE               GPIO_Pin_11         //70(44)
#define MAIN_STBY               GPIO_Pin_12         //71(45)

//72(46),JTMS(GPIOA,GPIO_Pin_13)
//73 NC
//74(47),VSS-2
//75(48),VDD-2

//76(49),JTCK(GPIOA,GPIO_Pin_14)
//77(50),JTDI(GPIOA,GPIO_Pin_15)

//for UARTx (UART4 ->RotelLink)
//#define GPIOx                 GPIOC
//#define RCC_APB2Periph_GPIOx  RCC_APB2Periph_GPIOC
#define GPIO_Tx4Pin             GPIO_Pin_10         //78(51)
#define GPIO_Rx4Pin             GPIO_Pin_11         //79(52)

#define Cdummy012               GPIO_Pin_12         //80(53)

//
#define PORTD_PORT              GPIOD
#define Ddummy000               GPIO_Pin_0          //81
#define Ddummy001               GPIO_Pin_1          //82
#define Ddummy002               GPIO_Pin_2          //83(54)
//for CMedia ( i2c, int )
#define CMEDIA_PORT             GPIOD
#define CMEDIA_i2cSUSPEND       GPIO_Pin_3          //84 : input [ High:SuspendMode , Low:ConfigudMode ] : 20130111
#define CMEDIA_USB_CLASS        GPIO_Pin_4          //85 : H:1.1
#define CMEDIA_I2C_CLK          GPIO_Pin_5          //86
#define CMEDIA_I2C_DATA         GPIO_Pin_6          //87
#define CMEDIA_INT              GPIO_Pin_7          //88 : input

//89(55),JTDO(GPIOB,GPIO_Pin_3)
//90(56),JNTRST(GPIOB,GPIO_Pin_4)

#define PORTB_PORT              GPIOB
#define Bdummy005               GPIO_Pin_5          //91(57)

//for eeprom port
#define IIC_PORT                GPIOB
#define IIC_SCL                 GPIO_Pin_6          //92(58)
#define IIC_SDA                 GPIO_Pin_7          //93(59)
    
//94(60),BOOT0( Low => High ) : upgrade start!!
//
#define PORTB_PORT              GPIOB
#define Bdummy008               GPIO_Pin_8          //95(61)
#define Bdummy009               GPIO_Pin_9          //96(62)

//
#define PORTE_PORT              GPIOE
#define Edummy000               GPIO_Pin_0          //97
#define Edummy001               GPIO_Pin_1          //98

//99(63),VSS-3
//100(64),VDD-3

    
/****************************************************************************************/
//for printf
#define  PRINTx                 USART1

/****************************************************************************************/
/* macro definition */

//3 option RC
#define GetOptionRC             GPIO_ReadInputDataBit(PROJ_PORT, PROJ_RA_RC)

//4,5 speaker A/B : LowActive => test 20120926
//#define Speaker_A(x)            ( (x==1)? GPIO_ResetBits(SPEAKER_PORT, SPEAKER_a) : GPIO_SetBits(SPEAKER_PORT, SPEAKER_a) );
//#define Speaker_B(x)            ( (x==1)? GPIO_ResetBits(SPEAKER_PORT, SPEAKER_b) : GPIO_SetBits(SPEAKER_PORT, SPEAKER_b) );
//4,5 speaker A/B : HighActive
#define Speaker_A(x)            ( (x==1)? GPIO_SetBits(SPEAKER_PORT, SPEAKER_a) : GPIO_ResetBits(SPEAKER_PORT, SPEAKER_a) );
#define Speaker_B(x)            ( (x==1)? GPIO_SetBits(SPEAKER_PORT, SPEAKER_b) : GPIO_ResetBits(SPEAKER_PORT, SPEAKER_b) );

//7(2)power down
#define GetPowerDown            GPIO_ReadInputDataBit(POWERDOWN_PORT, POWER_DOWN)

//8(3)amp protection
#define Amp_protection          GPIO_ReadInputDataBit(PROTECTION_PORT, AMP_PROTECT)

//9(4)ext rmc
#define p_rmc_ext               GPIO_ReadInputDataBit(RMC_PORT, p_Rmc_Ext)

//23(14)LED : highActive
#define StandbyLED(x)           ( (x==1)? GPIO_SetBits(GPIO_LED, STANDBY_LED) : GPIO_ResetBits(GPIO_LED, STANDBY_LED) );
//24(15)rmc
#define p_rmc                   GPIO_ReadInputDataBit(FRONT_RMC, p_Rmc)
//29~32(20~23)VFD
#define VFD_Clock(x)            ( (x==1)? GPIO_SetBits(VFD_PORT, VFD_CLOCK) : GPIO_ResetBits(VFD_PORT, VFD_CLOCK) );
#define VFD_Data(x)             ( (x==1)? GPIO_SetBits(VFD_PORT, VFD_DATA) : GPIO_ResetBits(VFD_PORT, VFD_DATA) );
#define VFD_Latch(x)            ( (x==1)? GPIO_SetBits(VFD_PORT, VFD_LAT) : GPIO_ResetBits(VFD_PORT, VFD_LAT) );
#define VFD_Bk(x)               ( (x==1)? GPIO_SetBits(VFD_PORT, VFD_BK) : GPIO_ResetBits(VFD_PORT, VFD_BK) );

//35(26) CM6631 XRST reset control : High->Low
#define CM6631_reset(x)         ( (x==1)? GPIO_SetBits(CM6631_PORT, CM6631_RESET) : GPIO_ResetBits(CM6631_PORT, CM6631_RESET) );

//36(27) AMP Power On : HighActive : now not use
#define AMP_PowerOn(x)          ( (x==1)? GPIO_SetBits(AMP_PORT, AMP_POWER) : GPIO_ResetBits(AMP_PORT, AMP_POWER) );

//38~41 PGA2310 volume
#define PGA2310_mute(x)         ( (x==1)? GPIO_SetBits(PGA2310_PORT, PGA2310_MUTE) : GPIO_ResetBits(PGA2310_PORT, PGA2310_MUTE) );
#define PGA2310_cs(x)           ( (x==1)? GPIO_SetBits(PGA2310_PORT, PGA2310_CS) : GPIO_ResetBits(PGA2310_PORT, PGA2310_CS) );
#define PGA2310_data(x)         ( (x==1)? GPIO_SetBits(PGA2310_PORT, PGA2310_DA) : GPIO_ResetBits(PGA2310_PORT, PGA2310_DA) );
#define PGA2310_clock(x)        ( (x==1)? GPIO_SetBits(PGA2310_PORT, PGA2310_CK) : GPIO_ResetBits(PGA2310_PORT, PGA2310_CK) );

//42~45 NJW1194 tone
#define NJW1194_lh(x)           ( (x==1)? GPIO_SetBits(NJW1194_PORT, NJW1194_LH) : GPIO_ResetBits(NJW1194_PORT, NJW1194_LH) );
#define NJW1194_data(x)         ( (x==1)? GPIO_SetBits(NJW1194_PORT, NJW1194_DA) : GPIO_ResetBits(NJW1194_PORT, NJW1194_DA) );
#define NJW1194_clock(x)        ( (x==1)? GPIO_SetBits(NJW1194_PORT, NJW1194_CK) : GPIO_ResetBits(NJW1194_PORT, NJW1194_CK) );
//#define NJW1194_bypass(x)       ( (x==1)? GPIO_SetBits(NJW1194_PORT, NJW1194_BYPASS) : GPIO_ResetBits(NJW1194_PORT, NJW1194_BYPASS) );
//bypassOn=port_Low ,bypassOff=port_High
#define NJW1194_bypass(x)       ( (x==1)? GPIO_ResetBits(NJW1194_PORT, NJW1194_BYPASS) : GPIO_SetBits(NJW1194_PORT, NJW1194_BYPASS) );

//51(33)iPod  Telechips reset
#define iPod_nRESET(x)          ( (x==1)? GPIO_SetBits(iPOD_PORT, iPOD_RESET) : GPIO_ResetBits(iPOD_PORT, iPOD_RESET) );

//52~54(34~36)
#define RELAY_phono(x)          ( (x==1)? GPIO_SetBits(SOURCE_RELAY_PORTB, RELAY_PHONO) : GPIO_ResetBits(SOURCE_RELAY_PORTB, RELAY_PHONO) );
#define RELAY_cd(x)             ( (x==1)? GPIO_SetBits(SOURCE_RELAY_PORTB, RELAY_CD) : GPIO_ResetBits(SOURCE_RELAY_PORTB, RELAY_CD) );
#define RELAY_balXLR(x)         ( (x==1)? GPIO_SetBits(SOURCE_RELAY_PORTB, RELAY_BALANCED) : GPIO_ResetBits(SOURCE_RELAY_PORTB, RELAY_BALANCED) );
//55~58
#define RELAY_tuner(x)          ( (x==1)? GPIO_SetBits(SOURCE_RELAY_PORTD, RELAY_TUNER) : GPIO_ResetBits(SOURCE_RELAY_PORTD, RELAY_TUNER) );
#define RELAY_aux1(x)           ( (x==1)? GPIO_SetBits(SOURCE_RELAY_PORTD, RELAY_AUX1) : GPIO_ResetBits(SOURCE_RELAY_PORTD, RELAY_AUX1) );
#define RELAY_aux2(x)           ( (x==1)? GPIO_SetBits(SOURCE_RELAY_PORTD, RELAY_AUX2) : GPIO_ResetBits(SOURCE_RELAY_PORTD, RELAY_AUX2) );
#define RELAY_dac(x)            ( (x==1)? GPIO_SetBits(SOURCE_RELAY_PORTD, RELAY_DAC) : GPIO_ResetBits(SOURCE_RELAY_PORTD, RELAY_DAC) );

//59~62 AK4114  
#define Get4114INT1             GPIO_ReadInputDataBit(AK4114_PORTD, p_4114_Int1)
#define Get4114INT0             GPIO_ReadInputDataBit(AK4114_PORTD, p_4114_Int0)
#define p_4114_cs(x)            ( (x==1)? GPIO_SetBits(AK4114_PORTD, p_4114_CS) : GPIO_ResetBits(AK4114_PORTD, p_4114_CS) );
#define p_4114_clk(x)           ( (x==1)? GPIO_SetBits(AK4114_PORTD, p_4114_CLK) : GPIO_ResetBits(AK4114_PORTD, p_4114_CLK) );

//63~65(37~39)AK4114
#define p_4114_do(x)            ( (x==1)? GPIO_SetBits(AK4114_PORTC, p_4114_DO) : GPIO_ResetBits(AK4114_PORTC, p_4114_DO) );
#define Get4114di               GPIO_ReadInputDataBit(AK4114_PORTC, p_4114_DI)
#define p_4114_reset(x)         ( (x==1)? GPIO_SetBits(AK4114_PORTC, p_4114_RESET) : GPIO_ResetBits(AK4114_PORTC, p_4114_RESET) );

//66(40),67(41)WM8740
#define WM8740_nReset(x)        ( (x==1)? GPIO_SetBits(DAC_PORTC, WM8740_RESET) : GPIO_ResetBits(DAC_PORTC, WM8740_RESET) );
#define WM8740_nML(x)           ( (x==1)? GPIO_SetBits(DAC_PORTA, WM8740_ML) : GPIO_ResetBits(DAC_PORTA, WM8740_ML) );
#define WM8740_MD(x)            ( (x==1)? GPIO_SetBits(AK4114_PORTC, p_4114_DO) : GPIO_ResetBits(AK4114_PORTC, p_4114_DO) );
#define WM8740_MC(x)            ( (x==1)? GPIO_SetBits(AK4114_PORTD, p_4114_CLK) : GPIO_ResetBits(AK4114_PORTD, p_4114_CLK) );
//70(44)main mute (HighActive) => test 20121010
//#define Main_nMute(x)            ( (x==1)? GPIO_ResetBits(MAIN_PORT, MAIN_MUTE) : GPIO_SetBits(MAIN_PORT, MAIN_MUTE) );
//70(44)main mute (LowActive)
#define Main_nMute(x)           ( (x==1)? GPIO_SetBits(MAIN_PORT, MAIN_MUTE) : GPIO_ResetBits(MAIN_PORT, MAIN_MUTE) );
//71(45)standby relay
#define Standby(x)              ( (x==1)? GPIO_SetBits(MAIN_PORT, MAIN_STBY) : GPIO_ResetBits(MAIN_PORT, MAIN_STBY) );
//84 USB SUSPEND read : 20130111 => High : i2c suspend
#define GetCmI2cSuspend         GPIO_ReadInputDataBit(CMEDIA_PORT, CMEDIA_i2cSUSPEND)
//85 USB CLASS select : 20130108
#define CM_UsbClass(x)          ( (x==1)? GPIO_SetBits(CMEDIA_PORT, CMEDIA_USB_CLASS) : GPIO_ResetBits(CMEDIA_PORT, CMEDIA_USB_CLASS) );
//86~87 CMedia i2c : 20121029
#define CM_i2c_scl(x)           ( (x==1)? GPIO_SetBits(CMEDIA_PORT, CMEDIA_I2C_CLK) : GPIO_ResetBits(CMEDIA_PORT, CMEDIA_I2C_CLK) );
#define CM_i2c_sda(x)           ( (x==1)? GPIO_SetBits(CMEDIA_PORT, CMEDIA_I2C_DATA) : GPIO_ResetBits(CMEDIA_PORT, CMEDIA_I2C_DATA) );
#define GetCmediaInt            GPIO_ReadInputDataBit(CMEDIA_PORT, CMEDIA_INT)
//92~93(58~59)eeprom
#define p_i2c_sda(x)            ( (x==1)? GPIO_SetBits(IIC_PORT, IIC_SDA) : GPIO_ResetBits(IIC_PORT, IIC_SDA) );
#define p_i2c_scl(x)            ( (x==1)? GPIO_SetBits(IIC_PORT, IIC_SCL) : GPIO_ResetBits(IIC_PORT, IIC_SCL) );
//----------------------------------------------------------    
//readInput : 16Bits
//#define GetInputTC2002A_u16BUS		GPIO_ReadInputData(GPIOB)
//readOutput : 16Bits
//#define GetOutputTC2002A_u16BUS		GPIO_ReadOutputData(GPIOB)
//writeOutput : 16Bits
//#define WritePortB_realy(nnnn)	    GPIO_Write(GPIOB, ((GPIO_ReadOutputData(GPIOB))&0x00ff)|(nnnn&0xff00))
#define WritePortB_realy(nnnn)	    GPIO_Write(GPIOB, ((GPIO_ReadOutputData(GPIOB))&0x1fff)|(nnnn&0xe000))
#define WritePortD_realy(nnnn)	    GPIO_Write(GPIOD, ((GPIO_ReadOutputData(GPIOD))&0xf0ff)|(nnnn&0x0f00))
//---CMedia-------------------------------------------------------    
#define CM_SetSDA               {\
                                    GPIO_InitTypeDef GPIO_InitStructure;;\
                                    GPIO_InitStructure.GPIO_Pin = CMEDIA_I2C_DATA;\
                                    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;\
                                    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;\
                                    GPIO_Init(CMEDIA_PORT, &GPIO_InitStructure);\
                                };


#define CM_ResSDA               {\
                                    GPIO_InitTypeDef GPIO_InitStructure;;\
                                    GPIO_InitStructure.GPIO_Pin = CMEDIA_I2C_DATA;\
                                    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;\
                                    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;\
                                    GPIO_Init(CMEDIA_PORT, &GPIO_InitStructure);\
                                };

#define CM_ResSDAA              {\
                                    GPIO_InitTypeDef GPIO_InitStructure;;\
                                    GPIO_InitStructure.GPIO_Pin = CMEDIA_I2C_DATA;\
                                    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;\
                                    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;\
                                    GPIO_Init(CMEDIA_PORT, &GPIO_InitStructure);\
                                };

#define CM_GetSDA               GPIO_ReadInputDataBit(CMEDIA_PORT, CMEDIA_I2C_DATA)
#define CM_SetSCL               GPIO_SetBits(CMEDIA_PORT, CMEDIA_I2C_CLK)
#define CM_ResSCL               GPIO_ResetBits(CMEDIA_PORT, CMEDIA_I2C_CLK)
#define CM_GetSCL               GPIO_ReadOutputDataBit(CMEDIA_PORT, CMEDIA_I2C_CLK)
//----eeprom------------------------------------------------------    
#define SetSDA                  {\
                                    GPIO_InitTypeDef GPIO_InitStructure;;\
                                    GPIO_InitStructure.GPIO_Pin = IIC_SDA;\
                                    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;\
                                    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;\
                                    GPIO_Init(IIC_PORT, &GPIO_InitStructure);\
                                };


#define ResSDA                  {\
                                    GPIO_InitTypeDef GPIO_InitStructure;;\
                                    GPIO_InitStructure.GPIO_Pin = IIC_SDA;\
                                    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;\
                                    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;\
                                    GPIO_Init(IIC_PORT, &GPIO_InitStructure);\
                                };

#define ResSDAA                  {\
                                    GPIO_InitTypeDef GPIO_InitStructure;;\
                                    GPIO_InitStructure.GPIO_Pin = IIC_SDA;\
                                    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;\
                                    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;\
                                    GPIO_Init(IIC_PORT, &GPIO_InitStructure);\
                                };

#define GetSDA                  GPIO_ReadInputDataBit(IIC_PORT, IIC_SDA)
#define SetSCL                  GPIO_SetBits(IIC_PORT, IIC_SCL)
#define ResSCL                  GPIO_ResetBits(IIC_PORT, IIC_SCL)
#define GetSCL                  GPIO_ReadOutputDataBit(IIC_PORT, IIC_SCL)

/****************************************************************************************/

#if 0
//DAC AK4353
#define DACReset(x)             ( (x==1)? GPIO_SetBits(DAC_PORT, DAC_RESET) : GPIO_ResetBits(DAC_PORT, DAC_RESET) );
#define p_DACi2c_sda(x)         ( (x==1)? GPIO_SetBits(DAC_PORT, DAC_SDA) : GPIO_ResetBits(DAC_PORT, DAC_SDA) );
#define p_DACi2c_scl(x)         ( (x==1)? GPIO_SetBits(DAC_PORTC, DAC_SCL) : GPIO_ResetBits(DAC_PORTC, DAC_SCL) );

#define SetDACSDA               {\
                                    GPIO_InitTypeDef GPIO_InitStructure;;\
                                    GPIO_InitStructure.GPIO_Pin = DAC_SDA;\
                                    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;\
                                    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;\
                                    GPIO_Init(DAC_PORT, &GPIO_InitStructure);\
                                };


#define ResDACSDA               {\
                                    GPIO_InitTypeDef GPIO_InitStructure;;\
                                    GPIO_InitStructure.GPIO_Pin = DAC_SDA;\
                                    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;\
                                    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;\
                                    GPIO_Init(DAC_PORT, &GPIO_InitStructure);\
                                };

#define GetDACSDA               GPIO_ReadInputDataBit(DAC_PORT, DAC_SDA)

#endif /* if 0 */
/****************************************************************************************/



#endif  //__PORT_DEF__

/* end of file */
/******************* (C) COPYRIGHT 2008 Rotel Technology Global **************/

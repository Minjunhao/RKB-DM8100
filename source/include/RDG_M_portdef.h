/**************** (c)2008 Rotel Technology Global Inc..*************************
PROJECT         : Front Panel for Large VFD
COMPILER        : IAR Embeded Workbench 4.4.1
MODULE          : RCX_portdef.h
VERSION         : V 1.0
USED CHIP       : RDG_M(stm32f101ZE) [Z=144pin, E=512KB]
CREATION DATE   : /2010
AUTHORS         : / RTG
DESCRIPTION     :
*******************************************************************************/
#ifndef __PORT_DEF__
#define __PORT_DEF__

//Address
#define PORTE_PORT				GPIOE
#define FSMC_A23				GPIO_Pin_2			//1
#define FSMC_A19				GPIO_Pin_3			//2
#define FSMC_A20				GPIO_Pin_4			//3
#define FSMC_A21				GPIO_Pin_5			//4
#define FSMC_A22				GPIO_Pin_6			//5

//6,VBAT

//for power-down
#define POWERDOWN_PORT			GPIOC
#define POWER_DOWN				GPIO_Pin_13			//7

//for option port
#define PROJ_PORT				GPIOC
#define PROJ_RT09				GPIO_Pin_14			//8,RDG(low)

//for rmc in port
#define RMC_PORT				GPIOC
#define p_Rmc_Ext				GPIO_Pin_15			//9

//Address
#define PORTF_PORT				GPIOF
#define FSMC_A0					GPIO_Pin_0			//10
#define FSMC_A1					GPIO_Pin_1			//11
#define FSMC_A2					GPIO_Pin_2			//12
#define FSMC_A3					GPIO_Pin_3			//13
#define FSMC_A4					GPIO_Pin_4			//14
#define FSMC_A5					GPIO_Pin_5			//15

//16,VSS-5
//17,VDD-5

//for AK4104
#define AK4104_PORT				GPIOF
#define AK4104_DO				GPIO_Pin_6			//18	: input

#define ANTENNA					GPIOF
#define ANTENNA_AB				GPIO_Pin_7			//19

#define TC9162_PORT				GPIOF
#define TC9162_ST				GPIO_Pin_8			//20
#define TC9162_DA				GPIO_Pin_9			//21
#define TC9162_CK				GPIO_Pin_10			//22

//23,OSC-in(4MHz)
//24,OSC-out
//25,NRST

//for ADC port
#define ADC_PORT				GPIOC
#define ADC_IN0					GPIO_Pin_0			//26
#define ADC_IN1					GPIO_Pin_1			//27
#define ADC_IN2					GPIO_Pin_2			//28
#define ADC_IN3					GPIO_Pin_3			//29

//30,VSSA
//31,VREF-
//32,VREF+
//33,VDDA

//for front LED
#define GPIO_LED				GPIOA    
#define STANDBY_LED				GPIO_Pin_0			//34

//for front Micom Reset
#define FRONT_RMC				GPIOA    
#define p_Rmc					GPIO_Pin_1			//35

//for UARTx (UART2->Telechips)
//#define GPIOx					GPIOA
//#define RCC_APB2Periph_GPIOx	RCC_APB2Periph_GPIOA
#define GPIO_Tx2Pin				GPIO_Pin_2			//36
#define GPIO_Rx2Pin				GPIO_Pin_3			//37
	
//38,VAA-4
//39,VDD-4

//for vfd port
#define VFD_PORT				GPIOA
#define VFD_CLOCK				GPIO_Pin_4			//40
#define VFD_DATA				GPIO_Pin_5			//41	
#define VFD_LAT					GPIO_Pin_6			//42
#define VFD_BK					GPIO_Pin_7			//43 : Blanking(1), DisplayOn(0)

//for encoder port
#define ENC_PORT				GPIOC
#define ENC_0					GPIO_Pin_4			//44
#define ENC_1					GPIO_Pin_5			//45

//for i2s switching port
#define i2s_PORT				GPIOB
#define iPOD_i2s_SELECT			GPIO_Pin_0			//46,stingray_i2s=L
//for stingray port
#define GPIO_STINGRAY			GPIOB
#define STR_nRESET				GPIO_Pin_1			//47

//48,BOOT1=L(GPIOB,GPIO_Pin_2)

#define PORTF_PORT				GPIOF
#define dummy_f11				GPIO_Pin_11			//49

#define FSMC_A6					GPIO_Pin_12			//50

//51,VSS-6
//52,VDD-6

#define PORTF_PORT				GPIOF
#define FSMC_A7					GPIO_Pin_13			//53
#define FSMC_A8					GPIO_Pin_14			//54
#define FSMC_A9					GPIO_Pin_15			//55

#define PORTG_PORT				GPIOG
#define FSMC_A10				GPIO_Pin_0			//56
#define FSMC_A11				GPIO_Pin_1			//57

#define PORTE_PORT				GPIOE
#define FSMC_D4					GPIO_Pin_7			//58
#define FSMC_D5					GPIO_Pin_8			//59
#define FSMC_D6					GPIO_Pin_9			//60

//61,VSS-7
//62,VDD-7

#define PORTE_PORT				GPIOE
#define FSMC_D7					GPIO_Pin_10			//63
#define FSMC_D8					GPIO_Pin_11			//64
#define FSMC_D9					GPIO_Pin_12			//65
#define FSMC_D10				GPIO_Pin_13			//66
#define FSMC_D11				GPIO_Pin_14			//67
#define FSMC_D12				GPIO_Pin_15			//68

//for UARTx (UART3->Stingray)
//#define GPIOx					GPIOB
//#define RCC_APB2Periph_GPIOx	RCC_APB2Periph_GPIOB
#define GPIO_Tx3Pin				GPIO_Pin_10			//69
#define GPIO_Rx3Pin				GPIO_Pin_11			//70

//71,VSS-1
//72,VDD-1

//for 4114 switching port
#define AK4114_PORT				GPIOB
#define p_4114_Int1				GPIO_Pin_12			//73
#define p_4114_Int0				GPIO_Pin_13			//74
#define p_4114_CS				GPIO_Pin_14			//75
#define p_4114_CLK				GPIO_Pin_15			//76

#define PORTD_PORT				GPIOD
#define FSMC_D13				GPIO_Pin_8			//77
#define FSMC_D14				GPIO_Pin_9			//78
#define FSMC_D15				GPIO_Pin_10			//79

#define FSMC_A16				GPIO_Pin_11			//80
#define FSMC_A17				GPIO_Pin_12			//81
#define FSMC_A18				GPIO_Pin_13			//82

//83,VSS-8
//84,VDD-8

#define FSMC_D0					GPIO_Pin_14			//85
#define FSMC_D1					GPIO_Pin_15			//86

#define PORTG_PORT				GPIOG
#define FSMC_A12				GPIO_Pin_2			//87
#define FSMC_A13				GPIO_Pin_3			//88
#define FSMC_A14				GPIO_Pin_4			//89
#define FSMC_A15				GPIO_Pin_5			//90

#define TC9482_PORT				GPIOG
#define TC9482_CS				GPIO_Pin_6			//91
#define TC9482_DA				GPIO_Pin_7			//92
#define TC9482_CK				GPIO_Pin_8			//93

//94,VSS-9
//95,VDD-9

//for 4114 switching port
#define AK4114_PORTC			GPIOC
#define p_4114_DO				GPIO_Pin_6			//96
#define p_4114_DI				GPIO_Pin_7			//97
#define p_4114_RESET			GPIO_Pin_8			//98

//for DAC port
#define DAC_PORT				GPIOC
#define WM8740_RESET			GPIO_Pin_9			//99
//for DAC port
#define DAC_PORTA				GPIOA
#define WM8740_ML				GPIO_Pin_8			//100

//for UARTx (UART1)
//#define GPIOx					GPIOA
//#define RCC_APB2Periph_GPIOx	RCC_APB2Periph_GPIOA
#define GPIO_Tx1Pin				GPIO_Pin_9			//101
#define GPIO_Rx1Pin				GPIO_Pin_10			//102

//for main mute port
#define MAIN_PORT				GPIOA
#define MAIN_MUTE				GPIO_Pin_11			//103
#define MAIN_STBY				GPIO_Pin_12			//104

//105,JTMS(GPIOA,GPIO_Pin_13)
//106,NC
//107,VSS-2
//108,VDD-2

//109,JTCK(GPIOA,GPIO_Pin_14)
//110,JTDI(GPIOA,GPIO_Pin_15)

//for UARTx (UART4 ->MPEG)
//#define GPIOx					GPIOC
//#define RCC_APB2Periph_GPIOx	RCC_APB2Periph_GPIOC
#define GPIO_Tx4Pin				GPIO_Pin_10			//111
#define GPIO_Rx4Pin				GPIO_Pin_11			//112

//for iPod port
#define iPOD_PORTC				GPIOC
#define iPOD_RESET				GPIO_Pin_12			//113,(Low active)

//
#define PORTD_PORT				GPIOD
#define FSMC_D2					GPIO_Pin_0			//114
#define FSMC_D3					GPIO_Pin_1			//115

#define STBY2_PORT				GPIOD
#define STANDBY2				GPIO_Pin_2			//116			//standby2=RDGV2

#define AMP_PORT				GPIOD
#define nAMP_MUTE				GPIO_Pin_3			//117

#define FSMC_NOE				GPIO_Pin_4			//118
#define FSMC_NWE				GPIO_Pin_5			//119

//120,VSS-10
//121,VDD-10

#define FSMC_NWAIT				GPIO_Pin_6			//122
#define FSMC_NE1				GPIO_Pin_7			//123

#define PORTG_PORT				GPIOG
#define FSMC_NE2				GPIO_Pin_9			//124
#define FSMC_NE3				GPIO_Pin_10			//125
#define CDP_PORT				GPIOG
#define nMPEG_RST				GPIO_Pin_11			//126
#define FSMC_NE4				GPIO_Pin_12			//127
#define HP_PORT					GPIOG
#define nHP_MUTE				GPIO_Pin_13			//128			//FSMC_A24
#define nHP_inCHECK				GPIO_Pin_14			//129 : input	//FSMC_A25

//130,VSS-11
//131,VDD-11

#define EXTCLK_PORT				GPIOG
#define EXTCLK_DIGITAL			GPIO_Pin_15			//132

//133,JTDO(GPIOB,GPIO_Pin_3)
//134,JNTRST(GPIOB,GPIO_Pin_4)

//for Trigger 12V port
#define T12V_PORT				GPIOB
#define T12V_IN					GPIO_Pin_5			//135

//for eeprom port
#define IIC_PORT				GPIOB
#define IIC_SCL					GPIO_Pin_6			//136
#define IIC_SDA					GPIO_Pin_7			//137

//138,BOOT0

#define AK4104_PORTB			GPIOB
#define AK4104_DA				GPIO_Pin_8			//139
#define AK4104_CK				GPIO_Pin_9			//140

#define AK4104_PORTE			GPIOE
#define AK4104_CS				GPIO_Pin_0			//141
#define AK4104_nRST				GPIO_Pin_1			//142

//143,VSS-3
//144,VDD-3

	
/****************************************************************************************/
//for printf
#define  PRINTx					USART1

/****************************************************************************************/
/* macro definition */

//T12V out : 20091119
#define Standby2(x)	 			( (x==1)? GPIO_SetBits(STBY2_PORT, STANDBY2) : GPIO_ResetBits(STBY2_PORT, STANDBY2) );
//T12V in
#define GetT12V_IN				GPIO_ReadInputDataBit(T12V_PORT, T12V_IN)
//EXT Clock disable
#define ExtClockDisable(x)		( (x==1)? GPIO_SetBits(EXTCLK_PORT, EXTCLK_DIGITAL) : GPIO_ResetBits(EXTCLK_PORT, EXTCLK_DIGITAL) );




//1 AK4104 Do Input
#define GetAK4104_Do			GPIO_ReadInputDataBit(AK4104_PORT, AK4104_DO)
//7(2)power down
#define GetPowerDown			GPIO_ReadInputDataBit(POWERDOWN_PORT, POWER_DOWN)
//8(3)option RCX_II
//#define GetOptionRCX_II			GPIO_ReadInputDataBit(PROJ_PORT, PROJ_RCX_II)
//optin RT09
#define GetOptionRT09			GPIO_ReadInputDataBit(PROJ_PORT, PROJ_RT09)

//9(4)ext rmc
#define p_rmc_ext				GPIO_ReadInputDataBit(RMC_PORT, p_Rmc_Ext)
//23(14)LED
#define StandbyLED(x) 			( (x==1)? GPIO_SetBits(GPIO_LED, STANDBY_LED) : GPIO_ResetBits(GPIO_LED, STANDBY_LED) );
//24(15)rmc
#define p_rmc					GPIO_ReadInputDataBit(FRONT_RMC, p_Rmc)
//29~32(20~23)VFD
#define VFD_Clock(x)			( (x==1)? GPIO_SetBits(VFD_PORT, VFD_CLOCK) : GPIO_ResetBits(VFD_PORT, VFD_CLOCK) );
#define VFD_Data(x)				( (x==1)? GPIO_SetBits(VFD_PORT, VFD_DATA) : GPIO_ResetBits(VFD_PORT, VFD_DATA) );
#define VFD_Latch(x)			( (x==1)? GPIO_SetBits(VFD_PORT, VFD_LAT) : GPIO_ResetBits(VFD_PORT, VFD_LAT) );
#define VFD_Bk(x)				( (x==1)? GPIO_SetBits(VFD_PORT, VFD_BK) : GPIO_ResetBits(VFD_PORT, VFD_BK) );

//35(26)i2s select(iPod & Stingray)
#define i2s_iPod_nSTM(x)		( (x==1)? GPIO_SetBits(i2s_PORT, iPOD_i2s_SELECT) : GPIO_ResetBits(i2s_PORT, iPOD_i2s_SELECT) );
//36(27)stingray reset
#define STRnRESET(x) 			( (x==1)? GPIO_SetBits(GPIO_STINGRAY, STR_nRESET) : GPIO_ResetBits(GPIO_STINGRAY, STR_nRESET) );

//38 Antenna A/B
#define Antenna_aB(x) 			( (x==1)? GPIO_SetBits(ANTENNA, ANTENNA_AB) : GPIO_ResetBits(ANTENNA, ANTENNA_AB) );

//39~41 TC9162
#define TC9162_STB(x)			( (x==1)? GPIO_SetBits(TC9162_PORT, TC9162_ST) : GPIO_ResetBits(TC9162_PORT, TC9162_ST) );
#define TC9162_Data(x)			( (x==1)? GPIO_SetBits(TC9162_PORT, TC9162_DA) : GPIO_ResetBits(TC9162_PORT, TC9162_DA) );
#define TC9162_Clock(x)			( (x==1)? GPIO_SetBits(TC9162_PORT, TC9162_CK) : GPIO_ResetBits(TC9162_PORT, TC9162_CK) );
//42~44 TC9482
#define TC9482_STB(x)			( (x==1)? GPIO_SetBits(TC9482_PORT, TC9482_CS) : GPIO_ResetBits(TC9482_PORT, TC9482_CS) );
#define TC9482_Data(x)			( (x==1)? GPIO_SetBits(TC9482_PORT, TC9482_DA) : GPIO_ResetBits(TC9482_PORT, TC9482_DA) );
#define TC9482_Clock(x)			( (x==1)? GPIO_SetBits(TC9482_PORT, TC9482_CK) : GPIO_ResetBits(TC9482_PORT, TC9482_CK) );

//45 HP mute
#define HP_nMute(x) 			( (x==1)? GPIO_SetBits(HP_PORT, nHP_MUTE) : GPIO_ResetBits(HP_PORT, nHP_MUTE) );
//46 HP in check
#define GetHP_nInput			GPIO_ReadInputDataBit(HP_PORT, nHP_inCHECK)

//51~54(33~36)AK4114
#define Get4114INT1				GPIO_ReadInputDataBit(AK4114_PORT, p_4114_Int1)
#define Get4114INT0				GPIO_ReadInputDataBit(AK4114_PORT, p_4114_Int0)
#define p_4114_cs(x) 			( (x==1)? GPIO_SetBits(AK4114_PORT, p_4114_CS) : GPIO_ResetBits(AK4114_PORT, p_4114_CS) );
#define p_4114_clk(x) 			( (x==1)? GPIO_SetBits(AK4114_PORT, p_4114_CLK) : GPIO_ResetBits(AK4114_PORT, p_4114_CLK) );


//62 AMP mute
#define AMP_nMute(x) 			( (x==1)? GPIO_SetBits(AMP_PORT, nAMP_MUTE) : GPIO_ResetBits(AMP_PORT, nAMP_MUTE) );
//63~65(37~39)AK4114
#define p_4114_do(x) 			( (x==1)? GPIO_SetBits(AK4114_PORTC, p_4114_DO) : GPIO_ResetBits(AK4114_PORTC, p_4114_DO) );
#define Get4114di				GPIO_ReadInputDataBit(AK4114_PORTC, p_4114_DI)
#define p_4114_reset(x) 		( (x==1)? GPIO_SetBits(AK4114_PORTC, p_4114_RESET) : GPIO_ResetBits(AK4114_PORTC, p_4114_RESET) );

//66(40),67(41)WM8740
#define WM8740_nReset(x)		( (x==1)? GPIO_SetBits(DAC_PORT, WM8740_RESET) : GPIO_ResetBits(DAC_PORT, WM8740_RESET) );
#define WM8740_nML(x)			( (x==1)? GPIO_SetBits(DAC_PORTA, WM8740_ML) : GPIO_ResetBits(DAC_PORTA, WM8740_ML) );
#define WM8740_MD(x) 			( (x==1)? GPIO_SetBits(AK4114_PORTC, p_4114_DO) : GPIO_ResetBits(AK4114_PORTC, p_4114_DO) );
#define WM8740_MC(x) 			( (x==1)? GPIO_SetBits(AK4114_PORT, p_4114_CLK) : GPIO_ResetBits(AK4114_PORT, p_4114_CLK) );
//70(44)main mute
#define Main_nMute(x) 			( (x==1)? GPIO_SetBits(MAIN_PORT, MAIN_MUTE) : GPIO_ResetBits(MAIN_PORT, MAIN_MUTE) );
//71(45)standby
#define Standby(x)	 			( (x==1)? GPIO_SetBits(MAIN_PORT, MAIN_STBY) : GPIO_ResetBits(MAIN_PORT, MAIN_STBY) );
//80(53)iPod
#define iPod_nRESET(x) 			( (x==1)? GPIO_SetBits(iPOD_PORTC, iPOD_RESET) : GPIO_ResetBits(iPOD_PORTC, iPOD_RESET) );
//84 MPEG reset
#define MPEG_nRESET(x) 			( (x==1)? GPIO_SetBits(CDP_PORT, nMPEG_RST) : GPIO_ResetBits(CDP_PORT, nMPEG_RST) );

//85~88 AK4104
#define AK4104_data(x)			( (x==1)? GPIO_SetBits(AK4104_PORTB, AK4104_DA) : GPIO_ResetBits(AK4104_PORTB, AK4104_DA) );
#define AK4104_clock(x)			( (x==1)? GPIO_SetBits(AK4104_PORTB, AK4104_CK) : GPIO_ResetBits(AK4104_PORTB, AK4104_CK) );
#define AK4104_cs(x)			( (x==1)? GPIO_SetBits(AK4104_PORTE, AK4104_CS) : GPIO_ResetBits(AK4104_PORTE, AK4104_CS) );
#define AK4104_nReset(x)		( (x==1)? GPIO_SetBits(AK4104_PORTE, AK4104_nRST) : GPIO_ResetBits(AK4104_PORTE, AK4104_nRST) );

//92~93(58~59)eeprom
#define p_i2c_sda(x) 			( (x==1)? GPIO_SetBits(IIC_PORT, IIC_SDA) : GPIO_ResetBits(IIC_PORT, IIC_SDA) );
#define p_i2c_scl(x) 			( (x==1)? GPIO_SetBits(IIC_PORT, IIC_SCL) : GPIO_ResetBits(IIC_PORT, IIC_SCL) );

//96(62)STM32 Reset enable
#define STM32ResetEnable(x)		( (x==1)? GPIO_SetBits(RESET_PORT, RESET_ENABLE) : GPIO_ResetBits(RESET_PORT, RESET_ENABLE) );

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

#define GetSDA					GPIO_ReadInputDataBit(IIC_PORT, IIC_SDA)
#define SetSCL					GPIO_SetBits(IIC_PORT, IIC_SCL)
#define ResSCL					GPIO_ResetBits(IIC_PORT, IIC_SCL)
#define GetSCL					GPIO_ReadOutputDataBit(IIC_PORT, IIC_SCL)

/****************************************************************************************/

#if 0
//DAC AK4353
#define DACReset(x) 			( (x==1)? GPIO_SetBits(DAC_PORT, DAC_RESET) : GPIO_ResetBits(DAC_PORT, DAC_RESET) );
#define p_DACi2c_sda(x) 		( (x==1)? GPIO_SetBits(DAC_PORT, DAC_SDA) : GPIO_ResetBits(DAC_PORT, DAC_SDA) );
#define p_DACi2c_scl(x) 		( (x==1)? GPIO_SetBits(DAC_PORTC, DAC_SCL) : GPIO_ResetBits(DAC_PORTC, DAC_SCL) );

#define SetDACSDA				{\
                                    GPIO_InitTypeDef GPIO_InitStructure;;\
	                                GPIO_InitStructure.GPIO_Pin = DAC_SDA;\
                                    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;\
                                    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;\
  	                                GPIO_Init(DAC_PORT, &GPIO_InitStructure);\
                                };


#define ResDACSDA				{\
                                    GPIO_InitTypeDef GPIO_InitStructure;;\
	                                GPIO_InitStructure.GPIO_Pin = DAC_SDA;\
                                    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;\
                                    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;\
  	                                GPIO_Init(DAC_PORT, &GPIO_InitStructure);\
                                };

#define GetDACSDA				GPIO_ReadInputDataBit(DAC_PORT, DAC_SDA)

#endif /* if 0 */
/****************************************************************************************/



#endif 	//__PORT_DEF__

/* end of file */
/******************* (C) COPYRIGHT 2008 Rotel Technology Global **************/

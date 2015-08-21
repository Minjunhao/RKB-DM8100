/**************** (c)2008 Rotel Technology Global Inc..*************************
PROJECT         : Front Panel for Large VFD
COMPILER        : IAR Embeded Workbench 4.4.1
MODULE          : RCX_portdef.h
VERSION         : V 1.0
USED CHIP       : RDG(stm32f101RC), RCX(stm32f101VC) [R=64pin, V=100pin, C=256KB/32KB]
CREATION DATE   : /2009
AUTHORS         : / RTG
DESCRIPTION     :
*******************************************************************************/
#ifndef __PORT_DEF__
#define __PORT_DEF__

//for AK4104
#define AK4104_PORT				GPIOE
#define AK4104_DO				GPIO_Pin_2			//1	: input

#define PORTE_PORT				GPIOE
#define Edummy003				GPIO_Pin_3			//2
#define Edummy004				GPIO_Pin_4			//3
#define Edummy005				GPIO_Pin_5			//4
#define Edummy006				GPIO_Pin_6			//5

//6(1),VBAT

//for power-down
#define POWERDOWN_PORT			GPIOC
#define POWER_DOWN				GPIO_Pin_13			//7(2)	

//for option port
#define PROJ_PORT				GPIOC
#define PROJ_RCX_II				GPIO_Pin_14			//8(3),RCX_II(high)

//for rmc in port
//#define RMC_PORT				GPIOC
//#define p_Rmc_Ext				GPIO_Pin_15			//9(4)

//10,VSS-5
//11,VDD-5

//12(5),OSC-in(4MHz)
//13(6),OSC-out
//14(7),NRST

//for ADC port
#define ADC_PORT				GPIOC
#define ADC_IN0					GPIO_Pin_0			//15(8)
#define ADC_IN1					GPIO_Pin_1			//16(9)
#define ADC_IN2					GPIO_Pin_2			//17(10)
#define ADC_IN3					GPIO_Pin_3			//18(11)

//19(12),VSSA
//20,VREF-
//21,VREF+
//22(13),VDDA

//for front LED
#define GPIO_LED				GPIOA    
#define STANDBY_LED				GPIO_Pin_0			//23(14)

//for front Micom Reset
#define FRONT_RMC				GPIOA    
#define p_Rmc					GPIO_Pin_1			//24(15)

//for UARTx (UART2->Telechips)
//#define GPIOx					GPIOA
//#define RCC_APB2Periph_GPIOx	RCC_APB2Periph_GPIOA
#define GPIO_Tx2Pin				GPIO_Pin_2			//25(16)
#define GPIO_Rx2Pin				GPIO_Pin_3			//26(17)
	
//27(18),VAA-4
//28(19),VDD-4

//for vfd port
#define VFD_PORT				GPIOA
#define VFD_CLOCK				GPIO_Pin_4			//29(20)
#define VFD_DATA				GPIO_Pin_5			//30(21)	
#define VFD_LAT					GPIO_Pin_6			//31(22)
#define VFD_BK					GPIO_Pin_7			//32(23) : Blanking(1), DisplayOn(0)

//for encoder port
#define ENC_PORT				GPIOC
#define ENC_0					GPIO_Pin_4			//33(24)
#define ENC_1					GPIO_Pin_5			//34(25)

//for i2s switching port
#define i2s_PORT				GPIOB
#define iPOD_i2s_SELECT			GPIO_Pin_0			//35(26),stingray_i2s=L
//for stingray port
#define GPIO_STINGRAY			GPIOB
#define STR_nRESET				GPIO_Pin_1			//36(27)

//37(28),BOOT1=L(GPIOB,GPIO_Pin_2)

#define ANTENNA					GPIOE
#define ANTENNA_AB				GPIO_Pin_7			//38

#define PORTE_PORT				GPIOE
#define TC9162_ST				GPIO_Pin_8			//39
#define TC9162_DA				GPIO_Pin_9			//40
#define TC9162_CK				GPIO_Pin_10			//41
#define TC9482_CS				GPIO_Pin_11			//42
#define TC9482_DA				GPIO_Pin_12			//43
#define TC9482_CK				GPIO_Pin_13			//44
#define nHP_MUTE				GPIO_Pin_14			//45
#define nHP_inCHECK				GPIO_Pin_15			//46 : input

//for UARTx (UART3->Stingray)
//#define GPIOx					GPIOB
//#define RCC_APB2Periph_GPIOx	RCC_APB2Periph_GPIOB
#define GPIO_Tx3Pin				GPIO_Pin_10			//47(29)
#define GPIO_Rx3Pin				GPIO_Pin_11			//48(30)

//49(31),VSS-1
//50(32),VDD-1

//for 4114 switching port
#define AK4114_PORT				GPIOB
#define p_4114_Int1				GPIO_Pin_12			//51(33)
#define p_4114_Int0				GPIO_Pin_13			//52(34)
#define p_4114_CS				GPIO_Pin_14			//53(35)
#define p_4114_CLK				GPIO_Pin_15			//54(36)

#define PORTD_PORT				GPIOD
#define Ddummy008				GPIO_Pin_8			//55
#define Ddummy009				GPIO_Pin_9			//56
#define Ddummy0010				GPIO_Pin_10			//57
#define Ddummy0011				GPIO_Pin_11			//58
#define Ddummy0012				GPIO_Pin_12			//59
#define Ddummy0013				GPIO_Pin_13			//60
#define Ddummy0014				GPIO_Pin_14			//61
#define nAMP_MUTE				GPIO_Pin_15			//62


//for 4114 switching port
#define AK4114_PORTC			GPIOC
#define p_4114_DO				GPIO_Pin_6			//63(37)
#define p_4114_DI				GPIO_Pin_7			//64(38)
#define p_4114_RESET			GPIO_Pin_8			//65(39)

//for DAC port
#define DAC_PORT				GPIOC
#define WM8740_RESET			GPIO_Pin_9			//66(40)
//for DAC port
#define DAC_PORTA				GPIOA
#define WM8740_ML				GPIO_Pin_8			//67(41)

//for UARTx (UART1)
//#define GPIOx					GPIOA
//#define RCC_APB2Periph_GPIOx	RCC_APB2Periph_GPIOA
#define GPIO_Tx1Pin				GPIO_Pin_9			//68(42)
#define GPIO_Rx1Pin				GPIO_Pin_10			//69(43)

//for main mute port
#define MAIN_PORT				GPIOA
#define MAIN_MUTE				GPIO_Pin_11			//70(44)
#define MAIN_STBY				GPIO_Pin_12			//71(45)

//72(46),JTMS(GPIOA,GPIO_Pin_13)
//73 NC
//74(47),VSS-2
//75(48),VDD-2

//76(49),JTCK(GPIOA,GPIO_Pin_14)
//77(50),JTDI(GPIOA,GPIO_Pin_15)

//for UARTx (UART4 ->MPEG)
//#define GPIOx					GPIOC
//#define RCC_APB2Periph_GPIOx	RCC_APB2Periph_GPIOC
#define GPIO_Tx4Pin				GPIO_Pin_10			//78(51)
#define GPIO_Rx4Pin				GPIO_Pin_11			//79(52)

//for iPod port
#define iPOD_PORTC				GPIOC
#define iPOD_RESET				GPIO_Pin_12			//80(53),(Low active)

//
#define PORTD_PORT				GPIOD
#define Ddummy000				GPIO_Pin_0			//81
#define Ddummy001				GPIO_Pin_1			//82
#define Ddummy002				GPIO_Pin_2			//83(54)
#define nMPEG_RST				GPIO_Pin_3			//84
#define AK4104_DA				GPIO_Pin_4			//85
#define AK4104_CK				GPIO_Pin_5			//86
#define AK4104_CS				GPIO_Pin_6			//87
#define AK4104_nRST				GPIO_Pin_7			//88

//89(55),JTDO(GPIOB,GPIO_Pin_3)
//90(56),JNTRST(GPIOB,GPIO_Pin_4)

//for Trigger 12V port
//#define T12V_PORT				GPIOB
#define Bdummy005				GPIO_Pin_5			//91(57)

//for eeprom port
#define IIC_PORT				GPIOB
#define IIC_SCL					GPIO_Pin_6			//92(58)
#define IIC_SDA					GPIO_Pin_7			//93(59)
	
//94(60),BOOT0
//
#define PORTB_PORT				GPIOB
#define Bdummy008				GPIO_Pin_8			//95(61)
//62,STM32 mpu reset(High active)
#define RESET_PORT				GPIOB
#define RESET_ENABLE			GPIO_Pin_9			//96(62)

//
#define PORTE_PORT				GPIOE
#define Edummy000				GPIO_Pin_0			//97
#define Edummy001				GPIO_Pin_1			//98

//99(63),VSS-3
//100(64),VDD-3

	
/****************************************************************************************/
//for printf
#define  PRINTx					USART1

/****************************************************************************************/
/* macro definition */

//1 AK4104 Do Input
#define GetAK4104_Do			GPIO_ReadInputDataBit(AK4104_PORT, AK4104_DO)
//7(2)power down
#define GetPowerDown			GPIO_ReadInputDataBit(POWERDOWN_PORT, POWER_DOWN)
//8(3)option RCX_II
#define GetOptionRCX_II			GPIO_ReadInputDataBit(PROJ_PORT, PROJ_RCX_II)

//9(4)ext rmc
//#define p_rmc_ext				GPIO_ReadInputDataBit(RMC_PORT, p_Rmc_Ext)
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
#define TC9162_STB(x)			( (x==1)? GPIO_SetBits(PORTE_PORT, TC9162_ST) : GPIO_ResetBits(PORTE_PORT, TC9162_ST) );
#define TC9162_Data(x)			( (x==1)? GPIO_SetBits(PORTE_PORT, TC9162_DA) : GPIO_ResetBits(PORTE_PORT, TC9162_DA) );
#define TC9162_Clock(x)			( (x==1)? GPIO_SetBits(PORTE_PORT, TC9162_CK) : GPIO_ResetBits(PORTE_PORT, TC9162_CK) );
//42~44 TC9482
#define TC9482_STB(x)			( (x==1)? GPIO_SetBits(PORTE_PORT, TC9482_CS) : GPIO_ResetBits(PORTE_PORT, TC9482_CS) );
#define TC9482_Data(x)			( (x==1)? GPIO_SetBits(PORTE_PORT, TC9482_DA) : GPIO_ResetBits(PORTE_PORT, TC9482_DA) );
#define TC9482_Clock(x)			( (x==1)? GPIO_SetBits(PORTE_PORT, TC9482_CK) : GPIO_ResetBits(PORTE_PORT, TC9482_CK) );

//45 HP mute
#define HP_nMute(x) 			( (x==1)? GPIO_SetBits(PORTE_PORT, nHP_MUTE) : GPIO_ResetBits(PORTE_PORT, nHP_MUTE) );
//46 HP in check
#define GetHP_nInput			GPIO_ReadInputDataBit(PORTE_PORT, nHP_inCHECK)

//51~54(33~36)AK4114
#define Get4114INT1				GPIO_ReadInputDataBit(AK4114_PORT, p_4114_Int1)
#define Get4114INT0				GPIO_ReadInputDataBit(AK4114_PORT, p_4114_Int0)
#define p_4114_cs(x) 			( (x==1)? GPIO_SetBits(AK4114_PORT, p_4114_CS) : GPIO_ResetBits(AK4114_PORT, p_4114_CS) );
#define p_4114_clk(x) 			( (x==1)? GPIO_SetBits(AK4114_PORT, p_4114_CLK) : GPIO_ResetBits(AK4114_PORT, p_4114_CLK) );


//62 AMP mute
#define AMP_nMute(x) 			( (x==1)? GPIO_SetBits(PORTD_PORT, nAMP_MUTE) : GPIO_ResetBits(PORTD_PORT, nAMP_MUTE) );
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
#define MPEG_nRESET(x) 			( (x==1)? GPIO_SetBits(PORTD_PORT, nMPEG_RST) : GPIO_ResetBits(PORTD_PORT, nMPEG_RST) );

//85~88 AK4104
#define AK4104_data(x)			( (x==1)? GPIO_SetBits(PORTD_PORT, AK4104_DA) : GPIO_ResetBits(PORTD_PORT, AK4104_DA) );
#define AK4104_clock(x)			( (x==1)? GPIO_SetBits(PORTD_PORT, AK4104_CK) : GPIO_ResetBits(PORTD_PORT, AK4104_CK) );
#define AK4104_cs(x)			( (x==1)? GPIO_SetBits(PORTD_PORT, AK4104_CS) : GPIO_ResetBits(PORTD_PORT, AK4104_CS) );
#define AK4104_nReset(x)		( (x==1)? GPIO_SetBits(PORTD_PORT, AK4104_nRST) : GPIO_ResetBits(PORTD_PORT, AK4104_nRST) );

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

/**************** (c)2008 Rotel Technology Global Inc..*************************
PROJECT         : test of OLED , External Flash , USB 
COMPILER        : IAR Embeded Workbench 4.4.1
MODULE          : T144_portdef.h
VERSION         : V 1.0
USED CHIP       : TargetBoard(stm32f103ZG)[144pin, 1024KB, 96KB]	[Z=144pin, V=100pin, R=64pin, C=48pin, T=36pin]
				: G=1024KB[0x0800 2000 ~ 0x080f ffff] /96KB[0x2000 0000 ~ 0x2001 7fff]
				: F= 768KB[0x0800 2000 ~ 0x080b ffff] /96KB[0x2000 0000 ~ 0x2001 7fff]
				: E= 512KB[0x0800 2000 ~ 0x0807 ffff] /64KB[0x2000 0000 ~ 0x2000 ffff]
				: D= 384KB[0x0800 2000 ~ 0x0805 ffff] /64KB[0x2000 0000 ~ 0x2000 ffff]
				: C= 256KB[0x0800 2000 ~ 0x0803 ffff] /48KB[0x2000 0000 ~ 0x2000 bfff] 	/32KB[0x2000 0000 ~ 0x2000 7fff]	
				: RCX1500 (STM32F101VC) [100pin, 256KB, 32KB]
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
//#define POWER_DOWN			GPIO_Pin_13			//7

//for option port
#define PROJ_PORT				GPIOC
//#define PROJ_RT09				GPIO_Pin_14			//8,RDG(low)

//for rmc in port
#define RMC_PORT				GPIOC
//#define p_Rmc_Ext				GPIO_Pin_15			//9

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
//#define AK4104_DO				GPIO_Pin_6			//18	: input

#define ANTENNA					GPIOF
//#define ANTENNA_AB			GPIO_Pin_7			//19

#define TC9162_PORT				GPIOF
//#define TC9162_ST				GPIO_Pin_8			//20
//#define TC9162_DA				GPIO_Pin_9			//21
//#define TC9162_CK				GPIO_Pin_10			//22

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

//for i2s switching port
#define i2s_PORT				GPIOA    
#define iPOD_i2s_SELECT			GPIO_Pin_0			//34,stingray_i2s=L

//for iPod port
#define iPOD_PORTC				GPIOA
#define iPOD_RESET				GPIO_Pin_1			//35,(Low active)

//for UARTx (UART2->Telechips)
//#define GPIOx					GPIOA
//#define RCC_APB2Periph_GPIOx	RCC_APB2Periph_GPIOA
#define GPIO_Tx2Pin				GPIO_Pin_2			//36
#define GPIO_Rx2Pin				GPIO_Pin_3			//37
	
//38,VAA-4
//39,VDD-4

//for vfd port
#define VFD_PORT				GPIOA
//#define VFD_CLOCK				GPIO_Pin_4			//40
//#define VFD_DATA				GPIO_Pin_5			//41	
//#define VFD_LAT				GPIO_Pin_6			//42
//#define VFD_BK				GPIO_Pin_7			//43 : Blanking(1), DisplayOn(0)

//for encoder port
#define ENC_PORT				GPIOC
//#define ENC_0					GPIO_Pin_4			//44
//#define ENC_1					GPIO_Pin_5			//45

//for i2s switching port
#define i2s_PORT				GPIOB
//#define iPOD_i2s_SELECT		GPIO_Pin_0			//46,stingray_i2s=L

//for stingray port
#define GPIO_STINGRAY			GPIOB
//#define STR_nRESET			GPIO_Pin_1			//47

//48,BOOT1=L(GPIOB,GPIO_Pin_2)

#define PORTF_PORT				GPIOF
//#define dummy_f11				GPIO_Pin_11			//49

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
#define PROJ_PORT				GPIOB
#define PROJ_RT09				GPIO_Pin_12			//73
#define p_option1				GPIO_Pin_13			//74
#define p_stwup					GPIO_Pin_14			//75

//for stingray port
#define GPIO_STINGRAY			GPIOB
#define STR_nRESET				GPIO_Pin_15			//76

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
//#define TC9482_CS				GPIO_Pin_6			//91
//#define TC9482_DA				GPIO_Pin_7			//92
//#define TC9482_CK				GPIO_Pin_8			//93

//94,VSS-9
//95,VDD-9

//for 4114 switching port
#define AK4114_PORTC			GPIOC
//#define p_4114_DO				GPIO_Pin_6			//96
//#define p_4114_DI				GPIO_Pin_7			//97
//#define p_4114_RESET			GPIO_Pin_8			//98

//for DAC port
#define DAC_PORT				GPIOC
//#define WM8740_RESET			GPIO_Pin_9			//99
//for DAC port
#define DAC_PORTA				GPIOA
//#define WM8740_ML				GPIO_Pin_8			//100

//for UARTx (UART1 : RS232)
//#define GPIOx					GPIOA
//#define RCC_APB2Periph_GPIOx	RCC_APB2Periph_GPIOA
#define GPIO_Tx1Pin				GPIO_Pin_9			//101
#define GPIO_Rx1Pin				GPIO_Pin_10			//102

//for main mute port
#define USBDM					GPIO_Pin_11			//103
#define USBDP					GPIO_Pin_12			//104

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

//#define iPOD_RESET			GPIO_Pin_12			//113,(Low active)

//
#define PORTD_PORT				GPIOD
#define FSMC_D2					GPIO_Pin_0			//114
#define FSMC_D3					GPIO_Pin_1			//115

#define STBY2_PORT				GPIOD
//#define STANDBY2				GPIO_Pin_2			//116			//standby2=RDGV2

#define AMP_PORT				GPIOD
//#define nAMP_MUTE				GPIO_Pin_3			//117

#define FSMC_NOE				GPIO_Pin_4			//118
#define FSMC_NWE				GPIO_Pin_5			//119

//120,VSS-10
//121,VDD-10

#define FSMC_NWAIT				GPIO_Pin_6			//122
#define FSMC_NE1				GPIO_Pin_7			//123

#define PORTG_PORT				GPIOG
#define FSMC_NE2				GPIO_Pin_9			//124
#define FSMC_NE3				GPIO_Pin_10			//125
//#define nMPEG_RST				GPIO_Pin_11			//126
#define FSMC_NE4				GPIO_Pin_12			//127
//#define nHP_MUTE				GPIO_Pin_13			//128			//FSMC_A24
//#define nHP_inCHECK			GPIO_Pin_14			//129 : input	//FSMC_A25

//130,VSS-11
//131,VDD-11

#define EXTCLK_PORT				GPIOG
//#define EXTCLK_DIGITAL		GPIO_Pin_15			//132

//133,JTDO(GPIOB,GPIO_Pin_3)
//134,JNTRST(GPIOB,GPIO_Pin_4)

//for Trigger 12V port
#define T12V_PORT				GPIOB
//#define T12V_IN				GPIO_Pin_5			//135

//for eeprom port
#define IIC_PORT				GPIOB
#define IIC_SCL					GPIO_Pin_6			//136
#define IIC_SDA					GPIO_Pin_7			//137

//138,BOOT0

#define AK4104_PORTB			GPIOB
//#define AK4104_DA				GPIO_Pin_8			//139
//#define AK4104_CK				GPIO_Pin_9			//140

#define AK4104_PORTE			GPIOE
//#define AK4104_CS				GPIO_Pin_0			//141
//#define AK4104_nRST			GPIO_Pin_1			//142

//143,VSS-3
//144,VDD-3

	
/****************************************************************************************/
//for printf
#define  PRINTx					USART1

/****************************************************************************************/
/* macro definition */


//optin RT09
#define GetOptionRT09			GPIO_ReadInputDataBit(PROJ_PORT, PROJ_RT09)


//35(26)i2s select(iPod & Stingray)
#define i2s_iPod_nSTM(x)		( (x==1)? GPIO_SetBits(i2s_PORT, iPOD_i2s_SELECT) : GPIO_ResetBits(i2s_PORT, iPOD_i2s_SELECT) );
//36(27)stingray reset
#define STRnRESET(x) 			( (x==1)? GPIO_SetBits(GPIO_STINGRAY, STR_nRESET) : GPIO_ResetBits(GPIO_STINGRAY, STR_nRESET) );

//80(53)iPod
#define iPod_nRESET(x) 			( (x==1)? GPIO_SetBits(iPOD_PORTC, iPOD_RESET) : GPIO_ResetBits(iPOD_PORTC, iPOD_RESET) );


//92~93(58~59)eeprom
#define p_i2c_sda(x) 			( (x==1)? GPIO_SetBits(IIC_PORT, IIC_SDA) : GPIO_ResetBits(IIC_PORT, IIC_SDA) );
#define p_i2c_scl(x) 			( (x==1)? GPIO_SetBits(IIC_PORT, IIC_SCL) : GPIO_ResetBits(IIC_PORT, IIC_SCL) );


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

/****************************************************************************************/



#endif 	//__PORT_DEF__

/* end of file */
/******************* (C) COPYRIGHT 2008 Rotel Technology Global **************/

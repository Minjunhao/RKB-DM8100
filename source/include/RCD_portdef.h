/**************** (c)2008 Rotel Technology Global Inc..*************************
PROJECT         : Front Panel for VFD
COMPILER        : IAR Embeded Workbench 5.50
MODULE          : RCD_portdef.h
VERSION         : V 1.0
USED CHIP       : C8(stm32f101C8) [C=48pin, 8=64KB/10KB]
USED CHIP       : C8(stm32f101RC) [R=64pin, C=256KB/32KB]
CREATION DATE   : /2011
AUTHORS         : / RTG
DESCRIPTION     :
*******************************************************************************/
#ifndef __PORT_DEF__
#define __PORT_DEF__

#ifdef RCD12_64PIN

//1,VBAT

#define Cdummy013				GPIO_Pin_13			//2	
#define Cdummy014				GPIO_Pin_14			//3
#define Cdummy015				GPIO_Pin_15			//4

//5,OSC-in(4MHz)
//6,OSC-out
//7,NRST

#define Cdummy000				GPIO_Pin_0			//8
#define Cdummy001				GPIO_Pin_1			//9

#define STBY_PORT				GPIOC
#define MAIN_STBY				GPIO_Pin_2			//10

#define POWERDOWN_PORT			GPIOC    
#define POWER_DOWN				GPIO_Pin_3			//11 : input

//12,VSSA
//13,VDDA

#define ADC_PORT				GPIOA    
#define ADC_IN0					GPIO_Pin_0			//14 : input

//for front LED
#define GPIO_LED				GPIOA
#define STANDBY_LED				GPIO_Pin_1			//15

//for UART2 (x)
//#define GPIOx				GPIOA
//#define RCC_APB2Periph_GPIOx	RCC_APB2Periph_GPIOA
#define GPIO_Tx2Pin				GPIO_Pin_2			//16 : MPEG 19200,n,8,1
#define GPIO_Rx2Pin				GPIO_Pin_3			//17
	
//18,VAA-4
//19,VDD-4

#define T12V_PORT				GPIOA
#define T12V_IN					GPIO_Pin_4			//20 : input

#define TC2002A_PORTA			GPIOA
#define TC2002A_brt				GPIO_Pin_5			//21 : TC2002A bright , D/Ac

#define FRONT_RMC				GPIOA    
#define p_Rmc					GPIO_Pin_6			//22 : input

#define eRMC_PORT				GPIOA
#define p_Rmc_Ext				GPIO_Pin_7			//23 : input

#define TC2002A_PORTC			GPIOC
#define TC2002A_e				GPIO_Pin_4			//24
#define TC2002A_rw				GPIO_Pin_5			//25

#define TC2002A_PORTB			GPIOB
#define TC2002A_rs				GPIO_Pin_0			//26


#define PWM_PORT				GPIOB
#define TC2002A_BLU				GPIO_Pin_1			//27 : PWM

//28,BOOT1=L(GPIOB,GPIO_Pin_2)

//for UARTx (UART3->Stingray)
//#define GPIOx					GPIOB
//#define RCC_APB2Periph_GPIOx	RCC_APB2Periph_GPIOB
//#define GPIO_Tx3Pin				GPIO_Pin_10			//29
//#define GPIO_Rx3Pin				GPIO_Pin_11			//30
#define TC2002A_D2				GPIO_Pin_10			//29
#define TC2002A_D3				GPIO_Pin_11			//30

//31,VSS-1
//32,VDD-1

#define PORT_PORTB				GPIOB
#define TC2002A_D4				GPIO_Pin_12			//33
#define TC2002A_D5				GPIO_Pin_13			//34
#define TC2002A_D6				GPIO_Pin_14			//35
#define TC2002A_D7				GPIO_Pin_15			//36

#define Cdummy006				GPIO_Pin_6			//37
#define Cdummy007				GPIO_Pin_7			//38

#define MPEG_PORTC				GPIOC
#define nMPEG_RST				GPIO_Pin_8			//39

//for DAC port
#define DAC_PORTC				GPIOC
#define WM8740_RESET			GPIO_Pin_9			//40

#define DAC_PORTA				GPIOA
#define WM8740_ML				GPIO_Pin_8			//41

//for UARTx (UART1)
//#define GPIOx					GPIOA
//#define RCC_APB2Periph_GPIOx	RCC_APB2Periph_GPIOA
#define GPIO_Tx1Pin				GPIO_Pin_9			//42
#define GPIO_Rx1Pin				GPIO_Pin_10			//43

#define DAC_PORTA				GPIOA
#define WM8740_mC				GPIO_Pin_11			//44
#define WM8740_mD				GPIO_Pin_12			//45

//46,JTMS(GPIOA,GPIO_Pin_13)

//47,VSS-2
//48,VDD-2

//49,JTCK(GPIOA,GPIO_Pin_14)
//50,JTDI(GPIOA,GPIO_Pin_15)

//for UARTx (UART4)
//#define GPIOx					GPIOC
//#define RCC_APB2Periph_GPIOx	RCC_APB2Periph_GPIOC
//#define GPIO_Tx4Pin			GPIO_Pin_10			//51
//#define GPIO_Rx4Pin			GPIO_Pin_11			//52
#define Cdummy010				GPIO_Pin_10			//51
#define Cdummy011				GPIO_Pin_11			//52
#define Cdummy012				GPIO_Pin_12			//53

//RCD1570 option (pull-up) 20120116
//RCD12(pull-down)
#define OPTION_PORT				GPIOD
#define RCD1570_ON				GPIO_Pin_2			//54

//55,JTDO(GPIOB,GPIO_Pin_3)
//56,JNTRST(GPIOB,GPIO_Pin_4)

#define MUTE_PORT				GPIOB
#define MAIN_MUTE				GPIO_Pin_5			//57

//for eeprom port
#define IIC_PORT				GPIOB
#define IIC_SCL					GPIO_Pin_6			//58
#define IIC_SDA					GPIO_Pin_7			//59
	
//60,BOOT0
#define TC2002A_D0				GPIO_Pin_8			//61
#define TC2002A_D1				GPIO_Pin_9			//62
//63,VSS-3
//64,VDD-3
	
/****************************************************************************************/
//for printf
#define  PRINTx					USART1

/****************************************************************************************/
/* macro definition */


//24,25,26 STN LCD
#define TC2002A_RS(x)		( (x==1)? GPIO_SetBits(TC2002A_PORTB, TC2002A_rs) : GPIO_ResetBits(TC2002A_PORTB, TC2002A_rs) );
#define TC2002A_RW(x)		( (x==1)? GPIO_SetBits(TC2002A_PORTC, TC2002A_rw) : GPIO_ResetBits(TC2002A_PORTC, TC2002A_rw) );
#define TC2002A_E(x)		( (x==1)? GPIO_SetBits(TC2002A_PORTC, TC2002A_e) : GPIO_ResetBits(TC2002A_PORTC, TC2002A_e) );
#define TC2002A_BRT(x)		( (x==1)? GPIO_SetBits(TC2002A_PORTA, TC2002A_brt) : GPIO_ResetBits(TC2002A_PORTA, TC2002A_brt) );
//61,62,29,30,33,34,35,36 STN LCD Bus[PB8~15]
//BUS inputPort :
//GPIO_Mode_IPU	//GPIO_Mode_IN_FLOATING
#define SetBusInput		{\
							GPIO_InitTypeDef GPIO_InitStructure;;\
							GPIO_InitStructure.GPIO_Pin = TC2002A_D7|TC2002A_D6|TC2002A_D5|TC2002A_D4|TC2002A_D3|TC2002A_D2|TC2002A_D1|TC2002A_D0;\
							GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;\
							GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;\
							GPIO_Init(GPIOB, &GPIO_InitStructure);\
						};

//BUS outputPort : GPIO_Mode_AF_PP : GPIO_Mode_Out_PP
#define ResBusOutput	{\
							GPIO_InitTypeDef GPIO_InitStructure;;\
							GPIO_InitStructure.GPIO_Pin = TC2002A_D7|TC2002A_D6|TC2002A_D5|TC2002A_D4|TC2002A_D3|TC2002A_D2|TC2002A_D1|TC2002A_D0;\
							GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;\
							GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;\
							GPIO_Init(GPIOB, &GPIO_InitStructure);\
						};
//readInput : 16Bits
//#define GetInputTC2002A_u16BUS		GPIO_ReadInputData(GPIOB)
//readOutput : 16Bits
//#define GetOutputTC2002A_u16BUS		GPIO_ReadOutputData(GPIOB)
//writeOutput : 16Bits
#define WriteTC2002A_u16BUS(nnnn)	GPIO_Write(GPIOB, ((GPIO_ReadOutputData(GPIOB))&0x00ff)|(nnnn&0xff00))

//11 power down
#define GetPowerDown			GPIO_ReadInputDataBit(POWERDOWN_PORT, POWER_DOWN)

//20 T12V in
#define GetT12V_IN				GPIO_ReadInputDataBit(T12V_PORT, T12V_IN)

//57 main mute
#define Main_nMute(x) 		( (x==1)? GPIO_SetBits(MUTE_PORT, MAIN_MUTE) : GPIO_ResetBits(MUTE_PORT, MAIN_MUTE) );

//22 rmc
#define p_rmc					GPIO_ReadInputDataBit(FRONT_RMC, p_Rmc)

//23 ext rmc
#define p_rmc_ext				GPIO_ReadInputDataBit(eRMC_PORT, p_Rmc_Ext)

//39 MPEG reset
#define MPEG_nRESET(x) 		( (x==1)? GPIO_SetBits(MPEG_PORTC, nMPEG_RST) : GPIO_ResetBits(MPEG_PORTC, nMPEG_RST) );

//10 standby
#define Standby(x)	 		( (x==1)? GPIO_SetBits(STBY_PORT, MAIN_STBY) : GPIO_ResetBits(STBY_PORT, MAIN_STBY) );

//40,41,44,45 WM8740
#define WM8740_nReset(x)	( (x==1)? GPIO_SetBits(DAC_PORTC, WM8740_RESET) : GPIO_ResetBits(DAC_PORTC, WM8740_RESET) );
#define WM8740_nML(x)		( (x==1)? GPIO_SetBits(DAC_PORTA, WM8740_ML) : GPIO_ResetBits(DAC_PORTA, WM8740_ML) );
#define WM8740_MD(x) 		( (x==1)? GPIO_SetBits(DAC_PORTA, WM8740_mD) : GPIO_ResetBits(DAC_PORTA, WM8740_mD) );
#define WM8740_MC(x) 		( (x==1)? GPIO_SetBits(DAC_PORTA, WM8740_mC) : GPIO_ResetBits(DAC_PORTA, WM8740_mC) );

//15 LED
#define StandbyLED(x) 		( (x==1)? GPIO_SetBits(GPIO_LED, STANDBY_LED) : GPIO_ResetBits(GPIO_LED, STANDBY_LED) );

//54 RCD1570(pull-up) in : 20120116
#define GetRCD1570_IN		GPIO_ReadInputDataBit(OPTION_PORT, RCD1570_ON)

//58,59 eeprom
#define p_i2c_scl(x) 		( (x==1)? GPIO_SetBits(IIC_PORT, IIC_SCL) : GPIO_ResetBits(IIC_PORT, IIC_SCL) );
#define p_i2c_sda(x) 		( (x==1)? GPIO_SetBits(IIC_PORT, IIC_SDA) : GPIO_ResetBits(IIC_PORT, IIC_SDA) );

//IIC
#define SetSDA				{\
								GPIO_InitTypeDef GPIO_InitStructure;;\
								GPIO_InitStructure.GPIO_Pin = IIC_SDA;\
								GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;\
								GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;\
								GPIO_Init(IIC_PORT, &GPIO_InitStructure);\
							};


#define ResSDA				{\
								GPIO_InitTypeDef GPIO_InitStructure;;\
								GPIO_InitStructure.GPIO_Pin = IIC_SDA;\
								GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;\
								GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;\
								GPIO_Init(IIC_PORT, &GPIO_InitStructure);\
							};

#define ResSDAA             {\
								GPIO_InitTypeDef GPIO_InitStructure;;\
								GPIO_InitStructure.GPIO_Pin = IIC_SDA;\
								GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;\
								GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;\
								GPIO_Init(IIC_PORT, &GPIO_InitStructure);\
							};

#define GetSDA				GPIO_ReadInputDataBit(IIC_PORT, IIC_SDA)
#define SetSCL				GPIO_SetBits(IIC_PORT, IIC_SCL)
#define ResSCL				GPIO_ResetBits(IIC_PORT, IIC_SCL)
#define GetSCL				GPIO_ReadOutputDataBit(IIC_PORT, IIC_SCL)

/****************************************************************************************/

#if 0
//DAC AK4353
#define DACReset(x) 		( (x==1)? GPIO_SetBits(DAC_PORT, DAC_RESET) : GPIO_ResetBits(DAC_PORT, DAC_RESET) );
#define p_DACi2c_sda(x) 	( (x==1)? GPIO_SetBits(DAC_PORT, DAC_SDA) : GPIO_ResetBits(DAC_PORT, DAC_SDA) );
#define p_DACi2c_scl(x) 	( (x==1)? GPIO_SetBits(DAC_PORTC, DAC_SCL) : GPIO_ResetBits(DAC_PORTC, DAC_SCL) );

#define SetDACSDA			{\
								GPIO_InitTypeDef GPIO_InitStructure;;\
								GPIO_InitStructure.GPIO_Pin = DAC_SDA;\
								GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;\
								GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;\
								GPIO_Init(DAC_PORT, &GPIO_InitStructure);\
							};


#define ResDACSDA			{\
								GPIO_InitTypeDef GPIO_InitStructure;;\
								GPIO_InitStructure.GPIO_Pin = DAC_SDA;\
								GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;\
								GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;\
								GPIO_Init(DAC_PORT, &GPIO_InitStructure);\
							};

#define GetDACSDA			GPIO_ReadInputDataBit(DAC_PORT, DAC_SDA)

#endif /* if 0 */

/****************************************************************************************/
#else	//RCD12_64PIN
/****************************************************************************************/

//1,VBAT

//
#define PORT_PORTC			GPIOC
#define TC2002A_rs				GPIO_Pin_13			//2

#define TC2002A_PORT		GPIOC
#define TC2002A_rw				GPIO_Pin_14			//3
#define TC2002A_e				GPIO_Pin_15			//4

//5,OSC-in(4MHz)
//6,OSC-out
//7,NRST
//8,VSSA
//9,VDDA

//
#define ADC_PORT			GPIOA    
#define ADC_IN0					GPIO_Pin_0			//10 : input

#define POWERDOWN_PORT		GPIOA    
#define POWER_DOWN				GPIO_Pin_1			//11 : input

//for UART2 (x)
//#define GPIOx				GPIOA
//#define RCC_APB2Periph_GPIOx	RCC_APB2Periph_GPIOA
#define GPIO_Tx2Pin				GPIO_Pin_2			//12 : MPEG 19200,n,8,1
#define GPIO_Rx2Pin				GPIO_Pin_3			//13

#define T12V_PORT			GPIOA
#define T12V_IN					GPIO_Pin_4			//14 : input

#define PORT_PORTA			GPIOA
#define TC2002A_brt				GPIO_Pin_5			//15 : TC2002A bright
#define GPIO_LED			GPIOA
#define STANDBY_LED				GPIO_Pin_5			//15

#define FRONT_RMC			GPIOA    
#define p_Rmc					GPIO_Pin_6			//16 : input

#define eRMC_PORT			GPIOA
#define p_Rmc_Ext				GPIO_Pin_7			//17 : input

#define PORT_PORTB			GPIOB
#define nMPEG_RST				GPIO_Pin_0			//18
//for DAC port
#define DAC_PORT			GPIOB
#define WM8740_RESET			GPIO_Pin_1			//19

//20,BOOT1=L(GPIOB,GPIO_Pin_2)
#define STBY_PORT			GPIOB
#define MAIN_STBY				GPIO_Pin_2			//20

//for UART3 (x)
//#define GPIOx				GPIOB
//#define RCC_APB2Periph_GPIOx	RCC_APB2Periph_GPIOB
//#define GPIO_Tx3Pin				GPIO_Pin_10			//21
//#define GPIO_Rx3Pin				GPIO_Pin_11			//22
#define TC2002A_D2				GPIO_Pin_10			//21
#define TC2002A_D3				GPIO_Pin_11			//22

//23,VSS-1
//24,VDD-1

//for vfd port
#define PORT_PORTB			GPIOB
#define TC2002A_D4				GPIO_Pin_12			//25
#define TC2002A_D5				GPIO_Pin_13			//26
#define TC2002A_D6				GPIO_Pin_14			//27
#define TC2002A_D7				GPIO_Pin_15			//28

#define DAC_PORTA			GPIOA
#define WM8740_ML				GPIO_Pin_8			//29

//for UART1 (main MCU)
//#define GPIOx				GPIOA
//#define RCC_APB2Periph_GPIOx	RCC_APB2Periph_GPIOA
#define GPIO_Tx1Pin				GPIO_Pin_9			//30
#define GPIO_Rx1Pin				GPIO_Pin_10			//31

//
#define DAC_PORTA			GPIOA
#define WM8740_mC				GPIO_Pin_11			//32
#define WM8740_mD				GPIO_Pin_12			//33

//34,JTMS(GPIOA,GPIO_Pin_13)

//35,VSS-2
//36,VDD-2

//37,JTCK(GPIOA,GPIO_Pin_14)
//38,JTDI(GPIOA,GPIO_Pin_15)

//39,JTDO(GPIOB,GPIO_Pin_3)
//40,JNTRST(GPIOB,GPIO_Pin_4)

#define MUTE_PORT			GPIOB
#define MAIN_MUTE				GPIO_Pin_5			//41

#define IIC_PORT			GPIOB
#define IIC_SCL					GPIO_Pin_6			//42
#define IIC_SDA					GPIO_Pin_7			//43 : input
	
//44,BOOT0
//
#define TC2002A_D0				GPIO_Pin_8			//45
#define TC2002A_D1				GPIO_Pin_9			//46

//47,VSS-3
//48,VDD-3

	
/****************************************************************************************/
//for printf
#define  PRINTx					USART1

/****************************************************************************************/
/* macro definition */


//2,3,4,15 STN LCD
#define TC2002A_RS(x)		( (x==1)? GPIO_SetBits(PORT_PORTC, TC2002A_rs) : GPIO_ResetBits(PORT_PORTC, TC2002A_rs) );
#define TC2002A_RW(x)		( (x==1)? GPIO_SetBits(TC2002A_PORT, TC2002A_rw) : GPIO_ResetBits(TC2002A_PORT, TC2002A_rw) );
#define TC2002A_E(x)		( (x==1)? GPIO_SetBits(PORT_PORTC, TC2002A_e) : GPIO_ResetBits(PORT_PORTC, TC2002A_e) );
#define TC2002A_BRT(x)		( (x==1)? GPIO_SetBits(PORT_PORTA, TC2002A_brt) : GPIO_ResetBits(PORT_PORTA, TC2002A_brt) );
//45,46,21,22,25,26,27,28 STN LCD Bus[PB8~15]
//BUS inputPort :
//GPIO_Mode_IPU	//GPIO_Mode_IN_FLOATING
#define SetBusInput		{\
							GPIO_InitTypeDef GPIO_InitStructure;;\
							GPIO_InitStructure.GPIO_Pin = TC2002A_D7|TC2002A_D6|TC2002A_D5|TC2002A_D4|TC2002A_D3|TC2002A_D2|TC2002A_D1|TC2002A_D0;\
							GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;\
							GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;\
							GPIO_Init(GPIOB, &GPIO_InitStructure);\
						};

//BUS outputPort : GPIO_Mode_AF_PP : GPIO_Mode_Out_PP
#define ResBusOutput	{\
							GPIO_InitTypeDef GPIO_InitStructure;;\
							GPIO_InitStructure.GPIO_Pin = TC2002A_D7|TC2002A_D6|TC2002A_D5|TC2002A_D4|TC2002A_D3|TC2002A_D2|TC2002A_D1|TC2002A_D0;\
							GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;\
							GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;\
							GPIO_Init(GPIOB, &GPIO_InitStructure);\
						};
//readInput : 16Bits
//#define GetInputTC2002A_u16BUS		GPIO_ReadInputData(GPIOB)
//readOutput : 16Bits
//#define GetOutputTC2002A_u16BUS		GPIO_ReadOutputData(GPIOB)
//writeOutput : 16Bits
#define WriteTC2002A_u16BUS(nnnn)	GPIO_Write(GPIOB, ((GPIO_ReadOutputData(GPIOB))&0x00ff)|(nnnn&0xff00))

//11 power down
#define GetPowerDown			GPIO_ReadInputDataBit(POWERDOWN_PORT, POWER_DOWN)

//3 T12V in
#define GetT12V_IN				GPIO_ReadInputDataBit(T12V_PORT, T12V_IN)

//15 main mute
#define Main_nMute(x) 		( (x==1)? GPIO_SetBits(MUTE_PORT, MAIN_MUTE) : GPIO_ResetBits(MUTE_PORT, MAIN_MUTE) );

//16 rmc
#define p_rmc					GPIO_ReadInputDataBit(FRONT_RMC, p_Rmc)

//17 ext rmc
#define p_rmc_ext				GPIO_ReadInputDataBit(eRMC_PORT, p_Rmc_Ext)

//18 MPEG reset
#define MPEG_nRESET(x) 		( (x==1)? GPIO_SetBits(PORT_PORTB, nMPEG_RST) : GPIO_ResetBits(PORT_PORTB, nMPEG_RST) );

//20 standby
#define Standby(x)	 		( (x==1)? GPIO_SetBits(STBY_PORT, MAIN_STBY) : GPIO_ResetBits(STBY_PORT, MAIN_STBY) );

//19,29,32,33 WM8740
#define WM8740_nReset(x)	( (x==1)? GPIO_SetBits(DAC_PORT, WM8740_RESET) : GPIO_ResetBits(DAC_PORT, WM8740_RESET) );
#define WM8740_nML(x)		( (x==1)? GPIO_SetBits(DAC_PORTA, WM8740_ML) : GPIO_ResetBits(DAC_PORTA, WM8740_ML) );
#define WM8740_MD(x) 		( (x==1)? GPIO_SetBits(DAC_PORTA, WM8740_mD) : GPIO_ResetBits(DAC_PORTA, WM8740_mD) );
#define WM8740_MC(x) 		( (x==1)? GPIO_SetBits(DAC_PORTA, WM8740_mC) : GPIO_ResetBits(DAC_PORTA, WM8740_mC) );

//41 LED
#define StandbyLED(x) 		( (x==1)? GPIO_SetBits(GPIO_LED, STANDBY_LED) : GPIO_ResetBits(GPIO_LED, STANDBY_LED) );

//42,43 eeprom
#define p_i2c_scl(x) 		( (x==1)? GPIO_SetBits(IIC_PORT, IIC_SCL) : GPIO_ResetBits(IIC_PORT, IIC_SCL) );
#define p_i2c_sda(x) 		( (x==1)? GPIO_SetBits(IIC_PORT, IIC_SDA) : GPIO_ResetBits(IIC_PORT, IIC_SDA) );

//IIC
#define SetSDA				{\
								GPIO_InitTypeDef GPIO_InitStructure;;\
								GPIO_InitStructure.GPIO_Pin = IIC_SDA;\
								GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;\
								GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;\
								GPIO_Init(IIC_PORT, &GPIO_InitStructure);\
							};


#define ResSDA				{\
								GPIO_InitTypeDef GPIO_InitStructure;;\
								GPIO_InitStructure.GPIO_Pin = IIC_SDA;\
								GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;\
								GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;\
								GPIO_Init(IIC_PORT, &GPIO_InitStructure);\
							};

#define ResSDAA             {\
								GPIO_InitTypeDef GPIO_InitStructure;;\
								GPIO_InitStructure.GPIO_Pin = IIC_SDA;\
								GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;\
								GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;\
								GPIO_Init(IIC_PORT, &GPIO_InitStructure);\
							};

#define GetSDA				GPIO_ReadInputDataBit(IIC_PORT, IIC_SDA)
#define SetSCL				GPIO_SetBits(IIC_PORT, IIC_SCL)
#define ResSCL				GPIO_ResetBits(IIC_PORT, IIC_SCL)
#define GetSCL				GPIO_ReadOutputDataBit(IIC_PORT, IIC_SCL)

/****************************************************************************************/

#if 0
//DAC AK4353
#define DACReset(x) 		( (x==1)? GPIO_SetBits(DAC_PORT, DAC_RESET) : GPIO_ResetBits(DAC_PORT, DAC_RESET) );
#define p_DACi2c_sda(x) 	( (x==1)? GPIO_SetBits(DAC_PORT, DAC_SDA) : GPIO_ResetBits(DAC_PORT, DAC_SDA) );
#define p_DACi2c_scl(x) 	( (x==1)? GPIO_SetBits(DAC_PORTC, DAC_SCL) : GPIO_ResetBits(DAC_PORTC, DAC_SCL) );

#define SetDACSDA			{\
								GPIO_InitTypeDef GPIO_InitStructure;;\
								GPIO_InitStructure.GPIO_Pin = DAC_SDA;\
								GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;\
								GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;\
								GPIO_Init(DAC_PORT, &GPIO_InitStructure);\
							};


#define ResDACSDA			{\
								GPIO_InitTypeDef GPIO_InitStructure;;\
								GPIO_InitStructure.GPIO_Pin = DAC_SDA;\
								GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;\
								GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;\
								GPIO_Init(DAC_PORT, &GPIO_InitStructure);\
							};

#define GetDACSDA			GPIO_ReadInputDataBit(DAC_PORT, DAC_SDA)

#endif /* if 0 */
/****************************************************************************************/

#endif	//RCD12_64PIN

#endif 	//__PORT_DEF__

/* end of file */
/******************* (C) COPYRIGHT 2008 Rotel Technology Global **************/

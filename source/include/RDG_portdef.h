/**************** (c)2008 Rotel Technology Global Inc..*************************
PROJECT         : Front Panel for Large VFD
COMPILER        : IAR Embeded Workbench 4.4.1
MODULE          : RDG_portdef.h
VERSION         : V 1.0
USED CHIP       : stm32f10x
CREATION DATE   : /2008
AUTHORS         : An Kyung Chan / RTG
DESCRIPTION     :
*******************************************************************************/
#ifndef __PORT_DEF__
#define __PORT_DEF__

//1,VBAT

//for power-down
#define POWERDOWN_PORT			GPIOC
#define POWER_DOWN				GPIO_Pin_13			//2	

//for option port
#define PROJ_PORT				GPIOC
#define PROJ_RT09				GPIO_Pin_14			//3,RDG=L

//for rmc in port
#define RMC_PORT				GPIOC
#define p_Rmc_Ext				GPIO_Pin_15			//4

//5,OSC-in(4MHz)
//6,OSC-out
//7,NRST

//for ADC port
#define ADC_PORT				GPIOC
#define ADC_IN0					GPIO_Pin_0			//8
#define ADC_IN1					GPIO_Pin_1			//9
#define ADC_IN2					GPIO_Pin_2			//10
#define ADC_IN3					GPIO_Pin_3			//11

//12,VSSA
//13,VDDA

//for front LED
#define GPIO_LED				GPIOA    
#define STANDBY_LED				GPIO_Pin_0			//14

//for front Micom Reset
#define FRONT_RMC				GPIOA    
#define p_Rmc					GPIO_Pin_1			//15

//for UARTx (UART2->Telechips)
//#define GPIOx					GPIOA
//#define RCC_APB2Periph_GPIOx	RCC_APB2Periph_GPIOA
#define GPIO_Tx2Pin				GPIO_Pin_2			//16
#define GPIO_Rx2Pin				GPIO_Pin_3			//17
	
//18,VAA-4
//19,VDD-4

//for vfd port
#define VFD_PORT				GPIOA
#define VFD_CLOCK				GPIO_Pin_4			//20
#define VFD_DATA				GPIO_Pin_5			//21	
#define VFD_LAT					GPIO_Pin_6			//22				
#define VFD_BK					GPIO_Pin_7			//23 : Blanking(1), DisplayOn(0)

//for encoder port
#define ENC_PORT				GPIOC
#define ENC_0					GPIO_Pin_4			//24				
#define ENC_1					GPIO_Pin_5			//25

//for i2s switching port
#define i2s_PORT				GPIOB
#define iPOD_i2s_SELECT			GPIO_Pin_0			//26,stingray_i2s=L
//for stingray port
#define GPIO_STINGRAY			GPIOB
#define STR_nRESET				GPIO_Pin_1			//27

//28,BOOT1=L(GPIOB,GPIO_Pin_2)

//for UARTx (UART3->Stingray)
//#define GPIOx					GPIOB
//#define RCC_APB2Periph_GPIOx	RCC_APB2Periph_GPIOB
#define GPIO_Tx3Pin				GPIO_Pin_10			//29
#define GPIO_Rx3Pin				GPIO_Pin_11			//30

//31,VSS-1
//32,VDD-1

//for 4114 switching port
#define AK4114_PORT				GPIOB
#define p_4114_Int1				GPIO_Pin_12			//33
#define p_4114_Int0				GPIO_Pin_13			//34
#define p_4114_CS				GPIO_Pin_14			//35
#define p_4114_CLK				GPIO_Pin_15			//36

//for 4114 switching port
#define AK4114_PORTC			GPIOC
#define p_4114_DO				GPIO_Pin_6			//37
#define p_4114_DI				GPIO_Pin_7			//38
#define p_4114_RESET			GPIO_Pin_8			//39

//for DAC port
#define DAC_PORT				GPIOC
#define WM8740_RESET			GPIO_Pin_9			//40
//for DAC port
#define DAC_PORTA				GPIOA
#define WM8740_ML				GPIO_Pin_8			//41

//for UARTx (UART1)
//#define GPIOx					GPIOA
//#define RCC_APB2Periph_GPIOx	RCC_APB2Periph_GPIOA
#define GPIO_Tx1Pin				GPIO_Pin_9			//42
#define GPIO_Rx1Pin				GPIO_Pin_10			//43

//for main mute port
#define MAIN_PORT				GPIOA
#define MAIN_MUTE				GPIO_Pin_11			//44
#define MAIN_STBY				GPIO_Pin_12			//45

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
#define MAIN_PORTC				GPIOC								//20091119
#define STANDBY2				GPIO_Pin_10			//51			//standby2=RDGV2
#define ANTENNA					GPIOC
#define ANTENNA_B				GPIO_Pin_11			//52

//for iPod port
#define iPOD_PORTC				GPIOC
#define iPOD_RESET				GPIO_Pin_12			//53,(Low active)

//for iPod port
#define iPOD_PORTD				GPIOD
#define iPOD_UPGL				GPIO_Pin_2			//54
//for Proj 1Watt option [ H: old, L: new ]
#define PROJ_PORTD				GPIOD
#define PROJ_1WATT				GPIO_Pin_2			//54			//2009.10.28

//55,JTDO(GPIOB,GPIO_Pin_3)
//56,JNTRST(GPIOB,GPIO_Pin_4)

//for Trigger 12V port
#define T12V_PORT				GPIOB
#define T12V_IN					GPIO_Pin_5			//57

//for eeprom port
#define IIC_PORT				GPIOB
#define IIC_SCL					GPIO_Pin_6			//58
#define IIC_SDA					GPIO_Pin_7			//59
	
//60,BOOT0
//61,4114 Digital(High), Analog(Low)
#define EXTCLK_PORT				GPIOB
#define EXTCLK_DIGITAL			GPIO_Pin_8			//61
//62,STM32 mpu reset(High active)
#define RESET_PORT				GPIOB
#define RESET_ENABLE			GPIO_Pin_9			//62
//63,VSS-3
//64,VDD-3

	
/****************************************************************************************/
//for printf
#define  PRINTx					USART1

/****************************************************************************************/
/* macro definition */

//power down
#define GetPowerDown			GPIO_ReadInputDataBit(POWERDOWN_PORT, POWER_DOWN)
//optin RT09
#define GetOptionRT09			GPIO_ReadInputDataBit(PROJ_PORT, PROJ_RT09)
//ext rmc
#define p_rmc_ext				GPIO_ReadInputDataBit(RMC_PORT, p_Rmc_Ext)
//ext rmc
#define p_rmc					GPIO_ReadInputDataBit(FRONT_RMC, p_Rmc)
//LED
#define StandbyLED(x) 			( (x==1)? GPIO_SetBits(GPIO_LED, STANDBY_LED) : GPIO_ResetBits(GPIO_LED, STANDBY_LED) );
//VFD
#define VFD_Clock(x)			( (x==1)? GPIO_SetBits(VFD_PORT, VFD_CLOCK) : GPIO_ResetBits(VFD_PORT, VFD_CLOCK) );
#define VFD_Data(x)				( (x==1)? GPIO_SetBits(VFD_PORT, VFD_DATA) : GPIO_ResetBits(VFD_PORT, VFD_DATA) );
#define VFD_Latch(x)			( (x==1)? GPIO_SetBits(VFD_PORT, VFD_LAT) : GPIO_ResetBits(VFD_PORT, VFD_LAT) );
#define VFD_Bk(x)				( (x==1)? GPIO_SetBits(VFD_PORT, VFD_BK) : GPIO_ResetBits(VFD_PORT, VFD_BK) );
//i2s select(iPod & Stingray)
#define i2s_iPod_nSTM(x)		( (x==1)? GPIO_SetBits(i2s_PORT, iPOD_i2s_SELECT) : GPIO_ResetBits(i2s_PORT, iPOD_i2s_SELECT) );
//stingray reset
#define STRnRESET(x) 			( (x==1)? GPIO_SetBits(GPIO_STINGRAY, STR_nRESET) : GPIO_ResetBits(GPIO_STINGRAY, STR_nRESET) );
//4114
#define Get4114INT1				GPIO_ReadInputDataBit(AK4114_PORT, p_4114_Int1)
#define Get4114INT0				GPIO_ReadInputDataBit(AK4114_PORT, p_4114_Int0)
#define p_4114_cs(x) 			( (x==1)? GPIO_SetBits(AK4114_PORT, p_4114_CS) : GPIO_ResetBits(AK4114_PORT, p_4114_CS) );
#define p_4114_clk(x) 			( (x==1)? GPIO_SetBits(AK4114_PORT, p_4114_CLK) : GPIO_ResetBits(AK4114_PORT, p_4114_CLK) );
//
#define p_4114_do(x) 			( (x==1)? GPIO_SetBits(AK4114_PORTC, p_4114_DO) : GPIO_ResetBits(AK4114_PORTC, p_4114_DO) );
#define Get4114di				GPIO_ReadInputDataBit(AK4114_PORTC, p_4114_DI)
#define p_4114_reset(x) 		( (x==1)? GPIO_SetBits(AK4114_PORTC, p_4114_RESET) : GPIO_ResetBits(AK4114_PORTC, p_4114_RESET) );

//WM8740
#define WM8740_nReset(x)		( (x==1)? GPIO_SetBits(DAC_PORT, WM8740_RESET) : GPIO_ResetBits(DAC_PORT, WM8740_RESET) );
#define WM8740_nML(x)			( (x==1)? GPIO_SetBits(DAC_PORTA, WM8740_ML) : GPIO_ResetBits(DAC_PORTA, WM8740_ML) );
#define WM8740_MD(x) 			( (x==1)? GPIO_SetBits(AK4114_PORTC, p_4114_DO) : GPIO_ResetBits(AK4114_PORTC, p_4114_DO) );
#define WM8740_MC(x) 			( (x==1)? GPIO_SetBits(AK4114_PORT, p_4114_CLK) : GPIO_ResetBits(AK4114_PORT, p_4114_CLK) );
//main mute
#define Main_nMute(x) 			( (x==1)? GPIO_SetBits(MAIN_PORT, MAIN_MUTE) : GPIO_ResetBits(MAIN_PORT, MAIN_MUTE) );
//RDG=standbyT12V, RDGV2=T12V
#define Standby(x)	 			( (x==1)? GPIO_SetBits(MAIN_PORT, MAIN_STBY) : GPIO_ResetBits(MAIN_PORT, MAIN_STBY) );
//iPod
#define iPod_nRESET(x) 			( (x==1)? GPIO_SetBits(iPOD_PORTC, iPOD_RESET) : GPIO_ResetBits(iPOD_PORTC, iPOD_RESET) );
//#define iPod_UPGL(x) 			( (x==1)? GPIO_SetBits(iPOD_PORTD, iPOD_UPGL) : GPIO_ResetBits(iPOD_PORTD, iPOD_UPGL) );
//optin Under1Watt model
#define GetOptionUnder1Watt		GPIO_ReadInputDataBit(PROJ_PORTD, PROJ_1WATT)
//T12V out : 20091119
#define Standby2(x)	 			( (x==1)? GPIO_SetBits(MAIN_PORTC, STANDBY2) : GPIO_ResetBits(MAIN_PORTC, STANDBY2) );
//T12V in
#define GetT12V_IN				GPIO_ReadInputDataBit(T12V_PORT, T12V_IN)
//Antenna A/B
//#define Antenna_A(x) 			( (x==1)? GPIO_SetBits(ANTENNA, ANTENNA_A) : GPIO_ResetBits(ANTENNA, ANTENNA_A) );
#define Antenna_aB(x) 			( (x==1)? GPIO_SetBits(ANTENNA, ANTENNA_B) : GPIO_ResetBits(ANTENNA, ANTENNA_B) );
//eeprom
#define p_i2c_sda(x) 			( (x==1)? GPIO_SetBits(IIC_PORT, IIC_SDA) : GPIO_ResetBits(IIC_PORT, IIC_SDA) );
#define p_i2c_scl(x) 			( (x==1)? GPIO_SetBits(IIC_PORT, IIC_SCL) : GPIO_ResetBits(IIC_PORT, IIC_SCL) );
//EXT Clock disable
#define ExtClockDisable(x)		( (x==1)? GPIO_SetBits(EXTCLK_PORT, EXTCLK_DIGITAL) : GPIO_ResetBits(EXTCLK_PORT, EXTCLK_DIGITAL) );
//Reset enable
#define STM32ResetEnable(x)	( (x==1)? GPIO_SetBits(RESET_PORT, RESET_ENABLE) : GPIO_ResetBits(RESET_PORT, RESET_ENABLE) );

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
#define DACReset(x) 				( (x==1)? GPIO_SetBits(DAC_PORT, DAC_RESET) : GPIO_ResetBits(DAC_PORT, DAC_RESET) );
#define p_DACi2c_sda(x) 			( (x==1)? GPIO_SetBits(DAC_PORT, DAC_SDA) : GPIO_ResetBits(DAC_PORT, DAC_SDA) );
#define p_DACi2c_scl(x) 			( (x==1)? GPIO_SetBits(DAC_PORTC, DAC_SCL) : GPIO_ResetBits(DAC_PORTC, DAC_SCL) );

#define SetDACSDA                  {\
                                    GPIO_InitTypeDef GPIO_InitStructure;;\
	                                GPIO_InitStructure.GPIO_Pin = DAC_SDA;\
                                    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;\
                                    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;\
  	                                GPIO_Init(DAC_PORT, &GPIO_InitStructure);\
                                };


#define ResDACSDA                  {\
                                    GPIO_InitTypeDef GPIO_InitStructure;;\
	                                GPIO_InitStructure.GPIO_Pin = DAC_SDA;\
                                    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;\
                                    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;\
  	                                GPIO_Init(DAC_PORT, &GPIO_InitStructure);\
                                };

#define GetDACSDA					GPIO_ReadInputDataBit(DAC_PORT, DAC_SDA)

#endif /* if 0 */
/****************************************************************************************/



#endif 	//__PORT_DEF__

/* end of file */
/******************* (C) COPYRIGHT 2008 Rotel Technology Global **************/

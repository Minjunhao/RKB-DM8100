/**************** (c)2008 Rotel Technology Global Inc..*************************
PROJECT         : Front Panel for Large VFD
COMPILER        : IAR Embeded Workbench 4.4.1
MODULE          : stingray_portdef.h
VERSION         : V 1.0
USED CHIP       : stm32f10x
CREATION DATE   : /2008
AUTHORS         : An Kyung Chan / RTG
DESCRIPTION     :
*******************************************************************************/
#ifndef __PORT_DEF__
#define __PORT_DEF__

//Stingray
//for power-down
#define POWERDOWN_PORT			GPIOC
#define POWER_DOWN				GPIO_Pin_13	

//for option port
#define PROJ_PORT					GPIOC
#define PROJ_OPT1					GPIO_Pin_14	

//for rmc in port
#define RMC_PORT					GPIOC
#define RMC_IN						GPIO_Pin_15

//for ADC port
#define ADC_PORT					GPIOC
#define ADC_IN0					GPIO_Pin_0
#define ADC_IN1					GPIO_Pin_1
#define ADC_IN2					GPIO_Pin_2
#define ADC_IN3					GPIO_Pin_3

//for front LED
#define GPIO_LED					GPIOA    
#define STANDBY_LED				GPIO_Pin_0

//for front Micom Reset
#define FRONT_PORT				GPIOA    
#define FRONT_RST					GPIO_Pin_1

//for tuner port
#define RDS_PORT					GPIOA
#define RDS_CLOCK					GPIO_Pin_4				//falling edge, ?Interrupt
#define RDS_DATA					GPIO_Pin_5	

//for tuner port
#define PLL_PORTA					GPIOA
#define PLL_CS						GPIO_Pin_6				
#define PLL_DIN					GPIO_Pin_7	

//for tuner port
#define PLL_PORTC					GPIOC
#define PLL_CLOCK					GPIO_Pin_4				
#define PLL_DOUT					GPIO_Pin_5	

//for tuner port
#define GPIO_TUNER				GPIOB
#define TUNER_ST					GPIO_Pin_0				
#define TUNER_TUNED				GPIO_Pin_1
#define TUNER_EMPHA				GPIO_Pin_12
#define TUNER_MUTE				GPIO_Pin_13

//for encoder
#define VOL_PORT					GPIOB
#define VOL_UP						GPIO_Pin_14
#define VOL_DOWN					GPIO_Pin_15
//for DAC port
#define DAC_PORT					GPIOB
#define DAC_RESET					GPIO_Pin_14		
#define DAC_SDA					GPIO_Pin_15
//for DAC port
#define DAC_PORTC					GPIOC
#define DAC_SCL					GPIO_Pin_6

//for mute port
#define MUTE_PORT					GPIOC
#define MUTE_SPDIF				GPIO_Pin_7
	
//for select port
#define SEL_PORT					GPIOC
#define SEL_STINGRAY				GPIO_Pin_8

//for eeprom port
#define IIC_PORTC					GPIOC
#define IIC_SDA					GPIO_Pin_9
//for eeprom port
#define IIC_PORTA					GPIOA
#define IIC_SCL					GPIO_Pin_8

//for stingray port
#define GPIO_STINGRAY			GPIOA
#define STR_WAKEUP				GPIO_Pin_11				//stingray wakeup
#define STR_nRESET				GPIO_Pin_12				//stingray nReset

//for Trigger 12V port
#define T12V_PORT					GPIOB
#define T12V_OUT					GPIO_Pin_5
#define T12V_IN					GPIO_Pin_6

//for iPod port
#define IPOD_PORT					GPIOB
#define IPOD_RESET				GPIO_Pin_7

//for sw upgrade port
#define SWUP_PORT					GPIOB
#define SWUP_FRONT				GPIO_Pin_8

//for main mute port
#define MAIN_PORT					GPIOB
#define MAIN_MUTE					GPIO_Pin_9
	
	
//for UARTx (UART1)
#define  GPIOx                  GPIOA
#define  RCC_APB2Periph_GPIOx   RCC_APB2Periph_GPIOA
#define  GPIO_TxPin             GPIO_Pin_9
#define  GPIO_RxPin             GPIO_Pin_10

//for printf
#define  PRINTx					USART1


/* macro definition */
#define StandbyLED(x) 			( (x==1)? GPIO_SetBits(GPIO_LED, STANDBY_LED) : GPIO_ResetBits(GPIO_LED, STANDBY_LED) );
#define T12VOut(x)		 		( (x==1)? GPIO_SetBits(T12V_PORT, T12V_OUT) : GPIO_ResetBits(T12V_PORT, T12V_OUT) );
//#define TestLED(x)	 			( (x==1)? GPIO_SetBits(FRONT_PORT, FRONT_RST) : GPIO_ResetBits(FRONT_PORT, FRONT_RST) );				//test PA1
#define STRnRESET(x) 			( (x==1)? GPIO_SetBits(GPIO_STINGRAY, STR_nRESET) : GPIO_ResetBits(GPIO_STINGRAY, STR_nRESET) );
#define WAKEUP(x) 				( (x==1)? GPIO_SetBits(GPIO_STINGRAY, STR_WAKEUP) : GPIO_ResetBits(GPIO_STINGRAY, STR_WAKEUP) );
#define p_i2c_sda(x) 			( (x==1)? GPIO_SetBits(IIC_PORTC, IIC_SDA) : GPIO_ResetBits(IIC_PORTC, IIC_SDA) );
#define p_i2c_scl(x) 			( (x==1)? GPIO_SetBits(IIC_PORTA, IIC_SCL) : GPIO_ResetBits(IIC_PORTA, IIC_SCL) );

#define SetSDA                  {\
                                    GPIO_InitTypeDef GPIO_InitStructure;;\
	                                GPIO_InitStructure.GPIO_Pin = IIC_SDA;\
                                    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;\
                                    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;\
  	                                GPIO_Init(IIC_PORTC, &GPIO_InitStructure);\
                                };


#define ResSDA                  {\
                                    GPIO_InitTypeDef GPIO_InitStructure;;\
	                                GPIO_InitStructure.GPIO_Pin = IIC_SDA;\
                                    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;\
                                    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;\
  	                                GPIO_Init(IIC_PORTC, &GPIO_InitStructure);\
                                };

#define GetSDA					GPIO_ReadInputDataBit(IIC_PORTC, IIC_SDA)


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




#endif 	//__PORT_DEF__

/* end of file */
/******************* (C) COPYRIGHT 2008 Rotel Technology Global **************/

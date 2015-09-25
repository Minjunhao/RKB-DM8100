/**************** (c)2012 Rotel Technology Global Inc..*************************
PROJECT         : RKB-850,8100,D850,D8100
COMPILER        : IAR Embeded Workbench 5.50
MODULE          : RKB_initmcu.c
VERSION         : V 1.0
USED CHIP       : RKB (stm32f101RC) [T=36pin,C=48pin,R=64pin,V=100pin : 6=32KB/6KB,8=64KB/10KB,B=128KB/16KB,C=256KB/32KB]
CREATION DATE   : /2013
AUTHORS         : /Rotel
DESCRIPTION     : Used Interrupt function
					1) ADC+DMA channel 1 	:: for check 6channel key data
					2) Systick Timer		:: for timer per 1ms
					3) Timer 1				:: for timer per 300us
					4) UART 1
					
*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "stm32f2xx.h"
#include "config.h"
#include "adckey.h"
#include "netconf.h"
#include "main.h"
#include "initmcu.h"

//#define	STM32_64PIN_ENABLE			//RKB
#ifdef _ETHERNET
#include "stm32f2x7_eth.h"
__IO uint32_t  EthInitStatus = 0;
__IO uint8_t EthLinkStatus = 0;
#endif

u8 mode_AHB_prescaler;
u8 rkb_mode_id;
//#define	STM32_100PIN_ENABLE
/*----------------------------------------------------------------------------*/
#define _MN_ "INIT_MCU: "
/*-----------------------------------------------------------------------------
ROUTINE NAME : ini_DisableInterrupt
INPUT        :
OUTPUT       :
DESCRIPTION  : disable IRQ & Each interrupt Function
COMMENTS     : 
-----------------------------------------------------------------------------*/


void ini_DisableInterrupt(void)
{
	RCC_DeInit();
  	NVIC_DeInit();
	//NVIC_SCBDeInit();
	
	//ADC_DeInit(ADC1);
	ADC_DeInit();
	GPIO_DeInit(GPIOA);
	GPIO_DeInit(GPIOB);
	GPIO_DeInit(GPIOC);
	GPIO_DeInit(GPIOD);
#ifdef _STM32_F2XX_144PIN
    GPIO_DeInit(GPIOE);
    GPIO_DeInit(GPIOF);
	GPIO_DeInit(GPIOG);
#endif
#ifdef STM32_100PIN_ENABLE	
	GPIO_DeInit(GPIOE);				//100pin
#endif	
	//GPIO_AFIODeInit();
	USART_DeInit(USART1);           //RS232     [115200,n,8,1]      

#ifdef UART2_ENABLE	
	USART_DeInit(USART2);           //
#endif	

#ifdef UART3_ENABLE	
	USART_DeInit(USART3);           //
#endif	

#ifdef UART4_ENABLE	
	USART_DeInit(UART4);			//
#endif	
#ifdef UART5_ENABLE	
	USART_DeInit(UART5);			//
#endif	

#ifdef UART6_ENABLE	
	USART_DeInit(USART6);           //not used
#endif	

	//DMA_DeInit(DMA1_Channel1);
	TIM_DeInit(TIM1);				//50usec : audio signal sensing
//20110824
#ifdef TIMER2_ENABLE	            //not used ?VFD
	TIM_DeInit(TIM2);
#endif

#ifdef _ETHERNET
    SYSCFG_DeInit();
    ETH_DeInit();
#endif
}
#ifdef _IWDG_ENABLE
/*-----------------------------------------------------------------------------
 Function Name  : IWDG_Configuration
 Description    : Configures Vector Table base location.
 Input          : None
 Output         : None
 Return         : None
 Comments	 	: ..
-----------------------------------------------------------------------------*/
void IWDG_Configuration(void)
{
#ifdef _IWDG_ENABLE
	RCC_LSICmd(ENABLE);
	while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET) {};
	// Enable write access to IWDG_PR and IWDG_RLR registers
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	// IWDG Prescaler value :
	//	IWDG_Prescaler_4
	//	IWDG_Prescaler_8
	//	IWDG_Prescaler_16
	//	IWDG_Prescaler_32
	//	IWDG_Prescaler_64
	//	IWDG_Prescaler_128
	//	IWDG_Prescaler_256
	// IWDG counter clock : LSI(32KHz)/32 = 1.0KHz (1.0msec)
	// IWDG counter clock : LSI(32KHz)/64 = 500Hz (2.0msec)
	IWDG_SetPrescaler(IWDG_Prescaler_32);
	// Set counter reload value(0~0x0FFF) to obtain ?sec IWDG Timeout
	// = 2sec/(LSI/32)
	IWDG_SetReload(2000);		//(Max 4095)
	// Reload IWDG counter
	//(0xAAAA)
	IWDG_ReloadCounter();
	// Enable IWDG(the LSI oscillator will be enabled by hardware)
	//(0xCCCC)
	IWDG_Enable();
	// for debugging stop IWDG
	//DBGMCU_APB1PeriphConfig(DBGMCU_IWDG_STOP,ENABLE);
#endif
}
#endif //IWDG
/*-----------------------------------------------------------------------------
 Function Name  : ini_InitMcu
 Description    : Initialize the MCU
 Input          : None
 Output         : None
 Return         : None
 Comments	 	: ..
-----------------------------------------------------------------------------*/
void ini_InitMcu(void)
{
	ini_DisableInterrupt();
  	/* System Clocks Configuration *******************************************/
  	RCC_Configuration();
#ifdef _RKB_DM8100
	GPIO_Configuration(1); 

    UART_Configuration();

	ADC_Configuration(1);

	TIM1_Configuration();           //50usec: audio signal sensing

	SYSTICK_Configuration();

	NVIC_Configuration();

#else
	//--------------------------------------------
	//define : pin53 : PC12 : ( input ) : for PP/MP and WS1/2 option
	//--------------------------------------------
  	GPIO_InitTypeDef GPIO_InitStructure;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  	/* configure RKB_mode_PP_MP(PC12) as input pull-up */
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  	GPIO_Init(GPIOC, &GPIO_InitStructure);
	//
	//--------------------------------------------
	if (RKB_mode_PP_MP == 0) {							//?(pin53:PC12): WS1/2=0, PP=1(add SMPS_sensor_1/2)
  		GPIO_Configuration(1);      //20121030 : normal(1)
  	}else{
		if (RKB_mode_PP_nMP2 == 0) {	//20140523 : add signal sensing feature
			GPIO_Configuration_MP2(1);
		}else{
  			GPIO_Configuration_PP(1);	//20131024 : normal(1)
  		}
  	}
	//--------------------------------------------
	UART_Configuration();
	//--------------------------------------------
	//ADC disable!
	if (RKB_mode_PP_MP == 0) {							//?(pin53:PC12): WS1/2=0, PP=1(add SMPS_sensor_1/2) : 20131024
		ADC_Configuration(0);		//20130709 : 
	}else{
		ADC_Configuration_PP(0);	//20131024 : 
	}
	//--------------------------------------------
	TIM1_Configuration();			//50usec : audio signal sensing
//20110824
#ifdef TIMER2_ENABLE	
	TIM2_Configuration();			//50usec
#endif
	SYSTICK_Configuration();
  	/* NVIC(interrupt) Configuration *****************************************/
  	NVIC_Configuration();
#endif //RKB-DM8100
}
#ifdef _STM32_F2XX_144PIN
/*-----------------------------------------------------------------------------
 Function Name  : RCC_Configuration
 Description    : Configures the different system clocks.
 Input          : None
 Output         : None
 Return         : None
 Comments	 	: ..
-----------------------------------------------------------------------------*/
void RCC_Configuration(void)
{   
	ErrorStatus	 HSEStartUpStatus;

  	/* RCC system reset(for debug purpose) */
  	RCC_DeInit();

  	/* Enable HSE */
  	RCC_HSEConfig(RCC_HSE_ON);
  	
  	/* Wait till HSE is ready */
  	HSEStartUpStatus = RCC_WaitForHSEStartUp();
  	
  	if(HSEStartUpStatus == SUCCESS)
  	{
  	  	/* Enable Prefetch Buffer */
		FLASH_PrefetchBufferCmd(ENABLE);
  	  	
  	  	/* Flash 2 wait state */
  	  	FLASH_SetLatency(FLASH_Latency_2);
 			
  	  	/* AHB = HCLK = SYSCLK 
  	  	//to AHB bus, core,memory and DMA
			This parameter can be one of the following values:
			   - RCC_SYSCLK_Div1: AHB clock = SYSCLK
			   - RCC_SYSCLK_Div2: AHB clock = SYSCLK/2
			   - RCC_SYSCLK_Div4: AHB clock = SYSCLK/4
			   - RCC_SYSCLK_Div8: AHB clock = SYSCLK/8
			   - RCC_SYSCLK_Div16: AHB clock = SYSCLK/16
			   - RCC_SYSCLK_Div64: AHB clock = SYSCLK/64
			   - RCC_SYSCLK_Div128: AHB clock = SYSCLK/128
			   - RCC_SYSCLK_Div256: AHB clock = SYSCLK/256
			   - RCC_SYSCLK_Div512: AHB clock = SYSCLK/512			*/
		//20140408
		switch(mode_AHB_prescaler)
		{ 
			case 1:
				RCC_HCLKConfig(RCC_SYSCLK_Div1);           //120M
				RCC_PCLK1Config(RCC_HCLK_Div4) ;           //120M/4
				RCC_PCLK2Config(RCC_HCLK_Div2);            //120M/2
				break;
			case 2:
				RCC_HCLKConfig(RCC_SYSCLK_Div2);
				RCC_PCLK1Config(RCC_HCLK_Div2);
				RCC_PCLK2Config(RCC_HCLK_Div1);
				break;
			case 4:
				RCC_HCLKConfig(RCC_SYSCLK_Div4);
				RCC_PCLK1Config(RCC_HCLK_Div1);
				RCC_PCLK2Config(RCC_HCLK_Div1);
				break;
			case 8:
				RCC_HCLKConfig(RCC_SYSCLK_Div8);
				RCC_PCLK1Config(RCC_HCLK_Div1);
				RCC_PCLK2Config(RCC_HCLK_Div1);
				break;
			case 16:
				RCC_HCLKConfig(RCC_SYSCLK_Div16);
				RCC_PCLK1Config(RCC_HCLK_Div1);
				RCC_PCLK2Config(RCC_HCLK_Div1);
				break;
			default:
				break;
		}
		
		RCC_SYSCLKConfig(RCC_SYSCLKSource_HSE);
		
		/* Wait till HSI(0),HSE(4),PLL(8) is used as system clock source */
  	  	while(RCC_GetSYSCLKSource() != 0x04)
  	  	{
  	  	}
  	}
  	/* DMA clock enable */

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);

	RCC_AHB1PeriphClockCmd(  RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB
		                   | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD
		                   | RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOF
		                   | RCC_AHB1Periph_GPIOG,  ENABLE);
	//ADC3,SYSCFG
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG | RCC_APB2Periph_ADC3, ENABLE);

  	/* Timer1 */
  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 , ENABLE);

  	/* uart1 */
  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 , ENABLE);	



#ifdef UART2_ENABLE
  	/* uart2 */
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2 , ENABLE);
#endif

#ifdef UART3_ENABLE	
  	/* uart2/3 */
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3 , ENABLE);
#endif

#ifdef UART4_ENABLE
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);					//greenchips
#endif

#ifdef UART5_ENABLE
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);					//greenchips
#endif
//RCX
//20110824
#ifdef TIMER2_ENABLE	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
#endif
}

#else

/*-----------------------------------------------------------------------------
 Function Name  : RCC_Configuration
 Description    : Configures the different system clocks.
 Input          : None
 Output         : None
 Return         : None
 Comments	 	: ..
-----------------------------------------------------------------------------*/
void RCC_Configuration(void)
{   
	ErrorStatus	 HSEStartUpStatus;

  	/* RCC system reset(for debug purpose) */
  	RCC_DeInit();

  	/* Enable HSE */
  	RCC_HSEConfig(RCC_HSE_ON);
  	
  	/* Wait till HSE is ready */
  	HSEStartUpStatus = RCC_WaitForHSEStartUp();
  	
  	if(HSEStartUpStatus == SUCCESS)
  	{
  	  	/* Enable Prefetch Buffer */
        #ifdef _STM32_F2XX_144PIN
		FLASH_PrefetchBufferCmd(ENABLE);
		#else
  	  	FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
		#endif
  	  	
  	  	/* Flash 2 wait state */
  	  	FLASH_SetLatency(FLASH_Latency_2);
 			
  	  	/* AHB = HCLK = SYSCLK 
  	  	//to AHB bus, core,memory and DMA
			This parameter can be one of the following values:
			   - RCC_SYSCLK_Div1: AHB clock = SYSCLK
			   - RCC_SYSCLK_Div2: AHB clock = SYSCLK/2
			   - RCC_SYSCLK_Div4: AHB clock = SYSCLK/4
			   - RCC_SYSCLK_Div8: AHB clock = SYSCLK/8
			   - RCC_SYSCLK_Div16: AHB clock = SYSCLK/16
			   - RCC_SYSCLK_Div64: AHB clock = SYSCLK/64
			   - RCC_SYSCLK_Div128: AHB clock = SYSCLK/128
			   - RCC_SYSCLK_Div256: AHB clock = SYSCLK/256
			   - RCC_SYSCLK_Div512: AHB clock = SYSCLK/512			*/
		//20140408
		switch(mode_AHB_prescaler)
		{ 

			case 2:		RCC_HCLKConfig(RCC_SYSCLK_Div2);		break;
			case 4:		RCC_HCLKConfig(RCC_SYSCLK_Div4);		break;
			case 8:		RCC_HCLKConfig(RCC_SYSCLK_Div8);		break;
			case 16:	RCC_HCLKConfig(RCC_SYSCLK_Div16);		break;
			default:
			case 1:		RCC_HCLKConfig(RCC_SYSCLK_Div1);		break;
		}
  	  	/* APB2 = PCLK2 = HCLK 
  	  	//for peripherals to APB2
			This parameter can be one of the following values:
			   - RCC_HCLK_Div1: APB1 clock = HCLK
			   - RCC_HCLK_Div2: APB1 clock = HCLK/2
			   - RCC_HCLK_Div4: APB1 clock = HCLK/4
			   - RCC_HCLK_Div8: APB1 clock = HCLK/8
			   - RCC_HCLK_Div16: APB1 clock = HCLK/16				*/
  	  	RCC_PCLK2Config(RCC_HCLK_Div1); 
  	  	// APB1 = PCLK1 = HCLK
  	  	//for peripherals to APB1
  	  	RCC_PCLK1Config(RCC_HCLK_Div1);
  	  	
	    /* ADCCLK = PCLK2/4 */
    	//RCC_ADCCLKConfig(RCC_PCLK2_Div4); 

  	  	/* PLLCLK = 4MHz * 9 = 36 MHz */
  	  	RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
  	  	
  	  	/* Enable PLL */ 
  	  	RCC_PLLCmd(ENABLE);
  	  	
  	  	/* Wait till PLL is ready */
  	  	while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
  	  	{
  	  	}
  	  	
  	  	/* Select PLL as system clock source */
  	  	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
  	  	
  	  	/* Wait till PLL is used as system clock source */
  	  	while(RCC_GetSYSCLKSource() != 0x08)
  	  	{
  	  	}
  	}
  	/* DMA clock enable */
	
  	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);


#ifdef STM32_100PIN_ENABLE	
//100pin
  	/* port/uart/adc1/spi1/ timer 1 clock enable AFIO=alternate function io */
  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA  | RCC_APB2Periph_GPIOB |
  	                       RCC_APB2Periph_GPIOC  | RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE |		//100pin
  	                       RCC_APB2Periph_USART1 | RCC_APB2Periph_AFIO  |
  	                       RCC_APB2Periph_TIM1   | RCC_APB2Periph_ADC1 , ENABLE);
  	                       
  	RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOA  | RCC_APB2Periph_GPIOB |
  	                       RCC_APB2Periph_GPIOC  | RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE |		//100pin
  	                       RCC_APB2Periph_USART1 | RCC_APB2Periph_AFIO  |
  	                       RCC_APB2Periph_TIM1   | RCC_APB2Periph_ADC1 , DISABLE);
#endif
#ifdef STM32_64PIN_ENABLE	
//64pin
  	/* port/uart/adc1/spi1/ timer 1 clock enable AFIO=alternate function io */
  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA  | RCC_APB2Periph_GPIOB |
  	                       RCC_APB2Periph_GPIOC  | RCC_APB2Periph_GPIOD |
  	                       RCC_APB2Periph_USART1 | RCC_APB2Periph_AFIO  |
  	                       RCC_APB2Periph_TIM1   | RCC_APB2Periph_ADC1 , ENABLE);
  	                       
  	RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOA  | RCC_APB2Periph_GPIOB |
  	                       RCC_APB2Periph_GPIOC  | RCC_APB2Periph_GPIOD |
  	                       RCC_APB2Periph_USART1 | RCC_APB2Periph_AFIO  |
  	                       RCC_APB2Periph_TIM1   | RCC_APB2Periph_ADC1 , DISABLE);
#endif

#ifdef UART2_ENABLE
  	/* uart2 */
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2 , ENABLE);
#endif

#ifdef UART3_ENABLE	
  	/* uart2/3 */
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3 , ENABLE);
#endif

#ifdef UART4_ENABLE
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);					//greenchips
#endif

#ifdef UART5_ENABLE
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);					//greenchips
#endif
//RCX
//20110824
#ifdef TIMER2_ENABLE	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
#endif
}
#endif
/*-----------------------------------------------------------------------------
 Function Name  : NVIC_Configuration
 Description    : Configures Vector Table base location.
 Input          : None
 Output         : None
 Return         : None
 Comments	 	: ..
-----------------------------------------------------------------------------*/
#ifdef _RKB_DM8100
void NVIC_Configuration(void)
{
   	NVIC_InitTypeDef NVIC_InitStructure;

  	/* Set the Vector Table base location at 0x08000000 */ 
 	//IAP_UPGRADE
  	//NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x2000);   
  	//NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   

  	/* Configure the NVIC Preemption Priority Bits */  
  	/* 0 bit for pre-emption priority, 4 bits for subpriority */
  	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

  	/* Enable the USART1 Interrupt */                           //RS232
  	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);

#ifdef UART2_ENABLE	
  	/* Enable the USART2 Interrupt */                           //
  	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);
#endif
#ifdef UART3_ENABLE	
  	/* Enable the USART3 Interrupt */                           //
  	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);
#endif
#ifdef UART4_ENABLE	
  	/* Enable the USART4 Interrupt */		                    //
  	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);
#endif
#ifdef UART5_ENABLE	
  	/* Enable the USART5 Interrupt */							//
  	NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);
#endif
 	/* 4 bit for pre-emption priority, 1 bits for subpriority */
  	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

	/* Enable the TIM1 gloabal Interrupt */						//TIMER 1
  	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_TIM10_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
 	NVIC_Init(&NVIC_InitStructure);

//20110824
#ifdef TIMER2_ENABLE	
	/* Enable the TIM2 gloabal Interrupt */
  	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
 	NVIC_Init(&NVIC_InitStructure);
#endif
//#ifdef ADC_ENABLEx	
	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);  
//#endif
}

#else
void NVIC_Configuration(void)
{
   	NVIC_InitTypeDef NVIC_InitStructure;

  	/* Set the Vector Table base location at 0x08000000 */ 
 	//IAP_UPGRADE
  	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x2000);   
  	//NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   

  	/* Configure the NVIC Preemption Priority Bits */  
  	/* 0 bit for pre-emption priority, 4 bits for subpriority */
  	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

  	/* Enable the USART1 Interrupt */                           //RS232
  	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQChannel;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);

#ifdef UART2_ENABLE	
  	/* Enable the USART2 Interrupt */                           //
  	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQChannel;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);
#endif
#ifdef UART3_ENABLE	
  	/* Enable the USART3 Interrupt */                           //
  	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQChannel;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);
#endif
#ifdef UART4_ENABLE	
  	/* Enable the USART4 Interrupt */		                    //
  	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQChannel;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);
#endif
#ifdef UART5_ENABLE	
  	/* Enable the USART5 Interrupt */							//
  	NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQChannel;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);
#endif
 	/* 4 bit for pre-emption priority, 1 bits for subpriority */
  	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

	/* Enable the TIM1 gloabal Interrupt */						//TIMER 1
  	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQChannel;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
 	NVIC_Init(&NVIC_InitStructure);

//20110824
#ifdef TIMER2_ENABLE	
	/* Enable the TIM2 gloabal Interrupt */
  	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
 	NVIC_Init(&NVIC_InitStructure);
#endif
}
#endif //_RKB_DM8100
/*-----------------------------------------------------------------------------
 Function Name  : GPIO_Configuration
 Description    : Configures the different GPIO ports.
 Input          : None
 Output         : None
 Return         : None
 Comments	 	: ..
 	normal call 			: GPIO_Configuration(1);
 	power consumption call 	: GPIO_Configuration(0); 
 	
 	added for PP/MP because add ADC2/3 so move PA2/3=>PC10/11
 	added for MP2/signal sensing because add PA13/14/15, PB03 : so need disable JTAG feature
-----------------------------------------------------------------------------*/
//------------------------------------------------------------------------------
//	RKB MP2 for signal sensing feature
//20140523
//------------------------------------------------------------------------------
#ifndef _RKB_DM8100
void GPIO_Configuration_MP2(unsigned char type)
{
  	GPIO_InitTypeDef GPIO_InitStructure;

  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	/*--------------------------*/
	/* GPIO A configuration 	*/
	/*--------------------------*/
  if (type != 0)
	{
    //-(1)--Normal------------------------------------------------------
  	/* configure AD key(PA0~PA3) as analoge in */ 
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;		//20131024
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  	GPIO_Init(GPIOA, &GPIO_InitStructure);

  	/* configure SMPS_nON(PA3),REC_MUTE(PA4),NJW1159_LAT(PA5),NJW1159_CLK(PA6),NJW1159_DAT(PA7),p_4114_D_CS(PA11) as output function push-pull */
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_11; 
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  	GPIO_Init(GPIOA, &GPIO_InitStructure);
  	
  	/* Configure p_4114_D_Int0(PA8),TRIGGER_IN(PA12) as input pull-up */
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_12;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  	GPIO_Init(GPIOA, &GPIO_InitStructure);

#ifdef JTAG_DISABLE //20140523
    // Full SWJ Disabled (JTAG-DP + SW-DP)
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
    // Configure PA.13 (JTMS/SWDAT), PA.14 (JTCK/SWCLK) and PA.15 (JTDI) as input
  	/* Configure SIGNAL_SENSING_nSW(PA13),SIGNAL_SENSING_nMP2(PA14),SIGNAL_SENSING_nIN(PA15) as input pull-up */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
#endif  

#ifdef UART2_ENABLE	
  	/* configure USART2 Rx (PA.3) as input floating */                      //Rx
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  	GPIO_Init(GPIOA, &GPIO_InitStructure);
  	
  	/* configure USART2 Tx (PA.2) as alternate function push-pull */        //Tx
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  	GPIO_Init(GPIOA, &GPIO_InitStructure);
#endif
  	/* configure USART1 Rx (PA.10) as input floating */		//RS232 Rx
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  	GPIO_Init(GPIOA, &GPIO_InitStructure);
  	
  	/* configure USART1 Tx (PA.09) as alternate function push-pull */       //RS232 Tx
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  	GPIO_Init(GPIOA, &GPIO_InitStructure);

  }else{
    //-(0)--power consumption------------------------------------------------------
    //
    //no change SMPS_nON,REC_MUTE,,LED_STBY, mainMute, mainSTBY
    //
  	/* configure AD key(PA0~PA1),NJW1159_LAT(PA5),NJW1159_CLK(PA6),NJW1159_DAT(PA7),p_4114_D_CS(PA11) as output function push-pull */
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 |GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_11; 
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  	GPIO_Init(GPIOA, &GPIO_InitStructure);

  	//20130710 : add p_4114_D_Int0(PA8)
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; 
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  	GPIO_Init(GPIOA, &GPIO_InitStructure);
    
#ifdef JTAG_DISABLE //20121112
    // Full SWJ Disabled (JTAG-DP + SW-DP)
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
    // Configure PA.13 (JTMS/SWDAT), PA.14 (JTCK/SWCLK) and PA.15 (JTDI) as input
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
#endif  
  }

	/*--------------------------*/
	/* GPIO B configuration 	*/
	/*--------------------------*/
  if (type != 0)
	{
    //-(1)--Normal------------------------------------------------------
	/* Configure CH_C_DIGITAL(PB0),CH_D_DIGITAL(PB1),IIC_SCL(PB6),p_4114_RESET(PB10),p_4114_CLK(PB11),p_4114_DO(PB12),p_4114_A_CS(PB15) as output pull-up */
  	GPIO_InitStructure.GPIO_Pin = 	GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_6 | GPIO_Pin_10 | GPIO_Pin_11 |
  	                                GPIO_Pin_12 | GPIO_Pin_15;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
//	/* Configure SDA_4353(PB15) as alternate function push-pull */
//  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
//  	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
//  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//  	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
//	/* Configure POWER_DOWN(PB5),PROJ_8100(PB8),PROJ_DIGITAL(PB9),p_4114_DI(PB13),p_4114_A_Int0(PB14) as input pull-up */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_13 | GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

#ifdef JTAG_DISABLE //20140523
    // Full SWJ Disabled (JTAG-DP + SW-DP)
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
    // Configure PB.03 (JTDO) and PB.04 (JTRST) as input
  	/* Configure FAN_LS_nOFF(PB3) as output pull-up */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
#endif  

#ifdef UART3_ENABLE	
  	/* configure USART3 rx (PB.11) as input floating */
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  	GPIO_Init(GPIOB, &GPIO_InitStructure);
  	
  	/* configure USART3 Tx (PB.10) as alternate function push-pull */       //?not used
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  	GPIO_Init(GPIOB, &GPIO_InitStructure);
#endif  //UART3_ENABLE

  }else{
    //-(0)--power consumption------------------------------------------------------
    //20140224 : delete IIC_SCL(PB6)
	/* Configure CH_C_DIGITAL(PB0),CH_D_DIGITAL(PB1),p_4114_RESET(PB10),p_4114_CLK(PB11),p_4114_DO(PB12),p_4114_A_CS(PB15) as output pull-up */
	// 20130710 add p_4114_DI(PB13),p_4114_A_Int0(PB14)
  	GPIO_InitStructure.GPIO_Pin = 	GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_10 | GPIO_Pin_11 |
  	                                GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  	GPIO_Init(GPIOB, &GPIO_InitStructure);
    
#ifdef JTAG_DISABLE //20121112
    // Full SWJ Disabled (JTAG-DP + SW-DP)
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
    // Configure PB.03 (JTDO) and PB.04 (JTRST) as input
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
#endif  
  }  	

	/*--------------------------*/
	/* GPIO C configuration 	*/
	/*--------------------------*/
  if (type != 0)
	{
    //-(1)--Normal------------------------------------------------------
  	/* configure AD key(PC0~PC3) as analoge in */ 
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  	GPIO_Init(GPIOC, &GPIO_InitStructure);

  	/* configure CH_A_DIGITAL(PC4),CH_B_DIGITAL(PC5),p_4114_B_CS(PC7),p_4114_C_CS(PC9),SMPS_nON(PC11),STANDBY_LED(PC13),MAIN_STBY(PC14),FAN_CTRL(PC15) as output push-pull */
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 |GPIO_Pin_5 |GPIO_Pin_7 | GPIO_Pin_9 | GPIO_Pin_11 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  	GPIO_Init(GPIOC, &GPIO_InitStructure);

  	/* configure p_4114_B_Int0(PC6),p_4114_C_Int0(PC8),RKB_mode_PP_MP(PC12) as input pull-up */
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_8 | GPIO_Pin_12;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  	GPIO_Init(GPIOC, &GPIO_InitStructure);

  	/* configure AMP_nRDY(PC.10) as input floating */		//AMP_nRDY : 20131024
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  	GPIO_Init(GPIOC, &GPIO_InitStructure);
  	
	/* configure POWER DOWN(PC13) as input pull-down */
//  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
//  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
//  	GPIO_Init(GPIOC, &GPIO_InitStructure);

#ifdef UART4_ENABLE	
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);		//greenchips

  	/* configure UART4 Rx (PC.11) as input floating */              //RotelLink [57600,n,8,1]
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  	GPIO_Init(GPIOC, &GPIO_InitStructure);
  	
  	/* configure UART4 Tx (PC.10) as alternate function push-pull */
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  	GPIO_Init(GPIOC, &GPIO_InitStructure);
#endif  //UART4_ENABLE

  }else{
    //-(0)--power consumption------------------------------------------------------
	//20130709 : STANDBY_LED(PC13) delete    
  	/* configure CH_A_DIGITAL(PC4),CH_B_DIGITAL(PC5),p_4114_B_CS(PC7),p_4114_C_CS(PC9),MAIN_STBY(PC14),FAN_CTRL(PC15) as output push-pull */
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 |GPIO_Pin_5 |GPIO_Pin_7 | GPIO_Pin_9 | GPIO_Pin_14 | GPIO_Pin_15;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  	GPIO_Init(GPIOC, &GPIO_InitStructure);

  	/* configure AD key(PC0~3) */
  	/* configure p_4114_B_Int0(PC6),p_4114_C_Int0(PC8),AMP_nRDY(PC10),SMPS_nON(PC11) as input pull-up */
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_6 | GPIO_Pin_8 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  	GPIO_Init(GPIOC, &GPIO_InitStructure);
  }  	

	/*--------------------------*/
	/* GPIO D configuration 	*/
	/* 64pin : PD2 only         */
	/* 100pin : PD0~15          */
	/*--------------------------*/
#ifdef STM32_64PIN_ENABLE	
  if (type != 0)
	{
    //-(1)--Normal------------------------------------------------------
  	/* configure (PD2) as output push-pull */
//  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
//  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//  	GPIO_Init(GPIOD, &GPIO_InitStructure);

  	/* Configure nOverLoad(PD2) as input pull-up */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

  }else{
    //-(0)--power consumption------------------------------------------------------
#ifdef OPTICAL_POWER_ENABLE
	//20140515 : if optical_power=enable , then skip!
	//20140515 : if optical_power=disalbe, then go!
	if (optical_power_mode == OFF) {
		//disable=OFF
  		//20140515 : configure nOverLoad(PD2) as input floating
  		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  		GPIO_Init(GPIOD, &GPIO_InitStructure);
	}
#else    
  	//20130710 : configure nOverLoad(PD2) as input floating
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  	GPIO_Init(GPIOD, &GPIO_InitStructure);
#endif
  }  	
#endif	//STM32_64PIN_ENABLE
#ifdef STM32_100PIN_ENABLE	
  if (type != 0)
	{
    //-(1)--Normal------------------------------------------------------
  	/* configure CMEDIA_USB_CLASS(PD4), CMEDIA_I2C_CLK(PD5) as output push-pull */     //20130108
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  	GPIO_Init(GPIOD, &GPIO_InitStructure);

  	/* configure TunerRelay(PD8),Aux1Relay(PD9),Aux2Relay(PD10),DacOutRelay(PD11),4114Cs(PD14),4114Ck(PD15) as input push-up */
  	GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_14 | GPIO_Pin_15;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  	GPIO_Init(GPIOD, &GPIO_InitStructure);

	/* Configure CMEDIA_SUSPEND(PD3),CMEDIA_INT(PD7),4114Int1(PD12),4114Int0(PD13) as input pull-up */  //20130111
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_7 | GPIO_Pin_12 | GPIO_Pin_13;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  	GPIO_Init(GPIOD, &GPIO_InitStructure);

  	/* Configure CMEDIA_INT(PD7) as input pull-up */
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
//	GPIO_Init(GPIOD, &GPIO_InitStructure);

  }else{
    //-(0)--power consumption------------------------------------------------------
    
  	/* configure CMEDIA_USB_CLASS(PD4), CMEDIA_I2C_CLK(PD5) as output push-pull */     //20130108
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  	GPIO_Init(GPIOD, &GPIO_InitStructure);

  	/* configure CMEDIA_INT(PD7),TunerRelay(PD8),Aux1Relay(PD9),Aux2Relay(PD10),DacOutRelay(PD11),4114Cs(PD14),4114Ck(PD15) as input push-up */
  	GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_14 | GPIO_Pin_15;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  	GPIO_Init(GPIOD, &GPIO_InitStructure);
   
  }  	
#endif	//STM32_100PIN_ENABLE

	/*--------------------------*/
	/* GPIO E configuration 	*/
	/*--------------------------*/
#ifdef STM32_100PIN_ENABLE	
  if (type != 0)
	{
    //-(1)--Normal------------------------------------------------------
  	/* Configure Option(PE4) as input pull-up */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;             //20130201
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

  	/* configure SpeakerA(PE5),SpeakerB(PE6),2311Mute(PE7),2311Cs(PE8),2311Data(PE9),2311Clk(PE10),1194Lh(PE11),1194Data(PE12),1194Clk(PE13),toneBypass(PE14) as input push-up */
  	GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 |
                                    GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  	GPIO_Init(GPIOE, &GPIO_InitStructure);

  }else{
    //-(0)--power consumption------------------------------------------------------
    
  	/* Configure Option(PE4) as input pull-up */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;                 //20130201
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

  	/* configure SpeakerA(PE5),SpeakerB(PE6),2311Mute(PE7),2311Cs(PE8),2311Data(PE9),2311Clk(PE10),1194Lh(PE11),1194Data(PE12),1194Clk(PE13),toneBypass(PE14) as input push-up */
  	GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 |
                                    GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  	GPIO_Init(GPIOE, &GPIO_InitStructure);
    
  }
#endif	//STM32_100PIN_ENABLE  	
}
//------------------------------------------------------------------------------
//	RKB PP
//------------------------------------------------------------------------------
void GPIO_Configuration_PP(unsigned char type)
{
  	GPIO_InitTypeDef GPIO_InitStructure;

  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	/*--------------------------*/
	/* GPIO A configuration 	*/
	/*--------------------------*/
  if (type != 0)
	{
    //-(1)--Normal------------------------------------------------------
  	/* configure AD key(PA0~PA3) as analoge in */ 
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;		//20131024
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  	GPIO_Init(GPIOA, &GPIO_InitStructure);

  	/* configure SMPS_nON(PA3),REC_MUTE(PA4),NJW1159_LAT(PA5),NJW1159_CLK(PA6),NJW1159_DAT(PA7),p_4114_D_CS(PA11) as output function push-pull */
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_11; 
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  	GPIO_Init(GPIOA, &GPIO_InitStructure);
  	
  	/* Configure p_4114_D_Int0(PA8),TRIGGER_IN(PA12) as input pull-up */
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_12;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  	GPIO_Init(GPIOA, &GPIO_InitStructure);

#ifdef UART2_ENABLE	
  	/* configure USART2 Rx (PA.3) as input floating */                      //Rx
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  	GPIO_Init(GPIOA, &GPIO_InitStructure);
  	
  	/* configure USART2 Tx (PA.2) as alternate function push-pull */        //Tx
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  	GPIO_Init(GPIOA, &GPIO_InitStructure);
#endif
  	/* configure USART1 Rx (PA.10) as input floating */		//RS232 Rx
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  	GPIO_Init(GPIOA, &GPIO_InitStructure);
  	
  	/* configure USART1 Tx (PA.09) as alternate function push-pull */       //RS232 Tx
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  	GPIO_Init(GPIOA, &GPIO_InitStructure);

  }else{
    //-(0)--power consumption------------------------------------------------------
    //
    //no change SMPS_nON,REC_MUTE,,LED_STBY, mainMute, mainSTBY
    //
  	/* configure AD key(PA0~PA1),NJW1159_LAT(PA5),NJW1159_CLK(PA6),NJW1159_DAT(PA7),p_4114_D_CS(PA11) as output function push-pull */
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 |GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_11; 
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  	GPIO_Init(GPIOA, &GPIO_InitStructure);

  	//20130710 : add p_4114_D_Int0(PA8)
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; 
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  	GPIO_Init(GPIOA, &GPIO_InitStructure);
    
#ifdef JTAG_DISABLE //20121112
    // Full SWJ Disabled (JTAG-DP + SW-DP)
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
    // Configure PA.13 (JTMS/SWDAT), PA.14 (JTCK/SWCLK) and PA.15 (JTDI) as input
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
#endif  
  }

	/*--------------------------*/
	/* GPIO B configuration 	*/
	/*--------------------------*/
  if (type != 0)
	{
    //-(1)--Normal------------------------------------------------------
	/* Configure CH_C_DIGITAL(PB0),CH_D_DIGITAL(PB1),IIC_SCL(PB6),p_4114_RESET(PB10),p_4114_CLK(PB11),p_4114_DO(PB12),p_4114_A_CS(PB15) as output pull-up */
  	GPIO_InitStructure.GPIO_Pin = 	GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_6 | GPIO_Pin_10 | GPIO_Pin_11 |
  	                                GPIO_Pin_12 | GPIO_Pin_15;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
//	/* Configure SDA_4353(PB15) as alternate function push-pull */
//  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
//  	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
//  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//  	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
//	/* Configure POWER_DOWN(PB5),PROJ_8100(PB8),PROJ_DIGITAL(PB9),p_4114_DI(PB13),p_4114_A_Int0(PB14) as input pull-up */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_13 | GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

#ifdef UART3_ENABLE	
  	/* configure USART3 rx (PB.11) as input floating */
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  	GPIO_Init(GPIOB, &GPIO_InitStructure);
  	
  	/* configure USART3 Tx (PB.10) as alternate function push-pull */       //?not used
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  	GPIO_Init(GPIOB, &GPIO_InitStructure);
#endif  //UART3_ENABLE

  }else{
    //-(0)--power consumption------------------------------------------------------
    //20140224 : delete IIC_SCL(PB6)
	/* Configure CH_C_DIGITAL(PB0),CH_D_DIGITAL(PB1),p_4114_RESET(PB10),p_4114_CLK(PB11),p_4114_DO(PB12),p_4114_A_CS(PB15) as output pull-up */
	// 20130710 add p_4114_DI(PB13),p_4114_A_Int0(PB14)
  	GPIO_InitStructure.GPIO_Pin = 	GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_10 | GPIO_Pin_11 |
  	                                GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  	GPIO_Init(GPIOB, &GPIO_InitStructure);
    
#ifdef JTAG_DISABLE //20121112
    // Full SWJ Disabled (JTAG-DP + SW-DP)
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
    // Configure PB.03 (JTDO) and PB.04 (JTRST) as input
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
#endif  
  }  	

	/*--------------------------*/
	/* GPIO C configuration 	*/
	/*--------------------------*/
  if (type != 0)
	{
    //-(1)--Normal------------------------------------------------------
  	/* configure AD key(PC0~PC3) as analoge in */ 
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  	GPIO_Init(GPIOC, &GPIO_InitStructure);

  	/* configure CH_A_DIGITAL(PC4),CH_B_DIGITAL(PC5),p_4114_B_CS(PC7),p_4114_C_CS(PC9),SMPS_nON(PC11),STANDBY_LED(PC13),MAIN_STBY(PC14),FAN_CTRL(PC15) as output push-pull */
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 |GPIO_Pin_5 |GPIO_Pin_7 | GPIO_Pin_9 | GPIO_Pin_11 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  	GPIO_Init(GPIOC, &GPIO_InitStructure);

  	/* configure p_4114_B_Int0(PC6),p_4114_C_Int0(PC8),RKB_mode_PP_MP(PC12) as input pull-up */
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_8 | GPIO_Pin_12;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  	GPIO_Init(GPIOC, &GPIO_InitStructure);

  	/* configure AMP_nRDY(PC.10) as input floating */		//AMP_nRDY : 20131024
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  	GPIO_Init(GPIOC, &GPIO_InitStructure);
  	
	/* configure POWER DOWN(PC13) as input pull-down */
//  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
//  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
//  	GPIO_Init(GPIOC, &GPIO_InitStructure);

#ifdef UART4_ENABLE	
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);		//greenchips

  	/* configure UART4 Rx (PC.11) as input floating */              //RotelLink [57600,n,8,1]
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  	GPIO_Init(GPIOC, &GPIO_InitStructure);
  	
  	/* configure UART4 Tx (PC.10) as alternate function push-pull */
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  	GPIO_Init(GPIOC, &GPIO_InitStructure);
#endif  //UART4_ENABLE

  }else{
    //-(0)--power consumption------------------------------------------------------
	//20130709 : STANDBY_LED(PC13) delete    
  	/* configure CH_A_DIGITAL(PC4),CH_B_DIGITAL(PC5),p_4114_B_CS(PC7),p_4114_C_CS(PC9),MAIN_STBY(PC14),FAN_CTRL(PC15) as output push-pull */
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 |GPIO_Pin_5 |GPIO_Pin_7 | GPIO_Pin_9 | GPIO_Pin_14 | GPIO_Pin_15;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  	GPIO_Init(GPIOC, &GPIO_InitStructure);

  	/* configure AD key(PC0~3) */
  	/* configure p_4114_B_Int0(PC6),p_4114_C_Int0(PC8),AMP_nRDY(PC10),SMPS_nON(PC11) as input pull-up */
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_6 | GPIO_Pin_8 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  	GPIO_Init(GPIOC, &GPIO_InitStructure);
  }  	

	/*--------------------------*/
	/* GPIO D configuration 	*/
	/* 64pin : PD2 only         */
	/* 100pin : PD0~15          */
	/*--------------------------*/
#ifdef STM32_64PIN_ENABLE	
  if (type != 0)
	{
    //-(1)--Normal------------------------------------------------------
  	/* configure (PD2) as output push-pull */
//  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
//  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//  	GPIO_Init(GPIOD, &GPIO_InitStructure);

  	/* Configure nOverLoad(PD2) as input pull-up */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

  }else{
    //-(0)--power consumption------------------------------------------------------
#ifdef OPTICAL_POWER_ENABLE
	//20140515 : if optical_power=enable , then skip!
	//20140515 : if optical_power=disalbe, then go!
	if (optical_power_mode == OFF) {
		//disable=OFF
  		//20140515 : configure nOverLoad(PD2) as input floating
  		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  		GPIO_Init(GPIOD, &GPIO_InitStructure);
	}
#else    
  	//20130710 : configure nOverLoad(PD2) as input floating
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  	GPIO_Init(GPIOD, &GPIO_InitStructure);
#endif
  }  	
#endif	//STM32_64PIN_ENABLE
#ifdef STM32_100PIN_ENABLE	
  if (type != 0)
	{
    //-(1)--Normal------------------------------------------------------
  	/* configure CMEDIA_USB_CLASS(PD4), CMEDIA_I2C_CLK(PD5) as output push-pull */     //20130108
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  	GPIO_Init(GPIOD, &GPIO_InitStructure);

  	/* configure TunerRelay(PD8),Aux1Relay(PD9),Aux2Relay(PD10),DacOutRelay(PD11),4114Cs(PD14),4114Ck(PD15) as input push-up */
  	GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_14 | GPIO_Pin_15;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  	GPIO_Init(GPIOD, &GPIO_InitStructure);

	/* Configure CMEDIA_SUSPEND(PD3),CMEDIA_INT(PD7),4114Int1(PD12),4114Int0(PD13) as input pull-up */  //20130111
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_7 | GPIO_Pin_12 | GPIO_Pin_13;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  	GPIO_Init(GPIOD, &GPIO_InitStructure);

  	/* Configure CMEDIA_INT(PD7) as input pull-up */
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
//	GPIO_Init(GPIOD, &GPIO_InitStructure);

  }else{
    //-(0)--power consumption------------------------------------------------------
    
  	/* configure CMEDIA_USB_CLASS(PD4), CMEDIA_I2C_CLK(PD5) as output push-pull */     //20130108
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  	GPIO_Init(GPIOD, &GPIO_InitStructure);

  	/* configure CMEDIA_INT(PD7),TunerRelay(PD8),Aux1Relay(PD9),Aux2Relay(PD10),DacOutRelay(PD11),4114Cs(PD14),4114Ck(PD15) as input push-up */
  	GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_14 | GPIO_Pin_15;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  	GPIO_Init(GPIOD, &GPIO_InitStructure);
   
  }  	
#endif	//STM32_100PIN_ENABLE

	/*--------------------------*/
	/* GPIO E configuration 	*/
	/*--------------------------*/
#ifdef STM32_100PIN_ENABLE	
  if (type != 0)
	{
    //-(1)--Normal------------------------------------------------------
  	/* Configure Option(PE4) as input pull-up */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;             //20130201
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

  	/* configure SpeakerA(PE5),SpeakerB(PE6),2311Mute(PE7),2311Cs(PE8),2311Data(PE9),2311Clk(PE10),1194Lh(PE11),1194Data(PE12),1194Clk(PE13),toneBypass(PE14) as input push-up */
  	GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 |
                                    GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  	GPIO_Init(GPIOE, &GPIO_InitStructure);

  }else{
    //-(0)--power consumption------------------------------------------------------
    
  	/* Configure Option(PE4) as input pull-up */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;                 //20130201
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

  	/* configure SpeakerA(PE5),SpeakerB(PE6),2311Mute(PE7),2311Cs(PE8),2311Data(PE9),2311Clk(PE10),1194Lh(PE11),1194Data(PE12),1194Clk(PE13),toneBypass(PE14) as input push-up */
  	GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 |
                                    GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  	GPIO_Init(GPIOE, &GPIO_InitStructure);
    
  }
#endif	//STM32_100PIN_ENABLE  	
}
#endif
#ifdef _RKB_DM8100
//type: 0-power comsuption
//      1-normal mode
void GPIO_Configuration(unsigned char type)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	if(type != 0)
		{
		   /* PORT A */
		   /*configure SS_ON(PA0),12V_TRIGIN(PA11) SS_MODE(PA12) as input pullup*/
           GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_11 | GPIO_Pin_12;
		   GPIO_InitStructure.GPIO_Mode= GPIO_Mode_IN;
		   GPIO_InitStructure.GPIO_PuPd= GPIO_PuPd_UP;
		   GPIO_Init(GPIOA,&GPIO_InitStructure);
           /*USART1_TX(PA9)*/
		   GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);
		   /*USART1_RX(PA10)*/
		   GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);

		   GPIO_InitStructure.GPIO_OType= GPIO_OType_PP;
		   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		   GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_9;

		   GPIO_Init(GPIOA,&GPIO_InitStructure);

		   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		   GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_10;

		   GPIO_Init(GPIOA,&GPIO_InitStructure);       
		}
   else
		{

	    }
   if(type != 0)
   	{
		   /* PORT B */
		   /*configure EEPROM_SCL(PB6) as output nopullup */
		   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
		   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
		   GPIO_Init(GPIOB,&GPIO_InitStructure);

		   /*2752_A_SEL(PB8) 2752_B_SEL(PB9)*/
		   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
		   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		   GPIO_Init(GPIOB,&GPIO_InitStructure);

		   /*configure NULL(PB15) as input no pullup*/
           GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
		   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
		   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
		   GPIO_Init(GPIOB,&GPIO_InitStructure);	
   	}
   else
   	{
           GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_8 | GPIO_Pin_9;
		   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
		   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
		   GPIO_Init(GPIOB,&GPIO_InitStructure);
    }

   
   if(type != 0)
   	{
		   /*PORT C*/
		   /*configure AMP_READY(PC6) AMP_ERROR(PC7),NULL(13,14,15) as input no pullup*/
		   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
		   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
		   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
		   GPIO_Init(GPIOC,&GPIO_InitStructure);

		   /*AMP_ON(PC8) SMPS_ON(PC9)*/
		   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
		   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		   GPIO_Init(GPIOC,&GPIO_InitStructure);
#if 0
		   /*AMP_ON(PC8) SMPS_ON(PC9)*/
		   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
		   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		   GPIO_Init(GPIOC,&GPIO_InitStructure);
#endif		   
           /*USART4_TX(PC10)*/
		   GPIO_PinAFConfig(GPIOC,GPIO_PinSource10,GPIO_AF_UART4);
		   /*USART4_RX(PC11)*/
		   GPIO_PinAFConfig(GPIOC,GPIO_PinSource11,GPIO_AF_UART4);

		   GPIO_InitStructure.GPIO_OType= GPIO_OType_PP;
		   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		   GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_10;

		   GPIO_Init(GPIOC,&GPIO_InitStructure);

		   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		   GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_11;

		   GPIO_Init(GPIOC,&GPIO_InitStructure);	

           /*USART5_TX(PC12)*/
		   GPIO_PinAFConfig(GPIOC,GPIO_PinSource12,GPIO_AF_UART5);
		   /*USART5_RX(PD2)*/
		   GPIO_PinAFConfig(GPIOD,GPIO_PinSource2,GPIO_AF_UART5);

		   GPIO_InitStructure.GPIO_OType= GPIO_OType_PP;
		   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		   GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_12;

		   GPIO_Init(GPIOC,&GPIO_InitStructure);

		   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		   GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_2;

		   GPIO_Init(GPIOD,&GPIO_InitStructure);
   	}
   else
   	{
		   /*configure AMP_ON(PC8) SMPS_ON(PC9) as input no pullup*/
		   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
		   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
		   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
		   GPIO_Init(GPIOC,&GPIO_InitStructure);
    }
   if(type != 0)
   	{
		   /*PORT D*/
		   /*configure ID0(PD0) ID1(PD1),NULL(8~15) as input no pullup*/
		   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10
		                                |GPIO_Pin_11| GPIO_Pin_12| GPIO_Pin_13| GPIO_Pin_14| GPIO_Pin_15 ;
		   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
		   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;

		   GPIO_Init(GPIOD,&GPIO_InitStructure);

		   /*configure FAN_L(PD4) FAN_H(PD5) NJW1194_LAT(PD6) NJW1194_CLK(PD7) as output pullup*/
		   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
		   GPIO_InitStructure.GPIO_Mode= GPIO_Mode_OUT;
		   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;

		   GPIO_Init(GPIOD,&GPIO_InitStructure);
   	}
   else
   	{
		   /*configure FAN_L(PD4) FAN_H(PD5) NJW1194_LAT(PD6) NJW1194_CLK(PD7) as input no pullup*/
		   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
		   GPIO_InitStructure.GPIO_Mode= GPIO_Mode_IN;
		   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

		   GPIO_Init(GPIOD,&GPIO_InitStructure);          
    }
   if(type != 0)
   	{

		   /*PORT E*/
		   /*configure 2752_C_SEL(PE0) 2752_D_SEL(PE1) 4117_A_CS(PE9) 4117_B_CS(PE11) 4117_C_CS(PE13)
		     4117_D_CS(PE15) as output pullup 
		   */
		   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_9 | GPIO_Pin_11
		                                |GPIO_Pin_13 | GPIO_Pin_15;
		   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;

		   GPIO_Init(GPIOE,&GPIO_InitStructure);

		   /*configure MODE_OPTION(PE2),MODE_OPTION(PE3),MODE_SET_INPUT(PE4) 4117_DO(PE7) 4117_A_INT0(PE8) 4117_B_INT0(PE10) 4117_C_INT0(PE12) 4117_D_INT0(PE14)*/
		   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 |GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_10 | GPIO_Pin_12
		                                |GPIO_Pin_14;
		   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
		   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

		   GPIO_Init(GPIOE,&GPIO_InitStructure);

		   /*configure NULL(PE5,PE6) as input no pullup*/
		   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6;
		   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
		   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
		   GPIO_Init(GPIOE,&GPIO_InitStructure);
   	}
   else
   	{
		   /*PORT E*/
		   /*configure 2752_C_SEL(PE0) 2752_D_SEL(PE1) 4117_A_CS(PE9) 4117_B_CS(PE11) 4117_C_CS(PE13)
		     4117_D_CS(PE15) as input no pullup
		   */
		   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_9 | GPIO_Pin_11
		                                |GPIO_Pin_13 | GPIO_Pin_15;
		   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
		   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

		   GPIO_Init(GPIOE,&GPIO_InitStructure);
    }
   if(type != 0)
   	{
		   /*PORT F*/
		   /*configure NTC-AMP1(PF3) NTC-AMP2(PF4) NTC-SMPS(PF5) NTC-SMPS(PF6)
		     F_VOL_A(PF7) F_VOL_B(PF8) F_VOL_C(PF9) F_VOL_D(PF10) as input no pullup*/

		   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10;
		   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
		   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

		   GPIO_Init(GPIOF,&GPIO_InitStructure);
           /*configure NTC-AMP1(PF3) NTC-AMP2(PF4) NTC-SMPS(PF5) NTC-SMPS(PF6)*/
		   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6;

		   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
		   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

		   GPIO_Init(GPIOF,&GPIO_InitStructure);		   

           /*configure 4117_RST(PF15) as output pullup*/
		   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
		   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;

		   GPIO_Init(GPIOF,&GPIO_InitStructure);

		   #ifdef _FRT_RMC
		   /*configure front RMC(PF0) as input pull up*/
           GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
		   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
		   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		   GPIO_Init(GPIOF,&GPIO_InitStructure);
		   #endif

		   /*configure NULL(PF1,PF2,PF13,PF14) as input no pullup*/
		   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_13 | GPIO_Pin_14;
		   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
		   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

		   GPIO_Init(GPIOF,&GPIO_InitStructure);
		   
   	}
   else
   	{
           /*configure 4117_RST(PF15) as input no pullup*/
		   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
		   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
		   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

		   GPIO_Init(GPIOF,&GPIO_InitStructure);
    }
   if(type != 0)
   	{
		   /*PORT G*/
		   /*4117_CLK(PG0) 4117_DI(PG1) PWR_RLY(PG2) PWR_LED(PG3) SENSE_LED(PG4) 1194_DAT(PG9)
		     1112_LAT(PG10) 1112_CLK(PG11) 1112_DAT(PG12)*/

		   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3
		                                |GPIO_Pin_4 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11
		                                |GPIO_Pin_12;
		   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;

		   GPIO_Init(GPIOG,&GPIO_InitStructure);

		   /*PDN(PG5) FACTORY_DEFAULT(PG15)*/
		   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
		   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
		   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

		   GPIO_Init(GPIOG,&GPIO_InitStructure);
		   /*configure PDN(PG5) as input */

		   /*configure NULL(PG6,PG7,PG8) as Analog no pull*/
		   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8;
		   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
		   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
		   GPIO_Init(GPIOG,&GPIO_InitStructure);
		   
	    }
   else
   	{
           /*configure 4117_CLK(PG0) 4117_DI(PG1) PWR_RLY(PG2) PWR_LED(PG3) SENSE_LED(PG4) 1194_DAT(PG9)
		     1112_LAT(PG10) 1112_CLK(PG11) 1112_DAT(PG12) PDN(PG5)as input no pull*/
		   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3
		                                |GPIO_Pin_4 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11
		                                |GPIO_Pin_12| GPIO_Pin_5;
		   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
		   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
		   GPIO_Init(GPIOG,&GPIO_InitStructure);
    }
}
#else
//
//------------------------------------------------------------------------------
//	RKB Working Sample 2
//------------------------------------------------------------------------------
void GPIO_Configuration(unsigned char type)
{
  	GPIO_InitTypeDef GPIO_InitStructure;

  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	/*--------------------------*/
	/* GPIO A configuration 	*/
	/*--------------------------*/
  if (type != 0)
	{
    //-(1)--Normal------------------------------------------------------
  	/* configure AD key(PA0~PA1) as analoge in */ 
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  	GPIO_Init(GPIOA, &GPIO_InitStructure);

  	/* configure SMPS_nON(PA3),REC_MUTE(PA4),NJW1159_LAT(PA5),NJW1159_CLK(PA6),NJW1159_DAT(PA7),p_4114_D_CS(PA11) as output function push-pull */
  	GPIO_InitStructure.GPIO_Pin = 	GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_11; 
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  	GPIO_Init(GPIOA, &GPIO_InitStructure);
  	
  	/* Configure p_4114_D_Int0(PA8),TRIGGER_IN(PA12) as input pull-up */
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_12;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  	GPIO_Init(GPIOA, &GPIO_InitStructure);

#ifdef UART2_ENABLE	
  	/* configure USART2 Rx (PA.3) as input floating */                      //Rx
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  	GPIO_Init(GPIOA, &GPIO_InitStructure);
  	
  	/* configure USART2 Tx (PA.2) as alternate function push-pull */        //Tx
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  	GPIO_Init(GPIOA, &GPIO_InitStructure);
#endif
  	/* configure AMP_nRDY(PA.2), USART1 Rx (PA.10) as input floating */		//AMP_nRDY,RS232 Rx
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_10;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  	GPIO_Init(GPIOA, &GPIO_InitStructure);
  	
  	/* configure USART1 Tx (PA.09) as alternate function push-pull */       //RS232 Tx
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  	GPIO_Init(GPIOA, &GPIO_InitStructure);

  }else{
    //-(0)--power consumption------------------------------------------------------
    //
    //no change SMPS_nON,REC_MUTE,,LED_STBY, mainMute, mainSTBY
    //
  	/* configure AD key(PA0~PA1),NJW1159_LAT(PA5),NJW1159_CLK(PA6),NJW1159_DAT(PA7),p_4114_D_CS(PA11) as output function push-pull */
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 |GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_11; 
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  	GPIO_Init(GPIOA, &GPIO_InitStructure);

  	//20130710 : add p_4114_D_Int0(PA8)
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; 
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  	GPIO_Init(GPIOA, &GPIO_InitStructure);
    
#ifdef JTAG_DISABLE //20121112
    // Full SWJ Disabled (JTAG-DP + SW-DP)
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
    // Configure PA.13 (JTMS/SWDAT), PA.14 (JTCK/SWCLK) and PA.15 (JTDI) as input
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
#endif  
  }

	/*--------------------------*/
	/* GPIO B configuration 	*/
	/*--------------------------*/
  if (type != 0)
	{
    //-(1)--Normal------------------------------------------------------
	/* Configure CH_C_DIGITAL(PB0),CH_D_DIGITAL(PB1),IIC_SCL(PB6),p_4114_RESET(PB10),p_4114_CLK(PB11),p_4114_DO(PB12),p_4114_A_CS(PB15) as output pull-up */
  	GPIO_InitStructure.GPIO_Pin = 	GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_6 | GPIO_Pin_10 | GPIO_Pin_11 |
  	                                GPIO_Pin_12 | GPIO_Pin_15;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
//	/* Configure SDA_4353(PB15) as alternate function push-pull */
//  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
//  	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
//  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//  	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
//	/* Configure POWER_DOWN(PB5),PROJ_8100(PB8),PROJ_DIGITAL(PB9),p_4114_DI(PB13),p_4114_A_Int0(PB14) as input pull-up */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_13 | GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

#ifdef UART3_ENABLE	
  	/* configure USART3 rx (PB.11) as input floating */
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  	GPIO_Init(GPIOB, &GPIO_InitStructure);
  	
  	/* configure USART3 Tx (PB.10) as alternate function push-pull */       //?not used
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  	GPIO_Init(GPIOB, &GPIO_InitStructure);
#endif  //UART3_ENABLE

  }else{
    //-(0)--power consumption------------------------------------------------------
    
	/* Configure CH_C_DIGITAL(PB0),CH_D_DIGITAL(PB1),IIC_SCL(PB6),p_4114_RESET(PB10),p_4114_CLK(PB11),p_4114_DO(PB12),p_4114_A_CS(PB15) as output pull-up */
	// 20130710 add p_4114_DI(PB13),p_4114_A_Int0(PB14)
  	GPIO_InitStructure.GPIO_Pin = 	GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_6 | GPIO_Pin_10 | GPIO_Pin_11 |
  	                                GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  	GPIO_Init(GPIOB, &GPIO_InitStructure);
    
#ifdef JTAG_DISABLE //20121112
    // Full SWJ Disabled (JTAG-DP + SW-DP)
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
    // Configure PB.03 (JTDO) and PB.04 (JTRST) as input
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
#endif  
  }  	

	/*--------------------------*/
	/* GPIO C configuration 	*/
	/*--------------------------*/
  if (type != 0)
	{
    //-(1)--Normal------------------------------------------------------
  	/* configure AD key(PC0~PC3) as analoge in */ 
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  	GPIO_Init(GPIOC, &GPIO_InitStructure);

  	/* configure CH_A_DIGITAL(PC4),CH_B_DIGITAL(PC5),p_4114_B_CS(PC7),p_4114_C_CS(PC9),STANDBY_LED(PC13),MAIN_STBY(PC14),FAN_CTRL(PC15) as output push-pull */
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 |GPIO_Pin_5 |GPIO_Pin_7 | GPIO_Pin_9 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  	GPIO_Init(GPIOC, &GPIO_InitStructure);

  	/* configure p_4114_B_Int0(PC6),p_4114_C_Int0(PC8),SUBID0(PC10),SUBID1(PC11),S3(PC12) as input pull-up */
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_8 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* configure POWER DOWN(PC13) as input pull-down */
//  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
//  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
//  	GPIO_Init(GPIOC, &GPIO_InitStructure);

#ifdef UART4_ENABLE	
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);		//greenchips

  	/* configure UART4 Rx (PC.11) as input floating */              //RotelLink [57600,n,8,1]
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  	GPIO_Init(GPIOC, &GPIO_InitStructure);
  	
  	/* configure UART4 Tx (PC.10) as alternate function push-pull */
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  	GPIO_Init(GPIOC, &GPIO_InitStructure);
#endif  //UART4_ENABLE

  }else{
    //-(0)--power consumption------------------------------------------------------
	//20130709 : STANDBY_LED(PC13) delete    
  	/* configure CH_A_DIGITAL(PC4),CH_B_DIGITAL(PC5),p_4114_B_CS(PC7),p_4114_C_CS(PC9),MAIN_STBY(PC14),FAN_CTRL(PC15) as output push-pull */
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 |GPIO_Pin_5 |GPIO_Pin_7 | GPIO_Pin_9 | GPIO_Pin_14 | GPIO_Pin_15;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  	GPIO_Init(GPIOC, &GPIO_InitStructure);

  	/* configure AD key(PC0~3) */
  	/* configure p_4114_B_Int0(PC6),p_4114_C_Int0(PC8),SUBID0(PC10),SUBID1(PC11),S3(PC12) as input pull-up */
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_6 | GPIO_Pin_8 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  	GPIO_Init(GPIOC, &GPIO_InitStructure);
  }  	

	/*--------------------------*/
	/* GPIO D configuration 	*/
	/* 64pin : PD2 only         */
	/* 100pin : PD0~15          */
	/*--------------------------*/
#ifdef STM32_64PIN_ENABLE	
  if (type != 0)
	{
    //-(1)--Normal------------------------------------------------------
  	/* configure (PD2) as output push-pull */
//  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
//  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//  	GPIO_Init(GPIOD, &GPIO_InitStructure);

  	/* Configure nOverLoad(PD2) as input pull-up */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

  }else{
    //-(0)--power consumption------------------------------------------------------
    
  	//20130710 : configure nOverLoad(PD2) as input floating
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  	GPIO_Init(GPIOD, &GPIO_InitStructure);

  }  	
#endif	//STM32_64PIN_ENABLE
#ifdef STM32_100PIN_ENABLE	
  if (type != 0)
	{
    //-(1)--Normal------------------------------------------------------
  	/* configure CMEDIA_USB_CLASS(PD4), CMEDIA_I2C_CLK(PD5) as output push-pull */     //20130108
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  	GPIO_Init(GPIOD, &GPIO_InitStructure);

  	/* configure TunerRelay(PD8),Aux1Relay(PD9),Aux2Relay(PD10),DacOutRelay(PD11),4114Cs(PD14),4114Ck(PD15) as input push-up */
  	GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_14 | GPIO_Pin_15;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  	GPIO_Init(GPIOD, &GPIO_InitStructure);

	/* Configure CMEDIA_SUSPEND(PD3),CMEDIA_INT(PD7),4114Int1(PD12),4114Int0(PD13) as input pull-up */  //20130111
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_7 | GPIO_Pin_12 | GPIO_Pin_13;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  	GPIO_Init(GPIOD, &GPIO_InitStructure);

  	/* Configure CMEDIA_INT(PD7) as input pull-up */
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
//	GPIO_Init(GPIOD, &GPIO_InitStructure);

  }else{
    //-(0)--power consumption------------------------------------------------------
    
  	/* configure CMEDIA_USB_CLASS(PD4), CMEDIA_I2C_CLK(PD5) as output push-pull */     //20130108
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  	GPIO_Init(GPIOD, &GPIO_InitStructure);

  	/* configure CMEDIA_INT(PD7),TunerRelay(PD8),Aux1Relay(PD9),Aux2Relay(PD10),DacOutRelay(PD11),4114Cs(PD14),4114Ck(PD15) as input push-up */
  	GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_14 | GPIO_Pin_15;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  	GPIO_Init(GPIOD, &GPIO_InitStructure);
   
  }  	
#endif	//STM32_100PIN_ENABLE

	/*--------------------------*/
	/* GPIO E configuration 	*/
	/*--------------------------*/
#ifdef STM32_100PIN_ENABLE	
  if (type != 0)
	{
    //-(1)--Normal------------------------------------------------------
  	/* Configure Option(PE4) as input pull-up */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;             //20130201
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

  	/* configure SpeakerA(PE5),SpeakerB(PE6),2311Mute(PE7),2311Cs(PE8),2311Data(PE9),2311Clk(PE10),1194Lh(PE11),1194Data(PE12),1194Clk(PE13),toneBypass(PE14) as input push-up */
  	GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 |
                                    GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  	GPIO_Init(GPIOE, &GPIO_InitStructure);

  }else{
    //-(0)--power consumption------------------------------------------------------
    
  	/* Configure Option(PE4) as input pull-up */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;                 //20130201
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

  	/* configure SpeakerA(PE5),SpeakerB(PE6),2311Mute(PE7),2311Cs(PE8),2311Data(PE9),2311Clk(PE10),1194Lh(PE11),1194Data(PE12),1194Clk(PE13),toneBypass(PE14) as input push-up */
  	GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 |
                                    GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  	GPIO_Init(GPIOE, &GPIO_InitStructure);
    
  }
#endif	//STM32_100PIN_ENABLE  	
}
#endif
/*-----------------------------------------------------------------------------
 Function Name  : UART_Configuration
 Description    : Configures the UART.
 Input          : None
 Output         : None
 Return         : None
 Comments	 	: ..
-----------------------------------------------------------------------------*/
void UART_Configuration(void)
{
	USART_InitTypeDef USART_InitStructure;

  	/* USART and USART2 configured as follow:
  	      - BaudRate = 115200 baud  
  	      - Word Length = 8 Bits
  	      - One Stop Bit
  	      - No parity
  	      - Hardware flow control disabled (RTS and CTS signals)
  	      - Receive and transmit enabled
  	*/
  	//----------------------------------------------------------------
  	//	mode_AHB_prescaler
  	//	1=36MHz, 2=18MHz, 4=9MHz, 8=4.5MHz
  	// 20140408
  	//----------------------------------------------------------------
	/* USART1 */  	
	#ifdef _STM32_F2XX_144PIN
  	USART_InitStructure.USART_BaudRate = 115200;	    //RS232 : 115200,n,8,1
	#else
  	USART_InitStructure.USART_BaudRate = 115200*2;	    //set 115200 used 36mhz         //RS232 : 115200,n,8,1
  	#endif
  	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  	USART_InitStructure.USART_StopBits = USART_StopBits_1;
  	USART_InitStructure.USART_Parity = USART_Parity_No;
  	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  	/* Configure USART1 */
  	USART_Init(USART1, &USART_InitStructure);

	/* Enable USART1 Receive and Transmit interrupts */
  	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);		//Receive Data register not empty interrupt
  	//USART_ITConfig(USART1, USART_IT_TXE, ENABLE);		//Tansmit Data Register empty interrupt
  	USART_ITConfig(USART1, USART_IT_TC, ENABLE);		//Transmission complete interrupt

  	/* Enable the USART1 */
  	USART_Cmd(USART1, ENABLE);

#ifdef UART2_ENABLE	
	/* USART2 */  	
#ifdef _STM32_F2XX_144PIN
    USART_InitStructure.USART_BaudRate = 115200;
#else
  	USART_InitStructure.USART_BaudRate = 115200*2;	    //set 115200 used 36mhz         //Telechips : 115200,n,8,1
#endif
  	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  	USART_InitStructure.USART_StopBits = USART_StopBits_1;
  	USART_InitStructure.USART_Parity = USART_Parity_No;
  	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  	/* Configure USART2 */
  	USART_Init(USART2, &USART_InitStructure);

	/* Enable USART2 Receive and Transmit interrupts */
  	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);		//Receive Data register not empty interrupt
  	//USART_ITConfig(USART2, USART_IT_TXE, ENABLE);		//Tansmit Data Register empty interrupt
  	USART_ITConfig(USART2, USART_IT_TC, ENABLE);		//Transmission complete interrupt

  	/* Enable the USART2 */
  	USART_Cmd(USART2, ENABLE);
#endif
#ifdef UART3_ENABLE	
	/* USART3 */  	
#ifdef _STM32_F2XX_144PIN
    USART_InitStructure.USART_BaudRate = 57600;
#else
//  	USART_InitStructure.USART_BaudRate = 4800*2;	//set 4800 used 36mhz
  	USART_InitStructure.USART_BaudRate = 57600*2;	    //set 57600 used 36mhz          //?AirPlay : 57600,n,8,1
//  	USART_InitStructure.USART_BaudRate = 115200*2;	//set 115200 used 36mhz
//  	USART_InitStructure.USART_BaudRate = 9600*2;	//set 9600 used 36mhz
#endif
  	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  	USART_InitStructure.USART_StopBits = USART_StopBits_1;
  	USART_InitStructure.USART_Parity = USART_Parity_No;
  	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  	/* Configure USART3 */
  	USART_Init(USART3, &USART_InitStructure);

	/* Enable USART3 Receive and Transmit interrupts */
  	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);		//Receive Data register not empty interrupt
  	//USART_ITConfig(USART3, USART_IT_TXE, ENABLE);		//Tansmit Data Register empty interrupt
  	USART_ITConfig(USART3, USART_IT_TC, ENABLE);		//Transmission complete interrupt

  	/* Enable the USART3 */
  	USART_Cmd(USART3, ENABLE);
#endif
#ifdef UART4_ENABLE
	/* UART4 */  											//100pin
#ifdef _STM32_F2XX_144PIN
    USART_InitStructure.USART_BaudRate = 57600;
#else
//  	USART_InitStructure.USART_BaudRate = 4800*2;	    //set 4800 used 36mhz
//  	USART_InitStructure.USART_BaudRate = 19200*2;		//set 19200 used 36mhz		//MPEG : 19200,n,8,1
  	USART_InitStructure.USART_BaudRate = 57600*2;	        //set 57600 used 36mhz      //RotelLink : 57600,n,8,1 
//  	USART_InitStructure.USART_BaudRate = 115200*2;		//set 115200 used 36mhz
#endif
  	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  	USART_InitStructure.USART_StopBits = USART_StopBits_1;
  	USART_InitStructure.USART_Parity = USART_Parity_No;
  	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  	/* Configure UART4 */
  	USART_Init(UART4, &USART_InitStructure);

	/* Enable UART4 Receive and Transmit interrupts */
  	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);		//Receive Data register not empty interrupt
  	//USART_ITConfig(USART4, USART_IT_TXE, ENABLE);		//Tansmit Data Register empty interrupt
  	USART_ITConfig(UART4, USART_IT_TC, ENABLE);			//Transmission complete interrupt

  	/* Enable the UART4 */
  	USART_Cmd(UART4, ENABLE);
#endif
#ifdef UART5_ENABLE
	/* UART5 : RA10 ( 115,200bps ) */  	
#ifdef _STM32_F2XX_144PIN
    USART_InitStructure.USART_BaudRate = 57600;
#else
//  	USART_InitStructure.USART_BaudRate = 4800*2;	//set 4800 used 36mhz
//  	USART_InitStructure.USART_BaudRate = 19200*2;	//set 19200 used 36mhz
  	USART_InitStructure.USART_BaudRate = 57600*2;		//set 57600 used 36mhz
#endif
  	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  	USART_InitStructure.USART_StopBits = USART_StopBits_1;
  	USART_InitStructure.USART_Parity = USART_Parity_No;
  	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  	/* Configure UART5 */
  	USART_Init(UART5, &USART_InitStructure);

	/* Enable UART5 Receive and Transmit interrupts */
  	USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);		//Receive Data register not empty interrupt
  	//USART_ITConfig(USART5, USART_IT_TXE, ENABLE);		//Tansmit Data Register empty interrupt
  	USART_ITConfig(UART5, USART_IT_TC, ENABLE);			//Transmission complete interrupt

  	/* Enable the UART5 */
  	USART_Cmd(UART5, ENABLE);
#endif
}
#ifndef _STM32_F2XX_144PIN
/*-----------------------------------------------------------------------------
 Function Name  : ADC_Configuration
 Description    : Configures the ADC.
 Input          : None
 Output         : None
 Return         : None
 Comments	 	: PC0(ADC_IN10),PC1(ADC_IN11),PC2(ADC_IN12),PC3(ADC_IN13)
				  PA0(ADC_IN0),PA1(ADC_IN1),PA2(ADC_IN2),PA3(ADC_IN3)
	ADC_DeInit(ADC10/11/12/13); Volume CH-A/B/C/D
	ADC_DeInit(ADC0/1); NTC-L(AMP_CD)/R(AMP_AB)

	ADC_DeInit(ADC2/3); SMPS_1/2	: 20131024 added for PP/MP
-----------------------------------------------------------------------------*/
void ADC_Configuration_PP(unsigned char type)
{
  if (type != 0)
	{
	//(normal)--------------------------------------------------------------------

	ADC_InitTypeDef ADC_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;

  	// DMA1 channel1 configuration ----------------------------------------------
  	DMA_DeInit(DMA1_Channel_1);
  	DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;
  	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&ADC_Val;
  	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  	DMA_InitStructure.DMA_BufferSize = 8;
//  	DMA_InitStructure.DMA_BufferSize = 4;
//  	DMA_InitStructure.DMA_BufferSize = 3;
  	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	
  	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  	DMA_Init(DMA1_Channel1, &DMA_InitStructure);
  	// Enable DMA1 channel1 */
  	DMA_Cmd(DMA1_Channel1, ENABLE);

  	// Enable DMA1 IT interupt */
	DMA_ITConfig(DMA1_Channel1,DMA_IT_TC,ENABLE);	

  	// ADC1 configuration ------------------------------------------------------
  	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
  	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
  	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  	ADC_InitStructure.ADC_NbrOfChannel = 8;
//  	ADC_InitStructure.ADC_NbrOfChannel = 4;
//  	ADC_InitStructure.ADC_NbrOfChannel = 3;
  	ADC_Init(ADC1, &ADC_InitStructure);
  	
  	// ADC1 regular channel0,11,12,13,0,1,2,3 configuration  
  	ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_55Cycles5);		//PC0
  	ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 2, ADC_SampleTime_55Cycles5);		//PC1
  	ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 3, ADC_SampleTime_55Cycles5);		//PC2
  	ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 4, ADC_SampleTime_55Cycles5);		//PC3
//  	ADC_RegularChannelConfig(ADC1, ADC_Channel_14, 5, ADC_SampleTime_55Cycles5);
//  	ADC_RegularChannelConfig(ADC1, ADC_Channel_15, 6, ADC_SampleTime_55Cycles5);
  	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 5, ADC_SampleTime_55Cycles5);		//PA0 : NTC-L(AMP_CD)
  	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 6, ADC_SampleTime_55Cycles5);		//PA1 : NTC-R(AMP_AB)
  	ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 7, ADC_SampleTime_55Cycles5);		//PA2 : SMPS-1 for 850/8100
  	ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 8, ADC_SampleTime_55Cycles5);		//PA3 : SMPS-2 for 8100 only
  	// Enable ADC1 EOC interupt 
	//ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);
  	
  	// Enable ADC1 DMA 
  	ADC_DMACmd(ADC1, ENABLE);
  	
  	// Enable ADC1 
  	ADC_Cmd(ADC1, ENABLE);
  	
  	// Enable ADC1 reset calibaration register  
  	ADC_ResetCalibration(ADC1);
  	// Check the end of ADC1 reset calibration register 
  	while(ADC_GetResetCalibrationStatus(ADC1));
  	
  	// Start ADC1 calibaration 
  	ADC_StartCalibration(ADC1);
  	// Check the end of ADC1 calibration 
  	while(ADC_GetCalibrationStatus(ADC1));
  	   
  	// Start ADC1 Software Conversion 
  	ADC_SoftwareStartConvCmd(ADC1, ENABLE);

  }else{
	//(power_consumption)--------------------------------------------------------------------
	ADC_DeInit(ADC1);						//ADC disable
/* 20130709 : delete
	ADC_InitTypeDef ADC_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;

  	//DMA1 channel1 configuration ----------------------------------------------
  	DMA_DeInit(DMA1_Channel1);
  	DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;
  	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&ADC_Val;
  	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
//  	DMA_InitStructure.DMA_BufferSize = 6;
	  	DMA_InitStructure.DMA_BufferSize = 4;
//      DMA_InitStructure.DMA_BufferSize = 2;
//  	DMA_InitStructure.DMA_BufferSize = 1;
  	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  	DMA_Init(DMA1_Channel1, &DMA_InitStructure);
  	// Enable DMA1 channel1 //
  	DMA_Cmd(DMA1_Channel1, ENABLE);

  	// Enable DMA1 IT interupt //
	DMA_ITConfig(DMA1_Channel1,DMA_IT_TC,ENABLE);	

  	// ADC1 configuration ------------------------------------------------------//
  	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
  	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
  	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
//  	ADC_InitStructure.ADC_NbrOfChannel = 6;
  		ADC_InitStructure.ADC_NbrOfChannel = 4;
//      ADC_InitStructure.ADC_NbrOfChannel = 2;
//  	ADC_InitStructure.ADC_NbrOfChannel = 1;
  	ADC_Init(ADC1, &ADC_InitStructure);
  	
  	// ADC1 regular channel0,11,12,13 configuration
//  	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);			//ch_0 :
	  	ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_55Cycles5);		//ch_10 : 
       	ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 2, ADC_SampleTime_55Cycles5);        //ch_11 : 
  		ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 3, ADC_SampleTime_55Cycles5);
  		ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 4, ADC_SampleTime_55Cycles5);
//  	ADC_RegularChannelConfig(ADC1, ADC_Channel_14, 5, ADC_SampleTime_55Cycles5);
//  	ADC_RegularChannelConfig(ADC1, ADC_Channel_15, 6, ADC_SampleTime_55Cycles5);
  	// Enable ADC1 EOC interupt
	//ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);
  	
  	// Enable ADC1 DMA 
  	ADC_DMACmd(ADC1, ENABLE);
  	
  	// Enable ADC1
  	ADC_Cmd(ADC1, ENABLE);
  	
  	// Enable ADC1 reset calibaration register 
  	ADC_ResetCalibration(ADC1);
  	// Check the end of ADC1 reset calibration register
  	while(ADC_GetResetCalibrationStatus(ADC1));
  	
  	// Start ADC1 calibaration
  	ADC_StartCalibration(ADC1);
  	// Check the end of ADC1 calibration
  	while(ADC_GetCalibrationStatus(ADC1));
  	   
  	// Start ADC1 Software Conversion 
  	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
*/
  }
}
#endif
#ifdef _STM32_F2XX_144PIN
void ADC_Configuration(unsigned char type)
{
   if(type != 0)
   	{
       ADC_InitTypeDef ADC_InitStructure;
	   ADC_CommonInitTypeDef ADC_CommonInitStructure;
	   DMA_InitTypeDef DMA_InitStructure;

	   /*DMA1 channel_1 config*/

	   DMA_DeInit(DMA2_Stream1);
	   DMA_InitStructure.DMA_Channel = DMA_Channel_2;
	   DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)0x4001224C;
	   DMA_InitStructure.DMA_Memory0BaseAddr = (u32)&ADC_Val;
	   DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	   DMA_InitStructure.DMA_BufferSize=4;
	   DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	   DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	   DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	   DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	   DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	   DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	   DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	   DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	   DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	   DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;

	   DMA_Init(DMA2_Stream1,&DMA_InitStructure);

	   DMA_Cmd(DMA2_Stream1, ENABLE);

  	   /* Enable DMA1 IT interupt */
	   DMA_ITConfig(DMA2_Stream1,DMA_IT_TC,ENABLE);	   

	   /*ADC COMMON config*/

	   ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	   ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
	   ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div6;
	   ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
	   ADC_CommonInit(&ADC_CommonInitStructure);

	   /*ADC3 config */

	   ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	   ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	   ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConvEdge_None;
	   ADC_InitStructure.ADC_NbrOfConversion = 4;
	   ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	   ADC_InitStructure.ADC_ScanConvMode = ENABLE;

	   ADC_Init(ADC3,&ADC_InitStructure);

	   ADC_RegularChannelConfig(ADC3, ADC_Channel_14, 1, ADC_SampleTime_56Cycles);    //NTC.SMPS1
	   //ADC_RegularChannelConfig(ADC3, ADC_Channel_5, 2, ADC_SampleTime_56Cycles);
	   //ADC_RegularChannelConfig(ADC3, ADC_Channel_6, 3, ADC_SampleTime_56Cycles);
	   //ADC_RegularChannelConfig(ADC3, ADC_Channel_7, 4, ADC_SampleTime_56Cycles);
	   //ADC_RegularChannelConfig(ADC3, ADC_Channel_8, 5, ADC_SampleTime_56Cycles);
	   ADC_RegularChannelConfig(ADC3, ADC_Channel_9, 2, ADC_SampleTime_56Cycles);    //NTC.SMPS2
	   ADC_RegularChannelConfig(ADC3, ADC_Channel_4, 3, ADC_SampleTime_56Cycles);   //NTC.AMP1
	   ADC_RegularChannelConfig(ADC3, ADC_Channel_15, 4, ADC_SampleTime_56Cycles);   //NTC.AMP2

	   ADC_DMARequestAfterLastTransferCmd(ADC3, ENABLE);

	   ADC_DMACmd(ADC3,ENABLE);

	   ADC_Cmd(ADC3,ENABLE);

	   ADC_SoftwareStartConv(ADC3);
    }
   else
   	{
   	   //for power comsuption
       ADC_DeInit();
    }
}
#else
//
//------------------------------------------------------------------------------
//
void ADC_Configuration(unsigned char type)
{
  if (type != 0)
	{
	//(normal)--------------------------------------------------------------------

	ADC_InitTypeDef ADC_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;

  	// DMA1 channel1 configuration ----------------------------------------------
  	DMA_DeInit(DMA1_Channel1);
  	DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;
  	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&ADC_Val;
  	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  	DMA_InitStructure.DMA_BufferSize = 6;
//  	DMA_InitStructure.DMA_BufferSize = 4;
//  	DMA_InitStructure.DMA_BufferSize = 3;
  	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  	DMA_Init(DMA1_Channel1, &DMA_InitStructure);
  	// Enable DMA1 channel1 */
  	DMA_Cmd(DMA1_Channel1, ENABLE);

  	// Enable DMA1 IT interupt */
	DMA_ITConfig(DMA1_Channel1,DMA_IT_TC,ENABLE);	

  	// ADC1 configuration ------------------------------------------------------
  	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
  	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
  	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  	ADC_InitStructure.ADC_NbrOfChannel = 6;
//  	ADC_InitStructure.ADC_NbrOfChannel = 4;
//  	ADC_InitStructure.ADC_NbrOfChannel = 3;
  	ADC_Init(ADC1, &ADC_InitStructure);
  	
  	// ADC1 regular channel0,11,12,13,0,1 configuration  
  	ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_55Cycles5);		//PC0
  	ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 2, ADC_SampleTime_55Cycles5);		//PC1
  	ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 3, ADC_SampleTime_55Cycles5);		//PC2
  	ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 4, ADC_SampleTime_55Cycles5);		//PC3
//  	ADC_RegularChannelConfig(ADC1, ADC_Channel_14, 5, ADC_SampleTime_55Cycles5);
//  	ADC_RegularChannelConfig(ADC1, ADC_Channel_15, 6, ADC_SampleTime_55Cycles5);
  	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 5, ADC_SampleTime_55Cycles5);		//PA0 : NTC-L
  	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 6, ADC_SampleTime_55Cycles5);		//PA1 : NTC-R
  	// Enable ADC1 EOC interupt 
	//ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);
  	
  	// Enable ADC1 DMA 
  	ADC_DMACmd(ADC1, ENABLE);
  	
  	// Enable ADC1 
  	ADC_Cmd(ADC1, ENABLE);
  	
  	// Enable ADC1 reset calibaration register  
  	ADC_ResetCalibration(ADC1);
  	// Check the end of ADC1 reset calibration register 
  	while(ADC_GetResetCalibrationStatus(ADC1));
  	
  	// Start ADC1 calibaration 
  	ADC_StartCalibration(ADC1);
  	// Check the end of ADC1 calibration 
  	while(ADC_GetCalibrationStatus(ADC1));
  	   
  	// Start ADC1 Software Conversion 
  	ADC_SoftwareStartConvCmd(ADC1, ENABLE);

  }else{
	//(power_consumption)--------------------------------------------------------------------
	ADC_DeInit(ADC1);						//ADC disable
/* 20130709 : delete
	ADC_InitTypeDef ADC_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;

  	//DMA1 channel1 configuration ----------------------------------------------
  	DMA_DeInit(DMA1_Channel1);
  	DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;
  	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&ADC_Val;
  	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
//  	DMA_InitStructure.DMA_BufferSize = 6;
	  	DMA_InitStructure.DMA_BufferSize = 4;
//      DMA_InitStructure.DMA_BufferSize = 2;
//  	DMA_InitStructure.DMA_BufferSize = 1;
  	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  	DMA_Init(DMA1_Channel1, &DMA_InitStructure);
  	// Enable DMA1 channel1 //
  	DMA_Cmd(DMA1_Channel1, ENABLE);

  	// Enable DMA1 IT interupt //
	DMA_ITConfig(DMA1_Channel1,DMA_IT_TC,ENABLE);	

  	// ADC1 configuration ------------------------------------------------------//
  	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
  	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
  	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
//  	ADC_InitStructure.ADC_NbrOfChannel = 6;
  		ADC_InitStructure.ADC_NbrOfChannel = 4;
//      ADC_InitStructure.ADC_NbrOfChannel = 2;
//  	ADC_InitStructure.ADC_NbrOfChannel = 1;
  	ADC_Init(ADC1, &ADC_InitStructure);
  	
  	// ADC1 regular channel0,11,12,13 configuration
//  	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);			//ch_0 :
	  	ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_55Cycles5);		//ch_10 : 
       	ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 2, ADC_SampleTime_55Cycles5);        //ch_11 : 
  		ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 3, ADC_SampleTime_55Cycles5);
  		ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 4, ADC_SampleTime_55Cycles5);
//  	ADC_RegularChannelConfig(ADC1, ADC_Channel_14, 5, ADC_SampleTime_55Cycles5);
//  	ADC_RegularChannelConfig(ADC1, ADC_Channel_15, 6, ADC_SampleTime_55Cycles5);
  	// Enable ADC1 EOC interupt
	//ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);
  	
  	// Enable ADC1 DMA 
  	ADC_DMACmd(ADC1, ENABLE);
  	
  	// Enable ADC1
  	ADC_Cmd(ADC1, ENABLE);
  	
  	// Enable ADC1 reset calibaration register 
  	ADC_ResetCalibration(ADC1);
  	// Check the end of ADC1 reset calibration register
  	while(ADC_GetResetCalibrationStatus(ADC1));
  	
  	// Start ADC1 calibaration
  	ADC_StartCalibration(ADC1);
  	// Check the end of ADC1 calibration
  	while(ADC_GetCalibrationStatus(ADC1));
  	   
  	// Start ADC1 Software Conversion 
  	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
*/
  }
}
#endif
/*-----------------------------------------------------------------------------
 Function Name  : TIM1_Configuration
 Description    : Configures the SPI.
 Input          : None
 Output         : None
 Return         : None
 Comments	 	: set timer per 10usec * 5 = 50us
-----------------------------------------------------------------------------*/
void TIM1_Configuration(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  	
  	// Set timer period 10usec * 25 = 250usc
	//20140408 : lower clock-> for power consumption
  	//TIM_TimeBaseStructure.TIM_Prescaler = 720/(mode_AHB_prescaler);  	// 10us resolution on 72mhz PLL
  	#ifdef _STM32_F2XX_144PIN
	TIM_TimeBaseStructure.TIM_Prescaler = 240/(mode_AHB_prescaler);
	#else
  	TIM_TimeBaseStructure.TIM_Prescaler = 360/(mode_AHB_prescaler);  	// 10us resolution on 36mhz PLL
  	#endif

  	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  	//TIM_TimeBaseStructure.TIM_Period = 25;  		// 10usec *25 = 250usec
  	TIM_TimeBaseStructure.TIM_Period = 5	;  		// 10usec * 5 =  50usec
  	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
  	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

  	// Clear update interrupt bit
  	TIM_ClearITPendingBit(TIM1,TIM_IT_Update);
  	// Enable update interrupt
  	TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE);
	//TIM_GenerateEvent(TIM1,TIM_EventSource_Update);
	//TIM_UpdateDisableConfig(TIM1,ENABLE);
	//TIM_UpdateRequestConfig(TIM1,ENABLE);
  	/* TIM1 enable counter */
  	TIM_Cmd(TIM1, ENABLE);
}
//20110824
#ifdef TIMER2_ENABLE	
void TIM2_Configuration(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  	
  	// Set timer period 10usec * 25 = 250usc
	//20140408 : lower clock-> for power consumption
  	//TIM_TimeBaseStructure.TIM_Prescaler = 720/(mode_AHB_prescaler);  	// 10us resolution on 72mhz PLL
  	TIM_TimeBaseStructure.TIM_Prescaler = 360/(mode_AHB_prescaler);  	// 10us resolution on 36mhz PLL

  	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  	TIM_TimeBaseStructure.TIM_Period = 25;  		// 25*10us = 250usec
  	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
  	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

  	// Clear update interrupt bit
  	TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
  	// Enable update interrupt
  	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
	//TIM_GenerateEvent(TIM2,TIM_EventSource_Update);
	//TIM_UpdateDisableConfig(TIM2,ENABLE);
	//TIM_UpdateRequestConfig(TIM2,ENABLE);
  	/* TIM2 enable counter */
  	TIM_Cmd(TIM2, ENABLE);
}
#endif
/*-----------------------------------------------------------------------------
 Function Name  : SYSTICK_Configuration
 Description    : SysTick end of count event each 1ms with input clock equal to 9MHz (HCLK/8, default)
 Input          : None
 Output         : None
 Return         : None
 Comments	 	: ..
-----------------------------------------------------------------------------*/
void SYSTICK_Configuration(void)
{
#ifdef _STM32_F2XX_144PIN
  /* Configure Systick clock source as HCLK */
  RCC_ClocksTypeDef RCC_Clocks;
  SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);

  /* SystTick configuration: an interrupt every 1ms */
  RCC_GetClocksFreq(&RCC_Clocks);
  SysTick_Config(RCC_Clocks.HCLK_Frequency/1000);
#else
  /*  */
  //20140408 : lower clock-> for power consumption
  //SysTick_SetReload( 9000/(mode_AHB_prescaler) );		//case of sysclock 72mhz
  SysTick_SetReload( 4500/(mode_AHB_prescaler) );			//case of sysclock 36mhz

  /* Enable SysTick interrupt */
  SysTick_ITConfig(ENABLE);
  /* Enable the SysTick Counter */
  SysTick_CounterCmd(SysTick_Counter_Enable);
#endif

}

#ifdef _ETHERNET
/*-----------------------------------------------------------------------------
 Function Name  : Ethernet GPIO Configuration
 Description    : Configures the ETHERNET.
 Input          : None
 Output         : None
 Return         : None
 Comments	 	: ..
-----------------------------------------------------------------------------*/
void ETH_GPIO_Config(void)
{

   /* Ethernet pins configuration ************************************************/
   /*
        //ETH_INT  ----------------------------> PB14
        ETH_RMII_TXD0 -----------------------> PB12
        ETH_RMII_TXD1 -----------------------> PB13
        ETH_RMII_RXD0 -----------------------> PC4
        ETH_RMII_RXD1 -----------------------> PC5
        ETH_MDIO ----------------------------> PA2
        ETH_RMII_REF_CLK --------------------> PA1
        ETH_MII_CRS -------------------------> PA0
        ETH_MII_TX_CLK ----------------------> PC3
        ETH_MDC -----------------------------> PC1
        ETH_RMII_CRS_DV ---------------------> PA7
        //ETH_RESET----------------------------> PF15
        //ETH_PWR_EN --------------------------> PG0
        ETH_RMII_TX_EN ----------------------> PB11
        ETH_MII_RX_ER -----------------------> PB10
                                                  */    
    GPIO_InitTypeDef GPIO_InitStructure;

    //interrupt
    GPIO_InitStructure.GPIO_Pin  =GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    //RESET(PF11) Power enable/disable(PF12)
    GPIO_InitStructure.GPIO_Pin  =GPIO_Pin_11 | GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOF, &GPIO_InitStructure);
	
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_ETH);

	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource12, GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_ETH);

	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_1 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource1, GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource3, GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource4, GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource5, GPIO_AF_ETH);


	SYSCFG_ETH_MediaInterfaceConfig(SYSCFG_ETH_MediaInterface_RMII);
 
}


/**
  * @brief  EXTI configuration for Ethernet link status.
  * @param PHYAddress: external PHY address  
  * @retval None
  */
void Eth_Link_EXTIConfig(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Enable the INT (PB14) Clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

  /* Configure INT pin as input */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  /* Connect EXTI Line to INT Pin */
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource14);

  /* Configure EXTI line */
  EXTI_InitStructure.EXTI_Line = EXTI_Line14;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);


  /* Enable and set the EXTI interrupt to the highest priority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  
  NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

}


/**
  * @brief  This function handles Ethernet link status.
  * @param  None
  * @retval None
  */
void Eth_Link_ITHandler(uint16_t PHYAddress)
{
  /* Check whether the link interrupt has occurred or not */
  if(((ETH_ReadPHYRegister(PHYAddress, PHY_MISR)) & PHY_LINK_STATUS) != 0)
  {
    EthLinkStatus = ~EthLinkStatus;

#ifdef USE_LCD
    /* Set the LCD Text Color */
    LCD_SetTextColor(Red);

    if(EthLinkStatus != 0)
    {
      /* Display message on the LCD */
      LCD_DisplayStringLine(Line5, (uint8_t*)"  Network Cable is  ");
      LCD_DisplayStringLine(Line6, (uint8_t*)"     unplugged      ");
    }
    else
    {
      /* Display message on the LCD */
      LCD_DisplayStringLine(Line5, (uint8_t*)"  Network Cable is  ");
      LCD_DisplayStringLine(Line6, (uint8_t*)"   now connected    ");
    }
#endif
  }
}
/**
  * @brief  Configures the Ethernet Interface
  * @param  None
  * @retval None
  */
void ETH_MACDMA_Config(void)
{
  ETH_InitTypeDef ETH_InitStructure;
  
  /* ETHERNET Configuration --------------------------------------------------*/
  /* Call ETH_StructInit if you don't like to configure all ETH_InitStructure parameter */
  ETH_StructInit(&ETH_InitStructure);

  /* Fill ETH_InitStructure parametrs */
  /*------------------------   MAC   -----------------------------------*/
  ETH_InitStructure.ETH_AutoNegotiation = ETH_AutoNegotiation_Enable;
  //ETH_InitStructure.ETH_AutoNegotiation = ETH_AutoNegotiation_Disable; 
  //  ETH_InitStructure.ETH_Speed = ETH_Speed_10M;
  //  ETH_InitStructure.ETH_Mode = ETH_Mode_FullDuplex;   

  ETH_InitStructure.ETH_LoopbackMode = ETH_LoopbackMode_Disable;
  ETH_InitStructure.ETH_RetryTransmission = ETH_RetryTransmission_Disable;
  ETH_InitStructure.ETH_AutomaticPadCRCStrip = ETH_AutomaticPadCRCStrip_Disable;
  ETH_InitStructure.ETH_ReceiveAll = ETH_ReceiveAll_Disable;
  ETH_InitStructure.ETH_BroadcastFramesReception = ETH_BroadcastFramesReception_Enable;
  ETH_InitStructure.ETH_PromiscuousMode = ETH_PromiscuousMode_Disable;
  ETH_InitStructure.ETH_MulticastFramesFilter = ETH_MulticastFramesFilter_Perfect;
  ETH_InitStructure.ETH_UnicastFramesFilter = ETH_UnicastFramesFilter_Perfect;
#ifdef CHECKSUM_BY_HARDWARE
  ETH_InitStructure.ETH_ChecksumOffload = ETH_ChecksumOffload_Enable;
#endif

  /*------------------------   DMA   -----------------------------------*/  
  
  /* When we use the Checksum offload feature, we need to enable the Store and Forward mode: 
  the store and forward guarantee that a whole frame is stored in the FIFO, so the MAC can insert/verify the checksum, 
  if the checksum is OK the DMA can handle the frame otherwise the frame is dropped */
  ETH_InitStructure.ETH_DropTCPIPChecksumErrorFrame = ETH_DropTCPIPChecksumErrorFrame_Enable; 
  ETH_InitStructure.ETH_ReceiveStoreForward = ETH_ReceiveStoreForward_Enable;         
  ETH_InitStructure.ETH_TransmitStoreForward = ETH_TransmitStoreForward_Enable;     
 
  ETH_InitStructure.ETH_ForwardErrorFrames = ETH_ForwardErrorFrames_Disable;       
  ETH_InitStructure.ETH_ForwardUndersizedGoodFrames = ETH_ForwardUndersizedGoodFrames_Disable;   
  ETH_InitStructure.ETH_SecondFrameOperate = ETH_SecondFrameOperate_Enable;
  ETH_InitStructure.ETH_AddressAlignedBeats = ETH_AddressAlignedBeats_Enable;      
  ETH_InitStructure.ETH_FixedBurst = ETH_FixedBurst_Enable;                
  ETH_InitStructure.ETH_RxDMABurstLength = ETH_RxDMABurstLength_32Beat;          
  ETH_InitStructure.ETH_TxDMABurstLength = ETH_TxDMABurstLength_32Beat;
  ETH_InitStructure.ETH_DMAArbitration = ETH_DMAArbitration_RoundRobin_RxTx_2_1;

  /* Configure Ethernet */
  //EthInitStatus = ETH_Init(&ETH_InitStructure, LAN8720_PHY_ADDRESS);
  if(ETH_Init_no_loop(&ETH_InitStructure, LAN8720_PHY_ADDRESS))
  	{
      EthInitStatus=3;
    }
}
/**
  * @brief  Configure the PHY to generate an interrupt on change of link status.
  * @param PHYAddress: external PHY address  
  * @retval None
  */
uint32_t Eth_Link_PHYITConfig(uint16_t PHYAddress)
{
  uint32_t tmpreg = 0;

  /* Read MICR register */
  tmpreg = ETH_ReadPHYRegister(PHYAddress, PHY_MICR);

  /* Enable output interrupt events to signal via the INT pin */
  tmpreg |= (uint32_t)PHY_MICR_INT_EN;
  if(!(ETH_WritePHYRegister(PHYAddress, PHY_MICR, tmpreg)))
  {
    /* Return ERROR in case of write timeout */
    return ETH_ERROR;
  }
  /* Return SUCCESS */
  return ETH_SUCCESS;   
}
#endif //ETHERNET

#ifndef _RKB_DM8100
#ifdef POWER_CONSUMPTION	//20121030
/*-----------------------------------------------------------------------------
 Function Name  : SetConfigUART2n3ToTelechipsReciva
 Description    : SysTick end of count event each 1ms with input clock equal to 9MHz (HCLK/8, default)
 Input          : None
 Output         : None
 Return         : None
 Comments	 	:   (0) start power consumption => Tx port => [ GPIO_Mode_IN_FLOATING ]
                    (1) normal => Tx port => [ GPIO_Mode_AF_PP ]
                    RA-1570 :   uart1 : RS232
                                uart2 : not used
                                uart3 : not used
                                uart4 : not used
                                uart5 : not used
-----------------------------------------------------------------------------*/
void SetConfigUART2n3ToTelechipsReciva(unsigned char on)
{
  	GPIO_InitTypeDef GPIO_InitStructure;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;

  	if (on == 1) {
    //----------power_normal start-----------------------------------------------------------
#ifdef UART2_ENABLE	
  		/* configure USART2 Tx (PA.2) as alternate function push-pull */        //Telechips
  		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  		GPIO_Init(GPIOA, &GPIO_InitStructure);
#endif
		
#ifdef UART3_ENABLE	
  		/* configure USART3 Tx (PB.10) as alternate function push-pull */       //not used
  		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  		GPIO_Init(GPIOB, &GPIO_InitStructure);
#endif

#ifdef UART4_ENABLE
      	/* configure UART4 Tx (PC.10) as alternate function push-pull */        
      	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
      	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
      	GPIO_Init(GPIOC, &GPIO_InitStructure);
#endif
#ifdef UART5_ENABLE
       	/* configure UART5 Tx (PC.12) as alternate function push-pull */        
      	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
      	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
      	GPIO_Init(GPIOC, &GPIO_InitStructure);      
#endif
  	} else {
    //----------power_consumption start-----------------------------------------------------------
#ifdef UART2_ENABLE	
	  	/* configure USART2 Tx (PA.2) as alternate function push-pull */
  		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;                               //Telechips
  		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  		GPIO_Init(GPIOA, &GPIO_InitStructure);
#endif
	  	
#ifdef UART3_ENABLE	
	  	/* configure USART3 Tx (PB.10) as alternate function push-pull */       //not used
  		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  		GPIO_Init(GPIOB, &GPIO_InitStructure);
#endif

#ifdef UART4_ENABLE
      	/* configure UART4 Tx (PC.10) as alternate function push-pull */        //RotelLink 유지해야함...        
      	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
      	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
      	GPIO_Init(GPIOC, &GPIO_InitStructure);
#endif
#ifdef UART5_ENABLE
       	/* configure UART5 Tx (PC.12) as alternate function push-pull */              
      	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
      	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
      	GPIO_Init(GPIOC, &GPIO_InitStructure);       
#endif
  	}
}

#endif  //POWER_CONSUMPTION
#endif //_RKB_DM8100

/* end of file */
/******************* (C) COPYRIGHT 2008 Rotel Technology Global **************/

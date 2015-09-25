/**************** (c)2008 Rotel Technology Global Inc..*************************
PROJECT         : RKB-850,8100,D850,D8100
COMPILER        : IAR Embeded Workbench 5.50
MODULE          : RKB_initmcu.c
VERSION         : V 1.0
USED CHIP       : RKB (stm32f101RC) [T=36pin,C=48pin,R=64pin,V=100pin : 6=32KB/6KB,8=64KB/10KB,B=128KB/16KB,C=256KB/32KB]
CREATION DATE   : /2013
AUTHORS         : /Rotel
DESCRIPTION     :
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H__
#define __MAIN_H__

extern __IO uint32_t  EthInitStatus;
// for capture compare interrupt
//extern vu16 CCR1_Val;
//extern vu16 CCR2_Val;
//extern vu16 CCR3_Val;
//extern vu16 CCR4_Val;

extern u8 mode_AHB_prescaler;
extern u8 rkb_mode_id;

/*----------------------------------------------------------------------------*/
void RCC_Configuration(void);
void NVIC_Configuration(void);
void GPIO_Configuration(unsigned char type);                    //20121030
void DMA_Configuration(void);
void UART_Configuration(void);
void TIM1_Configuration(void);
void SYSTICK_Configuration(void);
//void ADC_Configuration(void);
void ADC_Configuration(unsigned char type);						//20110719
void adc_SetInterruptDMAForADC(FunctionalState NewState);
void ini_DisableInterrupt(void);
void ini_InitMcu(void);

#ifdef POWER_CONSUMPTION	//20121030
void SetConfigUART2n3ToTelechipsReciva(unsigned char on);
#endif

void ADC_Configuration_PP(unsigned char type);					//20131024
void GPIO_Configuration_PP(unsigned char type);					//20131024
void GPIO_Configuration_MP2(unsigned char type);				//20140523

void Eth_Link_ITHandler(uint16_t PHYAddress);
void Eth_Link_EXTIConfig(void);
void ETH_GPIO_Config(void);
void ETH_MACDMA_Config(void);
uint32_t Eth_Link_PHYITConfig(uint16_t PHYAddress);
#endif /* __MAIN_H__ */

/* end of file */
/******************* (C) COPYRIGHT 2008 Rotel Technology Global **************/

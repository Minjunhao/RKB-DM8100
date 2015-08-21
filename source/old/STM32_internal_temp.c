/*******************************************************************************
	STM32F Internal Temperature Sensor :
	
*******************************************************************************/	
/* Includes ------------------------------------------------------------------*/
#include "stm32f4_discovery.h"
#include <stdio.h>
#include "main.h"
#include <math.h>
 
#define ADC1_DR_ADDRESS    ((uint32_t)0x4001204C)
 
/*************declaration*****************/
double Voltage = 0;
__IO uint32_t ValADC = 0;
double temp = 0;
 
void ADC1_DMA_Config(void);
void Delay(__IO uint32_t nCount);
 
/*********************Configuration ADC&DMA**********************************************/
 
void ADC1_DMA_Config(void)
{
  ADC_InitTypeDef       ADC_InitStructure;
  ADC_CommonInitTypeDef ADC_CommonInitStructure;
  DMA_InitTypeDef       DMA_InitStructure;
  GPIO_InitTypeDef      GPIO_InitStructure;
 
 
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2| RCC_AHB1Periph_GPIOD, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
 
  DMA_DeInit(DMA2_Stream0);
  DMA_InitStructure.DMA_Channel = DMA_Channel_0;
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)ADC1_DR_ADDRESS;
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&ValADC;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
  DMA_InitStructure.DMA_BufferSize = 1;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_Init(DMA2_Stream0, &DMA_InitStructure);
  DMA_Cmd(DMA2_Stream0, ENABLE);
 
 
  GPIO_DeInit(GPIOD);
 
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_12 |  GPIO_Pin_13 |  GPIO_Pin_14|  GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode =GPIO_Mode_OUT ;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN   ;
  GPIO_Init(GPIOD,&GPIO_InitStructure);
 
 
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
  ADC_CommonInit(&ADC_CommonInitStructure);
 
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStructure.ADC_ExternalTrigConv = 0;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfConversion = 1;
  ADC_Init(ADC1, &ADC_InitStructure);
 
  ADC_DMACmd(ADC1, ENABLE);
 
 ADC_RegularChannelConfig(ADC1, ADC_Channel_TempSensor, 1, ADC_SampleTime_480Cycles);
 
 ADC_TempSensorVrefintCmd(ENABLE);
 
 ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);
 
 ADC_Cmd(ADC1, ENABLE);
}
 
 
 
/****************************Programme Principal********************************************/
 
 
int main(void)
{
 
 
ADC1_DMA_Config();
 
ADC_SoftwareStartConv(ADC1);
 
 
    Voltage = (ValADC *3300)/4095;
    temp = ((Voltage - 760)/2.5)+25;
 
 
/********************************************/
 
void Delay(__IO uint32_t nCount)
{
  while(nCount--)
  {
  }
}
 
#ifdef  USE_FULL_ASSERT
 
void assert_failed(uint8_t* file, uint32_t line)
{
 
  while (1)
  {
  }
}
#endif

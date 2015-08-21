/*******************************************************************************
	STM32 Temperature :
	
				min		typ		max	
	V25			1.32	1.41	1.50	[V]
	Avg_slope 	4.0		4.3		4.6		[mV/C]
	
	Vtemp_sensor(u32)	=	(advalue) * 330 / 4096				[ 3.3V / 12bits ]
	Current_Temp(s32)	=	(s32)(V25 - Vtemp_sensor) / Avg_slope + 25
	
	return	(s16)Current_Temp
	printf	(Temp/100), (Temp%100)
	
	==>
    Vtemp_sensor = advalue * 330 / 4096;   
    Current_Temp = (s32)(143 - Vtemp_sensor)*10000/43 + 2500;    
    return (s16)Current_Temp;
    
*******************************************************************************/   
   
/* Includes ------------------------------------------------------------------*/   
#include "stm32f10x_lib.h"   
#include "stdio.h"   
   
/* Private typedef -----------------------------------------------------------*/   
/* Private define ------------------------------------------------------------*/   
#define ADC1_DR_Address    ((u32)0x4001244C)   
/* Private macro -------------------------------------------------------------*/   
/* Private variables ---------------------------------------------------------*/   
vu16 AD_Value[2];   
vu16 i=0;   
s16  Temp;   
u16  Volt;   
   
/* Private function prototypes -----------------------------------------------*/   
void RCC_Configuration(void);   
void GPIO_Configuration(void);   
void NVIC_Configuration(void);   
void USART1_Configuration(void);   
void ADC1_Configuration(void);   
void DMA_Configuration(void);   
   
int fputc(int ch, FILE *f);   
void Delay(void);   
u16 GetTemp(u16 advalue);   
u16 GetVolt(u16 advalue);   
/* Private functions ---------------------------------------------------------*/   
/*******************************************************************************  
* Function Name  : main  
* Description    : Main program.  
* Input          : None  
* Output         : None  
* Return         : None  
*******************************************************************************/   
int main(void)   
{   
    RCC_Configuration();   
    GPIO_Configuration();   
    NVIC_Configuration();   
    USART1_Configuration();   
    DMA_Configuration();   
    ADC1_Configuration();   
       
    //������һ��ADת��   
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);    
    //��Ϊ�Ѿ����ú���DMA��������AD�Զ�����ת��������Զ�������AD_Value��      
       
    while (1)   
    {   
        Delay();   
        Temp = GetTemp(AD_Value[1]);   
        Volt = GetVolt(AD_Value[0]);   
        USART_SendData(USART1, 0x0c);       //����   
        //ע�⣬USART_SendData����������Ƿ������   
        //�ȴ��������   
        while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);   
   
        printf("��ѹ��%d.%d\t�¶ȣ�%d.%d��\r\n", \   
            Volt/100, Volt%100, Temp/100, Temp%100);   
           
    }   
}   
   
/*******************************************************************************  
* Function Name  : �ض���ϵͳputchar����int fputc(int ch, FILE *f)  
* Description    : ���ڷ�һ���ֽ�  
* Input          : int ch, FILE *f  
* Output         :   
* Return         : int ch  
*******************************************************************************/   
int fputc(int ch, FILE *f)   
{   
    //USART_SendData(USART1, (u8) ch);   
    USART1->DR = (u8) ch;   
       
    /* Loop until the end of transmission */   
    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)   
    {   
    }   
   
    return ch;   
}   
   
/*******************************************************************************  
* Function Name  : Delay  
* Description    : ��ʱ����  
* Input          : None  
* Output         : None  
* Return         : None  
*******************************************************************************/   
void Delay(void)   
{   
    u32 i;   
    for(i=0;i<0x4f0000;i++);   
    return;   
}                               
   
/*******************************************************************************  
* Function Name  : GetTemp  
* Description    :  
* Input          : u16 advalue  
* Output         :   
* Return         : u16 temp  
*******************************************************************************/   
u16 GetTemp(u16 advalue)   
{   
    u32 Vtemp_sensor;   
    s32 Current_Temp;   
       
//    ADCת�������Ժ󣬶�ȡADC_DR�Ĵ����еĽ����ת���¶�ֵ���㹫ʽ���£�   
//          V25 - VSENSE   
//  T(��) = ------------  + 25   
//           Avg_Slope   
//   V25��  �¶ȴ�������25��ʱ �������ѹ������ֵ1.43 V��   
//  VSENSE���¶ȴ������ĵ�ǰ�����ѹ����ADC_DR �Ĵ����еĽ��ADC_ConvertedValue֮���ת����ϵΪ��    
//            ADC_ConvertedValue * Vdd   
//  VSENSE = --------------------------   
//            Vdd_convert_value(0xFFF)   
//  Avg_Slope���¶ȴ����������ѹ���¶ȵĹ�������������ֵ4.3 mV/�档   
   
    Vtemp_sensor = advalue * 330 / 4096;   
    Current_Temp = (s32)(143 - Vtemp_sensor)*10000/43 + 2500;    
    return (s16)Current_Temp;   
}     
   
   
/*******************************************************************************  
* Function Name  : GetVolt  
* Description    : ����ADC��������ѹ  
* Input          : u16 advalue  
* Output         :   
* Return         : u16 temp  
*******************************************************************************/   
u16 GetVolt(u16 advalue)   
{   
    return (u16)(advalue * 330 / 4096);   
}    
                             
   
/*******************************************************************************  
* Function Name  : RCC_Configuration  
* Description    : ϵͳʱ������  
* Input          : None  
* Output         : None  
* Return         : None  
*******************************************************************************/   
void RCC_Configuration(void)   
{   
    ErrorStatus HSEStartUpStatus;   
   
    //ʹ���ⲿ����   
    RCC_HSEConfig(RCC_HSE_ON);   
    //�ȴ��ⲿ�����ȶ�   
    HSEStartUpStatus = RCC_WaitForHSEStartUp();   
    //����ⲿ���������ɹ����������һ������   
    if(HSEStartUpStatus==SUCCESS)   
    {   
        //����HCLK��AHBʱ�ӣ�=SYSCLK   
        RCC_HCLKConfig(RCC_SYSCLK_Div1);   
   
        //PCLK1(APB1) = HCLK/2   
        RCC_PCLK1Config(RCC_HCLK_Div2);   
   
        //PCLK2(APB2) = HCLK   
        RCC_PCLK2Config(RCC_HCLK_Div1);   
           
        //����ADCʱ��Ƶ��   
        RCC_ADCCLKConfig(RCC_PCLK2_Div2);    
   
        //FLASHʱ�����   
        //�Ƽ�ֵ��SYSCLK = 0~24MHz   Latency=0   
        //        SYSCLK = 24~48MHz  Latency=1   
        //        SYSCLK = 48~72MHz  Latency=2   
        FLASH_SetLatency(FLASH_Latency_2);   
        //����FLASHԤȡָ����   
        FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);   
   
        //PLL���� SYSCLK/1 * 9 = 8*1*9 = 72MHz   
        RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);   
        //����PLL   
        RCC_PLLCmd(ENABLE);   
        //�ȴ�PLL�ȶ�   
        while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);   
        //ϵͳʱ��SYSCLK����PLL���   
        RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);   
        //�л�ʱ�Ӻ�ȴ�ϵͳʱ���ȶ�   
        while(RCC_GetSYSCLKSource()!=0x08);   
   
           
    }   
   
    //�����Ǹ���ģ�鿪��ʱ��   
    //����GPIO   
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | \   
                           RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD,\   
                           ENABLE);   
    //����AFIO   
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);   
    //����USART1   
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);   
    //����DMAʱ��   
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);   
    //����ADC1ʱ��   
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);   
   
}   
   
   
/*******************************************************************************  
* Function Name  : GPIO_Configuration  
* Description    : GPIO����  
* Input          : None  
* Output         : None  
* Return         : None  
*******************************************************************************/   
void GPIO_Configuration(void)   
{   
    GPIO_InitTypeDef GPIO_InitStructure;   
   
    //PC��4567������GPIO��������� 2M   
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;   
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;   
    GPIO_Init(GPIOC, &GPIO_InitStructure);   
   
    //KEY2 KEY3 JOYKEY   
    //λ��PD�ڵ�3 4 11-15�ţ�ʹ������Ϊ����   
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_11 | GPIO_Pin_12 |\   
        GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;   
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   
    GPIO_Init(GPIOD, &GPIO_InitStructure);   
   
    //USART1_TX   
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;   
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;   
    GPIO_Init(GPIOA, &GPIO_InitStructure);   
       
    //USART1_RX   
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;   
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   
    GPIO_Init(GPIOA, &GPIO_InitStructure);   
       
    //ADC_CH10--> PC0   
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;   
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;   
    GPIO_Init(GPIOC, &GPIO_InitStructure);    
   
}   
   
   
   
/*******************************************************************************  
* Function Name  : NVIC_Configuration  
* Description    : NVIC����  
* Input          : None  
* Output         : None  
* Return         : None  
*******************************************************************************/   
void NVIC_Configuration(void)   
{   
    NVIC_InitTypeDef NVIC_InitStructure;   
   
#ifdef  VECT_TAB_RAM   
    // Set the Vector Table base location at 0x20000000   
    NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);   
#else  /* VECT_TAB_FLASH  */   
    // Set the Vector Table base location at 0x08000000   
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
#endif   
   
    //����NVIC���ȼ�����ΪGroup2��0-3��ռʽ���ȼ���0-3����Ӧʽ���ȼ�   
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);   
    //�����жϴ�   
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQChannel;   
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;   
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;   
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;   
    NVIC_Init(&NVIC_InitStructure);   
}   
   
   
/*******************************************************************************  
* Function Name  : USART1_Configuration  
* Description    : NUSART1����  
* Input          : None  
* Output         : None  
* Return         : None  
*******************************************************************************/   
void USART1_Configuration(void)   
{   
    USART_InitTypeDef USART_InitStructure;   
       
    USART_InitStructure.USART_BaudRate = 19200;   
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;   
    USART_InitStructure.USART_StopBits = USART_StopBits_1;   
    USART_InitStructure.USART_Parity = USART_Parity_No;   
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;   
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;   
    USART_Init(USART1, &USART_InitStructure);   
       
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);   
       
    USART_Cmd(USART1, ENABLE);   
}   
   
/*******************************************************************************  
* Function Name  : ADC1_Configuration  
* Description    : ADC1���ã�����ADCģ�����ú���У׼��  
* Input          : None  
* Output         : None  
* Return         : None  
*******************************************************************************/   
void ADC1_Configuration(void)   
{   
    ADC_InitTypeDef ADC_InitStructure;   
   
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;   
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;   
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;  //����ת������   
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;   
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;   
    ADC_InitStructure.ADC_NbrOfChannel = 2;     //����ת�����г���Ϊ2   
    ADC_Init(ADC1, &ADC_InitStructure);   
       
    //ADC�����¶ȴ�����ʹ�ܣ�Ҫʹ��Ƭ���¶ȴ��������м�Ҫ��������   
    ADC_TempSensorVrefintCmd(ENABLE);   
       
    //����ת������1��ͨ��10   
    ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_13Cycles5);   
    //����ת������2��ͨ��16���ڲ��¶ȴ�������������ʱ��>2.2us,(239cycles)   
    ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 2, ADC_SampleTime_239Cycles5);   
       
    // Enable ADC1   
    ADC_Cmd(ADC1, ENABLE);   
    // ����ADC��DMA֧�֣�Ҫʵ��DMA���ܣ������������DMAͨ���Ȳ�����   
    ADC_DMACmd(ADC1, ENABLE);   
       
    // ������ADC�Զ�У׼����������ִ��һ�Σ���֤����   
    // Enable ADC1 reset calibaration register    
    ADC_ResetCalibration(ADC1);   
    // Check the end of ADC1 reset calibration register   
    while(ADC_GetResetCalibrationStatus(ADC1));   
   
    // Start ADC1 calibaration   
    ADC_StartCalibration(ADC1);   
    // Check the end of ADC1 calibration   
    while(ADC_GetCalibrationStatus(ADC1));   
    // ADC�Զ�У׼����---------------   
       
}   
   
/*******************************************************************************  
* Function Name  : DMA_Configuration  
* Description    : DMA���ã���ADCģ���Զ���ת��������ڴ�  
* Input          : None  
* Output         : None  
* Return         : None  
*******************************************************************************/   
void DMA_Configuration(void)   
{   
    DMA_InitTypeDef DMA_InitStructure;   
       
    DMA_DeInit(DMA1_Channel1);   
    DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;   
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&AD_Value;   
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;   
    //BufferSize=2����ΪADCת��������2��ͨ��   
    //������ã�ʹ����1�������AD_Value[0]������2�������AD_Value[1]   
    DMA_InitStructure.DMA_BufferSize = 2;   
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;   
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;   
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;   
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;   
    //ѭ��ģʽ������Bufferд�����Զ��ص���ʼ��ַ��ʼ����   
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;   
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;   
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;   
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);   
    //������ɺ�����DMAͨ��   
    DMA_Cmd(DMA1_Channel1, ENABLE);   
}   

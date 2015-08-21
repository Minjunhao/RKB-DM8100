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
       
    //启动第一次AD转换   
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);    
    //因为已经配置好了DMA，接下来AD自动连续转换，结果自动保存在AD_Value处      
       
    while (1)   
    {   
        Delay();   
        Temp = GetTemp(AD_Value[1]);   
        Volt = GetVolt(AD_Value[0]);   
        USART_SendData(USART1, 0x0c);       //清屏   
        //注意，USART_SendData函数不检查是否发送完成   
        //等待发送完成   
        while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);   
   
        printf("电压：%d.%d\t温度：%d.%d℃\r\n", \   
            Volt/100, Volt%100, Temp/100, Temp%100);   
           
    }   
}   
   
/*******************************************************************************  
* Function Name  : 重定义系统putchar函数int fputc(int ch, FILE *f)  
* Description    : 串口发一个字节  
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
* Description    : 延时函数  
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
       
//    ADC转换结束以后，读取ADC_DR寄存器中的结果，转换温度值计算公式如下：   
//          V25 - VSENSE   
//  T(℃) = ------------  + 25   
//           Avg_Slope   
//   V25：  温度传感器在25℃时 的输出电压，典型值1.43 V。   
//  VSENSE：温度传感器的当前输出电压，与ADC_DR 寄存器中的结果ADC_ConvertedValue之间的转换关系为：    
//            ADC_ConvertedValue * Vdd   
//  VSENSE = --------------------------   
//            Vdd_convert_value(0xFFF)   
//  Avg_Slope：温度传感器输出电压和温度的关联参数，典型值4.3 mV/℃。   
   
    Vtemp_sensor = advalue * 330 / 4096;   
    Current_Temp = (s32)(143 - Vtemp_sensor)*10000/43 + 2500;    
    return (s16)Current_Temp;   
}     
   
   
/*******************************************************************************  
* Function Name  : GetVolt  
* Description    : 根据ADC结果计算电压  
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
* Description    : 系统时钟设置  
* Input          : None  
* Output         : None  
* Return         : None  
*******************************************************************************/   
void RCC_Configuration(void)   
{   
    ErrorStatus HSEStartUpStatus;   
   
    //使能外部晶振   
    RCC_HSEConfig(RCC_HSE_ON);   
    //等待外部晶振稳定   
    HSEStartUpStatus = RCC_WaitForHSEStartUp();   
    //如果外部晶振启动成功，则进行下一步操作   
    if(HSEStartUpStatus==SUCCESS)   
    {   
        //设置HCLK（AHB时钟）=SYSCLK   
        RCC_HCLKConfig(RCC_SYSCLK_Div1);   
   
        //PCLK1(APB1) = HCLK/2   
        RCC_PCLK1Config(RCC_HCLK_Div2);   
   
        //PCLK2(APB2) = HCLK   
        RCC_PCLK2Config(RCC_HCLK_Div1);   
           
        //设置ADC时钟频率   
        RCC_ADCCLKConfig(RCC_PCLK2_Div2);    
   
        //FLASH时序控制   
        //推荐值：SYSCLK = 0~24MHz   Latency=0   
        //        SYSCLK = 24~48MHz  Latency=1   
        //        SYSCLK = 48~72MHz  Latency=2   
        FLASH_SetLatency(FLASH_Latency_2);   
        //开启FLASH预取指功能   
        FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);   
   
        //PLL设置 SYSCLK/1 * 9 = 8*1*9 = 72MHz   
        RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);   
        //启动PLL   
        RCC_PLLCmd(ENABLE);   
        //等待PLL稳定   
        while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);   
        //系统时钟SYSCLK来自PLL输出   
        RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);   
        //切换时钟后等待系统时钟稳定   
        while(RCC_GetSYSCLKSource()!=0x08);   
   
           
    }   
   
    //下面是给各模块开启时钟   
    //启动GPIO   
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | \   
                           RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD,\   
                           ENABLE);   
    //启动AFIO   
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);   
    //启动USART1   
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);   
    //启动DMA时钟   
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);   
    //启动ADC1时钟   
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);   
   
}   
   
   
/*******************************************************************************  
* Function Name  : GPIO_Configuration  
* Description    : GPIO设置  
* Input          : None  
* Output         : None  
* Return         : None  
*******************************************************************************/   
void GPIO_Configuration(void)   
{   
    GPIO_InitTypeDef GPIO_InitStructure;   
   
    //PC口4567脚设置GPIO输出，推挽 2M   
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;   
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;   
    GPIO_Init(GPIOC, &GPIO_InitStructure);   
   
    //KEY2 KEY3 JOYKEY   
    //位于PD口的3 4 11-15脚，使能设置为输入   
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
* Description    : NVIC设置  
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
   
    //设置NVIC优先级分组为Group2：0-3抢占式优先级，0-3的响应式优先级   
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);   
    //串口中断打开   
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQChannel;   
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;   
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;   
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;   
    NVIC_Init(&NVIC_InitStructure);   
}   
   
   
/*******************************************************************************  
* Function Name  : USART1_Configuration  
* Description    : NUSART1设置  
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
* Description    : ADC1设置（包括ADC模块配置和自校准）  
* Input          : None  
* Output         : None  
* Return         : None  
*******************************************************************************/   
void ADC1_Configuration(void)   
{   
    ADC_InitTypeDef ADC_InitStructure;   
   
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;   
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;   
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;  //连续转换开启   
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;   
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;   
    ADC_InitStructure.ADC_NbrOfChannel = 2;     //设置转换序列长度为2   
    ADC_Init(ADC1, &ADC_InitStructure);   
       
    //ADC内置温度传感器使能（要使用片内温度传感器，切忌要开启它）   
    ADC_TempSensorVrefintCmd(ENABLE);   
       
    //常规转换序列1：通道10   
    ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_13Cycles5);   
    //常规转换序列2：通道16（内部温度传感器），采样时间>2.2us,(239cycles)   
    ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 2, ADC_SampleTime_239Cycles5);   
       
    // Enable ADC1   
    ADC_Cmd(ADC1, ENABLE);   
    // 开启ADC的DMA支持（要实现DMA功能，还需独立配置DMA通道等参数）   
    ADC_DMACmd(ADC1, ENABLE);   
       
    // 下面是ADC自动校准，开机后需执行一次，保证精度   
    // Enable ADC1 reset calibaration register    
    ADC_ResetCalibration(ADC1);   
    // Check the end of ADC1 reset calibration register   
    while(ADC_GetResetCalibrationStatus(ADC1));   
   
    // Start ADC1 calibaration   
    ADC_StartCalibration(ADC1);   
    // Check the end of ADC1 calibration   
    while(ADC_GetCalibrationStatus(ADC1));   
    // ADC自动校准结束---------------   
       
}   
   
/*******************************************************************************  
* Function Name  : DMA_Configuration  
* Description    : DMA设置：从ADC模块自动读转换结果至内存  
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
    //BufferSize=2，因为ADC转换序列有2个通道   
    //如此设置，使序列1结果放在AD_Value[0]，序列2结果放在AD_Value[1]   
    DMA_InitStructure.DMA_BufferSize = 2;   
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;   
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;   
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;   
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;   
    //循环模式开启，Buffer写满后，自动回到初始地址开始传输   
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;   
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;   
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;   
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);   
    //配置完成后，启动DMA通道   
    DMA_Cmd(DMA1_Channel1, ENABLE);   
}   

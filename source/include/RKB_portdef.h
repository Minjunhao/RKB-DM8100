/**************** (c)2012 Rotel Inc..*******************************************
PROJECT         : RKB-850,8100,D850,D8100
COMPILER        : IAR Embeded Workbench 5.50
MODULE          : RKB_portdef.h
VERSION         : V 1.0
USED CHIP       : RKB (stm32f101RC) [T=36pin,C=48pin,R=64pin,V=100pin : 6=32KB/6KB,8=64KB/10KB,B=128KB/16KB,C=256KB/32KB]
CREATION DATE   : /2013
AUTHORS         : /Rotel
DESCRIPTION     :
*******************************************************************************/
#ifndef __PORT_DEF__
#define __PORT_DEF__
#ifdef _STM32_F2XX_144PIN
#define PORTA_PORT              GPIOA
#define SS_ON                   GPIO_Pin_0               //add for SS MODE on
#define RMII_REF_CLK            GPIO_Pin_1
#define ETH_MDIO                GPIO_Pin_2
#define Adummy003               GPIO_Pin_3
#define Adummy004               GPIO_Pin_4
#define Adummy005               GPIO_Pin_5
#define Adummy006               GPIO_Pin_6
#define RMII_CRS_DV             GPIO_Pin_7
#define Adummy008               GPIO_Pin_8
#define UART1_TX                GPIO_Pin_9
#define UART1_RX                GPIO_Pin_10

#define T12V_PORT				GPIOA
#define T12V_IN					GPIO_Pin_11  

#define SIGNAL_SENSING_PORTA	GPIOA
#define SIGNAL_SENSING_nIN		GPIO_Pin_12              //normal: LOW , quick: H

#define JTCK                    GPIO_Pin_14
#define JTDI                    GPIO_Pin_15

#define PORTB_PORT              GPIOB
#define Bdummy000               GPIO_Pin_0
#define Bdummy001               GPIO_Pin_1
#define BOOT1                   GPIO_Pin_2               //BOOT1=LOW
#define JTDO                    GPIO_Pin_3
#define JNTRST                  GPIO_Pin_4
#define Bdummy005               GPIO_Pin_5

//for eeprom port
#define IIC_PORT                GPIOB
#define IIC_SCL                 GPIO_Pin_6          
#define IIC_SDA                 GPIO_Pin_7          

#define NJM2752_PORTB           GPIOB
#define NJM2752_A_SEL           GPIO_Pin_8
#define NJM2752_B_SEL           GPIO_Pin_9
#define ETH_RX_ER               GPIO_Pin_10
#define ETH_TX_EN               GPIO_Pin_11
#define ETH_TXD0                GPIO_Pin_12
#define ETH_TXD1                GPIO_Pin_13
#define ETH_INT                 GPIO_Pin_14
#define Bdummy015               GPIO_Pin_15

#define PORTC_PORT              GPIOC
#define Cdummy000               GPIO_Pin_0
#define ETH_MDC                 GPIO_Pin_1
#define Cdummy002               GPIO_Pin_2
#define ETH_TX_CLK              GPIO_Pin_3
#define ETH_RXD0                GPIO_Pin_4
#define ETH_RXD1                GPIO_Pin_5

#define AMP_PORT                GPIOC
#define AMP_n_READY             GPIO_Pin_6
#define AMP_n_ERROR             GPIO_Pin_7
#define AMP_ON                  GPIO_Pin_8

#define SMPS_PORT               GPIOC
#define SMPS_nON                GPIO_Pin_9
#define UART4_TX                GPIO_Pin_10
#define UART4_RX                GPIO_Pin_11
#define UART5_TX                GPIO_Pin_12
#define Cdummy013               GPIO_Pin_13
#define Cdummy014               GPIO_Pin_14
#define Cdummy015               GPIO_Pin_15

#define PORTD_PORT              GPIOD
#define Mode_Id0                GPIO_Pin_0
#define Mode_Id1                GPIO_Pin_1
#define UART5_RX                GPIO_Pin_2
//#define D_OR_DM8100             GPIO_Pin_3
#define Ddummy003               GPIO_Pin_3

#define FAN_PORT                GPIOD
#define Fan_L                   GPIO_Pin_4
#define Fan_H                   GPIO_Pin_5

#define NJW1194_PORTD           GPIOD
#define NJW1194_LAT             GPIO_Pin_6
#define NJW1194_CLK             GPIO_Pin_7

#define Ddummy008               GPIO_Pin_8
#define Ddummy009               GPIO_Pin_9
#define Ddummy010               GPIO_Pin_10
#define Ddummy011               GPIO_Pin_11
#define Ddummy012               GPIO_Pin_12
#define Ddummy013               GPIO_Pin_13
#define Ddummy014               GPIO_Pin_14
#define Ddummy015               GPIO_Pin_15

#define PORTE_PORT              GPIOE

#define NJM2752_PORTE           GPIOE
#define NJM2752_C_SEL           GPIO_Pin_0
#define NJM2752_D_SEL           GPIO_Pin_1

#define Mode_Option1            GPIO_Pin_2
#define Mode_Option2            GPIO_Pin_3
#define Mode_Set                GPIO_Pin_4                       //H:DM8100, L:M8100
#define Edummy005               GPIO_Pin_5
#define Edummy006               GPIO_Pin_6

#define AK4117_PORTE            GPIOE
#define AK4117_DO               GPIO_Pin_7
#define AK4117_A_INT0           GPIO_Pin_8
#define AK4117_A_CS             GPIO_Pin_9
#define AK4117_B_INT0           GPIO_Pin_10
#define AK4117_B_CS             GPIO_Pin_11
#define AK4117_C_INT0           GPIO_Pin_12
#define AK4117_C_CS             GPIO_Pin_13
#define AK4117_D_INT0           GPIO_Pin_14
#define AK4117_D_CS             GPIO_Pin_15

#define PORTF_PORT              GPIOF
#define F_RMC                   GPIO_Pin_0                     //for testing
#define Fdummy001               GPIO_Pin_1
#define Fdummy002               GPIO_Pin_2

#define NTC_DETECT_PORT         GPIOF
#define NTC_AMP1                GPIO_Pin_3
#define NTC_AMP2                GPIO_Pin_4
#define NTC_SMPS1               GPIO_Pin_5
#define NTC_SMPS2               GPIO_Pin_6
#define F_VOL_A                 GPIO_Pin_7                    //no use
#define F_VOL_B                 GPIO_Pin_8                    //no use
#define F_VOL_C                 GPIO_Pin_9                    //no use
#define F_VOL_D                 GPIO_Pin_10                   //no use

#define ETH_RESET               GPIO_Pin_11
#define ETH_PWR_EN              GPIO_Pin_12
#define Fdummy013               GPIO_Pin_13
#define Fdummy014               GPIO_Pin_14

#define AK4117_PORTF            GPIOF
#define AK4117_RESET            GPIO_Pin_15

#define PORTG_PORT              GPIOG

#define AK4117_PORTG            GPIOG
#define AK4117_CLK              GPIO_Pin_0
#define AK4117_DI               GPIO_Pin_1

#define POWER_RELAY             GPIO_Pin_2
#define POWER_LED               GPIO_Pin_3
#define SENSE_LED               GPIO_Pin_4
#define POWER_DOWN_CHK          GPIO_Pin_5
#define Gdummy006               GPIO_Pin_6
#define Gdummy007               GPIO_Pin_7
#define Gdummy008               GPIO_Pin_8

#define NJW1194_PORTG           GPIOG
#define NJW1194_DAT             GPIO_Pin_9

#define NJW1112_PORT            GPIOG
#define NJW1112_LAT             GPIO_Pin_10
#define NJW1112_CLK             GPIO_Pin_11
#define NJW1112_DAT             GPIO_Pin_12

#define Gdummy013               GPIO_Pin_13
#define Gdummy014               GPIO_Pin_14
#define Factory_Default         GPIO_Pin_15


#define SMPS_nPowerOn(x)		( (x==1)? GPIO_SetBits(SMPS_PORT, SMPS_nON) : GPIO_ResetBits(SMPS_PORT, SMPS_nON) );

#define Standby(x)              ((x==1)? GPIO_SetBits(PORTG_PORT,POWER_RELAY) : GPIO_ResetBits(PORTG_PORT,POWER_RELAY));

//8(3)FAN control
#define FAN_hs_on(x)			( (x==1)? GPIO_SetBits(FAN_PORT, Fan_H) : GPIO_ResetBits(FAN_PORT, Fan_H) );
#define FAN_ls_on(x)            (( x==1)? GPIO_SetBits(FAN_PORT, Fan_L) : GPIO_ResetBits(FAN_PORT, Fan_L) ); 

#define StandbyLED(x)           ( (x==1)? GPIO_SetBits(GPIOG, POWER_LED) : GPIO_ResetBits(GPIOG, POWER_LED) );

#define GetSignalSensing_nSW		    GPIO_ReadInputDataBit(SIGNAL_SENSING_PORTA, SIGNAL_SENSING_nIN)

#define amp_on(x)               ((x==1)? GPIO_SetBits(AMP_PORT,AMP_ON) : GPIO_ResetBits(AMP_PORT,AMP_ON));


#define GetSignalSense          GPIO_ReadInputDataBit(SIGNAL_SENSING_PORTA, SS_ON)

#define get_amp_n_ready         GPIO_ReadInputDataBit(AMP_PORT,AMP_n_READY)
#define get_amp_n_err           GPIO_ReadInputDataBit(AMP_PORT,AMP_n_ERROR)

//ak4117
#define ak4117_reset_ctrl(x)    ((x==1)? GPIO_SetBits(AK4117_PORTF,AK4117_RESET) : GPIO_ResetBits(AK4117_PORTF,AK4117_RESET));
#define ak4117_a_cs_ctrl(x)     ((x==1)? GPIO_SetBits(AK4117_PORTE,AK4117_A_CS)  : GPIO_ResetBits(AK4117_PORTE,AK4117_A_CS));
#define ak4117_b_cs_ctrl(x)     ((x==1)? GPIO_SetBits(AK4117_PORTE,AK4117_B_CS)  : GPIO_ResetBits(AK4117_PORTE,AK4117_B_CS));
#define ak4117_c_cs_ctrl(x)     ((x==1)? GPIO_SetBits(AK4117_PORTE,AK4117_C_CS)  : GPIO_ResetBits(AK4117_PORTE,AK4117_C_CS));
#define ak4117_d_cs_ctrl(x)     ((x==1)? GPIO_SetBits(AK4117_PORTE,AK4117_D_CS)  : GPIO_ResetBits(AK4117_PORTE,AK4117_D_CS));

#define ak4117_di_ctrl(x)       ((x==1)? GPIO_SetBits(AK4117_PORTG,AK4117_DI)    : GPIO_ResetBits(AK4117_PORTG,AK4117_DI));

#define get_ak4117_do           GPIO_ReadInputDataBit(AK4117_PORTE, AK4117_DO)

#define ak4117_clk_ctrl(x)      ((x==1)? GPIO_SetBits(AK4117_PORTG,AK4117_CLK)    : GPIO_ResetBits(AK4117_PORTG,AK4117_CLK));

#define ak4117_a_int0_read      GPIO_ReadInputDataBit(AK4117_PORTE, AK4117_A_INT0)
#define ak4117_b_int0_read      GPIO_ReadInputDataBit(AK4117_PORTE, AK4117_B_INT0)
#define ak4117_c_int0_read      GPIO_ReadInputDataBit(AK4117_PORTE, AK4117_C_INT0)
#define ak4117_d_int0_read      GPIO_ReadInputDataBit(AK4117_PORTE, AK4117_D_INT0)

//NJW1112
#define njw1112_clk_ctrl(x)     ((x==1)? GPIO_SetBits(NJW1112_PORT,NJW1112_CLK) : GPIO_ResetBits(NJW1112_PORT,NJW1112_CLK));
#define njw1112_data_ctrl(x)    ((x==1)? GPIO_SetBits(NJW1112_PORT,NJW1112_DAT) : GPIO_ResetBits(NJW1112_PORT,NJW1112_DAT));
#define njw1112_lat_ctrl(x)     ((x==1)? GPIO_SetBits(NJW1112_PORT,NJW1112_LAT) : GPIO_ResetBits(NJW1112_PORT,NJW1112_LAT));

//NJM2752
#define njm2752_a_cnt_ctrl(x)   ((x==1)? GPIO_SetBits(NJM2752_PORTB,NJM2752_A_SEL) : GPIO_ResetBits(NJM2752_PORTB,NJM2752_A_SEL));
#define njm2752_b_cnt_ctrl(x)   ((x==1)? GPIO_SetBits(NJM2752_PORTB,NJM2752_B_SEL) : GPIO_ResetBits(NJM2752_PORTB,NJM2752_B_SEL));
#define njm2752_c_cnt_ctrl(x)   ((x==1)? GPIO_SetBits(NJM2752_PORTE,NJM2752_C_SEL) : GPIO_ResetBits(NJM2752_PORTE,NJM2752_C_SEL));
#define njm2752_d_cnt_ctrl(x)   ((x==1)? GPIO_SetBits(NJM2752_PORTE,NJM2752_D_SEL) : GPIO_ResetBits(NJM2752_PORTE,NJM2752_D_SEL));

//NJW1194
#define njw1194_clk_ctrl(x)     ((x==1)? GPIO_SetBits(NJW1194_PORTD,NJW1194_CLK) : GPIO_ResetBits(NJW1194_PORTD,NJW1194_CLK));
#define njw1194_lat_ctrl(x)     ((x==1)? GPIO_SetBits(NJW1194_PORTD,NJW1194_LAT) : GPIO_ResetBits(NJW1194_PORTD,NJW1194_LAT));
#define njw1194_data_ctrl(x)    ((x==1)? GPIO_SetBits(NJW1194_PORTG,NJW1194_DAT) : GPIO_ResetBits(NJW1194_PORTG,NJW1194_DAT));

#else   //_STM32_F2XX_144PIN
//
//PIN-numbering 100pin(64pin)
//
#define PORTE_PORT              GPIOE
#define Edummy002               GPIO_Pin_2          //1
#define Edummy003               GPIO_Pin_3          //2
#define Edummy004				GPIO_Pin_4          //3
#define Edummy005               GPIO_Pin_5          //4
#define Edummy006               GPIO_Pin_6          //5
//
//6(1),VBAT
//
//for standby-led
#define GPIO_LED				GPIOC
#define STANDBY_LED				GPIO_Pin_13         //7(2)  
//
//for Power Relay
#define MAIN_PORT				GPIOC
#define MAIN_STBY				GPIO_Pin_14         //8(3)
//
//for Fan control
#define FAN_PORT                GPIOC
#define FAN_CTRL				GPIO_Pin_15         //9(4)
//
//10,VSS-5
//11,VDD-5
//
//12(5),OSC-in(4MHz)
//13(6),OSC-out
//14(7),NRST
//
//for ADC port
#define ADC_PORT                GPIOC
#define ADC_IN0                 GPIO_Pin_0          //15(8) //ADC10
#define ADC_IN1                 GPIO_Pin_1          //16(9) //ADC11
#define ADC_IN2                 GPIO_Pin_2          //17(10)//ADC12
#define ADC_IN3					GPIO_Pin_3          //18(11)//ADC13
//
//19(12),VSSA
//20,VREF-
//21,VREF+
//22(13),VDDA
//
//for Temperature R/L
#define TEMP_ADC_PORT			GPIOA    
#define TEMP_RIGHT				GPIO_Pin_0          //23(14)//ADC0
#define TEMP_LEFT				GPIO_Pin_1          //24(15)//ADC1
//
//for UARTx (UART2->Telechips)
//#define GPIOx                 GPIOA
//#define RCC_APB2Periph_GPIOx  RCC_APB2Periph_GPIOA
//#define GPIO_Tx2Pin             GPIO_Pin_2          //25(16)
//#define GPIO_Rx2Pin             GPIO_Pin_3          //26(17)
//
#define AMP_PORT				GPIOA    
#define AMP_nRDY				GPIO_Pin_2          //25(16) : input : AMP/RDY : powerOn > (1.5sec) > check Low => AMP Protection
#define TEMP_SMPS1				GPIO_Pin_2          //25(16)//ADC2 : add Temp-sensor
//
#define SMPS_PORT				GPIOA    
#define SMPS_nON				GPIO_Pin_3          //26(17) : SMPS control ( Low Active )
#define TEMP_SMPS2				GPIO_Pin_3          //26(17)//ADC3 : add Temp-sensor
//   
//27(18),VAA-4
//28(19),VDD-4
//
//for pre-out port
#define PREOUT_PORT				GPIOA
#define REC_MUTE				GPIO_Pin_4          //29(20) : PP/MP2 => also AMP on/off
//
//for volume njw1159v port
#define NJW1159_PORT			GPIOA				//(CE1,CE0)00=A,01=B,10=C,11=D
#define NJW1159_LAT				GPIO_Pin_5          //30(21)    
#define NJW1159_CLK				GPIO_Pin_6          //31(22)
#define NJW1159_DAT				GPIO_Pin_7          //32(23)

//for CH A,B select port
#define CH_AB_SEL_PORT			GPIOC
#define CH_A_DIGITAL			GPIO_Pin_4          //33(24) : High =digital
#define CH_B_DIGITAL			GPIO_Pin_5          //34(25) : Low = analog
//
//for CH C,D select port
#define CH_CD_SEL_PORT			GPIOB
#define CH_C_DIGITAL			GPIO_Pin_0          //35(26)
#define CH_D_DIGITAL			GPIO_Pin_1          //36(27)
//
//37(28),BOOT1=Low(GPIOB,GPIO_Pin_2)
//
#define PORTE_PORT				GPIOE
#define Edummy007				GPIO_Pin_7          //38
#define Edummy008				GPIO_Pin_8          //39
#define Edummy009				GPIO_Pin_9          //40
#define Edummy010				GPIO_Pin_10         //41
#define Edummy011				GPIO_Pin_11         //42
#define Edummy012				GPIO_Pin_12         //43
#define Edummy013				GPIO_Pin_13         //44
#define Edummy014				GPIO_Pin_14         //45
#define Edummy015               GPIO_Pin_15         //46
//
//#define nHP_MUTE              GPIO_Pin_14         //45
//#define nHP_inCHECK           GPIO_Pin_15         //46 : input
//
//for UARTx (UART3->notUsed)
//#define GPIOx                 GPIOB
//#define RCC_APB2Periph_GPIOx  RCC_APB2Periph_GPIOB
//#define GPIO_Tx3Pin             GPIO_Pin_10         //47(29)
//#define GPIO_Rx3Pin             GPIO_Pin_11         //48(30)
//
//for 4114 switching port
#define AK4114_PORTB			GPIOB
#define p_4114_RESET			GPIO_Pin_10         //47(29)
#define p_4114_CLK				GPIO_Pin_11         //48(30)
//
//49(31),VSS-1
//50(32),VDD-1
//
#define p_4114_DO				GPIO_Pin_12         //51(33) : output
#define p_4114_DI				GPIO_Pin_13         //52(34) : input
#define p_4114_A_Int0			GPIO_Pin_14         //53(35) : input
#define p_4114_A_CS				GPIO_Pin_15         //54(36)
//
#define PORTD_PORTD				GPIOD
#define Ddummy008				GPIO_Pin_8          //55
#define Ddummy009				GPIO_Pin_9          //56
#define Ddummy010				GPIO_Pin_10         //57
#define Ddummy011				GPIO_Pin_11         //58
#define Ddummy012				GPIO_Pin_12         //59
#define ddummy013				GPIO_Pin_13         //60
#define Ddummy014				GPIO_Pin_14         //61
#define Ddummy015				GPIO_Pin_15         //62
//
//for 4114 switching port
#define AK4114_PORTC            GPIOC
#define p_4114_B_Int0			GPIO_Pin_6          //63(37) : input
#define p_4114_B_CS				GPIO_Pin_7          //64(38)
#define p_4114_C_Int0			GPIO_Pin_8          //65(39) : input
#define p_4114_C_CS				GPIO_Pin_9          //66(40)
//
//for 4114 switching port
#define AK4114_PORTA			GPIOA
#define p_4114_D_Int0			GPIO_Pin_8          //67(41)
//
//for UARTx (UART1)
//#define GPIOx                 GPIOA
//#define RCC_APB2Periph_GPIOx  RCC_APB2Periph_GPIOA
#define GPIO_Tx1Pin             GPIO_Pin_9          //68(42)
#define GPIO_Rx1Pin             GPIO_Pin_10         //69(43)
//
#define p_4114_D_CS				GPIO_Pin_11         //70(44)
//
//for 12V trigger input
#define T12V_PORT				GPIOA
#define T12V_IN					GPIO_Pin_12         //71(45) : input
//
//72(46),JTMS(GPIOA,GPIO_Pin_13)
//20140523 : for signal sensing feature :
#define SIGNAL_SENSING_PORTA	GPIOA
#define SIGNAL_SENSING_nSW		GPIO_Pin_13         //72(46) : input s/w-intput
//73 NC
//74(47),VSS-2
//75(48),VDD-2
//
//76(49),JTCK(GPIOA,GPIO_Pin_14)
//20140523 : for signal sensing feature :
#define SIGNAL_SENSING_nMP2		GPIO_Pin_14         //76(49) : input MP2-intput
//77(50),JTDI(GPIOA,GPIO_Pin_15)
//20140523 : for signal sensing feature :
#define SIGNAL_SENSING_nIN		GPIO_Pin_15         //77(50) : input sense-intput
//
//for UARTx (UART4 ->RotelLink)
//#define GPIOx                 GPIOC
//#define RCC_APB2Periph_GPIOx  RCC_APB2Periph_GPIOC
//#define GPIO_Tx4Pin             GPIO_Pin_10         //78(51)
//#define GPIO_Rx4Pin             GPIO_Pin_11         //79(52)
//
//for sub-id option port : internal pull-up!
#define SUBID_PORT              GPIOC				//11="0", 10="1", 01="2", 00="3" [ ex) "60:0A:freq=192!" ]
//20131024 : because add Temp-sensor, move this.
#define AMP_PORTC				GPIOC    
#define SUBID0					GPIO_Pin_10         //78(51) : input : RKB WS2 =>
#define AMP_nRDY_PC10			GPIO_Pin_10			//78(51) : input : RKB PP =>AMP/RDY : powerOn > (1.5sec) > check Low => AMP Protection
//20131024 : because add Temp-sensor, move this.
#define SMPS_PORTC				GPIOC    
#define SUBID1					GPIO_Pin_11         //79(52) : input : RKB WS2 =>
#define SMPS_nON_PC11			GPIO_Pin_11			//79(52) : output : RKB PP => SMPS control ( Low Active )
//
#define PORTC_PORT              GPIOC
#define WS_S3					GPIO_Pin_12         //80(53) : input : RKB WS2 =>
#define RKB_mode_PPMP			GPIO_Pin_12         //80(53) : input : Low=WorkingSample , High=PP,MP
//
#define PORTD_PORT              GPIOD
#define Ddummy000               GPIO_Pin_0          //81
#define Ddummy001               GPIO_Pin_1          //82
//20140515 : used optical_power_mode
#define PORTD_PORT              GPIOD
#define nOverLoad               GPIO_Pin_2          //83(54) : add 20130710 =>LowLevel 10msec, goto AMP Protection , MP=> not used, 
//
#define PORTD_PORT              GPIOD
#define Ddummy003				GPIO_Pin_3          //84
#define Ddummy004				GPIO_Pin_4          //85
#define Ddummy005				GPIO_Pin_5          //86
#define Ddummy006				GPIO_Pin_6          //87
#define Ddummy007				GPIO_Pin_7          //88
//
//89(55),JTDO(GPIOB,GPIO_Pin_3)
//20140523 : for signal sensing feature :
#define FAN_LS_PORTB			GPIOB
#define FAN_LS_nOFF				GPIO_Pin_3			//89(55) : output FAN LowSpeed nOff
//90(56),JNTRST(GPIOB,GPIO_Pin_4)
//
#define POWERDOWN_PORT			GPIOB
#define POWER_DOWN				GPIO_Pin_5          //91(57)	//20130624
//
//for eeprom port
#define IIC_PORT                GPIOB
#define IIC_SCL                 GPIO_Pin_6          //92(58)
#define IIC_SDA                 GPIO_Pin_7          //93(59)
//    
//94(60),BOOT0( Low => High ) : upgrade start!!
//
//for project option port : internal pull-up!
#define PROJ_PORTB              GPIOB				//00=850, 01=8100, 10=D850, 11=D8100
#define PROJ_8100				GPIO_Pin_8          //95(61) : 0=850, 1=8100
#define PROJ_DIGITAL			GPIO_Pin_9          //96(62) : 0=analog, 1=Digital
//
#define PORTE_PORT              GPIOE
#define Edummy000               GPIO_Pin_0          //97
#define Edummy001               GPIO_Pin_1          //98
//
//99(63),VSS-3
//100(64),VDD-3
    
/****************************************************************************************/
//for printf
#define  PRINTx                 USART1

/****************************************************************************************/
/* macro definition */


//7(2)LED : highActive
#define StandbyLED(x)           ( (x==1)? GPIO_SetBits(GPIO_LED, STANDBY_LED) : GPIO_ResetBits(GPIO_LED, STANDBY_LED) );

//8(3)standby relay
#define Standby(x)              ( (x==1)? GPIO_SetBits(MAIN_PORT, MAIN_STBY) : GPIO_ResetBits(MAIN_PORT, MAIN_STBY) );

//8(3)FAN control
#define FAN_hs_on(x)			( (x==1)? GPIO_SetBits(FAN_PORT, FAN_CTRL) : GPIO_ResetBits(FAN_PORT, FAN_CTRL) );
//#define FAN_ls_on(x)			( (x==1)? GPIO_SetBits(FAN_PORT, FAN_CTRL) : GPIO_ResetBits(FAN_PORT, FAN_CTRL) );

//25(16)AMP./RDY =>(High)=> AMP protection
#define Amp_nReadyPortProtection		GPIO_ReadInputDataBit(AMP_PORT, AMP_nRDY)
//78(51)AMP./RDY =>(High)=> AMP protection : 20131024
#define Amp_nReadyPortC10Protection		GPIO_ReadInputDataBit(AMP_PORTC, AMP_nRDY_PC10)
//80(53)mode_PP_MP =>(High)=> PP_MP : 20131024
#define RKB_mode_PP_MP					GPIO_ReadInputDataBit(PORTC_PORT, RKB_mode_PPMP)

//26(17) SMPS Power On : (Low Active) :
#define SMPS_nPowerOn(x)		( (x==1)? GPIO_SetBits(SMPS_PORT, SMPS_nON) : GPIO_ResetBits(SMPS_PORT, SMPS_nON) );
//79(52) SMPS Power On : (Low Active) : 20131024
#define SMPS_nPowerOn_PC11(x)	( (x==1)? GPIO_SetBits(SMPS_PORTC, SMPS_nON_PC11) : GPIO_ResetBits(SMPS_PORTC, SMPS_nON_PC11) );

//70(44)pre-out mute (High Active)
#define preOut_Mute(x)			( (x==1)? GPIO_SetBits(PREOUT_PORT, REC_MUTE) : GPIO_ResetBits(PREOUT_PORT, REC_MUTE) );

//30~32(21~23) NJW1159 volume IC
#define NJW1159_lat(x)			( (x==1)? GPIO_SetBits(NJW1159_PORT, NJW1159_LAT) : GPIO_ResetBits(NJW1159_PORT, NJW1159_LAT) );
#define NJW1159_clock(x)		( (x==1)? GPIO_SetBits(NJW1159_PORT, NJW1159_CLK) : GPIO_ResetBits(NJW1159_PORT, NJW1159_CLK) );
#define NJW1159_data(x)			( (x==1)? GPIO_SetBits(NJW1159_PORT, NJW1159_DAT) : GPIO_ResetBits(NJW1159_PORT, NJW1159_DAT) );

//33,34(24,25) CH A/B select Digital(1)/Analog(0)
#define SEL_CH_A_Digital(x)		( (x==1)? GPIO_SetBits(CH_AB_SEL_PORT, CH_A_DIGITAL) : GPIO_ResetBits(CH_AB_SEL_PORT, CH_A_DIGITAL) );
#define SEL_CH_B_Digital(x)		( (x==1)? GPIO_SetBits(CH_AB_SEL_PORT, CH_B_DIGITAL) : GPIO_ResetBits(CH_AB_SEL_PORT, CH_B_DIGITAL) );
//35,36(26,27) CH C/D select Digital/Analog
#define SEL_CH_C_Digital(x)		( (x==1)? GPIO_SetBits(CH_CD_SEL_PORT, CH_C_DIGITAL) : GPIO_ResetBits(CH_CD_SEL_PORT, CH_C_DIGITAL) );
#define SEL_CH_D_Digital(x)		( (x==1)? GPIO_SetBits(CH_CD_SEL_PORT, CH_D_DIGITAL) : GPIO_ResetBits(CH_CD_SEL_PORT, CH_D_DIGITAL) );

// AK4114  
#define p_4114_reset(x)         ( (x==1)? GPIO_SetBits(AK4114_PORTB, p_4114_RESET) : GPIO_ResetBits(AK4114_PORTB, p_4114_RESET) );
#define p_4114_clk(x)           ( (x==1)? GPIO_SetBits(AK4114_PORTB, p_4114_CLK) : GPIO_ResetBits(AK4114_PORTB, p_4114_CLK) );
#define p_4114_do(x)            ( (x==1)? GPIO_SetBits(AK4114_PORTB, p_4114_DO) : GPIO_ResetBits(AK4114_PORTB, p_4114_DO) );
#define Get4114di               GPIO_ReadInputDataBit(AK4114_PORTB, p_4114_DI)

//#define Get4114INT1             GPIO_ReadInputDataBit(AK4114_PORTD, p_4114_Int1)
//53,54(35,36) AK4114 A int0, CS
#define Get4114INT0_A			GPIO_ReadInputDataBit(AK4114_PORTB, p_4114_A_Int0)
#define p_4114_cs_A(x)			( (x==1)? GPIO_SetBits(AK4114_PORTB, p_4114_A_CS) : GPIO_ResetBits(AK4114_PORTB, p_4114_A_CS) );
#define p_4114_cs(x)			( (x==1)? GPIO_SetBits(AK4114_PORTB, p_4114_A_CS) : GPIO_ResetBits(AK4114_PORTB, p_4114_A_CS) );

//63,64(37,38) AK4114 B int0, CS
#define Get4114INT0_B			GPIO_ReadInputDataBit(AK4114_PORTC, p_4114_B_Int0)
#define p_4114_cs_B(x)			( (x==1)? GPIO_SetBits(AK4114_PORTC, p_4114_B_CS) : GPIO_ResetBits(AK4114_PORTC, p_4114_B_CS) );
	
//65,66(39,40) AK4114 C int0, CS
#define Get4114INT0_C			GPIO_ReadInputDataBit(AK4114_PORTC, p_4114_C_Int0)
#define p_4114_cs_C(x)			( (x==1)? GPIO_SetBits(AK4114_PORTC, p_4114_C_CS) : GPIO_ResetBits(AK4114_PORTC, p_4114_C_CS) );
	
//67,70(41,44) AK4114 D int0, CS
#define Get4114INT0_D			GPIO_ReadInputDataBit(AK4114_PORTA, p_4114_D_Int0)
#define p_4114_cs_D(x)			( (x==1)? GPIO_SetBits(AK4114_PORTA, p_4114_D_CS) : GPIO_ResetBits(AK4114_PORTA, p_4114_D_CS) );

//91(57)power down =>(High)=> power down=>reley off->volume mute Tx
#define Power_down				GPIO_ReadInputDataBit(POWERDOWN_PORT, POWER_DOWN)

//71(45)12V trigger input
#define GetT12V_IN				GPIO_ReadInputDataBit(T12V_PORT, T12V_IN)
//20140523 : for signal sensing feature :
#define GetSignalSensing_nSW		GPIO_ReadInputDataBit(SIGNAL_SENSING_PORTA, SIGNAL_SENSING_nSW)
#define RKB_mode_PP_nMP2			GPIO_ReadInputDataBit(SIGNAL_SENSING_PORTA, SIGNAL_SENSING_nMP2)
#define GetSignalSensing_nPort		GPIO_ReadInputDataBit(SIGNAL_SENSING_PORTA, SIGNAL_SENSING_nIN)
#define FAN_ls_on(x)				( (x==1)? GPIO_SetBits(FAN_LS_PORTB, FAN_LS_nOFF) : GPIO_ResetBits(FAN_LS_PORTB, FAN_LS_nOFF) );

//83(54)nOverLoad =>(Low,10msec)=> AMP Protection : 20130710
//MP : not used
//20140515 : used optical_power_mode
#define nOverLoadLowLevelCheck	GPIO_ReadInputDataBit(PORTD_PORT, nOverLoad)
//92~93(58~59)eeprom
#define p_i2c_sda(x)            ( (x==1)? GPIO_SetBits(IIC_PORT, IIC_SDA) : GPIO_ResetBits(IIC_PORT, IIC_SDA) );
#define p_i2c_scl(x)            ( (x==1)? GPIO_SetBits(IIC_PORT, IIC_SCL) : GPIO_ResetBits(IIC_PORT, IIC_SCL) );

//95,96(61,62) option Projet 850,8100,D850,D8100
#define GetOption_8100			GPIO_ReadInputDataBit(PROJ_PORTB, PROJ_8100)
#define GetOption_Digital		GPIO_ReadInputDataBit(PROJ_PORTB, PROJ_DIGITAL)

//----------------------------------------------------------    
//readInput : 16Bits
//#define GetInputTC2002A_u16BUS		GPIO_ReadInputData(GPIOB)
//readOutput : 16Bits
//#define GetOutputTC2002A_u16BUS		GPIO_ReadOutputData(GPIOB)
//writeOutput : 16Bits
//#define WritePortB_realy(nnnn)	    GPIO_Write(GPIOB, ((GPIO_ReadOutputData(GPIOB))&0x00ff)|(nnnn&0xff00))
//#define WritePortB_realy(nnnn)	    GPIO_Write(GPIOB, ((GPIO_ReadOutputData(GPIOB))&0x1fff)|(nnnn&0xe000))
//#define WritePortD_realy(nnnn)	    GPIO_Write(GPIOD, ((GPIO_ReadOutputData(GPIOD))&0xf0ff)|(nnnn&0x0f00))
//----eeprom------------------------------------------------------    
#define SetSDA                  {\
                                    GPIO_InitTypeDef GPIO_InitStructure;;\
                                    GPIO_InitStructure.GPIO_Pin = IIC_SDA;\
                                    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;\
                                    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;\
                                    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;\
                                    GPIO_Init(IIC_PORT, &GPIO_InitStructure);\
                                };


#define ResSDA                  {\
                                    GPIO_InitTypeDef GPIO_InitStructure;;\
                                    GPIO_InitStructure.GPIO_Pin = IIC_SDA;\
                                    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;\
                                    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out;\
                                    GPIO_InitStructure.GPIO_PuPd = GPIO_OType_PP;\
                                    GPIO_Init(IIC_PORT, &GPIO_InitStructure);\
                                };

#define ResSDAA                  {\
                                    GPIO_InitTypeDef GPIO_InitStructure;;\
                                    GPIO_InitStructure.GPIO_Pin = IIC_SDA;\
                                    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;\
                                    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out;\
                                    GPIO_InitStructure.GPIO_PuPd = GPIO_OType_PP;\
                                    GPIO_Init(IIC_PORT, &GPIO_InitStructure);\
                                };

#define GetSDA                  GPIO_ReadInputDataBit(IIC_PORT, IIC_SDA)
#define SetSCL                  GPIO_SetBits(IIC_PORT, IIC_SCL)
#define ResSCL                  GPIO_ResetBits(IIC_PORT, IIC_SCL)
#define GetSCL                  GPIO_ReadOutputDataBit(IIC_PORT, IIC_SCL)

/****************************************************************************************/

#if 0
//DAC AK4353
#define DACReset(x)             ( (x==1)? GPIO_SetBits(DAC_PORT, DAC_RESET) : GPIO_ResetBits(DAC_PORT, DAC_RESET) );
#define p_DACi2c_sda(x)         ( (x==1)? GPIO_SetBits(DAC_PORT, DAC_SDA) : GPIO_ResetBits(DAC_PORT, DAC_SDA) );
#define p_DACi2c_scl(x)         ( (x==1)? GPIO_SetBits(DAC_PORTC, DAC_SCL) : GPIO_ResetBits(DAC_PORTC, DAC_SCL) );

#define SetDACSDA               {\
                                    GPIO_InitTypeDef GPIO_InitStructure;;\
                                    GPIO_InitStructure.GPIO_Pin = DAC_SDA;\
                                    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;\
                                    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;\
                                    GPIO_Init(DAC_PORT, &GPIO_InitStructure);\
                                };


#define ResDACSDA               {\
                                    GPIO_InitTypeDef GPIO_InitStructure;;\
                                    GPIO_InitStructure.GPIO_Pin = DAC_SDA;\
                                    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;\
                                    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;\
                                    GPIO_Init(DAC_PORT, &GPIO_InitStructure);\
                                };

#define GetDACSDA               GPIO_ReadInputDataBit(DAC_PORT, DAC_SDA)

#endif /* if 0 */
/****************************************************************************************/



#endif  //__PORT_DEF__
#endif //STM32_F2XX_104PIN

/* end of file */
/******************* (C) COPYRIGHT 2008 Rotel Technology Global **************/

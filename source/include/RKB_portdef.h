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

#define amp_on_off(x)               ((x==1)? GPIO_SetBits(AMP_PORT,AMP_ON) : GPIO_ResetBits(AMP_PORT,AMP_ON));


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
#define p_4114_reset(x)         ((x==1)? GPIO_SetBits(AK4117_PORTF,AK4117_RESET) : GPIO_ResetBits(AK4117_PORTF,AK4117_RESET));
#define p_4114_cs_D(x)          ((x==1)? GPIO_SetBits(AK4117_PORTE,AK4117_D_CS)  : GPIO_ResetBits(AK4117_PORTE,AK4117_D_CS));
#define p_4114_cs_C(x)          ak4117_c_cs_ctrl(x)          
#define p_4114_cs_B(x)          ak4117_b_cs_ctrl(x)   
#define p_4114_cs_A(x)          ak4117_a_cs_ctrl(x)

#define Get4114di               get_ak4117_do


#define p_4114_clk(x) 			( (x==1)? GPIO_SetBits(AK4117_PORTG, AK4117_CLK) : GPIO_ResetBits(AK4117_PORTG, AK4117_CLK) );
#define p_4114_do(x)            ( (x==1)? GPIO_SetBits(AK4117_PORTG, AK4117_DI) : GPIO_ResetBits(AK4117_PORTG, AK4117_DI) );

#define ak4117_a_int0_read      GPIO_ReadInputDataBit(AK4117_PORTE, AK4117_A_INT0)
#define ak4117_b_int0_read      GPIO_ReadInputDataBit(AK4117_PORTE, AK4117_B_INT0)
#define ak4117_c_int0_read      GPIO_ReadInputDataBit(AK4117_PORTE, AK4117_C_INT0)
#define ak4117_d_int0_read      GPIO_ReadInputDataBit(AK4117_PORTE, AK4117_D_INT0)

#define Get4114INT0_A           ak4117_a_int0_read
#define Get4114INT0_B           ak4117_b_int0_read
#define Get4114INT0_C           ak4117_c_int0_read
#define Get4114INT0_D           ak4117_d_int0_read

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

// eeprom
//input 
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
                                    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;\
                                    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;\
                                    GPIO_Init(IIC_PORT, &GPIO_InitStructure);\
                                };

#define ResSDAA                  {\
                                    GPIO_InitTypeDef GPIO_InitStructure;;\
                                    GPIO_InitStructure.GPIO_Pin = IIC_SDA;\
                                    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;\
                                    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;\
                                    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;\
                                    GPIO_Init(IIC_PORT, &GPIO_InitStructure);\
                                };

#define GetSDA                  GPIO_ReadInputDataBit(IIC_PORT, IIC_SDA)
#define SetSCL                  GPIO_SetBits(IIC_PORT, IIC_SCL)
#define ResSCL                  GPIO_ResetBits(IIC_PORT, IIC_SCL)
#define GetSCL                  GPIO_ReadOutputDataBit(IIC_PORT, IIC_SCL)

#define p_i2c_scl(x)            ( (x==1)? GPIO_SetBits(IIC_PORT, IIC_SCL) : GPIO_ResetBits(IIC_PORT, IIC_SCL) );
#define p_i2c_sda(x)            ( (x==1)? GPIO_SetBits(IIC_PORT, IIC_SDA) : GPIO_ResetBits(IIC_PORT, IIC_SDA) );
#endif

/* end of file */
/******************* (C) COPYRIGHT 2008 Rotel Technology Global **************/

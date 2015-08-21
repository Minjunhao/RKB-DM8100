/**************** (c)2008 Rotel Technology Global Inc..*************************
PROJECT         : Front Panel for Large VFD
COMPILER        : IAR Embeded Workbench 4.4.1
MODULE          : uart.h
VERSION         : V 1.0
USED CHIP       : stm32f10x
CREATION DATE   : /2008
AUTHORS         : An Kyung Chan / RTG
DESCRIPTION     :
*******************************************************************************/

#ifndef __UART_H__
#define __UART_H__

#define RX_MAX_SIZE 	1000
#define TX_MAX_SIZE 	1000
#define RX2_MAX_SIZE 	1000
#define TX2_MAX_SIZE 	1000
#define RX3_MAX_SIZE 	1000
#define TX3_MAX_SIZE 	1000
#define RX4_MAX_SIZE 	1000
#define TX4_MAX_SIZE 	1000
#define RX5_MAX_SIZE 	1000
#define TX5_MAX_SIZE 	1000

#define RX_DATA_SYNC	0xfe
#define RX_DATA_SYNCSUB 0xf7
#define RX_META_CODE	0xfd

#define BPS9600		0
#define BPS19200	1
#define BPS38400	2
#define BPS115200	3



#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)

#define debug(msg) __asm("nop")
#define upgrade(msg) __asm("nop")
//#define debug(msg) 		do{rs232_printf msg; uart1_TxData();}while(0);
//#define upgrade(msg) 	do{rs232_printf msg; uart1_TxData();}while(0);



/*	UART1 PC:RS232 	bps:115,200
	UART2  			bps:115,200
	UART3 
	UART4  			bps:19,200
	UART5 
------------------------------------------------------------------------------*/
extern byte rx_buf[RX_MAX_SIZE];		//RS232
extern byte tx_buf[TX_MAX_SIZE];
#ifdef UART2_ENABLE	
extern byte rx2_buf[RX2_MAX_SIZE];		// 
extern byte tx2_buf[TX2_MAX_SIZE];
#endif
#ifdef UART3_ENABLE	
extern byte rx3_buf[RX3_MAX_SIZE];		//unused
extern byte tx3_buf[TX3_MAX_SIZE];
#endif
#ifdef UART4_ENABLE	
extern byte rx4_buf[RX4_MAX_SIZE];		// 
extern byte tx4_buf[TX4_MAX_SIZE];
#endif
#ifdef UART5_ENABLE	
extern byte rx5_buf[RX5_MAX_SIZE];		//unused
extern byte tx5_buf[TX5_MAX_SIZE];
#endif

extern volatile word rs232_Rx_Head;
extern volatile word rs232_Rx_Tail;
extern volatile word rs232_Tx_Head;
extern volatile word rs232_Tx_Tail;
extern volatile byte tx_existed;

#ifdef UART3_ENABLE	
extern volatile word rs232_Rx2_Head;
extern volatile word rs232_Rx2_Tail;
extern volatile word rs232_Tx2_Head;
extern volatile word rs232_Tx2_Tail;
extern volatile byte tx2_existed;
#endif
#ifdef UART3_ENABLE	
extern volatile word rs232_Rx3_Head;
extern volatile word rs232_Rx3_Tail;
extern volatile word rs232_Tx3_Head;
extern volatile word rs232_Tx3_Tail;
extern volatile byte tx3_existed;
#endif
#ifdef UART4_ENABLE	
extern volatile word rs232_Rx4_Head;	// 
extern volatile word rs232_Rx4_Tail;
extern volatile word rs232_Tx4_Head;
extern volatile word rs232_Tx4_Tail;
extern volatile byte tx4_existed;
#endif
#ifdef UART5_ENABLE	
extern volatile word rs232_Rx5_Head;	//unused
extern volatile word rs232_Rx5_Tail;
extern volatile word rs232_Tx5_Head;
extern volatile word rs232_Tx5_Tail;
extern volatile byte tx5_existed;
#endif

extern volatile unsigned char ic_comm_ok;
extern volatile unsigned char ic_comm_use;
extern volatile unsigned char ic_comm_buf[32];
extern volatile unsigned char ic_comm_index;
extern volatile unsigned char ic_start_frame;
extern volatile unsigned char ic_comm_state;

static byte cmd_count;
static byte cmd_mode;
static byte rx_checksum;


/*----------------------------------------------------------------------------*/
void uart_Init(void);					//RS232
void uart2_Init(void);					// 
void uart3_Init(void);					//unused
void uart4_Init(void);					// 
void uart5_Init(void);					//unused
void uart1_IRQHandler(void);
void uart2_IRQHandler(void);
void uart3_IRQHandler(void);
void uart4_IRQHandler(void);			//
void uart5_IRQHandler(void);			//

byte uart1_GetRxCommand(void);
bool uart1_GetTxData(byte *data);
bool uart2_GetTxData(byte *data);
bool uart3_GetTxData(byte *data);
bool uart4_GetTxData(byte *data);		//
bool uart5_GetTxData(byte *data);		//
void uart1_TxData(void);
void uart2_TxData(void);
void uart3_TxData(void);
void uart4_TxData(void);				//
void uart5_TxData(void);				//
short rs232_printf(const char format[],...);
short rs232_PutString(const char str[]);
byte rs232_GetUpgradeMode(void);
void rs232_SetUpgradeMode(byte mode);
void rs232_CheckUpgrade(void);
void PutChar(byte data);
void PutChar2(byte data);
void PutChar3(byte data);
void PutChar4(byte data);
void PutChar5(byte data);

void 	Put_RxBuffer(char ch);

byte GetChar(void);
byte GetChar2(void);
byte GetChar3(void);
byte GetChar4(void);
byte GetChar5(void);

#endif	//__UART_H__

/* end of file */
/******************* (C) COPYRIGHT 2008 Rotel Technology Global **************/

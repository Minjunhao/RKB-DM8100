/**************** (c)2008 Rotel Technology Global Inc..*************************
PROJECT         : RKB-D850
COMPILER        : IAR Embeded Workbench 5.50
MODULE          : uart.c
VERSION         : V 1.0
USED CHIP       : stm32f10x
CREATION DATE   : /2013
AUTHORS         : /Rotel
DESCRIPTION     :
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "config.h"
//#include "initmcu.h"
#include "main.h"

#include "uart.h"
#include "uart.mem"

#ifdef IPOD_INCLUDE
#include "iPod_config.h"
#endif

#ifdef IAP_UPGRADE
static byte up_buf[15];
static byte up_count = 0;
static byte UpgradeMode = 0;
#endif

#ifdef IPOD_INCLUDE
static unsigned char cmd_mode;
static unsigned long rx2_checksum;
static unsigned char word_count;
static unsigned short didx;
static unsigned short dlength;
static unsigned char sync;
/* 090626 */
static unsigned char  mStep;
static unsigned char  mData[4];

#endif

/*----------------------------------------------------------------------------*/
#define _MN_ "UART: "
/*
	UART1	: RS232			: 115200,n,8,1
	UART2	: not used		: 115200,n,8,1
	UART3	: not used		: 115200,n,8,1
	UART4	: not used		: 19200,n,8,1
	UART5	: not used		: 19200,n,8,1
-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 Function Name  : uart_Init
 Description    : initialize for header and tail
 Input          : None
 Output         : None
 Return         : None
 Comments	 	: ..
-----------------------------------------------------------------------------*/
void uart_Init(void)
{
	rs232_Rx_Head = 0;
	rs232_Rx_Tail = 0;
	rs232_Tx_Tail = 0;
	rs232_Tx_Head = 0;

#ifdef IC_DEBUG
     ic_comm_index = 0;
     ic_comm_use = 0;
     ic_start_frame = 0;
     ic_comm_state = 0;
     ic_comm_ok = 0;
#endif
}
#ifdef UART2_ENABLE	
void uart2_Init(void)
{
	rs232_Rx2_Head = 0;
	rs232_Rx2_Tail = 0;
	rs232_Tx2_Tail = 0;
	rs232_Tx2_Head = 0;
}
#endif
#ifdef UART3_ENABLE	
void uart3_Init(void)
{
	rs232_Rx3_Head = 0;
	rs232_Rx3_Tail = 0;
	rs232_Tx3_Tail = 0;
	rs232_Tx3_Head = 0;
}
#endif
#ifdef UART4_ENABLE	
void uart4_Init(void)
{
	rs232_Rx4_Head = 0;
	rs232_Rx4_Tail = 0;
	rs232_Tx4_Tail = 0;
	rs232_Tx4_Head = 0;
}
#endif
#ifdef UART5_ENABLE	
void uart5_Init(void)
{
	rs232_Rx5_Head = 0;
	rs232_Rx5_Tail = 0;
	rs232_Tx5_Tail = 0;
	rs232_Tx5_Head = 0;
}
#endif
/*-----------------------------------------------------------------------------
 Function Name  : uart1_IRQHandler
 Description    : UART1 interrupt handler
 Input          : None
 Output         : None
 Return         : None
 Comments	 	: ..
-----------------------------------------------------------------------------*/
void uart1_IRQHandler(void)
{
	byte rx_data;

  	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
  	{
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);

  	  	/* Read one byte from the receive data register */
  	  	rx_data = USART_ReceiveData(USART1);      

		++rs232_Rx_Head;
		if(rs232_Rx_Head >= RX_MAX_SIZE) rs232_Rx_Head = 0;
		rx_buf[rs232_Rx_Head] = rx_data;
//chd, IC_DEBUG
		Put_RxBuffer(rx_data);				//IC_DEBUG
//
#ifdef IAP_UPGRADE
		if (rx_data == 'u' || up_count>=15) {
			up_count = 0;
		}
		up_buf[up_count] = rx_data;
		++up_count;
#endif
  	}
	
	/* transmitter handler */  	
  	//if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)
  	if(USART_GetITStatus(USART1, USART_IT_TC) != RESET)
  	{   
		USART_ClearITPendingBit(USART1,USART_IT_TC);
	    tx_existed = FALSE;
		uart1_TxData();
  	}
}
/*-----------------------------------------------------------------------------
 Function Name  : uart2_IRQHandler
 Description    : UART2 interrupt handler
 Input          : None
 Output         : None
 Return         : None
 Comments	 	: ..
-----------------------------------------------------------------------------*/
#ifdef UART2_ENABLE	
void uart2_IRQHandler(void)
{
	byte rx_data;

  	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
  	{
		USART_ClearITPendingBit(USART2,USART_IT_RXNE);

  	  	/* Read one byte from the receive data register */
  	  	rx_data = USART_ReceiveData(USART2);      

		++rs232_Rx2_Head;
		if(rs232_Rx2_Head >= RX2_MAX_SIZE) rs232_Rx2_Head = 0;
		rx2_buf[rs232_Rx2_Head] = rx_data;	
  	}
	
	/* transmitter handler */  	
  	//if(USART_GetITStatus(USART2, USART_IT_TXE) != RESET)
  	if(USART_GetITStatus(USART2, USART_IT_TC) != RESET)
  	{   
		USART_ClearITPendingBit(USART2,USART_IT_TC);
	    tx2_existed = FALSE;
		uart2_TxData();
  	}
}
#endif
/*-----------------------------------------------------------------------------
 Function Name  : uart3_IRQHandler
 Description    : UART3 interrupt handler
 Input          : None
 Output         : None
 Return         : None
 Comments	 	: ..
-----------------------------------------------------------------------------*/
#ifdef UART3_ENABLE	
void uart3_IRQHandler(void)
{
	byte rx_data;

  	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
  	{
		USART_ClearITPendingBit(USART3,USART_IT_RXNE);

  	  	/* Read one byte from the receive data register */
  	  	rx_data = USART_ReceiveData(USART3);      

		++rs232_Rx3_Head;
		if(rs232_Rx3_Head >= RX3_MAX_SIZE) rs232_Rx3_Head = 0;
		rx3_buf[rs232_Rx3_Head] = rx_data;	
  	}
	
	/* transmitter handler */  	
  	//if(USART_GetITStatus(USART3, USART_IT_TXE) != RESET)
  	if(USART_GetITStatus(USART3, USART_IT_TC) != RESET)
  	{   
		USART_ClearITPendingBit(USART3,USART_IT_TC);
	    tx3_existed = FALSE;
		uart3_TxData();
  	}
}
#endif
/*-----------------------------------------------------------------------------
 Function Name  : uart4_IRQHandler
 Description    : UART4 interrupt handler
 Input          : None
 Output         : None
 Return         : None
 Comments	 	: ..
-----------------------------------------------------------------------------*/
#ifdef UART4_ENABLE	
void uart4_IRQHandler(void)
{
	byte rx_data;

  	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)
  	{
		USART_ClearITPendingBit(UART4,USART_IT_RXNE);

  	  	/* Read one byte from the receive data register */
  	  	rx_data = USART_ReceiveData(UART4);
//  	  	PutChar(rx_data);								//chd  
//		USART_SendData(UART4,(u16)rx_data);				//greenchips : test

		++rs232_Rx4_Head;
		if(rs232_Rx4_Head >= RX4_MAX_SIZE) rs232_Rx4_Head = 0;
		rx4_buf[rs232_Rx4_Head] = rx_data;	
  	}
	
	/* transmitter handler */  	
  	//if(USART_GetITStatus(USART4, USART_IT_TXE) != RESET)
  	if(USART_GetITStatus(UART4, USART_IT_TC) != RESET)
  	{   
		USART_ClearITPendingBit(UART4,USART_IT_TC);
	    tx4_existed = FALSE;
		uart4_TxData();
  	}
}
#endif
/*-----------------------------------------------------------------------------
 Function Name  : uart5_IRQHandler
 Description    : UART5 interrupt handler
 Input          : None
 Output         : None
 Return         : None
 Comments	 	: ..
-----------------------------------------------------------------------------*/
#ifdef UART5_ENABLE	
void uart5_IRQHandler(void)
{
	byte rx_data;

  	if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)
  	{
		USART_ClearITPendingBit(UART5,USART_IT_RXNE);

  	  	/* Read one byte from the receive data register */
  	  	rx_data = USART_ReceiveData(UART5);
//  	  	PutChar(rx_data);								//chd  
//		USART_SendData(UART5,(u16)rx_data);				//greenchips : test

		++rs232_Rx5_Head;
		if(rs232_Rx5_Head >= RX5_MAX_SIZE) rs232_Rx5_Head = 0;
		rx5_buf[rs232_Rx5_Head] = rx_data;	
  	}
	
	/* transmitter handler */  	
  	//if(USART_GetITStatus(USART5, USART_IT_TXE) != RESET)
  	if(USART_GetITStatus(UART5, USART_IT_TC) != RESET)
  	{   
		USART_ClearITPendingBit(UART5,USART_IT_TC);
	    tx5_existed = FALSE;
		uart5_TxData();
  	}
}
#endif
/*-----------------------------------------------------------------------------
 Function Name  : uart1_TxData
 Description    : Write one byte to the transmit data register
 Input          : None
 Output         : None
 Return         : None
 Comments	 	: ..
-----------------------------------------------------------------------------*/
void uart1_TxData(void)                         //RS232
{	
	byte tx_data;
	
	if(uart1_GetTxData(&tx_data) == TRUE) {
  	 	USART_SendData(USART1, tx_data);  
	}
}
#ifdef UART2_ENABLE	
void uart2_TxData(void)                         //
{	
	byte tx_data;
	
	if(uart2_GetTxData(&tx_data) == TRUE) {
  	 	USART_SendData(USART2, tx_data);  
	}
}
#endif
#ifdef UART3_ENABLE	
void uart3_TxData(void)
{	
	byte tx_data;
	
	if(uart3_GetTxData(&tx_data) == TRUE) {
  	 	USART_SendData(USART3, tx_data);  
	}
}
#endif
#ifdef UART4_ENABLE	
void uart4_TxData(void)                         //
{	
	byte tx_data;
	
	if(uart4_GetTxData(&tx_data) == TRUE) {
  	 	USART_SendData(UART4, tx_data);  
	}
}
#endif
#ifdef UART5_ENABLE	
void uart5_TxData(void)
{	
	byte tx_data;
	
	if(uart5_GetTxData(&tx_data) == TRUE) {
  	 	USART_SendData(UART5, tx_data);  
	}
}
#endif
/*-----------------------------------------------------------------------------
 Function Name  : uart1_GetTxData
 Description    : Get one byte tx data from tx buf
 Input          : None
 Output         : *data,bool
 Return         : None
 Comments	 	: ..
-----------------------------------------------------------------------------*/
bool uart1_GetTxData(byte *data)
{
	bool ret_val = FALSE;
	
	if(tx_existed == FALSE) {
		if(rs232_Tx_Head != rs232_Tx_Tail) {
    		tx_existed = TRUE;
        	if(++rs232_Tx_Tail >= TX_MAX_SIZE) rs232_Tx_Tail = 0;
			*data = tx_buf[rs232_Tx_Tail];
			ret_val = TRUE;
		}
	}	
	return ret_val;
}
#ifdef UART2_ENABLE	
bool uart2_GetTxData(byte *data)
{
	bool ret_val = FALSE;
	
	if(tx2_existed == FALSE) {
		if(rs232_Tx2_Head != rs232_Tx2_Tail) {
    		tx2_existed = TRUE;
        	if(++rs232_Tx2_Tail >= TX2_MAX_SIZE) rs232_Tx2_Tail = 0;
			*data = tx2_buf[rs232_Tx2_Tail];
			ret_val = TRUE;
		}
	}	
	return ret_val;
}
#endif
#ifdef UART3_ENABLE	
bool uart3_GetTxData(byte *data)
{
	bool ret_val = FALSE;
	
	if(tx3_existed == FALSE) {
		if(rs232_Tx3_Head != rs232_Tx3_Tail) {
    		tx3_existed = TRUE;
        	if(++rs232_Tx3_Tail >= TX3_MAX_SIZE) rs232_Tx3_Tail = 0;
			*data = tx3_buf[rs232_Tx3_Tail];
			ret_val = TRUE;
		}
	}	
	return ret_val;
}
#endif
#ifdef UART4_ENABLE	
bool uart4_GetTxData(byte *data)
{
	bool ret_val = FALSE;
	
	if(tx4_existed == FALSE) {
		if(rs232_Tx4_Head != rs232_Tx4_Tail) {
    		tx4_existed = TRUE;
        	if(++rs232_Tx4_Tail >= TX4_MAX_SIZE) rs232_Tx4_Tail = 0;
			*data = tx4_buf[rs232_Tx4_Tail];
			ret_val = TRUE;
		}
	}	
	return ret_val;
}
#endif
#ifdef UART5_ENABLE	
bool uart5_GetTxData(byte *data)
{
	bool ret_val = FALSE;
	
	if(tx5_existed == FALSE) {
		if(rs232_Tx5_Head != rs232_Tx5_Tail) {
    		tx5_existed = TRUE;
        	if(++rs232_Tx5_Tail >= TX5_MAX_SIZE) rs232_Tx5_Tail = 0;
			*data = tx5_buf[rs232_Tx5_Tail];
			ret_val = TRUE;
		}
	}	
	return ret_val;
}
#endif
/*-----------------------------------------------------------------------------
 Function Name  : PutChar
 Description    : Save the tx data
 Input          : None
 Output         : None
 Return         : None
 Comments	 	: ..
-----------------------------------------------------------------------------*/
void PutChar(byte data)							//RS232
{	
	rs232_Tx_Head++;
	if(rs232_Tx_Head >= TX_MAX_SIZE){
		rs232_Tx_Head = 0;
	}
	tx_buf[rs232_Tx_Head] = data;
}
#ifdef UART2_ENABLE	
void PutChar2(byte data)						//
{	
	rs232_Tx2_Head++;
	if(rs232_Tx2_Head >= TX2_MAX_SIZE){
		rs232_Tx2_Head = 0;
	}
	tx2_buf[rs232_Tx2_Head] = data;
}
#endif
#ifdef UART3_ENABLE	
void PutChar3(byte data)						//
{	
	rs232_Tx3_Head++;
	if(rs232_Tx3_Head >= TX3_MAX_SIZE){
		rs232_Tx3_Head = 0;
	}
	tx3_buf[rs232_Tx3_Head] = data;
}
#endif
#ifdef UART4_ENABLE	
void PutChar4(byte data)						//
{	
	rs232_Tx4_Head++;
	if(rs232_Tx4_Head >= TX4_MAX_SIZE){
		rs232_Tx4_Head = 0;
	}
	tx4_buf[rs232_Tx4_Head] = data;
}
#endif
#ifdef UART5_ENABLE	
void PutChar5(byte data)						//
{	
	rs232_Tx5_Head++;
	if(rs232_Tx5_Head >= TX5_MAX_SIZE){
		rs232_Tx5_Head = 0;
	}
	tx5_buf[rs232_Tx5_Head] = data;
}
#endif
/*-----------------------------------------------------------------------------
	UART1
-----------------------------------------------------------------------------*/
byte GetChar(void)
{
    if(rs232_Rx_Head != rs232_Rx_Tail) {
     	++rs232_Rx_Tail;
		if(rs232_Rx_Tail >= RX_MAX_SIZE) rs232_Rx_Tail = 0;
		return(rx_buf[rs232_Rx_Tail]);
	}else{
		return(0xff);
	}
}
#ifdef UART2_ENABLE	
byte GetChar2(void)
{
    if(rs232_Rx2_Head != rs232_Rx2_Tail) {
     	++rs232_Rx2_Tail;
		if(rs232_Rx2_Tail >= RX2_MAX_SIZE) rs232_Rx2_Tail = 0;
		return(rx2_buf[rs232_Rx2_Tail]);
	}else{
		return(0xff);
	}
}
#endif
#ifdef UART3_ENABLE	
byte GetChar3(void)
{
    if(rs232_Rx3_Head != rs232_Rx3_Tail) {
     	++rs232_Rx3_Tail;
		if(rs232_Rx3_Tail >= RX3_MAX_SIZE) rs232_Rx3_Tail = 0;
		return(rx3_buf[rs232_Rx3_Tail]);
	}else{
		return(0xff);
	}
}
#endif
#ifdef UART4_ENABLE	
byte GetChar4(void)
{
    if(rs232_Rx4_Head != rs232_Rx4_Tail) {
     	++rs232_Rx4_Tail;
		if(rs232_Rx4_Tail >= RX4_MAX_SIZE) rs232_Rx4_Tail = 0;
		return(rx4_buf[rs232_Rx4_Tail]);
	}else{
		return(0xff);
	}
}
#endif
#ifdef UART5_ENABLE	
byte GetChar5(void)
{
    if(rs232_Rx5_Head != rs232_Rx5_Tail) {
     	++rs232_Rx5_Tail;
		if(rs232_Rx5_Tail >= RX5_MAX_SIZE) rs232_Rx5_Tail = 0;
		return(rx5_buf[rs232_Rx5_Tail]);
	}else{
		return(0xff);
	}
}
#endif

/*-----------------------------------------------------------------------------
 Function Name  : rs232_printf
 Description    : 
 Input          : None
 Output         : None
 Return         : None
 Comments	 	: ..
-----------------------------------------------------------------------------*/
short rs232_printf(const char format[],...)
{
	char format_buf[100];        
    va_list list;

    va_start(list,format);
    vsprintf(format_buf,format,list);
    va_end(list);
    return rs232_PutString(format_buf);
}
/*-----------------------------------------------------------------------------
 Function Name  : rs232_PutString
 Description    : put string to uart1 tx buffer
 Input          : None
 Output         : None
 Return         : None
 Comments	 	: ..
-----------------------------------------------------------------------------*/
short rs232_PutString(const char str[])
{
     short length=0;
     while(str[length] != '\0') {
		++length;
	}     
     while (*str != '\0') {
          if(*str == '\n'){
		     PutChar('\r');
          }else{		     
               PutChar(*str);
          }               
          str++;
     }
     return length;
}     

#ifdef IAP_UPGRADE
/*-----------------------------------------------------------------------------
 Function Name  : rs232_CheckUpgrade
 Description    : put string to uart1 tx buffer
 Input          : None
 Output         : None
 Return         : None
 Comments	 	: ..
-----------------------------------------------------------------------------*/
void rs232_CheckUpgrade(void)
{
	char temp[] = "upgrade";

	if (up_buf[7] == '\r') {
		if (memcmp(up_buf,temp,7)==0) {
			UpgradeMode = ON;
			memset(&up_buf,0,sizeof(up_buf));
		}
	}
}	
byte rs232_GetUpgradeMode(void)
{
	return UpgradeMode;
}
void rs232_SetUpgradeMode(byte mode)
{
	UpgradeMode = mode;
}
#endif

/*

*********************************************************************/


void 	Put_RxBuffer(char ch)
{

#ifdef IC_DEBUG	
	if(ic_start_frame == 1){
		if(!ic_comm_use){ 
			if(ch != '\r'){
				if(ch == 0x08) { //  BS
					if(ic_comm_index) --ic_comm_index;
				}else{           				
					ic_comm_buf[ic_comm_index++] = ch;
				}                		
			}else{
				ic_start_frame = 0;
				ic_comm_ok = 1;                                         
			}
		}               
	}else{
		if(ch == 'i'){
			ic_start_frame = 1;
			//ic_comm_state = 1;
			ic_comm_index = 0;
		}               
	}          
#endif
}
/*
void	Clear_RxBuffer(void) 
{
	unsigned int i;
	    
	for(i=0;i<2048;i++) BUF_Rs232Rx[i] = 0;
	rs232_Rx_Head = 0;
	rs232_Rx_Tail = 0;
	
}*/

#ifdef IPOD_INCLUDE
/*-----------------------------------------------------------------------------
 Function Name  : uart2_GetRxCommand
 Description    : Get rx command from rx buf
 Input          : None
 Output         : None
 Return         : None
 Comments	 	: ..
-----------------------------------------------------------------------------*/
unsigned char uart2_GetRxCommand( TCLMPACKET *pTCLPacket, unsigned char uiPacketType, int IsNecessaryWaiting ,int* ErrCode)
{	
    unsigned char command_ok = iPod_FALSE;
    
    if (rs232_Rx2_Head != rs232_Rx2_Tail) {
    	do{
        	++rs232_Rx2_Tail;
            if (rs232_Rx2_Tail >= RX2_MAX_SIZE) rs232_Rx2_Tail = 0;

#ifdef ADD_SYNCSUB
            if ((rx2_buf[rs232_Rx2_Tail]==RX2_DATA_SYNCSUB)&&(sync==RX2_DATA_SYNC)&&(cmd_mode!=RX2232_SYNCSUB)) {
				pTCLPacket->ucSync = sync;
                rx2_checksum = pTCLPacket->ucSync;
                cmd_mode = RX2232_SYNCSUB;
               	word_count = mStep = didx = 0;
            }
            sync = rx2_buf[rs232_Rx2_Tail];
#endif
            
            switch(cmd_mode) {
                case RX2232_SYNC:
                    if(rx2_buf[rs232_Rx2_Tail] == RX2_DATA_SYNC) {
						pTCLPacket->ucSync = rx2_buf[rs232_Rx2_Tail];
                        rx2_checksum = pTCLPacket->ucSync;
                        cmd_mode++;
                    }
                    break;
#ifdef ADD_SYNCSUB
                case RX2232_SYNCSUB:
                    if(rx2_buf[rs232_Rx2_Tail] == RX2_DATA_SYNCSUB) {
						pTCLPacket->ucSyncSub = rx2_buf[rs232_Rx2_Tail];
                        rx2_checksum += pTCLPacket->ucSyncSub;
                        cmd_mode++;
                    } else {
		                cmd_mode = RX2232_SYNC;
                    }
                	break;
                case RX2232_SYNCSUB2:
                    if(rx2_buf[rs232_Rx2_Tail] == RX2_DATA_SYNCSUB2) {
						pTCLPacket->ucSyncSub2 = rx2_buf[rs232_Rx2_Tail];
                        rx2_checksum += pTCLPacket->ucSyncSub2;
                        cmd_mode++;
                    } else {
		                cmd_mode = RX2232_SYNC;
                    }
                    break;
#endif
                case RX2232_PID:
                	pTCLPacket->ucPID = rx2_buf[rs232_Rx2_Tail];
                    rx2_checksum += pTCLPacket->ucPID;
                   	cmd_mode++;
                   	word_count = mStep = 0;
                	break;
                case RX2232_TID:		/* get 2byte */
#if 1	//090626
					switch (mStep) {
					case 0: 
						mData[0] = rx2_buf[rs232_Rx2_Tail]; mStep++;
						break;
    	            case 1: 
						mData[1] = rx2_buf[rs232_Rx2_Tail];
						pTCLPacket->wTID = (mData[1]<<8) | mData[0];
                    	rx2_checksum += pTCLPacket->wTID;
	                   	word_count = mStep = 0;
                    	cmd_mode++;
    	                break;
    	            }
#else
                	if (++word_count >= 2) {
               			if (rs232_Rx2_Tail == 0) rs232_Rx2_Tail = (RX2_MAX_SIZE-1);
               			else rs232_Rx2_Tail--;
               				
                		rs232_Rx2_Tail = GetWord(&pTCLPacket->wTID,rs232_Rx2_Tail);
                    	rx2_checksum += pTCLPacket->wTID;
                    	cmd_mode++;
	                   	word_count = 0;
                    }
#endif
                	break;
                case RX2232_CODE:	/* get 2byte */
#if 1	//090626
					switch (mStep) {
					case 0: 
						mData[0] = rx2_buf[rs232_Rx2_Tail]; mStep++;
    	                break;
    	            case 1: 
						mData[1] = rx2_buf[rs232_Rx2_Tail];
						pTCLPacket->wCODE = (mData[1]<<8) | mData[0];
                    	rx2_checksum += pTCLPacket->wCODE;
	                   	word_count = mStep = 0;
                    	cmd_mode++;
    	                break;
    	            }
#else
                	if (++word_count >= 2) {
               			if (rs232_Rx2_Tail == 0) rs232_Rx2_Tail = (RX2_MAX_SIZE-1);
               			else rs232_Rx2_Tail--;

	                	rs232_Rx2_Tail = GetWord(&pTCLPacket->wCODE,rs232_Rx2_Tail);
    	                rx2_checksum += pTCLPacket->wCODE;
        	            cmd_mode++;
                       	word_count = 0;
	    	        }
#endif
					break;
                case RX2232_LENGTH:	/* get 2byte */
#if 1	//090626
					switch (mStep) {
					case 0: 
						mData[0] = rx2_buf[rs232_Rx2_Tail]; mStep++;
    	                break;
    	            case 1: 
						mData[1] = rx2_buf[rs232_Rx2_Tail];
						pTCLPacket->wLength = (mData[1]<<8) | mData[0];
                    	rx2_checksum += pTCLPacket->wLength;
    	                dlength = pTCLPacket->wLength;
	                   	word_count = mStep = didx = 0;
                    	cmd_mode++;
						if (dlength == 0) cmd_mode = RX2232_CHECKSUM;
    	                break;
    	            }
#else
                	if (++word_count >= 2) {
               			if (rs232_Rx2_Tail == 0) rs232_Rx2_Tail = (RX2_MAX_SIZE-1);
               			else rs232_Rx2_Tail--;

	                	rs232_Rx2_Tail = GetWord(&pTCLPacket->wLength,rs232_Rx2_Tail);
    	                rx2_checksum += pTCLPacket->wLength;
    	                dlength = pTCLPacket->wLength;
        	            cmd_mode++;
	                   	word_count = 0;
						if (dlength == 0) cmd_mode = RX2232_CHECKSUM;
        	        }
                   	didx = 0;
#endif
                    break;
                case RX2232_DATA:
                   	word_count = 0;
					didx = GetByte(pTCLPacket->pPacketData.pData,rs232_Rx2_Tail,didx);
                    rx2_checksum += rx2_buf[rs232_Rx2_Tail];
					if (--dlength == 0) cmd_mode++;
					break;
				case RX2232_CHECKSUM:
#if 1	//090626
					switch (mStep) {
					case 0: 
						mData[0] = rx2_buf[rs232_Rx2_Tail]; mStep++;
    	                break;
					case 1: 
						mData[1] = rx2_buf[rs232_Rx2_Tail]; mStep++;
    	                break;
					case 2: 
						mData[2] = rx2_buf[rs232_Rx2_Tail]; mStep++;
    	                break;
    	            case 3: 
						mData[3] = rx2_buf[rs232_Rx2_Tail];
						pTCLPacket->dwCRC = (mData[3]<<24) | (mData[2]<<16) | (mData[1]<<8) | mData[0];
						rx2_checksum += pTCLPacket->dwCRC;
	
						if( 0 == rx2_checksum ) {
                	        command_ok = iPod_TRUE;

							if ( TCL_GET_PACKET_TYPE(pTCLPacket->ucPID) != uiPacketType) {
								if ( TCL_GET_PACKET_TYPE(pTCLPacket->ucPID) == TCL_EVENT) {
									*ErrCode = TCLM_ERR_PACKETTYPE_MISMATCH;
								} else if ( TCL_GET_PACKET_TYPE(pTCLPacket->ucPID) == TCL_RESPONSE) {
									*ErrCode = TCLM_ERR_PACKETTYPE_MISMATCH_RESPONSE;
								} else if ( TCL_GET_PACKET_TYPE(pTCLPacket->ucPID) == TCL_DATA) {
									*ErrCode = TCLM_ERR_PACKETTYPE_MISMATCH_DATA;
								} else {
									*ErrCode = TCLM_ERR_PACKETTYPE_MISMATCH;
								}
                    		} else {
								*ErrCode = TCLM_NO_ERROR;
							}
						} else {
							*ErrCode = TCLM_ERR_CHECKSUM_FAIL;
						}
						cmd_mode = RX2232_SYNC;
	                   	word_count = mStep = didx = 0;
    	            }
#else
                	if (++word_count >= 4) {
                		for (; word_count > 1 ; word_count--) {
                			if (rs232_Rx2_Tail == 0) rs232_Rx2_Tail = (RX2_MAX_SIZE-1);
                			else rs232_Rx2_Tail--;
                		}
	                	rs232_Rx2_Tail = GetDoubleWord(&pTCLPacket->dwCRC,rs232_Rx2_Tail);

						rx2_checksum += pTCLPacket->dwCRC;
	
						if( 0 == rx2_checksum )
						{
                	        command_ok = iPod_TRUE;

							if ( TCL_GET_PACKET_TYPE(pTCLPacket->ucPID) != uiPacketType) {
								if ( TCL_GET_PACKET_TYPE(pTCLPacket->ucPID) == TCL_EVENT) {
									*ErrCode = TCLM_ERR_PACKETTYPE_MISMATCH;
								} else if ( TCL_GET_PACKET_TYPE(pTCLPacket->ucPID) == TCL_RESPONSE) {
									*ErrCode = TCLM_ERR_PACKETTYPE_MISMATCH_RESPONSE;
								} else if ( TCL_GET_PACKET_TYPE(pTCLPacket->ucPID) == TCL_DATA) {
									*ErrCode = TCLM_ERR_PACKETTYPE_MISMATCH_DATA;
								} else {
									*ErrCode = TCLM_ERR_PACKETTYPE_MISMATCH;
								}
                    		} else {
								*ErrCode = TCLM_NO_ERROR;
							}
						}
						else 
						{
							*ErrCode = TCLM_ERR_CHECKSUM_FAIL;
						}
						cmd_mode = RX2232_SYNC;
					}
#endif
                    break;
                default:
					cmd_mode = RX2232_SYNC;
                    break;
            }
        } while((rs232_Rx2_Head != rs232_Rx2_Tail) && (command_ok == iPod_FALSE)); 
    }
    return command_ok;
}        
/*-----------------------------------------------------------------------------
 Function Name  : GetByte
 Description    : Get rx command from rx buf
 Input          : None
 Output         : None
 Return         : None
 Comments	 	: ..
-----------------------------------------------------------------------------*/
unsigned short GetByte(unsigned char *pdata , unsigned short point , unsigned short didx)
{
	*(pdata+didx) = rx2_buf[point];
	didx++;
	return didx;
}
/*-----------------------------------------------------------------------------
 Function Name  : GetWord
 Description    : Get rx command from rx buf
 Input          : None
 Output         : None
 Return         : None
 Comments	 	: ..
-----------------------------------------------------------------------------*/
unsigned short GetWord( unsigned short *pdata , unsigned short point)
{
	unsigned char pTemp[2] = {0, };

	pTemp[0] = rx2_buf[point++];
    if(point >= RX2_MAX_SIZE) point = 0;
    	
	pTemp[1] = rx2_buf[point];
	*pdata = (pTemp[1]<<8) | pTemp[0];
	return point;
}

/*-----------------------------------------------------------------------------
 Function Name  : GetDoubleWord
 Description    : Get rx command from rx buf
 Input          : None
 Output         : None
 Return         : None
 Comments	 	: ..
-----------------------------------------------------------------------------*/
unsigned short GetDoubleWord( unsigned int *pdata ,unsigned short point)
{
	unsigned char pTemp[4] = {0, };
	
	pTemp[0] = rx2_buf[point++];
    if(point >= RX2_MAX_SIZE) point = 0;
		
	pTemp[1] = rx2_buf[point++];
    if(point >= RX2_MAX_SIZE) point = 0;

	pTemp[2] = rx2_buf[point++];
    if(point >= RX2_MAX_SIZE) point = 0;
		
	pTemp[3] = rx2_buf[point];

	*pdata = (pTemp[3]<<24) | (pTemp[2]<<16) | (pTemp[1]<<8) | pTemp[0];
	
	return point;
}
#endif	//IPOD_INCLUDE


/* end of file */
/******************* (C) COPYRIGHT 2008 Rotel Technology Global **************/

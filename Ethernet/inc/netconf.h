/**
  ******************************************************************************
  * @file    netconf.h
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    07-October-2011 
  * @brief   This file contains all the functions prototypes for the netconf.c 
  *          file.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __NETCONF_H
#define __NETCONF_H

#ifdef __cplusplus
 extern "C" {
#endif
typedef enum 
{ 
  DHCP_START=0,
  DHCP_WAIT_ADDRESS,
  DHCP_ADDRESS_ASSIGNED,
  DHCP_TIMEOUT
} 
DHCP_State_TypeDef;
typedef enum
{
  STATIC_WAIT_ETH_CONFIG,
  STATIC_IP_CONFIGURED
}
STATIC_STATE_TYPEDEF;
extern STATIC_STATE_TYPEDEF STATIC_state;
extern unsigned char iptab[4];
extern unsigned char iptxt[20];   
extern DHCP_State_TypeDef DHCP_state;
/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void LwIP_Init(void);
void Service_Restart(void);
void LwIP_Pkt_Handle(void);
void LwIP_Periodic_Handle(__IO uint32_t localtime);
void EthernetMain(void);
void EthernetStandy(void);
void EthernetRestartLwIP(void);
void CloseConnection(struct tcp_pcb *pcb);

#ifdef __cplusplus
}
#endif

#endif /* __NETCONF_H */


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/


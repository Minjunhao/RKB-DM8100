/**
  ******************************************************************************
  * @file    netconf.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    07-October-2011
  * @brief   Network connection configuration
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

/* Includes ------------------------------------------------------------------*/
#include "stm32f2xx.h"
#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/tcp.h"
#include "lwip/udp.h"
#include "lwip/dns.h"
#include "netif/etharp.h"
#include "lwip/dhcp.h"
#include "ethernetif.h"
#include "main.h"
#include "netconf.h"
#include "timer.h"
#include "stm32f2x7_eth.h"
#include "function.ext"
#include "initmcu.h"
#ifdef LWIP_IGMP
#include "lwip/igmp.h"
#endif
#ifdef IP_REASSEMBLY
#include "lwip/ip_frag.h"
#endif
#include "httpserver.h"
#include "power_led.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Private typedef -----------------------------------------------------------*/
#define MAX_DHCP_TRIES        4

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
struct netif netif;
//extern struct tcp_pcb *connected_pcb;
extern struct tcp_pcb *listenning_pcb;
uint32_t TCP_timer = 0;
uint32_t ARP_timer = 0;
uint32_t ip_address = 0;
uint32_t subnet_mask = 0;
uint32_t gateway    = 0;
uint32_t dns_addr    = 0;

//#ifdef USE_DHCP
uint32_t DHCP_fine_timer = 0;
uint32_t DHCP_coarse_timer = 0;
DHCP_State_TypeDef DHCP_state = DHCP_START;
STATIC_STATE_TYPEDEF STATIC_state = STATIC_WAIT_ETH_CONFIG;
uint8_t iptab[4];
uint8_t iptxt[20];
//#endif
#ifdef LWIP_DNS
uint32_t DNS_timer=0;
#endif
uint32_t TCP_out_timer=0;
#ifdef LWIP_IGMP
uint32_t IGMP_timer=0;
#endif
#ifdef IP_REASSEMBLY
uint32_t IPREASBLY_timer=0;
#endif

u8 dhcp_ip_addr[4];
u8 dhcp_subnet_mask[4];
u8 dhcp_gateway_addr[4];
u8 dhcp_dns[4];
u8 mode_dhcp_enable=1;
u8 mac_address[6]={0,0,0,0,0xff,0xff};

u8 manual_ip_addr[4];
u8 manual_subnet_mask[4];
u8 manual_gateway_addr[4];
u8 manual_dns[4];


/* Private functions ---------------------------------------------------------*/
void LwIP_DHCP_Process_Handle(void);
/**
  * @brief  ethernet process initial
  * @param  None
  * @retval None
  */
void ethernet_configuration(void)
{
    u8 i=0;
	EthInitStatus=0;
	if(mode_dhcp_enable==1){
	   for(i=0;i<4;i++)
		  {
             dhcp_ip_addr[i]=0;
			 dhcp_subnet_mask[i]=0;
			 dhcp_gateway_addr[i]=0;
			 dhcp_dns[i]=0;
	      }
		}
	PHY_POWER_EN(0);   //must power down
	PHY_initial_status=PHY_RESET;	
	PHY_NRESET(0);
    DHCP_state=DHCP_START;	
	STATIC_state=STATIC_WAIT_ETH_CONFIG;	
}
/**
  * @brief  reinitializes the lwIP stack and the tcp/udp service
  * @param  None
  * @retval None
  */
void service_restart(void)
{
  DHCP_state = DHCP_START;
  
  LwIP_Init();
  /* Initialize the TCP server*/
  tcp_server_init();
  /* Initialize the UDP server*/
  udp_server_init(); 
}
/**
  * @brief  Initializes the lwIP stack
  * @param  None
  * @retval None
  */
void LwIP_Init(void)
{
  struct ip_addr ipaddr;
  struct ip_addr netmask;
  struct ip_addr gw;

//#ifndef USE_DHCP
 #ifdef USE_LCD  
  uint8_t iptab[4];
  uint8_t iptxt[20];
 #endif /* USE_LCD */
//#endif /* USE_DHCP */

  /* Initializes the dynamic memory heap defined by MEM_SIZE.*/
  mem_init();

  /* Initializes the memory pools defined by MEMP_NUM_x.*/
  memp_init();
  
  if(mode_dhcp_enable==1)
  	{
      ipaddr.addr = 0;
      netmask.addr = 0;
      gw.addr = 0;
  	}
  else
  	{
  	 #if 0
     IP4_ADDR(&ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
     IP4_ADDR(&netmask, NETMASK_ADDR0, NETMASK_ADDR1 , NETMASK_ADDR2, NETMASK_ADDR3);
     IP4_ADDR(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
	 #endif
     IP4_ADDR(&ipaddr, manual_ip_addr[0], manual_ip_addr[1], manual_ip_addr[2], manual_ip_addr[3]);
     IP4_ADDR(&netmask, manual_subnet_mask[0], manual_subnet_mask[1] , manual_subnet_mask[2], manual_subnet_mask[3]);
	 IP4_ADDR(&gw, manual_gateway_addr[0], manual_gateway_addr[1], manual_gateway_addr[2], manual_gateway_addr[3]);
	 IP4_ADDR(&static_dns,manual_dns[0],manual_dns[1],manual_dns[2],manual_dns[3]);
	 STATIC_state=STATIC_IP_CONFIGURED;
  	}
#ifdef LWIP_DNS
   dns_init();
#endif
#ifdef LWIP_IGMP
   igmp_init();
#endif
  /* - netif_add(struct netif *netif, struct ip_addr *ipaddr,
            struct ip_addr *netmask, struct ip_addr *gw,
            void *state, err_t (* init)(struct netif *netif),
            err_t (* input)(struct pbuf *p, struct netif *netif))
    
   Adds your network interface to the netif_list. Allocate a struct
  netif and pass a pointer to this structure as the first argument.
  Give pointers to cleared ip_addr structures when using DHCP,
  or fill them with sane numbers otherwise. The state pointer may be NULL.

  The init function pointer must point to a initialization function for
  your ethernet netif interface. The following code illustrates it's use.*/
  netif_add(&netif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &ethernet_input);

  /*  Registers the default network interface.*/
  netif_set_default(&netif);

  /*  Creates a new DHCP client for this interface on the first call.
  Note: you must call dhcp_fine_tmr() and dhcp_coarse_tmr() at
  the predefined regular intervals after starting the client.
  You can peek in the netif->dhcp struct for the actual DHCP status.*/
  //if(use_dhcp==1)
    //dhcp_start(&netif);
   //igmp_init();
  /*  When the netif is fully configured this function must be called.*/
  netif_set_up(&netif);
}

/**
  * @brief  Called when a frame is received
  * @param  None
  * @retval None
  */
void LwIP_Pkt_Handle(void)
{
  /* Read a received packet from the Ethernet buffers and send it to the lwIP for handling */
  ethernetif_input(&netif);
}

/**
  * @brief  LwIP periodic tasks
  * @param  localtime the current LocalTime value
  * @retval None
  */
void LwIP_Periodic_Handle(__IO uint32_t localtime)
{
  struct tcp_pcb *pcb;
  u8_t i=0;
#if LWIP_TCP
  /* TCP periodic process every 250 ms */
  if (localtime - TCP_timer >= TCP_TMR_INTERVAL)
  {
    TCP_timer =  localtime;
    tcp_tmr();
  }
#endif
  
  /* ARP periodic process every 5s */
  if ((localtime - ARP_timer) >= ARP_TMR_INTERVAL)
  {
    ARP_timer =  localtime;
    etharp_tmr();
  }

#ifdef USE_DHCP
  if(mode_dhcp_enable==1)
  	{
  /* Fine DHCP periodic process every 500ms */
  if (localtime - DHCP_fine_timer >= DHCP_FINE_TIMER_MSECS)
  {
    DHCP_fine_timer =  localtime;
    dhcp_fine_tmr();
    if ((DHCP_state != DHCP_ADDRESS_ASSIGNED)&&(DHCP_state != DHCP_TIMEOUT))
    {      
      /* process DHCP state machine */
      LwIP_DHCP_Process_Handle();    
    }
  }

  /* DHCP Coarse periodic process every 60s */
  if (localtime - DHCP_coarse_timer >= DHCP_COARSE_TIMER_MSECS)
  {
    DHCP_coarse_timer =  localtime;
    dhcp_coarse_tmr();
  }
  	}
#endif
#ifdef LWIP_DNS
  if(localtime-DNS_timer>DNS_TMR_INTERVAL)
    {
      DNS_timer=localtime;
      dns_tmr();
    }
#endif
#ifdef LWIP_IGMP
  if(localtime-IGMP_timer>IGMP_TMR_INTERVAL)
  	{
      IGMP_timer=localtime;
	  igmp_tmr();
    }
#endif
    if(localtime-TCP_out_timer>TCP_OUT_TMR_INTERVAL)
     {
       TCP_out_timer=localtime;
       if(DHCP_state==DHCP_ADDRESS_ASSIGNED || STATIC_state==1){
	   	for (pcb = tcp_active_pcbs; NULL != pcb && i<MEMP_NUM_TCP_PCB; pcb = pcb->next,i++){
                          //ethernet_send_start(pcb);
                }
	   		}
	   tcp_connection_state_check();
     }
	
}

#ifdef _ETHERNET
void EthernetDmaInit(void)
{
	  	 ETH_GPIO_Config();
         /* Enable ETHERNET clock  */
         RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_ETH_MAC | RCC_AHB1Periph_ETH_MAC_Tx |
                        RCC_AHB1Periph_ETH_MAC_Rx, ENABLE);
                        
         /* Reset ETHERNET on AHB Bus */
         ETH_DeInit();
         /* Software reset */
         ETH_SoftwareReset();
		 EthInitStatus=1;
	  	 while(ETH_GetSoftwareResetStatus() != SET)
	  		{
		    }
              EthInitStatus=2;
			  //VFD_Bk(1);
			  //VFD_display("  Connecting...    ", 20, 1);
			  //VFD_display("                   ", 20, 2);
			  PHY_POWER_EN(1);
              PHY_NRESET(1);
              SMPS_nPowerOn(0);
              DelayMs(2000);
              //PHY_NRESET(1);        
}

static u16 ethernet_main_time_out=0;
void ethernet_dma_reset(void)
{
    ETH_GPIO_Config();
	//Eth_Link_EXTIConfig();              //20150630
    /* Enable ETHERNET clock  */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_ETH_MAC | RCC_AHB1Periph_ETH_MAC_Tx |
                        RCC_AHB1Periph_ETH_MAC_Rx, ENABLE);         
    /* Reset ETHERNET on AHB Bus */
    ETH_DeInit();       
    /* Software reset */
    ETH_SoftwareReset();

	while(ETH_GetSoftwareResetStatus() != SET){}

    EthInitStatus=2;
	//ethernet_main_time_out=0;
    PHY_POWER_EN(1);
	PHY_NRESET(1); 
	SMPS_On();	
    //DelayMs(2000);
    //PHY_NRESET(1); 	
}
void EthernetMain(void)
{
   switch(EthInitStatus)
   	{
      case 0:
	  	 ETH_GPIO_Config();
		 //Eth_Link_EXTIConfig();              //20150630
         /* Enable ETHERNET clock  */
         RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_ETH_MAC | RCC_AHB1Periph_ETH_MAC_Tx |
                        RCC_AHB1Periph_ETH_MAC_Rx, ENABLE);         
         /* Reset ETHERNET on AHB Bus */
         ETH_DeInit();       
         /* Software reset */
         ETH_SoftwareReset();
	
         EthInitStatus=1;
	    break;
	  case 1:
	  	if(ETH_GetSoftwareResetStatus() == SET)
	  		{
              EthInitStatus=2;
			  ethernet_main_time_out=0;
			  PHY_POWER_EN(1); 
              PHY_NRESET(1); 
		    }
		else
			{
              if(ethernet_main_time_out>0x0FFF)
              	{
                  EthInitStatus=0;
				  ethernet_main_time_out=0;
			    }
		    }
		ethernet_main_time_out++;
		break;
	  case 2:
	  	PHY_NRESET(1);
	  	ETH_MACDMA_Config();
		break;
	  case 3:   //set the interrupt
	  	Eth_Link_PHYITConfig(LAN8720_PHY_ADDRESS);
	    Eth_Link_EXTIConfig();
		EthInitStatus=4;
		break;
	  case 4:
		LwIP_Init();
	    /* Initialize the TCP server*/
        tcp_server_init();
        /* Initialize the UDP server*/
	     //udp_server_init();
	    EthInitStatus=5;
		break;
	  case 5:
        if (ETH_CheckFrameReceived())
   	      {
           /* process received ethernet packet */
           LwIP_Pkt_Handle();
          }
          /* handle periodic timers for LwIP */
        LwIP_Periodic_Handle(LocalTime); 
	  break;
	  case 6:           
	  	break;
	  case 7:
	  	EthernetRestartLwIP();
	  	break;
	  default:
	  	break;
    }
}
void CloseConnection(struct tcp_pcb *pcb)
{
   if(pcb != NULL)
   	{
      tcp_close(pcb);
	  tcp_abandon(pcb,1);
    }
}
void EthernetStandy(void)
{
   u8_t i=0;
   if(EthInitStatus==5){
   	CloseConnection(listenning_pcb);
	CloseConnection(connected_pcb_upgrade);
   for(i=0;i<TCP_CLIENT_NUM;i++)
   	{
   	 p_tcp[i].state=TCP_CLOSED;
	 CloseConnection(p_tcp[i].pcb);
   	}
   #ifdef _SC_CONTROL
   for(i=0;i<TCP_SC_CLIENT_NUM;i++)
   	{
      p_sc_tcp[i].state=TCP_CLOSED;
	  CloseConnection(p_sc_tcp[i].pcb);
    }
   #endif
   netif_remove(&netif);
   }
   EthInitStatus=6;
   //20141106 : Ed	
   PHY_POWER_EN(0); 
}
void EthernetRestartLwIP(void)
{
   u8_t i=0;
   for(i=0;i<TCP_CLIENT_NUM;i++){
   	p_tcp[i].state=TCP_CLOSED;
	CloseConnection(p_tcp[i].pcb);
   	}
   #ifdef _SC_CONTROL
   for(i=0;i<TCP_SC_CLIENT_NUM;i++)
   	{
      p_sc_tcp[i].state=TCP_CLOSED;
	  CloseConnection(p_sc_tcp[i].pcb);
    }   
   #endif
   CloseConnection(listenning_pcb);
   CloseConnection(connected_pcb_upgrade);
   EXTI_DeInit();
   netif_remove(&netif);
   ethernet_configuration();  
}
#endif
#ifdef USE_DHCP
/**
  * @brief  LwIP_DHCP_Process_Handle
  * @param  None
  * @retval None
  */
void LwIP_DHCP_Process_Handle()
{

  switch (DHCP_state)
  {
    case DHCP_START:
    {
      dhcp_start(&netif);
      ip_address= 0;
      DHCP_state = DHCP_WAIT_ADDRESS;
    }
    break;

    case DHCP_WAIT_ADDRESS:
    {
      /* Read the new IP address */
      ip_address = netif.ip_addr.addr;
	  subnet_mask = netif.netmask.addr;
	  gateway    = netif.gw.addr;
      dns_addr    = (netif.dhcp)->offered_dns_addr[0].addr;
      if (ip_address!=0) 
      {
        DHCP_state = DHCP_ADDRESS_ASSIGNED;	

        /* Stop DHCP */
        dhcp_stop(&netif);
        udp_server_init();
   
        iptab[3] = (uint8_t)(ip_address >> 24);
        iptab[2] = (uint8_t)(ip_address >> 16);
        iptab[1] = (uint8_t)(ip_address >> 8);
        iptab[0] = (uint8_t)(ip_address);

		memcpy(dhcp_ip_addr,iptab,4);
		
        sprintf((char*)iptxt, "%d.%d.%d.%d", iptab[0], iptab[1], iptab[2], iptab[3]);       

        dhcp_subnet_mask[3] = (uint8_t)(subnet_mask >> 24);
        dhcp_subnet_mask[2] = (uint8_t)(subnet_mask >> 16);
        dhcp_subnet_mask[1] = (uint8_t)(subnet_mask >> 8);
        dhcp_subnet_mask[0] = (uint8_t)(subnet_mask);

        dhcp_gateway_addr[3] = (uint8_t)(gateway>> 24);
        dhcp_gateway_addr[2] = (uint8_t)(gateway >> 16);
        dhcp_gateway_addr[1] = (uint8_t)(gateway >> 8);
        dhcp_gateway_addr[0] = (uint8_t)(gateway);	

        dhcp_dns[3] = (uint8_t)(dns_addr >> 24);
        dhcp_dns[2] = (uint8_t)(dns_addr >> 16);
        dhcp_dns[1] = (uint8_t)(dns_addr >> 8);
        dhcp_dns[0] = (uint8_t)(dns_addr);	
      }
      else
      {
        /* DHCP timeout */
        if (netif.dhcp->tries > MAX_DHCP_TRIES)
        {
          DHCP_state = DHCP_TIMEOUT;

          /* Stop DHCP */
          dhcp_stop(&netif);
             
        }
      }
    }
    break;
    default: break;
  }
}
#endif      

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/

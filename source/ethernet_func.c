/**************************************************************************//**
 * @file     ethernet_func.c
 * @brief    manage the ethernet function
 * @version  V0.01
 * @date     13. August 2015
 *
 * @note
 * Copyright (C) 2015 ROTEL Limited. All rights reserved.
 *
 ******************************************************************************/
 #include "ethernet_func.h"



u8 dhcp_ip_addr[4];
u8 dhcp_subnet_mask[4];
u8 dhcp_gateway_addr[4];
u8 dhcp_dns[4];
u8 mode_dhcp_enable;

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
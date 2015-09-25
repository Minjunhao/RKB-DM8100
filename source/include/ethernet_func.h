#ifndef _ETHERNET_FUNC
#define _ETHERNET_FUNC

extern u8 dhcp_ip_addr[4];
extern u8 dhcp_subnet_mask[4];
extern u8 dhcp_gateway_addr[4];
extern u8 dhcp_dns[4];
extern u8 mode_dhcp_enable;


void ethernet_configuration(void);
#endif //_ETHERNET_FUNC

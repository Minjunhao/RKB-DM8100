/*
 * Copyright (c) 2001, Swedish Institute of Computer Science.
 * All rights reserved. 
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions 
 * are met: 
 * 1. Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer. 
 * 2. Redistributions in binary form must reproduce the above copyright 
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the distribution. 
 * 3. Neither the name of the Institute nor the names of its contributors 
 *    may be used to endorse or promote products derived from this software 
 *    without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND 
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE 
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS 
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY 
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF 
 * SUCH DAMAGE. 
 *
 * http.h
 *                     
 * Author : Adam Dunkels <adam@sics.se>                               
 *
 * CHANGELOG: this file has been modified by Sergio Perez Alcañiz <serpeal@upvnet.upv.es> 
 *            Departamento de Informática de Sistemas y Computadores          
 *            Universidad Politécnica de Valencia                             
 *            Valencia (Spain)    
 *            Date: March 2003                                          
 *
 */

#ifndef __HTTPD_H__
#define __HTTPD_H__

#include "lwip/def.h"
#include "fsdata.h"
#include "lwipopts.h"




struct fs_file {
  char *data;
  int len;
};
typedef enum{
  TCP_CLOSED,
  TCP_CONNECTED,
  TCP_WAIT_SENDBACK
} TCP_STATE;
typedef enum{
 SW_GET_VERSION_STATUS,
 SW_UP_TO_DATE,
 SW_NEW_VERSION_EXISTED,
 SW_GET_VERSION_FAILED
}
CHECK_SW_TYPEDEF;
#define TCP_OUT_TMR_INTERVAL  250*4
typedef struct message_buffer_state
{
  char *buffer;    //ring buffer for sending data
  u16_t head;      //ring buffer data in
  u16_t tail;      //ring buffer data out
  u16_t left;      //data left in buffer
  TCP_STATE state; //TCP connect state
  struct tcp_pcb *pcb;
} CONNECTED_PCB;

extern CONNECTED_PCB p_tcp[TCP_CLIENT_NUM];
extern CONNECTED_PCB p_sc_tcp[TCP_SC_CLIENT_NUM];
extern CHECK_SW_TYPEDEF SW_version_status;
extern struct tcp_pcb *connected_pcb_upgrade;

#define DEST_PORT       80
#define ISDIGIT(x) ((x)>='0' && (x)<='9')
#define ISLOWER(x) ((x)>='a' && (x)<='f')
#define ISUPPER(x) ((x)>='A' && (x)<='F')
#define HEX_TO_NUM(x) (ISDIGIT(x))?((x)-'0'):(ISLOWER(x)?((x)-'a'+10):(ISUPPER(x)?((x)-'A'+10):(-1)))
extern uint16_t ethernet_data_head;
extern uint16_t ethernet_data_tail;
extern u16_t ethernet_sc_data_head;
extern u16_t ethernet_sc_data_tail;
void IAP_httpd_init(void);
void udp_server_init (void);
int udp_sendx(char *data, u16_t len) ;
void tcp_server_init(void);
void ethernet_put_data(byte data);
u8_t ethernet_get_data(void);
void ip_control_init(void);
void MSG_send(char *msg,u16_t len);
void httpd_client_init(void);
void dns_server_found(const char *name,struct ip_addr *ipaddrServer,void *arg);
void ethernet_send_message(char *msg,u16_t len);
//void ethernet_send_start(struct tcp_pcb *pcb);
void tcp_connection_state_check(void);
void close_tcp(struct tcp_pcb *pcb);
u8_t ethernet_get_SC_data(void);
void ethernet_sc_send_message(char *msg,u16_t len);
#endif /* __HTTPD_H__ */

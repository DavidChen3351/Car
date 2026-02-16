#include "UDP.h"
#include "rl_net.h"
#include "cmsis_os2.h"
#include "uart.h"

#define PORT 2000
#define UPPER_IP4_STRING "192.168.0.100"

uint32_t UDP_cb_func (int32_t socket, const  NET_ADDR *addr, const uint8_t *buf, uint32_t len);
void UDP_SendData (uint8_t* buffer, uint32_t len);

int32_t udp_sock;                       // UDP socket handle
NET_ADDR4 UPPER_NET_ADDR;
uint8_t UPPER_IP4_ADDR[NET_ADDR_IP4_LEN];

void UDP_Ini()
{
	netIP_aton (UPPER_IP4_STRING, NET_ADDR_IP4, &UPPER_IP4_ADDR[0]);

  UPPER_NET_ADDR.port = PORT;
	UPPER_NET_ADDR.addr_type = NET_ADDR_IP4;
	netIP_aton (UPPER_IP4_STRING,NET_ADDR_IP4,&UPPER_NET_ADDR.addr[0]);
	
	uint8_t mac_addr [NET_ADDR_ETH_LEN];
  while (netARP_GetMAC (NET_IF_CLASS_ETH | 0, &UPPER_IP4_ADDR[0], mac_addr) != netOK) 
  {
	  // Requested IP address not yet cached
	  netARP_CacheIP (NET_IF_CLASS_ETH | 0, &UPPER_IP4_ADDR[0], netARP_CacheTemporaryIP);
	  osDelay(1000);
	}
	
  udp_sock = netUDP_GetSocket (UDP_cb_func);
  if(udp_sock >= 0) {
    netUDP_Open (udp_sock, PORT);
  }

  return;
}

// Notify the user application about UDP socket events.
uint32_t UDP_cb_func (int32_t socket, const  NET_ADDR *addr, const uint8_t *buf, uint32_t len) 
{
	(void) socket;
	(void) addr;
	(void) buf;
	(void) len;
  // Data received
  /* Example
  if ((buf[0] == 0x01) && (len == 2)) {
    // Switch LEDs on and off
    // LED_out (buf[1]);
  }
  */
  return (0);
}

uint8_t* UDP_GetBuffer(uint32_t len)
{
  if(udp_sock < 0){
    return NULL;
  }
  return netUDP_GetBuffer(len);
} 

// Send UDP data to destination client.
void UDP_SendData (uint8_t* buffer, uint32_t len) {
  if (udp_sock >= 0) {
    if(netUDP_Send (udp_sock, (NET_ADDR *)&UPPER_NET_ADDR, buffer, len) == netOK)
		{
			
		}
  }
}

// Allocate and initialize the socket.
/* Example
int main (void) {
 
  netInitialize ();
 
  // Initialize UDP socket and open port 2000
  udp_sock = netUDP_GetSocket (udp_cb_func);
  if (udp_sock > 0) {
    netUDP_Open (udp_sock, 2000);
  }
}
*/
//! [code_UDP_Socket]

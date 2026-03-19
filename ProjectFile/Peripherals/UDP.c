#include "UDP.h"
#include "rl_net.h"
#include "cmsis_os2.h"
#include "uart.h"

#include <stdbool.h>
#include <string.h>

#define MAX_HANDLE 4
#define PORT 2000
#define UPPER_IP4_STRING "192.168.0.100"

typedef struct{
  uint8_t *bufferPtr;
  uint32_t bufferMaxLen;
  uint32_t writePtr;
}UDP_Buffer;

typedef enum{
  UNINTIALIZED = 0,
  READY,
  BUFFER_ALLOCATED,
}UDP_HandleState;

typedef struct{
  NET_ADDR remoteAddr;
  int32_t udp_sock;
  UDP_HandleState state;
  UDP_Buffer buffer;
}UDP_Handle;

int32_t udp_sock; // UDP socket handle
NET_ADDR4 UPPER_NET_ADDR;
uint8_t UPPER_IP4_ADDR[NET_ADDR_IP4_LEN];

UDP_Handle handleTable[MAX_HANDLE];

void UDP_Ini()
{
  // initialize network
  netInitialize();
  
  netIP_aton(UPPER_IP4_STRING, NET_ADDR_IP4, &UPPER_IP4_ADDR[0]);

  UPPER_NET_ADDR.port = PORT;
  UPPER_NET_ADDR.addr_type = NET_ADDR_IP4;
  netIP_aton(UPPER_IP4_STRING, NET_ADDR_IP4, &UPPER_NET_ADDR.addr[0]);

  uint8_t mac_addr[NET_ADDR_ETH_LEN];
  while (netARP_GetMAC(NET_IF_CLASS_ETH | 0, &UPPER_IP4_ADDR[0], mac_addr) != netOK)
  {
    // Requested IP address not yet cached
    netARP_CacheIP(NET_IF_CLASS_ETH | 0, &UPPER_IP4_ADDR[0], netARP_CacheTemporaryIP);
    osDelay(1000);
  }

  udp_sock = netUDP_GetSocket(UDP_cb_func);
  if (udp_sock >= 0)
  {
    netUDP_Open(udp_sock, PORT);
  }

  return;
}

// Notify the user application about UDP socket events.
uint32_t UDP_cb_func(int32_t socket, const NET_ADDR *addr, const uint8_t *buf, uint32_t len)
{
  (void)socket;
  (void)addr;
  (void)buf;
  (void)len;
  // Data received
  /* Example
  if ((buf[0] == 0x01) && (len == 2)) {
    // Switch LEDs on and off
    // LED_out (buf[1]);
  }
  */
  return (0);
}


bool UDP_GetBuffer(UDP_Handle handle, uint32_t len)
{
  if(handle.state != READY)
  {
    return false;
  }
  handle.buffer.bufferPtr = netUDP_GetBuffer(len);
  handle.buffer.bufferMaxLen = len;
  handle.buffer.writePtr = 0;
  handle.state = BUFFER_ALLOCATED;
  return true;
}

bool UDP_WriteBuffer(UDP_Handle handle, uint8_t *ptr, uint32_t len)
{
  if (handle.state != BUFFER_ALLOCATED || handle.buffer.writePtr + len > handle.buffer.bufferMaxLen)
  {
    return false;
  }
  memcpy(handle.buffer.bufferPtr + handle.buffer.writePtr, ptr, len);
  handle.buffer.writePtr += len;
  return true;
}

// Send UDP data to destination client.
bool UDP_SendData(UDP_Handle handle)
{
  if (handle.state == BUFFER_ALLOCATED)
  {
    handle.state = READY;//the buffer will be released no matter send success or not
    if (netUDP_Send(udp_sock, (NET_ADDR *)&UPPER_NET_ADDR, handle.buffer.bufferPtr, handle.buffer.writePtr) == netOK)
    {
      return true;
    }
  }
  return false;
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

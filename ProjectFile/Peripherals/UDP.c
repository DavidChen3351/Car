#include "UDP.h"
#include "rl_net.h"
#include "cmsis_os2.h"
#include "uart.h"

#include <stdbool.h>
#include <string.h>

#define MAX_HANDLE 4
#define PORT 2000
#define UPPER_IP4_STRING "192.168.0.100"
uint32_t UDP_cb_func(int32_t socket, const NET_ADDR *addr, const uint8_t *buf, uint32_t len);

typedef struct{
  uint8_t *bufferPtr;
  uint32_t bufferMaxLen;
  uint32_t bufferLen;
}UDP_Buffer;

typedef enum{
  UNINTIALIZED = 0,
  READY,
  BUFFER_ALLOCATED,
}UDP_HandleState;

typedef struct{
  NET_ADDR4 netAddr;
  int32_t udpSock;
  UDP_HandleState state;
  UDP_Buffer buffer;
  UDP_UserCallBack CB_Func;
}UDP_Handle;

NET_ADDR4 UPPER_NET_ADDR;

UDP_Handle handleTable[MAX_HANDLE];

void UDP_Ini()
{
	for(int i =0;i<MAX_HANDLE;i++)
	{
		handleTable[i].state = UNINTIALIZED;
	}
  // initialize network
  netInitialize();

  UPPER_NET_ADDR.port = 0;
  UPPER_NET_ADDR.addr_type = NET_ADDR_IP4;
  netIP_aton(UPPER_IP4_STRING, NET_ADDR_IP4, &UPPER_NET_ADDR.addr[0]);

  uint8_t mac_addr[NET_ADDR_ETH_LEN];
  while (netARP_GetMAC(NET_IF_CLASS_ETH | 0, &UPPER_NET_ADDR.addr[0], mac_addr) != netOK)
  {
    // Requested IP address not yet cached
    netARP_CacheIP(NET_IF_CLASS_ETH | 0, &UPPER_NET_ADDR.addr[0], netARP_CacheTemporaryIP);
    osDelay(50);
  }
  return;
}

int UDP_GetHandle(uint16_t userPort,UDP_UserCallBack cb)
{
	int i;
  for(i=0;i < MAX_HANDLE;i++)
  {
    if(handleTable[i].state == UNINTIALIZED) break;
    if(i == MAX_HANDLE - 1) return -1;
  }
  int32_t sock = netUDP_GetSocket(UDP_cb_func);
  if(sock <= 0) return -1;
  if(netUDP_Open(sock, userPort) != netOK) return -1;
  //open successful
  handleTable[i].netAddr = UPPER_NET_ADDR;
  handleTable[i].netAddr.port = userPort;
  handleTable[i].udpSock = sock;
  handleTable[i].state = READY;
  handleTable[i].CB_Func = cb;
  return(i);
}

// Notify the user application about UDP socket events.
uint32_t UDP_cb_func(int32_t socket, const NET_ADDR *addr, const uint8_t *buf, uint32_t len)
{
  (void)addr;
  int i = 0;
  while(i < MAX_HANDLE)
  {
    if(handleTable[i].udpSock == socket) break;
    if(i == MAX_HANDLE) return 0;
  }
  handleTable[i].CB_Func(buf,len);
  return (0);
}

bool UDP_GetBuffer(int i, uint32_t len)
{
  UDP_Handle* handle = &handleTable[i];
  if(handle->state != READY)
  {
    return false;
  }

  handle->buffer.bufferPtr = netUDP_GetBuffer(len);
  handle->buffer.bufferMaxLen = len;
  handle->buffer.bufferLen = 0;
  handle->state = BUFFER_ALLOCATED;
  return true;
}

bool UDP_WriteBuffer(int i, uint8_t *ptr, uint32_t len)
{
  UDP_Handle* handle = &handleTable[i];
  if (handle->state != BUFFER_ALLOCATED || handle->buffer.bufferLen + len > handle->buffer.bufferMaxLen)
  {
    return false;
  }
  memcpy(handle->buffer.bufferPtr + handle->buffer.bufferLen, ptr, len);
  handle->buffer.bufferLen += len;
  return true;
}

// Send UDP data to destination client.
bool UDP_SendData(int i)
{
  UDP_Handle* handle = &handleTable[i];
  if (handle->state == BUFFER_ALLOCATED)
  {
    handle->state = READY;//the buffer will be released no matter send success or not
    if (netUDP_Send(handle->udpSock, (NET_ADDR *)&handle->netAddr, handle->buffer.bufferPtr, handle->buffer.bufferLen) == netOK)
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

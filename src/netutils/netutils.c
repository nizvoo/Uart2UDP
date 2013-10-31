/*
  Copyright (C) 2000-2013
  nizvoo (AT) gmail.com
*/


#include <stdio.h>
#include <stdlib.h>

#include <Winsock2.h> 

#include "inc/netutils/netutils.h"

BOOL FreeClientSocket(struct net_t* net)
{
  if (net && net->socket != SOCKET_ERROR)
    closesocket(net->socket);
  return TRUE;
}

BOOL BuildClientSocket(const char* ip, unsigned short port, BOOL block, struct net_t* net)
{
    //---------------------------------------------
    // Create a socket for sending data
    net->socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (net->socket == INVALID_SOCKET) {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        return FALSE;
    }

    // If iMode!=0, non-blocking mode is enabled.
	if (!block) {
		u_long iMode=1;
		ioctlsocket(net->socket,FIONBIO,&iMode);
	}

    //---------------------------------------------
    // Set up the RecvAddr structure with the IP address of
    // the receiver (in this example case "192.168.1.100")
    // and the specified port number.
    net->sockaddr.sin_family = AF_INET;
    net->sockaddr.sin_port = htons(port);
    net->sockaddr.sin_addr.s_addr = inet_addr(ip);
	
	return TRUE;
}

BOOL BuildServerSocket(unsigned short port, BOOL block, struct net_t* net)
{
  int rev_buf_sz = 16 * 1024;
  int snd_buf_sz = 16 * 1024;
  int iResult;
  int addr_sz;
  //---------------------------------------------
  // Create a socket for sending data
  net->socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (net->socket == INVALID_SOCKET) {
    printf("socket failed with error: %ld\n", WSAGetLastError());
    return FALSE;
  }

  // If iMode!=0, non-blocking mode is enabled.
  if (!block) {
    u_long iMode=1;
    ioctlsocket(net->socket,FIONBIO,&iMode);
  }
  memset(&net->sockaddr, 0x00, sizeof(net->sockaddr));
  net->sockaddr.sin_family = AF_INET;
  net->sockaddr.sin_port = htons(port);
  net->sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);

  addr_sz = sizeof (net->sockaddr);
  iResult = bind(net->socket, (SOCKADDR*)&net->sockaddr, addr_sz);
  if (iResult != 0) {
    int error = WSAGetLastError();
    closesocket(net->socket);

    fprintf(stderr, "Bind failed with error %d\n", WSAGetLastError());
    return FALSE;
  }

  setsockopt(net->socket,SOL_SOCKET,SO_RCVBUF,(const char*)&rev_buf_sz,sizeof(int));

  return TRUE;
}

BOOL InitSocket()
{
    WSADATA wsaData;

    //-----------------------------------------------
    // Initialize Winsock
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != NO_ERROR) {
        wprintf(L"WSAStartup failed with error %d\n", iResult);
        return FALSE;
    }

    return TRUE;
}

void UninitSocket()
{
  WSACleanup();
}

int net_sendto(const struct net_t* net, const char* buf, int len)
{
  return sendto(net->socket, buf, len, 0, (SOCKADDR*)&(net->sockaddr), sizeof(net->sockaddr));
}

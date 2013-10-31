/*
  Copyright (C) 2000-2013
  nizvoo (AT) gmail.com
*/


#include <stdio.h>
#include <stdlib.h>

#include <WinSock2.h>

#include "inc/netutils/netutils.h"

#define HI8(X) ((X) >> 8)
#define LO8(X) ((X) & 0x00ff)

struct tagActiveReceiver
{
  unsigned char id;
};

static
void show_user_buf(const char* buf, int len, int dir)
{
  int i;
  if (dir == 0)
    fprintf(stdout, "--->");
  else
    fprintf(stdout, "<---");
  for (i = 0; i < len; ++i)
    fprintf(stdout, "%02X ", (BYTE)buf[i]);
    
  fprintf(stdout, " :%d\n", len);
}

static HANDLE g_pattern_wait_event;

static HANDLE g_running_event = NULL;
static HANDLE g_stop_event = NULL;


static BOOL ProcessPacketMessage(const char* buf, int len)
{
  unsigned char i;

  ForwardToUART(buf, len);
  
  return TRUE;
}

static void ClearThreadEvent()
{
  CloseHandle(g_running_event);
  CloseHandle(g_stop_event);

  g_running_event = NULL;
  g_stop_event = NULL;
}

static void SendActiveReceiver()
{
  // Data 
  struct tagActiveReceiver data;
  struct net_t net;

  BuildClientSocket("127.0.0.1", 8233, 0, &net);

  net_sendto(&net, (const char*)&data, sizeof(data));

  FreeClientSocket(&net);
}

int StopControlServer()
{
  int ret;

  if (g_running_event) {
    SetEvent(g_running_event);
    SendActiveReceiver();
    WaitForSingleObject(g_stop_event, INFINITE);
  }

  ClearThreadEvent();

  ret = WSAGetLastError();

  if (ret == SOCKET_ERROR) {
    fprintf(stderr, "\nclosesocket failed with error %d\n", WSAGetLastError());
    return FALSE;
  }
  //-----------------------------------------------
  // Clean up and exit.
  fprintf(stdout, "\nExiting.\n");

  return TRUE;
}

#define MAX_BUF_LEN      (8192)
static DWORD __stdcall RecvProc(void* p)
{
  struct net_t* net = (struct net_t*)p;
  int ret;
  char rev_buf[MAX_BUF_LEN];

  struct sockaddr_in client_sockaddr;
  int sendsize = sizeof (client_sockaddr);
  
  if (!p) return S_FALSE;
  
  while (1) {
    ret = recvfrom(net->socket, rev_buf, MAX_BUF_LEN, 0, (SOCKADDR*)&client_sockaddr, &sendsize);
    if (ret != SOCKET_ERROR) {
      fprintf(stdout, "Received:%d\n", ret);

      ProcessPacketMessage(rev_buf, ret);
    }
    if (WaitForSingleObject(g_running_event, 0) == WAIT_OBJECT_0) break;
  }
  SetEvent(g_stop_event);

  return S_OK;
}

DWORD ProcessUDPRawData(struct net_t* net)
{
  int ret;
  char rev_buf[MAX_BUF_LEN];

  struct sockaddr_in client_sockaddr;
  int sendsize = sizeof (client_sockaddr);
  
  if (!net) return S_FALSE;
  
  ret = recvfrom(net->socket, rev_buf, MAX_BUF_LEN, 0, (SOCKADDR*)&client_sockaddr, &sendsize);
  if (ret != SOCKET_ERROR) {
    fprintf(stdout, "Received:%d\n", ret);
    show_user_buf(rev_buf, ret, 1);
    ProcessPacketMessage(rev_buf, ret);
    //ForwardToUART(rev_buf, ret);
    //net_sendto(net, rev_buf, ret);
  }

  return S_OK;
}

int StartControlServer(struct net_t* net)
{
  g_running_event = CreateEvent(NULL, TRUE, FALSE, NULL);
  g_stop_event = CreateEvent(NULL, TRUE, FALSE, NULL);
  CreateThread(NULL, 0, RecvProc, (void*)net, 0, NULL);
  return TRUE;
}
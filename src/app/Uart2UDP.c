/*
  Copyright (C) 2000-2013
  nizvoo (AT) gmail.com
*/

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

#include "inc/comm/ComPortUtils.h"

#include "inc/netutils/netutils.h"

#include <stdint.h>


#define UART_DELAY 10

#define BUF_LEN (512)

#define HI8(X) ((X) >> 8)
#define LO8(X) ((X) & 0x00ff)

#define MAX_ONLINE (128)

struct user_st
{
  uint16_t server_port;
  uint16_t client_port;
  TCHAR port_name[MAX_PATH + 1];
  uint16_t baudrate;
  char client_addr[128];
};

static
void show_user_helper(void)
{
  fprintf(stdout, "Uart2UDP.exe COMx BAUDRATE SVRADDR SVRPORT CLIADDR CLIPORT");
}

static
void parse_entry_param(int argc, char * argv[], struct user_st* user_data)
{
  uint16_t svr_port = 8233;
  uint16_t cli_port = 8235;
  char cli_addr[120] = "127.0.0.1";
  uint16_t baudrate = 9600;
  
  
  TCHAR full_port_name[MAX_PATH + 1] = {0};
  // Setup comm port for user query
  char* ascii_port_name = "COM3";
  TCHAR unicode_port_name[MAX_PATH + 1] = {0};
  

  if( argc > 1 ) {
    ascii_port_name = argv[1];
  }
  if (argc > 2) {
    baudrate = atoi(argv[2]);
  }
  if (argc > 3) {
    user_data->server_port = atoi(argv[3]);
  }
  
  if (argc > 4) {
    sprintf(cli_addr, "%s", argv[4]);
  }
  
  if (argc > 5) {
    user_data->client_port = atoi(argv[5]);
  }
  
  user_data->baudrate = baudrate;
  user_data->client_port = cli_port;
  user_data->server_port = svr_port;
  sprintf(user_data->client_addr, "%s", cli_addr);  
  mbstowcs(user_data->port_name, ascii_port_name, MAX_PATH);
}

static
void show_user_buf(const char* buf, int32_t len, int32_t dir)
{
  int32_t i;
  if (dir == 0)
    fprintf(stdout, "--->");
  else
    fprintf(stdout, "<---");
  for (i = 0; i < len; ++i)
    fprintf(stdout, "%02X ", (BYTE)buf[i]);
    
  fprintf(stdout, " :%d\n", len);
}

static   HANDLE h;

void ProcessSecurityData(uint8_t* data, uint8_t len)
{
  WriteRSData(h, data, len);
}

int main(int argc, char * argv[])
{
  struct net_t net;
  struct net_t svr_net;
  uint16_t port = 8235;
  BOOL running = TRUE;
  int len;
  
  int32_t i;
  BYTE buf[BUF_LEN] = {0};
  int32_t count;
  DWORD wstm, rstm;
  struct user_st user_data;
  TCHAR full_port_name[MAX_PATH + 1] = {0};
  
  if (argc < 2) {
    show_user_helper();
    return;
  }
  parse_entry_param(argc, argv, &user_data);
  _ftprintf(stdout, _T("Port name: %s, baudrate:%d"), user_data.port_name, user_data.baudrate);
  fprintf(stdout, "UDP listen port: %d, client address:%s:%d\n", user_data.server_port, user_data.client_addr, user_data.client_port);
 
  h = ConnctComPort(user_data.port_name, user_data.baudrate); 
  if (0 == h) {
    _sntprintf(full_port_name, MAX_PATH, _T("\\\\.\\%s"), user_data.port_name);
    h = ConnctComPort(full_port_name, user_data.baudrate);
  }
  
  if (0 == h) {
    _tprintf(_T("Open %s failed\n"), user_data.port_name);
    return 0;
  }
  count = 0;
  
  InitSocket();
  
  BuildClientSocket(user_data.client_addr, user_data.client_port, FALSE, &net);

  BuildServerSocket(user_data.server_port, FALSE, &svr_net);  
  
  /* use the thread receive the data */
  //StartControlServer(&svr_net);  
  
  fprintf(stdout, "Uart2UDP version beta 0.91\n");

  
  while (running) {
    len = ReadRSData(h, buf, BUF_LEN);
    if (len > 0) {
      net_sendto(&net, buf, len);
      net_sendto(&svr_net, buf, len);
      show_user_buf(buf, len, 0);
      fprintf(stdout, "Forward to UDP %d\n", len);      
    }

    ProcessUDPRawData(&svr_net);
    if (_kbhit()) break;    
  }

  CloseComPort(h);
  
  StopControlServer();
  
  FreeClientSocket(&net);
  FreeClientSocket(&svr_net);
  
  UninitSocket();  

  return 0;
}

void ForwardToUART(const char* buf, int len)
{
  WriteRSData(h, buf, len);
}
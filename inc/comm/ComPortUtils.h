/*
  Copyright (C) 2000-2013
  nizvoo (AT) gmail.com
*/


#ifndef __COM_PORT_UTILS_H__
#define __COM_PORT_UTILS_H__

#include <stdio.h>
#include <stdio.h>


#include <windows.h>
#include <tchar.h>

#ifdef __cplusplus
extern "C" {
#endif

HANDLE ConnctComPort(const TCHAR* name, int baudrate);

BOOL StartCommPortThread(HWND callback, int mesg_id);


BOOL WriteRSData(HANDLE h, const BYTE* data, int len);
int ReadRSData(HANDLE h, BYTE* buf, int len);
void CloseComPort(HANDLE h);

#ifdef __cplusplus
}
#endif

#endif

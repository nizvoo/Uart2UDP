/*
  Copyright (C) 2000-2013
  nizvoo (AT) gmail.com
*/


#ifndef __COM_PORT_UTILS_H__
#define __COM_PORT_UTILS_H__

#include <stdio.h>
#include <stdio.h>


#include <Windows.h>
#include <tchar.h>


HANDLE ConnctComPort(const TCHAR* name, int baudrate);

BOOL StartCommPortThread(HWND callback, int mesg_id);


BOOL WriteRSData(HANDLE h, const BYTE* data, int len);
int ReadRSData(HANDLE h, BYTE* buf, int len);
void CloseComPort(HANDLE h);


#endif
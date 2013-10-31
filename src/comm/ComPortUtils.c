/*
  Copyright (C) 2000-2013
  nizvoo (AT) gmail.com
*/


#include "inc/comm/ComPortUtils.h"


HANDLE ConnctComPort(const TCHAR* name, int dwBaudRate)
{
	DCB dcbConfig;
   long dwTimeOutInSec = 100;
	COMMTIMEOUTS commTimeout;
  
  HANDLE hFile = CreateFile(
    name,
    GENERIC_READ | GENERIC_WRITE,
    0,
    NULL,
    OPEN_EXISTING,
    0,
    NULL);

  if(hFile == INVALID_HANDLE_VALUE) {
		return INVALID_HANDLE_VALUE;
	}

  // Comm port settings:
  if(!SetupComm(hFile, 65534, 65534)) {
		goto CLOSE_ON_FAILED;
  }
  
  // purge any information in the buffer
  PurgeComm( hFile, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR );


  if(GetCommState(hFile, &dcbConfig)) {		// Configuring Serial Port Settings
		dcbConfig.BaudRate  = dwBaudRate;	    // Baud:  9600
		dcbConfig.ByteSize  = 8;			        // Data bits: 8
		dcbConfig.Parity    = NOPARITY;		    // Parity:  none
		dcbConfig.StopBits  = ONESTOPBIT;	    // Stop bits: 1
		dcbConfig.fBinary   = TRUE;
		dcbConfig.fParity   = TRUE;
    dcbConfig.fRtsControl  = RTS_CONTROL_ENABLE;
  } else {

		goto CLOSE_ON_FAILED;
  }

  if(!SetCommState(hFile, &dcbConfig)) {

		goto CLOSE_ON_FAILED;
  }


  if(GetCommTimeouts(hFile, &commTimeout)) { /* Configuring Read & Write Time Outs */
		commTimeout.ReadIntervalTimeout         = dwTimeOutInSec;
		commTimeout.ReadTotalTimeoutConstant    = dwTimeOutInSec;
		commTimeout.ReadTotalTimeoutMultiplier  = 0;
		commTimeout.WriteTotalTimeoutConstant   = dwTimeOutInSec;
		commTimeout.WriteTotalTimeoutMultiplier = 0;
  } else {

		goto CLOSE_ON_FAILED;
  }

  if( !SetCommTimeouts(hFile, &commTimeout)) {

		goto CLOSE_ON_FAILED;
  }

  if(!SetCommMask(hFile, EV_RXCHAR)) { // Setting Event Type

		goto CLOSE_ON_FAILED;
  }

  return hFile;


CLOSE_ON_FAILED:
	if(hFile != INVALID_HANDLE_VALUE) {
		CloseHandle(hFile);
	}
  return INVALID_HANDLE_VALUE;
}

int WriteRSData(HANDLE h, const BYTE* data, int len)
{
	DWORD numWritten;
	WriteFile(h, data, len, &numWritten, NULL); 

	return numWritten;
}


int ReadRSData(HANDLE h, BYTE* buf, int len)
{
	DWORD numRead;

	BOOL ret = ReadFile(h, buf, len, &numRead, NULL);

	if(!ret)
	{
		return 0;
	}

	return numRead;
}

void CloseComPort(HANDLE h)
{
  CloseHandle(h);
}

static HANDLE g_comm_port_handle = INVALID_HANDLE_VALUE;

BOOL StartCommPortThread(HWND callback, int mesg_id)
{
  return TRUE;
}
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
HANDLE hComm;
OVERLAPPED m_ov;
COMSTAT comstat;
DWORD  m_dwCommEvents;
bool openport(char *portname)//打开一个串口
{
	hComm = CreateFile(portname,
		GENERIC_READ | GENERIC_WRITE,
		0,
		0,
		OPEN_EXISTING,
		FILE_FLAG_OVERLAPPED,
		0);

	if (hComm == INVALID_HANDLE_VALUE)
		return FALSE;
	else
		return true;
}


bool setupdcb(int rate_arg)

{
	DCB dcb;
	int rate= rate_arg;
	memset(&dcb,0,sizeof(dcb));
	if(!GetCommState(hComm,&dcb))//获取当前DCB配置
	{
		return FALSE;
	}
	/* -------------------------------------------------------------------- */
	// set DCB to configure the serial port
	dcb.DCBlength      = sizeof(dcb);
	/* ---------- Serial Port Config ------- */
	dcb.BaudRate       = rate;
	dcb.Parity     = NOPARITY;
	dcb.fParity    = 0;
	dcb.StopBits       = ONESTOPBIT;
	dcb.ByteSize       = 8;
	dcb.fOutxCtsFlow   = 0;
	dcb.fOutxDsrFlow   = 0;
	dcb.fDtrControl    = DTR_CONTROL_DISABLE;
	dcb.fDsrSensitivity = 0;
	dcb.fRtsControl    = RTS_CONTROL_DISABLE;
	dcb.fOutX          = 0;
	dcb.fInX           = 0;
	/* ----------------- misc parameters ----- */
	dcb.fErrorChar     = 0;
	dcb.fBinary        = 1;
	dcb.fNull          = 0;
	dcb.fAbortOnError  = 0;
	dcb.wReserved      = 0;
	dcb.XonLim         = 2;
	dcb.XoffLim        = 4;
	dcb.XonChar        = 0x13;
	dcb.XoffChar       = 0x19;
	dcb.EvtChar        = 0;
	/* -------------------------------------------------------------------- */

	// set DCB
	if(!SetCommState(hComm,&dcb))
	{
		return false;
	}
	else
		return true;
}



bool setuptimeout(DWORD ReadInterval,DWORD ReadTotalMultiplier,DWORD ReadTotalconstant,DWORD WriteTotalMultiplier,DWORD WriteTotalconstant)

{

	COMMTIMEOUTS timeouts;
	timeouts.ReadIntervalTimeout=ReadInterval;
	timeouts.ReadTotalTimeoutConstant=ReadTotalconstant;
	timeouts.ReadTotalTimeoutMultiplier=ReadTotalMultiplier;
	timeouts.WriteTotalTimeoutConstant=WriteTotalconstant;
	timeouts.WriteTotalTimeoutMultiplier=WriteTotalMultiplier;
	if(!SetCommTimeouts(hComm, &timeouts))
		return false;
	else
		return true;

}





void ReceiveChar( )

{
	BOOL bRead = TRUE;
	BOOL bResult = TRUE;
	DWORD dwError = 0;
	DWORD BytesRead = 0;
	char RXBuff;
	for (;;)
	{
		bResult = ClearCommError(hComm, &dwError, &comstat);
		if (comstat.cbInQue == 0)
			continue;
		if (bRead)
		{
			bResult = ReadFile(hComm,      // Handle to COMM port
				&RXBuff,         // RX Buffer Pointer
				1,                   // Read one byte
				&BytesRead,          // Stores number of bytes read
				&m_ov);          // pointer to the m_ov structure
		printf("%c",RXBuff);
		if (!bResult)
		{
			switch (dwError = GetLastError())
			{
			case ERROR_IO_PENDING:					
				{
						bRead = FALSE;
						break;
					}
				default:
					{
						break;
				}
				}
			}
			else
			{
				bRead = TRUE;
			}
		} // close if (bRead)
		if (!bRead)
		{
			bRead = TRUE;
			bResult = GetOverlappedResult(hComm,  // Handle to COMM port
				&m_ov,  // Overlapped structure
				&BytesRead,     // Stores number of bytes read
				TRUE);          // Wait flag
		}
	}
}





bool WriteChar(BYTE* m_szWriteBuffer,DWORD m_nToSend)

{
	BOOL bWrite = TRUE;
	BOOL bResult = TRUE;
	DWORD BytesSent = 0;
	//	HANDLE m_hWriteEvent;
	//	ResetEvent(m_hWriteEvent);
	if (bWrite)
	{
		m_ov.Offset = 0;
		m_ov.OffsetHigh = 0;
		// Clear buffer
		bResult = WriteFile(hComm, // Handle to COMM Port
			m_szWriteBuffer, // Pointer to message buffer in calling finction
			m_nToSend,      // Length of message to send
			&BytesSent,     // Where to store the number of bytes sent
			&m_ov );        // Overlapped structure
		if (!bResult)
		{
			DWORD dwError = GetLastError();
			switch (dwError)
			{
			case ERROR_IO_PENDING:
			{
					// continue to GetOverlappedResults()
					BytesSent = 0;
					bWrite = FALSE;
					break;
				}
			default:
				{
					// all other error codes
					break;
				}
			}
		}
	} // end if(bWrite)
	if (!bWrite)
	{
		bWrite = TRUE;
		bResult = GetOverlappedResult(hComm,   // Handle to COMM port
			&m_ov,     // Overlapped structure
			&BytesSent,    // Stores number of bytes sent
			TRUE);         // Wait flag

		// deal with the error code
		if (!bResult)
		{
			printf("GetOverlappedResults() in WriteFile()");
		}
	} // end if (!bWrite)

	// Verify that the data size send equals what we tried to send
	if (BytesSent != m_nToSend)
	{
		printf("WARNING: WriteFile() error.. Bytes Sent: %d; Message Length: %d\n", BytesSent, strlen((char*)m_szWriteBuffer));
	}
	return true;
}

void SerailInit()
{
	if(!openport("com2"))////////////////////////////////////////////////////////////////////////
		MessageBox(NULL, "open comport failure\n", "error", MB_ICONWARNING);
	if(!setupdcb(9600))
		MessageBox(NULL, "setupDCB failure\n", "error", MB_ICONWARNING);
	if(!setuptimeout(0,0,0,0,0))
		MessageBox(NULL, "setuptimeout failure\n", "error", MB_ICONWARNING);
	PurgeComm(hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);
//	WriteChar((BYTE*)"s",1);
}

BYTE  ScanSerail()
{
	static BOOL bRead = TRUE;
	static BOOL bResult = TRUE;
	static DWORD dwError = 0;
	static DWORD BytesRead = 0;
	static char RXBuff;
	bResult = ClearCommError(hComm, &dwError, &comstat);
	if (comstat.cbInQue == 0)
		//continue;
		return 0;
	if (bRead)
	{
		bResult = ReadFile(hComm,      // Handle to COMM port
			&RXBuff,         // RX Buffer Pointer
			1,                   // Read one byte
			&BytesRead,          // Stores number of bytes read
			&m_ov);          // pointer to the m_ov structure
		//printf("%c",RXBuff);
		return RXBuff;
		if (!bResult)
		{
			switch (dwError = GetLastError())
			{
			case ERROR_IO_PENDING:
				{
					bRead = FALSE;
					break;
				}
			default:
				{
					break;
				}
			}
		}
		else
		{
			bRead = TRUE;
		}
	} // close if (bRead)
	if (!bRead)
	{
		bRead = TRUE;
		bResult = GetOverlappedResult(hComm,  // Handle to COMM port
			&m_ov,  // Overlapped structure
			&BytesRead,     // Stores number of bytes read
			TRUE);          // Wait flag
	}
//	system("pause");

}

/************************** program end***************************************/

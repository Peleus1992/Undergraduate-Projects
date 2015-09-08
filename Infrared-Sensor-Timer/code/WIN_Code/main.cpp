

#include <windows.h>
#include <stdio.h>
#include"serailAPI.h" 
#include <conio.h>
#define ID_TIMER    1

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM) ;
char ikey, okey;//ikey: input from 51; okey date(8 bit) to 51
int time = 0;
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = TEXT ("Clock") ;
	HWND         hwnd;
	MSG          msg;
	WNDCLASS     wndclass ;

	//
	




	wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
	wndclass.lpfnWndProc   = WndProc ;
	wndclass.cbClsExtra    = 0 ;
	wndclass.cbWndExtra    = 0 ;
	wndclass.hInstance     = hInstance ;
	wndclass.hIcon         = NULL ;
	wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
	wndclass.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH) ;
	wndclass.lpszMenuName  = NULL ;
	wndclass.lpszClassName = szAppName ;

	if (!RegisterClass (&wndclass))
	{
		MessageBox (NULL, TEXT ("Program requires Windows NT!"), 
			szAppName, MB_ICONERROR) ;
		return 0 ;
	}

	hwnd = CreateWindow (szAppName, TEXT ("MCU Clock"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInstance, NULL) ;

	ShowWindow (hwnd, iCmdShow) ;
	UpdateWindow (hwnd) ;

	while(TRUE){
		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))//检测是否有消息 有的话处理 否则进行else的内容
		{
			if(msg.message == WM_QUIT)
				break;
			TranslateMessage(&msg);  
			DispatchMessage(&msg);
		}
		else
		{
			InvalidateRect(hwnd, NULL, FALSE);                                                                                            //////
			SendMessage(hwnd, WM_PAINT,0, 0);
//			Sleep(3);
		}
	} 
	return msg.wParam ;
}



LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	 HDC hdc ;
	 static HFONT  font;
	 static int cxClient, cyClient;
	 char numBit[4];
	 static HDC hdcBack;
	 static HBITMAP hBack;
	 static char sbuf[6];
	 PAINTSTRUCT ps;
     switch (message)
     {
     case WM_CREATE:
		 hdc = GetDC(hwnd);
  //        SetTimer(hwnd, ID_TIMER, 2, NULL);
		  hdcBack = CreateCompatibleDC(hdc);
		SerailInit();
		 
          return 0 ;
     case  WM_SIZE:
		 cxClient = LOWORD(lParam);
		 cyClient = HIWORD(lParam);
		 font=CreateFont(cyClient - 10,cxClient/5,0,0,0,0,0,0,GB2312_CHARSET,0,0,0,0,TEXT("方正舒体"));
		 
		hBack = CreateCompatibleBitmap(hdcBack, cxClient, cyClient);
		SelectObject(hdcBack, hBack);
		 return 0;
	 case WM_LBUTTONDOWN:
		 okey = 's';
		 return 0;
	 case WM_RBUTTONDOWN:
		okey = 'r';
   		  return 0;

     case WM_PAINT:
		 ikey = ScanSerail();
		 if(ikey == 1)
			 time++;
		 else if(ikey == 2)
			 time = 0;
		 numBit[0] = time / 1000;
		 numBit[1] = (time % 1000) / 100;
		 numBit[2] = (time % 100) / 10;
		 numBit[3] = (time % 10);
		 sprintf(sbuf, "%d%d:%d%d", numBit[0], numBit[1], numBit[2], numBit[3]);
		if(okey == 's')
		{
				 WriteChar((BYTE*)"s", 1);
				 okey = 0;
		}
		else if(okey == 'r')
		{
				 WriteChar((BYTE*)"r", 1);
				 okey = 0;
		}
          hdc = BeginPaint (hwnd, &ps) ;
			    
		  SelectObject(hdcBack, font);
          TextOut(hdcBack, 0, 0, sbuf, strlen(sbuf));
		  BitBlt(hdc,5,20,cxClient, cyClient, hdcBack, 0, 0, SRCCOPY);
          EndPaint (hwnd, &ps) ;
          return 0 ;
          
     case WM_DESTROY:

     //     KillTimer (hwnd, ID_TIMER) ;
          PostQuitMessage (0) ;
          return 0 ;
     }
     return DefWindowProc (hwnd, message, wParam, lParam) ;
}

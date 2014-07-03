/**********************************************************************
 * 
 *  Toby Opferman
 *
 *  Example Application which uses shared memory to display the contents
 *  of a virtual second monitor
 *
 *  This example is for educational purposes only.  I license this source
 *  out for use in learning.
 *
 *  Copyright (c) 2005, All Rights Reserved  
 **********************************************************************/
 

                                                                     
#include <windows.h>
#include <stdio.h>
#include "appmon.h"
#include "virtualmonitor.h"

              


 /***********************************************************************
  * Internal APIs
  ***********************************************************************/
 LRESULT CALLBACK AppMon_WindowProc(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam);
 void AppMon_CreateHandler(HWND hWnd);
 void AppMon_PaintScreen(HWND hWnd);
 void AppMon_PaintScreenForce(HWND hWnd);
  void AppMon_HandleClose(HWND hWnd);
 VOID CALLBACK AppMon_VirtualMonitorRefresh(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);


 /***********************************************************************
  * AppMon_RegisterClass
  *  
  *    Registers the Window Class
  *
  * Parameters
  *     instance Handle
  * 
  * Return Value
  *     TRUE on Success, FALSE on Failure
  *
  ***********************************************************************/
 BOOL WINAPI AppMon_RegisterClass(HINSTANCE hInstance)
 {
     WNDCLASSEX WndClassEx = {0};

     WndClassEx.cbSize        = sizeof(WndClassEx);
     WndClassEx.style         = 0;
     WndClassEx.lpfnWndProc   = AppMon_WindowProc;
     WndClassEx.cbWndExtra    = 0;
     WndClassEx.hInstance     = hInstance;
     WndClassEx.hIcon         = NULL;
     WndClassEx.hCursor       = LoadCursor(hInstance, IDC_ARROW);
     WndClassEx.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
     WndClassEx.lpszMenuName  = NULL;
     WndClassEx.lpszClassName = APPMON_CLASS_NAME;
     WndClassEx.hIconSm       = NULL;
     
     return RegisterClassEx(&WndClassEx);
 }



 /***********************************************************************
  * AppMon_CreateWindow
  *  
  *    Creates The Window
  *
  * Parameters
  *     instance Handle
  * 
  * Return Value
  *     Window Handle on Success, NULL on Failure
  *
  ***********************************************************************/
 HWND WINAPI AppMon_CreateWindow(HINSTANCE hInstance)
 {
     HWND hWnd = NULL;

     if(!FindWindow(APPMON_CLASS_NAME, NULL))
     {
        if(hWnd = CreateWindowEx(0, APPMON_CLASS_NAME, APPMON_TITLE, WS_SYSMENU | WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_OVERLAPPED, 0,  0, APPMON_WINDOW_WIDTH, APPMON_WINDOW_HEIGHT, HWND_DESKTOP, NULL,  hInstance, NULL))
        {
            ShowWindow(hWnd, SW_SHOWNORMAL);
            UpdateWindow(hWnd);
        }
     }

     return hWnd;
 }



 /***********************************************************************
  * AppMon_MessageLoop
  *  
  *    Handles Window Messages to be dispatched to the Window
  *
  * Parameters
  *     instance Handle, Window Handle
  * 
  * Return Value
  *     Exit Value
  *
  ***********************************************************************/
 int WINAPI AppMon_MessageLoop(HINSTANCE hInstance, HWND hWnd)
 {
     MSG Msg = {0};
     BOOL bMessageLoop = TRUE;

     while(bMessageLoop)
     {
         if(GetMessage(&Msg, 0, 0, 0))
         {
             TranslateMessage(&Msg);
             DispatchMessage(&Msg);
        }
        else
        {
            bMessageLoop = FALSE;
        }
     }

     return Msg.wParam;
 }



 /***********************************************************************
  * AppMon_WindowProc
  *  
  *    Handles Window Messages Send to the window
  *
  * Parameters
  *     Standard Windows Proc Handler
  * 
  * Return Value
  *     Exit Value
  *
  ***********************************************************************/
 LRESULT CALLBACK AppMon_WindowProc(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam)
 {
     LRESULT dwRetValue = 0;

     switch(wMsg)
     {
         case WM_CREATE:
              AppMon_CreateHandler(hWnd);
              break;

         case WM_PAINT:
              AppMon_PaintScreen(hWnd);
              break;

         case WM_CLOSE:
              AppMon_HandleClose(hWnd);
              break;

         case WM_DESTROY:
              PostQuitMessage(0);
              break;

         default:
              dwRetValue = DefWindowProc(hWnd, wMsg, wParam, lParam);
     }

     return dwRetValue;
 }




 /***********************************************************************
  * AppMon_HandleClose
  *  
  *    Handle a close message
  *
  * Parameters
  *     Window Handle
  * 
  * Return Value
  *     None
  *
  ***********************************************************************/
 void AppMon_HandleClose(HWND hWnd)
 {
     HVMON hVirtualMonitor = (HVMON)GetWindowLong(hWnd, GWL_USERDATA);
     
     KillTimer(hWnd, MONITOR_REFRESH_TIMER);
     SetWindowLong(hWnd, GWL_USERDATA, 0);

     VirtualMonitor_Free(hVirtualMonitor);

     DestroyWindow(hWnd);
 }


 /***********************************************************************
  * AppMon_PaintScreen
  *  
  *    Handles a repaint of the screen
  *
  * Parameters
  *     Window Handle
  * 
  * Return Value
  *     None
  *
  ***********************************************************************/
 void AppMon_PaintScreen(HWND hWnd)
 {
     HVMON hVirtualMonitor = (HVMON)GetWindowLong(hWnd, GWL_USERDATA);
     HDC hDC, hMonitorDC;
	 POINT sz;
	 int depth;
     PAINTSTRUCT ps;
     RECT Rect;

     hMonitorDC = VirtualMonitor_GetVideoDC(hVirtualMonitor);
     hDC = BeginPaint(hWnd, &ps);

     GetClientRect(hWnd, &Rect);

	 VirtualMonitor_GetInfo(hVirtualMonitor,&sz,&depth);
     StretchBlt(hDC, Rect.left, Rect.top, Rect.right,Rect.bottom, hMonitorDC, 0, 0,sz.x,sz.y, SRCCOPY);

     EndPaint(hWnd, &ps);
     VirtualMonitor_ReleaseVideoDC(hVirtualMonitor, hMonitorDC);
 }
      

 /***********************************************************************
  * AppMon_PaintScreenForce
  *  
  *    Forces a screen re-paint
  *
  * Parameters
  *     Window Handle
  * 
  * Return Value
  *     None
  *
  ***********************************************************************/
 void AppMon_PaintScreenForce(HWND hWnd)
 {
     HVMON hVirtualMonitor = (HVMON)GetWindowLong(hWnd, GWL_USERDATA);
     HDC hDC, hMonitorDC;
	 POINT sz;
	 int depth;
     RECT Rect;

     hMonitorDC = VirtualMonitor_GetVideoDC(hVirtualMonitor);
     hDC = GetDC(hWnd);
     GetClientRect(hWnd, &Rect);

	 VirtualMonitor_GetInfo(hVirtualMonitor,&sz,&depth);

     GetClientRect(hWnd, &Rect);
     StretchBlt(hDC, Rect.left, Rect.top, Rect.right,Rect.bottom, hMonitorDC, 0, 0,sz.x,sz.y, SRCCOPY);
     
     ReleaseDC(hWnd, hDC);
     VirtualMonitor_ReleaseVideoDC(hVirtualMonitor, hMonitorDC);
 }


 /***********************************************************************
 * AppMon_VirtualMonitorRefresh
 *  
 *    Processes a refresh of the Virtual Monitor Display
 *
 * Parameters
 *     Standard Timer Procedure
 * 
 * Return Value
 *     None
 *
 ***********************************************************************/
VOID CALLBACK AppMon_VirtualMonitorRefresh(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
    HVMON hVirtualMonitor = (HVMON)GetWindowLong(hwnd, GWL_USERDATA);

    VirtualMonitor_Update(hVirtualMonitor);

    AppMon_PaintScreenForce(hwnd);

}

 /***********************************************************************
  * AppMon_CreateHandler
  *  
  *    Processes the Initial Creation of the Window
  *
  * Parameters
  *     Window Handle
  * 
  * Return Value
  *     None
  *
  ***********************************************************************/
void AppMon_CreateHandler(HWND hWnd)
{
	HVMON hVirtualMonitor = NULL;
	char title[256];
	POINT sz;
	int depth;

	hVirtualMonitor = VirtualMonitor_Initialize(hWnd);
	VirtualMonitor_GetInfo(hVirtualMonitor,&sz,&depth);

	sprintf(title,"Virtual Monitor Viewer (%d,%d,%d)",sz.x,sz.y,depth);
	SetWindowText(hWnd,title);

   if(hVirtualMonitor == NULL)
   {
       DestroyWindow(hWnd);
   }
   else
   {
       SetWindowLong(hWnd, GWL_USERDATA, (ULONG)hVirtualMonitor);
    
       InvalidateRect(hWnd, NULL, FALSE);
    
       SetTimer(hWnd, MONITOR_REFRESH_TIMER, MONITOR_REFRESH_FREQ, (TIMERPROC)AppMon_VirtualMonitorRefresh);
   }

}


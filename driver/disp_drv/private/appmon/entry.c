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
 #include "appmon.h"
 
 
 
 /*********************************************************************
 * WinMain
 *
 *   Standard Windows Application Entry
 *
 *********************************************************************/
 int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PCHAR pCmdLine, int iShow)
 {
     int iRetValue = 0xFF;
     HWND hWnd;

     if(AppMon_RegisterClass(hInstance))
     {
         if(hWnd = AppMon_CreateWindow(hInstance))
         {
             iRetValue = AppMon_MessageLoop(hInstance, hWnd);
         }
     }
     
     return iRetValue;
 }

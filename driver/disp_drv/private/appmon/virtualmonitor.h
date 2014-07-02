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
 
 
#ifndef __VIRTUALMONITOR_H__
#define __VIRTUALMONITOR_H__

 typedef PVOID HVMON;
 
 HVMON VirtualMonitor_Initialize(HWND hWnd);
 void VirtualMonitor_GetInfo(HVMON hVmon,POINT *sz,int *depth);
 void VirtualMonitor_Update(HVMON hVmon);
 HDC VirtualMonitor_GetVideoDC(HVMON hVmon);
 void VirtualMonitor_ReleaseVideoDC(HVMON hVmon, HDC hVideoDC);
 void VirtualMonitor_Free(HVMON hVmon);



#endif



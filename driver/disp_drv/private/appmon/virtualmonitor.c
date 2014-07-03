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
 #include "gdi.h"
 #include "virtualmonitor.h"
 #include "videomemory.h"
 
 typedef struct _VIRTUALMONITOR
 {
    HWND hWnd;
    HGDI hGDI;
	
	int res_x;
	int res_y;
	int bit_depth;

    PCHAR pVideoMemory;
	PCHAR pSharedVideoMemory;

 } VIRTUALMONITOR, *PVIRTUALMONITOR;
 

 /***********************************************************************
  * VirtualMonitor_Initialize
  *  
  *    Virtual Monitor Initialize
  *
  * Parameters
  *     Window Handle
  * 
  * Return Value
  *     Handle To Virtual Monitor
  *
  ***********************************************************************/
 HVMON VirtualMonitor_Initialize(HWND hWnd)
 {
     PVIRTUALMONITOR pVirtualMonitor = NULL;

     pVirtualMonitor = LocalAlloc(LMEM_ZEROINIT, sizeof(VIRTUALMONITOR));

     if(pVirtualMonitor)
     {
		char *ptr = VideoMemory_GetSharedMemory();

        if(ptr)
        {
	        pVirtualMonitor->pVideoMemory = ptr + (sizeof(int)*3);
			pVirtualMonitor->res_x = *((int*)ptr);
			pVirtualMonitor->res_y = *(((int*)ptr)+1);
			pVirtualMonitor->bit_depth = *(((int*)ptr)+2);
            pVirtualMonitor->hWnd = hWnd;
            pVirtualMonitor->hGDI = GDI_Init(hWnd, pVirtualMonitor->res_x,pVirtualMonitor->res_y);
        }
        else
        {
            LocalFree(pVirtualMonitor);
            pVirtualMonitor = NULL;
        }
     }

     return (HVMON)pVirtualMonitor;
 }

 void VirtualMonitor_GetInfo(HVMON hVmon,POINT *sz,int *depth)
 {
	PVIRTUALMONITOR pvm = (PVIRTUALMONITOR)hVmon;
	sz->x = pvm->res_x;
	sz->y = pvm->res_y;
	*depth = pvm->bit_depth;
 }

 /***********************************************************************
  * VirtualMonitor_Free
  *  
  *    Virtual Monitor Free
  *
  * Parameters
  *     Handle To Virtual Monitor
  * 
  * Return Value
  *     None
  ***********************************************************************/
 void VirtualMonitor_Free(HVMON hVmon)
 {
     PVIRTUALMONITOR pVirtualMonitor = (PVIRTUALMONITOR)hVmon;

     pVirtualMonitor = LocalAlloc(LMEM_ZEROINIT, sizeof(VIRTUALMONITOR));

     if(pVirtualMonitor)
     {
        GDI_UnInit(pVirtualMonitor->hGDI);
        VideoMemory_ReleaseSharedMemory(pVirtualMonitor->pSharedVideoMemory);
        LocalFree(pVirtualMonitor);
     }
 }


 /***********************************************************************
  * VirtualMonitor_Update
  *  
  *    Updates the Screen Image from the shared memory with the
  *    video driver
  *
  * Parameters
  *     Handle To Virtual Monitor
  * 
  * Return Value
  *     None
  *
  ***********************************************************************/
 void VirtualMonitor_Update(HVMON hVmon)
 {
     PVIRTUALMONITOR pVirtualMonitor = (PVIRTUALMONITOR)hVmon;
     PCHAR pVirtualMonitorBuffer;

     pVirtualMonitorBuffer = GDI_BeginPaint(pVirtualMonitor->hGDI);

     memcpy(pVirtualMonitorBuffer, pVirtualMonitor->pVideoMemory, 
		 pVirtualMonitor->res_x*pVirtualMonitor->res_y*(pVirtualMonitor->bit_depth/8));
     
     GDI_EndPaint(pVirtualMonitor->hGDI);
 }


 /***********************************************************************
  * VirtualMonitor_GetVideoDC
  *  
  *    Updates the Screen Image from the shared memory with the
  *    video driver
  *
  * Parameters
  *     Handle To Virtual Monitor
  * 
  * Return Value
  *     Video HDC
  *
  ***********************************************************************/
 HDC VirtualMonitor_GetVideoDC(HVMON hVmon)
 {
    HDC hDC;
    PVIRTUALMONITOR pVirtualMonitor = (PVIRTUALMONITOR)hVmon;

    hDC = GDI_GetDC(pVirtualMonitor->hGDI);

    return hDC;
 }

 /***********************************************************************
  * VirtualMonitor_ReleaseVideoDC
  *  
  *    Releases the Video DC
  *
  * Parameters
  *     Handle To Virtual Monitor, handle to DC
  * 
  * Return Value
  *     None
  *
  ***********************************************************************/
 void VirtualMonitor_ReleaseVideoDC(HVMON hVmon, HDC hVideoDC)
 {
     PVIRTUALMONITOR pVirtualMonitor = (PVIRTUALMONITOR)hVmon;

     GDI_ReleaseDC(pVirtualMonitor->hGDI);
 }

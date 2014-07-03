/**********************************************************************
 * 
 *  Toby Opferman
 *
 *  Shared Video Memory
 *
 *  This example is for educational purposes only.  I license this source
 *  out for use in learning how to write a device driver.
 *
 *  Copyright (c) 2005, All Rights Reserved  
 **********************************************************************/

#include <windows.h>
#include <stdio.h>
#include "videomemory.h"


/*********************************************************************
 * Internal Prototypes
 *********************************************************************/


/*********************************************************************
 * VideoMemory_GetSharedMemory
 *
 *   This function is used to get shared video memory.
 *
 *********************************************************************/
PCHAR VideoMemory_GetSharedMemory(void)
{
   PCHAR pVideoMemory;
   HANDLE hMapFile, hFile; 
   
   hFile = CreateFile("c:\\video.dat", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

   if(hFile && hFile != INVALID_HANDLE_VALUE)
   {
       hMapFile = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, 0, NULL);

       if(hMapFile && hMapFile != INVALID_HANDLE_VALUE)
       {
           pVideoMemory =  MapViewOfFile(hMapFile, FILE_MAP_READ, 0, 0, 0);
    
           CloseHandle(hMapFile);
       }

       CloseHandle(hFile);
   }
   
   return pVideoMemory;
}

/*********************************************************************
 * VideoMemory_ReleaseSharedMemory
 *
 *   This function is used to Release shared video memory.
 *
 *********************************************************************/
void VideoMemory_ReleaseSharedMemory(PCHAR pVideoMemory)
{
    UnmapViewOfFile(pVideoMemory);
}



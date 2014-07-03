/***********************************************************************
 * GDI.C
 *  
 *    API for GDI
 *
 *
 * Toby Opferman Copyright (c) 2003
 *
 ***********************************************************************/
 

                                                                     
#include <windows.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include "gdi.h"


 /***********************************************************************
  * Macros
  ***********************************************************************/
#define COLOR_GOTO_COLOR(x, y, z) if(x > y) { x--; z = FALSE; } else if(x < y) { x++; z = FALSE; } 


 /***********************************************************************
  * Internal Structures
  ***********************************************************************/
  typedef struct {

      HWND hWnd;
      HDC hDoubleBuffer;
      HDC hDoubleBufferWorker;
      BITMAPINFO BitmapInfo;
      HBITMAP hCompatBitmap;
      HBITMAP hOldCompatBitmap;
      HGDIOBJ hOldBitMap;
      HBITMAP hDBBitMap;
      UCHAR *pBuffer;
      DWORD dwScreenX;
      DWORD dwScreenY;
      PALETTEENTRY VirtualPalette[256];
      DWORD dwOptimizedPaletteLookUp[256];
      CRITICAL_SECTION csRepaintBlock;

  } GDI, *PGDI;




 /***********************************************************************
  * GDI_Init
  *  
  *    Initialize Full Screen
  *
  * Parameters
  *     Window  Handle
  * 
  * Return Value
  *     Handle To Cubix
  *
  ***********************************************************************/
HGDI WINAPI GDI_Init(HWND hWnd, UINT dwScreenX, UINT dwScreenY)
{
    PGDI pGDI = NULL;
    HDC hDC;
    int iScreenY = dwScreenY;

    /* Bitmaps Are Upside Down, Need To Negate */
    iScreenY = -iScreenY;

    if(pGDI = (PGDI)LocalAlloc(LMEM_ZEROINIT, sizeof(GDI)))
    {
        
        InitializeCriticalSection(&pGDI->csRepaintBlock);
        

        pGDI->dwScreenX = dwScreenX;
        pGDI->dwScreenY = dwScreenY;

        pGDI->BitmapInfo.bmiHeader.biSize     = sizeof(BITMAPINFOHEADER);
        pGDI->BitmapInfo.bmiHeader.biBitCount = 32;
        pGDI->BitmapInfo.bmiHeader.biWidth    = dwScreenX;
        pGDI->BitmapInfo.bmiHeader.biHeight   = iScreenY;
        pGDI->BitmapInfo.bmiHeader.biPlanes   = 1;
        pGDI->BitmapInfo.bmiHeader.biCompression = BI_RGB;

        hDC = GetDC(hWnd);

        pGDI->hDoubleBuffer = CreateCompatibleDC(hDC);
        pGDI->hDoubleBufferWorker = CreateCompatibleDC(hDC);
        pGDI->hDBBitMap  = CreateDIBSection(pGDI->hDoubleBufferWorker, &pGDI->BitmapInfo, DIB_RGB_COLORS, (VOID **)&pGDI->pBuffer, NULL, 0);
        pGDI->hCompatBitmap = CreateCompatibleBitmap(hDC, dwScreenX, dwScreenY);
        pGDI->hOldCompatBitmap = SelectObject(pGDI->hDoubleBuffer, pGDI->hCompatBitmap);
        pGDI->hOldBitMap = SelectObject(pGDI->hDoubleBufferWorker, pGDI->hDBBitMap);

        ReleaseDC(hWnd, hDC);
        
    }

    return (HGDI)pGDI;
}



 /***********************************************************************
  * GDI_UnInit
  *  
  *    UnInitialize Test Graphics
  *
  * Parameters
  *     Handle to Test
  * 
  * Return Value
  *     Nothing
  *
  ***********************************************************************/
void WINAPI GDI_UnInit(HGDI hGDI)
{
    PGDI pGDI = (PGDI)hGDI;

    if(pGDI)
    {
        LocalFree(pGDI);
    }
}


 /***********************************************************************
  * GDI_GetDC
  *  
  *    Get The Video DC
  *
  * Parameters
  *     Handle to Test
  * 
  * Return Value
  *     Nothing
  *
  ***********************************************************************/
HDC GDI_GetDC(HGDI hGDI)
{
    PGDI pGDI = (PGDI)hGDI;

    EnterCriticalSection(&pGDI->csRepaintBlock);
    return pGDI->hDoubleBuffer;
}


 /***********************************************************************
  * GDI_GetDC
  *  
  *    Get The Video DC
  *
  * Parameters
  *     Handle to Test
  * 
  * Return Value
  *     Nothing
  *
  ***********************************************************************/
void GDI_ReleaseDC(HGDI hGDI)
{
    PGDI pGDI = (PGDI)hGDI;

    LeaveCriticalSection(&pGDI->csRepaintBlock);
}

 /***********************************************************************
  * GDI_BeginPaint
  *  
  *    Get The Video Buffer
  *
  * Parameters
  *     Handle to Test
  * 
  * Return Value
  *     Nothing
  *
  ***********************************************************************/
PUCHAR WINAPI GDI_BeginPaint(HGDI hGDI)
{
   PGDI pGDI = (PGDI)hGDI;

   return pGDI->pBuffer;
}



 /***********************************************************************
  * GDI_EndPaint
  *  
  *    End Painting
  *
  * Parameters
  *     Handle to Test
  * 
  * Return Value
  *     Nothing
  *
  ***********************************************************************/
void WINAPI GDI_EndPaint(HGDI hGDI)
{
    PGDI pGDI = (PGDI)hGDI;

    EnterCriticalSection(&pGDI->csRepaintBlock);
    BitBlt(pGDI->hDoubleBuffer, 0, 0, pGDI->dwScreenX, pGDI->dwScreenY, pGDI->hDoubleBufferWorker, 0, 0, SRCCOPY);
    LeaveCriticalSection(&pGDI->csRepaintBlock);
    
}



/***********************************************************************
 * GDI_BlastBuffer
 *  
 *    Clear Video Buffer
 *
 * Parameters
 *     Video Buffer
 *
 * Return Value
 *     Nothing
 *
 ***********************************************************************/
void WINAPI GDI_BlastBuffer(HGDI hGDI, PUCHAR pBuffer)
{
   PGDI pGDI = (PGDI)hGDI;

   memcpy(pGDI->pBuffer, pBuffer, pGDI->dwScreenX*pGDI->dwScreenY*4);
}



/***********************************************************************
 * GDI_ClearVideoBuffer
 *  
 *    Clear Video Buffer
 *
 * Parameters
 *     Video Buffer
 *
 * Return Value
 *     Nothing
 *
 ***********************************************************************/
void WINAPI GDI_ClearVideoBuffer(HGDI hGDI)
{
   PGDI pGDI = (PGDI)hGDI;

   memset(pGDI->pBuffer, 0, pGDI->dwScreenX*pGDI->dwScreenY*4);
}

/***********************************************************************
 * GDI_SetPalette
 *  
 *    
 *
 * Parameters
 *     Video Buffer
 *
 * Return Value
 *     Nothing
 *
 ***********************************************************************/
void WINAPI GDI_SetPalette(HGDI hGDI, PALETTEENTRY *pPaletteEntries)
{
   PGDI pGDI = (PGDI)hGDI;
   DWORD dwIndex;

   for(dwIndex = 0; dwIndex < 256; dwIndex++)
   {
      pGDI->VirtualPalette[dwIndex].peBlue  = pPaletteEntries[dwIndex].peBlue;
      pGDI->VirtualPalette[dwIndex].peGreen = pPaletteEntries[dwIndex].peGreen;
      pGDI->VirtualPalette[dwIndex].peRed   = pPaletteEntries[dwIndex].peRed;

      pGDI->dwOptimizedPaletteLookUp[dwIndex] = pGDI->VirtualPalette[dwIndex].peBlue | (pGDI->VirtualPalette[dwIndex].peGreen<<8) | (pGDI->VirtualPalette[dwIndex].peRed<<16);
   }
}


/***********************************************************************
 * GDI_FadePalette
 *  
 *    
 *
 * Parameters
 *     Video Buffer
 *
 * Return Value
 *     Nothing
 *
 ***********************************************************************/
BOOL WINAPI GDI_FadePalette(HGDI hGDI)
{
   PGDI pGDI = (PGDI)hGDI;
   BOOL bFaded = TRUE;
   DWORD dwIndex;

   for(dwIndex = 0; dwIndex < 256; dwIndex++)
   {
      COLOR_GOTO_COLOR(pGDI->VirtualPalette[dwIndex].peBlue, 0, bFaded);
      COLOR_GOTO_COLOR(pGDI->VirtualPalette[dwIndex].peGreen, 0, bFaded);
      COLOR_GOTO_COLOR(pGDI->VirtualPalette[dwIndex].peRed, 0, bFaded);

      pGDI->dwOptimizedPaletteLookUp[dwIndex] = pGDI->VirtualPalette[dwIndex].peBlue | (pGDI->VirtualPalette[dwIndex].peGreen<<8) | (pGDI->VirtualPalette[dwIndex].peRed<<16);
   }

   return bFaded;
}


/***********************************************************************
 * GDI_GetVPalEntry
 *  
 *    
 *
 * Parameters
 *     Video Buffer
 *
 * Return Value
 *     Nothing
 *
 ***********************************************************************/
DWORD WINAPI GDI_GetVPalEntry(HGDI hGDI, DWORD dwIndex)
{
   PGDI pGDI = (PGDI)hGDI;

   return pGDI->dwOptimizedPaletteLookUp[dwIndex];
}

/***********************************************************************
 * GDI_SetPixelUsingVPal
 *  
 *    
 *
 * Parameters
 *     Video Buffer
 *
 * Return Value
 *     Nothing
 *
 ***********************************************************************/
void WINAPI GDI_SetPixelUsingVPal(HGDI hGDI, DWORD dwIndexX, DWORD dwIndexY, DWORD dwIndex)
{
   PGDI pGDI = (PGDI)hGDI;

   memcpy(&pGDI->pBuffer[(dwIndexX*4) + (dwIndexY*pGDI->dwScreenX*4)], &pGDI->dwOptimizedPaletteLookUp[dwIndex], 4);

}



/***********************************************************************
 * DirectX_FadeToPalette
 *  
 *    
 *
 * Parameters
 *     Video Buffer
 *
 * Return Value
 *     Nothing
 *
 ***********************************************************************/
BOOL WINAPI GDI_FadeToPalette(HGDI hGDI, PALETTEENTRY *pPaletteEntries)
{
   PGDI pGDI = (PGDI)hGDI;
   BOOL bFaded = TRUE;
   DWORD dwIndex;

   for(dwIndex = 0; dwIndex < 256; dwIndex++)
   {
      COLOR_GOTO_COLOR(pGDI->VirtualPalette[dwIndex].peBlue, pPaletteEntries[dwIndex].peBlue, bFaded);
      COLOR_GOTO_COLOR(pGDI->VirtualPalette[dwIndex].peGreen, pPaletteEntries[dwIndex].peGreen, bFaded);
      COLOR_GOTO_COLOR(pGDI->VirtualPalette[dwIndex].peRed, pPaletteEntries[dwIndex].peRed, bFaded);

      pGDI->dwOptimizedPaletteLookUp[dwIndex] = pGDI->VirtualPalette[dwIndex].peBlue | (pGDI->VirtualPalette[dwIndex].peGreen<<8) | (pGDI->VirtualPalette[dwIndex].peRed<<16);
   }

   return bFaded;
}







/***********************************************************************
 * GDI_Debug
 *  
 *    Debug Shit
 *
 *    
 *
 * Parameters
 *     Debug
 *
 * Return Value
 *     Nothing
 *
 ***********************************************************************/
 void GDI_Debug(char *pszFormatString, ...)
 {
     char DebugString[256];
     va_list vl;

     va_start(vl, pszFormatString);
     vsprintf(DebugString, pszFormatString, vl);
     va_end(vl);

     OutputDebugString(DebugString);
 }






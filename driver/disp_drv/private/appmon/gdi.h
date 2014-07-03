/***********************************************************************
 * GDI.h
 *  
 *    GDI API
 *
 *
 * Toby Opferman Copyright (c) 2003
 *
 ***********************************************************************/


#ifndef __GDI_H__
#define __GDI_H__


typedef PVOID HGDI;



#ifdef __cplusplus
extern "C" {
#endif


/* GDI is Always 32 bit color */
HGDI WINAPI GDI_Init(HWND hWnd, UINT dwScreenX, UINT dwScreenY);
void WINAPI GDI_UnInit(HGDI hGDI);
HDC GDI_GetDC(HGDI hGDI);
PUCHAR WINAPI GDI_BeginPaint(HGDI hGDI);
void WINAPI GDI_EndPaint(HGDI hGDI);
void WINAPI GDI_BlastBuffer(HGDI hGDI, PUCHAR pBuffer);
void WINAPI GDI_ClearVideoBuffer(HGDI hGDI);
BOOL WINAPI GDI_FadeToPalette(HGDI hGDI, PALETTEENTRY *pPaletteEntries);
BOOL WINAPI GDI_FadePalette(HGDI hGDI);
DWORD WINAPI GDI_GetVPalEntry(HGDI hGDI, DWORD dwIndex);
void WINAPI GDI_SetPixelUsingVPal(HGDI hGDI, DWORD dwIndexX, DWORD dwIndexY, DWORD dwIndex);
void GDI_ReleaseDC(HGDI hGDI);
void WINAPI GDI_SetPalette(HGDI hGDI, PALETTEENTRY *pPaletteEntries);

void WINAPI GDI_Debug(char *pszFormatString, ...);

#ifdef __cplusplus
}
#endif



#endif


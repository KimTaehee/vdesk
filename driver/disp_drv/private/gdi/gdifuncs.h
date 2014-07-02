/**********************************************************************
 * 
 *  Toby Opferman
 *
 *  Example GDI DDI (Device Driver Interface) Driver Entry Point
 *
 *  This example is for educational purposes only.  I license this source
 *  out for use in learning how to write a device driver.
 *
 *  Copyright (c) 2005, All Rights Reserved  
 **********************************************************************/

#ifndef __GDIFUNCS_H__
#define __GDIFUNCS_H__

#define FAKE_GFX_TAG (ULONG)'ekaF'
#define DLL_NAME "VX_DDR"

#define MAX_VIDEO_PATH	256

typedef struct _DEVICE_SURFACE_
{
    PVOID pPlaceHolder;

} DEVICE_SURFACE, *PDEVICE_SURFACE;

typedef struct _VIDEO_HEADER_
{
	int res_x;
	int res_y;
	int bit_depth;
}VIDEO_HEADER, *PVIDEO_HEADER;

typedef struct _VIDEO_DATA_
{
	PVIDEO_HEADER info;
	wchar_t path[MAX_VIDEO_PATH];
    PVOID  pVideoMemory;	//display part 시작 포인터
}VIDEO_DATA, *PVIDEO_DATA;

typedef struct _DEVICE_DATA_ 
{
    HDEV   hdev;
    HANDLE hDriver;
    BOOL   bAllocateMappedFile;
    VIDEO_DATA Display;		//Display Information
    PDEVICE_SURFACE pDeviceSurface;
    ULONG_PTR pMappedFile;
    HSURF       hsurf;

} DEVICE_DATA, *PDEVICE_DATA;
              
VOID GdiExample_DrvDisableDriver(VOID);
BOOL GdiExample_DrvAssertMode(DHPDEV  dhpdev,BOOL  bEnable);
void GdiExample_DrvCompletePDEV(DHPDEV  dhpdev, HDEV  hdev);
void GdiExample_DrvDisablePDEV(DHPDEV  dhpdev);
void GdiExample_DrvDisableSurface(DHPDEV  dhpdev);
DHPDEV GdiExample_DrvEnablePDEV(DEVMODEW *pdm, PWSTR pwszLogAddr, ULONG cPat, HSURF *phsurfPatterns, ULONG cjCaps, GDIINFO *pGdiInfo, ULONG cjDevInfo, DEVINFO *pDevInfo, HDEV hdev, PWSTR pwszDeviceName, HANDLE hDriver);
ULONG GdiExample_DrvEscape(SURFOBJ *pso, ULONG iEsc, ULONG  cjIn, PVOID  pvIn, ULONG cjOut, PVOID pvOut);
ULONG GdiExample_DrvGetModes(HANDLE hDriver, ULONG cjSize, DEVMODEW *pdm);
HSURF GdiExample_DrvEnableSurface(DHPDEV  dhpdev);
void GdiExample_DrvMovePointer(SURFOBJ  *pso, LONG  x, LONG  y, RECTL  *prcl);
void GdiExample_DrvNotify(SURFOBJ  *pso, ULONG  iType, PVOID  pvData);      
BOOL GdiExample_DrvRealizeBrush(BRUSHOBJ  *pbo, SURFOBJ  *psoTarget, SURFOBJ  *psoPattern, SURFOBJ  *psoMask, XLATEOBJ  *pxlo, ULONG  iHatch);
BOOL GdiExample_DrvResetPDEV(DHPDEV  dhpdevOld, DHPDEV  dhpdevNew);                                                                           
BOOL GdiExample_DrvSetPalette(DHPDEV  dhpdev, PALOBJ  *ppalo, FLONG  fl, ULONG  iStart, ULONG  cColors);                                      
ULONG GdiExample_DrvSetPointerShape(SURFOBJ  *pso, SURFOBJ  *psoMask, SURFOBJ  *psoColor, XLATEOBJ  *pxlo, LONG  xHot, LONG  yHot, LONG  x, LONG  y, RECTL  *prcl, FLONG  fl);
BOOL GdiExample_DrvStretchBlt(SURFOBJ  *psoDest, SURFOBJ  *psoSrc, SURFOBJ  *psoMask, CLIPOBJ  *pco, XLATEOBJ  *pxlo, COLORADJUSTMENT  *pca, POINTL  *pptlHTOrg, RECTL  *prclDest, RECTL  *prclSrc, POINTL  *pptlMask, ULONG  iMode);            
void GdiExample_DrvSynchronizeSurface(SURFOBJ  *pso, RECTL  *prcl, FLONG  fl);
BOOL GdiExample_DrvAlphaBlend(SURFOBJ  *psoDest, SURFOBJ  *psoSrc, CLIPOBJ  *pco, XLATEOBJ  *pxlo, RECTL  *prclDest, RECTL  *prclSrc, BLENDOBJ  *pBlendObj);
BOOL GdiExample_DrvBitBlt(SURFOBJ  *psoTrg, SURFOBJ  *psoSrc, SURFOBJ  *psoMask, CLIPOBJ  *pco, XLATEOBJ  *pxlo, RECTL  *prclTrg, POINTL  *pptlSrc, POINTL  *pptlMask, BRUSHOBJ  *pbo, POINTL  *pptlBrush, ROP4  rop4);
BOOL GdiExample_DrvCopyBits(SURFOBJ  *psoDest, SURFOBJ  *psoSrc, CLIPOBJ  *pco, XLATEOBJ  *pxlo, RECTL  *prclDest, POINTL  *pptlSrc);              
BOOL GdiExample_DrvFillPath(SURFOBJ  *pso, PATHOBJ  *ppo, CLIPOBJ  *pco, BRUSHOBJ  *pbo, POINTL  *pptlBrushOrg, MIX  mix, FLONG  flOptions);
BOOL GdiExample_DrvGradientFill(SURFOBJ  *psoDest, CLIPOBJ  *pco, XLATEOBJ  *pxlo, TRIVERTEX  *pVertex, ULONG  nVertex, PVOID  pMesh, ULONG  nMesh, RECTL  *prclExtents, POINTL  *pptlDitherOrg, ULONG  ulMode);          
BOOL GdiExample_DrvLineTo(SURFOBJ  *pso, CLIPOBJ  *pco, BRUSHOBJ  *pbo, LONG  x1, LONG  y1, LONG  x2, LONG  y2, RECTL  *prclBounds, MIX  mix);
BOOL GdiExample_DrvStrokePath(SURFOBJ  *pso, PATHOBJ  *ppo, CLIPOBJ  *pco, XFORMOBJ  *pxo, BRUSHOBJ  *pbo, POINTL  *pptlBrushOrg, LINEATTRS  *plineattrs, MIX  mix);            
BOOL GdiExample_DrvTextOut(SURFOBJ  *pso, STROBJ  *pstro, FONTOBJ  *pfo, CLIPOBJ  *pco, RECTL  *prclExtra, RECTL  *prclOpaque, BRUSHOBJ  *pboFore, BRUSHOBJ  *pboOpaque, POINTL  *pptlOrg, MIX  mix);               
BOOL GdiExample_DrvTransparentBlt(SURFOBJ  *psoDst, SURFOBJ  *psoSrc, CLIPOBJ  *pco, XLATEOBJ  *pxlo, RECTL  *prclDst, RECTL  *prclSrc, ULONG  iTransColor, ULONG  ulReserved);
HBITMAP GdiExample_DrvCreateDeviceBitmap(DHPDEV  dhpdev, SIZEL  sizl, ULONG  iFormat); 
void GdiExample_DrvDeleteDeviceBitmap(DHSURF  dhsurf);   
void GdiExample_DrvDestroyFont(FONTOBJ  *pfo);
ULONG_PTR GdiExample_DrvSaveScreenBits(SURFOBJ  *pso, ULONG  iMode, ULONG_PTR  ident, RECTL  *prcl);



/*
 * Rops Defined in the DDK Examples
 */



#define ROP3_NOP            0xAA
#define ROP3_SRCCOPY        (SRCCOPY >> 16)     // 0xCC
#define ROP3_SRCPAINT       (SRCPAINT >> 16)    // 0xEE
#define ROP3_SRCAND         (SRCAND >> 16)      // 0x88
#define ROP3_SRCINVERT      (SRCINVERT >> 16)   // 0x66
#define ROP3_SRCERASE       (SRCERASE >> 16)    // 0x44
#define ROP3_NOTSRCCOPY     (NOTSRCCOPY >> 16)  // 0x33
#define ROP3_NOTSRCERASE    (NOTSRCERASE >> 16) // 0x11
#define ROP3_MERGECOPY      (MERGECOPY >> 16)   // 0xC0
#define ROP3_MERGEPAINT     (MERGEPAINT >> 16)  // 0xBB
#define ROP3_PATCOPY        (PATCOPY >> 16)     // 0xF0
#define ROP3_PATPAINT       (PATPAINT >> 16)    // 0xFB
#define ROP3_PATINVERT      (PATINVERT >> 16)   // 0x5A
#define ROP3_DSTINVERT      (DSTINVERT >> 16)   // 0x55
#define ROP3_BLACKNESS      (BLACKNESS >> 16)   // 0x00
#define ROP3_WHITENESS      (WHITENESS >> 16)   // 0xFF


#define ROP4_SRCCOPY       (ROP3_SRCCOPY | (ROP3_SRCCOPY << 8))       // 0xCCCC
#define ROP4_NOTSRCCOPY    (ROP3_NOTSRCCOPY | (ROP3_NOTSRCCOPY << 8)) // 0x3333
#define ROP4_NOP           (ROP3_NOP | (ROP3_NOP << 8))               // 0xAAAA
#define ROP4_PATCOPY       (ROP3_PATCOPY | (ROP3_PATCOPY << 8))       // 0xFOFO
#define ROP4_BLACKNESS     (ROP3_BLACKNESS | (ROP3_BLACKNESS << 8))   // 0x0000
#define ROP4_WHITENESS     (ROP3_WHITENESS | (ROP3_WHITENESS << 8))   // 0xFFFF

#endif






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

#define __NTDDKCOMP__        
#define _MPPC_
#define _WIN32_WINNT 0x500


#include <stddef.h>
#include <stdarg.h>
#include <limits.h>
#include <windef.h>
#include <wingdi.h>
#include <math.h>
#include <winddi.h>
#include <devioctl.h>

#include <ntddvdeo.h>

#include "gdifuncs.h"
/*
#define RESOLUTION_X      800
#define RESOLUTION_Y      600
#define COLOR_DEPTH_BYTES 4
#define COLOR_DEPTH_BITS  32
*/

#define SYSTM_LOGFONT {16,7,0,0,700,0,0,0,ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,VARIABLE_PITCH | FF_DONTCARE,L"System"}
#define HELVE_LOGFONT {12,9,0,0,400,0,0,0,ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_STROKE_PRECIS,PROOF_QUALITY,VARIABLE_PITCH | FF_DONTCARE,L"MS Sans Serif"}
#define COURI_LOGFONT {12,9,0,0,400,0,0,0,ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_STROKE_PRECIS,PROOF_QUALITY,FIXED_PITCH | FF_DONTCARE, L"Courier"}

const DEVINFO gDevInfoFrameBuffer = {
    ( GCAPS_OPAQUERECT  |
     /* GCAPS_LAYERED     |    If you do not really support this you can have big problems */
      GCAPS_DITHERONREALIZE  |
      GCAPS_ALTERNATEFILL    |
      GCAPS_WINDINGFILL      |
      GCAPS_MONO_DITHER      |
      GCAPS_GRAY16           |
      GCAPS_COLOR_DITHER
                   ), /* Graphics capabilities         */
    SYSTM_LOGFONT,    /* Default font description */
    HELVE_LOGFONT,    /* ANSI variable font description   */
    COURI_LOGFONT,    /* ANSI fixed font description          */
    0,                /* Count of device fonts          */
    0,                /* Preferred DIB format          */
    8,                /* Width of color dither          */
    8,                /* Height of color dither   */
    0,                  /* Default palette to use for this device */
    GCAPS2_SYNCTIMER |
    GCAPS2_SYNCFLUSH
};


//#define ENGDEBUGPRINT
#define ENGDEBUGPRINT _EngDebugPrint

//변화감지를 반영하는 내부 함수
void i_NotifyScreenChanged(SURFOBJ *pso);
wchar_t *i_wcscpy(wchar_t *s1, const wchar_t *s2);
wchar_t *i_wcscat(wchar_t *s1,const wchar_t *s2);


void _EngDebugPrint(int dbg_lv,const char *format, ...)
{
   va_list ap;
   va_start(ap, format);
   EngDebugPrint("", (PCHAR)format, ap);
   va_end(ap);
}

 
/*********************************************************************
 * GdiExample_DrvEnablePDEV
 *
 *   This function will provide a description of the Physical Device.  The data
 *   returned is a user defined data context to be used as a handle for this
 *   display device.
 *
 *   The hDriver is a handle to the miniport driver associated with this display
 *   device.  This handle can be used to communicate to the miniport through APIs
 *   to send things like IOCTLs.
 *
 *********************************************************************/
DHPDEV GdiExample_DrvEnablePDEV(DEVMODEW *pdm, PWSTR pwszLogAddr, ULONG cPat, HSURF *phsurfPatterns, ULONG cjCaps, GDIINFO *pGdiInfo, ULONG cjDevInfo, DEVINFO *pDevInfo, HDEV hdev, PWSTR pwszDeviceName, HANDLE hDriver) 
{
    PDEVICE_DATA pDeviceData = NULL;
    
    ENGDEBUGPRINT(0, "GdiExample_DrvEnablePDEV Enter \r\n", NULL);

    pDeviceData = (PDEVICE_DATA) EngAllocMem(0, sizeof(DEVICE_DATA), FAKE_GFX_TAG);

    if(pDeviceData)
    {
        memset(pDeviceData, 0, sizeof(DEVICE_DATA));
        memset(pGdiInfo, 0, cjCaps);
        memset(pDevInfo, 0, cjDevInfo);
		
		ENGDEBUGPRINT(0, "pdm resolution %d, %d(%d bit) \r\n",pdm->dmPelsWidth,pdm->dmPelsHeight,pdm->dmBitsPerPel);
        {
            pGdiInfo->ulVersion    = 0x5000;
            pGdiInfo->ulTechnology = DT_RASDISPLAY;
            pGdiInfo->ulHorzSize   = 0;
            pGdiInfo->ulVertSize   = 0;
            pGdiInfo->ulHorzRes        = pdm->dmPelsWidth;
            pGdiInfo->ulVertRes        = pdm->dmPelsHeight;
            pGdiInfo->ulPanningHorzRes = 0;
            pGdiInfo->ulPanningVertRes = 0;
            pGdiInfo->cBitsPixel       = pdm->dmBitsPerPel;
            pGdiInfo->cPlanes          = 4;
            pGdiInfo->ulNumColors      = 20;
            pGdiInfo->ulVRefresh       = 1;       
            pGdiInfo->ulBltAlignment   = 1;     
            pGdiInfo->ulLogPixelsX = 96;
            pGdiInfo->ulLogPixelsY = 96;
            pGdiInfo->flTextCaps   = TC_RA_ABLE;
            pGdiInfo->flRaster     = 0;
            pGdiInfo->ulDACRed     = 8;
            pGdiInfo->ulDACGreen   = 8;
            pGdiInfo->ulDACBlue    = 8;
            pGdiInfo->ulAspectX    = 0x24; 
            pGdiInfo->ulNumPalReg  = 256;
            pGdiInfo->ulAspectY    = 0x24;
            pGdiInfo->ulAspectXY   = 0x33;
            pGdiInfo->xStyleStep   = 1;       
            pGdiInfo->yStyleStep   = 1;
            pGdiInfo->denStyleStep = 3;
            pGdiInfo->ptlPhysOffset.x = 0;
            pGdiInfo->ptlPhysOffset.y = 0;
            pGdiInfo->szlPhysSize.cx  = 0;
            pGdiInfo->szlPhysSize.cy  = 0;
            pGdiInfo->ciDevice.Red.x = 6700;
            pGdiInfo->ciDevice.Red.y = 3300;
            pGdiInfo->ciDevice.Red.Y = 0;
            pGdiInfo->ciDevice.Green.x = 2100;
            pGdiInfo->ciDevice.Green.y = 7100;
            pGdiInfo->ciDevice.Green.Y = 0;
            pGdiInfo->ciDevice.Blue.x = 1400;
            pGdiInfo->ciDevice.Blue.y = 800;
            pGdiInfo->ciDevice.Blue.Y = 0;
            pGdiInfo->ciDevice.AlignmentWhite.x = 3127;
            pGdiInfo->ciDevice.AlignmentWhite.y = 3290;
            pGdiInfo->ciDevice.AlignmentWhite.Y = 0;
            pGdiInfo->ciDevice.RedGamma = 20000;
            pGdiInfo->ciDevice.GreenGamma = 20000;
            pGdiInfo->ciDevice.BlueGamma = 20000;
            pGdiInfo->ciDevice.Cyan.x = 1750;
            pGdiInfo->ciDevice.Cyan.y = 3950;
            pGdiInfo->ciDevice.Cyan.Y = 0;
            pGdiInfo->ciDevice.Magenta.x = 4050;
            pGdiInfo->ciDevice.Magenta.y = 2050;
            pGdiInfo->ciDevice.Magenta.Y = 0;
            pGdiInfo->ciDevice.Yellow.x = 4400;
            pGdiInfo->ciDevice.Yellow.y = 5200;
            pGdiInfo->ciDevice.Yellow.Y = 0;
            pGdiInfo->ciDevice.MagentaInCyanDye = 0;
            pGdiInfo->ciDevice.YellowInCyanDye = 0;
            pGdiInfo->ciDevice.CyanInMagentaDye = 0;
            pGdiInfo->ciDevice.YellowInMagentaDye = 0;
            pGdiInfo->ciDevice.CyanInYellowDye = 0;
            pGdiInfo->ciDevice.MagentaInYellowDye = 0;
            pGdiInfo->ulDevicePelsDPI = 0; 
            pGdiInfo->ulPrimaryOrder = PRIMARY_ORDER_CBA;
            pGdiInfo->ulHTPatternSize = HT_PATSIZE_4x4_M;
            pGdiInfo->flHTFlags = HT_FLAG_ADDITIVE_PRIMS;
            pGdiInfo->ulHTOutputFormat = HT_FORMAT_32BPP;

            
            *pDevInfo = gDevInfoFrameBuffer;
            pDevInfo->iDitherFormat = BMF_32BPP;

        }

		// res_x res_y bit_depth data(...)
		{			
			int i;
			wchar_t str[256];
			i_wcscpy(str,L"\\??\\");
			i_wcscpy(pDeviceData->Display.path,L"c:\\tmp\\v");

			for(i=7;i>=0;i--)
			{
				int val = (((int)pDeviceData)>>(i*4))&7;
				wchar_t list[][2] = {L"0",L"1",L"2",L"3",L"4",L"5",L"6",L"7",L"8",L"9",L"A",L"B",L"C",L"D",L"E",L"F"};

				i_wcscat(pDeviceData->Display.path,list[val]);
			}
   
			i_wcscat(str,pDeviceData->Display.path);

			ENGDEBUGPRINT(0,"%ws\r\n", str);
			ENGDEBUGPRINT(0,"%ws\r\n", pDeviceData->Display.path);

			//파일과 매핑되는 메모리를 할당받고
			pDeviceData->Display.pVideoMemory = EngMapFile(str, sizeof(VIDEO_HEADER)+(pdm->dmPelsWidth*pdm->dmPelsHeight*(pdm->dmBitsPerPel/8)), &pDeviceData->pMappedFile);
			//pDeviceData->pMappedFile = pDeviceData->Display.pVideoMemory = EngAllocMem(0,sizeof(VIDEO_HEADER)+(pdm->dmPelsWidth*pdm->dmPelsHeight*(pdm->dmBitsPerPel/8)), FAKE_GFX_TAG);

			if(!pDeviceData->Display.pVideoMemory)
			{
				ENGDEBUGPRINT(0,"GdiExample_DrvEnablePDEV::EngMapFile Failed with error code %d\r\n",EngGetLastError());

				return NULL;
			}

			// Display 정보는 할당받은 공간으로부터 메모리를 이용한다
			pDeviceData->Display.info = (PVIDEO_HEADER)pDeviceData->Display.pVideoMemory;
			pDeviceData->Display.pVideoMemory = ((PBYTE)pDeviceData->Display.pVideoMemory) + sizeof(VIDEO_HEADER);

			//내용을 채움
			pDeviceData->Display.info->res_x = pdm->dmPelsWidth;
			pDeviceData->Display.info->res_y = pdm->dmPelsHeight;
			pDeviceData->Display.info->bit_depth = pdm->dmBitsPerPel;
		}

        pDeviceData->hDriver = hDriver;
        pDevInfo->hpalDefault = EngCreatePalette(PAL_BITFIELDS, 0, NULL, 0xFF0000, 0xFF00, 0xFF);
    }

    ENGDEBUGPRINT(0, "GdiExample_DrvEnablePDEV Exit \r\n", NULL);

    return (DHPDEV)pDeviceData;
}

/*********************************************************************
 * GdiExample_DrvCompletePDEV
 *
 *   This is called to complete the process of enabling the device.
 *   
 *
 *********************************************************************/
void GdiExample_DrvCompletePDEV(DHPDEV  dhpdev, HDEV  hdev)
{
    PDEVICE_DATA pDeviceData = (PDEVICE_DATA)dhpdev;
    ENGDEBUGPRINT(0, "GdiExample_DrvCompletePDEV Enter \r\n", NULL);

    pDeviceData->hdev = hdev;

    ENGDEBUGPRINT(0, "GdiExample_DrvCompletePDEV Exit \r\n", NULL);
}


/*********************************************************************
 * GdiExample_DrvAssertMode
 *
 *    This API sets the display mode of the device to the original mode
 *    which was used during initialization or the default mode of the hardware.
 *
 *********************************************************************/
BOOL GdiExample_DrvAssertMode(DHPDEV  dhpdev,BOOL  bEnable)
{

    ENGDEBUGPRINT(0, "GdiExample_DrvAssertMode\r\n", NULL);

    /*
     * We do nothing here
     */
    return TRUE;
}

/*********************************************************************
 * GdiExample_DrvDisablePDEV
 *
 *   This is called to disable the PDEV we created.
 *   
 *
 *********************************************************************/
void GdiExample_DrvDisablePDEV(DHPDEV  dhpdev)
{
    PDEVICE_DATA pDeviceData = (PDEVICE_DATA)dhpdev;
    UINT dwBytesReturned = 0;

    ENGDEBUGPRINT(0, "GdiExample_DrvDisablePDEV %p\r\n", pDeviceData);

	if(pDeviceData->pMappedFile)
	{
		wchar_t str[256];
		i_wcscpy(str,L"\\??\\");
		i_wcscat(str,pDeviceData->Display.path);

		EngUnmapFile(pDeviceData->pMappedFile);
		EngDeleteFile(str);
		//EngFreeMem(pDeviceData->pMappedFile);
    }

    EngFreeMem(dhpdev);
}


/*********************************************************************
 * GdiExample_DrvEscape
 *
 *    This is the equivlent of an "IOCTL" for a display driver but instead
 *    of an "IOCTL" you have "ESCAPE CODES".  This is how you can communicate
 *    to this display driver in a device DEPENDENT manner instead of INDEPENDENT.
 *
 *    To call into this interface from user mode you use the "ExtEscape" API.
 *
 *		Microsoft reserves the range 0 to 0X10000 for its escape codes. 
 *		Third-party vendors are free to choose escape codes 
 *		for their own use above this range
 *
 *********************************************************************/
#define	ESC_GET_BUFFER	0x10001
#define	ESC_GET_UNIQ	0x10002
#define	ESC_GET_MAPPATH	0x10003

ULONG GdiExample_DrvEscape(SURFOBJ *pso, ULONG iEsc, ULONG  cjIn, PVOID  pvIn, ULONG cjOut, PVOID pvOut)
{
    ENGDEBUGPRINT(0, "GdiExample_DrvEscape\r\n", NULL);

	switch(iEsc)
	{
	case ESC_GET_BUFFER:
		{
			PDEVICE_DATA dd = (PDEVICE_DATA)pso->dhpdev;
			PBYTE *ptr = (PBYTE*)pvOut;

			if(cjOut < sizeof(PBYTE))
				return 0;

			*ptr = (PBYTE)dd->pMappedFile;

			return sizeof(PBYTE);
		}
		break;
	case ESC_GET_UNIQ:
		{
			int *p = (int*)pvOut;
			
			if(cjOut < sizeof(int))
				return 0;

			*p = pso->iUniq;
			return sizeof(int);
		}
		break;
	case ESC_GET_MAPPATH:
		{
			PDEVICE_DATA dd = (PDEVICE_DATA)pso->dhpdev;

			if(cjOut >=MAX_VIDEO_PATH)
			{
				i_wcscpy((wchar_t*)pvOut,dd->Display.path);
				return MAX_VIDEO_PATH;
			}
			else
				return 0;

		}
		break;
	default:
		break;
	}
    return 0;
}



/*********************************************************************
 * GdiExample_DrvGetModes
 *
 *    This API is used to enumerate display modes.
 *
 *    Supperted_Resolution 리스트에 있는 항목들을 선택 가능한 해상도로
 *	  운영체제에 통보해준다.
 *		edited by dekaf
 *
 *	* reference below
 *	http://doxygen.reactos.org/db/dc4/win32ss_2drivers_2displays_2framebufacc_2screen_8c_aeea37169dcacc164939934406882e762.html
 *
 *********************************************************************/
ULONG GdiExample_DrvGetModes(HANDLE hDriver, ULONG cjSize, DEVMODEW *pdm)
{
   ULONG ulBytesWritten = 0, ulBytesNeeded = sizeof(DEVMODEW);
   ULONG ulReturnValue=0;

	int Supported_Resolution[][2] = {
		{1716,2288},
		{1536,2048},
		{1440,1920},
		{1200,1920},
		{1080,1920},
		{1050,1680},
		{1200,1600},
		{900,1440},
		{1050,1400},
		{768,1366},
		{1024,1280},
		{960,1280},
		{800,1280},
		{768,1280},
		{720,1280},
		{8641152},
		{768,1024},
		{600,800},

		{2288,1716},
		{2048,1536},
		{1920,1440},
		{1920,1200},
		{1920,1080},
		{1680,1050},
		{1600,1200},
		{1440,900},
		{1400,1050},
		{1366,768},
		{1280,1024},
		{1280,960},
		{1280,800},
		{1280,768},
		{1280,720},
		{1152,864},
		{1024,768},
		{800,600}

	};
	int ModeCount = sizeof(Supported_Resolution)/(sizeof(int)*2);

   ENGDEBUGPRINT(0, "GdiExample_DrvGetModes (%s)\r\n",__TIMESTAMP__);

   if(pdm == NULL)
   {
	   //몇바이트 필요한지 운영체제에게 알려준다
       ulReturnValue =  ulBytesNeeded * ModeCount;
   }
   else 
   {
		ulBytesWritten = sizeof(DEVMODEW);

		while(ModeCount-- > 0)
		{
			memset(pdm, 0, sizeof(DEVMODEW));
			memcpy(pdm->dmDeviceName, DLL_NAME, sizeof(DLL_NAME));

			pdm->dmSpecVersion   = DM_SPECVERSION;
			pdm->dmDriverVersion = DM_SPECVERSION;

			pdm->dmDriverExtra      = 0;
			pdm->dmSize             = sizeof(DEVMODEW);
			pdm->dmBitsPerPel       = 32;
			pdm->dmPelsWidth        = Supported_Resolution[ModeCount][0];
			pdm->dmPelsHeight       = Supported_Resolution[ModeCount][1];
			pdm->dmDisplayFrequency = 60;

			pdm->dmDisplayFlags     = 0;
       
			pdm->dmPanningWidth     = pdm->dmPelsWidth;
			pdm->dmPanningHeight    = pdm->dmPelsHeight;

			pdm->dmFields           = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFLAGS | DM_DISPLAYFREQUENCY;

			pdm = (LPDEVMODEW)(((ULONG_PTR)pdm) + sizeof(DEVMODEW));
			ulReturnValue += ulBytesWritten;

		}
   }
	return ulReturnValue;
}


/*********************************************************************
 * GdiExample_DrvResetPDEV
 *
 *    This API is used to pass state information from one PDEV
 *    instance to another.
 *
 *********************************************************************/
BOOL GdiExample_DrvResetPDEV(DHPDEV  dhpdevOld, DHPDEV  dhpdevNew)
{
    ENGDEBUGPRINT(0, "GdiExample_DrvResetPDEV\r\n", NULL);

    return TRUE;
}




/*********************************************************************
 * GdiExample_DrvEnableSurface
 *
 *  This API is used to enable the physical device surface.  
 *
 *  You have two choices here.
 *     
 *     1. Driver Manages it's own surface
 *          EngCreateDeviceSurface - Create the handle
 *          EngModifySurface - Let GDI Know about the object.
 *
 *     2. GDI Manages the surface
 *          EngCreateBitmap - Create a handle in a format that GDI Understands
 *          EngAssociateSurface - Let GDI Know about the object.
 *
 *
 *********************************************************************/
HSURF GdiExample_DrvEnableSurface(DHPDEV  dhpdev)
{
    SIZEL       sizl;
    PDEVICE_DATA pDeviceData = (PDEVICE_DATA)dhpdev;
    
    ENGDEBUGPRINT(0, "GdiExample_DrvEnableSurface\r\n", NULL);

    pDeviceData->pDeviceSurface = (PDEVICE_SURFACE)EngAllocMem(FL_ZERO_MEMORY, sizeof(DEVICE_SURFACE), FAKE_GFX_TAG);

    sizl.cx = pDeviceData->Display.info->res_x;
    sizl.cy = pDeviceData->Display.info->res_y;

    pDeviceData->hsurf = (HSURF)EngCreateDeviceSurface((DHSURF)pDeviceData->pDeviceSurface, sizl, BMF_32BPP);

	if(pDeviceData->hsurf)
	{
	    ENGDEBUGPRINT(0, "GdiExample_DrvEnableSurface::EngCreateDeviceSurface failed!(Error Code : %ld)\r\n", EngGetLastError());
	}

	if(!EngModifySurface(pDeviceData->hsurf, pDeviceData->hdev, HOOK_FILLPATH | HOOK_STROKEPATH | HOOK_LINETO | HOOK_TEXTOUT | HOOK_BITBLT | HOOK_COPYBITS, 0, (DHSURF)pDeviceData->pDeviceSurface, pDeviceData->Display.pVideoMemory, pDeviceData->Display.info->res_x*(pDeviceData->Display.info->bit_depth/8), NULL))
	{
	    ENGDEBUGPRINT(0, "GdiExample_DrvEnableSurface::EngModifySurface failed!\r\n", NULL);
	}
    
    return(pDeviceData->hsurf);

}


/*********************************************************************
 * GdiExample_DrvDisableSurface
 *
 *  This API is called to disable the GDI Surface.
 *
 *
 *********************************************************************/

void GdiExample_DrvDisableSurface(DHPDEV  dhpdev)
{
    PDEVICE_DATA pDeviceData = (PDEVICE_DATA)dhpdev;

    ENGDEBUGPRINT(0, "GdiExample_DrvDisableSurface\r\n", NULL);

    EngDeleteSurface(pDeviceData->hsurf);
    pDeviceData->hsurf = NULL;

    EngFreeMem(pDeviceData->pDeviceSurface);
    pDeviceData->pDeviceSurface = NULL;
}



/*********************************************************************
 * GdiExample_DrvMovePointer
 *
 *    This API is used to move the Mouse Pointer
 *
 *********************************************************************/
void GdiExample_DrvMovePointer(SURFOBJ  *pso, LONG  x, LONG  y, RECTL  *prcl)
{
    ENGDEBUGPRINT(0, "GdiExample_DrvMovePointer\r\n", NULL);

    if(pso)
    {
       EngMovePointer(pso, x, y, prcl);
    }

}

/*********************************************************************
 * GdiExample_DrvSetPointerShape
 *
 *    This API is used to set the Mouse Pointer shape
 *
 *********************************************************************/
ULONG GdiExample_DrvSetPointerShape(SURFOBJ  *pso, SURFOBJ  *psoMask, SURFOBJ  *psoColor, XLATEOBJ  *pxlo, LONG  xHot, LONG  yHot, LONG  x, LONG  y, RECTL  *prcl, FLONG  fl)
{
    ENGDEBUGPRINT(0, "GdiExample_DrvSetPointerShape\r\n", NULL);

    return SPS_ACCEPT_NOEXCLUDE;
}


/*********************************************************************
 * GdiExample_DrvNotify
 *
 *    This API is optional. GDI will call it to "notify" the driver of certain operations.
 *
 *********************************************************************/
void GdiExample_DrvNotify(SURFOBJ  *pso, ULONG  iType, PVOID  pvData)
{
    ENGDEBUGPRINT(0, "GdiExample_DrvNotify\r\n", NULL);
     /* Do Nothing */
}

/*********************************************************************
 * GdiExample_DrvRealizeBrush
 *
 *    Realizes a brush
 *
 *********************************************************************/
BOOL GdiExample_DrvRealizeBrush(BRUSHOBJ  *pbo, SURFOBJ  *psoTarget, SURFOBJ  *psoPattern, SURFOBJ  *psoMask, XLATEOBJ  *pxlo, ULONG  iHatch)
{
    ENGDEBUGPRINT(0, "GdiExample_DrvRealizeBrush\r\n", NULL);
    
    return TRUE;
}
          
/*********************************************************************
 * GdiExample_DrvSetPalette
 *
 *    Sets the palette for palette modes.  We don't support this.
 *
 *********************************************************************/
BOOL GdiExample_DrvSetPalette(DHPDEV  dhpdev, PALOBJ  *ppalo, FLONG  fl, ULONG  iStart, ULONG  cColors)
{
    ENGDEBUGPRINT(0, "GdiExample_DrvSetPalette\r\n", NULL);
    return FALSE;
}
            

/*********************************************************************
 * GdiExample_DrvStretchBlt
 *
 *   Performs a stretch Blt
 *
 *********************************************************************/       
BOOL GdiExample_DrvStretchBlt(SURFOBJ  *psoDest, SURFOBJ  *psoSrc, SURFOBJ  *psoMask, CLIPOBJ  *pco, XLATEOBJ  *pxlo, COLORADJUSTMENT  *pca, POINTL  *pptlHTOrg, RECTL  *prclDest, RECTL  *prclSrc, POINTL  *pptlMask, ULONG  iMode)
{
    ENGDEBUGPRINT(0, "GdiExample_DrvStretchBlt\r\n", NULL);
//	i_NotifyScreenChanged(psoDest);
    return EngStretchBlt(psoDest, psoSrc, psoMask, pco, pxlo, pca, pptlHTOrg, prclDest, prclSrc, pptlMask, iMode);
}



/*********************************************************************
 * GdiExample_DrvSynchronizeSurface
 *
 *   Helps synchronize GDI writes with the co-processor.  This is optional.
 *
 *********************************************************************/ 
void GdiExample_DrvSynchronizeSurface(SURFOBJ  *pso, RECTL  *prcl, FLONG  fl)
{
    ENGDEBUGPRINT(0, "GdiExample_DrvSynchronizeSurface\r\n", NULL);
    /* Do Nothing */
}
    

/*********************************************************************
 * GdiExample_DrvAlphaBlend
 *
 *   Performs alpha blending.
 *
 *********************************************************************/
BOOL GdiExample_DrvAlphaBlend(SURFOBJ  *psoDest, SURFOBJ  *psoSrc, CLIPOBJ  *pco, XLATEOBJ  *pxlo, RECTL  *prclDest, RECTL  *prclSrc, BLENDOBJ  *pBlendObj)
{
    ENGDEBUGPRINT(0, "GdiExample_DrvAlphaBlend\r\n", NULL);
//	i_NotifyScreenChanged(psoDest);
    return EngAlphaBlend(psoDest, psoSrc, pco, pxlo, prclDest, prclSrc, pBlendObj);
}


/*********************************************************************
 * GdiExample_DrvSaveScreenBits
 *
 *    This API is used to save parts of the screen when they are being overlapped.
 *    We return 0 to let GDI handle this
 *
 *********************************************************************/
ULONG_PTR GdiExample_DrvSaveScreenBits(SURFOBJ  *pso, ULONG  iMode, ULONG_PTR  ident, RECTL  *prcl)
{
    ENGDEBUGPRINT(0, "GdiExample_DrvSaveScreenBits\r\n", NULL);

    return 0;
}

/*********************************************************************
 * GdiExample_DrvBitBlt
 *
 *    This API is used to perform a BitBlt operation
 *
 *********************************************************************/
BOOL GdiExample_DrvBitBlt(SURFOBJ  *psoTrg, SURFOBJ  *psoSrc, SURFOBJ  *psoMask, CLIPOBJ  *pco, XLATEOBJ  *pxlo, RECTL  *prclTrg, POINTL  *pptlSrc, POINTL  *pptlMask, BRUSHOBJ  *pbo, POINTL  *pptlBrush, ROP4  rop4)
{
    ENGDEBUGPRINT(0, "GdiExample_DrvBitBlt\r\n", NULL);
	i_NotifyScreenChanged(psoTrg);

    return EngBitBlt(psoTrg, psoSrc, psoMask, pco, pxlo, prclTrg, pptlSrc, pptlMask, pbo, pptlBrush, rop4);
}
                

/*********************************************************************
 * GdiExample_DrvCopyBits
 *
 *    This API copies between different formats.
 *
 *********************************************************************/
BOOL GdiExample_DrvCopyBits(SURFOBJ  *psoDest, SURFOBJ  *psoSrc, CLIPOBJ  *pco, XLATEOBJ  *pxlo, RECTL  *prclDest, POINTL  *pptlSrc)
{
    ENGDEBUGPRINT(0, "GdiExample_DrvCopyBits\r\n", NULL);
//	i_NotifyScreenChanged(psoDest);

   // return EngCopyBits(psoDest, psoSrc, pco, pxlo, prclDest, pptlSrc);
    return GdiExample_DrvBitBlt(psoDest, psoSrc, NULL, pco, pxlo, prclDest, pptlSrc, NULL, NULL, NULL, ROP4_SRCCOPY);
}

/*********************************************************************
 * GdiExample_DrvFillPath
 *
 *    This API fills the path.
 *
 *********************************************************************/

BOOL GdiExample_DrvFillPath(SURFOBJ  *pso, PATHOBJ  *ppo, CLIPOBJ  *pco, BRUSHOBJ  *pbo, POINTL  *pptlBrushOrg, MIX  mix, FLONG  flOptions)
{
    ENGDEBUGPRINT(0, "GdiExample_DrvFillPath\r\n", NULL);
//	i_NotifyScreenChanged(pso);

    return EngFillPath(pso, ppo,pco, pbo, pptlBrushOrg, mix, flOptions);
}
              
/*********************************************************************
 * GdiExample_DrvGradientFill
 *
 *    This API does a gradient fill.
 *
 *********************************************************************/
BOOL GdiExample_DrvGradientFill(SURFOBJ  *psoDest, CLIPOBJ  *pco, XLATEOBJ  *pxlo, TRIVERTEX  *pVertex, ULONG  nVertex, PVOID  pMesh, ULONG  nMesh, RECTL  *prclExtents, POINTL  *pptlDitherOrg, ULONG  ulMode)          
{
    ENGDEBUGPRINT(0, "GdiExample_DrvGradientFill\r\n", NULL);
//	i_NotifyScreenChanged(psoDest);
    return EngGradientFill(psoDest, pco, pxlo, pVertex, nVertex, pMesh, nMesh, prclExtents, pptlDitherOrg, ulMode);
}


/*********************************************************************
 * GdiExample_DrvLineTo
 *
 *    This API implements a LINE TO operation.
 *
 *********************************************************************/
BOOL GdiExample_DrvLineTo(SURFOBJ  *pso, CLIPOBJ  *pco, BRUSHOBJ  *pbo, LONG  x1, LONG  y1, LONG  x2, LONG  y2, RECTL  *prclBounds, MIX  mix)
{
    ENGDEBUGPRINT(0, "GdiExample_DrvLineTo\r\n", NULL);
//	i_NotifyScreenChanged(pso);
    return EngLineTo(pso, pco, pbo, x1, y1, x2, y2, prclBounds, mix);
}

/*********************************************************************
 * GdiExample_DrvStrokePath
 *
 *    This API implements a stroke path operation.
 *
 *********************************************************************/
BOOL GdiExample_DrvStrokePath(SURFOBJ  *pso, PATHOBJ  *ppo, CLIPOBJ  *pco, XFORMOBJ  *pxo, BRUSHOBJ  *pbo, POINTL  *pptlBrushOrg, LINEATTRS  *plineattrs, MIX  mix)            
{
    ENGDEBUGPRINT(0, "GdiExample_DrvStrokePath\r\n", NULL);
//	i_NotifyScreenChanged(pso);

    return EngStrokePath(pso, ppo, pco, pxo, pbo, pptlBrushOrg, plineattrs, mix);
}

/*********************************************************************
 * GdiExample_DrvTextOut
 *
 *    This API writes text on to a surface.
 *
 *********************************************************************/
BOOL GdiExample_DrvTextOut(SURFOBJ  *pso, STROBJ  *pstro, FONTOBJ  *pfo, CLIPOBJ  *pco, RECTL  *prclExtra, RECTL  *prclOpaque, BRUSHOBJ  *pboFore, BRUSHOBJ  *pboOpaque, POINTL  *pptlOrg, MIX  mix)
{
    ENGDEBUGPRINT(0, "GdiExample_DrvTextOut\r\n", NULL);
//	i_NotifyScreenChanged(pso);
    return EngTextOut(pso, pstro, pfo, pco, prclExtra, prclOpaque, pboFore, pboOpaque, pptlOrg, mix);
}
               

/*********************************************************************
 * GdiExample_DrvTransparentBlt
 *
 *    This API performs a TransparentBlt.
 *
 *********************************************************************/
BOOL GdiExample_DrvTransparentBlt(SURFOBJ  *psoDst, SURFOBJ  *psoSrc, CLIPOBJ  *pco, XLATEOBJ  *pxlo, RECTL  *prclDst, RECTL  *prclSrc, ULONG  iTransColor, ULONG  ulReserved)
{
    ENGDEBUGPRINT(0, "GdiExample_DrvTransparentBlt\r\n", NULL);
//	i_NotifyScreenChanged(psoDst);
    return EngTransparentBlt(psoDst, psoSrc, pco, pxlo, prclDst, prclSrc, iTransColor, ulReserved);
}


/*********************************************************************
 * GdiExample_DrvCreateDeviceBitmap
 *
 *    This API creates and manages bitmaps.  We return 0 to let GDI handle this.
 *
 *********************************************************************/

HBITMAP GdiExample_DrvCreateDeviceBitmap(DHPDEV  dhpdev, SIZEL  sizl, ULONG  iFormat) 
{
    ENGDEBUGPRINT(0, "GdiExample_DrvCreateDeviceBitmap\r\n", NULL);

    return NULL;
}

/*********************************************************************
 * GdiExample_DrvDeleteDeviceBitmap
 *
 *    This API deletes managed bitmaps.  We are not implementing this.
 *
 *********************************************************************/
void GdiExample_DrvDeleteDeviceBitmap(DHSURF  dhsurf)   
{
    ENGDEBUGPRINT(0, "GdiExample_DrvDeleteDeviceBitmap\r\n", NULL);

}


/*********************************************************************
 * GdiExample_DrvDestroyFont
 *
 *    This API is called to delete a FONT
 *
 *********************************************************************/
void GdiExample_DrvDestroyFont(FONTOBJ  *pfo)
{		
    ENGDEBUGPRINT(0, "GdiExample_DrvDestroyFont\r\n", NULL);
}

// 변화가 감지되었음을 알린다.
void i_NotifyScreenChanged(SURFOBJ *pso)
{
	PDEVICE_DATA dd = (PDEVICE_DATA)pso->dhpdev;
//	return;
//	dd->Display.info->chgToken++;
	ENGDEBUGPRINT(0,"chgToken Changed %d\r\n",pso->iUniq);
}


wchar_t *i_wcscpy(wchar_t *s1, const wchar_t *s2)
{
	wchar_t *cp;

	cp = s1;
	while ((*cp++ = *s2++) != L'\0')
		;

	return (s1);
}

wchar_t *i_wcscat(wchar_t *s1,const wchar_t *s2)
{
	wchar_t *cp;

	cp = s1;
	while (*cp != L'\0')
		cp++;
	while ((*cp++ = *s2++) != L'\0')
		;

	return (s1);
}

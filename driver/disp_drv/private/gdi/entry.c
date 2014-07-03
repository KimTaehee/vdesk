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


#include "gdifuncs.h"

/*
 * Display Drivers provide a list of function entry points for specific GDI
 * tasks.  These are identified by providing a pre-defined "INDEX" value (pre-defined
 * by microsoft) followed by the function entry point.  There are levels of flexibility
 * on which ones you are REQUIRED and which ones are technically OPTIONAL.
 *
 */
                                            
DRVFN g_DrvFunctions[] =
{
    {   INDEX_DrvAssertMode,            (PFN) GdiExample_DrvAssertMode            },
    {   INDEX_DrvCompletePDEV,          (PFN) GdiExample_DrvCompletePDEV          },
    {   INDEX_DrvCreateDeviceBitmap,    (PFN) GdiExample_DrvCreateDeviceBitmap    },
    {   INDEX_DrvDeleteDeviceBitmap,    (PFN) GdiExample_DrvDeleteDeviceBitmap    },
    {   INDEX_DrvDestroyFont,           (PFN) GdiExample_DrvDestroyFont           },
    {   INDEX_DrvDisablePDEV,           (PFN) GdiExample_DrvDisablePDEV           },
    {   INDEX_DrvDisableDriver,         (PFN) GdiExample_DrvDisableDriver         },
    {   INDEX_DrvDisableSurface,        (PFN) GdiExample_DrvDisableSurface        },
    {   INDEX_DrvSaveScreenBits,        (PFN) GdiExample_DrvSaveScreenBits        },
    {   INDEX_DrvEnablePDEV,            (PFN) GdiExample_DrvEnablePDEV            },
    {   INDEX_DrvEnableSurface,         (PFN) GdiExample_DrvEnableSurface         },
    {   INDEX_DrvEscape,                (PFN) GdiExample_DrvEscape                },
    {   INDEX_DrvGetModes,              (PFN) GdiExample_DrvGetModes              },
    {   INDEX_DrvMovePointer,           (PFN) GdiExample_DrvMovePointer           },
    {   INDEX_DrvNotify,                (PFN) GdiExample_DrvNotify                },
  //  {   INDEX_DrvRealizeBrush,          (PFN) GdiExample_DrvRealizeBrush          },
    {   INDEX_DrvResetPDEV,             (PFN) GdiExample_DrvResetPDEV             },
    {   INDEX_DrvSetPalette,            (PFN) GdiExample_DrvSetPalette            },
    {   INDEX_DrvSetPointerShape,       (PFN) GdiExample_DrvSetPointerShape       },
    {   INDEX_DrvStretchBlt,            (PFN) GdiExample_DrvStretchBlt            },
    {   INDEX_DrvSynchronizeSurface,    (PFN) GdiExample_DrvSynchronizeSurface    },
    {   INDEX_DrvAlphaBlend,            (PFN) GdiExample_DrvAlphaBlend            },
    {   INDEX_DrvBitBlt,                (PFN) GdiExample_DrvBitBlt                },
    {   INDEX_DrvCopyBits,              (PFN) GdiExample_DrvCopyBits              },
    {   INDEX_DrvFillPath,              (PFN) GdiExample_DrvFillPath              },
    {   INDEX_DrvGradientFill,          (PFN) GdiExample_DrvGradientFill          },
    {   INDEX_DrvLineTo,                (PFN) GdiExample_DrvLineTo                },
    {   INDEX_DrvStrokePath,            (PFN) GdiExample_DrvStrokePath            },
    {   INDEX_DrvTextOut,               (PFN) GdiExample_DrvTextOut               },
    {   INDEX_DrvTransparentBlt,        (PFN) GdiExample_DrvTransparentBlt        },
};
                                            
ULONG g_ulNumberOfFunctions = sizeof(g_DrvFunctions) / sizeof(DRVFN);


/*********************************************************************
 * DrvEnableDriver
 *
 *   This is the initial driver entry point.  This is the "DriverEntry" equivlent
 *   for Display and Printer drivers.  This function must return a function table
 *   that represents all the supported entry points into this driver.
 *
 *********************************************************************/
BOOL DrvEnableDriver(ULONG ulEngineVersion, ULONG ulDataSize, DRVENABLEDATA *pDrvEnableData)
{
    BOOL bDriverEnabled = FALSE;

    /*
     * We only want to support versions > NT 4
     *
     */
    if(HIWORD(ulEngineVersion) >= 0x3 && ulDataSize >= sizeof(DRVENABLEDATA))
    {
       pDrvEnableData->iDriverVersion = DDI_DRIVER_VERSION_NT5_01;
       pDrvEnableData->pdrvfn         = g_DrvFunctions;
       pDrvEnableData->c              = g_ulNumberOfFunctions;
       bDriverEnabled                 = TRUE;
    }

    return bDriverEnabled;              
}



/*********************************************************************
 * GdiExample_DrvDisableDriver
 *
 *   This function is used to notify the driver when the driver is
 *   getting ready to be unloaded.
 *
 *********************************************************************/
VOID GdiExample_DrvDisableDriver(VOID)
{
    /*
     *  No Clean up To Do
     */
}

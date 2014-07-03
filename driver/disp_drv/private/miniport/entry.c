/**********************************************************************
 * 
 *  Toby Opferman
 *
 *  Example Video Miniport Driver Entry Point
 *
 *  This example is for educational purposes only.  I license this source
 *  out for use in learning how to write a device driver.
 *
 *  Copyright (c) 2005, All Rights Reserved  
 **********************************************************************/
 
#define _X86_

#include <dderror.h>
#include <devioctl.h>
#include <miniport.h>
#include <ntddvdeo.h>
#include <video.h>

#include "fakegfxcard.h"
      
ULONG DriverEntry(PVOID pContext1, PVOID pContext2);

#pragma alloc_text(INIT, DriverEntry)


/**********************************************************************
 * 
 *  DriverEntry
 *
 *    This is the entry point for this video miniport driver
 *
 **********************************************************************/
ULONG DriverEntry(PVOID pContext1, PVOID pContext2)
{
    VIDEO_HW_INITIALIZATION_DATA hwInitData;
    VP_STATUS vpStatus;

    /*
     * The Video Miniport is "technically" restricted to calling "Video*" APIs.  There is
     * a driver that encapsulates this driver by setting your driver's entry points to
     * locations in itself.  It will then handle your IRP's for you and determine which
     * of the entry points (provided below) into your driver that should be called.
     *
     * This driver however does run in the context of system memory unlike the GDI component.
     *
     */

    VideoPortZeroMemory(&hwInitData, sizeof(VIDEO_HW_INITIALIZATION_DATA));

    hwInitData.HwInitDataSize            = sizeof(VIDEO_HW_INITIALIZATION_DATA);

    hwInitData.HwFindAdapter             = FakeGfxCard_FindAdapter;
    hwInitData.HwInitialize              = FakeGfxCard_Initialize;
    hwInitData.HwStartIO                 = FakeGfxCard_StartIO;
    hwInitData.HwResetHw                 = FakeGfxCard_ResetHW;
    hwInitData.HwInterrupt               = FakeGfxCard_VidInterrupt;
    hwInitData.HwGetPowerState           = FakeGfxCard_GetPowerState;
    hwInitData.HwSetPowerState           = FakeGfxCard_SetPowerState;
    hwInitData.HwGetVideoChildDescriptor = FakeGfxCard_GetChildDescriptor;

	// ioctl 장치 등록 관련
	//DriverObject->DriverExtension->AddDevice           = FilterAddDevice;

    vpStatus = VideoPortInitialize(pContext1, pContext2, &hwInitData, NULL);

    return vpStatus;
}



/**********************************************************************
 * 
 *  Toby Opferman
 *
 *  Example Video Miniport Driver
 *
 *  This example is for educational purposes only.  I license this source
 *  out for use in learning how to write a device driver.
 *
 *  Copyright (c) 2005, All Rights Reserved  
 **********************************************************************/


#include <dderror.h>
#include <devioctl.h>
#include <miniport.h>
#include <ntddvdeo.h>
#include <video.h>
#include "FAKEGFXCARD.h"

/*#pragma alloc_text(PAGE, FakeGfxCard_ResetHW)      */
/*#pragma alloc_text(PAGE, FakeGfxCard_VidInterrupt) */
#pragma alloc_text(PAGE, FakeGfxCard_GetPowerState)
#pragma alloc_text(PAGE, FakeGfxCard_SetPowerState)
#pragma alloc_text(PAGE, FakeGfxCard_GetChildDescriptor)
#pragma alloc_text(PAGE, FakeGfxCard_FindAdapter)
#pragma alloc_text(PAGE, FakeGfxCard_Initialize)
#pragma alloc_text(PAGE, FakeGfxCard_StartIO)



/**********************************************************************
 * 
 *  FakeGfxCard_ResetHW
 *
 *     This routine would reset the hardware when a soft reboot is
 *     performed.  Returning FALSE from this routine would force
 *     the HAL to perform an INT 10h and set Mode 3 (Text).  
 *
 *     We are not real hardware so we will just return TRUE so the HAL
 *     does nothing.
 *
 **********************************************************************/
BOOLEAN FakeGfxCard_ResetHW(PVOID HwDeviceExtension, ULONG Columns, ULONG Rows)
{
   return TRUE;
}

/**********************************************************************
 * 
 *  FakeGfxCard_VidInterrupt
 *
 *     Checks if it's adapter generated an interrupt and dismisses it
 *     or returns FALSE if it did not.
 *
 **********************************************************************/
BOOLEAN FakeGfxCard_VidInterrupt(PVOID HwDeviceExtension)
{
   return FALSE;
}

/**********************************************************************
 * 
 *  FakeGfxCard_GetPowerState
 *
 *         Queries if the device can support the requested power state.
 *
 **********************************************************************/
VP_STATUS FakeGfxCard_GetPowerState(PVOID HwDeviceExtension, ULONG HwId, PVIDEO_POWER_MANAGEMENT VideoPowerControl)
{
   return NO_ERROR;
}

/**********************************************************************
 * 
 *  FakeGfxCard_SetPowerState
 *
 *         Sets the power state.
 *
 **********************************************************************/
VP_STATUS FakeGfxCard_SetPowerState(PVOID HwDeviceExtension, ULONG HwId, PVIDEO_POWER_MANAGEMENT VideoPowerControl)
{
   return NO_ERROR;
}

/**********************************************************************
 * 
 *  FakeGfxCard_GetChildDescriptor
 *
 *        Returns an identifer for any child device supported by the miniport.
 *
 **********************************************************************/
ULONG FakeGfxCard_GetChildDescriptor (PVOID HwDeviceExtension, PVIDEO_CHILD_ENUM_INFO ChildEnumInfo, PVIDEO_CHILD_TYPE pChildType, PUCHAR pChildDescriptor, PULONG pUId, PULONG pUnused)
{
/*	static int cnt = 0;
	if(cnt)
	{
		*pChildType = Monitor;
		*pChildDescriptor = NULL;
		*pUId = cnt;
		*pUnused = 0;
	}
	return (cnt--)?VIDEO_ENUM_MORE_DEVICES:VIDEO_ENUM_NO_MORE_DEVICES;
*/
	return VIDEO_ENUM_NO_MORE_DEVICES;
}

/**********************************************************************
 * 
 *  FakeGfxCard_FindAdapter
 *
 *        This function performs initialization specific to devices 
 *        maintained by this miniport driver.
 *
 **********************************************************************/
VP_STATUS FakeGfxCard_FindAdapter(PVOID HwDeviceExtension, PVOID HwContext, PWSTR ArgumentString, PVIDEO_PORT_CONFIG_INFO ConfigInfo, PUCHAR Again)
{
   return NO_ERROR;
}

/**********************************************************************
 * 
 *  FakeGfxCard_Initialize
 *
 *      This initializes the device.
 *
 **********************************************************************/
BOOLEAN FakeGfxCard_Initialize(PVOID HwDeviceExtension)
{
   return TRUE;
}

/**********************************************************************
 * 
 *  FakeGfxCard_StartIO
 *
 *      This routine executes requests on behalf of the GDI Driver and the
 *      system.  The GDI driver is allowed to issue IOCTLs which would then
 *      be sent to this routine to be performed on it's behalf.
 *
 *      We can add our own proprietary IOCTLs here to be processed from
 *      the GDI driver.  
 *
 **********************************************************************/
BOOLEAN FakeGfxCard_StartIO(PVOID HwDeviceExtension,PVIDEO_REQUEST_PACKET RequestPacket)
{
   RequestPacket->StatusBlock->Status      = 0;
   RequestPacket->StatusBlock->Information = 0;

   return TRUE;
}     




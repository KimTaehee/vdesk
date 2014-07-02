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


#ifndef __FAKEGFXCARD_H__
#define __FAKEGFXCARD_H__


BOOLEAN FakeGfxCard_ResetHW(PVOID HwDeviceExtension, ULONG Columns, ULONG Rows);
BOOLEAN FakeGfxCard_VidInterrupt(PVOID HwDeviceExtension); 
VP_STATUS FakeGfxCard_GetPowerState(PVOID HwDeviceExtension, ULONG HwId, PVIDEO_POWER_MANAGEMENT VideoPowerControl);
VP_STATUS FakeGfxCard_SetPowerState(PVOID HwDeviceExtension, ULONG HwId, PVIDEO_POWER_MANAGEMENT VideoPowerControl);
ULONG FakeGfxCard_GetChildDescriptor (PVOID HwDeviceExtension, PVIDEO_CHILD_ENUM_INFO ChildEnumInfo, PVIDEO_CHILD_TYPE pChildType, PVOID pChildDescriptor, PULONG pUId, PULONG pUnused);
VP_STATUS FakeGfxCard_FindAdapter(PVOID HwDeviceExtension, PVOID HwContext, PWSTR ArgumentString, PVIDEO_PORT_CONFIG_INFO ConfigInfo, PUCHAR Again);
BOOLEAN FakeGfxCard_Initialize(PVOID HwDeviceExtension);
BOOLEAN FakeGfxCard_StartIO(PVOID HwDeviceExtension,PVIDEO_REQUEST_PACKET RequestPacket);

#endif


#pragma once

#include <afxtempl.h>
#include <setupapi.h>
#include <Cfgmgr32.h>

#define OCULUS_DRIVER_NAME "oculus"

BOOL IsWow64();
HDEVINFO GetDevInfoFromDeviceId(SP_DEVINFO_DATA *dev_info_data, CHAR *device_id);
void DestroyDevInfo(HDEVINFO info);
BOOL GetDevStatus(HDEVINFO h, SP_DEVINFO_DATA *dev_info_data, PULONG status, PULONG problem);
BOOL executeCommandLine(CString cmdLine, DWORD & exitCode);

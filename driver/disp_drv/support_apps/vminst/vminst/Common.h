#pragma once

#include <windows.h>
#include <setupapi.h>
#include <Cfgmgr32.h>

BOOL IsWow64();
HDEVINFO GetDevInfoFromDeviceId(SP_DEVINFO_DATA *dev_info_data, CHAR *device_id);
void DestroyDevInfo(HDEVINFO info);
BOOL GetDevStatus(HDEVINFO h, SP_DEVINFO_DATA *dev_info_data, PULONG status, PULONG problem);
BOOL executeCommandLine(char *cmdLine, DWORD & exitCode);
int rebootSystem(void);
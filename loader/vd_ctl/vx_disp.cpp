#include "vx_disp.h"
#include "Common.h"

#define DEFAULT_MODE_XRES	800
#define DEFAULT_MODE_YRES	600
#define DEFAULT_MODE_BPP	32

//#define DRIVER_NAME "VirtualMonitor Graphics Adapter"
#define DISPLAY_VIDEO_KEY "SYSTEM\\CurrentControlSet\\Control\\Video"

/*
#define DRIVER_NAME "VirtualMonitor Graphics Adapter"
#define HARDWAREID_NAME "VirtualMonitorVideo\0\0\0"
*/ ///*
#define DRIVER_NAME "V-Desk Display Adapter"
#define HARDWAREID_NAME "VDeskDisplay"
//*/

int CVxDispDrv::SetDriverName(std::string drvname)
{
	driver_str = drvname;
	return 0;
}

int CVxDispDrv::SetHardwareID(std::string hdwid)
{
	hardwareid_str = hdwid;
	return 0;
}

int CVxDispDrv::GetMonitorCount(void)
{
	return m_cntMon;
}
BOOL CVxDispDrv::FindDeviceName(char *driverName)
{
    INT devNum = 0;
    DISPLAY_DEVICE displayDevice;
    BOOL result;
    BOOL bFound = FALSE;

    FillMemory(&displayDevice, sizeof(DISPLAY_DEVICE), 0);
    displayDevice.cb = sizeof(DISPLAY_DEVICE);
	m_cntMon = 0;

    // First enumerate for Primary display device:
    while ((result = EnumDisplayDevices(NULL, devNum, &displayDevice, 0))) {
#if 0
		printf("%s, name: %s\n\tid: %s\n\t key: %s\n",
				&displayDevice.DeviceString[0],
				&displayDevice.DeviceName[0],
				&displayDevice.DeviceID[0],
				&displayDevice.DeviceKey[0]);
#endif
		{
			printf("candidate : %s\n",displayDevice.DeviceString);
			printf("candidate's dn : %s\n",(LPSTR)&displayDevice.DeviceName[0]);
		}
        if (strcmp(&displayDevice.DeviceString[0], driverName) == 0) {
            bFound = TRUE;
			m_cntMon++;
			deviceNames.push_back((LPSTR)&displayDevice.DeviceName[0]);
//	        memcpy(&deviceName[0], (LPSTR)&displayDevice.DeviceName[0], sizeof(displayDevice.DeviceName));
		    printf("selected %s\n",(LPSTR)&displayDevice.DeviceName[0]);
//			break;  
        }
        devNum++;
    }
    return bFound;
}

int CVxDispDrv::enable(int idx,POINT sz)
{
    DEVMODE defaultMode;
    HDC hdc;
    int nWidth;
    INT code;
    DWORD dwFlags = 0;

	//전체 작업 영역 사이즈를 얻어옴
    hdc = GetDC(0);
    // nWidth = GetDeviceCaps(hdc,HORZRES);
    nWidth = GetSystemMetrics(SM_CXSCREEN);
    ReleaseDC(0,hdc);
    
    ZeroMemory(&defaultMode, sizeof(DEVMODE));
    defaultMode.dmSize = sizeof(DEVMODE);
        defaultMode.dmDriverExtra = 0;

    /* Without this, Windows will not ask the miniport for its
     * mode table but uses an internal cache instead.
     */
		EnumDisplaySettings(deviceNames[idx].c_str(), 0xffffff, &defaultMode);

    ZeroMemory(&defaultMode, sizeof(DEVMODE));
    defaultMode.dmSize = sizeof(DEVMODE);
        defaultMode.dmDriverExtra = 0;

    if (!EnumDisplaySettings(deviceNames[idx].c_str(),
             ENUM_REGISTRY_SETTINGS, &defaultMode)) {
        printf("Device: %s\n", deviceNames[idx].c_str());
        return -1; // Store default failed
    }

#if 0
    if (defaultMode.dmPelsWidth == 0 || defaultMode.dmPelsHeight == 0) {
        if (!EnumDisplaySettings(gXpdmMonitor.s_DeviceName,
             ENUM_CURRENT_SETTINGS, &defaultMode)) {
            ZeroMemory(&defaultMode, sizeof(DEVMODE));
            defaultMode.dmSize = sizeof(DEVMODE);
            defaultMode.dmDriverExtra = 0;
        }
    }
#endif

    defaultMode.dmPelsWidth = sz.x;
    defaultMode.dmPelsHeight = sz.y;
    defaultMode.dmBitsPerPel = DEFAULT_MODE_BPP;

    defaultMode.dmFields = DM_BITSPERPEL |
                DM_PELSWIDTH | 
                DM_PELSHEIGHT |
                DM_POSITION ;


    defaultMode.dmPosition.x += nWidth;

    // StringCbCopy((LPSTR)&defaultMode.dmDeviceName[0], sizeof(defaultMode.dmDeviceName), "mirror");

    code = ChangeDisplaySettingsEx(deviceNames[idx].c_str(), 
                    &defaultMode,
                    NULL,
                    CDS_NORESET | CDS_UPDATEREGISTRY,
                    NULL); 


    // printf("Update Registry on device mode: %s\n", GetDispCode(code));
    code = ChangeDisplaySettingsEx(NULL,
                    NULL,
                    NULL,
                    0,
                    NULL);
    // printf("Raw dynamic mode change on device mode: %s\n", GetDispCode(code));
/*
    if (!XpdmDrvCtrlReStart()) {
        Disable();
        return -1;
    }
*/
    return 0;
}

int CVxDispDrv::disable(int idx)
{
    DEVMODE defaultMode;
    INT code;
/*
	if (pVideoMemory) {
		ExtEscape(hDC,
				   VM_CTL_UNMAP_VIDEO_MEMORY,
                   sizeof(pVideoMemory), 
                   (LPSTR)&pVideoMemory, 
                   0, 
                   NULL);
	}
*/
    ZeroMemory(&defaultMode, sizeof(DEVMODE));
    defaultMode.dmSize = sizeof(DEVMODE);
        defaultMode.dmDriverExtra = 0;

    defaultMode.dmFields = DM_BITSPERPEL |
                DM_PELSWIDTH | 
                DM_PELSHEIGHT |
                DM_POSITION |
                DM_DISPLAYFREQUENCY |
                DM_DISPLAYFLAGS;

    code = ChangeDisplaySettingsEx(deviceNames[idx].c_str(), 
                    &defaultMode,
                    NULL,
                    CDS_NORESET | CDS_UPDATEREGISTRY,
                    NULL); 

    // printf("Update Registry on device mode: %s\n", GetDispCode(code));
    code = ChangeDisplaySettingsEx(NULL,
                    NULL,
                    NULL,
                    0,
                    NULL);
    // printf("Raw dynamic mode change on device mode: %s\n", GetDispCode(code));
	
//	aeroctrl(TRUE);
    return 0;
}


int CVxDispDrv::aeroctrl(BOOL enable)
{
	int ret = -1;
	BOOL isEnabled = TRUE;
	HMODULE dwmapi;

	typedef HRESULT (CALLBACK *PfnDwmEnableComposition)(BOOL   fEnable);
	PfnDwmEnableComposition pfnDwmEnableComposition;
	typedef HRESULT (CALLBACK *PfnDwmIsCompositionEnabled)(BOOL *pfEnabled);
	PfnDwmIsCompositionEnabled pfnDwmIsCompositionEnabled = NULL;

	if (osvi.dwMajorVersion >= 6) {
		dwmapi = LoadLibrary("dwmapi.dll");
		if (dwmapi != NULL) {
			pfnDwmIsCompositionEnabled = (PfnDwmIsCompositionEnabled)GetProcAddress(dwmapi, "DwmIsCompositionEnabled");
			pfnDwmEnableComposition = (PfnDwmEnableComposition)GetProcAddress(dwmapi, "DwmEnableComposition");
			if (pfnDwmIsCompositionEnabled) {
				pfnDwmIsCompositionEnabled(&isEnabled);
			}
			ret = 0;
			// printf("Aero: %d, new: %d\n", isEnabled, enable);
			if (isEnabled != enable) {
				if (pfnDwmEnableComposition) {
					pfnDwmEnableComposition(enable);
				}
			}
			FreeLibrary(dwmapi);
		}
	} else {
		ret = 0;
	}
	return ret;
}

bool CVxDispDrv::changePrimaryMonitor()
{
	DWORD DispNum = 0;
    DISPLAY_DEVICE DisplayDevice;
    DEVMODE defaultMode;
    HDC hdc;
    int nWidth, nHeight;
    BOOL bFoundSecondary = FALSE;
	
	hdc = GetDC(0);
    nWidth = GetDeviceCaps(hdc,HORZRES);
	nHeight = GetDeviceCaps(hdc,VERTRES);
    ReleaseDC(0,hdc);

	// Initialize DisplayDevice.
    ZeroMemory(&DisplayDevice, sizeof(DisplayDevice));
    DisplayDevice.cb = sizeof(DisplayDevice);

	// Get display devices.
    while ((EnumDisplayDevices(NULL, DispNum, &DisplayDevice, 0)) &&
           (bFoundSecondary == FALSE))
    { 
        ZeroMemory(&defaultMode, sizeof(DEVMODE));
        defaultMode.dmSize = sizeof(DEVMODE);
        if (!EnumDisplaySettings((LPSTR)DisplayDevice.DeviceName,
             ENUM_REGISTRY_SETTINGS, &defaultMode))
            return FALSE; // Store default failed

		printf("DispNum : %d ", DispNum);
		printf("%s \n", (LPSTR)DisplayDevice.DeviceName);
		
		if ((DisplayDevice.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE) && (DisplayDevice.StateFlags & DISPLAY_DEVICE_ACTIVE))
		{
			defaultMode.dmFields = DM_POSITION; 
			//defaultMode.dmPosition.x += nWidth;
			//defaultMode.dmPosition.x -= nWidth;
			//defaultMode.dmPosition.y = 0;
			defaultMode.dmPosition.x = -1024;
			defaultMode.dmPosition.y = 0;

			ChangeDisplaySettingsEx((LPSTR)DisplayDevice.DeviceName, 
                    &defaultMode, NULL, CDS_UPDATEREGISTRY|CDS_SET_PRIMARY, NULL); 
			
			// A second call to ChangeDisplaySettings updates the monitor.
			//ChangeDisplaySettings(NULL, 0); 

			printf("I changed PRIMARY DEVICE\n");

			bFoundSecondary = TRUE;

			//DispNum++;
			//EnumDisplayDevices(NULL, DispNum, &DisplayDevice, 0);
		}
		
		defaultMode.dmFields = DM_POSITION; 
		
		printf("x : %d, y : %d\n", defaultMode.dmPosition.x, defaultMode.dmPosition.y);
		
        // Reinitialize DisplayDevice. 
        ZeroMemory(&DisplayDevice, sizeof(DisplayDevice));
        DisplayDevice.cb = sizeof(DisplayDevice);
        DispNum++;
    } // End while the display devices. 

	return true;
}



int CVxDispDrv::init(void)
{
	HDEVINFO h = NULL;
	SP_DEVINFO_DATA dev_info_data;
	ULONG status = 0, problem = 0;

	if (!FindDeviceName((char*)driver_str.c_str())) {
		h = GetDevInfoFromDeviceId(&dev_info_data, (char*)hardwareid_str.c_str());
		if (!h) {
			printf("Driver Not installed\n");
		} else {
			GetDevStatus(h, &dev_info_data, &status, &problem);
			printf("Driver report status: %08x, problem: %08x\n", status, problem);
			DestroyDevInfo(h);
		}
        return -1;
    }
/*

    if (!OpenDeviceRegistryKey()) {
        printf("Can't find %s in Registery\n", DRIVER_NAME);
        return -1;
    }
*/	
	ZeroMemory(&osvi, sizeof(osvi));
	osvi.dwOSVersionInfoSize = sizeof(osvi);
	GetVersionEx(&osvi);

    return 0;
}

int CVxDispDrv::mon_begin(void)
{
	aeroctrl(FALSE);
	return 0;
}

int CVxDispDrv::mon_end(void)
{
	aeroctrl(TRUE);
	return 0;
}

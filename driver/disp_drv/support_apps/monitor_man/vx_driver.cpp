#include <windows.h>
#include <stdio.h>
#include "videomemory.h"

#include "vx_driver.h"

#define	ESC_GET_BUFFER	0x10001
#define	ESC_GET_UNIQ	0x10002
#define	ESC_GET_MAPPATH	0x10003

#define MAX_VIDEO_PATH	256


//vx 드라이버의 모니터 정보가 보관되는 structure
struct vx_mon_state {
	POINT pos;
	POINT res;
	int depth;
	wchar_t wpath[MAX_VIDEO_PATH];
	PCHAR DeviceVideoMemory;
	PCHAR DeviceFramebuffer;
	BOOL is_vxDevice;	
	int token;	//모니터 상태 변화 감지 토큰
	int token_last;	//마지막 호출한 모니터 정보를 임시 저장함

	HDC hDC;
	char DeviceName[32];
	char DeviceString[128];
	unsigned long DeviceState;
};

// vx 드라이버의 정보가 보관되는 structure
struct vx_state{
	struct vx_mon_state monitor[10];
	int max_mon;
};

void i_vx_add_device(vx_state *vx,DISPLAY_DEVICE dd);


vx_state *vx_init()			//드라이버 초기화
{
	vx_state *vx = new vx_state;
	
	memset(vx,0,sizeof(vx_state));
	
	return vx;
}

void vx_release(vx_state *vx)		//드라이버 해제
{	
	int i;

	for(i=0;i<vx->max_mon;i++)
	{
		if(vx->monitor[i].is_vxDevice)
		{
			DeleteDC(vx->monitor[i].hDC);
			i_vx_ReleaseSharedMemory(vx->monitor[i].DeviceVideoMemory);
		}
	}

	delete vx;
}

void i_vx_add_device(vx_state *vx,DISPLAY_DEVICE dd)
{
	int res;

	strcpy(vx->monitor[vx->max_mon].DeviceName,dd.DeviceName);
	strcpy(vx->monitor[vx->max_mon].DeviceString,dd.DeviceString);
	vx->monitor[vx->max_mon].DeviceState = dd.StateFlags;

	//framebuffer / token / vxDevice / res / depth 갱신하기
	// Token
	vx->monitor[vx->max_mon].hDC = CreateDC(NULL,vx->monitor[vx->max_mon].DeviceName, NULL, NULL);
	res = ExtEscape(vx->monitor[vx->max_mon].hDC,ESC_GET_UNIQ,0,NULL,sizeof(vx->monitor[vx->max_mon].token),(PCHAR)&(vx->monitor[vx->max_mon].token));
	if(!res)
	{
		//만일 가상 장치가 아닌 경우 release 시킨다
		vx->monitor[vx->max_mon].is_vxDevice = FALSE;
		DeleteDC(vx->monitor[vx->max_mon].hDC);
	}
	else
	{
		//Framebuffer 및 해상도 정보를 갱신한다.
		vx->monitor[vx->max_mon].is_vxDevice = TRUE;
		
		res = ExtEscape(vx->monitor[vx->max_mon].hDC,ESC_GET_MAPPATH,0,NULL,sizeof(vx->monitor[vx->max_mon].wpath),(PCHAR)&(vx->monitor[vx->max_mon].wpath));
		if(!res)
		{
			//failed!
			return;
		}

		vx->monitor[vx->max_mon].DeviceVideoMemory = i_vx_GetSharedMemoryW(vx->monitor[vx->max_mon].wpath);
		vx->monitor[vx->max_mon].pos.x = *(((int*)vx->monitor[vx->max_mon].DeviceVideoMemory));
		vx->monitor[vx->max_mon].pos.y = *(((int*)vx->monitor[vx->max_mon].DeviceVideoMemory)+1);
		vx->monitor[vx->max_mon].depth = *(((int*)vx->monitor[vx->max_mon].DeviceVideoMemory)+2);
		vx->monitor[vx->max_mon].DeviceFramebuffer = (char*)(((int*)vx->monitor[vx->max_mon].DeviceVideoMemory)+3);
	}
	vx->max_mon++;
}

//모니터 개수가 리턴됨
int vx_get_devices(vx_state *vx)	//목록을 새로 얻어옴
{
    DISPLAY_DEVICE dd;

    dd.cb = sizeof(DISPLAY_DEVICE);

    DWORD deviceNum = 0;

	memset(vx,0,sizeof(vx_state));

    while( EnumDisplayDevices(NULL, deviceNum, &dd, 0) ){
		i_vx_add_device(vx,dd);
        DISPLAY_DEVICE newdd = {0};
        newdd.cb = sizeof(DISPLAY_DEVICE);
        DWORD monitorNum = 0;
        while ( EnumDisplayDevices(dd.DeviceName, monitorNum, &newdd, 0))
        {
			i_vx_add_device(vx,newdd);
            monitorNum++;
        }
        deviceNum++;
    }
	return vx->max_mon;
}
/*
void vx_set_devices_position()	//모니터들의 배치 및 해상도를 변경함
{
	DEVMODE dm;
	CString strdev;

	memset(&dm,0,sizeof(DEVMODE));
	dm.dmSize = sizeof (DEVMODE);
	dm.dmFields = DM_POSITION;
	dm.dmPosition.x = 1024;
	dm.dmPosition.y = 0;
	dm.dmPelsWidth = 1024;
	dm.dmPelsHeight = 768;

	LONG ret = ChangeDisplaySettingsEx (strdev, &dm, NULL, CDS_UPDATEREGISTRY, NULL);
	if (ret != DISP_CHANGE_SUCCESSFUL)
	AfxMessageBox (_T("Mode change failed!"));
}
*/

vx_mon_state *vx_get_desc(vx_state *vs,int idx)		//해당 드라이버의 디스크립터를 얻어옴
{
	return vs->monitor[idx].is_vxDevice?&vs->monitor[idx]:NULL;
}

BOOL vx_mon_isUpdate(vx_mon_state *mon)	//해당 모니터가 업데이트 되었나 ?
{
	int res;
	res = ExtEscape(mon->hDC,ESC_GET_UNIQ,0,NULL,sizeof(mon->token_last),(PCHAR)&mon->token_last);
	if(!res)
		return FALSE;
	if(mon->token == mon->token_last)
		return FALSE;
	return TRUE;
}

//토큰정보를 반영하여 프레임 갱신하였음을 알림
void vx_mon_tokenUpdate(vx_mon_state *mon)
{
	mon->token = mon->token_last;
}

PCHAR vx_mon_getFramebuffer(vx_mon_state *mon)	//framebuffer의 포인터를 얻어옴
{
	return mon->DeviceFramebuffer;
}

POINT vx_mon_getResolution(vx_mon_state *mon)	//해상도 정보를 얻어옴
{
	return mon->res;
}

//http://www.poshy.net/55012
//http://www.codeproject.com/Articles/37900/NET-Wrapper-for-ChangeDisplaySettingsEX
/*
void vx_mon_changeResolution(vx_mon_state *mon,POINT res)	//해당 모니터의 해상도를 변경
{
	DEVMODE *pDevMode=&devMode;
	int n;
 
	EnumDisplaySettings(NULL,원하는해상도ID, pDevMode);
	pDevMode->dmSize=sizeof(DEVMODE);
	pDevMode->dmFields=DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
	n=ChangeDisplaySettings(pDevMode, 0);
	if(n==DISP_CHANGE_RESTART)
		printf("윈도우를 재부팅해야합니다.\n");


}
*/

/*
	if(EnumDisplaySettingsEx(device.DeviceName, ENUM_CURRENT_SETTINGS, &deviceMode, 0)) {
			actualWidth = deviceMode.dmPelsWidth;
			actualHeight = deviceMode.dmPelsHeight;
			if((actualWidth != newWidth) || (actualHeight != newHeight)) {
				//	change requested
				deviceMode.dmPelsWidth	= newWidth;
				deviceMode.dmPelsHeight	= newHeight;
				if(ChangeDisplaySettingsEx(device.DeviceName, &deviceMode, 					
					NULL, 0, NULL) == DISP_CHANGE_SUCCESSFUL) {
					//	broadcast change to system
					SendMessage(HWND_BROADCAST, WM_DISPLAYCHANGE, 
						(WPARAM)(deviceMode.dmBitsPerPel), 
						MAKELPARAM(newWidth, newHeight));
				}
			}
		}
*/
#include <afxtempl.h>
#include <stdio.h>
#include <vector>
#include <string>

#define DEVICE_NAME_SIZE 32

class CVxDispDrv
{
public:

private:
	int m_cntMon;

	OSVERSIONINFO osvi;
    HDC hDC;
    std::vector<std::string> deviceNames;
	//CHAR deviceName[DEVICE_NAME_SIZE];

	std::string driver_str;
	std::string hardwareid_str;

private:
	int aeroctrl(BOOL enable);

public:
	int init(void);
	int mon_begin(void);
	int mon_end(void);
	int enable(int idx,POINT sz);
	int disable(int idx);
	int GetMonitorCount(void);
	int SetDriverName(std::string drvname);
	int SetHardwareID(std::string hdwid);
	BOOL FindDeviceName(char *driverName);
	bool changePrimaryMonitor();
};

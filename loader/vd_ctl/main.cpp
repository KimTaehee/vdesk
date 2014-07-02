#include <stdio.h>
#include "vx_disp.h"
#include "Common.h"

/* ini
http://www.codeguru.com/cpp/misc/misc/article.php/c233/CIniFile--Class-for-Reading-and-Writing-INI-Files.htm
*/

using namespace std;

#include "iniFile/iniFile.h"

#pragma comment(lib,"Setupapi.lib")

struct configure{
	string launcher_opt;
	string hardwareid;
	string drivername;
	POINT resol[3];
};

struct configure g_conf;

int load_configure(void)
{
	string res;
	CIniFile ini_p("setting.ini");
	if(ini_p.ReadFile())
	{

		g_conf.launcher_opt = ini_p.GetValue("Launcher","executeArgument");

		sscanf((char*)ini_p.GetValue("Monitor","1").c_str()," %d, %d",&g_conf.resol[0].x,&g_conf.resol[0].y);
		sscanf((char*)ini_p.GetValue("Monitor","2").c_str()," %d, %d",&g_conf.resol[1].x,&g_conf.resol[1].y);
		sscanf((char*)ini_p.GetValue("Monitor","3").c_str()," %d, %d",&g_conf.resol[2].x,&g_conf.resol[2].y);	

		g_conf.drivername = ini_p.GetValue("Driver","DRIVER_NAME");
		g_conf.hardwareid = ini_p.GetValue("Driver","HARDWAREID_NAME");
	}
	else
	{
		puts("setting.ini file not found!");
		return -1;
	}
	return 0;
}

int main(int argc,char **argv)
{
	int p,i,monCnt;
	CVxDispDrv vx;
	POINT sz;
	DWORD exitCode = 0;

	/*
		해야할 일
		1. 모니터 별 설정이 가능하도록 파일 설정 추가.(ini 파일을 통해서 또는 XML) - OK
		2. Oculus의 Monitor Adapter 번호가 몇번인지 확인(확인용 스트링은 설정파일을 통해 입력) 후 executeCommandLine으로 adapter 번호를 넘겨줄것
		3. executeCommandLine을 통해 프로그램 실행 전, (vx.enable하면서) primary monitor를 vx.enable의 모니터 중 하나로 설정 및 프로그램 종료 전에 primary monitor를 원래 primary monitor로 되돌림
	*/
	if(load_configure())
		return -1;

	sz.x = 1600;
	sz.y = 1200;
	
	vx.SetDriverName(g_conf.drivername);
	vx.SetHardwareID(g_conf.hardwareid);
	vx.init();


	vx.mon_begin();
	monCnt = vx.GetMonitorCount();
	// 실패했을 때 fail 뜨게 할 수 있도록
	for(i=0;i<monCnt;i++)
	{
		sz = g_conf.resol[i];
		vx.enable(i,sz);
	}

	if(monCnt)
	{
		string argm;

		Sleep(1000);	//모니터가 정상적으로 확장될때까지 기다림
		argm = "protype.exe ";
		argm += g_conf.launcher_opt;

		vx.changePrimaryMonitor();

		if(!executeCommandLine(argm.c_str(),exitCode))
			MessageBox(NULL,"Fail!","Error",MB_OK);
	}

	for(i=0;i<monCnt;i++)
		vx.disable(i);

	vx.mon_end();

	return 0;
}
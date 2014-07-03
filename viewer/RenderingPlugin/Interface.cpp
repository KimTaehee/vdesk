#include "Interface.h"
#include "vx_driver.h"
#include "CommonVar.h"

MON_UPDATE_PARAM g_inf_RenderList;

// --------------------------------------------------------------------------
// SetTimeFromUnity, an example function we export which is called by one of the scripts.

static float g_Time;

extern "C" void EXPORT_API SetTimeFromUnity (float t) { g_Time = t; }
// --------------------------------------------------------------------------
// SetTextureFromUnity, an example function we export which is called by one of the scripts.

extern "C" int EXPORT_API GetMonitorResolutionX(int targ_mon)
{
	vx_mon_state *vmon = NULL;
	
	if(mon_lst.size() > targ_mon)
		vmon = mon_lst[targ_mon];
	else
		return -1;

	POINT p = vx_mon_getResolution(vmon);

	return p.x;
}

extern "C" int EXPORT_API GetMonitorResolutionY(int targ_mon)
{
	vx_mon_state *vmon = NULL;
	
	if(mon_lst.size() > targ_mon)
		vmon = mon_lst[targ_mon];
	else
		return -1;

	POINT p = vx_mon_getResolution(vmon);

	return p.y;
}

extern "C" void EXPORT_API LinkMonitor(int monitor,void *texturePtr)
{
	MonitorUpdateParam m;
	
	m.targMon = monitor;
	m.texPtr = texturePtr;
	
	g_inf_RenderList.push_back(m);
}

extern "C" void EXPORT_API UnlinkMonitor(int monitor,void *texturePtr)
{
	MON_UPDATE_PARAM::iterator it;

	for(it = g_inf_RenderList.begin();it!=g_inf_RenderList.end();++it)
		if ((*it).targMon == monitor && (*it).texPtr == texturePtr)
		{
			g_inf_RenderList.erase(it);
			break;
		}
}
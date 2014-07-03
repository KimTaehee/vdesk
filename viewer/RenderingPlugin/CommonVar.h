#pragma once

#include "vx_driver.h"
#include <list>
#include <vector>

typedef struct MonitorUpdateParam MonitorUpdateParam;
typedef std::list<MonitorUpdateParam> MON_UPDATE_PARAM;

struct MonitorUpdateParam{
	int targMon;
	void *texPtr;
	int token;
};

extern MON_UPDATE_PARAM g_inf_RenderList;
extern float g_Time;
extern std::vector<vx_mon_state*> mon_lst;
extern vx_state *vx;

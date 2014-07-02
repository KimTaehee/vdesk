#pragma once

#include "RenderingPlugin.h"

extern "C" void EXPORT_API SetTimeFromUnity (float t);
extern "C" int EXPORT_API GetMonitorResolutionX(int targ_mon);
extern "C" int EXPORT_API GetMonitorResolutionY(int targ_mon);
extern "C" void EXPORT_API LinkMonitor(int monitor,void *texturePtr);
extern "C" void EXPORT_API UnlinkMonitor(int monitor,void *texturePtr);
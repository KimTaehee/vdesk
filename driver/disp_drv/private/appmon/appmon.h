/**********************************************************************
 * 
 *  Toby Opferman
 *
 *  Example Application which uses shared memory to display the contents
 *  of a virtual second monitor
 *
 *  This example is for educational purposes only.  I license this source
 *  out for use in learning.
 *
 *  Copyright (c) 2005, All Rights Reserved  
 **********************************************************************/


#ifndef __APPMON_H__
#define __APPMON_H__

/*
 * Basic Constants For Title and Class Name
 */
#define APPMON_CLASS_NAME "TobyApplicationMonitor"
#define APPMON_TITLE  "Virtual Second Monitor"

/*
 * Hard coded sizes for Window Dimensions.  Idealy the "+ 10" should actually
 * be calculated by determining the border size instead of being fixed.
 *
 * The Client Area should also be recieved dynamically instead of using contants.
 */
#define APPMON_WINDOW_WIDTH  800 + 10
#define APPMON_WINDOW_HEIGHT 600 + 10
#define APPMON_CLIENT_WIDTH  800
#define APPMON_CLIENT_HEIGHT 600

/*
 * Resources
 */       
#define IDDCONFIGBOX 100
#define IDPID        101
#define IDPADD       102

/*
 * The Refresh Freq. Timer.  This isn't a high precision timer and 
 * Windows itself it not even real time so this is really just a "suggestion".
 * 
 * We are not looking for 100% accuracy and this is just a demonstration.
 */                                      
#define MONITOR_REFRESH_TIMER 1
#define MONITOR_REFRESH_FREQ  1000/60 

             
BOOL WINAPI AppMon_RegisterClass(HINSTANCE hInstance);
HWND WINAPI AppMon_CreateWindow(HINSTANCE hInstance);
int WINAPI AppMon_MessageLoop(HINSTANCE hInstance, HWND hWnd);



#endif



/**********************************************************************
 * 
 *  Toby Opferman
 *
 *  Video Memory - Header File Used To Share Memory Between
 *                 Video Driver & Application
 *
 *  This example is for educational purposes only.  I license this source
 *  out for use in learning how to write a device driver.
 *
 *  Copyright (c) 2005, All Rights Reserved  
 **********************************************************************/



#ifndef __VIDEOMEMORY_H__
#define __VIDEOMEMORY_H__

PCHAR VideoMemory_GetSharedMemory(void);
void VideoMemory_ReleaseSharedMemory(PCHAR pVideoMemory);


#endif



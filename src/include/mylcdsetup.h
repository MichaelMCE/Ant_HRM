
// libmylcd - http://mylcd.sourceforge.net/
// An LCD framebuffer and text rendering API
// Michael McElligott
// okio@users.sourceforge.net

//  Copyright (c) 2005-2011  Michael McElligott
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU LIBRARY GENERAL PUBLIC LICENSE
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU LIBRARY GENERAL PUBLIC LICENSE for more details.
//
//	You should have received a copy of the GNU Library General Public
//	License along with this library; if not, write to the Free
//	Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#ifndef _MYLCDSETUP_H_
#define _MYLCDSETUP_H_


//#include <conio.h>
#include <windows.h>
#include <mylcd.h>

#ifndef mylog
#define mylog printf
#endif

#define G19DISPLAY				(1)
#define FONTD(x)				L"data/"x
#define FONT					LFTW_B14
#define LPAGE					CMT_ISO8859_15


#if G19DISPLAY		// configure for the G19 @ 320x240
#define DISPLAYMAX	(2)
char *device[] = {"DDRAW", "G19", ""};

#define DWIDTH		(320)
#define DHEIGHT		(240)
#define DBPP		LFRM_BPP_32A	/* ARGB surfaces needed for the UI */
#define VPITCH(w)	((w)<<2)

#else				// configure for the USBD480 @ 480x272

#define DISPLAYMAX	(3)
char *device[] = {"USBD480:LIBUSBHID", "USBD480:LIBUSB", "DDRAW"};

#define DWIDTH		(480)
#define DHEIGHT		(272)
#define DBPP		LFRM_BPP_32A	/* ARGB surfaces are needed for the UI */
#define VPITCH(w)	((w)<<2)		/* pitch based upon above chroma*/
#endif


THWD *hw = NULL;
TFRAME *frame = NULL;
static TRECT displays[DISPLAYMAX];
static TRECT *display;

int initMYLCD ()
{
	
	mylog("initLibrary: starting libmylcd...\n");
	/* open libmylcd using default font and cmap directory location */
    if (!(hw=lOpen(FONTD(""), FONTD("")))){
    	mylog("initLibrary: lOpen() failed\n");
    	return 0;
    }

	mylog("initLibrary: requesting global display surface\n");
	// this is our primary display surface.
    if (!(frame=lNewFrame(hw, DWIDTH, DHEIGHT, DBPP))){
    	lClose(hw);
    	mylog("initLibrary: lNewFrame() failed\n");
    	return 0;
    }else{
		lRefresh(frame);	// clear display
		display = displays;
		mylog("initLibrary: libmylcd started successfully\n");
    	return 1;
    }
}

void closeMYLCD_USBD480 ()
{
	mylog("cleanup: freeing global frame handle\n");
	if (frame){
		lDeleteFrame(frame);
		frame = NULL;
	}
	mylog("cleanup: closing libmylcd device handle\n");
	if (hw){
		lClose(hw);
		hw = NULL;
	}
	mylog("cleanup: libmylcd shutdown\n");
}

int initMYLCD_USBD480 ()
{
	if (!initMYLCD())
		return 0;
		
	memset(displays, 0, sizeof(TRECT) * DISPLAYMAX);
	displays[0].left = 0;
	displays[0].top = 0;
	displays[0].right = DWIDTH-1;
	displays[0].btm = DHEIGHT-1;

	mylog("searching for device..\n");
	
	int did = 0;
	int i;
	for (i = 0; i < DISPLAYMAX; i++){
		mylog("trying '%s'...",device[i]);
		did = lSelectDevice(hw, device[i], "NULL", DWIDTH, DHEIGHT, DBPP, 0, &displays[0]);
		if (!did){
			mylog(" failed\n");
		}else{
			mylog(" attached\n");
			break;
		}
	}
		
	if (did){
		mylog("device '%s' selected\n", device[i]);

		lSetPixelWriteMode(frame, LSP_SET);
		lSetBackgroundColour(hw, lGetRGBMask(frame, LMASK_BLACK));
		lSetForegroundColour(hw, lGetRGBMask(frame, LMASK_WHITE));
		lSetCharacterEncoding(hw, LPAGE);
		lCacheCharacterRange(hw, 0, 128, FONT);
	}else{
		mylog("program startup failed\n");
	}
	return did;
}

#endif


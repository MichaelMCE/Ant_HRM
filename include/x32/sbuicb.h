
// libmylcd
// An LCD framebuffer library
// Michael McElligott
// okio@users.sourceforge.net

//  Copyright (c) 2005-2012  Michael McElligott
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


#ifndef _SBUICB153_H_
#define _SBUICB153_H_

#ifndef SBUISDK_VERSION
#define SBUISDK_VERSION	"1.5.3"
#endif

#define SBUI_ISV_VID				0x1532
#define SBUI_ISV_PID				0x0117

#define SBUI_CTX_CANRENDER_KEYLCD	0x01
#define SBUI_CTX_CANRENDER_PADLCD	0x02

#define SBUI_DK_WIDTH				120
#define SBUI_DK_HEIGHT				120
#define SBUI_PAD_WIDTH				800
#define SBUI_PAD_HEIGHT				480

#define SBSDKDLL "SwitchBladeSDK32.dll"


#ifndef TSBUIRENDERSTATSRUCT
#define TSBUIRENDERSTATSRUCT 1

typedef struct {
	int count;
	int maxTime;
	int lastTime;
	int averageTime;
}TSBUIRENDERSTATS;


typedef struct{
	int type;
	int params;
	int x;
	int y;
	int z;

	unsigned int ct;
	//int64_t time;
	//int64_t timePrev;
	double time;
	double timePrev;
	double dt;
	unsigned int id;
}TSBGESTURE;

#define SBUI_SETDK_FILE			1
#define SBUI_SETDK_IMAGE		2
#define SBUI_SETDK_DIRECT		3		// render anywhere on to the upper LCD

typedef struct{
	size_t size;	// size of this struct
	int op;			// render via file, image or direct

	union{
		struct {
			int key;
			//int state;			// up or down
			wchar_t *path;		// path to image, must be 115x115
		}file;
		struct {
			int key;
			TFRAME *image;		// any size is fine
		}image;
		struct {
			TLPOINTEX src;		// source region within image [to copy]
			T2POINT des;		// where to place the pixels
			TFRAME *image;
		}direct;
	}u;
}TSBGESTURESETDK;


typedef struct{
	int op;
	int gesture;
	int state;
}TSBGESTURECBCFG;

enum _SBUI_DK
{
    SBUI_DK_INVALID = 0,
    SBUI_DK_1,
    SBUI_DK_2,
    SBUI_DK_3,
    SBUI_DK_4,
    SBUI_DK_5,
    SBUI_DK_6,
    SBUI_DK_7,
    SBUI_DK_8,
    SBUI_DK_9,
    SBUI_DK_10,
    SBUI_DK_UNDEFINED
};


#define	SBUI_DK_RZBUTTON		(1024)
#define	SBUI_DK_CLOSE			(SBUI_DK_RZBUTTON+0)
#define	SBUI_DK_EXIT			(SBUI_DK_RZBUTTON+1)
#define	SBUI_DK_ACTIVATE		(SBUI_DK_RZBUTTON+2)
#define	SBUI_DK_DEACTIVATE		(SBUI_DK_RZBUTTON+3)


enum _SBUI_DK_STATE
{
    SBUI_DK_DISABLED = 1,
    SBUI_DK_UP,
    SBUI_DK_DOWN,
};



//gestures
// keep in sync with SwitchBladeSDK_types.h::RZSDKGESTURETYPE
#define SBUICB_GESTURE_INVALID    0x00000000
#define SBUICB_GESTURE_NONE       0x00000001
#define SBUICB_GESTURE_PRESS      0x00000002
#define SBUICB_GESTURE_TAP        0x00000004
#define SBUICB_GESTURE_FLICK      0x00000008
#define SBUICB_GESTURE_ZOOM       0x00000010
#define SBUICB_GESTURE_ROTATE     0x00000020
#define SBUICB_GESTURE_ALL        0x0000003f
#define SBUICB_GESTURE_UNDEFINED  0xffffffc0

#endif


//op functions
#define SBUICB_OP_GestureEnable					1
#define SBUICB_OP_GestureSetNotification		2
#define SBUICB_OP_GestureSetOSNotification		3


//states
#define SBUICB_STATE_DISABLED	0
#define SBUICB_STATE_ENABLED 	1



typedef int (*pgesturecb) (const TSBGESTURE *sbg, void *ptr);
typedef int (*pdkcb) (const int dk, const int state, void *ptr);

//int sbuiDKCB (const int dk, const int state, void *ptr);


#endif


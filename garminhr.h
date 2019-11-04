
// anthrm - http://mylcd.sourceforge.net/
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

#ifndef _GARMINHR_H_
#define _GARMINHR_H_

#include "libantplus.h"


#define KEY_SIZE		8
#define MARQUEE_CENTER	0x01
#define MARQUEE_LEFT	0x02

enum _akeys {
	KEY_ANTSPORT,
	KEY_SUUNTO,
	KEY_GARMIN,
	KEY_ANTPLUS,
	KEY_TOTAL
};

typedef struct{
	char line[MAX_PATH];	// text to display
	unsigned int time;			// display until this time is reached
}TMARQUEELINE;

typedef struct{
	TMARQUEELINE *entry;
	int total;
	HANDLE hLock;		// in this instance, this lock isn't actually required as we're single threaded. can  be removed.
	unsigned int flags;
	int ready;
}TMARQUEE;

typedef struct{
	int channel;
	int keyIdx;
	int RFFreq;
	int channelPeriod;
	int searchTimeout;
	int searchWaveform;
	
	int channelType;
	int networkNumber;
	int deviceNumber;
	int deviceType;
	int transType;

	void *lock;		// ready to update hr
	void *unlock;	// update complete
}TDCONFIG;

typedef struct{
	int channelStatus;
	int chanIdOnce;
	int running;
	int vDisplay;
	int openCt;
}TCONNSTATES;

#define HRBMP_BUFFERLENGTH (1024)

typedef struct{
	unsigned char bpm[HRBMP_BUFFERLENGTH];
	unsigned char currentBpm;
	int currentSequence;
	int previousSequence;
	
	int low;	// lowest thus far
	int high;	// maximum read thus far
}THRBUFFER;

typedef struct{
	int scidDeviceNumber;
	int scidDeviceType;
	int scidTransType;
}TDEVICET;


enum _imglist {
	IMG_NUM0,
	IMG_NUM1,
	IMG_NUM2,
	IMG_NUM3,
	IMG_NUM4,
	IMG_NUM5,
	IMG_NUM6,
	IMG_NUM7,
	IMG_NUM8,
	IMG_NUM9,
	IMG_BACKGROUND,
	IMG_TOTAL
};

typedef struct{
	TFRAME *list[IMG_TOTAL];
}TIMAGES;

typedef struct{
	TLIBANTPLUS *ant;
	TDCONFIG	*dcfg;
	TDEVICET	*dev;
	THRBUFFER	*rate;
	TCONNSTATES	*cstates;
	
	TMARQUEE	*marquee;
	TIMAGES		*images;
}THR;



int messageEventCb (const int chan, const int msgId, const unsigned char *payLoad, const size_t dataLength, void *uPtr);



#endif


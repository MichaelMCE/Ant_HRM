
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


#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <ctype.h>
#include "mylcdsetup.h"

#include "garminhr.h"
#include "console.h"


#define G19CB			(0)
#define RELEASE			(0)


#if !RELEASE
#include <conio.h>
#endif

#if G19CB
#include "../mylcd/src/g19/g19.h"
#endif




// when trying each of the ant+ keys
// each key is tried CONNECTION_TIMEOUT times before moving on to the next key
// then repeated CONNECTION_RETRIES times
#define CONNECTION_TIMEOUT	(2)		// per key retry on failure
#define CONNECTION_RETRIES	(5)		// all key retry
#define LIBUSB_DISCONNECT	(-5)
#define LIBUSB_TIMEOUT		(-116)



static int kb_hit ()
{
#if RELEASE
	return 0;
#else
	return kbhit();
#endif
}

static int getRunState (THR *hr)
{
	return hr->cstates->running;
}

static void setRunState (THR *hr, const int state)
{
	hr->cstates->running = state;
}

static int imageAdd (TIMAGES *img, const int imgIdx, const wchar_t *path)
{
	return ((img->list[imgIdx] = lNewImage(hw, path, DBPP)) != NULL);
}

static TFRAME *imageGet (TIMAGES *img, const int imgIdx)
{
	return img->list[imgIdx];
}

static void imageDelete (TIMAGES *img, const int imgIdx)
{
	lDeleteFrame(img->list[imgIdx]);
}

static int loadImageData (THR *hr, TIMAGES *imgs)
{
	wchar_t path[MAX_PATH];
	for (int i = 0; i < 10; i++){
		snwprintf(path, MAX_PATH, L"data/digits/%i.png", i);
		if (!imageAdd(imgs, i, path)){
			wprintf(L"image file missing or corrupt: '%s'\n", path);
			return 0;
		}
	}
	
	// this is allowed to fail so as to allow the user to have no background (white)
	imageAdd(imgs, IMG_BACKGROUND, L"data/background.png");
	return 1;
}

static int antOpen (THR *hr)
{
	int status = libantplus_Open(hr->ant, 0);
	if (status > 0)
		hr->cstates->openCt = 1;
	return status;
}

static void antClose (THR *hr)
{
	if (hr->cstates->openCt)
		libantplus_Close(hr->ant);
	hr->cstates->openCt = 0;
}

static int antGetOpenCt (THR *hr)
{
	return hr->cstates->openCt;
}

static void antCloseChannel (THR *hr)
{
	if (hr->cstates->channelStatus && antGetOpenCt(hr))
		libantplus_CloseChannel(hr->ant, hr->dcfg->channel);
}

THR *new_HR ()
{
	THR *hr = calloc(1, sizeof(THR));
	if (hr){
		hr->dcfg = calloc(1, sizeof(TDCONFIG));
		if (hr->dcfg){
			hr->cstates = calloc(1, sizeof(TCONNSTATES));
			if (hr->cstates){
				hr->rate = calloc(1, sizeof(THRBUFFER));
				if (hr->rate){
					hr->dev = calloc(1, sizeof(TDEVICET));
					if (hr->dev){
						hr->images = calloc(1, sizeof(TIMAGES));
						if (hr->images){
							hr->ant = libantplus_Init();
							if (hr->ant){
								setRunState(hr, 1);
								return hr;
							}
							free(hr->images);
						}
						free(hr->dev);
					}
					free(hr->rate);
				}
				free(hr->cstates);
			}
			free(hr->dcfg);
		}
		free(hr);
	}
	
	return NULL;
}

void delete_HR (THR *hr)
{
	if (hr){
		free(hr->ant);
		free(hr->dcfg);
		free(hr->cstates);
		free(hr->dev);
		free(hr->rate);
		free(hr->images);
		free(hr);
	}
}

static void setKey (THR *hr, const int keyIdx)
{
	hr->dcfg->keyIdx = keyIdx;
	hr->cstates->chanIdOnce = 0;
	hr->cstates->channelStatus = 0;
	hr->rate->low = 255;
	hr->rate->high = 0;
	libantplus_ResetSystem(hr->ant);
}

static void drawGraph (THR *hr, const unsigned int colourFil, const unsigned int colourPts)
{
	for (int x = 0; x < frame->width; x++){
		int x1 = frame->width-x-1;
		int y1 = frame->height-1 - hr->rate->bpm[HRBMP_BUFFERLENGTH-x-1];
		lSetPixel(frame, x1, y1, colourPts);
		lDrawLine(frame, x1, y1+1, x1, frame->height-1, colourFil);
	}
}

static void drawPulse (THR *hr, const int bmp)
{
	if (bmp >= 20 && bmp <= 99){
		TFRAME *d1 = imageGet(hr->images, bmp / 10);
		TFRAME *d2 = imageGet(hr->images, bmp % 10);
    						
		lDrawImage(d1, frame, (frame->width/2 - d1->width) + 3, (frame->height-d1->height)/2);
		lDrawImage(d2, frame, (frame->width/2) - 3, (frame->height - d2->height)/2);
	}else if (bmp >= 100 && bmp <= 240){
		TFRAME *d1 = imageGet(hr->images, bmp / 100);
		TFRAME *d2 = imageGet(hr->images, (bmp / 10) % 10);
		TFRAME *d3 = imageGet(hr->images, bmp % 10);

		lDrawImage(d1, frame, (((frame->width - d2->width)/2) - d1->width) + 6 , (frame->height - d1->height)/2);
		lDrawImage(d2, frame, (frame->width - d2->width)/2 , (frame->height - d2->height)/2);
		lDrawImage(d3, frame, (((frame->width + d2->width)/2)) - 6, (frame->height - d3->height)/2);
	}else{
		TFRAME *d0 = imageGet(hr->images, IMG_NUM0);
		lDrawImage(d0, frame, (frame->width - d0->width)/2 , (frame->height - d0->height)/2);
	}
}

static void drawBackground (THR *hr)
{
	TFRAME *background = imageGet(hr->images, IMG_BACKGROUND);
	if (background)
		lDrawImage(background, frame, 0, 0);
	else
		lClearFrame(frame);
}

static int getAve (unsigned char *bpm, const int len)
{
	int ct = 0;
	int sum = 0;
	
	for (int i = (HRBMP_BUFFERLENGTH-len-1); i < HRBMP_BUFFERLENGTH; i++){
		if (bpm[i]){
			sum += bpm[i];
			ct++;
		}
	}
	
	if (ct)
		return sum/(float)ct;
	else
		return 0;
}

static int getMode (unsigned char *bpm, const int len)
{
	unsigned char hist[HRBMP_BUFFERLENGTH];
	memset(hist, 0, HRBMP_BUFFERLENGTH);
		
	for (int i = (HRBMP_BUFFERLENGTH-len-1); i < HRBMP_BUFFERLENGTH; i++)
		hist[bpm[i]]++;
	
	int mode = 0;
	int most = 1;
	for (int i = 20; i < HRBMP_BUFFERLENGTH && i <= 240; i++){
		if (hist[i] > most){
			most = hist[i];
			mode = i;
		}
	}
	
	return mode;
}

void drawHeading (THR *hr, const THRBUFFER *rate)
{
	lSetBackgroundColour(hw, 0x00FFFFFF);
	lSetForegroundColour(hw, 0xFF000000);
	lSetRenderEffect(hw, LTR_OUTLINE2);
	lSetFilterAttribute(hw, LTR_OUTLINE2, 0, 220<<24 | 0xFFFFFF);
	const int font = LFTW_COMICSANS20;
	
	TFRAME *min = lNewString(hw, DBPP, 0, font, "  Min: %i", rate->low);
	TFRAME *ave = lNewString(hw, DBPP, 0, font, "Ave: %i\t\t\tMode: %i",
		getAve((unsigned char*)rate->bpm, frame->width),
		getMode((unsigned char*)rate->bpm, frame->width));
	//TFRAME *mode =lNewString(hw, DBPP, 0, font, "Mode: %i",getMode((unsigned char*)rate->bpm, frame->width));
	TFRAME *max = lNewString(hw, DBPP, 0, font, "Max: %i  ", rate->high);	
	
	lSetRenderEffect(hw, LTR_DEFAULT);

	lDrawImage(min, frame, 0, 0);
	lDrawImage(ave, frame, (frame->width - ave->width)/2 , 0);
	//lDrawImage(mode, frame, (frame->width/2) + 8, 0);
	lDrawImage(max, frame, frame->width - max->width, 0);
	lDeleteFrame(min);
	lDeleteFrame(ave);
	//lDeleteFrame(mode);
	lDeleteFrame(max);
}

static void doWindowLoop (THR *hr)
{
	if (hr->cstates->vDisplay){
		MSG message;
		while(PeekMessage(&message, NULL, 0, 0, PM_REMOVE)){
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
	}
}

// search for a hrm by trying each of the available keys in sequence
int doHRMSearch (THR *hr, const int attempts, const int timeout)
{

	int ct, readOK;
	int retries = attempts;
			
	do{
		int keyIdx = 0;
		do{			
			setKey(hr, keyIdx);
			ct = timeout;
									
			do{
				do{
					drawBackground(hr);
					marqueeDraw(hr, frame, hr->marquee);
					//drawHeading(hr, hr->rate);
					lRefresh(frame);
					lSleep(100);

					readOK = libantplus_HandleMessages(hr->ant);
					if (readOK == LIBUSB_DISCONNECT) return -1;
					
					if (kb_hit()) setRunState(hr, 0);					
				}while(readOK > 0 && !hr->dev->scidDeviceNumber && getRunState(hr));
			}while(ct-- && !hr->dev->scidDeviceNumber && getRunState(hr));
		}while(keyIdx++ < KEY_TOTAL && !hr->dev->scidDeviceNumber && getRunState(hr));
	}while(retries-- && !hr->dev->scidDeviceNumber && getRunState(hr));

	int found = hr->dev->scidDeviceNumber && getRunState(hr);
	if (found)
		dbprintf(hr, "Found HRM %i", hr->dev->scidDeviceNumber);
	return found;
}

// block until the ant+ usb dongle is found
int waitForDeviceConnect (THR *hr)
{
	int found = 0;
	
	do{
		doWindowLoop(hr);
		drawBackground(hr);
		dbprintf(hr, "searching for Ant+ stick (0x%.4X/0x%.4X)", ANTSTICK_VID, ANTSTICK_PID);
		
		found = libantplus_Discover(hr->ant, 0);
		if (found){
			dbprintf(hr, "Ant+ USB stick found");
			if (hr->ant->strings[0][0])
				dbprintf(hr, " %s ",hr->ant->strings[0]);
			if (hr->ant->strings[1][0] && hr->ant->strings[2][0])
				dbprintf(hr, " %s - %s ",hr->ant->strings[1], hr->ant->strings[2]);
			else if (hr->ant->strings[1][0])
				dbprintf(hr, " %s ",hr->ant->strings[1]);		
		}
		
		marqueeDraw(hr, frame, hr->marquee);			
		lRefresh(frame);
		
		if (kb_hit()) setRunState(hr, 0);
		if (!found && getRunState(hr))
			lSleep(900);
	}while(!found && getRunState(hr));

	return found;
}

#if G19CB

DWORD softkeyg19cb (int device, DWORD dwButtons, struct TMYLCDG19 *mylcdg19)
{
	TMYLCDG19 *g19 = (TMYLCDG19*)mylcdg19;
	if (g19){
		THR *hr = (THR*)g19->ptrUser;
		if (hr){
			if (LGLCDBUTTON_OK == dwButtons)
				setRunState(hr, 0);
		}
	}
	return 1;
}

static void assignG19Callback (THR *hr, void *softkeycb)
{

	lDISPLAY did = lDriverNameToID(hw, "G19", LDRV_DISPLAY);
	if (did){
		TMYLCDG19 *lcdg19;
		lGetDisplayOption(hw, did, lOPT_G19_STRUCT, (intptr_t*)&lcdg19);
		lcdg19->ptrUser = hr;
		lSetDisplayOption(hw, did, lOPT_G19_SOFTKEYCB, (intptr_t*)softkeycb);
	}else{
		hr->cstates->vDisplay = 1;
	}

}
#endif

int main (int ac, char *av[])
{
	if (!initMYLCD_USBD480())
		return EXIT_FAILURE;

	THR *hr = new_HR();
	if (hr == NULL){
		printf("startup failed allocating memory\n");
		return EXIT_FAILURE;
	}

	if (!loadImageData(hr, hr->images)){
		delete_HR(hr);
		return EXIT_FAILURE;
	}

	hr->marquee = marqueeNew(16, MARQUEE_LEFT);
#if G19CB
	assignG19Callback(hr, softkeyg19cb);
#else
	hr->cstates->vDisplay = 1;
#endif
	drawBackground(hr);
	lRefresh(frame);
		
	if (hr->ant){
		hr->dcfg->deviceNumber = 0;		// 0 
		hr->dcfg->deviceType = 0x78;	// 1
		hr->dcfg->transType = 0;		// 5
		hr->dcfg->channelType = 0;		
		hr->dcfg->networkNumber = 0;
		hr->dcfg->channel = 0;
		hr->dcfg->channelPeriod = /*4096*/8070;
		hr->dcfg->RFFreq = /*0x32*/0x39;
		hr->dcfg->searchTimeout = 255;
		hr->dcfg->searchWaveform = 0x53;
		libantplus_SetEventFunction(hr->ant, EVENTI_MESSAGE, messageEventCb, hr);
								
		unsigned char graph[DWIDTH];
		memset(graph, 0, sizeof(graph));
		int dfound = 0;		// is stick found and connected
		int hfound = 0; 	// has hrm been found and synced
		int readOK = 1;
		
		do{
			do{
				doWindowLoop(hr);
    					
				if (!dfound){
    				dfound = waitForDeviceConnect(hr);
					if (dfound && getRunState(hr)){
						antClose(hr);
						dfound = antOpen(hr);
					}
					hfound = 0;
				}

				if (!hfound && getRunState(hr)){
					hfound = doHRMSearch(hr, CONNECTION_RETRIES, CONNECTION_TIMEOUT);
					if (hfound == -1) dfound = 0;

				}else if (getRunState(hr)){
					drawBackground(hr);
					if (!marqueeDraw(hr, frame, hr->marquee))
						drawHeading(hr, hr->rate);
					drawGraph(hr, 80<<24 | 0xF01010, 160<<24 | 0xFF0000);
    				drawPulse(hr, hr->rate->currentBpm);
    				lRefresh(frame);
    				readOK = libantplus_HandleMessages(hr->ant);
				}
				if (kb_hit()) setRunState(hr, 0);
			}while(readOK > 0 && getRunState(hr));

    		if (readOK == LIBUSB_DISCONNECT && getRunState(hr)){
    			libantplus_ResetSystem(hr->ant);
    			antClose(hr);
    			hr->dev->scidDeviceNumber = 0;
    			hfound = 0;
    			dfound = 0;
    			readOK = 1;
    			
    		// if we're here then contact between HRM and skin could be poor
    		// or HRM may be poorly fitted
    		}else if (readOK < 0){
    			dbprintf(hr, "read error, retrying...");
				lSleep(100);
			}
		}while(getRunState(hr));
	
		antCloseChannel(hr);
		antClose(hr);
	}

	for (int i = 0; i < IMG_TOTAL; i++)
		imageDelete(hr->images, i);
	closeMYLCD_USBD480();
	marqueeDelete(hr->marquee);
	delete_HR(hr);
	
	return EXIT_SUCCESS;
}



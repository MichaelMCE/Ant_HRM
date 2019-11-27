
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
#include <inttypes.h>
#include <windows.h>
#include <mylcd.h>
#include "garminhr.h"

#define FONT					LFTW_B14

#define my_calloc calloc
#define my_malloc malloc
#define my_free free
#define my_memcpy memcpy


#ifndef	_ANSIDECL_H
#undef VA_OPEN
#undef VA_CLOSE
#undef VA_FIXEDARG

#define VA_OPEN(AP, VAR)	{ va_list AP; va_start(AP, VAR); { struct Qdmy
#define VA_CLOSE(AP)		} va_end(AP); }
#define VA_FIXEDARG(AP, T, N)	struct Qdmy
#endif



uint64_t getTime ()
{
	return (uint64_t)GetTickCount();
}

void shadowTextEnable (THWD *hw, const int colour, const unsigned char trans)
{
	lSetRenderEffect(hw, LTR_SHADOW);
	// set direction to South-East, shadow thickness to 5, offset by 1 pixel(s) and transparency to trans
	lSetFilterAttribute(hw, LTR_SHADOW, 0, LTRA_SHADOW_S|LTRA_SHADOW_E | LTRA_SHADOW_S5 | LTRA_SHADOW_OS(1) | LTRA_SHADOW_TR(trans));
	lSetFilterAttribute(hw, LTR_SHADOW, 1, colour);
}

void shadowTextDisable (THWD *hw)
{
	lSetRenderEffect(hw, LTR_DEFAULT);
}

void printSingleLineShadow (TFRAME *frame, const int font, const int x, const int y, const int colour, const char *str)
{
	TLPRINTR rt;
	int w, h;
	
	lSetForegroundColour(frame->hw, colour);

	lGetTextMetrics(frame->hw, str, 0, font, &w, &h);
	shadowTextEnable(frame->hw, 0xFF0000, 120);
	
	if (w >= frame->width){
		rt.bx1 = 0; rt.by1 = y;
		rt.bx2 = frame->width-1;
		rt.by2 = frame->height-1;
		rt.sx = rt.bx1; rt.sy = y;
		lPrintEx(frame, &rt, font, PF_CLIPDRAW|PF_RIGHTJUSTIFY, LPRT_CPY, str);
	}else{
		rt.bx1 = (frame->width-w)/2.0;
		rt.by1 = y;
		rt.bx2 = frame->width-1;
		rt.by2 = frame->height-1;
		rt.sx = rt.bx1; rt.sy = y;
		lPrintEx(frame, &rt, font, PF_CLIPWRAP|PF_CLIPDRAW, LPRT_CPY, str);
	}
	shadowTextDisable(frame->hw);
}

void marqueeDelete (TMARQUEE *marquee)
{
	if (marquee){
		WaitForSingleObject(marquee->hLock, INFINITE);
		CloseHandle(marquee->hLock);
		my_free(marquee->entry);
		my_free(marquee);
	}
}

TMARQUEE *marqueeNew (const int tLines, const unsigned int flags)
{
	TMARQUEE *marquee = my_calloc(1, sizeof(TMARQUEE));
	if (marquee){
		marquee->entry = (TMARQUEELINE*)my_calloc(tLines, sizeof(TMARQUEELINE));
		if (marquee->entry){
			marquee->total = tLines;
			marquee->hLock = CreateMutex(NULL, FALSE, NULL);
			marquee->flags = flags;
		}else{
			my_free(marquee);
			marquee = NULL;
		}
	}
	return marquee;
}

void marqueeAdd (THR *hr, TMARQUEE *mq, const char *str, const unsigned int timeout)
{
	if (mq && WaitForSingleObject(mq->hLock, INFINITE) == WAIT_OBJECT_0){
		my_memcpy(&mq->entry[0], &mq->entry[1], sizeof(TMARQUEELINE) * (mq->total-1));

		strncpy(mq->entry[mq->total-1].line, str, MAX_PATH-1);
		mq->entry[mq->total-1].time = timeout;
		mq->ready = 1;
		ReleaseMutex(mq->hLock);
	}
}

int marqueeDraw (THR *hr, TFRAME *frame, TMARQUEE *mq)
{
	if (!mq || !mq->ready) return 0;

	int x = 2;
	int y = 2;
	const int flags = PF_CLIPDRAW;

	if (WaitForSingleObject(mq->hLock, INFINITE) == WAIT_OBJECT_0){
		lSetBackgroundColour(frame->hw, 120<<24 | 0x000000);
		lSetForegroundColour(frame->hw, 0xFFFFFFFF);
		uint64_t fTime = getTime();
		
		for (int i = 0; i < mq->total; i++){
			if (mq->entry[i].time > fTime){
				if (mq->flags&MARQUEE_CENTER){
					printSingleLineShadow(frame, FONT, x, y, 0xFFFFFFFF, mq->entry[i].line);
				}else{
					TFRAME *str = lNewString(frame->hw, frame->bpp, flags, FONT, mq->entry[i].line);
					if (str){
						lDrawLine(frame, x-1, y, x-1, y+str->height-1, 120<<24 | 0x000000);
						lDrawImage(str, frame, x, y);
						lDeleteFrame(str);
					}
				}
				y += 16;
			}
		}
		mq->ready = (y > 2);
		ReleaseMutex(mq->hLock);
	}
	return (y > 2);
}

static void addConsoleline (THR *hr, const char *str)
{
	marqueeAdd(hr, hr->marquee, str, getTime()+(10*1000));
}

int _vasprintf (char **result, const char *format, va_list *args)
{
	
  const char *p = format;
  /* Add one to make sure that it is never zero, which might cause malloc
     to return NULL.  */
  int total_width = strlen(format) + sizeof(char);
  va_list ap;

  my_memcpy(&ap, args, sizeof(va_list));

  while (*p != '\0')
    {
      if (*p++ == '%')
        {
          while (strchr("-+ #0", *p))
            ++p;
          if (*p == '*')
            {
              ++p;
              total_width += abs(va_arg(ap, int));
            }
          else
            total_width += strtoul(p, (char**)&p, 10);
          if (*p == '.')
            {
              ++p;
              if (*p == '*')
                {
                  ++p;
                  total_width += abs(va_arg(ap, int));
                }
              else
                total_width += strtoul(p, (char**)&p, 10);
            }
          while(strchr("hl", *p))
            ++p;
          /* Should be big enough for any format specifier except %s.  */
          total_width += 30;
          switch (*p)
            {
            case 'd':
            case 'i':
            case 'o':
            case 'u':
            case 'x':
            case 'X':
            case 'c':
              (void) va_arg(ap, int);
              break;
            case 'f':
            case 'e':
            case 'E':
            case 'g':
            case 'G':
              (void) va_arg(ap, double);
              break;
            case 's':
              total_width += strlen(va_arg(ap, char *));
              break;
            case 'p':
            case 'n':
              (void) va_arg(ap, char *);
              break;
            }
        }
    }

  if (!total_width) return 0;
  *result = my_calloc(sizeof(char), 1+total_width);
  
  if (*result != NULL)
    return vsnprintf(*result, total_width, format, *args);
  else
    return 0;
}

void dbprintf (THR *hr, const char *str, ...)
{
	char *buffer = NULL;
	
	VA_OPEN(ap, str);
	_vasprintf(&buffer, str, &ap);
	VA_CLOSE(ap);
	
	if (buffer){
		addConsoleline(hr, buffer);
		my_free(buffer);
	}
}



CC=gcc
x64 := 0

ifeq ($(x64),1)
MDIR=x64
BITMODE = -m64
RESTARGET = 
ARCH = -march=native -mtune=native
else
MDIR=x32
BITMODE = -m32
RESTARGET = --target=pe-i386
ARCH = -march=i686 -mtune=i686
endif


INCLUDE_DIR= -I"../include/" -I"include/" 
LIB_DIR = -L"lib/$(MDIR)/" -Lepg/sdk/lib/ -L"lib" 
WINVER = -D_WIN32 -D_WIN32_WINNT=0x0701 -DWINVER=0x0701 -D__WIN32__=1 
SSEMMX = -DUSE_MMX -DHAVE_MMX -DHAVE_MMX1 -DUSE_MMX1 -mmmx -msse -mfpmath=sse,387 
GLOP = -ftree-vectorize -floop-interchange -floop-strip-mine -floop-block -ftree-vectorizer-verbose=0
GCSE = -fgcse-las -fgcse-sm -fgcse-lm 
SCHED = -fmodulo-sched-allow-regmoves -fmodulo-sched
CFLAGSEXTRA = -funroll-loops -finline-functions -fomit-frame-pointer -Wno-strict-aliasing
CFLAGSOP = -Wall -O2 -std=gnu11 



CFLAGS = -flto $(ARCH) $(BITMODE) $(INCLUDE_DIR) $(GLOP) $(GCSE) $(WINVER) $(SSEMMX) $(SCHED) $(CFLAGSEXTRA) $(CFLAGSOP) -pipe -s 
LIBS = -flto $(BITMODE) $(LIB_DIR) -static-libgcc -lm -lusb -lmylcddll

EXAMPLES=ANT_HRM.exe

ANT_HRM.exe: anthrm.o console.o garminhr.o libantplus.o

%.exe: 
	$(CC) -o $@ $^ $(LIBS) -mwindows
	strip.exe $@

%.o: %.c $(DEPS) 
	$(CC) $(CFLAGS) -c -o $@ $< 

clean:
	rm -f *.exe *.o *.bak


all: $(EXAMPLES)

### PHONY define
.PHONY: all all-before all-after clean clean-custom




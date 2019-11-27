@echo off

call gccpath.bat
set outexe=Ant_HRM.exe

rem if x86_64 add -march=k8 -mtune=k8
rem when on windows, add -mwindows for a consoleless app. remove to enable console mode

gcc -m32 -Wall -Werror -L"lib/x32/" -I"./include/" -mwindows -O2 -std=gnu99 anthrm.c console.c garminhr.c libantplus.c -lusb -lmylcddll -o %outexe%

strip %outexe%

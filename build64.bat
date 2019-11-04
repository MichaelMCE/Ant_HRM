@echo off


call gccpath.bat
set outexe=Ant_HRM.exe

rem if x86_64 add -march=k8 -mtune=k8
rem when on windows, add -mwindows for a consoleless app

rem gcc -Wall -Werror -L"../mylcd/lib/" -I"../mylcd/include/" -mwindows -march=k8 -mtune=k8 -O2 -std=gnu99 anthrm.c console.c garminhr.c libantplus.c -lusb -lmylcd -o %outexe%
gcc -m64 -Wall -Werror -L"lib/x64/" -I"include/x64/" -march=k8 -mtune=k8 -mwindows -O2 -std=gnu99 anthrm.c console.c garminhr.c libantplus.c -lusb -lmylcddll -o %outexe%
strip %outexe%

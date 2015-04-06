qmake iriclib.pro
nmake clean
nmake debug release
qmake iriclib_ifort.pro
nmake clean
nmake debug release
move debug\iriclib_ifort.lib debug\ifort\iriclib.lib
move release\iriclib_ifort.lib release\ifort\iriclib.lib

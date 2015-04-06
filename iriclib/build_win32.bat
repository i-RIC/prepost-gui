qmake iriclib.pro
nmake clean
nmake debug release
qmake iriclib_ifort.pro
nmake clean
nmake debug release
del bins\iriclib_ia32_ifort_debug.lib
del bins\iriclib_ia32_ifort_release.lib
move debug\iriclib_ifort.lib bins\iriclib_ia32_ifort_debug.lib
move release\iriclib_ifort.lib bins\iriclib_ia32_ifort_release.lib

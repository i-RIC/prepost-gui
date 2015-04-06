qmake iriclib.pro
nmake clean
nmake debug release
qmake iriclib_ifort.pro
nmake clean
nmake debug release
del bins\iriclib_x64_ifort_debug.lib
del bins\iriclib_x64_ifort_release.lib
move debug\iriclib_ifort.lib bins\iriclib_x64_ifort_debug.lib
move release\iriclib_ifort.lib bins\iriclib_x64_ifort_release.lib

qmake iriclib_ifort10.pro
nmake clean
nmake debug release
del bins\iriclib_x64_ifort10_debug.lib
del bins\iriclib_x64_ifort10_release.lib
move debug\iriclib_ifort10.lib bins\iriclib_x64_ifort10_debug.lib
move release\iriclib_ifort10.lib bins\iriclib_x64_ifort10_release.lib

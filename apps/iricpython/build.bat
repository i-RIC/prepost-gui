del *.dll
del *.lib
del *.obj
del *.pyd
"C:\Program Files (x86)\swig\swig.exe" -python -c++ iricpython.i
cl /LD iricpython_wrap.cxx -I"E:\Python35\include" -I"..\..\libs\iricpython" -I"E:\Qt\5.3\msvc2013_64_opengl\include" "E:\Python35\libs\python35.lib" "..\..\libs\iricpython\release\iricPython.lib"
rename iricpython_wrap.dll _iric.pyd
del iricpython_wrap.lib
del iricpython_wrap.obj

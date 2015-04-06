gcc -g -c -I ../cgnslib iriclib.c
g++ -g -c -I ../cgnslib iriclib_bstream.cpp
g++ -g -c -I ../cgnslib iriclib_geo.cpp
g++ -g -c -I ../cgnslib iriclib_riversurvey.cpp
g++ -c -I ../cgnslib iriclib_polygon.cpp
gcc -g -c -I ../cgnslib -DLOWERCASE_ iric_ftoc.c
ar cr iriclib.a iriclib.o iriclib_bstream.o iriclib_geo.o iriclib_riversurvey.o iriclib_polygon.o iric_ftoc.o
mv iriclib.a debug/gfortran

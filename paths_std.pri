CONFIG(debug, debug|release) {
	# gdal
	LIBS += -L"D:/iRIC/dev_vc2019_64/iricdev-2019/lib/install/gdal-3.0.4/debug/lib"

	# vtk
	LIBS += -L"D:/iRIC/dev_vc2019_64/iricdev-2019/lib/install/vtk-8.2.0/debug/lib"

	# cgnslib
	LIBS += -L"D:/iRIC/dev_vc2019_64/iricdev-2019/lib/install/cgnslib-4.1.1-patch1/debug/lib"

	# iriclib
	LIBS += -L"D:/iRIC/dev_vc2019_64/iricdev-2019/lib/install/iriclib-0.2.7/debug/lib"

	# Qwt
	LIBS += -L"D:/iRIC/dev_vc2019_64/iricdev-2019/lib/install/Qwt-6.1.5/lib"

	# proj.4
	LIBS += -L"D:/iRIC/dev_vc2019_64/iricdev-2019/lib/install/proj-7.0.1/debug"

	# shapefile
	LIBS += -L"D:/iRIC/dev_vc2019_64/iricdev-2019/lib/install/shapelib-1.5.0/debug"

	# hdf5
	LIBS += -L"D:/iRIC/dev_vc2019_64/iricdev-2019/lib/install/hdf5-1.8.21/debug/lib"

	# netcdf
	LIBS += -L"D:/iRIC/dev_vc2019_64/iricdev-2019/lib/install/netcdf-c-4.7.4/debug/lib"

	# geos
	LIBS += -L"D:/iRIC/dev_vc2019_64/iricdev-2019/lib/install/geos-3.4.3/debug/lib"

	# yaml-cpp
	LIBS += -L"D:/iRIC/dev_vc2019_64/iricdev-2019/lib/install/yaml-cpp-0.6.3/debug/lib"

	# udunits
	LIBS += -L"D:/iRIC/dev_vc2019_64/iricdev-2019/lib/install/udunits-2.2.26/debug/lib"

	# libpng
	LIBS += -L"D:/iRIC/dev_vc2019_64/iricdev-2019/lib/install/libpng-1.6.37/debug/lib"

	# poco
	LIBS += -L"D:/iRIC/dev_vc2019_64/iricdev-2019/lib/install/poco-1.9.4/lib"
}
else {
	# gdal
	LIBS += -L"D:/iRIC/dev_vc2019_64/iricdev-2019/lib/install/gdal-3.0.4/release/lib"

	# vtk
	LIBS += -L"D:/iRIC/dev_vc2019_64/iricdev-2019/lib/install/vtk-8.2.0/release/lib"

	# cgnslib
	LIBS += -L"D:/iRIC/dev_vc2019_64/iricdev-2019/lib/install/cgnslib-4.1.1-patch1/release/lib"

	# iriclib
	LIBS += -L"D:/iRIC/dev_vc2019_64/iricdev-2019/lib/install/iriclib-0.2.7/release/lib"

	# Qwt
	LIBS += -L"D:/iRIC/dev_vc2019_64/iricdev-2019/lib/install/qwt-6.1.5/lib"

	# proj.4
	LIBS += -L"D:/iRIC/dev_vc2019_64/iricdev-2019/lib/install/proj-7.0.1/release/lib"

	# shapefile
	LIBS += -L"D:/iRIC/dev_vc2019_64/iricdev-2019/lib/install/shapelib-1.5.0/release"

	# hdf5
	LIBS += -L"D:/iRIC/dev_vc2019_64/iricdev-2019/lib/install/hdf5-1.8.21/release/lib"

	# netcdf
	LIBS += -L"D:/iRIC/dev_vc2019_64/iricdev-2019/lib/install/netcdf-c-4.7.4/release/lib"

	# geos
	LIBS += -L"D:/iRIC/dev_vc2019_64/iricdev-2019/lib/install/geos-3.4.3/release/lib"

	# yaml-cpp
	LIBS += -L"D:/iRIC/dev_vc2019_64/iricdev-2019/lib/install/yaml-cpp-0.6.3/release/lib"

	# udunits
	LIBS += -L"D:/iRIC/dev_vc2019_64/iricdev-2019/lib/install/udunits-2.2.26/release/lib"

	# libpng
	LIBS += -L"D:/iRIC/dev_vc2019_64/iricdev-2019/lib/install/libpng-1.6.37/release/lib"

	# poco
	LIBS += -L"D:/iRIC/dev_vc2019_64/iricdev-2019/lib/install/poco-1.9.4/lib"
}

INCLUDEPATH += .

# gdal
INCLUDEPATH += "D:/iRIC/dev_vc2019_64/iricdev-2019/lib/install/gdal-3.0.4/debug/include"

# vtk
INCLUDEPATH += "D:/iRIC/dev_vc2019_64/iricdev-2019/lib/install/vtk-8.2.0/debug/include/vtk-8.2"

# hdf5
INCLUDEPATH += "D:/iRIC/dev_vc2019_64/iricdev-2019/lib/install/hdf5-1.8.21/release/include"

# cgnslib
INCLUDEPATH += "D:/iRIC/dev_vc2019_64/iricdev-2019/lib/install/cgnslib-4.1.1-patch1/debug/include"

# iriclib
INCLUDEPATH += "D:/iRIC/dev_vc2019_64/iricdev-2019/lib/install/iriclib-0.2.7/debug/include"

# Qwt
INCLUDEPATH += "D:/iRIC/dev_vc2019_64/iricdev-2019/lib/install/qwt-6.1.5/include"

# shapelib
INCLUDEPATH += "D:/iRIC/dev_vc2019_64/iricdev-2019/lib/install/shapelib-1.5.0/release"

# proj.4
INCLUDEPATH += "D:/iRIC/dev_vc2019_64/iricdev-2019/lib/install/proj-7.0.1/release/include"

# netcdf
INCLUDEPATH += "D:/iRIC/dev_vc2019_64/iricdev-2019/lib/install/netcdf-c-4.7.4/release/include"

# geos
INCLUDEPATH += "D:/iRIC/dev_vc2019_64/iricdev-2019/lib/install/geos-3.4.3/release/include"

# yaml-cpp
INCLUDEPATH += "D:/iRIC/dev_vc2019_64/iricdev-2019/lib/install/yaml-cpp-0.6.3/release/include"

# boost
INCLUDEPATH += "D:/iRIC/dev_vc2013_64/iricdev/lib/src/boost-1.73.0"

# udunits
INCLUDEPATH += "D:/iRIC/dev_vc2019_64/iricdev-2019/lib/install/udunits-2.2.26/release/include"

# libpng
INCLUDEPATH += "D:/iRIC/dev_vc2019_64/iricdev-2019/lib/install/libpng-1.6.37/release/include"

# poco
INCLUDEPATH += "D:/iRIC/dev_vc2019_64/iricdev-2019/lib/install/poco-1.9.4/include"

#internal libs
INCLUDEPATH += "$$PWD/libs"

CONFIG += c++11

target.path = /usr/local/iRIC
INSTALLS += target

VTK_MAJOR_VERSION = 8
VTK_MINOR_VERSION = 2

VTK_MAJ_MIN = $${VTK_MAJOR_VERSION}.$${VTK_MINOR_VERSION}

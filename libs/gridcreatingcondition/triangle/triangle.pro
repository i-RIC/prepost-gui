TARGET = iricGccTriangle
TEMPLATE = lib
INCLUDEPATH += ../..

DEFINES += GCC_TRIANGLE_LIBRARY

DEFINES += ANSI_DECLARATORS

include( ../../../paths.pri )

QT += widgets xml

######################
# Internal libraries #
######################

# iricGeoio

win32 {
	CONFIG(debug, debug|release) {
		LIBS += -L"../../geoio/debug"
	} else {
		LIBS += -L"../../geoio/release"
	}
}
unix {
	LIBS += -L"../../geoio"
}
LIBS += -liricGeoio

# iricMisc

win32 {
	CONFIG(debug, debug|release) {
		LIBS += -L"../../misc/debug"
	} else {
		LIBS += -L"../../misc/release"
	}
}
unix {
	LIBS += -L"../../misc"
}
LIBS += -liricMisc

# iricTriangle

win32 {
	CONFIG(debug, debug|release) {
		LIBS += -L"../../triangle/debug"
	} else {
		LIBS += -L"../../triangle/release"
	}
}
unix {
	LIBS += -L"../../triangle"
}
LIBS += -liricTriangle

# iricGuibase

win32 {
	CONFIG(debug, debug|release) {
		LIBS += -L"../../guibase/debug"
	} else {
		LIBS += -L"../../guibase/release"
	}
}
unix {
	LIBS += -L"../../guibase"
}
LIBS += -liricGuibase

# iricGuicore

win32 {
	CONFIG(debug, debug|release) {
		LIBS += -L"../../guicore/debug"
	} else {
		LIBS += -L"../../guicore/release"
	}
}
unix {
	LIBS += -L"../../guicore"
}
LIBS += -liricGuicore

######################
# External libraries #
######################

# VTK

LIBS += \
	-lvtkCommonComputationalGeometry-6.1 \
	-lvtkCommonCore-6.1 \
	-lvtkCommonDataModel-6.1 \
	-lvtkRenderingCore-6.1 \
	-lvtkRenderingLabel-6.1

# geos

LIBS += -lgeos

# Post-Build Event
win32 {
	QMAKE_POST_LINK += copy $(TargetPath) $(SolutionDir)\\libdlls\\$(Configuration)
}

# Input
HEADERS += gcc_triangle_global.h \
           gridcreatingconditioncreatortriangle.h \
           gridcreatingconditiontriangle.h \
           gridcreatingconditiontriangleabstractline.h \
           gridcreatingconditiontriangleabstractpolygon.h \
           gridcreatingconditiontriangledivisionline.h \
           gridcreatingconditiontrianglegridregionpolygon.h \
           gridcreatingconditiontriangleholepolygon.h \
           gridcreatingconditiontriangleremeshdialog.h \
           gridcreatingconditiontriangleremeshpolygon.h \
           gridcreatingconditiontrianglesettingdialog.h \
           private/gridcreatingconditiontriangle_adddivisionlinecommand.h \
           private/gridcreatingconditiontriangle_addholepolygoncommand.h \
           private/gridcreatingconditiontriangle_addpolygonvertexcommand.h \
           private/gridcreatingconditiontriangle_addpolylinevertexcommand.h \
           private/gridcreatingconditiontriangle_addremeshpolygoncommand.h \
           private/gridcreatingconditiontriangle_definepolygonnewpointcommand.h \
           private/gridcreatingconditiontriangle_definepolylinenewpointcommand.h \
           private/gridcreatingconditiontriangle_editpolygoncoordinatescommand.h \
           private/gridcreatingconditiontriangle_editpolylinecoordinatescommand.h \
           private/gridcreatingconditiontriangle_finishpolygondefiningcommand.h \
           private/gridcreatingconditiontriangle_finishpolylinedefiningcommand.h \
           private/gridcreatingconditiontriangle_movepolygoncommand.h \
           private/gridcreatingconditiontriangle_movepolygonvertexcommand.h \
           private/gridcreatingconditiontriangle_movepolylinecommand.h \
           private/gridcreatingconditiontriangle_movepolylinevertexcommand.h \
           private/gridcreatingconditiontriangle_polygoncoordinateseditor.h \
           private/gridcreatingconditiontriangle_polylinecoordinateseditor.h \
           private/gridcreatingconditiontriangle_removepolygonvertexcommand.h \
           private/gridcreatingconditiontriangle_removepolylinevertexcommand.h
FORMS += \
         gridcreatingconditiontriangleremeshdialog.ui \
         gridcreatingconditiontrianglesettingdialog.ui
SOURCES += gridcreatingconditioncreatortriangle.cpp \
           gridcreatingconditiontriangle.cpp \
           gridcreatingconditiontriangle_creategrid.cpp \
           gridcreatingconditiontriangleabstractline.cpp \
           gridcreatingconditiontriangleabstractpolygon.cpp \
           gridcreatingconditiontriangledivisionline.cpp \
           gridcreatingconditiontrianglegridregionpolygon.cpp \
           gridcreatingconditiontriangleholepolygon.cpp \
           gridcreatingconditiontriangleremeshdialog.cpp \
           gridcreatingconditiontriangleremeshpolygon.cpp \
           gridcreatingconditiontrianglesettingdialog.cpp \
           private/gridcreatingconditiontriangle_adddivisionlinecommand.cpp \
           private/gridcreatingconditiontriangle_addholepolygoncommand.cpp \
           private/gridcreatingconditiontriangle_addpolygonvertexcommand.cpp \
           private/gridcreatingconditiontriangle_addpolylinevertexcommand.cpp \
           private/gridcreatingconditiontriangle_addremeshpolygoncommand.cpp \
           private/gridcreatingconditiontriangle_definepolygonnewpointcommand.cpp \
           private/gridcreatingconditiontriangle_definepolylinenewpointcommand.cpp \
           private/gridcreatingconditiontriangle_editpolygoncoordinatescommand.cpp \
           private/gridcreatingconditiontriangle_editpolylinecoordinatescommand.cpp \
           private/gridcreatingconditiontriangle_finishpolygondefiningcommand.cpp \
           private/gridcreatingconditiontriangle_finishpolylinedefiningcommand.cpp \
           private/gridcreatingconditiontriangle_movepolygoncommand.cpp \
           private/gridcreatingconditiontriangle_movepolygonvertexcommand.cpp \
           private/gridcreatingconditiontriangle_movepolylinecommand.cpp \
           private/gridcreatingconditiontriangle_movepolylinevertexcommand.cpp \
           private/gridcreatingconditiontriangle_polygoncoordinateseditor.cpp \
           private/gridcreatingconditiontriangle_polylinecoordinateseditor.cpp \
           private/gridcreatingconditiontriangle_removepolygonvertexcommand.cpp \
           private/gridcreatingconditiontriangle_removepolylinevertexcommand.cpp
TRANSLATIONS += languages/iricGccTriangle_es_ES.ts \
                languages/iricGccTriangle_fr_FR.ts \
                languages/iricGccTriangle_id_ID.ts \
                languages/iricGccTriangle_ja_JP.ts \
                languages/iricGccTriangle_ko_KR.ts \
                languages/iricGccTriangle_ru_RU.ts \
                languages/iricGccTriangle_th_TH.ts \
                languages/iricGccTriangle_vi_VN.ts \
                languages/iricGccTriangle_zh_CN.ts

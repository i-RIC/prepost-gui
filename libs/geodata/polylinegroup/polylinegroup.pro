TARGET = iricGdPolyLineGroup
TEMPLATE = lib
INCLUDEPATH += ../..

DEFINES += GD_POLYLINEGROUP_LIBRARY

DEFINES += ANSI_DECLARATORS

include( ../../../paths.pri )

QT += widgets xml

######################
# Internal libraries #
######################

# iricGdPolydata

win32 {
        CONFIG(debug, debug|release) {
                LIBS += -L"../polydata/debug"
        } else {
                LIBS += -L"../polydata/release"
        }
}
unix {
        LIBS += -L"../polydata"
}
LIBS += -liricGdPolydata

# iricGdPolyLine

win32 {
        CONFIG(debug, debug|release) {
                LIBS += -L"../polyline/debug"
        } else {
                LIBS += -L"../polyline/release"
        }
}
unix {
        LIBS += -L"../polyline"
}
LIBS += -liricGdPolyline

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

# iricPolydataGroup

win32 {
	CONFIG(debug, debug|release) {
		LIBS += -L"../polydatagroup/debug"
	} else {
		LIBS += -L"../polydatagroup/release"
	}
}
unix {
	LIBS += -L"../polydatagroup"
}
LIBS += -liricGdPolydataGroup

######################
# External libraries #
######################

# VTK

LIBS += \
	-lvtkCommonCore-6.1 \
	-lvtkCommonDataModel-6.1 \
	-lvtkRenderingCore-6.1

# shapelib

win32{
LIBS += -lshapelib_i
}

# geos

LIBS += -lgeos

# Post-Build Event
win32 {
	QMAKE_POST_LINK += copy $(TargetPath) $(SolutionDir)\\libdlls\\$(Configuration)
}

# Input
HEADERS += gd_polylinegroup_global.h \
           geodatapolylinegroup.h \
           geodatapolylinegroupcellmappert.h \
           geodatapolylinegroupcreator.h \
           geodatapolylinegroupcreatort.h \
           geodatapolylinegroupintegercreator.h \
           geodatapolylinegroupnodemappert.h \
           geodatapolylinegrouppolyline.h \
           geodatapolylinegrouprealcreator.h \
           geodatapolylinegroupshpexporter.h \
           geodatapolylinegroupshpimporter.h \
           private/geodatapolylinegroup_impl.h \
           private/geodatapolylinegroupcellmappert_detail.h \
           private/geodatapolylinegroupcreatort_detail.h \
           private/geodatapolylinegroupnodemappert_detail.h \
           private/geodatapolylinegrouppolyline_impl.h
FORMS +=
SOURCES += geodatapolylinegroup.cpp \
           geodatapolylinegroupcreator.cpp \
           geodatapolylinegroupintegercreator.cpp \
           geodatapolylinegrouppolyline.cpp \
           geodatapolylinegrouprealcreator.cpp \
           geodatapolylinegroupshpexporter.cpp \
           geodatapolylinegroupshpimporter.cpp \
           private/geodatapolylinegroup_impl.cpp
TRANSLATIONS += languages/iricGdPolyLineGroup_es_ES.ts \
                languages/iricGdPolyLineGroup_fr_FR.ts \
                languages/iricGdPolyLineGroup_id_ID.ts \
                languages/iricGdPolyLineGroup_ja_JP.ts \
                languages/iricGdPolyLineGroup_ko_KR.ts \
                languages/iricGdPolyLineGroup_ru_RU.ts \
                languages/iricGdPolyLineGroup_th_TH.ts \
                languages/iricGdPolyLineGroup_vi_VN.ts \
                languages/iricGdPolyLineGroup_zh_CN.ts

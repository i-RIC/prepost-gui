TARGET = iricGdPointGroup
TEMPLATE = lib
INCLUDEPATH += ../..

DEFINES += GD_POINTGROUP_LIBRARY

DEFINES += ANSI_DECLARATORS

include( ../../../paths.pri )

QT += widgets xml

######################
# Internal libraries #
######################

# iricGdPoint

win32 {
        CONFIG(debug, debug|release) {
                LIBS += -L"../point/debug"
        } else {
                LIBS += -L"../point/release"
        }
}
unix {
        LIBS += -L"../point"
}
LIBS += -liricGdPoint

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
HEADERS += gd_pointgroup_global.h \
           geodatapointgroup.h \
           geodatapointgroupcellmappert.h \
           geodatapointgroupcreator.h \
           geodatapointgroupcreatort.h \
           geodatapointgroupintegercreator.h \
           geodatapointgroupnodemappert.h \
           geodatapointgrouppoint.h \
           geodatapointgrouprealcreator.h \
           geodatapointgroupshpexporter.h \
           geodatapointgroupshpimporter.h \
           private/geodatapointgroup_impl.h \
           private/geodatapointgroupcellmappert_detail.h \
           private/geodatapointgroupcreatort_detail.h \
           private/geodatapointgroupnodemappert_detail.h \
           private/geodatapointgrouppoint_impl.h
FORMS +=
SOURCES += geodatapointgroup.cpp \
           geodatapointgroupcreator.cpp \
           geodatapointgroupintegercreator.cpp \
           geodatapointgrouppoint.cpp \
           geodatapointgrouprealcreator.cpp \
           geodatapointgroupshpexporter.cpp \
           geodatapointgroupshpimporter.cpp \
           private/geodatapointgroup_impl.cpp
TRANSLATIONS += languages/iricGdPointGroup_es_ES.ts \
                languages/iricGdPointGroup_fr_FR.ts \
                languages/iricGdPointGroup_id_ID.ts \
                languages/iricGdPointGroup_ja_JP.ts \
                languages/iricGdPointGroup_ko_KR.ts \
                languages/iricGdPointGroup_ru_RU.ts \
                languages/iricGdPointGroup_th_TH.ts \
                languages/iricGdPointGroup_vi_VN.ts \
                languages/iricGdPointGroup_zh_CN.ts

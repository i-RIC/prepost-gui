TARGET = iricGdPolygon
TEMPLATE = lib
INCLUDEPATH += ../..

DEFINES += GD_POLYGON_LIBRARY

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
	-lvtkCommonCore-6.1 \
	-lvtkCommonDataModel-6.1 \
	-lvtkRenderingCore-6.1

# iriclib

LIBS += -liriclib

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
HEADERS += gd_polygon_global.h \
           geodatapolygon.h \
           geodatapolygonabstractpolygon.h \
           geodatapolygoncellmappersetting.h \
           geodatapolygoncellmappert.h \
           geodatapolygoncolorsettingdialog.h \
           geodatapolygoncreator.h \
           geodatapolygoncreatort.h \
           geodatapolygonholepolygon.h \
           geodatapolygonimporter.h \
           geodatapolygonimportersettingdialog.h \
           geodatapolygonintegercreator.h \
           geodatapolygonnodemappert.h \
           geodatapolygonproxy.h \
           geodatapolygonrealcreator.h \
           geodatapolygonregionpolygon.h \
           geodatapolygonshapeexporter.h \
           geodatapolygontrianglethread.h \
           private/geodatapolygon_addholepolygoncommand.h \
           private/geodatapolygon_addvertexcommand.h \
           private/geodatapolygon_coordinateseditor.h \
           private/geodatapolygon_editcoordinatescommand.h \
           private/geodatapolygon_editpropertycommand.h \
           private/geodatapolygon_editvaluecommand.h \
           private/geodatapolygon_finishpolygondefinitioncommand.h \
           private/geodatapolygon_impl.h \
           private/geodatapolygon_modifyabstractpolygoncommand.h \
           private/geodatapolygon_movepolygoncommand.h \
           private/geodatapolygon_movevertexcommand.h \
           private/geodatapolygon_pushnewpointcommand.h \
           private/geodatapolygon_removevertexcommand.h \
           private/geodatapolygonabstractpolygon_impl.h \
           private/geodatapolygoncellmappert_detail.h \
           private/geodatapolygoncreatort_detail.h \
           private/geodatapolygonnodemappert_detail.h
FORMS += geodatapolygoncolorsettingdialog.ui \
         geodatapolygonimportersettingdialog.ui
SOURCES += geodatapolygon.cpp \
           geodatapolygonabstractpolygon.cpp \
           geodatapolygoncellmappersetting.cpp \
           geodatapolygoncolorsettingdialog.cpp \
           geodatapolygoncreator.cpp \
           geodatapolygonholepolygon.cpp \
           geodatapolygonimporter.cpp \
           geodatapolygonimportersettingdialog.cpp \
           geodatapolygonintegercreator.cpp \
           geodatapolygonproxy.cpp \
           geodatapolygonrealcreator.cpp \
           geodatapolygonregionpolygon.cpp \
           geodatapolygonshapeexporter.cpp \
           geodatapolygontrianglethread.cpp \
           private/geodatapolygon_addholepolygoncommand.cpp \
           private/geodatapolygon_addvertexcommand.cpp \
           private/geodatapolygon_coordinateseditor.cpp \
           private/geodatapolygon_editcoordinatescommand.cpp \
           private/geodatapolygon_editpropertycommand.cpp \
           private/geodatapolygon_editvaluecommand.cpp \
           private/geodatapolygon_finishpolygondefinitioncommand.cpp \
           private/geodatapolygon_modifyabstractpolygoncommand.cpp \
           private/geodatapolygon_movepolygoncommand.cpp \
           private/geodatapolygon_movevertexcommand.cpp \
           private/geodatapolygon_pushnewpointcommand.cpp \
           private/geodatapolygon_removevertexcommand.cpp
TRANSLATIONS += languages/iricGdPolygon_es_ES.ts \
                languages/iricGdPolygon_fr_FR.ts \
                languages/iricGdPolygon_id_ID.ts \
                languages/iricGdPolygon_ja_JP.ts \
                languages/iricGdPolygon_ko_KR.ts \
                languages/iricGdPolygon_ru_RU.ts \
                languages/iricGdPolygon_th_TH.ts \
                languages/iricGdPolygon_vi_VN.ts \
                languages/iricGdPolygon_zh_CN.ts

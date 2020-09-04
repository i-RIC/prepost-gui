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
	QMAKE_POST_LINK += IF NOT EXIST $(SolutionDir)\\libdlls\\$(Configuration) mkdir $(SolutionDir)\\libdlls\\$(Configuration) && copy $(TargetPath) $(SolutionDir)\\libdlls\\$(Configuration)
}

# Input
HEADERS += gd_polygon_global.h \
           geodatapolygon.h \
           geodatapolygonabstractpolygon.h \
           geodatapolygoncellmappersetting.h \
           geodatapolygoncellmappert.h \
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
           private/geodatapolygon_editvaluecommand.cpp \
           private/geodatapolygon_finishpolygondefinitioncommand.cpp \
           private/geodatapolygon_modifyabstractpolygoncommand.cpp \
           private/geodatapolygon_movepolygoncommand.cpp \
           private/geodatapolygon_movevertexcommand.cpp \
           private/geodatapolygon_pushnewpointcommand.cpp \
           private/geodatapolygon_removevertexcommand.cpp
TRANSLATIONS += languages/iricGdPolygon_ar_EG.ts \
                languages/iricGdPolygon_bg_BG.ts \
                languages/iricGdPolygon_bs_BA.ts \
                languages/iricGdPolygon_ca_ES.ts \
                languages/iricGdPolygon_cs_CZ.ts \
                languages/iricGdPolygon_da_DK.ts \
                languages/iricGdPolygon_de_DE.ts \
                languages/iricGdPolygon_el_GR.ts \
                languages/iricGdPolygon_es_ES.ts \
                languages/iricGdPolygon_et_EE.ts \
                languages/iricGdPolygon_eu_ES.ts \
                languages/iricGdPolygon_fi_FI.ts \
                languages/iricGdPolygon_fr_FR.ts \
                languages/iricGdPolygon_gl_ES.ts \
                languages/iricGdPolygon_hi_IN.ts \
                languages/iricGdPolygon_hu_HU.ts \
                languages/iricGdPolygon_id_ID.ts \
                languages/iricGdPolygon_is_IS.ts \
                languages/iricGdPolygon_it_IT.ts \
                languages/iricGdPolygon_ja_JP.ts \
                languages/iricGdPolygon_ko_KR.ts \
                languages/iricGdPolygon_ky_KG.ts \
                languages/iricGdPolygon_lt_LT.ts \
                languages/iricGdPolygon_lv_LV.ts \
                languages/iricGdPolygon_nb_NO.ts \
                languages/iricGdPolygon_nl_NL.ts \
                languages/iricGdPolygon_pl_PL.ts \
                languages/iricGdPolygon_pt_BR.ts \
                languages/iricGdPolygon_pt_PT.ts \
                languages/iricGdPolygon_ro_RO.ts \
                languages/iricGdPolygon_ru_RU.ts \
                languages/iricGdPolygon_sl_SI.ts \
                languages/iricGdPolygon_sv_SE.ts \
                languages/iricGdPolygon_th_TH.ts \
                languages/iricGdPolygon_tr_TR.ts \
                languages/iricGdPolygon_uk_UA.ts \
                languages/iricGdPolygon_vi_VN.ts \
                languages/iricGdPolygon_zh_CN.ts \
                languages/iricGdPolygon_zh_TW.ts

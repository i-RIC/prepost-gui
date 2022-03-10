TARGET = iricGdPolyLine
TEMPLATE = lib
INCLUDEPATH += ../..

DEFINES += GD_POLYLINE_LIBRARY

DEFINES += ANSI_DECLARATORS

include( ../../../paths.pri )

QT += widgets xml

# Use Precompiled headers (PCH)
PRECOMPILED_HEADER = polyline_pch.h

######################
# Internal libraries #
######################

# iricGeoio

unix {
	LIBS += -L"../../geoio"
}
LIBS += -liricGeoio

# iricMisc

unix {
	LIBS += -L"../../misc"
}
LIBS += -liricMisc

# iricGdPolydata

unix {
	LIBS += -L"../polydata"
}
LIBS += -liricGdPolydata

# iricGuibase

unix {
	LIBS += -L"../../guibase"
}
LIBS += -liricGuibase

# iricGuicore

unix {
	LIBS += -L"../../guicore"
}
LIBS += -liricGuicore

######################
# External libraries #
######################

# VTK

LIBS += \
	-lvtkCommonCore-$${VTK_MAJ_MIN} \
	-lvtkCommonDataModel-$${VTK_MAJ_MIN} \
	-lvtkRenderingCore-$${VTK_MAJ_MIN}

# iriclib

win32 {
    CONFIG(debug, debug|release) {
        LIBS += -liriclibd
    } else {
        LIBS += -liriclib
    }
}

unix {
    LIBS += -liriclib
}

# shapelib

win32{
LIBS += -lshapelib_i
}

# geos

LIBS += -lgeos

win32 {
	DESTDIR = $(SolutionDir)/libdlls/$(Configuration)
	LIBS += -L$(SolutionDir)/libdlls/$(Configuration)
}

# Input
HEADERS += gd_polyline_global.h \
           geodatapolyline.h \
           geodatapolylineabstractpolyline.h \
           geodatapolylinecellmappersetting.h \
           geodatapolylinecellmappert.h \
           geodatapolylinecreator.h \
           geodatapolylinecreatort.h \
           geodatapolylineimplpolyline.h \
           geodatapolylineimporter.h \
           geodatapolylineimportersettingdialog.h \
           geodatapolylineintegercreator.h \
           geodatapolylinenodemappert.h \
           geodatapolylineproxy.h \
           geodatapolylinerealcreator.h \
           geodatapolylineshapeexporter.h \
           private/geodatapolyline_addvertexcommand.h \
           private/geodatapolyline_coordinateseditor.h \
           private/geodatapolyline_editcoordinatescommand.h \
           private/geodatapolyline_finishpolylinedefinitioncommand.h \
           private/geodatapolyline_impl.h \
           private/geodatapolyline_modifyabstractpolylinecommand.h \
           private/geodatapolyline_movepolylinecommand.h \
           private/geodatapolyline_movevertexcommand.h \
           private/geodatapolyline_pushnewpointcommand.h \
           private/geodatapolyline_removevertexcommand.h \
           private/geodatapolylineabstractpolyline_impl.h \
           private/geodatapolylinecellmappert_detail.h \
           private/geodatapolylinecreatort_detail.h \
           private/geodatapolylinenodemappert_detail.h
FORMS += \
         geodatapolylineimportersettingdialog.ui
SOURCES += geodatapolyline.cpp \
           geodatapolylineabstractpolyline.cpp \
           geodatapolylinecellmappersetting.cpp \
           geodatapolylinecreator.cpp \
           geodatapolylineimplpolyline.cpp \
           geodatapolylineimporter.cpp \
           geodatapolylineimportersettingdialog.cpp \
           geodatapolylineintegercreator.cpp \
           geodatapolylineproxy.cpp \
           geodatapolylinerealcreator.cpp \
           geodatapolylineshapeexporter.cpp \
           private/geodatapolyline_addvertexcommand.cpp \
           private/geodatapolyline_coordinateseditor.cpp \
           private/geodatapolyline_editcoordinatescommand.cpp \
           private/geodatapolyline_finishpolylinedefinitioncommand.cpp \
           private/geodatapolyline_modifyabstractpolylinecommand.cpp \
           private/geodatapolyline_movepolylinecommand.cpp \
           private/geodatapolyline_movevertexcommand.cpp \
           private/geodatapolyline_pushnewpointcommand.cpp \
           private/geodatapolyline_removevertexcommand.cpp
TRANSLATIONS += languages/iricGdPolyLine_ar_EG.ts \
                languages/iricGdPolyLine_bg_BG.ts \
                languages/iricGdPolyLine_bs_BA.ts \
                languages/iricGdPolyLine_ca_ES.ts \
                languages/iricGdPolyLine_cs_CZ.ts \
                languages/iricGdPolyLine_da_DK.ts \
                languages/iricGdPolyLine_de_DE.ts \
                languages/iricGdPolyLine_el_GR.ts \
                languages/iricGdPolyLine_es_ES.ts \
                languages/iricGdPolyLine_et_EE.ts \
                languages/iricGdPolyLine_eu_ES.ts \
                languages/iricGdPolyLine_fi_FI.ts \
                languages/iricGdPolyLine_fr_FR.ts \
                languages/iricGdPolyLine_gl_ES.ts \
                languages/iricGdPolyLine_hi_IN.ts \
                languages/iricGdPolyLine_hu_HU.ts \
                languages/iricGdPolyLine_id_ID.ts \
                languages/iricGdPolyLine_is_IS.ts \
                languages/iricGdPolyLine_it_IT.ts \
                languages/iricGdPolyLine_ja_JP.ts \
                languages/iricGdPolyLine_ko_KR.ts \
                languages/iricGdPolyLine_ky_KG.ts \
                languages/iricGdPolyLine_lt_LT.ts \
                languages/iricGdPolyLine_lv_LV.ts \
                languages/iricGdPolyLine_nb_NO.ts \
                languages/iricGdPolyLine_nl_NL.ts \
                languages/iricGdPolyLine_pl_PL.ts \
                languages/iricGdPolyLine_pt_BR.ts \
                languages/iricGdPolyLine_pt_PT.ts \
                languages/iricGdPolyLine_ro_RO.ts \
                languages/iricGdPolyLine_ru_RU.ts \
                languages/iricGdPolyLine_sl_SI.ts \
                languages/iricGdPolyLine_sv_SE.ts \
                languages/iricGdPolyLine_th_TH.ts \
                languages/iricGdPolyLine_tr_TR.ts \
                languages/iricGdPolyLine_uk_UA.ts \
                languages/iricGdPolyLine_vi_VN.ts \
                languages/iricGdPolyLine_zh_CN.ts \
                languages/iricGdPolyLine_zh_TW.ts

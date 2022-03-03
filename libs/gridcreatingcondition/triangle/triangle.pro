TARGET = iricGccTriangle
TEMPLATE = lib
INCLUDEPATH += ../..

DEFINES += GCC_TRIANGLE_LIBRARY

DEFINES += ANSI_DECLARATORS

include( ../../../paths.pri )

QT += widgets xml

# Use Precompiled headers (PCH)
PRECOMPILED_HEADER = triangle_pch.h

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

# iricTriangle

unix {
	LIBS += -L"../../triangle"
}
LIBS += -liricTriangle

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
	-lvtkCommonComputationalGeometry-$${VTK_MAJ_MIN} \
	-lvtkCommonCore-$${VTK_MAJ_MIN} \
	-lvtkCommonDataModel-$${VTK_MAJ_MIN} \
	-lvtkRenderingCore-$${VTK_MAJ_MIN} \
	-lvtkRenderingLabel-$${VTK_MAJ_MIN}

# geos

LIBS += -lgeos

win32 {
	DESTDIR = $(SolutionDir)/libdlls/$(Configuration)
	LIBS += -L$(SolutionDir)/libdlls/$(Configuration)
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
TRANSLATIONS += languages/iricGccTriangle_ar_EG.ts \
                languages/iricGccTriangle_bg_BG.ts \
                languages/iricGccTriangle_bs_BA.ts \
                languages/iricGccTriangle_ca_ES.ts \
                languages/iricGccTriangle_cs_CZ.ts \
                languages/iricGccTriangle_da_DK.ts \
                languages/iricGccTriangle_de_DE.ts \
                languages/iricGccTriangle_el_GR.ts \
                languages/iricGccTriangle_es_ES.ts \
                languages/iricGccTriangle_et_EE.ts \
                languages/iricGccTriangle_eu_ES.ts \
                languages/iricGccTriangle_fi_FI.ts \
                languages/iricGccTriangle_fr_FR.ts \
                languages/iricGccTriangle_gl_ES.ts \
                languages/iricGccTriangle_hi_IN.ts \
                languages/iricGccTriangle_hu_HU.ts \
                languages/iricGccTriangle_id_ID.ts \
                languages/iricGccTriangle_is_IS.ts \
                languages/iricGccTriangle_it_IT.ts \
                languages/iricGccTriangle_ja_JP.ts \
                languages/iricGccTriangle_ko_KR.ts \
                languages/iricGccTriangle_ky_KG.ts \
                languages/iricGccTriangle_lt_LT.ts \
                languages/iricGccTriangle_lv_LV.ts \
                languages/iricGccTriangle_nb_NO.ts \
                languages/iricGccTriangle_nl_NL.ts \
                languages/iricGccTriangle_pl_PL.ts \
                languages/iricGccTriangle_pt_BR.ts \
                languages/iricGccTriangle_pt_PT.ts \
                languages/iricGccTriangle_ro_RO.ts \
                languages/iricGccTriangle_ru_RU.ts \
                languages/iricGccTriangle_sl_SI.ts \
                languages/iricGccTriangle_sv_SE.ts \
                languages/iricGccTriangle_th_TH.ts \
                languages/iricGccTriangle_tr_TR.ts \
                languages/iricGccTriangle_uk_UA.ts \
                languages/iricGccTriangle_vi_VN.ts \
                languages/iricGccTriangle_zh_CN.ts \
                languages/iricGccTriangle_zh_TW.ts

TARGET = iricGdPolyLineGroup
TEMPLATE = lib
INCLUDEPATH += ../..

DEFINES += GD_POLYLINEGROUP_LIBRARY

DEFINES += ANSI_DECLARATORS

include( ../../../paths.pri )

QT += widgets xml

# Use Precompiled headers (PCH)
PRECOMPILED_HEADER = polylinegroup_pch.h

######################

# Internal libraries #
######################

# iricCs

unix {
        LIBS += -L"../../cs"
}
LIBS += -liricCs

# iricGdPolydata

unix {
        LIBS += -L"../polydata"
}
LIBS += -liricGdPolydata

# iricGdPolyLine

unix {
        LIBS += -L"../polyline"
}
LIBS += -liricGdPolyline

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

# iricPolydataGroup

unix {
	LIBS += -L"../polydatagroup"
}
LIBS += -liricGdPolydataGroup

######################
# External libraries #
######################

# VTK

LIBS += \
	-lvtkCommonCore-$${VTK_MAJ_MIN} \
	-lvtkCommonDataModel-$${VTK_MAJ_MIN} \
	-lvtkRenderingCore-$${VTK_MAJ_MIN}

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
HEADERS += gd_polylinegroup_global.h \
           geodatapolylinegroup.h \
           geodatapolylinegroupcellmappert.h \
           geodatapolylinegroupcreator.h \
           geodatapolylinegroupcreatort.h \
           geodatapolylinegroupcsvexporter.h \
           geodatapolylinegroupcsvimporter.h \
           geodatapolylinegroupintegercreator.h \
           geodatapolylinegroupnodemappert.h \
           geodatapolylinegrouppolyline.h \
           geodatapolylinegroupproxy.h \
           geodatapolylinegrouprealcreator.h \
           geodatapolylinegroupshpexporter.h \
           geodatapolylinegroupshpimporter.h \
           private/geodatapolylinegroup_displaysetting.h \
           private/geodatapolylinegroup_impl.h \
           private/geodatapolylinegroupcellmappert_detail.h \
           private/geodatapolylinegroupcreatort_detail.h \
           private/geodatapolylinegroupnodemappert_detail.h \
           private/geodatapolylinegrouppolyline_impl.h \
           private/geodatapolylinegroupproxy_displaysetting.h \
           private/geodatapolylinegroupproxy_displaysettingwidget.h \
           private/geodatapolylinegroupproxy_impl.h \
           public/geodatapolylinegroup_displaysettingwidget.h
FORMS += \
         private/geodatapolylinegroupproxy_displaysettingwidget.ui \
         public/geodatapolylinegroup_displaysettingwidget.ui
SOURCES += geodatapolylinegroup.cpp \
           geodatapolylinegroupcreator.cpp \
           geodatapolylinegroupcsvexporter.cpp \
           geodatapolylinegroupcsvimporter.cpp \
           geodatapolylinegroupintegercreator.cpp \
           geodatapolylinegrouppolyline.cpp \
           geodatapolylinegroupproxy.cpp \
           geodatapolylinegrouprealcreator.cpp \
           geodatapolylinegroupshpexporter.cpp \
           geodatapolylinegroupshpimporter.cpp \
           private/geodatapolylinegroup_displaysetting.cpp \
           private/geodatapolylinegroup_impl.cpp \
           private/geodatapolylinegroupproxy_displaysetting.cpp \
           private/geodatapolylinegroupproxy_displaysettingwidget.cpp \
           private/geodatapolylinegroupproxy_impl.cpp \
           public/geodatapolylinegroup_displaysettingwidget.cpp
TRANSLATIONS += languages/iricGdPolyLineGroup_ar_EG.ts \
                languages/iricGdPolyLineGroup_bg_BG.ts \
                languages/iricGdPolyLineGroup_bs_BA.ts \
                languages/iricGdPolyLineGroup_ca_ES.ts \
                languages/iricGdPolyLineGroup_cs_CZ.ts \
                languages/iricGdPolyLineGroup_da_DK.ts \
                languages/iricGdPolyLineGroup_de_DE.ts \
                languages/iricGdPolyLineGroup_el_GR.ts \
                languages/iricGdPolyLineGroup_es_ES.ts \
                languages/iricGdPolyLineGroup_et_EE.ts \
                languages/iricGdPolyLineGroup_eu_ES.ts \
                languages/iricGdPolyLineGroup_fi_FI.ts \
                languages/iricGdPolyLineGroup_fr_FR.ts \
                languages/iricGdPolyLineGroup_gl_ES.ts \
                languages/iricGdPolyLineGroup_hi_IN.ts \
                languages/iricGdPolyLineGroup_hu_HU.ts \
                languages/iricGdPolyLineGroup_id_ID.ts \
                languages/iricGdPolyLineGroup_is_IS.ts \
                languages/iricGdPolyLineGroup_it_IT.ts \
                languages/iricGdPolyLineGroup_ja_JP.ts \
                languages/iricGdPolyLineGroup_ko_KR.ts \
                languages/iricGdPolyLineGroup_ky_KG.ts \
                languages/iricGdPolyLineGroup_lt_LT.ts \
                languages/iricGdPolyLineGroup_lv_LV.ts \
                languages/iricGdPolyLineGroup_nb_NO.ts \
                languages/iricGdPolyLineGroup_nl_NL.ts \
                languages/iricGdPolyLineGroup_pl_PL.ts \
                languages/iricGdPolyLineGroup_pt_BR.ts \
                languages/iricGdPolyLineGroup_pt_PT.ts \
                languages/iricGdPolyLineGroup_ro_RO.ts \
                languages/iricGdPolyLineGroup_ru_RU.ts \
                languages/iricGdPolyLineGroup_sl_SI.ts \
                languages/iricGdPolyLineGroup_sv_SE.ts \
                languages/iricGdPolyLineGroup_th_TH.ts \
                languages/iricGdPolyLineGroup_tr_TR.ts \
                languages/iricGdPolyLineGroup_uk_UA.ts \
                languages/iricGdPolyLineGroup_vi_VN.ts \
                languages/iricGdPolyLineGroup_zh_CN.ts \
                languages/iricGdPolyLineGroup_zh_TW.ts

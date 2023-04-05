TARGET = iricGdPolygonGroup
TEMPLATE = lib
INCLUDEPATH += ../..

DEFINES += GD_POLYGONGROUP_LIBRARY

DEFINES += ANSI_DECLARATORS

include( ../../../paths.pri )

QT += widgets xml

# Use Precompiled headers (PCH)
PRECOMPILED_HEADER = polygongroup_pch.h

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

# iricGdPolygon

unix {
        LIBS += -L"../polygon"
}
LIBS += -liricGdPolygon

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
HEADERS += gd_polygongroup_global.h \
           geodatapolygongroup.h \
           geodatapolygongroupcellmappert.h \
           geodatapolygongroupcreator.h \
           geodatapolygongroupcreatort.h \
           geodatapolygongroupcsvexporter.h \
           geodatapolygongroupcsvimporter.h \
           geodatapolygongroupintegercreator.h \
           geodatapolygongroupnodemappert.h \
           geodatapolygongrouppolygon.h \
           geodatapolygongroupproxy.h \
           geodatapolygongrouprealcreator.h \
           geodatapolygongroupshpexporter.h \
           geodatapolygongroupshpimporter.h \
           polygongroup_pch.h \
           private/geodatapolygongroup_displaysetting.h \
           private/geodatapolygongroup_impl.h \
           private/geodatapolygongroupcellmappert_detail.h \
           private/geodatapolygongroupcreatort_detail.h \
           private/geodatapolygongroupnodemappert_detail.h \
           private/geodatapolygongrouppolygon_impl.h \
           private/geodatapolygongrouppolygon_triangulatori.h \
           private/geodatapolygongrouppolygon_triangulatortriangle.h \
           private/geodatapolygongrouppolygon_triangulatorvtk.h \
           private/geodatapolygongroupproxy_displaysetting.h \
           private/geodatapolygongroupproxy_displaysettingwidget.h \
           private/geodatapolygongroupproxy_impl.h \
           public/geodatapolygongroup_displaysettingwidget.h
FORMS += private/geodatapolygongroupproxy_displaysettingwidget.ui \
         public/geodatapolygongroup_displaysettingwidget.ui
SOURCES += geodatapolygongroup.cpp \
           geodatapolygongroupcreator.cpp \
           geodatapolygongroupcsvexporter.cpp \
           geodatapolygongroupcsvimporter.cpp \
           geodatapolygongroupintegercreator.cpp \
           geodatapolygongrouppolygon.cpp \
           geodatapolygongroupproxy.cpp \
           geodatapolygongrouprealcreator.cpp \
           geodatapolygongroupshpexporter.cpp \
           geodatapolygongroupshpimporter.cpp \
           private/geodatapolygongroup_displaysetting.cpp \
           private/geodatapolygongroup_impl.cpp \
           private/geodatapolygongrouppolygon_triangulatortriangle.cpp \
           private/geodatapolygongrouppolygon_triangulatorvtk.cpp \
           private/geodatapolygongroupproxy_displaysetting.cpp \
           private/geodatapolygongroupproxy_displaysettingwidget.cpp \
           private/geodatapolygongroupproxy_impl.cpp \
           public/geodatapolygongroup_displaysettingwidget.cpp
TRANSLATIONS += languages/iricGdPolygonGroup_ar_EG.ts \
                languages/iricGdPolygonGroup_bg_BG.ts \
                languages/iricGdPolygonGroup_bs_BA.ts \
                languages/iricGdPolygonGroup_ca_ES.ts \
                languages/iricGdPolygonGroup_cs_CZ.ts \
                languages/iricGdPolygonGroup_da_DK.ts \
                languages/iricGdPolygonGroup_de_DE.ts \
                languages/iricGdPolygonGroup_el_GR.ts \
                languages/iricGdPolygonGroup_es_ES.ts \
                languages/iricGdPolygonGroup_et_EE.ts \
                languages/iricGdPolygonGroup_eu_ES.ts \
                languages/iricGdPolygonGroup_fi_FI.ts \
                languages/iricGdPolygonGroup_fr_FR.ts \
                languages/iricGdPolygonGroup_gl_ES.ts \
                languages/iricGdPolygonGroup_hi_IN.ts \
                languages/iricGdPolygonGroup_hu_HU.ts \
                languages/iricGdPolygonGroup_id_ID.ts \
                languages/iricGdPolygonGroup_is_IS.ts \
                languages/iricGdPolygonGroup_it_IT.ts \
                languages/iricGdPolygonGroup_ja_JP.ts \
                languages/iricGdPolygonGroup_ko_KR.ts \
                languages/iricGdPolygonGroup_ky_KG.ts \
                languages/iricGdPolygonGroup_lt_LT.ts \
                languages/iricGdPolygonGroup_lv_LV.ts \
                languages/iricGdPolygonGroup_nb_NO.ts \
                languages/iricGdPolygonGroup_nl_NL.ts \
                languages/iricGdPolygonGroup_pl_PL.ts \
                languages/iricGdPolygonGroup_pt_BR.ts \
                languages/iricGdPolygonGroup_pt_PT.ts \
                languages/iricGdPolygonGroup_ro_RO.ts \
                languages/iricGdPolygonGroup_ru_RU.ts \
                languages/iricGdPolygonGroup_sl_SI.ts \
                languages/iricGdPolygonGroup_sv_SE.ts \
                languages/iricGdPolygonGroup_th_TH.ts \
                languages/iricGdPolygonGroup_tr_TR.ts \
                languages/iricGdPolygonGroup_uk_UA.ts \
                languages/iricGdPolygonGroup_vi_VN.ts \
                languages/iricGdPolygonGroup_zh_CN.ts \
                languages/iricGdPolygonGroup_zh_TW.ts

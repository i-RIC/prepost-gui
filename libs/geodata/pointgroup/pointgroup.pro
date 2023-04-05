TARGET = iricGdPointGroup
TEMPLATE = lib
INCLUDEPATH += ../..

DEFINES += GD_POINTGROUP_LIBRARY

DEFINES += ANSI_DECLARATORS

include( ../../../paths.pri )

QT += widgets xml

# Use Precompiled headers (PCH)
PRECOMPILED_HEADER = pointgroup_pch.h

######################
# Internal libraries #
######################

# iricCs

unix {
        LIBS += -L"../../cs"
}
LIBS += -liricCs

# iricGdPoint

unix {
        LIBS += -L"../point"
}
LIBS += -liricGdPoint

# iricGdPolydata

unix {
        LIBS += -L"../polydata"
}
LIBS += -liricGdPolydata

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
	-lvtkCommonExecutionModel-$${VTK_MAJ_MIN} \
	-lvtkImagingCore-$${VTK_MAJ_MIN} \
	-lvtkRenderingCore-$${VTK_MAJ_MIN} \
	-lvtkRenderingQt-$${VTK_MAJ_MIN}

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
HEADERS += gd_pointgroup_global.h \
           geodatapointgroup.h \
           geodatapointgroupcellmappert.h \
           geodatapointgroupcolorsettingdialog.h \
           geodatapointgroupcreator.h \
           geodatapointgroupcreatort.h \
           geodatapointgroupcsvexporter.h \
           geodatapointgroupcsvimporter.h \
           geodatapointgroupintegercreator.h \
           geodatapointgroupnodemappert.h \
           geodatapointgrouppoint.h \
           geodatapointgroupproxy.h \
           geodatapointgrouprealcreator.h \
           geodatapointgroupshpexporter.h \
           geodatapointgroupshpimporter.h \
           private/geodatapointgroup_displaysetting.h \
           private/geodatapointgroup_displaysettingwidget.h \
           private/geodatapointgroup_impl.h \
           private/geodatapointgroupcellmappert_detail.h \
           private/geodatapointgroupcreatort_detail.h \
           private/geodatapointgroupnodemappert_detail.h \
           private/geodatapointgrouppoint_impl.h \
           private/geodatapointgroupproxy_displaysetting.h \
           private/geodatapointgroupproxy_displaysettingwidget.h \
           private/geodatapointgroupproxy_impl.h
FORMS += geodatapointgroupcolorsettingdialog.ui \
         private/geodatapointgroup_displaysettingwidget.ui \
         private/geodatapointgroupproxy_displaysettingwidget.ui
SOURCES += geodatapointgroup.cpp \
           geodatapointgroupcolorsettingdialog.cpp \
           geodatapointgroupcreator.cpp \
           geodatapointgroupcsvexporter.cpp \
           geodatapointgroupcsvimporter.cpp \
           geodatapointgroupintegercreator.cpp \
           geodatapointgrouppoint.cpp \
           geodatapointgroupproxy.cpp \
           geodatapointgrouprealcreator.cpp \
           geodatapointgroupshpexporter.cpp \
           geodatapointgroupshpimporter.cpp \
           private/geodatapointgroup_displaysetting.cpp \
           private/geodatapointgroup_displaysettingwidget.cpp \
           private/geodatapointgroup_impl.cpp \
           private/geodatapointgroupproxy_displaysetting.cpp \
           private/geodatapointgroupproxy_displaysettingwidget.cpp \
           private/geodatapointgroupproxy_impl.cpp
TRANSLATIONS += languages/iricGdPointGroup_ar_EG.ts \
                languages/iricGdPointGroup_bg_BG.ts \
                languages/iricGdPointGroup_bs_BA.ts \
                languages/iricGdPointGroup_ca_ES.ts \
                languages/iricGdPointGroup_cs_CZ.ts \
                languages/iricGdPointGroup_da_DK.ts \
                languages/iricGdPointGroup_de_DE.ts \
                languages/iricGdPointGroup_el_GR.ts \
                languages/iricGdPointGroup_es_ES.ts \
                languages/iricGdPointGroup_et_EE.ts \
                languages/iricGdPointGroup_eu_ES.ts \
                languages/iricGdPointGroup_fi_FI.ts \
                languages/iricGdPointGroup_fr_FR.ts \
                languages/iricGdPointGroup_gl_ES.ts \
                languages/iricGdPointGroup_hi_IN.ts \
                languages/iricGdPointGroup_hu_HU.ts \
                languages/iricGdPointGroup_id_ID.ts \
                languages/iricGdPointGroup_is_IS.ts \
                languages/iricGdPointGroup_it_IT.ts \
                languages/iricGdPointGroup_ja_JP.ts \
                languages/iricGdPointGroup_ko_KR.ts \
                languages/iricGdPointGroup_ky_KG.ts \
                languages/iricGdPointGroup_lt_LT.ts \
                languages/iricGdPointGroup_lv_LV.ts \
                languages/iricGdPointGroup_nb_NO.ts \
                languages/iricGdPointGroup_nl_NL.ts \
                languages/iricGdPointGroup_pl_PL.ts \
                languages/iricGdPointGroup_pt_BR.ts \
                languages/iricGdPointGroup_pt_PT.ts \
                languages/iricGdPointGroup_ro_RO.ts \
                languages/iricGdPointGroup_ru_RU.ts \
                languages/iricGdPointGroup_sl_SI.ts \
                languages/iricGdPointGroup_sv_SE.ts \
                languages/iricGdPointGroup_th_TH.ts \
                languages/iricGdPointGroup_tr_TR.ts \
                languages/iricGdPointGroup_uk_UA.ts \
                languages/iricGdPointGroup_vi_VN.ts \
                languages/iricGdPointGroup_zh_CN.ts \
                languages/iricGdPointGroup_zh_TW.ts

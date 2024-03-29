TARGET = iricGdNetcdf
TEMPLATE = lib
INCLUDEPATH += ../..

DEFINES += GD_NETCDF_LIBRARY

DEFINES += ANSI_DECLARATORS
win32 {
	DEFINES += YAML_CPP_DLL
}

include( ../../../paths.pri )

QT += widgets xml

# Use Precompiled headers (PCH)
PRECOMPILED_HEADER = netcdf_pch.h

######################
# Internal libraries #
######################

# iricCs

unix {
	LIBS += -L"../../cs"
}
LIBS += -liricCs

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
	-lvtkCommonCore-$${VTK_MAJ_MIN} \
	-lvtkCommonDataModel-$${VTK_MAJ_MIN} \
	-lvtkCommonExecutionModel-$${VTK_MAJ_MIN} \
	-lvtkRenderingCore-$${VTK_MAJ_MIN} \
	-lvtkFiltersCore-$${VTK_MAJ_MIN} \
	-lvtkFiltersGeometry-$${VTK_MAJ_MIN} \
	-lvtkFiltersExtraction-$${VTK_MAJ_MIN}

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

# netcdf

LIBS += -lnetcdf

#gdal

win32 {
	LIBS += -lgdal_i
}
unix {
	LIBS += -lgdal
}

# udunits

LIBS += -ludunits2

# libpng

win32 {
    CONFIG(debug, debug|release) {
        LIBS += -llibpng16d
    } else {
        LIBS += -llibpng16
    }
}

unix {
    LIBS += -llibpng16
}

# yaml-cpp

win32 {
	LIBS += -lyaml-cpp
}
unix {
	LIBS += -lyaml-cpp
}

win32 {
	DESTDIR = $(SolutionDir)/libdlls/$(Configuration)
	LIBS += -L$(SolutionDir)/libdlls/$(Configuration)
}

# Input
HEADERS += gd_netcdf_global.h \
           geodatanetcdf.h \
           geodatanetcdfcellmappert.h \
           geodatanetcdfcolorsettingdialog.h \
           geodatanetcdfcreator.h \
           geodatanetcdfcreatort.h \
           geodatanetcdffilelistdialog.h \
           geodatanetcdffilenamematcher.h \
           geodatanetcdffilenamepatterndialog.h \
           geodatanetcdfgdalexporter.h \
           geodatanetcdfgdalimporter.h \
           geodatanetcdfgdalintegerexporter.h \
           geodatanetcdfgdalintegerimporter.h \
           geodatanetcdfgdalrealexporter.h \
           geodatanetcdfgdalrealimporter.h \
           geodatanetcdfgrayscalepngrealexporter.h \
           geodatanetcdfgrayscalepngrealimporter.h \
           geodatanetcdfifacemappert.h \
           geodatanetcdfimporter.h \
           geodatanetcdfimporterdateselectdialog.h \
           geodatanetcdfimportersettingdialog.h \
           geodatanetcdfimportert.h \
           geodatanetcdfinteger.h \
           geodatanetcdfintegercreator.h \
           geodatanetcdfintegerimporter.h \
           geodatanetcdfjfacemappert.h \
           geodatanetcdfncexporter.h \
           geodatanetcdfnodemappert.h \
           geodatanetcdfproxy.h \
           geodatanetcdfreal.h \
           geodatanetcdfrealcreator.h \
           geodatanetcdfrealimporter.h \
           geodatanetcdft.h \
           geodatanetcdftimeseriescreator.h \
           geodatanetcdftimeseriescreatort.h \
           geodatanetcdftimeseriesintegercreator.h \
           geodatanetcdftimeseriesrealcreator.h \
           geodatanetcdfxbandimporter.h \
           private/geodatanetcdf_displaysetting.h \
           private/geodatanetcdf_impl.h \
           private/geodatanetcdfcellmappert_detail.h \
           private/geodatanetcdfcreatort_detail.h \
           private/geodatanetcdfifacemappert_detail.h \
           private/geodatanetcdfimportert_detail.h \
           private/geodatanetcdfjfacemappert_detail.h \
           private/geodatanetcdfnodemappert_detail.h \
           private/geodatanetcdfproxy_displaysetting.h \
           private/geodatanetcdfproxy_displaysettingwidget.h \
           private/geodatanetcdfproxy_impl.h \
           private/geodatanetcdft_detail.h \
           private/geodatanetcdftimeseriescreatort_detail.h \
           public/geodatanetcdf_displaysettingwidget.h
FORMS += geodatanetcdfcolorsettingdialog.ui \
         geodatanetcdffilelistdialog.ui \
         geodatanetcdffilenamepatterndialog.ui \
         geodatanetcdfimporterdateselectdialog.ui \
         geodatanetcdfimportersettingdialog.ui \
         private/geodatanetcdfproxy_displaysettingwidget.ui \
         public/geodatanetcdf_displaysettingwidget.ui
SOURCES += geodatanetcdf.cpp \
           geodatanetcdfcolorsettingdialog.cpp \
           geodatanetcdfcreator.cpp \
           geodatanetcdffilelistdialog.cpp \
           geodatanetcdffilenamematcher.cpp \
           geodatanetcdffilenamepatterndialog.cpp \
           geodatanetcdfgdalexporter.cpp \
           geodatanetcdfgdalimporter.cpp \
           geodatanetcdfgdalintegerexporter.cpp \
           geodatanetcdfgdalintegerimporter.cpp \
           geodatanetcdfgdalrealexporter.cpp \
           geodatanetcdfgdalrealimporter.cpp \
           geodatanetcdfgrayscalepngrealexporter.cpp \
           geodatanetcdfgrayscalepngrealimporter.cpp \
           geodatanetcdfimporter.cpp \
           geodatanetcdfimporterdateselectdialog.cpp \
           geodatanetcdfimportersettingdialog.cpp \
           geodatanetcdfinteger.cpp \
           geodatanetcdfintegercreator.cpp \
           geodatanetcdfintegerimporter.cpp \
           geodatanetcdfncexporter.cpp \
           geodatanetcdfproxy.cpp \
           geodatanetcdfreal.cpp \
           geodatanetcdfrealcreator.cpp \
           geodatanetcdfrealimporter.cpp \
           geodatanetcdftimeseriescreator.cpp \
           geodatanetcdftimeseriesintegercreator.cpp \
           geodatanetcdftimeseriesrealcreator.cpp \
           geodatanetcdfxbandimporter.cpp \
           private/geodatanetcdf_displaysetting.cpp \
           private/geodatanetcdf_impl.cpp \
           private/geodatanetcdfproxy_displaysetting.cpp \
           private/geodatanetcdfproxy_displaysettingwidget.cpp \
           private/geodatanetcdfproxy_impl.cpp \
           public/geodatanetcdf_displaysettingwidget.cpp
TRANSLATIONS += languages/iricGdNetcdf_ar_EG.ts \
                languages/iricGdNetcdf_bg_BG.ts \
                languages/iricGdNetcdf_bs_BA.ts \
                languages/iricGdNetcdf_ca_ES.ts \
                languages/iricGdNetcdf_cs_CZ.ts \
                languages/iricGdNetcdf_da_DK.ts \
                languages/iricGdNetcdf_de_DE.ts \
                languages/iricGdNetcdf_el_GR.ts \
                languages/iricGdNetcdf_es_ES.ts \
                languages/iricGdNetcdf_et_EE.ts \
                languages/iricGdNetcdf_eu_ES.ts \
                languages/iricGdNetcdf_fi_FI.ts \
                languages/iricGdNetcdf_fr_FR.ts \
                languages/iricGdNetcdf_gl_ES.ts \
                languages/iricGdNetcdf_hi_IN.ts \
                languages/iricGdNetcdf_hu_HU.ts \
                languages/iricGdNetcdf_id_ID.ts \
                languages/iricGdNetcdf_is_IS.ts \
                languages/iricGdNetcdf_it_IT.ts \
                languages/iricGdNetcdf_ja_JP.ts \
                languages/iricGdNetcdf_ko_KR.ts \
                languages/iricGdNetcdf_ky_KG.ts \
                languages/iricGdNetcdf_lt_LT.ts \
                languages/iricGdNetcdf_lv_LV.ts \
                languages/iricGdNetcdf_nb_NO.ts \
                languages/iricGdNetcdf_nl_NL.ts \
                languages/iricGdNetcdf_pl_PL.ts \
                languages/iricGdNetcdf_pt_BR.ts \
                languages/iricGdNetcdf_pt_PT.ts \
                languages/iricGdNetcdf_ro_RO.ts \
                languages/iricGdNetcdf_ru_RU.ts \
                languages/iricGdNetcdf_sl_SI.ts \
                languages/iricGdNetcdf_sv_SE.ts \
                languages/iricGdNetcdf_th_TH.ts \
                languages/iricGdNetcdf_tr_TR.ts \
                languages/iricGdNetcdf_uk_UA.ts \
                languages/iricGdNetcdf_vi_VN.ts \
                languages/iricGdNetcdf_zh_CN.ts \
                languages/iricGdNetcdf_zh_TW.ts

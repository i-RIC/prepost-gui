TARGET = iricGdGdal
TEMPLATE = lib
INCLUDEPATH += ../..

DEFINES += GD_GDAL_LIBRARY

DEFINES += ANSI_DECLARATORS
win32 {
	DEFINES += YAML_CPP_DLL
}

include( ../../../paths.pri )

QT += widgets xml

# Use Precompiled headers (PCH)
PRECOMPILED_HEADER = gdal_pch.h

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
HEADERS += gd_gdal_global.h \
           geodatagdal.h \
           geodatagdalcellmappert.h \
           geodatagdalcolorsettingdialog.h \
           geodatagdalcreator.h \
           geodatagdalcreatort.h \
           geodatagdalfilelistdialog.h \
           geodatagdalfilenamematcher.h \
           geodatagdalfilenamepatterndialog.h \
           geodatagdalgdalexporter.h \
           geodatagdalgdalimporter.h \
           geodatagdalgdalintegerexporter.h \
           geodatagdalgdalintegerimporter.h \
           geodatagdalgdalrealexporter.h \
           geodatagdalgdalrealimporter.h \
           geodatagdalgrayscalepngrealexporter.h \
           geodatagdalgrayscalepngrealimporter.h \
           geodatagdalifacemappert.h \
           geodatagdalinteger.h \
           geodatagdalintegercreator.h \
           geodatagdalintegerreadonlycreator.h \
           geodatagdaljfacemappert.h \
           geodatagdalncexporter.h \
           geodatagdalnetcdfimporter.h \
           geodatagdalnetcdfimporterdateselectdialog.h \
           geodatagdalnetcdfimportersettingdialog.h \
           geodatagdalnetcdfimportert.h \
           geodatagdalnetcdfintegerimporter.h \
           geodatagdalnetcdfrealimporter.h \
           geodatagdalnodemappert.h \
           geodatagdalproxy.h \
           geodatagdalreal.h \
           geodatagdalrealcreator.h \
           geodatagdalrealreadonlycreator.h \
           geodatagdalt.h \
           geodatagdaltimeseriescreator.h \
           geodatagdaltimeseriescreatort.h \
           geodatagdaltimeseriesintegercreator.h \
           geodatagdaltimeseriesintegerreadonlycreator.h \
           geodatagdaltimeseriesrealcreator.h \
           geodatagdaltimeseriesrealreadonlycreator.h \
           geodatagdalxbandimporter.h \
           private/geodatagdal_displaysetting.h \
           private/geodatagdal_impl.h \
           private/geodatagdalcellmappert_detail.h \
           private/geodatagdalcreatort_detail.h \
           private/geodatagdalgrayscalepngrealimporter_importersetting.h \
           private/geodatagdalifacemappert_detail.h \
           private/geodatagdaljfacemappert_detail.h \
           private/geodatagdalnetcdfimportert_detail.h \
           private/geodatagdalnodemappert_detail.h \
           private/geodatagdalproxy_displaysetting.h \
           private/geodatagdalproxy_displaysettingwidget.h \
           private/geodatagdalproxy_impl.h \
           private/geodatagdalt_detail.h \
           private/geodatagdaltimeseriescreatort_detail.h \
           public/geodatagdal_displaysettingwidget.h
FORMS += geodatagdalcolorsettingdialog.ui \
         geodatagdalfilelistdialog.ui \
         geodatagdalfilenamepatterndialog.ui \
         geodatagdalnetcdfimporterdateselectdialog.ui \
         geodatagdalnetcdfimportersettingdialog.ui \
         private/geodatagdalproxy_displaysettingwidget.ui \
         public/geodatagdal_displaysettingwidget.ui
SOURCES += geodatagdal.cpp \
           geodatagdalcolorsettingdialog.cpp \
           geodatagdalcreator.cpp \
           geodatagdalfilelistdialog.cpp \
           geodatagdalfilenamematcher.cpp \
           geodatagdalfilenamepatterndialog.cpp \
           geodatagdalgdalexporter.cpp \
           geodatagdalgdalimporter.cpp \
           geodatagdalgdalintegerexporter.cpp \
           geodatagdalgdalintegerimporter.cpp \
           geodatagdalgdalrealexporter.cpp \
           geodatagdalgdalrealimporter.cpp \
           geodatagdalgrayscalepngrealexporter.cpp \
           geodatagdalgrayscalepngrealimporter.cpp \
           geodatagdalinteger.cpp \
           geodatagdalintegercreator.cpp \
           geodatagdalintegerreadonlycreator.cpp \
           geodatagdalncexporter.cpp \
           geodatagdalnetcdfimporter.cpp \
           geodatagdalnetcdfimporterdateselectdialog.cpp \
           geodatagdalnetcdfimportersettingdialog.cpp \
           geodatagdalnetcdfintegerimporter.cpp \
           geodatagdalnetcdfrealimporter.cpp \
           geodatagdalproxy.cpp \
           geodatagdalreal.cpp \
           geodatagdalrealcreator.cpp \
           geodatagdalrealreadonlycreator.cpp \
           geodatagdaltimeseriescreator.cpp \
           geodatagdaltimeseriesintegercreator.cpp \
           geodatagdaltimeseriesintegerreadonlycreator.cpp \
           geodatagdaltimeseriesrealcreator.cpp \
           geodatagdaltimeseriesrealreadonlycreator.cpp \
           geodatagdalxbandimporter.cpp \
           private/geodatagdal_displaysetting.cpp \
           private/geodatagdal_impl.cpp \
           private/geodatagdalgrayscalepngrealimporter_importersetting.cpp \
           private/geodatagdalproxy_displaysetting.cpp \
           private/geodatagdalproxy_displaysettingwidget.cpp \
           private/geodatagdalproxy_impl.cpp \
           public/geodatagdal_displaysettingwidget.cpp
TRANSLATIONS += languages/iricGdGdal_ar_EG.ts \
                languages/iricGdGdal_bg_BG.ts \
                languages/iricGdGdal_bs_BA.ts \
                languages/iricGdGdal_ca_ES.ts \
                languages/iricGdGdal_cs_CZ.ts \
                languages/iricGdGdal_da_DK.ts \
                languages/iricGdGdal_de_DE.ts \
                languages/iricGdGdal_el_GR.ts \
                languages/iricGdGdal_es_ES.ts \
                languages/iricGdGdal_et_EE.ts \
                languages/iricGdGdal_eu_ES.ts \
                languages/iricGdGdal_fi_FI.ts \
                languages/iricGdGdal_fr_FR.ts \
                languages/iricGdGdal_gl_ES.ts \
                languages/iricGdGdal_hi_IN.ts \
                languages/iricGdGdal_hu_HU.ts \
                languages/iricGdGdal_id_ID.ts \
                languages/iricGdGdal_is_IS.ts \
                languages/iricGdGdal_it_IT.ts \
                languages/iricGdGdal_ja_JP.ts \
                languages/iricGdGdal_ko_KR.ts \
                languages/iricGdGdal_ky_KG.ts \
                languages/iricGdGdal_lt_LT.ts \
                languages/iricGdGdal_lv_LV.ts \
                languages/iricGdGdal_nb_NO.ts \
                languages/iricGdGdal_nl_NL.ts \
                languages/iricGdGdal_pl_PL.ts \
                languages/iricGdGdal_pt_BR.ts \
                languages/iricGdGdal_pt_PT.ts \
                languages/iricGdGdal_ro_RO.ts \
                languages/iricGdGdal_ru_RU.ts \
                languages/iricGdGdal_sl_SI.ts \
                languages/iricGdGdal_sv_SE.ts \
                languages/iricGdGdal_th_TH.ts \
                languages/iricGdGdal_tr_TR.ts \
                languages/iricGdGdal_uk_UA.ts \
                languages/iricGdGdal_vi_VN.ts \
                languages/iricGdGdal_zh_CN.ts \
                languages/iricGdGdal_zh_TW.ts

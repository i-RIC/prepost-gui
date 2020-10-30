TARGET = iricGdNetcdf
TEMPLATE = lib
INCLUDEPATH += ../..

DEFINES += GD_NETCDF_LIBRARY

DEFINES += ANSI_DECLARATORS

include( ../../../paths.pri )

QT += widgets xml

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
	-lvtkCommonCore-6.1 \
	-lvtkCommonDataModel-6.1 \
	-lvtkCommonExecutionModel-6.1 \
	-lvtkRenderingCore-6.1 \
	-lvtkFiltersCore-6.1 \
	-lvtkFiltersExtraction-6.1

# iriclib

LIBS += -liriclib

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
           geodatanetcdfgdalimporter.h \
           geodatanetcdfgdalintegerimporter.h \
           geodatanetcdfgdalrealimporter.h \
           geodatanetcdfimporter.h \
           geodatanetcdfimporterdateselectdialog.h \
           geodatanetcdfimportersettingdialog.h \
           geodatanetcdfimportert.h \
           geodatanetcdfinteger.h \
           geodatanetcdfintegercreator.h \
           geodatanetcdfintegerimporter.h \
           geodatanetcdfnodemappert.h \
           geodatanetcdfreal.h \
           geodatanetcdfrealcreator.h \
           geodatanetcdfrealimporter.h \
           geodatanetcdft.h \
           geodatanetcdfxbandimporter.h \
           private/geodatanetcdf_editpropertycommand.h \
           private/geodatanetcdfcellmappert_detail.h \
           private/geodatanetcdfcreatort_detail.h \
           private/geodatanetcdfimportert_detail.h \
           private/geodatanetcdfnodemappert_detail.h \
           private/geodatanetcdft_detail.h
FORMS += geodatanetcdfcolorsettingdialog.ui \
         geodatanetcdffilelistdialog.ui \
         geodatanetcdffilenamepatterndialog.ui \
         geodatanetcdfimporterdateselectdialog.ui \
         geodatanetcdfimportersettingdialog.ui
SOURCES += geodatanetcdf.cpp \
           geodatanetcdfcolorsettingdialog.cpp \
           geodatanetcdfcreator.cpp \
           geodatanetcdffilelistdialog.cpp \
           geodatanetcdffilenamematcher.cpp \
           geodatanetcdffilenamepatterndialog.cpp \
           geodatanetcdfgdalimporter.cpp \
           geodatanetcdfgdalintegerimporter.cpp \
           geodatanetcdfgdalrealimporter.cpp \
           geodatanetcdfimporter.cpp \
           geodatanetcdfimporterdateselectdialog.cpp \
           geodatanetcdfimportersettingdialog.cpp \
           geodatanetcdfinteger.cpp \
           geodatanetcdfintegercreator.cpp \
           geodatanetcdfintegerimporter.cpp \
           geodatanetcdfreal.cpp \
           geodatanetcdfrealcreator.cpp \
           geodatanetcdfrealimporter.cpp \
           geodatanetcdfxbandimporter.cpp \
           private/geodatanetcdf_editpropertycommand.cpp
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

TARGET = iricGeoio
TEMPLATE = lib
INCLUDEPATH += ..

DEFINES += GEOIO_LIBRARY

QT += widgets

include( ../../paths.pri )

# Use Precompiled headers (PCH)
PRECOMPILED_HEADER = geoio_pch.h

######################
# Internal libraries #
######################

# iricGuibase

unix {
        LIBS += -L"../guibase"
}
LIBS += -liricGuibase

# iricMisc

unix {
        LIBS += -L"../misc"
}
LIBS += -liricMisc

######################
# External libraries #
######################

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
HEADERS += geoio_global.h \
           io.h \
           polygoncsvexporter.h \
           polygoncsvimporter.h \
           polygonexporterfactory.h \
           polygonexporteri.h \
           polygonimporterfactory.h \
           polygonimporteri.h \
           polygonio.h \
           polygonshapeexporter.h \
           polygonshapeimporter.h \
           polygonutil.h \
           polylinecsvexporter.h \
           polylinecsvimporter.h \
           polylineexporterfactory.h \
           polylineexporteri.h \
           polylineimporterfactory.h \
           polylineimporteri.h \
           polylineio.h \
           polylineshapeexporter.h \
           polylineshapeimporter.h \
           private/csvhelper.h \
           private/dbfitemselectdialog.h \
           private/polygonexporterfactory_impl.h \
           private/polygonimporterfactory_impl.h \
           private/polylineexporterfactory_impl.h \
           private/polylineimporterfactory_impl.h
FORMS += private/dbfitemselectdialog.ui
SOURCES += polygoncsvexporter.cpp \
           polygoncsvimporter.cpp \
           polygonexporterfactory.cpp \
           polygonimporterfactory.cpp \
           polygonio.cpp \
           polygonshapeexporter.cpp \
           polygonshapeimporter.cpp \
           polygonutil.cpp \
           polylinecsvexporter.cpp \
           polylinecsvimporter.cpp \
           polylineexporterfactory.cpp \
           polylineimporterfactory.cpp \
           polylineio.cpp \
           polylineshapeexporter.cpp \
           polylineshapeimporter.cpp \
           private/csvhelper.cpp \
           private/dbfitemselectdialog.cpp
TRANSLATIONS += languages/iricGeoIO_ar_EG.ts \
                languages/iricGeoIO_bg_BG.ts \
                languages/iricGeoIO_bs_BA.ts \
                languages/iricGeoIO_ca_ES.ts \
                languages/iricGeoIO_cs_CZ.ts \
                languages/iricGeoIO_da_DK.ts \
                languages/iricGeoIO_de_DE.ts \
                languages/iricGeoIO_el_GR.ts \
                languages/iricGeoIO_es_ES.ts \
                languages/iricGeoIO_et_EE.ts \
                languages/iricGeoIO_eu_ES.ts \
                languages/iricGeoIO_fi_FI.ts \
                languages/iricGeoIO_fr_FR.ts \
                languages/iricGeoIO_gl_ES.ts \
                languages/iricGeoIO_hi_IN.ts \
                languages/iricGeoIO_hu_HU.ts \
                languages/iricGeoIO_id_ID.ts \
                languages/iricGeoIO_is_IS.ts \
                languages/iricGeoIO_it_IT.ts \
                languages/iricGeoIO_ja_JP.ts \
                languages/iricGeoIO_ko_KR.ts \
                languages/iricGeoIO_ky_KG.ts \
                languages/iricGeoIO_lt_LT.ts \
                languages/iricGeoIO_lv_LV.ts \
                languages/iricGeoIO_nb_NO.ts \
                languages/iricGeoIO_nl_NL.ts \
                languages/iricGeoIO_pl_PL.ts \
                languages/iricGeoIO_pt_BR.ts \
                languages/iricGeoIO_pt_PT.ts \
                languages/iricGeoIO_ro_RO.ts \
                languages/iricGeoIO_ru_RU.ts \
                languages/iricGeoIO_sl_SI.ts \
                languages/iricGeoIO_sv_SE.ts \
                languages/iricGeoIO_th_TH.ts \
                languages/iricGeoIO_tr_TR.ts \
                languages/iricGeoIO_uk_UA.ts \
                languages/iricGeoIO_vi_VN.ts \
                languages/iricGeoIO_zh_CN.ts \
                languages/iricGeoIO_zh_TW.ts

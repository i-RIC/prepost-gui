#-------------------------------------------------
#
# Project created by QtCreator 2014-10-17T11:05:29
#
#-------------------------------------------------

TARGET  = $$qtLibraryTarget(unstructured2dgridsewerimporter)
TEMPLATE = lib
CONFIG += plugin
QT       += widgets xml

include( ../../../paths.pri )

# Use Precompiled headers (PCH)
PRECOMPILED_HEADER = unstructured2dgridsewerimporter_pch.h

# iricGeoio library

LIBS += -liricGeoio

# iricGuibase library

LIBS += -liricGuibase

# iricGuicore library

LIBS += -liricGuicore

# iricMisc library

LIBS += -liricMisc

# shapelib

win32 {
        LIBS += -lshapelib_i
}

# VTK

LIBS += \
	-lvtkCommonCore-$${VTK_MAJ_MIN} \
	-lvtkCommonDataModel-$${VTK_MAJ_MIN}

win32 {
	DESTDIR = $(SolutionDir)/libdlls/$(Configuration)
	LIBS += -L$(SolutionDir)/libdlls/$(Configuration)
}

# Input
HEADERS += unstructured2dgridsewerimporter.h
SOURCES += unstructured2dgridsewerimporter.cpp
TRANSLATIONS += languages/unstructured2dgridsewerimporter_ar_EG.ts \
                languages/unstructured2dgridsewerimporter_bg_BG.ts \
                languages/unstructured2dgridsewerimporter_bs_BA.ts \
                languages/unstructured2dgridsewerimporter_ca_ES.ts \
                languages/unstructured2dgridsewerimporter_cs_CZ.ts \
                languages/unstructured2dgridsewerimporter_da_DK.ts \
                languages/unstructured2dgridsewerimporter_de_DE.ts \
                languages/unstructured2dgridsewerimporter_el_GR.ts \
                languages/unstructured2dgridsewerimporter_es_ES.ts \
                languages/unstructured2dgridsewerimporter_et_EE.ts \
                languages/unstructured2dgridsewerimporter_eu_ES.ts \
                languages/unstructured2dgridsewerimporter_fi_FI.ts \
                languages/unstructured2dgridsewerimporter_fr_FR.ts \
                languages/unstructured2dgridsewerimporter_gl_ES.ts \
                languages/unstructured2dgridsewerimporter_hi_IN.ts \
                languages/unstructured2dgridsewerimporter_hu_HU.ts \
                languages/unstructured2dgridsewerimporter_id_ID.ts \
                languages/unstructured2dgridsewerimporter_is_IS.ts \
                languages/unstructured2dgridsewerimporter_it_IT.ts \
                languages/unstructured2dgridsewerimporter_ja_JP.ts \
                languages/unstructured2dgridsewerimporter_ko_KR.ts \
                languages/unstructured2dgridsewerimporter_ky_KG.ts \
                languages/unstructured2dgridsewerimporter_lt_LT.ts \
                languages/unstructured2dgridsewerimporter_lv_LV.ts \
                languages/unstructured2dgridsewerimporter_nb_NO.ts \
                languages/unstructured2dgridsewerimporter_nl_NL.ts \
                languages/unstructured2dgridsewerimporter_pl_PL.ts \
                languages/unstructured2dgridsewerimporter_pt_BR.ts \
                languages/unstructured2dgridsewerimporter_pt_PT.ts \
                languages/unstructured2dgridsewerimporter_ro_RO.ts \
                languages/unstructured2dgridsewerimporter_ru_RU.ts \
                languages/unstructured2dgridsewerimporter_sl_SI.ts \
                languages/unstructured2dgridsewerimporter_sv_SE.ts \
                languages/unstructured2dgridsewerimporter_th_TH.ts \
                languages/unstructured2dgridsewerimporter_tr_TR.ts \
                languages/unstructured2dgridsewerimporter_uk_UA.ts \
                languages/unstructured2dgridsewerimporter_vi_VN.ts \
                languages/unstructured2dgridsewerimporter_zh_CN.ts \
                languages/unstructured2dgridsewerimporter_zh_TW.ts

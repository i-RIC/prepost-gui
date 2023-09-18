#-------------------------------------------------
#
# Project created by QtCreator 2014-10-17T11:05:29
#
#-------------------------------------------------

TARGET  = $$qtLibraryTarget(unstructured2dgridvtkimporter)
TEMPLATE = lib
CONFIG += plugin
QT       += widgets xml

include( ../../../paths.pri )

# Use Precompiled headers (PCH)
PRECOMPILED_HEADER = unstructured2dgridvtkimporter_pch.h

# iricGuicore library

LIBS += -liricGuibase
LIBS += -liricGuicore
LIBS += -liricMisc

# VTK

LIBS += \
	-lvtkCommonCore-$${VTK_MAJ_MIN} \
        -lvtkCommonDataModel-$${VTK_MAJ_MIN} \
        -lvtkIoLegacy-$${VTK_MAJ_MIN}

win32 {
	DESTDIR = $(SolutionDir)/libdlls/$(Configuration)
	LIBS += -L$(SolutionDir)/libdlls/$(Configuration)
}

# Input
HEADERS += unstructured2dgridvtkimporter.h \
    unstructured2dgridvtkimporter_pch.h
SOURCES += unstructured2dgridvtkimporter.cpp
TRANSLATIONS += languages/unstructured2dgridvtkimporter_ar_EG.ts \
                languages/unstructured2dgridvtkimporter_bg_BG.ts \
                languages/unstructured2dgridvtkimporter_bs_BA.ts \
                languages/unstructured2dgridvtkimporter_ca_ES.ts \
                languages/unstructured2dgridvtkimporter_cs_CZ.ts \
                languages/unstructured2dgridvtkimporter_da_DK.ts \
                languages/unstructured2dgridvtkimporter_de_DE.ts \
                languages/unstructured2dgridvtkimporter_el_GR.ts \
                languages/unstructured2dgridvtkimporter_es_ES.ts \
                languages/unstructured2dgridvtkimporter_et_EE.ts \
                languages/unstructured2dgridvtkimporter_eu_ES.ts \
                languages/unstructured2dgridvtkimporter_fi_FI.ts \
                languages/unstructured2dgridvtkimporter_fr_FR.ts \
                languages/unstructured2dgridvtkimporter_gl_ES.ts \
                languages/unstructured2dgridvtkimporter_hi_IN.ts \
                languages/unstructured2dgridvtkimporter_hu_HU.ts \
                languages/unstructured2dgridvtkimporter_id_ID.ts \
                languages/unstructured2dgridvtkimporter_is_IS.ts \
                languages/unstructured2dgridvtkimporter_it_IT.ts \
                languages/unstructured2dgridvtkimporter_ja_JP.ts \
                languages/unstructured2dgridvtkimporter_ko_KR.ts \
                languages/unstructured2dgridvtkimporter_ky_KG.ts \
                languages/unstructured2dgridvtkimporter_lt_LT.ts \
                languages/unstructured2dgridvtkimporter_lv_LV.ts \
                languages/unstructured2dgridvtkimporter_nb_NO.ts \
                languages/unstructured2dgridvtkimporter_nl_NL.ts \
                languages/unstructured2dgridvtkimporter_pl_PL.ts \
                languages/unstructured2dgridvtkimporter_pt_BR.ts \
                languages/unstructured2dgridvtkimporter_pt_PT.ts \
                languages/unstructured2dgridvtkimporter_ro_RO.ts \
                languages/unstructured2dgridvtkimporter_ru_RU.ts \
                languages/unstructured2dgridvtkimporter_sl_SI.ts \
                languages/unstructured2dgridvtkimporter_sv_SE.ts \
                languages/unstructured2dgridvtkimporter_th_TH.ts \
                languages/unstructured2dgridvtkimporter_tr_TR.ts \
                languages/unstructured2dgridvtkimporter_uk_UA.ts \
                languages/unstructured2dgridvtkimporter_vi_VN.ts \
                languages/unstructured2dgridvtkimporter_zh_CN.ts \
                languages/unstructured2dgridvtkimporter_zh_TW.ts

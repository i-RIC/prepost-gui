#-------------------------------------------------
#
# Project created by QtCreator 2014-10-17T11:05:29
#
#-------------------------------------------------

TARGET  = $$qtLibraryTarget(unstructured2dgridtriangleimporter)
TEMPLATE = lib
CONFIG += plugin
QT       += widgets xml

include( ../../../paths.pri )

# iricGuicore library

LIBS += -liricGuicore

# VTK

LIBS += \
	-lvtkCommonCore-$${VTK_MAJ_MIN} \
	-lvtkCommonDataModel-$${VTK_MAJ_MIN}

win32 {
	DESTDIR = $(SolutionDir)/libdlls/$(Configuration)
	LIBS += -L$(SolutionDir)/libdlls/$(Configuration)
}

# Input
HEADERS += unstructured2dgridtriangleimporter.h
SOURCES += unstructured2dgridtriangleimporter.cpp
TRANSLATIONS += languages/unstructured2dgridtriangleimporter_ar_EG.ts \
                languages/unstructured2dgridtriangleimporter_bg_BG.ts \
                languages/unstructured2dgridtriangleimporter_bs_BA.ts \
                languages/unstructured2dgridtriangleimporter_ca_ES.ts \
                languages/unstructured2dgridtriangleimporter_cs_CZ.ts \
                languages/unstructured2dgridtriangleimporter_da_DK.ts \
                languages/unstructured2dgridtriangleimporter_de_DE.ts \
                languages/unstructured2dgridtriangleimporter_el_GR.ts \
                languages/unstructured2dgridtriangleimporter_es_ES.ts \
                languages/unstructured2dgridtriangleimporter_et_EE.ts \
                languages/unstructured2dgridtriangleimporter_eu_ES.ts \
                languages/unstructured2dgridtriangleimporter_fi_FI.ts \
                languages/unstructured2dgridtriangleimporter_fr_FR.ts \
                languages/unstructured2dgridtriangleimporter_gl_ES.ts \
                languages/unstructured2dgridtriangleimporter_hi_IN.ts \
                languages/unstructured2dgridtriangleimporter_hu_HU.ts \
                languages/unstructured2dgridtriangleimporter_id_ID.ts \
                languages/unstructured2dgridtriangleimporter_is_IS.ts \
                languages/unstructured2dgridtriangleimporter_it_IT.ts \
                languages/unstructured2dgridtriangleimporter_ja_JP.ts \
                languages/unstructured2dgridtriangleimporter_ko_KR.ts \
                languages/unstructured2dgridtriangleimporter_ky_KG.ts \
                languages/unstructured2dgridtriangleimporter_lt_LT.ts \
                languages/unstructured2dgridtriangleimporter_lv_LV.ts \
                languages/unstructured2dgridtriangleimporter_nb_NO.ts \
                languages/unstructured2dgridtriangleimporter_nl_NL.ts \
                languages/unstructured2dgridtriangleimporter_pl_PL.ts \
                languages/unstructured2dgridtriangleimporter_pt_BR.ts \
                languages/unstructured2dgridtriangleimporter_pt_PT.ts \
                languages/unstructured2dgridtriangleimporter_ro_RO.ts \
                languages/unstructured2dgridtriangleimporter_ru_RU.ts \
                languages/unstructured2dgridtriangleimporter_sl_SI.ts \
                languages/unstructured2dgridtriangleimporter_sv_SE.ts \
                languages/unstructured2dgridtriangleimporter_th_TH.ts \
                languages/unstructured2dgridtriangleimporter_tr_TR.ts \
                languages/unstructured2dgridtriangleimporter_uk_UA.ts \
                languages/unstructured2dgridtriangleimporter_vi_VN.ts \
                languages/unstructured2dgridtriangleimporter_zh_CN.ts \
                languages/unstructured2dgridtriangleimporter_zh_TW.ts

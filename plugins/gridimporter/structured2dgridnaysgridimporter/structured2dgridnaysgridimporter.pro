#-------------------------------------------------
#
# Project created by QtCreator 2014-10-17T11:05:29
#
#-------------------------------------------------

TARGET  = $$qtLibraryTarget(structured2dgridnaysgridimporter)
TEMPLATE = lib
CONFIG += plugin
QT       += widgets xml

include( ../../../paths.pri )

# iricGuicore library

LIBS += -liricGuicore

# VTK

LIBS += \
	-lvtkCommonCore-6.1

win32 {
	DESTDIR = $(SolutionDir)/libdlls/$(Configuration)
	LIBS += -L$(SolutionDir)/libdlls/$(Configuration)
}

# Input
HEADERS += structured2dgridnaysgridimporter.h
SOURCES += structured2dgridnaysgridimporter.cpp
TRANSLATIONS += languages/structured2dgridnaysgridimporter_ar_EG.ts \
                languages/structured2dgridnaysgridimporter_bg_BG.ts \
                languages/structured2dgridnaysgridimporter_bs_BA.ts \
                languages/structured2dgridnaysgridimporter_ca_ES.ts \
                languages/structured2dgridnaysgridimporter_cs_CZ.ts \
                languages/structured2dgridnaysgridimporter_da_DK.ts \
                languages/structured2dgridnaysgridimporter_de_DE.ts \
                languages/structured2dgridnaysgridimporter_el_GR.ts \
                languages/structured2dgridnaysgridimporter_es_ES.ts \
                languages/structured2dgridnaysgridimporter_et_EE.ts \
                languages/structured2dgridnaysgridimporter_eu_ES.ts \
                languages/structured2dgridnaysgridimporter_fi_FI.ts \
                languages/structured2dgridnaysgridimporter_fr_FR.ts \
                languages/structured2dgridnaysgridimporter_gl_ES.ts \
                languages/structured2dgridnaysgridimporter_hi_IN.ts \
                languages/structured2dgridnaysgridimporter_hu_HU.ts \
                languages/structured2dgridnaysgridimporter_id_ID.ts \
                languages/structured2dgridnaysgridimporter_is_IS.ts \
                languages/structured2dgridnaysgridimporter_it_IT.ts \
                languages/structured2dgridnaysgridimporter_ja_JP.ts \
                languages/structured2dgridnaysgridimporter_ko_KR.ts \
                languages/structured2dgridnaysgridimporter_ky_KG.ts \
                languages/structured2dgridnaysgridimporter_lt_LT.ts \
                languages/structured2dgridnaysgridimporter_lv_LV.ts \
                languages/structured2dgridnaysgridimporter_nb_NO.ts \
                languages/structured2dgridnaysgridimporter_nl_NL.ts \
                languages/structured2dgridnaysgridimporter_pl_PL.ts \
                languages/structured2dgridnaysgridimporter_pt_BR.ts \
                languages/structured2dgridnaysgridimporter_pt_PT.ts \
                languages/structured2dgridnaysgridimporter_ro_RO.ts \
                languages/structured2dgridnaysgridimporter_ru_RU.ts \
                languages/structured2dgridnaysgridimporter_sl_SI.ts \
                languages/structured2dgridnaysgridimporter_sv_SE.ts \
                languages/structured2dgridnaysgridimporter_th_TH.ts \
                languages/structured2dgridnaysgridimporter_tr_TR.ts \
                languages/structured2dgridnaysgridimporter_uk_UA.ts \
                languages/structured2dgridnaysgridimporter_vi_VN.ts \
                languages/structured2dgridnaysgridimporter_zh_CN.ts \
                languages/structured2dgridnaysgridimporter_zh_TW.ts

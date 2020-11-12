#-------------------------------------------------
#
# Project created by QtCreator 2014-10-17T11:05:29
#
#-------------------------------------------------

TARGET  = $$qtLibraryTarget(structured2dgridnayscsvexporter)
QT       += widgets xml
TEMPLATE = lib
CONFIG += plugin

include( ../../../paths.pri )

# iricGuicore library

unix {
	LIBS += -L"../../../libs/guicore"
}
LIBS += -liricGuicore

# VTK

LIBS += \
	-lvtkCommonCore-$${VTK_MAJ_MIN}

win32 {
	DESTDIR = $(SolutionDir)/libdlls/$(Configuration)
	LIBS += -L$(SolutionDir)/libdlls/$(Configuration)
}

# Input
HEADERS += structured2dgridnayscsvexporter.h
SOURCES += structured2dgridnayscsvexporter.cpp
TRANSLATIONS += languages/structured2dgridnayscsvexporter_ar_EG.ts \
                languages/structured2dgridnayscsvexporter_bg_BG.ts \
                languages/structured2dgridnayscsvexporter_bs_BA.ts \
                languages/structured2dgridnayscsvexporter_ca_ES.ts \
                languages/structured2dgridnayscsvexporter_cs_CZ.ts \
                languages/structured2dgridnayscsvexporter_da_DK.ts \
                languages/structured2dgridnayscsvexporter_de_DE.ts \
                languages/structured2dgridnayscsvexporter_el_GR.ts \
                languages/structured2dgridnayscsvexporter_es_ES.ts \
                languages/structured2dgridnayscsvexporter_et_EE.ts \
                languages/structured2dgridnayscsvexporter_eu_ES.ts \
                languages/structured2dgridnayscsvexporter_fi_FI.ts \
                languages/structured2dgridnayscsvexporter_fr_FR.ts \
                languages/structured2dgridnayscsvexporter_gl_ES.ts \
                languages/structured2dgridnayscsvexporter_hi_IN.ts \
                languages/structured2dgridnayscsvexporter_hu_HU.ts \
                languages/structured2dgridnayscsvexporter_id_ID.ts \
                languages/structured2dgridnayscsvexporter_is_IS.ts \
                languages/structured2dgridnayscsvexporter_it_IT.ts \
                languages/structured2dgridnayscsvexporter_ja_JP.ts \
                languages/structured2dgridnayscsvexporter_ko_KR.ts \
                languages/structured2dgridnayscsvexporter_ky_KG.ts \
                languages/structured2dgridnayscsvexporter_lt_LT.ts \
                languages/structured2dgridnayscsvexporter_lv_LV.ts \
                languages/structured2dgridnayscsvexporter_nb_NO.ts \
                languages/structured2dgridnayscsvexporter_nl_NL.ts \
                languages/structured2dgridnayscsvexporter_pl_PL.ts \
                languages/structured2dgridnayscsvexporter_pt_BR.ts \
                languages/structured2dgridnayscsvexporter_pt_PT.ts \
                languages/structured2dgridnayscsvexporter_ro_RO.ts \
                languages/structured2dgridnayscsvexporter_ru_RU.ts \
                languages/structured2dgridnayscsvexporter_sl_SI.ts \
                languages/structured2dgridnayscsvexporter_sv_SE.ts \
                languages/structured2dgridnayscsvexporter_th_TH.ts \
                languages/structured2dgridnayscsvexporter_tr_TR.ts \
                languages/structured2dgridnayscsvexporter_uk_UA.ts \
                languages/structured2dgridnayscsvexporter_vi_VN.ts \
                languages/structured2dgridnayscsvexporter_zh_CN.ts \
                languages/structured2dgridnayscsvexporter_zh_TW.ts

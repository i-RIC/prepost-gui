#-------------------------------------------------
#
# Project created by QtCreator 2014-10-17T11:05:29
#
#-------------------------------------------------

TARGET  = $$qtLibraryTarget(structured2dgridnayscsvimporter)
QT       += widgets xml
TEMPLATE = lib
CONFIG += plugin

include( ../../../paths.pri )

# Use Precompiled headers (PCH)
PRECOMPILED_HEADER = structured2dgridnayscsvimporter_pch.h

# iricGuicore library

unix {
	LIBS += -L"../../../libs/guicore"
}
LIBS += -liricGuicore

# iricMisc library

unix {
        LIBS += -L"../../../libs/misc"
}
LIBS += -liricMisc

# VTK

LIBS += \
	-lvtkCommonCore-$${VTK_MAJ_MIN}

win32 {
	DESTDIR = $(SolutionDir)/libdlls/$(Configuration)
	LIBS += -L$(SolutionDir)/libdlls/$(Configuration)
}

# Input
HEADERS += structured2dgridnayscsvimporter.h
SOURCES += structured2dgridnayscsvimporter.cpp
TRANSLATIONS += languages/structured2dgridnayscsvimporter_ar_EG.ts \
                languages/structured2dgridnayscsvimporter_bg_BG.ts \
                languages/structured2dgridnayscsvimporter_bs_BA.ts \
                languages/structured2dgridnayscsvimporter_ca_ES.ts \
                languages/structured2dgridnayscsvimporter_cs_CZ.ts \
                languages/structured2dgridnayscsvimporter_da_DK.ts \
                languages/structured2dgridnayscsvimporter_de_DE.ts \
                languages/structured2dgridnayscsvimporter_el_GR.ts \
                languages/structured2dgridnayscsvimporter_es_ES.ts \
                languages/structured2dgridnayscsvimporter_et_EE.ts \
                languages/structured2dgridnayscsvimporter_eu_ES.ts \
                languages/structured2dgridnayscsvimporter_fi_FI.ts \
                languages/structured2dgridnayscsvimporter_fr_FR.ts \
                languages/structured2dgridnayscsvimporter_gl_ES.ts \
                languages/structured2dgridnayscsvimporter_hi_IN.ts \
                languages/structured2dgridnayscsvimporter_hu_HU.ts \
                languages/structured2dgridnayscsvimporter_id_ID.ts \
                languages/structured2dgridnayscsvimporter_is_IS.ts \
                languages/structured2dgridnayscsvimporter_it_IT.ts \
                languages/structured2dgridnayscsvimporter_ja_JP.ts \
                languages/structured2dgridnayscsvimporter_ko_KR.ts \
                languages/structured2dgridnayscsvimporter_ky_KG.ts \
                languages/structured2dgridnayscsvimporter_lt_LT.ts \
                languages/structured2dgridnayscsvimporter_lv_LV.ts \
                languages/structured2dgridnayscsvimporter_nb_NO.ts \
                languages/structured2dgridnayscsvimporter_nl_NL.ts \
                languages/structured2dgridnayscsvimporter_pl_PL.ts \
                languages/structured2dgridnayscsvimporter_pt_BR.ts \
                languages/structured2dgridnayscsvimporter_pt_PT.ts \
                languages/structured2dgridnayscsvimporter_ro_RO.ts \
                languages/structured2dgridnayscsvimporter_ru_RU.ts \
                languages/structured2dgridnayscsvimporter_sl_SI.ts \
                languages/structured2dgridnayscsvimporter_sv_SE.ts \
                languages/structured2dgridnayscsvimporter_th_TH.ts \
                languages/structured2dgridnayscsvimporter_tr_TR.ts \
                languages/structured2dgridnayscsvimporter_uk_UA.ts \
                languages/structured2dgridnayscsvimporter_vi_VN.ts \
                languages/structured2dgridnayscsvimporter_zh_CN.ts \
                languages/structured2dgridnayscsvimporter_zh_TW.ts

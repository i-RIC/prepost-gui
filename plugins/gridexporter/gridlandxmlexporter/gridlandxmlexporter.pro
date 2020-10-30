TARGET  = $$qtLibraryTarget(gridlandxmlexporter)
QT       += widgets xml
TEMPLATE = lib
CONFIG += plugin

include( ../../../paths.pri )

# iricGuibase library

unix {
	LIBS += -L"../../../libs/guibase"
}
LIBS += -liricGuibase

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
  -lvtkCommonCore-6.1 \
  -lvtkCommonDataModel-6.1

win32 {
	DESTDIR = $(SolutionDir)/libdlls/$(Configuration)
	LIBS += -L$(SolutionDir)/libdlls/$(Configuration)
}

# Input
HEADERS += gridlandxmlexporter.h
SOURCES += gridlandxmlexporter.cpp
TRANSLATIONS += languages/gridlandxmlexporter_ar_EG.ts \
                languages/gridlandxmlexporter_bg_BG.ts \
                languages/gridlandxmlexporter_bs_BA.ts \
                languages/gridlandxmlexporter_ca_ES.ts \
                languages/gridlandxmlexporter_cs_CZ.ts \
                languages/gridlandxmlexporter_da_DK.ts \
                languages/gridlandxmlexporter_de_DE.ts \
                languages/gridlandxmlexporter_el_GR.ts \
                languages/gridlandxmlexporter_es_ES.ts \
                languages/gridlandxmlexporter_et_EE.ts \
                languages/gridlandxmlexporter_eu_ES.ts \
                languages/gridlandxmlexporter_fi_FI.ts \
                languages/gridlandxmlexporter_fr_FR.ts \
                languages/gridlandxmlexporter_gl_ES.ts \
                languages/gridlandxmlexporter_hi_IN.ts \
                languages/gridlandxmlexporter_hu_HU.ts \
                languages/gridlandxmlexporter_id_ID.ts \
                languages/gridlandxmlexporter_is_IS.ts \
                languages/gridlandxmlexporter_it_IT.ts \
                languages/gridlandxmlexporter_ja_JP.ts \
                languages/gridlandxmlexporter_ko_KR.ts \
                languages/gridlandxmlexporter_ky_KG.ts \
                languages/gridlandxmlexporter_lt_LT.ts \
                languages/gridlandxmlexporter_lv_LV.ts \
                languages/gridlandxmlexporter_nb_NO.ts \
                languages/gridlandxmlexporter_nl_NL.ts \
                languages/gridlandxmlexporter_pl_PL.ts \
                languages/gridlandxmlexporter_pt_BR.ts \
                languages/gridlandxmlexporter_pt_PT.ts \
                languages/gridlandxmlexporter_ro_RO.ts \
                languages/gridlandxmlexporter_ru_RU.ts \
                languages/gridlandxmlexporter_sl_SI.ts \
                languages/gridlandxmlexporter_sv_SE.ts \
                languages/gridlandxmlexporter_th_TH.ts \
                languages/gridlandxmlexporter_tr_TR.ts \
                languages/gridlandxmlexporter_uk_UA.ts \
                languages/gridlandxmlexporter_vi_VN.ts \
                languages/gridlandxmlexporter_zh_CN.ts \
                languages/gridlandxmlexporter_zh_TW.ts

#-------------------------------------------------
#
# Project created by QtCreator 2014-10-17T11:05:29
#
#-------------------------------------------------

TARGET  = $$qtLibraryTarget(structured2dgridvtkexporter)
QT       += widgets xml
TEMPLATE = lib
CONFIG += plugin

include( ../../../paths.pri )

# iricGuicore library

win32 {
	CONFIG(debug, debug|release) {
		LIBS += -L"../../../libs/guicore/debug"
	} else {
		LIBS += -L"../../../libs/guicore/release"
	}
}
unix {
	LIBS += -L"../../../libs/guicore"
}
LIBS += -liricGuicore

# iricMisc library

win32 {
	CONFIG(debug, debug|release) {
		LIBS += -L"../../../libs/misc/debug"
	} else {
		LIBS += -L"../../../libs/misc/release"
	}
}
unix {
	LIBS += -L"../../../libs/misc"
}
LIBS += -liricMisc

# VTK

LIBS += \
	-lvtkCommonCore-6.1 \
	-lvtkCommonDataModel-6.1 \
	-lvtkIOCore-6.1 \
	-lvtkIOLegacy-6.1

# Input
HEADERS += structured2dgridvtkexporter.h
SOURCES += structured2dgridvtkexporter.cpp
TRANSLATIONS += languages/structured2dgridvtkexporter_ar_EG.ts \
                languages/structured2dgridvtkexporter_bg_BG.ts \
                languages/structured2dgridvtkexporter_bs_BA.ts \
                languages/structured2dgridvtkexporter_ca_ES.ts \
                languages/structured2dgridvtkexporter_cs_CZ.ts \
                languages/structured2dgridvtkexporter_da_DK.ts \
                languages/structured2dgridvtkexporter_de_DE.ts \
                languages/structured2dgridvtkexporter_el_GR.ts \
                languages/structured2dgridvtkexporter_es_ES.ts \
                languages/structured2dgridvtkexporter_et_EE.ts \
                languages/structured2dgridvtkexporter_eu_ES.ts \
                languages/structured2dgridvtkexporter_fi_FI.ts \
                languages/structured2dgridvtkexporter_fr_FR.ts \
                languages/structured2dgridvtkexporter_gl_ES.ts \
                languages/structured2dgridvtkexporter_hi_IN.ts \
                languages/structured2dgridvtkexporter_hu_HU.ts \
                languages/structured2dgridvtkexporter_id_ID.ts \
                languages/structured2dgridvtkexporter_is_IS.ts \
                languages/structured2dgridvtkexporter_it_IT.ts \
                languages/structured2dgridvtkexporter_ja_JP.ts \
                languages/structured2dgridvtkexporter_ko_KR.ts \
                languages/structured2dgridvtkexporter_ky_KG.ts \
                languages/structured2dgridvtkexporter_lt_LT.ts \
                languages/structured2dgridvtkexporter_lv_LV.ts \
                languages/structured2dgridvtkexporter_nb_NO.ts \
                languages/structured2dgridvtkexporter_nl_NL.ts \
                languages/structured2dgridvtkexporter_pl_PL.ts \
                languages/structured2dgridvtkexporter_pt_BR.ts \
                languages/structured2dgridvtkexporter_pt_PT.ts \
                languages/structured2dgridvtkexporter_ro_RO.ts \
                languages/structured2dgridvtkexporter_ru_RU.ts \
                languages/structured2dgridvtkexporter_sl_SI.ts \
                languages/structured2dgridvtkexporter_sv_SE.ts \
                languages/structured2dgridvtkexporter_th_TH.ts \
                languages/structured2dgridvtkexporter_tr_TR.ts \
                languages/structured2dgridvtkexporter_uk_UA.ts \
                languages/structured2dgridvtkexporter_vi_VN.ts \
                languages/structured2dgridvtkexporter_zh_CN.ts \
                languages/structured2dgridvtkexporter_zh_TW.ts

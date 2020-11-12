TARGET  = $$qtLibraryTarget(structured15dgridwithcrosssectionhecrasexporter)
QT       += widgets xml
TEMPLATE = lib
CONFIG += plugin

include( ../../../paths.pri )

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
HEADERS += structured15dgridwithcrosssectionhecrasexporter.h
SOURCES += structured15dgridwithcrosssectionhecrasexporter.cpp
TRANSLATIONS += languages/structured15dgridwithcrosssectionhecrasexporter_ar_EG.ts \
                languages/structured15dgridwithcrosssectionhecrasexporter_bg_BG.ts \
                languages/structured15dgridwithcrosssectionhecrasexporter_bs_BA.ts \
                languages/structured15dgridwithcrosssectionhecrasexporter_ca_ES.ts \
                languages/structured15dgridwithcrosssectionhecrasexporter_cs_CZ.ts \
                languages/structured15dgridwithcrosssectionhecrasexporter_da_DK.ts \
                languages/structured15dgridwithcrosssectionhecrasexporter_de_DE.ts \
                languages/structured15dgridwithcrosssectionhecrasexporter_el_GR.ts \
                languages/structured15dgridwithcrosssectionhecrasexporter_es_ES.ts \
                languages/structured15dgridwithcrosssectionhecrasexporter_et_EE.ts \
                languages/structured15dgridwithcrosssectionhecrasexporter_eu_ES.ts \
                languages/structured15dgridwithcrosssectionhecrasexporter_fi_FI.ts \
                languages/structured15dgridwithcrosssectionhecrasexporter_fr_FR.ts \
                languages/structured15dgridwithcrosssectionhecrasexporter_gl_ES.ts \
                languages/structured15dgridwithcrosssectionhecrasexporter_hi_IN.ts \
                languages/structured15dgridwithcrosssectionhecrasexporter_hu_HU.ts \
                languages/structured15dgridwithcrosssectionhecrasexporter_id_ID.ts \
                languages/structured15dgridwithcrosssectionhecrasexporter_is_IS.ts \
                languages/structured15dgridwithcrosssectionhecrasexporter_it_IT.ts \
                languages/structured15dgridwithcrosssectionhecrasexporter_ja_JP.ts \
                languages/structured15dgridwithcrosssectionhecrasexporter_ko_KR.ts \
                languages/structured15dgridwithcrosssectionhecrasexporter_ky_KG.ts \
                languages/structured15dgridwithcrosssectionhecrasexporter_lt_LT.ts \
                languages/structured15dgridwithcrosssectionhecrasexporter_lv_LV.ts \
                languages/structured15dgridwithcrosssectionhecrasexporter_nb_NO.ts \
                languages/structured15dgridwithcrosssectionhecrasexporter_nl_NL.ts \
                languages/structured15dgridwithcrosssectionhecrasexporter_pl_PL.ts \
                languages/structured15dgridwithcrosssectionhecrasexporter_pt_BR.ts \
                languages/structured15dgridwithcrosssectionhecrasexporter_pt_PT.ts \
                languages/structured15dgridwithcrosssectionhecrasexporter_ro_RO.ts \
                languages/structured15dgridwithcrosssectionhecrasexporter_ru_RU.ts \
                languages/structured15dgridwithcrosssectionhecrasexporter_sl_SI.ts \
                languages/structured15dgridwithcrosssectionhecrasexporter_sv_SE.ts \
                languages/structured15dgridwithcrosssectionhecrasexporter_th_TH.ts \
                languages/structured15dgridwithcrosssectionhecrasexporter_tr_TR.ts \
                languages/structured15dgridwithcrosssectionhecrasexporter_uk_UA.ts \
                languages/structured15dgridwithcrosssectionhecrasexporter_vi_VN.ts \
                languages/structured15dgridwithcrosssectionhecrasexporter_zh_CN.ts \
                languages/structured15dgridwithcrosssectionhecrasexporter_zh_TW.ts

TARGET  = $$qtLibraryTarget(structured2dgridgdalimporter)
TEMPLATE = lib
CONFIG += plugin
QT       += widgets xml

include( ../../../paths.pri )

# iricGuicore library

LIBS += -liricGuicore

# iricMisc library

LIBS += -liricMisc

#gdal

win32 {
        LIBS += -lgdal_i
}
unix {
        LIBS += -lgdal
}

# VTK

LIBS += \
	-lvtkCommonCore-$${VTK_MAJ_MIN}

win32 {
	DESTDIR = $(SolutionDir)/libdlls/$(Configuration)
	LIBS += -L$(SolutionDir)/libdlls/$(Configuration)
}

# Input
HEADERS += structured2dgridgdalimporter.h
SOURCES += structured2dgridgdalimporter.cpp
TRANSLATIONS += languages/structured2dgridgdalimporter_ar_EG.ts \
                languages/structured2dgridgdalimporter_bg_BG.ts \
                languages/structured2dgridgdalimporter_bs_BA.ts \
                languages/structured2dgridgdalimporter_ca_ES.ts \
                languages/structured2dgridgdalimporter_cs_CZ.ts \
                languages/structured2dgridgdalimporter_da_DK.ts \
                languages/structured2dgridgdalimporter_de_DE.ts \
                languages/structured2dgridgdalimporter_el_GR.ts \
                languages/structured2dgridgdalimporter_es_ES.ts \
                languages/structured2dgridgdalimporter_et_EE.ts \
                languages/structured2dgridgdalimporter_eu_ES.ts \
                languages/structured2dgridgdalimporter_fi_FI.ts \
                languages/structured2dgridgdalimporter_fr_FR.ts \
                languages/structured2dgridgdalimporter_gl_ES.ts \
                languages/structured2dgridgdalimporter_hi_IN.ts \
                languages/structured2dgridgdalimporter_hu_HU.ts \
                languages/structured2dgridgdalimporter_id_ID.ts \
                languages/structured2dgridgdalimporter_is_IS.ts \
                languages/structured2dgridgdalimporter_it_IT.ts \
                languages/structured2dgridgdalimporter_ja_JP.ts \
                languages/structured2dgridgdalimporter_ko_KR.ts \
                languages/structured2dgridgdalimporter_ky_KG.ts \
                languages/structured2dgridgdalimporter_lt_LT.ts \
                languages/structured2dgridgdalimporter_lv_LV.ts \
                languages/structured2dgridgdalimporter_nb_NO.ts \
                languages/structured2dgridgdalimporter_nl_NL.ts \
                languages/structured2dgridgdalimporter_pl_PL.ts \
                languages/structured2dgridgdalimporter_pt_BR.ts \
                languages/structured2dgridgdalimporter_pt_PT.ts \
                languages/structured2dgridgdalimporter_ro_RO.ts \
                languages/structured2dgridgdalimporter_ru_RU.ts \
                languages/structured2dgridgdalimporter_sl_SI.ts \
                languages/structured2dgridgdalimporter_sv_SE.ts \
                languages/structured2dgridgdalimporter_th_TH.ts \
                languages/structured2dgridgdalimporter_tr_TR.ts \
                languages/structured2dgridgdalimporter_uk_UA.ts \
                languages/structured2dgridgdalimporter_vi_VN.ts \
                languages/structured2dgridgdalimporter_zh_CN.ts \
                languages/structured2dgridgdalimporter_zh_TW.ts

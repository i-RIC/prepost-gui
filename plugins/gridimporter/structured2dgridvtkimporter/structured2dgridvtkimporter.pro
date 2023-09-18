#-------------------------------------------------
#
# Project created by QtCreator 2014-10-17T11:05:29
#
#-------------------------------------------------

TARGET  = $$qtLibraryTarget(structured2dgridvtkimporter)
TEMPLATE = lib
CONFIG += plugin
QT       += widgets xml

include( ../../../paths.pri )

# Use Precompiled headers (PCH)
PRECOMPILED_HEADER = structured2dgridvtkimporter_pch.h

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
	-lvtkCommonCore-$${VTK_MAJ_MIN} \
	-lvtkCommonDataModel-$${VTK_MAJ_MIN} \
	-lvtkIoLegacy-$${VTK_MAJ_MIN}

win32 {
	DESTDIR = $(SolutionDir)/libdlls/$(Configuration)
	LIBS += -L$(SolutionDir)/libdlls/$(Configuration)
}

# Input
HEADERS += structured2dgridvtkimporter.h \
    structured2dgridvtkimporter_pch.h
SOURCES += structured2dgridvtkimporter.cpp
TRANSLATIONS += languages/structured2dgridvtkimporter_ar_EG.ts \
                languages/structured2dgridvtkimporter_bg_BG.ts \
                languages/structured2dgridvtkimporter_bs_BA.ts \
                languages/structured2dgridvtkimporter_ca_ES.ts \
                languages/structured2dgridvtkimporter_cs_CZ.ts \
                languages/structured2dgridvtkimporter_da_DK.ts \
                languages/structured2dgridvtkimporter_de_DE.ts \
                languages/structured2dgridvtkimporter_el_GR.ts \
                languages/structured2dgridvtkimporter_es_ES.ts \
                languages/structured2dgridvtkimporter_et_EE.ts \
                languages/structured2dgridvtkimporter_eu_ES.ts \
                languages/structured2dgridvtkimporter_fi_FI.ts \
                languages/structured2dgridvtkimporter_fr_FR.ts \
                languages/structured2dgridvtkimporter_gl_ES.ts \
                languages/structured2dgridvtkimporter_hi_IN.ts \
                languages/structured2dgridvtkimporter_hu_HU.ts \
                languages/structured2dgridvtkimporter_id_ID.ts \
                languages/structured2dgridvtkimporter_is_IS.ts \
                languages/structured2dgridvtkimporter_it_IT.ts \
                languages/structured2dgridvtkimporter_ja_JP.ts \
                languages/structured2dgridvtkimporter_ko_KR.ts \
                languages/structured2dgridvtkimporter_ky_KG.ts \
                languages/structured2dgridvtkimporter_lt_LT.ts \
                languages/structured2dgridvtkimporter_lv_LV.ts \
                languages/structured2dgridvtkimporter_nb_NO.ts \
                languages/structured2dgridvtkimporter_nl_NL.ts \
                languages/structured2dgridvtkimporter_pl_PL.ts \
                languages/structured2dgridvtkimporter_pt_BR.ts \
                languages/structured2dgridvtkimporter_pt_PT.ts \
                languages/structured2dgridvtkimporter_ro_RO.ts \
                languages/structured2dgridvtkimporter_ru_RU.ts \
                languages/structured2dgridvtkimporter_sl_SI.ts \
                languages/structured2dgridvtkimporter_sv_SE.ts \
                languages/structured2dgridvtkimporter_th_TH.ts \
                languages/structured2dgridvtkimporter_tr_TR.ts \
                languages/structured2dgridvtkimporter_uk_UA.ts \
                languages/structured2dgridvtkimporter_vi_VN.ts \
                languages/structured2dgridvtkimporter_zh_CN.ts \
                languages/structured2dgridvtkimporter_zh_TW.ts

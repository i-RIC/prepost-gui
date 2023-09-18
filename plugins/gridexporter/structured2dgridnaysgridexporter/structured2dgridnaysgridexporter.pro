#-------------------------------------------------
#
# Project created by QtCreator 2014-10-17T11:05:29
#
#-------------------------------------------------

TARGET  = $$qtLibraryTarget(structured2dgridnaysgridexporter)
QT       += widgets xml
TEMPLATE = lib
CONFIG += plugin

include( ../../../paths.pri )

# Use Precompiled headers (PCH)
PRECOMPILED_HEADER = structured2dgridnaysgridexporter_pch.h

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

# VTK

LIBS += \
	-lvtkCommonCore-$${VTK_MAJ_MIN} \
	-lvtkCommonDataModel-$${VTK_MAJ_MIN}

win32 {
	DESTDIR = $(SolutionDir)/libdlls/$(Configuration)
	LIBS += -L$(SolutionDir)/libdlls/$(Configuration)
}

# Input
HEADERS += structured2dgridnaysgridexporter.h
SOURCES += structured2dgridnaysgridexporter.cpp
TRANSLATIONS += languages/structured2dgridnaysgridexporter_ar_EG.ts \
                languages/structured2dgridnaysgridexporter_bg_BG.ts \
                languages/structured2dgridnaysgridexporter_bs_BA.ts \
                languages/structured2dgridnaysgridexporter_ca_ES.ts \
                languages/structured2dgridnaysgridexporter_cs_CZ.ts \
                languages/structured2dgridnaysgridexporter_da_DK.ts \
                languages/structured2dgridnaysgridexporter_de_DE.ts \
                languages/structured2dgridnaysgridexporter_el_GR.ts \
                languages/structured2dgridnaysgridexporter_es_ES.ts \
                languages/structured2dgridnaysgridexporter_et_EE.ts \
                languages/structured2dgridnaysgridexporter_eu_ES.ts \
                languages/structured2dgridnaysgridexporter_fi_FI.ts \
                languages/structured2dgridnaysgridexporter_fr_FR.ts \
                languages/structured2dgridnaysgridexporter_gl_ES.ts \
                languages/structured2dgridnaysgridexporter_hi_IN.ts \
                languages/structured2dgridnaysgridexporter_hu_HU.ts \
                languages/structured2dgridnaysgridexporter_id_ID.ts \
                languages/structured2dgridnaysgridexporter_is_IS.ts \
                languages/structured2dgridnaysgridexporter_it_IT.ts \
                languages/structured2dgridnaysgridexporter_ja_JP.ts \
                languages/structured2dgridnaysgridexporter_ko_KR.ts \
                languages/structured2dgridnaysgridexporter_ky_KG.ts \
                languages/structured2dgridnaysgridexporter_lt_LT.ts \
                languages/structured2dgridnaysgridexporter_lv_LV.ts \
                languages/structured2dgridnaysgridexporter_nb_NO.ts \
                languages/structured2dgridnaysgridexporter_nl_NL.ts \
                languages/structured2dgridnaysgridexporter_pl_PL.ts \
                languages/structured2dgridnaysgridexporter_pt_BR.ts \
                languages/structured2dgridnaysgridexporter_pt_PT.ts \
                languages/structured2dgridnaysgridexporter_ro_RO.ts \
                languages/structured2dgridnaysgridexporter_ru_RU.ts \
                languages/structured2dgridnaysgridexporter_sl_SI.ts \
                languages/structured2dgridnaysgridexporter_sv_SE.ts \
                languages/structured2dgridnaysgridexporter_th_TH.ts \
                languages/structured2dgridnaysgridexporter_tr_TR.ts \
                languages/structured2dgridnaysgridexporter_uk_UA.ts \
                languages/structured2dgridnaysgridexporter_vi_VN.ts \
                languages/structured2dgridnaysgridexporter_zh_CN.ts \
                languages/structured2dgridnaysgridexporter_zh_TW.ts

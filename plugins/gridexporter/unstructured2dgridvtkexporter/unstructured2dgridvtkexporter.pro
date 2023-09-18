#-------------------------------------------------
#
# Project created by QtCreator 2014-10-17T11:05:29
#
#-------------------------------------------------

TARGET  = $$qtLibraryTarget(unstructured2dgridvtkexporter)
QT       += widgets xml
TEMPLATE = lib
CONFIG += plugin

include( ../../../paths.pri )

# Use Precompiled headers (PCH)
PRECOMPILED_HEADER = unstructured2dgridvtkexporter_pch.h

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
	-lvtkIOCore-$${VTK_MAJ_MIN} \
	-lvtkIOLegacy-$${VTK_MAJ_MIN}

win32 {
	DESTDIR = $(SolutionDir)/libdlls/$(Configuration)
	LIBS += -L$(SolutionDir)/libdlls/$(Configuration)
}

# Input
HEADERS += unstructured2dgridvtkexporter.h
SOURCES += unstructured2dgridvtkexporter.cpp
TRANSLATIONS += languages/unstructured2dgridvtkexporter_ar_EG.ts \
                languages/unstructured2dgridvtkexporter_bg_BG.ts \
                languages/unstructured2dgridvtkexporter_bs_BA.ts \
                languages/unstructured2dgridvtkexporter_ca_ES.ts \
                languages/unstructured2dgridvtkexporter_cs_CZ.ts \
                languages/unstructured2dgridvtkexporter_da_DK.ts \
                languages/unstructured2dgridvtkexporter_de_DE.ts \
                languages/unstructured2dgridvtkexporter_el_GR.ts \
                languages/unstructured2dgridvtkexporter_es_ES.ts \
                languages/unstructured2dgridvtkexporter_et_EE.ts \
                languages/unstructured2dgridvtkexporter_eu_ES.ts \
                languages/unstructured2dgridvtkexporter_fi_FI.ts \
                languages/unstructured2dgridvtkexporter_fr_FR.ts \
                languages/unstructured2dgridvtkexporter_gl_ES.ts \
                languages/unstructured2dgridvtkexporter_hi_IN.ts \
                languages/unstructured2dgridvtkexporter_hu_HU.ts \
                languages/unstructured2dgridvtkexporter_id_ID.ts \
                languages/unstructured2dgridvtkexporter_is_IS.ts \
                languages/unstructured2dgridvtkexporter_it_IT.ts \
                languages/unstructured2dgridvtkexporter_ja_JP.ts \
                languages/unstructured2dgridvtkexporter_ko_KR.ts \
                languages/unstructured2dgridvtkexporter_ky_KG.ts \
                languages/unstructured2dgridvtkexporter_lt_LT.ts \
                languages/unstructured2dgridvtkexporter_lv_LV.ts \
                languages/unstructured2dgridvtkexporter_nb_NO.ts \
                languages/unstructured2dgridvtkexporter_nl_NL.ts \
                languages/unstructured2dgridvtkexporter_pl_PL.ts \
                languages/unstructured2dgridvtkexporter_pt_BR.ts \
                languages/unstructured2dgridvtkexporter_pt_PT.ts \
                languages/unstructured2dgridvtkexporter_ro_RO.ts \
                languages/unstructured2dgridvtkexporter_ru_RU.ts \
                languages/unstructured2dgridvtkexporter_sl_SI.ts \
                languages/unstructured2dgridvtkexporter_sv_SE.ts \
                languages/unstructured2dgridvtkexporter_th_TH.ts \
                languages/unstructured2dgridvtkexporter_tr_TR.ts \
                languages/unstructured2dgridvtkexporter_uk_UA.ts \
                languages/unstructured2dgridvtkexporter_vi_VN.ts \
                languages/unstructured2dgridvtkexporter_zh_CN.ts \
                languages/unstructured2dgridvtkexporter_zh_TW.ts

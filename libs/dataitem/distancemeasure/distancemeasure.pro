TARGET = iricDistancemeasure
TEMPLATE = lib
INCLUDEPATH += .

DEFINES += DISTANCEMEASURE_LIBRARY

include( ../../../paths.pri )

QT += widgets xml

######################
# Internal libraries #
######################

# iricGuibase

unix {
	LIBS += -L"../../guibase"
}
LIBS += -liricGuibase

# iricGuicore

unix {
	LIBS += -L"../../guicore"
}
LIBS += -liricGuicore

# iricMisc

unix {
	LIBS += -L"../../misc"
}
LIBS += -liricMisc

######################
# External libraries #
######################

# VTK

LIBS += \
	-lvtkCommonCore-$${VTK_MAJ_MIN} \
	-lvtkCommonDataModel-$${VTK_MAJ_MIN} \
	-lvtkRenderingCore-$${VTK_MAJ_MIN} \
	-lvtkRenderingLabel-$${VTK_MAJ_MIN}

win32 {
	DESTDIR = $(SolutionDir)/libdlls/$(Configuration)
	LIBS += -L$(SolutionDir)/libdlls/$(Configuration)
}

# Input
HEADERS += distancemeasure_api.h \
           distancemeasuredataitem.h \
           distancemeasuregroupdataitem.h \
           distancemeasurepropertydialog.h \
           distancemeasuresetting.h \
           private/distancemeasuredataitem_definecommand.h \
           private/distancemeasuredataitem_impl.h \
           private/distancemeasuredataitem_movevertexcommand.h \
           private/distancemeasuredataitem_redefinecommand.h \
           private/distancemeasuredataitem_setsettingcommand.h \
           private/distancemeasuredataitem_translatecommand.h \
           private/distancemeasuregroupdataitem_impl.h
FORMS += distancemeasurepropertydialog.ui
SOURCES += \
           distancemeasuredataitem.cpp \
           distancemeasuregroupdataitem.cpp \
           distancemeasurepropertydialog.cpp \
           distancemeasuresetting.cpp \
           private/distancemeasuredataitem_definecommand.cpp \
           private/distancemeasuredataitem_movevertexcommand.cpp \
           private/distancemeasuredataitem_redefinecommand.cpp \
           private/distancemeasuredataitem_setsettingcommand.cpp \
           private/distancemeasuredataitem_translatecommand.cpp
TRANSLATIONS += languages/iricDistancemeasure_ar_EG.ts \
                languages/iricDistancemeasure_bg_BG.ts \
                languages/iricDistancemeasure_bs_BA.ts \
                languages/iricDistancemeasure_ca_ES.ts \
                languages/iricDistancemeasure_cs_CZ.ts \
                languages/iricDistancemeasure_da_DK.ts \
                languages/iricDistancemeasure_de_DE.ts \
                languages/iricDistancemeasure_el_GR.ts \
                languages/iricDistancemeasure_es_ES.ts \
                languages/iricDistancemeasure_et_EE.ts \
                languages/iricDistancemeasure_eu_ES.ts \
                languages/iricDistancemeasure_fi_FI.ts \
                languages/iricDistancemeasure_fr_FR.ts \
                languages/iricDistancemeasure_gl_ES.ts \
                languages/iricDistancemeasure_hi_IN.ts \
                languages/iricDistancemeasure_hu_HU.ts \
                languages/iricDistancemeasure_id_ID.ts \
                languages/iricDistancemeasure_is_IS.ts \
                languages/iricDistancemeasure_it_IT.ts \
                languages/iricDistancemeasure_ja_JP.ts \
                languages/iricDistancemeasure_ko_KR.ts \
                languages/iricDistancemeasure_ky_KG.ts \
                languages/iricDistancemeasure_lt_LT.ts \
                languages/iricDistancemeasure_lv_LV.ts \
                languages/iricDistancemeasure_nb_NO.ts \
                languages/iricDistancemeasure_nl_NL.ts \
                languages/iricDistancemeasure_pl_PL.ts \
                languages/iricDistancemeasure_pt_BR.ts \
                languages/iricDistancemeasure_pt_PT.ts \
                languages/iricDistancemeasure_ro_RO.ts \
                languages/iricDistancemeasure_ru_RU.ts \
                languages/iricDistancemeasure_sl_SI.ts \
                languages/iricDistancemeasure_sv_SE.ts \
                languages/iricDistancemeasure_th_TH.ts \
                languages/iricDistancemeasure_tr_TR.ts \
                languages/iricDistancemeasure_uk_UA.ts \
                languages/iricDistancemeasure_vi_VN.ts \
                languages/iricDistancemeasure_zh_CN.ts \
                languages/iricDistancemeasure_zh_TW.ts

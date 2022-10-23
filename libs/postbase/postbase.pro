TARGET = iricPostbase
TEMPLATE = lib


DEFINES += POSTBASE_LIBRARY

include( ../../paths.pri )

QT += qml widgets xml

# Use Precompiled headers (PCH)
PRECOMPILED_HEADER = postbase_pch.h

######################
# Internal libraries #
######################

# iricMisc

unix {
	LIBS += -L"../misc"
}
LIBS += -liricMisc

# iricGuibase

unix {
	LIBS += -L"../guibase"
}
LIBS += -liricGuibase

# iricGuicore

unix {
	LIBS += -L"../guicore"
}
LIBS += -liricGuicore

######################
# External libraries #
######################

# VTK

LIBS += \
	-lvtkCommonCore-$${VTK_MAJ_MIN} \
	-lvtkCommonDataModel-$${VTK_MAJ_MIN} \
	-lvtkCommonExecutionModel-$${VTK_MAJ_MIN} \
	-lvtkRenderingCore-$${VTK_MAJ_MIN} \
	-lvtkRenderingFreeType-$${VTK_MAJ_MIN} \
	-lvtkRenderingQt-$${VTK_MAJ_MIN}

win32 {
	DESTDIR = $(SolutionDir)/libdlls/$(Configuration)
	LIBS += -L$(SolutionDir)/libdlls/$(Configuration)
}

# Input
HEADERS += autoparticlewindowi.h \
           cfshapeexportwindowi.h \
           particleexportwindowi.h \
           postallzoneselectingdialog.h \
           postbase_global.h \
           postsolutionselectdialog.h \
           svkmlexportwindowi.h \
           particle/particlearbitrarytimeeditdialog.h \
           particle/particledatasetting.h \
           particle/particledatavectorsetting.h \
           polydata/polydatasetting.h \
           string/poststringresultdataitem.h \
           string/poststringresultsettingdialog.h \
           time/posttimedataitem.h \
           time/posttimeeditdialog.h \
           title/posttitledataitem.h \
           title/posttitleeditdialog.h \
           title/posttitlesetting.h \
           string/private/poststringresultdataitem_movecommand.h \
           string/private/poststringresultdataitem_resizecommand.h \
           string/private/poststringresultdataitem_setsettingcommand.h \
           time/private/posttimedataitem_setsettingcommand.h \
           title/private/posttitledataitem_setsettingcommand.h
FORMS += postallzoneselectingdialog.ui \
         particle/particlearbitrarytimeeditdialog.ui \
         postsolutionselectdialog.ui \
         string/poststringresultsettingdialog.ui \
         time/posttimeeditdialog.ui \
         title/posttitleeditdialog.ui
SOURCES += postallzoneselectingdialog.cpp \
           particle/particlearbitrarytimeeditdialog.cpp \
           particle/particledatasetting.cpp \
           particle/particledatavectorsetting.cpp \
           polydata/polydatasetting.cpp \
           postsolutionselectdialog.cpp \
           string/poststringresultdataitem.cpp \
           string/poststringresultsettingdialog.cpp \
           time/posttimedataitem.cpp \
           time/posttimeeditdialog.cpp \
           title/posttitledataitem.cpp \
           title/posttitleeditdialog.cpp \
           title/posttitlesetting.cpp \
           string/private/poststringresultdataitem_movecommand.cpp \
           string/private/poststringresultdataitem_resizecommand.cpp \
           string/private/poststringresultdataitem_setsettingcommand.cpp \
           time/private/posttimedataitem_setsettingcommand.cpp \
           title/private/posttitledataitem_setsettingcommand.cpp
TRANSLATIONS += languages/iricPostbase_ar_EG.ts \
                languages/iricPostbase_bg_BG.ts \
                languages/iricPostbase_bs_BA.ts \
                languages/iricPostbase_ca_ES.ts \
                languages/iricPostbase_cs_CZ.ts \
                languages/iricPostbase_da_DK.ts \
                languages/iricPostbase_de_DE.ts \
                languages/iricPostbase_el_GR.ts \
                languages/iricPostbase_es_ES.ts \
                languages/iricPostbase_et_EE.ts \
                languages/iricPostbase_eu_ES.ts \
                languages/iricPostbase_fi_FI.ts \
                languages/iricPostbase_fr_FR.ts \
                languages/iricPostbase_gl_ES.ts \
                languages/iricPostbase_hi_IN.ts \
                languages/iricPostbase_hu_HU.ts \
                languages/iricPostbase_id_ID.ts \
                languages/iricPostbase_is_IS.ts \
                languages/iricPostbase_it_IT.ts \
                languages/iricPostbase_ja_JP.ts \
                languages/iricPostbase_ko_KR.ts \
                languages/iricPostbase_ky_KG.ts \
                languages/iricPostbase_lt_LT.ts \
                languages/iricPostbase_lv_LV.ts \
                languages/iricPostbase_nb_NO.ts \
                languages/iricPostbase_nl_NL.ts \
                languages/iricPostbase_pl_PL.ts \
                languages/iricPostbase_pt_BR.ts \
                languages/iricPostbase_pt_PT.ts \
                languages/iricPostbase_ro_RO.ts \
                languages/iricPostbase_ru_RU.ts \
                languages/iricPostbase_sl_SI.ts \
                languages/iricPostbase_sv_SE.ts \
                languages/iricPostbase_th_TH.ts \
                languages/iricPostbase_tr_TR.ts \
                languages/iricPostbase_uk_UA.ts \
                languages/iricPostbase_vi_VN.ts \
                languages/iricPostbase_zh_CN.ts \
                languages/iricPostbase_zh_TW.ts

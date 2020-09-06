TARGET = iricPostbase
TEMPLATE = lib


DEFINES += POSTBASE_LIBRARY

include( ../../paths.pri )

QT += qml widgets xml

######################
# Internal libraries #
######################

# iricMisc

win32 {
	CONFIG(debug, debug|release) {
		LIBS += -L"../misc/debug"
	} else {
		LIBS += -L"../misc/release"
	}
}
unix {
	LIBS += -L"../misc"
}
LIBS += -liricMisc

# iricGuibase

win32 {
	CONFIG(debug, debug|release) {
		LIBS += -L"../guibase/debug"
	} else {
		LIBS += -L"../guibase/release"
	}
}
unix {
	LIBS += -L"../guibase"
}
LIBS += -liricGuibase

# iricGuicore

win32 {
	CONFIG(debug, debug|release) {
		LIBS += -L"../guicore/debug"
	} else {
		LIBS += -L"../guicore/release"
	}
}
unix {
	LIBS += -L"../guicore"
}
LIBS += -liricGuicore

######################
# External libraries #
######################

# VTK

LIBS += \
	-lvtkCommonCore-6.1 \
	-lvtkCommonDataModel-6.1 \
	-lvtkCommonExecutionModel-6.1 \
	-lvtkRenderingCore-6.1 \
	-lvtkRenderingFreeType-6.1 \
	-lvtkRenderingQt-6.1

# Post-Build Event
win32 {
	QMAKE_POST_LINK += IF NOT EXIST $(SolutionDir)\\libdlls\\$(Configuration) mkdir $(SolutionDir)\\libdlls\\$(Configuration) && copy $(TargetPath) $(SolutionDir)\\libdlls\\$(Configuration)
}

# Input
HEADERS += cfshapeexportwindowi.h \
           particleexportwindowi.h \
           post2dwindowcontoursetting.h \
           postallzoneselectingdialog.h \
           postbase_global.h \
           postwindowgridtypedataiteminterface.h \
           svkmlexportwindowi.h \
           particle/particlearbitrarytimeeditdialog.h \
           particle/postparticlebasicsetting.h \
           particle/postparticlebasicsettingdialog.h \
           particle/postparticlescalarpropertydialog.h \
           particle/postparticlevectorpropertydialog.h \
           polydata/postpolydatabasicsetting.h \
           polydata/postpolydatabasicsettingdialog.h \
           polydata/postpolydatascalarpropertydialog.h \
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
         particle/postparticlebasicsettingdialog.ui \
         particle/postparticlescalarpropertydialog.ui \
         particle/postparticlevectorpropertydialog.ui \
         polydata/postpolydatabasicsettingdialog.ui \
         polydata/postpolydatascalarpropertydialog.ui \
         string/poststringresultsettingdialog.ui \
         time/posttimeeditdialog.ui \
         title/posttitleeditdialog.ui
SOURCES += post2dwindowcontoursetting.cpp \
           postallzoneselectingdialog.cpp \
           particle/particlearbitrarytimeeditdialog.cpp \
           particle/postparticlebasicsetting.cpp \
           particle/postparticlebasicsettingdialog.cpp \
           particle/postparticlescalarpropertydialog.cpp \
           particle/postparticlevectorpropertydialog.cpp \
           polydata/postpolydatabasicsetting.cpp \
           polydata/postpolydatabasicsettingdialog.cpp \
           polydata/postpolydatascalarpropertydialog.cpp \
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

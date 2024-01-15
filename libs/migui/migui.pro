TARGET = iricMiGui
TEMPLATE = lib
CONFIG += qt

DEFINES += MIGUI_LIBRARY

include( ../../paths.pri )

QT += xml widgets

UI_DIR = ./ui

# Use Precompiled headers (PCH)
PRECOMPILED_HEADER = migui_pch.h

######################
# Internal libraries #
######################

#iricMisc library

unix {
	LIBS += -L"../misc"
}
LIBS += -liricMisc

#iricGuibase library

unix {
	LIBS += -L"../guibase"
}
LIBS += -liricGuibase

#iricGuicore library

unix {
	LIBS += -L"../guicore"
}
LIBS += -liricGuicore

#iricGui library

unix {
        LIBS += -L"../gui"
}
LIBS += -liricGui

######################
# External libraries #
######################

# iriclib

win32 {
        CONFIG(debug, debug|release) {
                LIBS += -liriclibd
        } else {
                LIBS += -liriclib
        }
}

unix {
        LIBS += -liriclib
}

INCLUDEPATH += ../../libs

win32 {
	DESTDIR = $(SolutionDir)/libdlls/$(Configuration)
	LIBS += -L$(SolutionDir)/libdlls/$(Configuration)
}

# Input
HEADERS += migui_api.h \
           dialogs/connectionadddialog.h \
           dialogs/existingmodeladddialog.h \
           dialogs/modeladddialog.h \
           dialogs/modeldeletedialog.h \
           main/iricmimainwindow.h \
           misc/recentprojectsmanager.h \
           project/connection.h \
           project/connectioninput.h \
           project/connectionnode.h \
           project/connectionoutput.h \
           project/iricmiproject.h \
           project/model.h \
           main/private/iricmimainwindow_impl.h \
           main/private/iricmimainwindow_modelstabledelegate.h \
           project/private/model_projectloader.h
FORMS += dialogs/connectionadddialog.ui \
         dialogs/existingmodeladddialog.ui \
         dialogs/modeladddialog.ui \
         dialogs/modeldeletedialog.ui \
         main/iricmimainwindow.ui
SOURCES += dialogs/connectionadddialog.cpp \
           dialogs/existingmodeladddialog.cpp \
           dialogs/modeladddialog.cpp \
           dialogs/modeldeletedialog.cpp \
           main/iricmimainwindow.cpp \
           misc/recentprojectsmanager.cpp \
           project/connection.cpp \
           project/connectioninput.cpp \
           project/connectionnode.cpp \
           project/connectionoutput.cpp \
           project/iricmiproject.cpp \
           project/model.cpp \
           main/private/iricmimainwindow_impl.cpp \
           main/private/iricmimainwindow_modelstabledelegate.cpp \
           project/private/model_projectloader.cpp
RESOURCES += migui.qrc
TRANSLATIONS += languages/iricMiGui_ar_EG.ts \
                languages/iricMiGui_bg_BG.ts \
                languages/iricMiGui_bs_BA.ts \
                languages/iricMiGui_ca_ES.ts \
                languages/iricMiGui_cs_CZ.ts \
                languages/iricMiGui_da_DK.ts \
                languages/iricMiGui_de_DE.ts \
                languages/iricMiGui_el_GR.ts \
                languages/iricMiGui_es_ES.ts \
                languages/iricMiGui_et_EE.ts \
                languages/iricMiGui_eu_ES.ts \
                languages/iricMiGui_fi_FI.ts \
                languages/iricMiGui_fr_FR.ts \
                languages/iricMiGui_gl_ES.ts \
                languages/iricMiGui_hi_IN.ts \
                languages/iricMiGui_hu_HU.ts \
                languages/iricMiGui_id_ID.ts \
                languages/iricMiGui_is_IS.ts \
                languages/iricMiGui_it_IT.ts \
                languages/iricMiGui_ja_JP.ts \
                languages/iricMiGui_ko_KR.ts \
                languages/iricMiGui_ky_KG.ts \
                languages/iricMiGui_lt_LT.ts \
                languages/iricMiGui_lv_LV.ts \
                languages/iricMiGui_nb_NO.ts \
                languages/iricMiGui_nl_NL.ts \
                languages/iricMiGui_pl_PL.ts \
                languages/iricMiGui_pt_BR.ts \
                languages/iricMiGui_pt_PT.ts \
                languages/iricMiGui_ro_RO.ts \
                languages/iricMiGui_ru_RU.ts \
                languages/iricMiGui_sl_SI.ts \
                languages/iricMiGui_sv_SE.ts \
                languages/iricMiGui_th_TH.ts \
                languages/iricMiGui_tr_TR.ts \
                languages/iricMiGui_uk_UA.ts \
                languages/iricMiGui_vi_VN.ts \
                languages/iricMiGui_zh_CN.ts \
                languages/iricMiGui_zh_TW.ts

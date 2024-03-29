######################################################################
# Automatically generated by qmake (2.01a) ? 10 23 18:49:25 2014
######################################################################

TARGET = iricGraph2d
TEMPLATE = lib
INCLUDEPATH += ../..

DEFINES += GRAPH2D_LIBRARY

include( ../../../paths.pri )

QT += widgets xml

# Use Precompiled headers (PCH)
PRECOMPILED_HEADER = graph2d_pch.h

######################
# Internal libraries #
######################

# iricMisc

unix {
	LIBS += -L"../../misc"
}
LIBS += -liricMisc

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

# iricPostbase

unix {
	LIBS += -L"../../postbase"
}
LIBS += -liricPostbase

######################
# External libraries #
######################

# Qwt

win32 {
	CONFIG(debug, debug|release) {
		LIBS += -lqwtd
	}
	else {
		LIBS += -lqwt
		DEFINES += QT_NO_DEBUG_OUTPUT
		DEFINES += QT_NO_WARNING_OUTPUT
	}
}
unix {
	LIBS += -lqwt
	DEFINES += QT_NO_DEBUG_OUTPUT
	DEFINES += QT_NO_WARNING_OUTPUT
}

# VTK

LIBS += \
	-lvtkCommonCore-$${VTK_MAJ_MIN}

win32 {
	DESTDIR = $(SolutionDir)/libdlls/$(Configuration)
	LIBS += -L$(SolutionDir)/libdlls/$(Configuration)
}

# Input
HEADERS += graph2d_global.h \
           graph2dwindowdataitem.h \
           graph2dwindowdatamodel.h \
           graph2dwindowmarkersetting.h \
           graph2dwindowmarkersettingdialog.h \
           graph2dwindowrootdataitem.h \
           graph2dwindowverticalaxissetting.h \
           graph2dwindowverticalaxissettingdialog.h \
           graph2dwindowview.h \
           datamodel/graph2dwindowmarkerdataitem.h \
           datamodel/graph2dwindowmarkergroupdataitem.h \
           private/graph2dwindowdataitem_standarditemchangecommand.h
FORMS += graph2dwindowmarkersettingdialog.ui \
         graph2dwindowverticalaxissettingdialog.ui
SOURCES += graph2dwindowdataitem.cpp \
           graph2dwindowdatamodel.cpp \
           graph2dwindowmarkersetting.cpp \
           graph2dwindowmarkersettingdialog.cpp \
           graph2dwindowrootdataitem.cpp \
           graph2dwindowverticalaxissetting.cpp \
           graph2dwindowverticalaxissettingdialog.cpp \
           graph2dwindowview.cpp \
           datamodel/graph2dwindowmarkerdataitem.cpp \
           datamodel/graph2dwindowmarkergroupdataitem.cpp \
           private/graph2dwindowdataitem_standarditemchangecommand.cpp
TRANSLATIONS += languages/iricGraph2d_ar_EG.ts \
                languages/iricGraph2d_bg_BG.ts \
                languages/iricGraph2d_bs_BA.ts \
                languages/iricGraph2d_ca_ES.ts \
                languages/iricGraph2d_cs_CZ.ts \
                languages/iricGraph2d_da_DK.ts \
                languages/iricGraph2d_de_DE.ts \
                languages/iricGraph2d_el_GR.ts \
                languages/iricGraph2d_es_ES.ts \
                languages/iricGraph2d_et_EE.ts \
                languages/iricGraph2d_eu_ES.ts \
                languages/iricGraph2d_fi_FI.ts \
                languages/iricGraph2d_fr_FR.ts \
                languages/iricGraph2d_gl_ES.ts \
                languages/iricGraph2d_hi_IN.ts \
                languages/iricGraph2d_hu_HU.ts \
                languages/iricGraph2d_id_ID.ts \
                languages/iricGraph2d_is_IS.ts \
                languages/iricGraph2d_it_IT.ts \
                languages/iricGraph2d_ja_JP.ts \
                languages/iricGraph2d_ko_KR.ts \
                languages/iricGraph2d_ky_KG.ts \
                languages/iricGraph2d_lt_LT.ts \
                languages/iricGraph2d_lv_LV.ts \
                languages/iricGraph2d_nb_NO.ts \
                languages/iricGraph2d_nl_NL.ts \
                languages/iricGraph2d_pl_PL.ts \
                languages/iricGraph2d_pt_BR.ts \
                languages/iricGraph2d_pt_PT.ts \
                languages/iricGraph2d_ro_RO.ts \
                languages/iricGraph2d_ru_RU.ts \
                languages/iricGraph2d_sl_SI.ts \
                languages/iricGraph2d_sv_SE.ts \
                languages/iricGraph2d_th_TH.ts \
                languages/iricGraph2d_tr_TR.ts \
                languages/iricGraph2d_uk_UA.ts \
                languages/iricGraph2d_vi_VN.ts \
                languages/iricGraph2d_zh_CN.ts \
                languages/iricGraph2d_zh_TW.ts

TARGET = iricGraph2dscattered
TEMPLATE = lib
INCLUDEPATH += ../..

DEFINES += GRAPH2DSCATTERED_LIBRARY

include( ../../../paths.pri )

QT += widgets xml

# Use Precompiled headers (PCH)
PRECOMPILED_HEADER = graph2dscattered_pch.h

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

# iricGraph2d

unix {
	LIBS += -L"../graph2d"
}
LIBS += -liricGraph2d

######################
# External libraries #
######################

# Qwt

win32 {
	CONFIG(debug, debug|release) {
		LIBS += -lqwtd
	} else {
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
	-lvtkCommonCore-$${VTK_MAJ_MIN} \
	-lvtkCommonDataModel-$${VTK_MAJ_MIN} \
	-lvtkCommonExecutionModel-$${VTK_MAJ_MIN} \
	-lvtkFiltersExtraction-$${VTK_MAJ_MIN}

win32 {
	DESTDIR = $(SolutionDir)/libdlls/$(Configuration)
	LIBS += -L$(SolutionDir)/libdlls/$(Configuration)
}

# Input
HEADERS += graph2dscattered_global.h \
           graph2dscatteredaxissettingdialog.h \
           graph2dscatteredwindow.h \
           graph2dscatteredwindowactionmanager.h \
           graph2dscatteredwindowcontrolwidget.h \
           graph2dscatteredwindowdataitem.h \
           graph2dscatteredwindowdatamodel.h \
           graph2dscatteredwindowdatasourcedialog.h \
           graph2dscatteredwindowdrawsettingdialog.h \
           graph2dscatteredwindowobjectbrowser.h \
           graph2dscatteredwindowobjectbrowserview.h \
           graph2dscatteredwindowprojectdataitem.h \
           graph2dscatteredwindowresultsetting.h \
           graph2dscatteredwindowsettingwidget.h \
           graph2dscatteredwindowview.h \
           datamodel/graph2dscatteredwindowgridresultdataitem.h \
           datamodel/graph2dscatteredwindowresultdataitem.h \
           datamodel/graph2dscatteredwindowresultgroupdataitem.h \
           datamodel/graph2dscatteredwindowrootdataitem.h
FORMS += graph2dscatteredaxissettingdialog.ui \
         graph2dscatteredwindow.ui \
         graph2dscatteredwindowcontrolwidget.ui \
         graph2dscatteredwindowdatasourcedialog.ui \
         graph2dscatteredwindowdrawsettingdialog.ui \
         graph2dscatteredwindowsettingwidget.ui
SOURCES += graph2dscatteredaxissettingdialog.cpp \
           graph2dscatteredwindow.cpp \
           graph2dscatteredwindowactionmanager.cpp \
           graph2dscatteredwindowcontrolwidget.cpp \
           graph2dscatteredwindowdataitem.cpp \
           graph2dscatteredwindowdatamodel.cpp \
           graph2dscatteredwindowdatasourcedialog.cpp \
           graph2dscatteredwindowdrawsettingdialog.cpp \
           graph2dscatteredwindowobjectbrowser.cpp \
           graph2dscatteredwindowobjectbrowserview.cpp \
           graph2dscatteredwindowprojectdataitem.cpp \
           graph2dscatteredwindowresultsetting.cpp \
           graph2dscatteredwindowsettingwidget.cpp \
           graph2dscatteredwindowview.cpp \
           datamodel/graph2dscatteredwindowgridresultdataitem.cpp \
           datamodel/graph2dscatteredwindowresultdataitem.cpp \
           datamodel/graph2dscatteredwindowresultgroupdataitem.cpp \
           datamodel/graph2dscatteredwindowrootdataitem.cpp
TRANSLATIONS += languages/iricGraph2dscattered_ar_EG.ts \
                languages/iricGraph2dscattered_bg_BG.ts \
                languages/iricGraph2dscattered_bs_BA.ts \
                languages/iricGraph2dscattered_ca_ES.ts \
                languages/iricGraph2dscattered_cs_CZ.ts \
                languages/iricGraph2dscattered_da_DK.ts \
                languages/iricGraph2dscattered_de_DE.ts \
                languages/iricGraph2dscattered_el_GR.ts \
                languages/iricGraph2dscattered_es_ES.ts \
                languages/iricGraph2dscattered_et_EE.ts \
                languages/iricGraph2dscattered_eu_ES.ts \
                languages/iricGraph2dscattered_fi_FI.ts \
                languages/iricGraph2dscattered_fr_FR.ts \
                languages/iricGraph2dscattered_gl_ES.ts \
                languages/iricGraph2dscattered_hi_IN.ts \
                languages/iricGraph2dscattered_hu_HU.ts \
                languages/iricGraph2dscattered_id_ID.ts \
                languages/iricGraph2dscattered_is_IS.ts \
                languages/iricGraph2dscattered_it_IT.ts \
                languages/iricGraph2dscattered_ja_JP.ts \
                languages/iricGraph2dscattered_ko_KR.ts \
                languages/iricGraph2dscattered_ky_KG.ts \
                languages/iricGraph2dscattered_lt_LT.ts \
                languages/iricGraph2dscattered_lv_LV.ts \
                languages/iricGraph2dscattered_nb_NO.ts \
                languages/iricGraph2dscattered_nl_NL.ts \
                languages/iricGraph2dscattered_pl_PL.ts \
                languages/iricGraph2dscattered_pt_BR.ts \
                languages/iricGraph2dscattered_pt_PT.ts \
                languages/iricGraph2dscattered_ro_RO.ts \
                languages/iricGraph2dscattered_ru_RU.ts \
                languages/iricGraph2dscattered_sl_SI.ts \
                languages/iricGraph2dscattered_sv_SE.ts \
                languages/iricGraph2dscattered_th_TH.ts \
                languages/iricGraph2dscattered_tr_TR.ts \
                languages/iricGraph2dscattered_uk_UA.ts \
                languages/iricGraph2dscattered_vi_VN.ts \
                languages/iricGraph2dscattered_zh_CN.ts \
                languages/iricGraph2dscattered_zh_TW.ts

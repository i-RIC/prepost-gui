######################################################################
# Automatically generated by qmake (2.01a) ? 10 23 18:49:25 2014
######################################################################

TARGET = iricGraph2dhybrid
TEMPLATE = lib
INCLUDEPATH += ../..

DEFINES += GRAPH2DHYBRID_LIBRARY

include( ../../../paths.pri )

QT += widgets xml

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

# iricGdPolyLine

unix {
	LIBS += -L"../../geodata/polyline"
}
LIBS += -liricGdPolyLine

# iricGdPolyDataGroup

unix {
        LIBS += -L"../../geodata/polydatagroup"
}
LIBS += -liricGdPolyDataGroup

# iricGdPolyLineGroup

unix {
        LIBS += -L"../../geodata/polylinegroup"
}
LIBS += -liricGdPolyLineGroup


# iricGdRiversurvey

unix {
	LIBS += -L"../../geodata/riversurvey"
}
LIBS += -liricGdRiversurvey

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
	-lvtkCommonCore-$${VTK_MAJ_MIN} \
	-lvtkCommonDataModel-$${VTK_MAJ_MIN} \
	-lvtkCommonExecutionModel-$${VTK_MAJ_MIN} \
	-lvtkFiltersCore-$${VTK_MAJ_MIN} \
	-lvtkFiltersExtraction-$${VTK_MAJ_MIN}

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

win32 {
	DESTDIR = $(SolutionDir)/libdlls/$(Configuration)
	LIBS += -L$(SolutionDir)/libdlls/$(Configuration)
}

# Input
HEADERS += graph2dhybrid_global.h \
           graph2dhybridsettingdialog.h \
           graph2dhybridwindow.h \
           graph2dhybridwindowactionmanager.h \
           graph2dhybridwindowcontinuousexportdialog.h \
           graph2dhybridwindowcontrolwidget.h \
           graph2dhybridwindowdataitem.h \
           graph2dhybridwindowdatamodel.h \
           graph2dhybridwindowdatasourcedialog.h \
           graph2dhybridwindowlinesettingdialog.h \
           graph2dhybridwindowobjectbrowser.h \
           graph2dhybridwindowobjectbrowserview.h \
           graph2dhybridwindowprojectdataitem.h \
           graph2dhybridwindowresultsetting.h \
           graph2dhybridwindowview.h \
           datamodel/graph2dhybridwindowbaseiterativeresultdataitem.h \
           datamodel/graph2dhybridwindowdrawsettingdialog.h \
           datamodel/graph2dhybridwindowgridabstractpolylineresultdataitem.h \
           datamodel/graph2dhybridwindowgridijkresultdataitem.h \
           datamodel/graph2dhybridwindowgridpointresultdataitem.h \
           datamodel/graph2dhybridwindowgridpolylinegrouppolylineresultdataitem.h \
           datamodel/graph2dhybridwindowgridpolylineresultdataitem.h \
           datamodel/graph2dhybridwindowimportdatadataitem.h \
           datamodel/graph2dhybridwindowimportdatagroupdataitem.h \
           datamodel/graph2dhybridwindowresultcopydataitem.h \
           datamodel/graph2dhybridwindowresultcopygroupdataitem.h \
           datamodel/graph2dhybridwindowresultdataitem.h \
           datamodel/graph2dhybridwindowresultgroupdataitem.h \
           datamodel/graph2dhybridwindowrootdataitem.h \
           datamodel/graph2dhybridwindowsettingwidget.h
FORMS += graph2dhybridsettingdialog.ui \
         graph2dhybridwindow.ui \
         graph2dhybridwindowcontinuousexportdialog.ui \
         graph2dhybridwindowcontrolwidget.ui \
         graph2dhybridwindowdatasourcedialog.ui \
         graph2dhybridwindowlinesettingdialog.ui \
         datamodel/graph2dhybridwindowdrawsettingdialog.ui \
         datamodel/graph2dhybridwindowsettingwidget.ui
SOURCES += graph2dhybridsettingdialog.cpp \
           graph2dhybridwindow.cpp \
           graph2dhybridwindowactionmanager.cpp \
           graph2dhybridwindowcontinuousexportdialog.cpp \
           graph2dhybridwindowcontrolwidget.cpp \
           graph2dhybridwindowdataitem.cpp \
           graph2dhybridwindowdatamodel.cpp \
           graph2dhybridwindowdatasourcedialog.cpp \
           graph2dhybridwindowlinesettingdialog.cpp \
           graph2dhybridwindowobjectbrowser.cpp \
           graph2dhybridwindowobjectbrowserview.cpp \
           graph2dhybridwindowprojectdataitem.cpp \
           graph2dhybridwindowresultsetting.cpp \
           graph2dhybridwindowview.cpp \
           datamodel/graph2dhybridwindowbaseiterativeresultdataitem.cpp \
           datamodel/graph2dhybridwindowdrawsettingdialog.cpp \
           datamodel/graph2dhybridwindowgridabstractpolylineresultdataitem.cpp \
           datamodel/graph2dhybridwindowgridijkresultdataitem.cpp \
           datamodel/graph2dhybridwindowgridpointresultdataitem.cpp \
           datamodel/graph2dhybridwindowgridpolylinegrouppolylineresultdataitem.cpp \
           datamodel/graph2dhybridwindowgridpolylineresultdataitem.cpp \
           datamodel/graph2dhybridwindowimportdatadataitem.cpp \
           datamodel/graph2dhybridwindowimportdatagroupdataitem.cpp \
           datamodel/graph2dhybridwindowresultcopydataitem.cpp \
           datamodel/graph2dhybridwindowresultcopygroupdataitem.cpp \
           datamodel/graph2dhybridwindowresultdataitem.cpp \
           datamodel/graph2dhybridwindowresultgroupdataitem.cpp \
           datamodel/graph2dhybridwindowrootdataitem.cpp \
           datamodel/graph2dhybridwindowsettingwidget.cpp
TRANSLATIONS += languages/iricGraph2dhybrid_ar_EG.ts \
                languages/iricGraph2dhybrid_bg_BG.ts \
                languages/iricGraph2dhybrid_bs_BA.ts \
                languages/iricGraph2dhybrid_ca_ES.ts \
                languages/iricGraph2dhybrid_cs_CZ.ts \
                languages/iricGraph2dhybrid_da_DK.ts \
                languages/iricGraph2dhybrid_de_DE.ts \
                languages/iricGraph2dhybrid_el_GR.ts \
                languages/iricGraph2dhybrid_es_ES.ts \
                languages/iricGraph2dhybrid_et_EE.ts \
                languages/iricGraph2dhybrid_eu_ES.ts \
                languages/iricGraph2dhybrid_fi_FI.ts \
                languages/iricGraph2dhybrid_fr_FR.ts \
                languages/iricGraph2dhybrid_gl_ES.ts \
                languages/iricGraph2dhybrid_hi_IN.ts \
                languages/iricGraph2dhybrid_hu_HU.ts \
                languages/iricGraph2dhybrid_id_ID.ts \
                languages/iricGraph2dhybrid_is_IS.ts \
                languages/iricGraph2dhybrid_it_IT.ts \
                languages/iricGraph2dhybrid_ja_JP.ts \
                languages/iricGraph2dhybrid_ko_KR.ts \
                languages/iricGraph2dhybrid_ky_KG.ts \
                languages/iricGraph2dhybrid_lt_LT.ts \
                languages/iricGraph2dhybrid_lv_LV.ts \
                languages/iricGraph2dhybrid_nb_NO.ts \
                languages/iricGraph2dhybrid_nl_NL.ts \
                languages/iricGraph2dhybrid_pl_PL.ts \
                languages/iricGraph2dhybrid_pt_BR.ts \
                languages/iricGraph2dhybrid_pt_PT.ts \
                languages/iricGraph2dhybrid_ro_RO.ts \
                languages/iricGraph2dhybrid_ru_RU.ts \
                languages/iricGraph2dhybrid_sl_SI.ts \
                languages/iricGraph2dhybrid_sv_SE.ts \
                languages/iricGraph2dhybrid_th_TH.ts \
                languages/iricGraph2dhybrid_tr_TR.ts \
                languages/iricGraph2dhybrid_uk_UA.ts \
                languages/iricGraph2dhybrid_vi_VN.ts \
                languages/iricGraph2dhybrid_zh_CN.ts \
                languages/iricGraph2dhybrid_zh_TW.ts

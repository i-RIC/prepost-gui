TARGET = iricCrosssection
TEMPLATE = lib
INCLUDEPATH += ..

DEFINES += CROSSSECTION_LIBRARY

QT += widgets xml

include( ../../paths.pri )

# Use Precompiled headers (PCH)
PRECOMPILED_HEADER = crosssection_pch.h

######################
# Internal libraries #
######################

# iricGuibase

unix {
        LIBS += -L"../guibase"
}
LIBS += -liricGuibase

# iricGuiCore

unix {
        LIBS += -L"../guicore"
}
LIBS += -liricGuicore

# iricMisc

unix {
        LIBS += -L"../misc"
}
LIBS += -liricMisc

######################
# External libraries #
######################

# geos

LIBS += -lgeos

win32 {
	DESTDIR = $(SolutionDir)/libdlls/$(Configuration)
	LIBS += -L$(SolutionDir)/libdlls/$(Configuration)
}

# VTK

LIBS += \
        -lvtkCommonCore-$${VTK_MAJ_MIN} \
        -lvtkCommonDataModel-$${VTK_MAJ_MIN} \
        -lvtkCommonExecutionModel-$${VTK_MAJ_MIN} \
        -lvtkFiltersExtraction-$${VTK_MAJ_MIN}

# Input
HEADERS += abstractcrosssectionwindow.h \
           crosssection_global.h \
           private/abstractcrosssectionwindow_csvexportcontroller.h \
           private/abstractcrosssectionwindow_displaysettingcontainer.h \
           private/abstractcrosssectionwindow_displaysettingdialog.h \
           private/abstractcrosssectionwindow_displaysettingtablecontroller.h \
           private/abstractcrosssectionwindow_displaysettingtablecontroller_delegate.h \
           private/abstractcrosssectionwindow_edittablecontroller.h \
           private/abstractcrosssectionwindow_edittablecontroller_delegate.h \
           private/abstractcrosssectionwindow_exportsettingdialog.h \
           private/abstractcrosssectionwindow_graphicsview_colormapcontroller.h \
           private/abstractcrosssectionwindow_graphicsview_drawregioninformation.h \
           private/abstractcrosssectionwindow_graphicsview_elevationchartcontroller.h \
           private/abstractcrosssectionwindow_graphicsview_independentchartcontroller.h \
           private/abstractcrosssectionwindow_gridattributedisplaysettingcontainer.h \
           private/abstractcrosssectionwindow_gridattributedisplaysettingeditdialog.h \
           private/abstractcrosssectionwindow_impl.h \
           private/abstractcrosssectionwindow_snapshotsavecontroller.h \
           private/abstractcrosssectionwindow_updatecommand.h \
           private/abstractcrosssectionwindow_updategraphicsviewcommand.h \
           public/abstractcrosssectionwindow_controller.h \
           public/abstractcrosssectionwindow_graphicsview.h
FORMS += abstractcrosssectionwindow.ui \
         private/abstractcrosssectionwindow_displaysettingdialog.ui \
         private/abstractcrosssectionwindow_exportsettingdialog.ui \
         private/abstractcrosssectionwindow_gridattributedisplaysettingeditdialog.ui \
         public/abstractcrosssectionwindow_controller.ui
SOURCES += abstractcrosssectionwindow.cpp \
           private/abstractcrosssectionwindow_csvexportcontroller.cpp \
           private/abstractcrosssectionwindow_displaysettingcontainer.cpp \
           private/abstractcrosssectionwindow_displaysettingdialog.cpp \
           private/abstractcrosssectionwindow_displaysettingtablecontroller.cpp \
           private/abstractcrosssectionwindow_displaysettingtablecontroller_delegate.cpp \
           private/abstractcrosssectionwindow_edittablecontroller.cpp \
           private/abstractcrosssectionwindow_edittablecontroller_delegate.cpp \
           private/abstractcrosssectionwindow_exportsettingdialog.cpp \
           private/abstractcrosssectionwindow_graphicsview_colormapcontroller.cpp \
           private/abstractcrosssectionwindow_graphicsview_drawregioninformation.cpp \
           private/abstractcrosssectionwindow_graphicsview_elevationchartcontroller.cpp \
           private/abstractcrosssectionwindow_graphicsview_independentchartcontroller.cpp \
           private/abstractcrosssectionwindow_gridattributedisplaysettingcontainer.cpp \
           private/abstractcrosssectionwindow_gridattributedisplaysettingeditdialog.cpp \
           private/abstractcrosssectionwindow_impl.cpp \
           private/abstractcrosssectionwindow_snapshotsavecontroller.cpp \
           private/abstractcrosssectionwindow_updatecommand.cpp \
           private/abstractcrosssectionwindow_updategraphicsviewcommand.cpp \
           public/abstractcrosssectionwindow_controller.cpp \
           public/abstractcrosssectionwindow_graphicsview.cpp
RESOURCES += crosssection.qrc
TRANSLATIONS += languages/iricCrosssection_ar_EG.ts \
                languages/iricCrosssection_bg_BG.ts \
                languages/iricCrosssection_bs_BA.ts \
                languages/iricCrosssection_ca_ES.ts \
                languages/iricCrosssection_cs_CZ.ts \
                languages/iricCrosssection_da_DK.ts \
                languages/iricCrosssection_de_DE.ts \
                languages/iricCrosssection_el_GR.ts \
                languages/iricCrosssection_es_ES.ts \
                languages/iricCrosssection_et_EE.ts \
                languages/iricCrosssection_eu_ES.ts \
                languages/iricCrosssection_fi_FI.ts \
                languages/iricCrosssection_fr_FR.ts \
                languages/iricCrosssection_gl_ES.ts \
                languages/iricCrosssection_hi_IN.ts \
                languages/iricCrosssection_hu_HU.ts \
                languages/iricCrosssection_id_ID.ts \
                languages/iricCrosssection_is_IS.ts \
                languages/iricCrosssection_it_IT.ts \
                languages/iricCrosssection_ja_JP.ts \
                languages/iricCrosssection_ko_KR.ts \
                languages/iricCrosssection_ky_KG.ts \
                languages/iricCrosssection_lt_LT.ts \
                languages/iricCrosssection_lv_LV.ts \
                languages/iricCrosssection_nb_NO.ts \
                languages/iricCrosssection_nl_NL.ts \
                languages/iricCrosssection_pl_PL.ts \
                languages/iricCrosssection_pt_BR.ts \
                languages/iricCrosssection_pt_PT.ts \
                languages/iricCrosssection_ro_RO.ts \
                languages/iricCrosssection_ru_RU.ts \
                languages/iricCrosssection_sl_SI.ts \
                languages/iricCrosssection_sv_SE.ts \
                languages/iricCrosssection_th_TH.ts \
                languages/iricCrosssection_tr_TR.ts \
                languages/iricCrosssection_uk_UA.ts \
                languages/iricCrosssection_vi_VN.ts \
                languages/iricCrosssection_zh_CN.ts \
                languages/iricCrosssection_zh_TW.ts

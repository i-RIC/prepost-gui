TEMPLATE = app
TARGET = Rivmaker
CONFIG += qt
CONFIG += debug_and_release

DEFINES += ANSI_DECLARATORS
DEFINES += QWT_DLL

include( ../../paths.pri )

QT += widgets network svg xml
RC_FILE = rivmaker.rc

# Use Precompiled headers (PCH)
PRECOMPILED_HEADER = rivmaker_pch.h

# iricGuiBase

unix {
        LIBS += -L"../../libs/guibase"
}
LIBS += -liricGuibase

# iricMisc

unix {
	LIBS += -L"../../libs/misc"
}
LIBS += -liricMisc

# iricTriangle

unix {
	LIBS += -L"../../libs/triangle"
}
LIBS += -liricTriangle

# Qwt

CONFIG(debug, debug|release) {
	win32 {
		LIBS += -lqwtd
	}
	unix {
		LIBS += -lqwt
	}
}
else {
	LIBS += -lqwt
	DEFINES += QT_NO_DEBUG_OUTPUT
	DEFINES += QT_NO_WARNING_OUTPUT
}

# geos

LIBS += -lgeos

win32 {
	LIBS += -L$(SolutionDir)/libdlls/$(Configuration)
}

# Input
HEADERS += csbuilder/crosssectionbuilderi.h \
           csbuilder/crosssectionbuildermapalltonearest.h \
           csbuilder/crosssectionbuildertemplatemapping.h \
           csbuilder/crosssectionbuildertin.h \
           dialogs/aboutdialog.h \
           dialogs/coordinateseditdialog.h \
           dialogs/graphicssettingdialog.h \
           dialogs/mappingsettingdialog.h \
           dialogs/mousehelpdialog.h \
           geom/geometry.h \
           geom/geometrypoint.h \
           geom/geometrytriangle.h \
           geom/quadtreecontainer.h \
           geom/rect.h \
           geom/rect_detail.h \
           io/rivcsvexporter.h \
           io/rivexporter.h \
           io/sacguiimporter.h \
           main/rivmakermainwindow.h \
           misc/geometryutil.h \
           misc/mathutil.h \
           misc/qwtcanvaswithpositionsignal.h \
           widgets/chartgraphicsview.h \
           widgets/chartgraphicsviewdisplaysetting.h \
           widgets/chartwindow.h \
           widgets/mousepositionwidget.h \
           widgets/valuewidget.h \
           window/viewwindowi.h \
           csbuilder/private/crosssectionbuildertemplatemapping_impl.h \
           data/arbitraryhwm/arbitraryhwm.h \
           data/arbitraryhwm/arbitraryhwmpreprocessorview.h \
           data/arbitraryhwm/arbitraryhwmpreprocessorviewhelper.h \
           data/base/coordinatesi.h \
           data/base/dataitem.h \
           data/base/dataitemcontroller.h \
           data/base/dataitemview.h \
           data/base/dataitemviewhelperi.h \
           data/base/model.h \
           data/base/model_detail.h \
           data/base/rootdataitem.h \
           data/base/topview.h \
           data/base/view.h \
           data/baseline/baseline.h \
           data/baseline/baselinepreprocessorcontroller.h \
           data/baseline/baselinepreprocessorview.h \
           data/baseline/baselinepreprocessorviewhelper.h \
           data/baseline/baselinepreprocessorviewlabelhelper.h \
           data/benchmark/benchmark.h \
           data/benchmark/benchmarkpreprocessorview.h \
           data/benchmark/benchmarkpreprocessorviewhelper.h \
           data/crosssection/crosssection.h \
           data/crosssection/crosssectionpreprocessorcontroller.h \
           data/crosssection/crosssectionpreprocessorview.h \
           data/crosssection/crosssectionpreprocessorviewhelper.h \
           data/crosssection/crosssectionpreprocessorviewlabelhelper.h \
           data/crosssections/crosssections.h \
           data/crosssections/crosssectionspreprocessorcontroller.h \
           data/elevationpoints/elevationpoints.h \
           data/elevationpoints/elevationpointspreprocessorview.h \
           data/elevationpoints/elevationpointspreprocessorviewhelper.h \
           data/hub/hub.h \
           data/hub/hubpreprocessorview.h \
           data/hub/hubpreprocessorviewhelper.h \
           data/leftbankhwm/leftbankhwm.h \
           data/leftbankhwm/leftbankhwmpreprocessorview.h \
           data/leftbankhwm/leftbankhwmpreprocessorviewhelper.h \
           data/points/points.h \
           data/points/pointsgraphicssetting.h \
           data/points/pointspreprocessorcontroller.h \
           data/points/pointspreprocessorview.h \
           data/points/pointspreprocessorviewhelper.h \
           data/points/pointspreprocessorviewnameshelper.h \
           data/project/project.h \
           data/referencemark/referencemark.h \
           data/referencemark/referencemarkpreprocessorview.h \
           data/referencemark/referencemarkpreprocessorviewhelper.h \
           data/rightbankhwm/rightbankhwm.h \
           data/rightbankhwm/rightbankhwmpreprocessorview.h \
           data/rightbankhwm/rightbankhwmpreprocessorviewhelper.h \
           data/streamgage/streamgage.h \
           data/streamgage/streamgagepreprocessorview.h \
           data/streamgage/streamgagepreprocessorviewhelper.h \
           data/watersurfaceelevationpoints/watersurfaceelevationpoints.h \
           data/watersurfaceelevationpoints/watersurfaceelevationpointspreprocessorview.h \
           dialogs/private/coordinateeditwidget.h \
           dialogs/private/coordinateseditdialog_impl.h \
           geom/private/quadtreecontainer_impl.h \
           io/crosssections/crosssectionscsvexporter.h \
           io/crosssections/crosssectionscsvimporter.h \
           io/crosssections/crosssectionsexporter.h \
           io/crosssections/crosssectionsexporteri.h \
           io/crosssections/crosssectionsimporter.h \
           io/crosssections/crosssectionsimporteri.h \
           io/points/pointscsvimporter.h \
           io/points/pointsimporter.h \
           io/points/pointsimporteri.h \
           io/polyline/polylinecsvexporter.h \
           io/polyline/polylinecsvimporter.h \
           io/polyline/polylineexporter.h \
           io/polyline/polylineexporteri.h \
           io/polyline/polylineimporter.h \
           io/polyline/polylineimporteri.h \
           main/private/rivmakermainwindow_impl.h \
           window/crosssection/crosssectionwindow.h \
           window/crosssection/crosssectionwindowdisplaysetting.h \
           window/crosssection/crosssectionwindowdisplaysettingdialog.h \
           window/crosssection/crosssectionwindowgraphicsview.h \
           window/preprocessor/preprocessordataitemi.h \
           window/preprocessor/preprocessormodel.h \
           window/preprocessor/preprocessorview.h \
           window/preprocessor/preprocessorwindow.h \
           window/verticalcrosssection/verticalcrosssectionwindow.h \
           window/verticalcrosssection/verticalcrosssectionwindowdisplaysettingdialog.h \
           window/verticalcrosssection/verticalcrosssectionwindowdisplaysettingdockwidget.h \
           window/verticalcrosssection/verticalcrosssectionwindowdisplaysettingdockwidgetview.h \
           window/verticalcrosssection/verticalcrosssectionwindowelevationsdockwidget.h \
           window/verticalcrosssection/verticalcrosssectionwindowelevationsdockwidgetview.h \
           window/verticalcrosssection/verticalcrosssectionwindowgraphicsview.h \
           data/base/private/dataitemcontroller_impl.h \
           data/base/private/dataitemview_impl.h \
           data/base/private/model_impl.h \
           data/baseline/private/baseline_impl.h \
           data/baseline/private/baselinepreprocessorcontroller_impl.h \
           data/crosssection/private/crosssection_impl.h \
           data/crosssection/private/crosssectionpreprocessorcontroller_impl.h \
           data/crosssections/private/crosssections_impl.h \
           data/crosssections/private/crosssectionspreprocessorcontroller_impl.h \
           data/elevationpoints/private/elevationpoints_impl.h \
           data/points/private/points_impl.h \
           data/points/private/pointspreprocessorcontroller_impl.h \
           data/project/private/project_impl.h \
           data/watersurfaceelevationpoints/private/watersurfaceelevationpoints_impl.h \
           window/preprocessor/objectbrowser/objectbrowser.h \
           window/preprocessor/objectbrowser/objectbrowserview.h \
           window/preprocessor/private/preprocessormodel_impl.h \
           window/preprocessor/private/preprocessorwindow_impl.h \
           window/verticalcrosssection/private/verticalcrosssectionwindowelevationsdockwidgetview_itemdelegete.h
FORMS += dialogs/aboutdialog.ui \
         dialogs/coordinateseditdialog.ui \
         dialogs/graphicssettingdialog.ui \
         dialogs/mappingsettingdialog.ui \
         dialogs/mousehelpdialog.ui \
         main/rivmakermainwindow.ui \
         widgets/mousepositionwidget.ui \
         widgets/valuewidget.ui \
         window/crosssection/crosssectionwindowdisplaysettingdialog.ui \
         window/verticalcrosssection/verticalcrosssectionwindow.ui \
         window/verticalcrosssection/verticalcrosssectionwindowdisplaysettingdialog.ui \
         window/verticalcrosssection/verticalcrosssectionwindowdisplaysettingdockwidgetview.ui \
         window/verticalcrosssection/verticalcrosssectionwindowelevationsdockwidgetview.ui
SOURCES += csbuilder/crosssectionbuildermapalltonearest.cpp \
           csbuilder/crosssectionbuildertemplatemapping.cpp \
           csbuilder/crosssectionbuildertin.cpp \
           dialogs/aboutdialog.cpp \
           dialogs/coordinateseditdialog.cpp \
           dialogs/graphicssettingdialog.cpp \
           dialogs/mappingsettingdialog.cpp \
           dialogs/mousehelpdialog.cpp \
           geom/geometrypoint.cpp \
           geom/geometrytriangle.cpp \
           geom/quadtreecontainer.cpp \
           geom/rect.cpp \
           io/rivcsvexporter.cpp \
           io/rivexporter.cpp \
           io/sacguiimporter.cpp \
           main/main.cpp \
           main/rivmakermainwindow.cpp \
           main/rivmakermainwindow_setupconnections.cpp \
           misc/geometryutil.cpp \
           misc/mathutil.cpp \
           misc/qwtcanvaswithpositionsignal.cpp \
           widgets/chartgraphicsview.cpp \
           widgets/chartgraphicsviewdisplaysetting.cpp \
           widgets/chartwindow.cpp \
           widgets/mousepositionwidget.cpp \
           widgets/valuewidget.cpp \
           data/arbitraryhwm/arbitraryhwm.cpp \
           data/arbitraryhwm/arbitraryhwmpreprocessorview.cpp \
           data/arbitraryhwm/arbitraryhwmpreprocessorviewhelper.cpp \
           data/base/dataitem.cpp \
           data/base/dataitemcontroller.cpp \
           data/base/dataitemview.cpp \
           data/base/dataitemviewhelperi.cpp \
           data/base/model.cpp \
           data/base/rootdataitem.cpp \
           data/base/topview.cpp \
           data/base/view.cpp \
           data/baseline/baseline.cpp \
           data/baseline/baselinepreprocessorcontroller.cpp \
           data/baseline/baselinepreprocessorview.cpp \
           data/baseline/baselinepreprocessorviewhelper.cpp \
           data/baseline/baselinepreprocessorviewlabelhelper.cpp \
           data/benchmark/benchmark.cpp \
           data/benchmark/benchmarkpreprocessorview.cpp \
           data/benchmark/benchmarkpreprocessorviewhelper.cpp \
           data/crosssection/crosssection.cpp \
           data/crosssection/crosssectionpreprocessorcontroller.cpp \
           data/crosssection/crosssectionpreprocessorview.cpp \
           data/crosssection/crosssectionpreprocessorviewhelper.cpp \
           data/crosssection/crosssectionpreprocessorviewlabelhelper.cpp \
           data/crosssections/crosssections.cpp \
           data/crosssections/crosssectionspreprocessorcontroller.cpp \
           data/elevationpoints/elevationpoints.cpp \
           data/elevationpoints/elevationpointspreprocessorview.cpp \
           data/elevationpoints/elevationpointspreprocessorviewhelper.cpp \
           data/hub/hub.cpp \
           data/hub/hubpreprocessorview.cpp \
           data/hub/hubpreprocessorviewhelper.cpp \
           data/leftbankhwm/leftbankhwm.cpp \
           data/leftbankhwm/leftbankhwmpreprocessorview.cpp \
           data/leftbankhwm/leftbankhwmpreprocessorviewhelper.cpp \
           data/points/points.cpp \
           data/points/pointsgraphicssetting.cpp \
           data/points/pointspreprocessorcontroller.cpp \
           data/points/pointspreprocessorview.cpp \
           data/points/pointspreprocessorviewhelper.cpp \
           data/points/pointspreprocessorviewnameshelper.cpp \
           data/project/project.cpp \
           data/referencemark/referencemark.cpp \
           data/referencemark/referencemarkpreprocessorview.cpp \
           data/referencemark/referencemarkpreprocessorviewhelper.cpp \
           data/rightbankhwm/rightbankhwm.cpp \
           data/rightbankhwm/rightbankhwmpreprocessorview.cpp \
           data/rightbankhwm/rightbankhwmpreprocessorviewhelper.cpp \
           data/streamgage/streamgage.cpp \
           data/streamgage/streamgagepreprocessorview.cpp \
           data/streamgage/streamgagepreprocessorviewhelper.cpp \
           data/watersurfaceelevationpoints/watersurfaceelevationpoints.cpp \
           dialogs/private/coordinateeditwidget.cpp \
           io/crosssections/crosssectionscsvexporter.cpp \
           io/crosssections/crosssectionscsvimporter.cpp \
           io/crosssections/crosssectionsexporter.cpp \
           io/crosssections/crosssectionsimporter.cpp \
           io/points/pointscsvimporter.cpp \
           io/points/pointsimporter.cpp \
           io/polyline/polylinecsvexporter.cpp \
           io/polyline/polylinecsvimporter.cpp \
           io/polyline/polylineexporter.cpp \
           io/polyline/polylineimporter.cpp \
           window/crosssection/crosssectionwindow.cpp \
           window/crosssection/crosssectionwindowdisplaysetting.cpp \
           window/crosssection/crosssectionwindowdisplaysettingdialog.cpp \
           window/crosssection/crosssectionwindowgraphicsview.cpp \
           window/preprocessor/preprocessormodel.cpp \
           window/preprocessor/preprocessorview.cpp \
           window/preprocessor/preprocessorwindow.cpp \
           window/verticalcrosssection/verticalcrosssectionwindow.cpp \
           window/verticalcrosssection/verticalcrosssectionwindowdisplaysettingdialog.cpp \
           window/verticalcrosssection/verticalcrosssectionwindowdisplaysettingdockwidget.cpp \
           window/verticalcrosssection/verticalcrosssectionwindowdisplaysettingdockwidgetview.cpp \
           window/verticalcrosssection/verticalcrosssectionwindowelevationsdockwidget.cpp \
           window/verticalcrosssection/verticalcrosssectionwindowelevationsdockwidgetview.cpp \
           window/verticalcrosssection/verticalcrosssectionwindowgraphicsview.cpp \
           window/preprocessor/objectbrowser/objectbrowser.cpp \
           window/preprocessor/objectbrowser/objectbrowserview.cpp \
           window/verticalcrosssection/private/verticalcrosssectionwindowelevationsdockwidgetview_itemdelegete.cpp
RESOURCES += rivmaker.qrc
TRANSLATIONS += languages/rivmaker_ar_EG.ts \
                languages/rivmaker_bg_BG.ts \
                languages/rivmaker_bs_BA.ts \
                languages/rivmaker_ca_ES.ts \
                languages/rivmaker_cs_CZ.ts \
                languages/rivmaker_da_DK.ts \
                languages/rivmaker_de_DE.ts \
                languages/rivmaker_el_GR.ts \
                languages/rivmaker_es_ES.ts \
                languages/rivmaker_et_EE.ts \
                languages/rivmaker_eu_ES.ts \
                languages/rivmaker_fi_FI.ts \
                languages/rivmaker_fr_FR.ts \
                languages/rivmaker_gl_ES.ts \
                languages/rivmaker_hi_IN.ts \
                languages/rivmaker_hu_HU.ts \
                languages/rivmaker_id_ID.ts \
                languages/rivmaker_is_IS.ts \
                languages/rivmaker_it_IT.ts \
                languages/rivmaker_ja_JP.ts \
                languages/rivmaker_ko_KR.ts \
                languages/rivmaker_ky_KG.ts \
                languages/rivmaker_lt_LT.ts \
                languages/rivmaker_lv_LV.ts \
                languages/rivmaker_nb_NO.ts \
                languages/rivmaker_nl_NL.ts \
                languages/rivmaker_pl_PL.ts \
                languages/rivmaker_pt_BR.ts \
                languages/rivmaker_pt_PT.ts \
                languages/rivmaker_ro_RO.ts \
                languages/rivmaker_ru_RU.ts \
                languages/rivmaker_sl_SI.ts \
                languages/rivmaker_sv_SE.ts \
                languages/rivmaker_th_TH.ts \
                languages/rivmaker_tr_TR.ts \
                languages/rivmaker_uk_UA.ts \
                languages/rivmaker_vi_VN.ts \
                languages/rivmaker_zh_CN.ts \
                languages/rivmaker_zh_TW.ts

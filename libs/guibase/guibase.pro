TARGET = iricGuibase
TEMPLATE = lib
INCLUDEPATH += ..

DEFINES += GUIBASE_LIBRARY

QT += widgets xml

include( ../../paths.pri )

######################
# Internal libraries #
######################

# iricCs

unix {
	LIBS += -L"../cs"
}
LIBS += -liricCs

# iricMisc

unix {
	LIBS += -L"../misc"
}
LIBS += -liricMisc

######################
# External libraries #
######################

# Qwt

win32 {
	CONFIG(debug, debug|release) {
		LIBS += -lqwtd
	}else {
		LIBS += -lqwt
		DEFINES += QT_NO_DEBUG_OUTPUT
		DEFINES += QT_NO_WARNING_OUTPUT
	}
}
unix {
	LIBS += -lqwt
}

# VTK

LIBS += \
	-lvtkCommonComputationalGeometry-6.1 \
	-lvtkCommonCore-6.1 \
	-lvtkCommonDataModel-6.1 \
	-lvtkCommonExecutionModel-6.1 \
	-lvtkCommonMath-6.1 \
	-lvtkFiltersExtraction-6.1 \
	-lvtkFiltersFlowPaths-6.1 \
	-lvtkInteractionStyle-6.1 \
	-lvtkInteractionWidgets-6.1 \
	-lvtkRenderingAnnotation-6.1 \
	-lvtkRenderingCore-6.1 \
	-lvtkRenderingFreeType-6.1 \
	-lvtkRenderingLabel-6.1

win32 {
	DESTDIR = $(SolutionDir)/libdlls/$(Configuration)
	LIBS += -L$(SolutionDir)/libdlls/$(Configuration)
}

# Input
HEADERS += arrowsettingcontainer.h \
           colortool.h \
           comboboxtool.h \
           cursorchanger.h \
           graphicsmisc.h \
           guibase_global.h \
           iricactivecellfilter.h \
           irictoolbar.h \
           landxmlutil.h \
           linestyleinformation.h \
           objectbrowser.h \
           objectbrowserview.h \
           overridecursorchanger.h \
           pointstyleinformation.h \
           qpainterpenbrushchanger.h \
           qpainterpenbrushrestorer.h \
           qwtplotcustomcurve.h \
           qwtplotcustommarker.h \
           scalarbardialog.h \
           scalarbarsetting.h \
           scalarbarwidget.h \
           scalarsettingcontainer.h \
           structuredgridregion.h \
           structuredgridregionselectwidget.h \
           tooltiplabel.h \
           vtk2dinteractorstyle.h \
           vtkCustomScalarBarActor.h \
           vtkCustomStreamPoints.h \
           vtkCustomStreamTracer.h \
           vtkdatasetattributestool.h \
           vtklegendboxrepresentation.h \
           vtklegendboxwidget.h \
           vtklinestylecontainer.h \
           vtklinestylewidget.h \
           vtkScalarBarActorInternal.h \
           vtksubdividegrid.h \
           vtktextpropertysettingcontainer.h \
           vtktextpropertysettingdialog.h \
           vtktextpropertysettingwidget.h \
           xyaxisdisplaysettingdialog.h \
           colormap/colormapcustomsetting.h \
           colormap/colormapcustomsettingcolor.h \
           colormap/colormapcustomsettingdialog.h \
           colormap/colormapsettingwidget.h \
           coordinates/coordinatesedit.h \
           coordinates/coordinateseditdialog.h \
           executer/executeri.h \
           executer/executerwatcher.h \
           executer/processexecuter.h \
           executer/silentexecuterwatcher.h \
           executer/threadexecuter.h \
           executer/unziparchiveprocessexecuter.h \
           executer/waitdialogexecuterwatcher.h \
           executer/ziparchiveprocessexecuter.h \
           geometry/pointring.h \
           geometry/polygonregion.h \
           geometry/rect.h \
           point/pointclearcommand.h \
           point/pointcontroller.h \
           point/pointmovecommand.h \
           point/pointsetcommand.h \
           polygon/polygonaddvertexcommand.h \
           polygon/polygoncontroller.h \
           polygon/polygoneditcoordinatescommand.h \
           polygon/polygonmovevertexcommand.h \
           polygon/polygonpushvertexcommand.h \
           polygon/polygonremovevertexcommand.h \
           polyline/polylineaddvertexcommand.h \
           polyline/polylinecontroller.h \
           polyline/polylinecontrollerutil.h \
           polyline/polylineeditcoordinatescommand.h \
           polyline/polylinemovevertexcommand.h \
           polyline/polylinepushvertexcommand.h \
           polyline/polylineremovevertexcommand.h \
           polyline/polylineutil.h \
           timeformat/timeformat.h \
           timeformat/timeformateditwidget.h \
           timeformat/timeformatutil.h \
           vtktool/vtkactorpolydatamapperpair.h \
           vtktool/vtklabel2dactor.h \
           vtktool/vtklineactor.h \
           vtktool/vtklinesactor.h \
           vtktool/vtkparametricsplineutil.h \
           vtktool/vtkpointsutil.h \
           vtktool/vtkpolydatalinesactor.h \
           vtktool/vtkpolydatapaintactor.h \
           vtktool/vtkpolydatapointsactor.h \
           vtktool/vtkpolygonactor.h \
           vtktool/vtkpolygonsactor.h \
           vtktool/vtkstreamtracerutil.h \
           widget/asciionlylineedit.h \
           widget/asciionlytextedit.h \
           widget/centeredcheckbox.h \
           widget/cgnszoneselectdialog.h \
           widget/coloreditwidget.h \
           widget/contoursettingwidget.h \
           widget/coordinateeditwidget.h \
           widget/dirnameeditwidget.h \
           widget/doublespinboxwithhelp.h \
           widget/filenameeditwidget.h \
           widget/gridshapeeditdialog.h \
           widget/integernumberdisplaylabel.h \
           widget/integernumbereditwidget.h \
           widget/itemmultiselectingdialog.h \
           widget/itemselectingdialog.h \
           widget/marginwidget.h \
           widget/realnumberdisplaylabel.h \
           widget/realnumbereditwidget.h \
           widget/sliderwithvalue.h \
           widget/textediti.h \
           widget/transparencywidget.h \
           widget/waitdialog.h \
           coordinates/private/coordinateseditdialogdelegate.h \
           point/private/pointcontroller_impl.h \
           polygon/private/polygoncontroller_impl.h \
           polyline/private/polylinecontroller_impl.h \
           vtktool/private/vtkactorpolydatamapperpair_impl.h \
           vtktool/private/vtklabel2dactor_impl.h \
           vtktool/private/vtklineactor_impl.h \
           vtktool/private/vtklinesactor_impl.h \
           vtktool/private/vtkpolygonactor_impl.h \
           vtktool/private/vtkpolygonsactor_impl.h \
           widget/private/itemselectingdialog_detail.h \
           widget/private/realnumbereditwidget_impl.h
FORMS += scalarbardialog.ui \
         scalarbarwidget.ui \
         structuredgridregionselectwidget.ui \
         vtklinestylewidget.ui \
         vtktextpropertysettingdialog.ui \
         vtktextpropertysettingwidget.ui \
         xyaxisdisplaysettingdialog.ui \
         colormap/colormapcustomsettingdialog.ui \
         colormap/colormapsettingwidget.ui \
         coordinates/coordinateseditdialog.ui \
         timeformat/timeformateditwidget.ui \
         widget/cgnszoneselectdialog.ui \
         widget/contoursettingwidget.ui \
         widget/dirnameeditwidget.ui \
         widget/doublespinboxwithhelp.ui \
         widget/filenameeditwidget.ui \
         widget/gridshapeeditdialog.ui \
         widget/itemmultiselectingdialog.ui \
         widget/itemselectingdialog.ui \
         widget/transparencywidget.ui \
         widget/waitdialog.ui
SOURCES += arrowsettingcontainer.cpp \
           colortool.cpp \
           comboboxtool.cpp \
           cursorchanger.cpp \
           graphicsmisc.cpp \
           iricactivecellfilter.cpp \
           irictoolbar.cpp \
           landxmlutil.cpp \
           linestyleinformation.cpp \
           objectbrowser.cpp \
           objectbrowserview.cpp \
           overridecursorchanger.cpp \
           pointstyleinformation.cpp \
           qpainterpenbrushchanger.cpp \
           qpainterpenbrushrestorer.cpp \
           qwtplotcustomcurve.cpp \
           qwtplotcustommarker.cpp \
           scalarbardialog.cpp \
           scalarbarsetting.cpp \
           scalarbarwidget.cpp \
           scalarsettingcontainer.cpp \
           structuredgridregion.cpp \
           structuredgridregionselectwidget.cpp \
           tooltiplabel.cpp \
           vtk2dinteractorstyle.cpp \
           vtkCustomScalarBarActor.cxx \
           vtkCustomStreamPoints.cxx \
           vtkCustomStreamTracer.cxx \
           vtkdatasetattributestool.cpp \
           vtklegendboxrepresentation.cpp \
           vtklegendboxwidget.cpp \
           vtklinestylecontainer.cpp \
           vtklinestylewidget.cpp \
           vtksubdividegrid.cpp \
           vtktextpropertysettingcontainer.cpp \
           vtktextpropertysettingdialog.cpp \
           vtktextpropertysettingwidget.cpp \
           xyaxisdisplaysettingdialog.cpp \
           colormap/colormapcustomsetting.cpp \
           colormap/colormapcustomsettingcolor.cpp \
           colormap/colormapcustomsettingdialog.cpp \
           colormap/colormapsettingwidget.cpp \
           coordinates/coordinatesedit.cpp \
           coordinates/coordinateseditdialog.cpp \
           executer/executerwatcher.cpp \
           executer/processexecuter.cpp \
           executer/silentexecuterwatcher.cpp \
           executer/threadexecuter.cpp \
           executer/unziparchiveprocessexecuter.cpp \
           executer/waitdialogexecuterwatcher.cpp \
           executer/ziparchiveprocessexecuter.cpp \
           geometry/pointring.cpp \
           geometry/polygonregion.cpp \
           geometry/rect.cpp \
           point/pointclearcommand.cpp \
           point/pointcontroller.cpp \
           point/pointmovecommand.cpp \
           point/pointsetcommand.cpp \
           polygon/polygonaddvertexcommand.cpp \
           polygon/polygoncontroller.cpp \
           polygon/polygoneditcoordinatescommand.cpp \
           polygon/polygonmovevertexcommand.cpp \
           polygon/polygonpushvertexcommand.cpp \
           polygon/polygonremovevertexcommand.cpp \
           polyline/polylineaddvertexcommand.cpp \
           polyline/polylinecontroller.cpp \
           polyline/polylinecontrollerutil.cpp \
           polyline/polylineeditcoordinatescommand.cpp \
           polyline/polylinemovevertexcommand.cpp \
           polyline/polylinepushvertexcommand.cpp \
           polyline/polylineremovevertexcommand.cpp \
           polyline/polylineutil.cpp \
           timeformat/timeformateditwidget.cpp \
           timeformat/timeformatutil.cpp \
           vtktool/vtkactorpolydatamapperpair.cpp \
           vtktool/vtklabel2dactor.cpp \
           vtktool/vtklineactor.cpp \
           vtktool/vtklinesactor.cpp \
           vtktool/vtkparametricsplineutil.cpp \
           vtktool/vtkpointsutil.cpp \
           vtktool/vtkpolydatalinesactor.cpp \
           vtktool/vtkpolydatapaintactor.cpp \
           vtktool/vtkpolydatapointsactor.cpp \
           vtktool/vtkpolygonactor.cpp \
           vtktool/vtkpolygonsactor.cpp \
           vtktool/vtkstreamtracerutil.cpp \
           widget/asciionlylineedit.cpp \
           widget/asciionlytextedit.cpp \
           widget/centeredcheckbox.cpp \
           widget/cgnszoneselectdialog.cpp \
           widget/coloreditwidget.cpp \
           widget/contoursettingwidget.cpp \
           widget/coordinateeditwidget.cpp \
           widget/dirnameeditwidget.cpp \
           widget/doublespinboxwithhelp.cpp \
           widget/filenameeditwidget.cpp \
           widget/gridshapeeditdialog.cpp \
           widget/integernumberdisplaylabel.cpp \
           widget/integernumbereditwidget.cpp \
           widget/itemmultiselectingdialog.cpp \
           widget/itemselectingdialog.cpp \
           widget/marginwidget.cpp \
           widget/realnumberdisplaylabel.cpp \
           widget/realnumbereditwidget.cpp \
           widget/sliderwithvalue.cpp \
           widget/transparencywidget.cpp \
           widget/waitdialog.cpp \
           coordinates/private/coordinateseditdialogdelegate.cpp
RESOURCES += guibase.qrc
TRANSLATIONS += languages/iricGuibase_ar_EG.ts \
                languages/iricGuibase_bg_BG.ts \
                languages/iricGuibase_bs_BA.ts \
                languages/iricGuibase_ca_ES.ts \
                languages/iricGuibase_cs_CZ.ts \
                languages/iricGuibase_da_DK.ts \
                languages/iricGuibase_de_DE.ts \
                languages/iricGuibase_el_GR.ts \
                languages/iricGuibase_es_ES.ts \
                languages/iricGuibase_et_EE.ts \
                languages/iricGuibase_eu_ES.ts \
                languages/iricGuibase_fi_FI.ts \
                languages/iricGuibase_fr_FR.ts \
                languages/iricGuibase_gl_ES.ts \
                languages/iricGuibase_hi_IN.ts \
                languages/iricGuibase_hu_HU.ts \
                languages/iricGuibase_id_ID.ts \
                languages/iricGuibase_is_IS.ts \
                languages/iricGuibase_it_IT.ts \
                languages/iricGuibase_ja_JP.ts \
                languages/iricGuibase_ko_KR.ts \
                languages/iricGuibase_ky_KG.ts \
                languages/iricGuibase_lt_LT.ts \
                languages/iricGuibase_lv_LV.ts \
                languages/iricGuibase_nb_NO.ts \
                languages/iricGuibase_nl_NL.ts \
                languages/iricGuibase_pl_PL.ts \
                languages/iricGuibase_pt_BR.ts \
                languages/iricGuibase_pt_PT.ts \
                languages/iricGuibase_ro_RO.ts \
                languages/iricGuibase_ru_RU.ts \
                languages/iricGuibase_sl_SI.ts \
                languages/iricGuibase_sv_SE.ts \
                languages/iricGuibase_th_TH.ts \
                languages/iricGuibase_tr_TR.ts \
                languages/iricGuibase_uk_UA.ts \
                languages/iricGuibase_vi_VN.ts \
                languages/iricGuibase_zh_CN.ts \
                languages/iricGuibase_zh_TW.ts

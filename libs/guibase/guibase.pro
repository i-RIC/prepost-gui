TARGET = iricGuibase
TEMPLATE = lib
INCLUDEPATH += ..

DEFINES += GUIBASE_LIBRARY

QT += widgets xml

include( ../../paths.pri )

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
	-lvtkCommonCore-6.1 \
	-lvtkCommonDataModel-6.1 \
	-lvtkCommonExecutionModel-6.1 \
	-lvtkCommonMath-6.1 \
	-lvtkFiltersFlowPaths-6.1 \
	-lvtkInteractionStyle-6.1 \
	-lvtkInteractionWidgets-6.1 \
	-lvtkRenderingAnnotation-6.1 \
	-lvtkRenderingCore-6.1 \
	-lvtkRenderingFreeType-6.1 \
	-lvtkRenderingLabel-6.1

# Post-Build Event
win32 {
	QMAKE_POST_LINK += copy $(TargetPath) $(SolutionDir)\\libdlls\\$(Configuration)
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
           linestyleinformation.h \
           objectbrowser.h \
           objectbrowserview.h \
           overridecursorchanger.h \
           pointstyleinformation.h \
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
           xyaxisdisplaysettingdialog.h \
           colormap/colormapcustomsetting.h \
           colormap/colormapcustomsettingcolor.h \
           colormap/colormapcustomsettingdialog.h \
           colormap/colormapsettingwidget.h \
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
           polyline/polylinecontroller.h \
           vtktool/vtkactorpolydatamapperpair.h \
           vtktool/vtklabel2dactor.h \
           vtktool/vtklineactor.h \
           vtktool/vtklinesactor.h \
           vtktool/vtkpointsutil.h \
           vtktool/vtkpolydatalinesactor.h \
           vtktool/vtkpolydatapaintactor.h \
           vtktool/vtkpolydatapointsactor.h \
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
           widget/itemselectingdialog.h \
           widget/marginwidget.h \
           widget/realnumberdisplaylabel.h \
           widget/realnumbereditwidget.h \
           widget/sliderwithvalue.h \
           widget/textediti.h \
           widget/transparencywidget.h \
           widget/waitdialog.h \
           polyline/private/polylinecontroller_impl.h \
           vtktool/private/vtkactorpolydatamapperpair_impl.h \
           vtktool/private/vtklabel2dactor_impl.h \
           vtktool/private/vtklineactor_impl.h \
           vtktool/private/vtklinesactor_impl.h \
           vtktool/private/vtkpolygonsactor_impl.h \
           widget/private/itemselectingdialog_detail.h \
           widget/private/realnumbereditwidget_impl.h
FORMS += scalarbardialog.ui \
         scalarbarwidget.ui \
         structuredgridregionselectwidget.ui \
         vtklinestylewidget.ui \
         vtktextpropertysettingdialog.ui \
         xyaxisdisplaysettingdialog.ui \
         colormap/colormapcustomsettingdialog.ui \
         colormap/colormapsettingwidget.ui \
         widget/cgnszoneselectdialog.ui \
         widget/contoursettingwidget.ui \
         widget/dirnameeditwidget.ui \
         widget/doublespinboxwithhelp.ui \
         widget/filenameeditwidget.ui \
         widget/gridshapeeditdialog.ui \
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
           linestyleinformation.cpp \
           objectbrowser.cpp \
           objectbrowserview.cpp \
           overridecursorchanger.cpp \
           pointstyleinformation.cpp \
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
           xyaxisdisplaysettingdialog.cpp \
           colormap/colormapcustomsetting.cpp \
           colormap/colormapcustomsettingcolor.cpp \
           colormap/colormapcustomsettingdialog.cpp \
           colormap/colormapsettingwidget.cpp \
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
           polyline/polylinecontroller.cpp \
           vtktool/vtkactorpolydatamapperpair.cpp \
           vtktool/vtklabel2dactor.cpp \
           vtktool/vtklineactor.cpp \
           vtktool/vtklinesactor.cpp \
           vtktool/vtkpointsutil.cpp \
           vtktool/vtkpolydatalinesactor.cpp \
           vtktool/vtkpolydatapaintactor.cpp \
           vtktool/vtkpolydatapointsactor.cpp \
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
           widget/itemselectingdialog.cpp \
           widget/marginwidget.cpp \
           widget/realnumberdisplaylabel.cpp \
           widget/realnumbereditwidget.cpp \
           widget/sliderwithvalue.cpp \
           widget/transparencywidget.cpp \
           widget/waitdialog.cpp
RESOURCES += guibase.qrc
TRANSLATIONS += languages/iricGuibase_es_ES.ts \
                languages/iricGuibase_fr_FR.ts \
                languages/iricGuibase_id_ID.ts \
                languages/iricGuibase_ja_JP.ts \
                languages/iricGuibase_ko_KR.ts \
                languages/iricGuibase_ru_RU.ts \
                languages/iricGuibase_th_TH.ts \
                languages/iricGuibase_vi_VN.ts \
                languages/iricGuibase_zh_CN.ts

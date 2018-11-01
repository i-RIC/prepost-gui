######################################################################
# Automatically generated by qmake (2.01a) ? 10 23 18:49:25 2014
######################################################################

TARGET = iricGdRiversurvey
TEMPLATE = lib
INCLUDEPATH += ../..

DEFINES += GD_RIVERSURVEY_LIBRARY

include( ../../../paths.pri )

QT += widgets xml

######################
# Internal libraries #
######################

# iricMisc

win32 {
	CONFIG(debug, debug|release) {
		LIBS += -L"../../misc/debug"
	} else {
		LIBS += -L"../../misc/release"
	}
}
unix {
	LIBS += -L"../../misc"
}
	LIBS += -liricMisc

# iricGuibase

win32 {
	CONFIG(debug, debug|release) {
		LIBS += -L"../../guibase/debug"
	} else {
		LIBS += -L"../../guibase/release"
	}
}
unix {
	LIBS += -L"../../guibase"
}
LIBS += -liricGuibase

# iricGuicore

win32 {
	CONFIG(debug, debug|release) {
		LIBS += -L"../../guicore/debug"
	} else {
		LIBS += -L"../../guicore/release"
	}
}
unix {
	LIBS += -L"../../guicore"
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
	-lvtkFiltersExtraction-6.1 \
	-lvtkFiltersGeometry-6.1 \
	-lvtkIOCore-6.1 \
	-lvtkIOLegacy-6.1 \
	-lvtkRenderingCore-6.1 \
	-lvtkRenderingLabel-6.1 \
	-lvtkRenderingLOD-6.1

# iriclib

LIBS += -liriclib

# Post-Build Event
win32 {
	QMAKE_POST_LINK += copy $(TargetPath) $(SolutionDir)\\libdlls\\$(Configuration)
}

# Input
HEADERS += gd_riversurvey_global.h \
           geodatarivercrosssection.h \
           geodatarivercrosssectionaltitudemovedialog.h \
           geodatariverpathpoint.h \
           geodatariverpathpointexpanddialog.h \
           geodatariverpathpointextensionadddialog.h \
           geodatariverpathpointinsertdialog.h \
           geodatariverpathpointmovedialog.h \
           geodatariverpathpointrenamedialog.h \
           geodatariverpathpointrotatedialog.h \
           geodatariverpathpointshiftdialog.h \
           geodatarivershapeinterpolator.h \
           geodatariversurvey.h \
           geodatariversurveybackgroundgridcreatethread.h \
           geodatariversurveycreator.h \
           geodatariversurveycrosssectionwindow.h \
           geodatariversurveycrosssectionwindowdelegate.h \
           geodatariversurveycrosssectionwindowgraphicsview.h \
           geodatariversurveycrosssectionwindowprojectdataitem.h \
           geodatariversurveycrosssectionwindowtableview.h \
           geodatariversurveyctrlpointbackup.h \
           geodatariversurveydisplaysetting.h \
           geodatariversurveydisplaysettingdialog.h \
           geodatariversurveyexporter.h \
           geodatariversurveyimporter.h \
           geodatariversurveyimportersettingdialog.h \
           geodatariversurveynodemapper.h \
           geodatariversurveyproxy.h \
           geodatariversurveyvtkexporter.h \
           gridcreatingconditionriversurveyinterface.h \
           private/geodatariversurvey_addextensioncommand.h \
           private/geodatariversurvey_changeselectioncommand.h \
           private/geodatariversurvey_deleteriverpathpointcommand.h \
           private/geodatariversurvey_expandcrosssectioncommand.h \
           private/geodatariversurvey_insertriverpathpointcommand.h \
           private/geodatariversurvey_mouseeditcrosssectioncommand.h \
           private/geodatariversurvey_mousemoveextensioncommand.h \
           private/geodatariversurvey_mouserotaterivercrosssectioncommand.h \
           private/geodatariversurvey_mouseshiftriverpathcentercommand.h \
           private/geodatariversurvey_moveriverpathpointcommand.h \
           private/geodatariversurvey_removeextensioncommand.h \
           private/geodatariversurvey_renameriverpathpointcommand.h \
           private/geodatariversurvey_rotaterivercrosssectioncommand.h \
           private/geodatariversurvey_setdisplaysettingcommand.h \
           private/geodatariversurvey_translateriverpathpointcommand.h \
           private/geodatariversurveycrosssectionwindow_impl.h \
           private/geodatariversurveyproxy_setsettingcommand.h
FORMS += geodatarivercrosssectionaltitudemovedialog.ui \
         geodatariverpathpointexpanddialog.ui \
         geodatariverpathpointextensionadddialog.ui \
         geodatariverpathpointinsertdialog.ui \
         geodatariverpathpointmovedialog.ui \
         geodatariverpathpointrenamedialog.ui \
         geodatariverpathpointrotatedialog.ui \
         geodatariverpathpointshiftdialog.ui \
         geodatariversurveycrosssectionwindow.ui \
         geodatariversurveydisplaysettingdialog.ui \
         geodatariversurveyimportersettingdialog.ui
SOURCES += geodatarivercrosssection.cpp \
           geodatarivercrosssectionaltitudemovedialog.cpp \
           geodatariverpathpoint.cpp \
           geodatariverpathpointexpanddialog.cpp \
           geodatariverpathpointextensionadddialog.cpp \
           geodatariverpathpointinsertdialog.cpp \
           geodatariverpathpointmovedialog.cpp \
           geodatariverpathpointrenamedialog.cpp \
           geodatariverpathpointrotatedialog.cpp \
           geodatariverpathpointshiftdialog.cpp \
           geodatarivershapeinterpolator.cpp \
           geodatariversurvey.cpp \
           geodatariversurveybackgroundgridcreatethread.cpp \
           geodatariversurveycreator.cpp \
           geodatariversurveycrosssectionwindow.cpp \
           geodatariversurveycrosssectionwindowdelegate.cpp \
           geodatariversurveycrosssectionwindowgraphicsview.cpp \
           geodatariversurveycrosssectionwindowprojectdataitem.cpp \
           geodatariversurveycrosssectionwindowtableview.cpp \
           geodatariversurveyctrlpointbackup.cpp \
           geodatariversurveydisplaysetting.cpp \
           geodatariversurveydisplaysettingdialog.cpp \
           geodatariversurveyexporter.cpp \
           geodatariversurveyimporter.cpp \
           geodatariversurveyimportersettingdialog.cpp \
           geodatariversurveynodemapper.cpp \
           geodatariversurveyproxy.cpp \
           geodatariversurveyvtkexporter.cpp \
           private/geodatariversurvey_addextensioncommand.cpp \
           private/geodatariversurvey_changeselectioncommand.cpp \
           private/geodatariversurvey_deleteriverpathpointcommand.cpp \
           private/geodatariversurvey_expandcrosssectioncommand.cpp \
           private/geodatariversurvey_insertriverpathpointcommand.cpp \
           private/geodatariversurvey_mouseeditcrosssectioncommand.cpp \
           private/geodatariversurvey_mousemoveextensioncommand.cpp \
           private/geodatariversurvey_mouserotaterivercrosssectioncommand.cpp \
           private/geodatariversurvey_mouseshiftriverpathcentercommand.cpp \
           private/geodatariversurvey_moveriverpathpointcommand.cpp \
           private/geodatariversurvey_removeextensioncommand.cpp \
           private/geodatariversurvey_renameriverpathpointcommand.cpp \
           private/geodatariversurvey_rotaterivercrosssectioncommand.cpp \
           private/geodatariversurvey_setdisplaysettingcommand.cpp \
           private/geodatariversurvey_translateriverpathpointcommand.cpp \
           private/geodatariversurveyproxy_setsettingcommand.cpp
RESOURCES += riversurvey.qrc
TRANSLATIONS += languages/iricGdRiversurvey_es_ES.ts \
                languages/iricGdRiversurvey_fr_FR.ts \
                languages/iricGdRiversurvey_id_ID.ts \
                languages/iricGdRiversurvey_ja_JP.ts \
                languages/iricGdRiversurvey_ko_KR.ts \
                languages/iricGdRiversurvey_ru_RU.ts \
                languages/iricGdRiversurvey_th_TH.ts \
                languages/iricGdRiversurvey_vi_VN.ts \
                languages/iricGdRiversurvey_zh_CN.ts

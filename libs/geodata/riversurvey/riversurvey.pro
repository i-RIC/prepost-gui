TARGET = iricGdRiversurvey
TEMPLATE = lib
INCLUDEPATH += ../..

DEFINES += GD_RIVERSURVEY_LIBRARY

include( ../../../paths.pri )

QT += widgets xml

######################
# Internal libraries #
######################

# iricGdPointmap

win32 {
        CONFIG(debug, debug|release) {
                LIBS += -L"../pointmap/debug"
        } else {
                LIBS += -L"../pointmap/release"
        }
}
unix {
        LIBS += -L"../pointmap"
}
LIBS += -liricGdPointmap

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

# iricGeoio

win32 {
        CONFIG(debug, debug|release) {
                LIBS += -L"../../geoio/debug"
        } else {
                LIBS += -L"../../geoio/release"
        }
}
unix {
        LIBS += -L"../../geoio"
}
LIBS += -liricGeoio

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

# iricGdPolyLine

win32 {
	CONFIG(debug, debug|release) {
		LIBS += -L"../../geodata/polyline/debug"
	} else {
		LIBS += -L"../../geodata/polyline/release"
	}
}
unix {
	LIBS += -L"../../geodata/polyline"
}
LIBS += -liricGdPolyLine

# iricHdRiversurveywaterelevation

win32 {
	CONFIG(debug, debug|release) {
		LIBS += -L"../../hydraulicdata/riversurveywaterelevation/debug"
	} else {
		LIBS += -L"../../hydraulicdata/riversurveywaterelevation/release"
	}
}
unix {
	LIBS += -L"../../hydraulicdata/riversurveywaterelevation"
}
LIBS += -liricHdRiversurveywaterelevation

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
           geodatariversurveybuildbanklinesdialog.h \
           geodatariversurveycreator.h \
           geodatariversurveycrosssectiondisplaysetting.h \
           geodatariversurveycrosssectiondisplaysettingdialog.h \
           geodatariversurveycrosssectioneditfrompointdialog.h \
           geodatariversurveycrosssectionwindow.h \
           geodatariversurveycrosssectionwindowgraphicsview.h \
           geodatariversurveycrosssectionwindowprojectdataitem.h \
           geodatariversurveycrosssectionwindowtableview.h \
           geodatariversurveycrosssectionwindowwsetablewidget.h \
           geodatariversurveyctrlpointbackup.h \
           geodatariversurveydisplaysetting.h \
           geodatariversurveydisplaysettingdialog.h \
           geodatariversurveyexporter.h \
           geodatariversurveygeneratedialog.h \
           geodatariversurveyhonryuexporter.h \
           geodatariversurveyhonryuexportersettingdialog.h \
           geodatariversurveyimporter.h \
           geodatariversurveyimportersettingdialog.h \
           geodatariversurveylandxmlexporter.h \
           geodatariversurveymappointsdialog.h \
           geodatariversurveymlitimporter.h \
           geodatariversurveynodemapper.h \
           geodatariversurveyproxy.h \
           geodatariversurveyvtkexporter.h \
           gridcreatingconditionriversurveyinterface.h \
           private/geodatariversurvey_addextensioncommand.h \
           private/geodatariversurvey_changeselectioncommand.h \
           private/geodatariversurvey_deleteriverpathpointcommand.h \
           private/geodatariversurvey_editcrosssectioncommand.h \
           private/geodatariversurvey_expandcrosssectioncommand.h \
           private/geodatariversurvey_impl.h \
           private/geodatariversurvey_insertriverpathpointcommand.h \
           private/geodatariversurvey_mouseeditcrosssectioncommand.h \
           private/geodatariversurvey_mousemoveextensioncommand.h \
           private/geodatariversurvey_mouserotaterivercrosssectioncommand.h \
           private/geodatariversurvey_mouseshiftriverpathcentercommand.h \
           private/geodatariversurvey_moveriverpathpointcommand.h \
           private/geodatariversurvey_polylinecoordinateseditor.h \
           private/geodatariversurvey_polylinefinishdefiningcommand.h \
           private/geodatariversurvey_polylineupdatelabelscommand.h \
           private/geodatariversurvey_removeextensioncommand.h \
           private/geodatariversurvey_renameriverpathpointcommand.h \
           private/geodatariversurvey_rotaterivercrosssectioncommand.h \
           private/geodatariversurvey_setdisplaysettingcommand.h \
           private/geodatariversurvey_shiftriverpathcentercommand.h \
           private/geodatariversurvey_translateriverpathpointcommand.h \
           private/geodatariversurveycrosssectionwindow_datatabledelegate.h \
           private/geodatariversurveycrosssectionwindow_impl.h \
           private/geodatariversurveycrosssectionwindow_riversurveytabledelegate.h \
           private/geodatariversurveycrosssectionwindow_wsetabledelegate.h \
           private/geodatariversurveycrosssectionwindowgraphicsview_setdisplaysettingcommand.h \
           private/geodatariversurveyimporter_problemsdialog.h \
           private/geodatariversurveymlitimporter_problemsdialog.h \
           private/geodatariversurveyproxy_setsettingcommand.h
FORMS += geodatarivercrosssectionaltitudemovedialog.ui \
         geodatariverpathpointexpanddialog.ui \
         geodatariverpathpointextensionadddialog.ui \
         geodatariverpathpointinsertdialog.ui \
         geodatariverpathpointmovedialog.ui \
         geodatariverpathpointrenamedialog.ui \
         geodatariverpathpointrotatedialog.ui \
         geodatariverpathpointshiftdialog.ui \
         geodatariversurveybuildbanklinesdialog.ui \
         geodatariversurveycrosssectiondisplaysettingdialog.ui \
         geodatariversurveycrosssectioneditfrompointdialog.ui \
         geodatariversurveycrosssectionwindow.ui \
         geodatariversurveydisplaysettingdialog.ui \
         geodatariversurveygeneratedialog.ui \
         geodatariversurveyhonryuexportersettingdialog.ui \
         geodatariversurveyimportersettingdialog.ui \
         geodatariversurveymappointsdialog.ui \
         private/geodatariversurveyimporter_problemsdialog.ui \
         private/geodatariversurveymlitimporter_problemsdialog.ui
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
           geodatariversurveybuildbanklinesdialog.cpp \
           geodatariversurveycreator.cpp \
           geodatariversurveycrosssectiondisplaysetting.cpp \
           geodatariversurveycrosssectiondisplaysettingdialog.cpp \
           geodatariversurveycrosssectioneditfrompointdialog.cpp \
           geodatariversurveycrosssectionwindow.cpp \
           geodatariversurveycrosssectionwindowgraphicsview.cpp \
           geodatariversurveycrosssectionwindowprojectdataitem.cpp \
           geodatariversurveycrosssectionwindowtableview.cpp \
           geodatariversurveycrosssectionwindowwsetablewidget.cpp \
           geodatariversurveyctrlpointbackup.cpp \
           geodatariversurveydisplaysetting.cpp \
           geodatariversurveydisplaysettingdialog.cpp \
           geodatariversurveyexporter.cpp \
           geodatariversurveygeneratedialog.cpp \
           geodatariversurveyhonryuexporter.cpp \
           geodatariversurveyhonryuexportersettingdialog.cpp \
           geodatariversurveyimporter.cpp \
           geodatariversurveyimportersettingdialog.cpp \
           geodatariversurveylandxmlexporter.cpp \
           geodatariversurveymappointsdialog.cpp \
           geodatariversurveymlitimporter.cpp \
           geodatariversurveynodemapper.cpp \
           geodatariversurveyproxy.cpp \
           geodatariversurveyvtkexporter.cpp \
           private/geodatariversurvey_addextensioncommand.cpp \
           private/geodatariversurvey_changeselectioncommand.cpp \
           private/geodatariversurvey_deleteriverpathpointcommand.cpp \
           private/geodatariversurvey_editcrosssectioncommand.cpp \
           private/geodatariversurvey_expandcrosssectioncommand.cpp \
           private/geodatariversurvey_impl.cpp \
           private/geodatariversurvey_insertriverpathpointcommand.cpp \
           private/geodatariversurvey_mouseeditcrosssectioncommand.cpp \
           private/geodatariversurvey_mousemoveextensioncommand.cpp \
           private/geodatariversurvey_mouserotaterivercrosssectioncommand.cpp \
           private/geodatariversurvey_mouseshiftriverpathcentercommand.cpp \
           private/geodatariversurvey_moveriverpathpointcommand.cpp \
           private/geodatariversurvey_polylinecoordinateseditor.cpp \
           private/geodatariversurvey_polylinefinishdefiningcommand.cpp \
           private/geodatariversurvey_polylineupdatelabelscommand.cpp \
           private/geodatariversurvey_removeextensioncommand.cpp \
           private/geodatariversurvey_renameriverpathpointcommand.cpp \
           private/geodatariversurvey_rotaterivercrosssectioncommand.cpp \
           private/geodatariversurvey_setdisplaysettingcommand.cpp \
           private/geodatariversurvey_shiftriverpathcentercommand.cpp \
           private/geodatariversurvey_translateriverpathpointcommand.cpp \
           private/geodatariversurveycrosssectionwindow_datatabledelegate.cpp \
           private/geodatariversurveycrosssectionwindow_riversurveytabledelegate.cpp \
           private/geodatariversurveycrosssectionwindow_wsetabledelegate.cpp \
           private/geodatariversurveycrosssectionwindowgraphicsview_setdisplaysettingcommand.cpp \
           private/geodatariversurveyimporter_problemsdialog.cpp \
           private/geodatariversurveymlitimporter_problemsdialog.cpp \
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

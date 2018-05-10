TARGET = iricGccCompoundchannel
TEMPLATE = lib
INCLUDEPATH += ../..

DEFINES += GCC_COMPOUNDCHANNEL_LIBRARY

include( ../../../paths.pri )

QT += widgets xml

######################
# Internal libraries #
######################

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
	-lvtkCommonComputationalGeometry-6.1 \
	-lvtkCommonCore-6.1 \
	-lvtkCommonDataModel-6.1 \
	-lvtkRenderingCore-6.1 \
	-lvtkRenderingLabel-6.1

# Post-Build Event
win32 {
	QMAKE_POST_LINK += copy $(TargetPath) $(SolutionDir)\\libdlls\\$(Configuration)
}

# Input
HEADERS += gcc_compoundchannel_global.h \
           gridcreatingconditioncompoundchannel.h \
           gridcreatingconditioncompoundchannelabstractline.h \
           gridcreatingconditioncompoundchannelabstractpolygon.h \
           gridcreatingconditioncompoundchannelcenterline.h \
           gridcreatingconditioncompoundchannelfunctions.h \
           gridcreatingconditioncompoundchannelgridregionpolygon.h \
           gridcreatingconditioncompoundchannellowwaterchannelpolygon.h \
           gridcreatingconditioncompoundchannelsettingdialog.h \
           gridcreatingconditioncompoundchannelspline.h \
           gridcreatingconditioncreatorcompoundchannel.h \
           private/gridcreatingconditioncompoundchannel_addpolygonvertexcommand.h \
           private/gridcreatingconditioncompoundchannel_addpolylinevertexcommand.h \
           private/gridcreatingconditioncompoundchannel_definepolygonnewpointcommand.h \
           private/gridcreatingconditioncompoundchannel_definepolylinenewpointcommand.h \
           private/gridcreatingconditioncompoundchannel_editpolygoncoordinatescommand.h \
           private/gridcreatingconditioncompoundchannel_editpolylinecoordinatescommand.h \
           private/gridcreatingconditioncompoundchannel_movepolygoncommand.h \
           private/gridcreatingconditioncompoundchannel_movepolygonvertexcommand.h \
           private/gridcreatingconditioncompoundchannel_movepolylinecommand.h \
           private/gridcreatingconditioncompoundchannel_movepolylinevertexcommand.h \
           private/gridcreatingconditioncompoundchannel_polygoncoordinateseditor.h \
           private/gridcreatingconditioncompoundchannel_polylinecoordinateseditor.h \
           private/gridcreatingconditioncompoundchannel_removepolygonvertexcommand.h \
           private/gridcreatingconditioncompoundchannel_removepolylinevertexcommand.h \
           private/gridcreatingconditioncompoundchannel_switchstatuscommand.h
FORMS += gridcreatingconditioncompoundchannelsettingdialog.ui
SOURCES += gridcreatingconditioncompoundchannel.cpp \
           gridcreatingconditioncompoundchannelabstractline.cpp \
           gridcreatingconditioncompoundchannelabstractpolygon.cpp \
           gridcreatingconditioncompoundchannelcenterline.cpp \
           gridcreatingconditioncompoundchannelfunctions.cpp \
           gridcreatingconditioncompoundchannelgridregionpolygon.cpp \
           gridcreatingconditioncompoundchannellowwaterchannelpolygon.cpp \
           gridcreatingconditioncompoundchannelsettingdialog.cpp \
           gridcreatingconditioncompoundchannelspline.cpp \
           gridcreatingconditioncreatorcompoundchannel.cpp \
           private/gridcreatingconditioncompoundchannel_addpolygonvertexcommand.cpp \
           private/gridcreatingconditioncompoundchannel_addpolylinevertexcommand.cpp \
           private/gridcreatingconditioncompoundchannel_definepolygonnewpointcommand.cpp \
           private/gridcreatingconditioncompoundchannel_definepolylinenewpointcommand.cpp \
           private/gridcreatingconditioncompoundchannel_editpolygoncoordinatescommand.cpp \
           private/gridcreatingconditioncompoundchannel_editpolylinecoordinatescommand.cpp \
           private/gridcreatingconditioncompoundchannel_movepolygoncommand.cpp \
           private/gridcreatingconditioncompoundchannel_movepolygonvertexcommand.cpp \
           private/gridcreatingconditioncompoundchannel_movepolylinecommand.cpp \
           private/gridcreatingconditioncompoundchannel_movepolylinevertexcommand.cpp \
           private/gridcreatingconditioncompoundchannel_polygoncoordinateseditor.cpp \
           private/gridcreatingconditioncompoundchannel_polylinecoordinateseditor.cpp \
           private/gridcreatingconditioncompoundchannel_removepolygonvertexcommand.cpp \
           private/gridcreatingconditioncompoundchannel_removepolylinevertexcommand.cpp \
           private/gridcreatingconditioncompoundchannel_switchstatuscommand.cpp
RESOURCES += compoundchannel.qrc
TRANSLATIONS += languages/iricGccCompoundchannel_es_ES.ts \
                languages/iricGccCompoundchannel_fr_FR.ts \
                languages/iricGccCompoundchannel_id_ID.ts \
                languages/iricGccCompoundchannel_ja_JP.ts \
                languages/iricGccCompoundchannel_ko_KR.ts \
                languages/iricGccCompoundchannel_ru_RU.ts \
                languages/iricGccCompoundchannel_th_TH.ts \
                languages/iricGccCompoundchannel_vi_VN.ts \
                languages/iricGccCompoundchannel_zh_CN.ts

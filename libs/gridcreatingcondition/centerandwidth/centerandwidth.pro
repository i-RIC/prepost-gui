TARGET = iricGccCenterandwidth
TEMPLATE = lib
INCLUDEPATH += ../..

DEFINES += GCC_CENTERANDWIDTH_LIBRARY

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
HEADERS += gcc_centerandwidth_global.h \
           gridcreatingconditioncenterandwidth.h \
           gridcreatingconditioncenterandwidthdialog.h \
           gridcreatingconditioncreatorcenterandwidth.h \
           private/gridcreatingconditioncenterandwidth_addvertexcommand.h \
           private/gridcreatingconditioncenterandwidth_coordinateseditor.h \
           private/gridcreatingconditioncenterandwidth_editcoordinatescommand.h \
           private/gridcreatingconditioncenterandwidth_finishdefiningcommand.h \
           private/gridcreatingconditioncenterandwidth_movecommand.h \
           private/gridcreatingconditioncenterandwidth_movevertexcommand.h \
           private/gridcreatingconditioncenterandwidth_pushvertexcommand.h \
           private/gridcreatingconditioncenterandwidth_removevertexcommand.h \
           private/gridcreatingconditioncenterandwidth_updateshapecommand.h
FORMS += gridcreatingconditioncenterandwidthdialog.ui
SOURCES += gridcreatingconditioncenterandwidth.cpp \
           gridcreatingconditioncenterandwidthdialog.cpp \
           gridcreatingconditioncreatorcenterandwidth.cpp \
           private/gridcreatingconditioncenterandwidth_addvertexcommand.cpp \
           private/gridcreatingconditioncenterandwidth_coordinateseditor.cpp \
           private/gridcreatingconditioncenterandwidth_editcoordinatescommand.cpp \
           private/gridcreatingconditioncenterandwidth_finishdefiningcommand.cpp \
           private/gridcreatingconditioncenterandwidth_movecommand.cpp \
           private/gridcreatingconditioncenterandwidth_movevertexcommand.cpp \
           private/gridcreatingconditioncenterandwidth_pushvertexcommand.cpp \
           private/gridcreatingconditioncenterandwidth_removevertexcommand.cpp \
           private/gridcreatingconditioncenterandwidth_updateshapecommand.cpp
RESOURCES += centerandwidth.qrc
TRANSLATIONS += languages/iricGccCenterandwidth_es_ES.ts \
                languages/iricGccCenterandwidth_fr_FR.ts \
                languages/iricGccCenterandwidth_id_ID.ts \
                languages/iricGccCenterandwidth_ja_JP.ts \
                languages/iricGccCenterandwidth_ko_KR.ts \
                languages/iricGccCenterandwidth_ru_RU.ts \
                languages/iricGccCenterandwidth_th_TH.ts \
                languages/iricGccCenterandwidth_vi_VN.ts \
                languages/iricGccCenterandwidth_zh_CN.ts

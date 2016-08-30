TARGET = iricMeasureddata
TEMPLATE = lib
INCLUDEPATH += .

DEFINES += MEASUREDDATA_LIBRARY

include( ../../../paths.pri )

QT += widgets xml

######################
# Internal libraries #
######################

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

######################
# External libraries #
######################

# VTK

LIBS += \
	-lvtkCommonCore-6.1 \
	-lvtkCommonDataModel-6.1 \
	-lvtkCommonExecutionModel-6.1 \
	-lvtkCommonMisc-6.1 \
	-lvtkFiltersCore-6.1 \
	-lvtkFiltersGeneral-6.1 \
	-lvtkFiltersGeometry-6.1 \
	-lvtkFiltersSources-6.1 \
	-lvtkInteractionWidgets-6.1 \
	-lvtkRenderingCore-6.1 \
	-lvtkRenderingFreeType-6.1 \
	-lvtkRenderingLOD-6.1

# Post-Build Event
win32 {
	QMAKE_POST_LINK += copy $(TargetPath) $(SolutionDir)\\libdlls\\$(Configuration)
}

# Input
HEADERS += measureddata_api.h \
           measureddatafiledataitem.h \
           measureddatapointdataitem.h \
           measureddatapointgroupdataitem.h \
           measureddatapointsetting.h \
           measureddatapointsettingdialog.h \
           measureddatavectordataitem.h \
           measureddatavectorgroupdataitem.h \
           measureddatavectorsetting.h \
           measureddatavectorsettingdialog.h \
           private/measureddatafiledataitem_impl.h \
           private/measureddatapointgroupdataitem_impl.h \
           private/measureddatapointgroupdataitem_setsettingcommand.h \
           private/measureddatavectorgroupdataitem_impl.h \
           private/measureddatavectorgroupdataitem_setsettingcommand.h
FORMS += measureddatapointsettingdialog.ui measureddatavectorsettingdialog.ui
SOURCES += measureddatafiledataitem.cpp \
           measureddatapointdataitem.cpp \
           measureddatapointgroupdataitem.cpp \
           measureddatapointsetting.cpp \
           measureddatapointsettingdialog.cpp \
           measureddatavectordataitem.cpp \
           measureddatavectorgroupdataitem.cpp \
           measureddatavectorsetting.cpp \
           measureddatavectorsettingdialog.cpp \
           private/measureddatapointgroupdataitem_setsettingcommand.cpp \
           private/measureddatavectorgroupdataitem_setsettingcommand.cpp
TRANSLATIONS += languages/iricMeasureddata_es_ES.ts \
                languages/iricMeasureddata_fr_FR.ts \
                languages/iricMeasureddata_id_ID.ts \
                languages/iricMeasureddata_ja_JP.ts \
                languages/iricMeasureddata_ko_KR.ts \
                languages/iricMeasureddata_ru_RU.ts \
                languages/iricMeasureddata_th_TH.ts \
                languages/iricMeasureddata_vi_VN.ts \
                languages/iricMeasureddata_zh_CN.ts

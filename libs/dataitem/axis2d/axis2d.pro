TARGET = iricAxis2d
TEMPLATE = lib
INCLUDEPATH += .

DEFINES += AXIS2D_LIBRARY

include( ../../../paths.pri )

QT += widgets

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

######################
# External libraries #
######################

# VTK

LIBS += \
	-lvtkCommonCore-6.1 \
	-lvtkInteractionWidgets-6.1 \
	-lvtkRenderingAnnotation-6.1 \
	-lvtkRenderingCore-6.1

# Post-Build Event
win32 {
	QMAKE_POST_LINK += copy $(TargetPath) $(SolutionDir)\\libdlls\\$(Configuration)
}

# Input
HEADERS += axis2d_api.h axis2ddataitem.h private/axis2ddataitem_impl.h
SOURCES += axis2ddataitem.cpp
TRANSLATIONS += languages/iricAxis2d_es_ES.ts \
                languages/iricAxis2d_fr_FR.ts \
                languages/iricAxis2d_id_ID.ts \
                languages/iricAxis2d_ja_JP.ts \
                languages/iricAxis2d_ko_KR.ts \
                languages/iricAxis2d_ru_RU.ts \
                languages/iricAxis2d_th_TH.ts \
                languages/iricAxis2d_vi_VN.ts \
                languages/iricAxis2d_zh_CN.ts

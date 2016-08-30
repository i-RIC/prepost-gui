TARGET = iricAxis3d
TEMPLATE = lib
INCLUDEPATH += .

DEFINES += AXIS3D_LIBRARY

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
HEADERS += axis3d_api.h axis3ddataitem.h private/axis3ddataitem_impl.h
SOURCES += axis3ddataitem.cpp
TRANSLATIONS += languages/iricAxis3d_es_ES.ts \
                languages/iricAxis3d_fr_FR.ts \
                languages/iricAxis3d_id_ID.ts \
                languages/iricAxis3d_ja_JP.ts \
                languages/iricAxis3d_ko_KR.ts \
                languages/iricAxis3d_ru_RU.ts \
                languages/iricAxis3d_th_TH.ts \
                languages/iricAxis3d_vi_VN.ts \
                languages/iricAxis3d_zh_CN.ts

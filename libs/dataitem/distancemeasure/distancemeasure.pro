TARGET = iricDistancemeasure
TEMPLATE = lib
INCLUDEPATH += .

DEFINES += DISTANCEMEASURE_LIBRARY

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
	-lvtkRenderingCore-6.1 \
	-lvtkRenderingLabel-6.1

# Post-Build Event
win32 {
	QMAKE_POST_LINK += copy $(TargetPath) $(SolutionDir)\\libdlls\\$(Configuration)
}

# Input
HEADERS += distancemeasure_api.h \
           distancemeasuredataitem.h \
           distancemeasuregroupdataitem.h \
           distancemeasurepropertydialog.h \
           distancemeasuresetting.h \
           private/distancemeasuredataitem_definecommand.h \
           private/distancemeasuredataitem_impl.h \
           private/distancemeasuredataitem_movevertexcommand.h \
           private/distancemeasuredataitem_redefinecommand.h \
           private/distancemeasuredataitem_setsettingcommand.h \
           private/distancemeasuredataitem_translatecommand.h \
           private/distancemeasuregroupdataitem_impl.h
FORMS += distancemeasurepropertydialog.ui
SOURCES += \
           distancemeasuredataitem.cpp \
           distancemeasuregroupdataitem.cpp \
           distancemeasurepropertydialog.cpp \
           distancemeasuresetting.cpp \
           private/distancemeasuredataitem_definecommand.cpp \
           private/distancemeasuredataitem_movevertexcommand.cpp \
           private/distancemeasuredataitem_redefinecommand.cpp \
           private/distancemeasuredataitem_setsettingcommand.cpp \
           private/distancemeasuredataitem_translatecommand.cpp
TRANSLATIONS += languages/iricDistancemeasure_es_ES.ts \
                languages/iricDistancemeasure_fr_FR.ts \
                languages/iricDistancemeasure_id_ID.ts \
                languages/iricDistancemeasure_ja_JP.ts \
                languages/iricDistancemeasure_ko_KR.ts \
                languages/iricDistancemeasure_ru_RU.ts \
                languages/iricDistancemeasure_th_TH.ts \
                languages/iricDistancemeasure_vi_VN.ts \
                languages/iricDistancemeasure_zh_CN.ts

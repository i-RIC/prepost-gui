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

# Input
HEADERS += distancemeasure_api.h \
           distancemeasurecopypropertydialog.h \
           distancemeasuredataitem.h \
           distancemeasuregroupdataitem.h \
           distancemeasuresetting.h \
           private/distancemeasuredataitem_redefinecommand.h \
           private/distancemeasuredataitem_translatecommand.h \
           private/distancemeasuregroupdataitem_impl.h
FORMS += distancemeasurecopypropertydialog.ui
SOURCES += distancemeasurecopypropertydialog.cpp \
           distancemeasuredataitem.cpp \
           distancemeasuregroupdataitem.cpp \
           distancemeasuresetting.cpp \
           private/distancemeasuredataitem_redefinecommand.cpp \
           private/distancemeasuredataitem_translatecommand.cpp

TARGET = iricGdPolydata
TEMPLATE = lib
INCLUDEPATH += ../..

DEFINES += GD_POLYDATA_LIBRARY

DEFINES += ANSI_DECLARATORS

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

# Post-Build Event
win32 {
	QMAKE_POST_LINK += copy $(TargetPath) $(SolutionDir)\\libdlls\\$(Configuration)
}

# Input
HEADERS += gd_polydata_global.h \
           geodatapolydata.h \
           geodatapolydatacolorsettingdialog.h \
           private/geodatapolydata_editnameandvaluecommand.h \
           private/geodatapolydata_impl.h \
           private/geodatapolydata_setcolorsettingcommand.h
FORMS += geodatapolydatacolorsettingdialog.ui
SOURCES += geodatapolydata.cpp \
           geodatapolydatacolorsettingdialog.cpp \
           private/geodatapolydata_editnameandvaluecommand.cpp \
           private/geodatapolydata_setcolorsettingcommand.cpp
TRANSLATIONS += languages/iricGdPolydata_es_ES.ts \
                languages/iricGdPolydata_fr_FR.ts \
                languages/iricGdPolydata_id_ID.ts \
                languages/iricGdPolydata_ja_JP.ts \
                languages/iricGdPolydata_ko_KR.ts \
                languages/iricGdPolydata_ru_RU.ts \
                languages/iricGdPolydata_th_TH.ts \
                languages/iricGdPolydata_vi_VN.ts \
                languages/iricGdPolydata_zh_CN.ts

TARGET = iricGccPoisson
TEMPLATE = lib
INCLUDEPATH += ../..

DEFINES += GCC_POISSON_LIBRARY

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
HEADERS += gcc_poisson_global.h \
           gridcreatingconditioncreatorpoisson.h \
           gridcreatingconditionpoisson.h
SOURCES += gridcreatingconditioncreatorpoisson.cpp \
           gridcreatingconditionpoisson.cpp
RESOURCES += poisson.qrc
TRANSLATIONS += languages/iricGccPoisson_es_ES.ts \
                languages/iricGccPoisson_fr_FR.ts \
                languages/iricGccPoisson_id_ID.ts \
                languages/iricGccPoisson_ja_JP.ts \
                languages/iricGccPoisson_ko_KR.ts \
                languages/iricGccPoisson_ru_RU.ts \
                languages/iricGccPoisson_th_TH.ts \
                languages/iricGccPoisson_vi_VN.ts \
                languages/iricGccPoisson_zh_CN.ts

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

# iricGdRiversurvey

win32 {
	CONFIG(debug, debug|release) {
		LIBS += -L"../../geodata/riversurvey/debug"
	} else {
		LIBS += -L"../../geodata/riversurvey/release"
	}
}
unix {
	LIBS += -L"../../geodata/riversurvey"
}
LIBS += -liricGdRiversurvey

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
           gridcreatingconditionpoisson.h \
           gridcreatingconditionpoissonbuildbanklinesdialog.h \
           gridcreatingconditionpoissonctrlpointsdialog.h \
           gridcreatingconditionpoissongridgeneratedialog.h \
           poissonsolver.h \
           springsolver.h \
           springsolverthread.h \
           private/gridcreatingconditionpoisson_addvertexcommand.h \
           private/gridcreatingconditionpoisson_definenewpointcommand.h \
           private/gridcreatingconditionpoisson_finishdefiningcommand.h \
           private/gridcreatingconditionpoisson_impl.h \
           private/gridcreatingconditionpoisson_movecommand.h \
           private/gridcreatingconditionpoisson_movevertexcommand.h \
           private/gridcreatingconditionpoisson_removevertexcommand.h \
           private/gridcreatingconditionpoisson_updatelabelscommand.h \
           springsolver/edge.h \
           springsolver/grid.h
FORMS += gridcreatingconditionpoissonbuildbanklinesdialog.ui \
         gridcreatingconditionpoissonctrlpointsdialog.ui \
         gridcreatingconditionpoissongridgeneratedialog.ui
SOURCES += gridcreatingconditioncreatorpoisson.cpp \
           gridcreatingconditionpoisson.cpp \
           gridcreatingconditionpoissonbuildbanklinesdialog.cpp \
           gridcreatingconditionpoissonctrlpointsdialog.cpp \
           gridcreatingconditionpoissongridgeneratedialog.cpp \
           poissonsolver.cpp \
           springsolver.cpp \
           springsolverthread.cpp \
           private/gridcreatingconditionpoisson_addvertexcommand.cpp \
           private/gridcreatingconditionpoisson_definenewpointcommand.cpp \
           private/gridcreatingconditionpoisson_finishdefiningcommand.cpp \
           private/gridcreatingconditionpoisson_movecommand.cpp \
           private/gridcreatingconditionpoisson_movevertexcommand.cpp \
           private/gridcreatingconditionpoisson_removevertexcommand.cpp \
           private/gridcreatingconditionpoisson_updatelabelscommand.cpp \
           springsolver/edge.cpp \
           springsolver/grid.cpp
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

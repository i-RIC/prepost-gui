TARGET = iricPython
TEMPLATE = lib
INCLUDEPATH += ..

DEFINES += IRICPYTHON_LIBRARY

QT += gui widgets network

CONFIG(debug, debug|release) {
	QT += xml
} else {

}

include( ../../paths.pri )

# Use Precompiled headers (PCH)
PRECOMPILED_HEADER = iricpython_pch.h

######################
# Internal libraries #
######################

unix {
	LIBS += -L"../gui"
}
LIBS += -liricGui

unix {
	LIBS += -L"../pre"
}
LIBS += -liricPre

unix {
	LIBS += -L"../solverconsole"
}
LIBS += -liricSolverconsole

unix {
	LIBS += -L"../guicore"
}
LIBS += -liricGuicore

unix {
	LIBS += -L"../misc"
}
LIBS += -liricMisc

######################
# External libraries #
######################

LIBS += \
	-lvtkCommonCore-$${VTK_MAJ_MIN} \
	-lvtkCommonDataModel-$${VTK_MAJ_MIN} \
	-lvtkInteractionStyle-$${VTK_MAJ_MIN} \
	-lvtkIOExport-$${VTK_MAJ_MIN} \
	-lvtkRenderingFreeType-$${VTK_MAJ_MIN} \

equals(VTK_MAJOR_VERSION, 6):equals(VTK_MINOR_VERSION, 1) {
    LIBS += \
	    -lvtkRenderingFreeTypeOpenGL-6.1 \
	    -lvtkRenderingOpenGL-6.1
}

win32 {
	DESTDIR = $(SolutionDir)/libdlls/$(Configuration)
	LIBS += -L$(SolutionDir)/libdlls/$(Configuration)
}

# Input
HEADERS += application.h \
           calculationresult.h \
           geodata.h \
           geodatagroup.h \
           geodataimporter.h \
           gridtype.h \
           gridzone.h \
           iricpython_api.h \
           pre.h \
           solver.h \
           private/application_impl.h
SOURCES += application.cpp \
           calculationresult.cpp \
           geodata.cpp \
           geodatagroup.cpp \
           geodataimporter.cpp \
           gridtype.cpp \
           gridzone.cpp \
           pre.cpp \
           solver.cpp

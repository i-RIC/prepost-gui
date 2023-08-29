TEMPLATE = app
TARGET = iRIC
CONFIG += qt
CONFIG += debug_and_release

include( ../../paths.pri )

QT += network widgets xml
RC_FILE = iricgui.rc

# Use Precompiled headers (PCH)
PRECOMPILED_HEADER = iricgui_pch.h

######################
# Internal libraries #
######################

#iricGui library

unix {
	LIBS += -L"../../libs/gui"
}
LIBS += -liricGui

#iricMisc library

unix {
	LIBS += -L"../../libs/misc"
}
LIBS += -liricMisc

######################
# External libraries #
######################

# VTK

LIBS += \
	-lvtkCommonCore-$${VTK_MAJ_MIN} \
	-lvtkCommonDataModel-$${VTK_MAJ_MIN} \
	-lvtkInteractionStyle-$${VTK_MAJ_MIN} \
	-lvtkIOExport-$${VTK_MAJ_MIN} \
	-lvtkRenderingFreeType-$${VTK_MAJ_MIN} \

equals(VTK_MAJOR_VERSION, 6):equals(VTK_MINOR_VERSION, 1) {
    LIBS += \
	    -lvtkRenderingFreeTypeOpenGL-$${VTK_MAJ_MIN} \
	    -lvtkRenderingOpenGL-$${VTK_MAJ_MIN}
}

equals(VTK_MAJOR_VERSION, 8):equals(VTK_MINOR_VERSION, 2) {
    LIBS += \
	    -lvtkIOExportOpenGL2-$${VTK_MAJ_MIN} \
	    -lvtkRenderingGL2PSOpenGL2-$${VTK_MAJ_MIN} \
	    -lvtkRenderingOpenGL2-$${VTK_MAJ_MIN}
}

win32 {
	LIBS += -L$(SolutionDir)/libdlls/$(Configuration)
}

# Input
SOURCES += main/main.cpp

TEMPLATE = app
TARGET = iRICMI
CONFIG += qt
CONFIG += debug_and_release

include( ../../paths.pri )

QT += network widgets xml
RC_FILE = iricmigui.rc


######################
# Internal libraries #
######################

#iricMisc library

unix {
        LIBS += -L"../misc"
}
LIBS += -liricMisc

#iricGui library

unix {
        LIBS += -L"../../libs/migui"
}
LIBS += -liricMiGui

win32 {
        LIBS += -L$(SolutionDir)/libdlls/$(Configuration)
}

SOURCES += main.cpp

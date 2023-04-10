######################################################################
# Automatically generated by qmake (2.01a) ? 10 24 09:50:18 2014
######################################################################

TARGET = iricTriangle
TEMPLATE = lib

DEFINES += TRIANGLE_LIBRARY
DEFINES += TRILIBRARY
DEFINES += ANSI_DECLARATORS

win32 {
	DEFINES += CPU86
	DEFINES += NO_TIMER
}

include( ../../paths.pri )

# Use Precompiled headers (PCH)
PRECOMPILED_HEADER = triangle_pch.h

######################
# Internal libraries #
######################

# iricMisc

unix {
	LIBS += -L"../misc"
}
LIBS += -liricMisc

win32 {
	DESTDIR = $(SolutionDir)/libdlls/$(Configuration)
	LIBS += -L$(SolutionDir)/libdlls/$(Configuration)
}

# Input
HEADERS += triangle.h \
           triangle_global.h \
           triangleexecutethread.h \
           private/triangleexecutethread_impl.h \
           triangleutil.h
SOURCES += triangle.cpp \
           triangleexecutethread.cpp \
           triangleutil.cpp

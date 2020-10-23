TARGET = iricTmsLoader
TEMPLATE = lib
QT       += core gui network webkitwidgets

DEFINES += TMSLOADER_LIBRARY

include( ../../paths.pri )

# Post-Build Event
win32 {
	DESTDIR = $(SolutionDir)\\libdlls\\$(Configuration)
	LIBS += -L$(SolutionDir)\\libdlls\\$(Configuration)
}

# Input
HEADERS += tmsloader.h \
           tmsloader_api.h \
           tmsloadertester.h \
           tmsrequest.h \
           tmsrequestbing.h \
           tmsrequestgooglemap.h \
           tmsrequestgsi.h \
           tmsrequestopenstreetmap.h \
           tmsrequestxyz.h \
           tmsutil.h \
           private/tmsloader_impl.h \
           private/tmsrequest_impl.h \
           private/tmsrequestbing_impl.h \
           private/tmsrequestgooglemap_impl.h \
           private/tmsrequestgsi_impl.h \
           private/tmsrequesthandler.h \
           private/tmsrequesthandlerbing.h \
           private/tmsrequesthandlergooglemap.h \
           private/tmsrequesthandleropenstreetmap.h \
           private/tmsrequesthandlerxyz.h \
           private/tmsrequestxyz_impl.h
SOURCES += tmsloader.cpp \
           tmsloadertester.cpp \
           tmsrequest.cpp \
           tmsrequestbing.cpp \
           tmsrequestgooglemap.cpp \
           tmsrequestgsi.cpp \
           tmsrequestopenstreetmap.cpp \
           tmsrequestxyz.cpp \
           tmsutil.cpp \
           private/tmsrequesthandler.cpp \
           private/tmsrequesthandlerbing.cpp \
           private/tmsrequesthandlergooglemap.cpp \
           private/tmsrequesthandleropenstreetmap.cpp \
           private/tmsrequesthandlerxyz.cpp
RESOURCES += tmsloader.qrc

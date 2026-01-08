TARGET = iricTmsLoader
TEMPLATE = lib
QT       += core gui network
equals(QT_MAJOR_VERSION, 5):equals(QT_MINOR_VERSION, 5) {
    QT += webkitwidgets
}
else {
    QT += webenginewidgets
}

DEFINES += TMSLOADER_LIBRARY

include( ../../paths.pri )

# Use Precompiled headers (PCH)
PRECOMPILED_HEADER = tmsloader_pch.h

######################
# Internal libraries #
######################

# iricCs

unix {
        LIBS += -L"../cs"
}
LIBS += -liricCs

# iricMisc

unix {
        LIBS += -L"../misc"
}
LIBS += -liricMisc

######################
# External libraries #
######################

#gdal

win32 {
        LIBS += -lgdal_i
}
unix {
        LIBS += -lgdal
}

win32 {
	DESTDIR = $(SolutionDir)/libdlls/$(Configuration)
	LIBS += -L$(SolutionDir)/libdlls/$(Configuration)
}

# Input
HEADERS += tmsimagecache.h \
           tmsimagecacheitem.h \
           tmsloader.h \
           tmsloader_api.h \
           tmsloadertester.h \
           tmsrequest.h \
           tmsrequestxyz.h \
           tmsutil.h \
           private/tmsimagecache_entry.h \
           private/tmsimagecache_garbagecollectthread.h \
           private/tmsimagecache_networkaccessmanager.h \
           private/tmsloader_impl.h \
           private/tmsrequest_impl.h \
           private/tmsrequesthandler.h \
           private/tmsrequesthandlerxyz.h \
           private/tmsrequestxyz_impl.h
SOURCES += tmsimagecache.cpp \
           tmsimagecacheitem.cpp \
           tmsloader.cpp \
           tmsloadertester.cpp \
           tmsrequest.cpp \
           tmsrequestxyz.cpp \
           tmsutil.cpp \
           private/tmsimagecache_entry.cpp \
           private/tmsimagecache_garbagecollectthread.cpp \
           private/tmsimagecache_networkaccessmanager.cpp \
           private/tmsrequesthandler.cpp \
           private/tmsrequesthandlerxyz.cpp

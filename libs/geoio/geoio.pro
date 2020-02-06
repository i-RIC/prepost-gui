TARGET = iricGeoio
TEMPLATE = lib
INCLUDEPATH += ..

DEFINES += GEOIO_LIBRARY

QT += widgets

include( ../../paths.pri )

######################
# Internal libraries #
######################

# iricGuibase

win32 {
        CONFIG(debug, debug|release) {
                LIBS += -L"../guibase/debug"
        } else {
                LIBS += -L"../guibase/release"
        }
}
unix {
        LIBS += -L"../guibase"
}
LIBS += -liricGuibase

# iricMisc

win32 {
        CONFIG(debug, debug|release) {
                LIBS += -L"../misc/debug"
        } else {
                LIBS += -L"../misc/release"
        }
}
unix {
        LIBS += -L"../misc"
}
LIBS += -liricMisc

######################
# External libraries #
######################

# shapelib

win32{
	LIBS += -lshapelib_i
}

# geos

LIBS += -lgeos

# Post-Build Event
win32 {
	QMAKE_POST_LINK += copy $(TargetPath) $(SolutionDir)\\libdlls\\$(Configuration)
}

# Input
HEADERS += geoio_global.h \
           io.h \
           polygoncsvexporter.h \
           polygoncsvimporter.h \
           polygonexporterfactory.h \
           polygonexporteri.h \
           polygonimporterfactory.h \
           polygonimporteri.h \
           polygonio.h \
           polygonshapeexporter.h \
           polygonshapeimporter.h \
           polygonutil.h \
           polylinecsvexporter.h \
           polylinecsvimporter.h \
           polylineexporterfactory.h \
           polylineexporteri.h \
           polylineimporterfactory.h \
           polylineimporteri.h \
           polylineio.h \
           polylineshapeexporter.h \
           polylineshapeimporter.h \
           private/csvhelper.h \
           private/dbfitemselectdialog.h \
           private/polygonexporterfactory_impl.h \
           private/polygonimporterfactory_impl.h \
           private/polylineexporterfactory_impl.h \
           private/polylineimporterfactory_impl.h
FORMS += private/dbfitemselectdialog.ui
SOURCES += polygoncsvexporter.cpp \
           polygoncsvimporter.cpp \
           polygonexporterfactory.cpp \
           polygonimporterfactory.cpp \
           polygonio.cpp \
           polygonshapeexporter.cpp \
           polygonshapeimporter.cpp \
           polygonutil.cpp \
           polylinecsvexporter.cpp \
           polylinecsvimporter.cpp \
           polylineexporterfactory.cpp \
           polylineimporterfactory.cpp \
           polylineio.cpp \
           polylineshapeexporter.cpp \
           polylineshapeimporter.cpp \
           private/csvhelper.cpp \
           private/dbfitemselectdialog.cpp
TRANSLATIONS += languages/iricGeoIO_ES.ts \
                languages/iricGeoIO_fr_FR.ts \
                languages/iricGeoIO_id_ID.ts \
                languages/iricGeoIO_ja_JP.ts \
                languages/iricGeoIO_ko_KR.ts \
                languages/iricGeoIO_ru_RU.ts \
                languages/iricGeoIO_th_TH.ts \
                languages/iricGeoIO_vi_VN.ts \
                languages/iricGeoIO_zh_CN.ts

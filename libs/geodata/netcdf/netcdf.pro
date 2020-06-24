TARGET = iricGdNetcdf
TEMPLATE = lib
INCLUDEPATH += ../..

DEFINES += GD_NETCDF_LIBRARY

DEFINES += ANSI_DECLARATORS

include( ../../../paths.pri )

QT += widgets xml

######################
# Internal libraries #
######################

# iricCs

win32 {
	CONFIG(debug, debug|release) {
		LIBS += -L"../../cs/debug"
	} else {
		LIBS += -L"../../cs/release"
	}
}
unix {
	LIBS += -L"../../cs"
}
LIBS += -liricCs

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

# iricTriangle

win32 {
	CONFIG(debug, debug|release) {
		LIBS += -L"../../triangle/debug"
	} else {
		LIBS += -L"../../triangle/release"
	}
}
unix {
	LIBS += -L"../../triangle"
}
LIBS += -liricTriangle

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
	-lvtkCommonCore-6.1 \
	-lvtkCommonDataModel-6.1 \
	-lvtkCommonExecutionModel-6.1 \
	-lvtkRenderingCore-6.1 \
	-lvtkFiltersCore-6.1 \
	-lvtkFiltersExtraction-6.1

# iriclib

LIBS += -liriclib

# netcdf

LIBS += -lnetcdf

#gdal

win32 {
	LIBS += -lgdal_i
}
unix {
	LIBS += -lgdal
}

# udunits

LIBS += -ludunits2

# Post-Build Event
win32 {
	QMAKE_POST_LINK += copy $(TargetPath) $(SolutionDir)\\libdlls\\$(Configuration)
}

# Input
HEADERS += gd_netcdf_global.h \
           geodatanetcdf.h \
           geodatanetcdfcellmappert.h \
           geodatanetcdfcreator.h \
           geodatanetcdfcreatort.h \
           geodatanetcdffilelistdialog.h \
           geodatanetcdffilenamematcher.h \
           geodatanetcdffilenamepatterndialog.h \
           geodatanetcdfgdalimporter.h \
           geodatanetcdfgdalintegerimporter.h \
           geodatanetcdfgdalrealimporter.h \
           geodatanetcdfimporter.h \
           geodatanetcdfimporterdateselectdialog.h \
           geodatanetcdfimportersettingdialog.h \
           geodatanetcdfimportert.h \
           geodatanetcdfinteger.h \
           geodatanetcdfintegercreator.h \
           geodatanetcdfintegerimporter.h \
           geodatanetcdfnodemappert.h \
           geodatanetcdfreal.h \
           geodatanetcdfrealcreator.h \
           geodatanetcdfrealimporter.h \
           geodatanetcdft.h \
           geodatanetcdfxbandimporter.h \
           private/geodatanetcdfcellmappert_detail.h \
           private/geodatanetcdfcreatort_detail.h \
           private/geodatanetcdfimportert_detail.h \
           private/geodatanetcdfnodemappert_detail.h \
           private/geodatanetcdft_detail.h
FORMS += geodatanetcdffilelistdialog.ui \
         geodatanetcdffilenamepatterndialog.ui \
         geodatanetcdfimporterdateselectdialog.ui \
         geodatanetcdfimportersettingdialog.ui
SOURCES += geodatanetcdf.cpp \
           geodatanetcdfcreator.cpp \
           geodatanetcdffilelistdialog.cpp \
           geodatanetcdffilenamematcher.cpp \
           geodatanetcdffilenamepatterndialog.cpp \
           geodatanetcdfgdalimporter.cpp \
           geodatanetcdfgdalintegerimporter.cpp \
           geodatanetcdfgdalrealimporter.cpp \
           geodatanetcdfimporter.cpp \
           geodatanetcdfimporterdateselectdialog.cpp \
           geodatanetcdfimportersettingdialog.cpp \
           geodatanetcdfinteger.cpp \
           geodatanetcdfintegercreator.cpp \
           geodatanetcdfintegerimporter.cpp \
           geodatanetcdfreal.cpp \
           geodatanetcdfrealcreator.cpp \
           geodatanetcdfrealimporter.cpp \
           geodatanetcdfxbandimporter.cpp
TRANSLATIONS += languages/iricGdNetcdf_es_ES.ts \
                languages/iricGdNetcdf_fr_FR.ts \
                languages/iricGdNetcdf_id_ID.ts \
                languages/iricGdNetcdf_ja_JP.ts \
                languages/iricGdNetcdf_ko_KR.ts \
                languages/iricGdNetcdf_ru_RU.ts \
                languages/iricGdNetcdf_th_TH.ts \
                languages/iricGdNetcdf_vi_VN.ts \
                languages/iricGdNetcdf_zh_CN.ts

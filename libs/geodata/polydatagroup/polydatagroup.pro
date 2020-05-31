TARGET = iricGdPolydataGroup
TEMPLATE = lib
INCLUDEPATH += ../..

DEFINES += GD_POLYDATAGROUP_LIBRARY

DEFINES += ANSI_DECLARATORS

include( ../../../paths.pri )

QT += widgets xml

######################
# Internal libraries #
######################

# iricGdPolydata

win32 {
        CONFIG(debug, debug|release) {
                LIBS += -L"../polydata/debug"
        } else {
                LIBS += -L"../polydata/release"
        }
}
unix {
        LIBS += -L"../polydata"
}
LIBS += -liricGdPolydata

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

######################
# External libraries #
######################

# VTK

LIBS += \
	-lvtkCommonCore-6.1 \
	-lvtkCommonDataModel-6.1 \
	-lvtkRenderingCore-6.1

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
HEADERS += gd_polydatagroup_global.h \
           geodatapolydatagroup.h \
           geodatapolydatagroupattributebrowser.h \
           geodatapolydatagroupcolorsettingdialog.h \
           geodatapolydatagroupcreator.h \
           geodatapolydatagroupmergesettingdialog.h \
           geodatapolydatagrouppolydata.h \
           geodatapolydatagrouppolydatawithboundingrect.h \
           geodatapolydatagroupshpimporter.h \
           geodatapolydatagroupshpimportersettingdialog.h \
           private/geodatapolydatagroup_editnameandvaluecommand.h \
           private/geodatapolydatagroup_impl.h \
           private/geodatapolydatagroup_setcolorsettingcommand.h \
           private/geodatapolydatagroup_sortcommand.h \
           private/geodatapolydatagroup_sortedittargetdatacommand.h \
           private/geodatapolydatagroupattributebrowser_tableitemdelegate.h \
           private/geodatapolydatagrouppolydata_impl.h
FORMS += geodatapolydatagroupcolorsettingdialog.ui \
         geodatapolydatagroupmergesettingdialog.ui \
         geodatapolydatagroupshpimportersettingdialog.ui
SOURCES += geodatapolydatagroup.cpp \
           geodatapolydatagroupattributebrowser.cpp \
           geodatapolydatagroupcolorsettingdialog.cpp \
           geodatapolydatagroupcreator.cpp \
           geodatapolydatagroupmergesettingdialog.cpp \
           geodatapolydatagrouppolydata.cpp \
           geodatapolydatagrouppolydatawithboundingrect.cpp \
           geodatapolydatagroupshpimporter.cpp \
           geodatapolydatagroupshpimportersettingdialog.cpp \
           private/geodatapolydatagroup_editnameandvaluecommand.cpp \
           private/geodatapolydatagroup_impl.cpp \
           private/geodatapolydatagroup_setcolorsettingcommand.cpp \
           private/geodatapolydatagroup_sortcommand.cpp \
           private/geodatapolydatagroup_sortedittargetdatacommand.cpp \
           private/geodatapolydatagroupattributebrowser_tableitemdelegate.cpp
TRANSLATIONS += languages/iricGdPolydataGroup_es_ES.ts \
                languages/iricGdPolydataGroup_fr_FR.ts \
                languages/iricGdPolydataGroup_id_ID.ts \
                languages/iricGdPolydataGroup_ja_JP.ts \
                languages/iricGdPolydataGroup_ko_KR.ts \
                languages/iricGdPolydataGroup_ru_RU.ts \
                languages/iricGdPolydataGroup_th_TH.ts \
                languages/iricGdPolydataGroup_vi_VN.ts \
                languages/iricGdPolydataGroup_zh_CN.ts

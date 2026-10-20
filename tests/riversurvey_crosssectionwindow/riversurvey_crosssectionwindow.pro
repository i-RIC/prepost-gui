TEMPLATE = app
TARGET = riversurveyCrosssectionWindowTest
CONFIG += console testcase
CONFIG -= app_bundle

QT += testlib widgets xml network

include( ../../paths.pri )

INCLUDEPATH += ../riversurvey_common

######################
# Internal libraries #
######################

CONFIG(debug, debug|release) {
	LIBS += -L"$$PWD/../../libdlls/Debug"
} else {
	LIBS += -L"$$PWD/../../libdlls/Release"
}

LIBS += \
	-liricGui \
	-liricPre \
	-liricGuicore \
	-liricGuibase \
	-liricMisc \
	-liricGdRiversurvey \
	-liricHdRiversurveywaterelevation

######################
# External libraries #
######################

# VTK (same modules as iRIC GUI)

LIBS += \
	-lvtkCommonCore-$${VTK_MAJ_MIN} \
	-lvtkCommonDataModel-$${VTK_MAJ_MIN} \
	-lvtkInteractionStyle-$${VTK_MAJ_MIN} \
	-lvtkIOExport-$${VTK_MAJ_MIN} \
	-lvtkRenderingFreeType-$${VTK_MAJ_MIN} \
	-lvtkIOExportOpenGL2-$${VTK_MAJ_MIN} \
	-lvtkRenderingGL2PSOpenGL2-$${VTK_MAJ_MIN} \
	-lvtkRenderingOpenGL2-$${VTK_MAJ_MIN}

HEADERS += \
	../riversurvey_common/testsupport.h \
	modaldialogresponder.h

SOURCES += \
	modaldialogresponder.cpp \
	tst_crosssectionwindow.cpp

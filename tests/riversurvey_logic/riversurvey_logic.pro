TEMPLATE = app
TARGET = riversurveyLogicTest
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

LIBS += -liricGdRiversurvey

HEADERS += ../riversurvey_common/testsupport.h

SOURCES += tst_riversurveylogic.cpp

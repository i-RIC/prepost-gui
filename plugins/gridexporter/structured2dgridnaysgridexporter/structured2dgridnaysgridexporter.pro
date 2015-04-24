#-------------------------------------------------
#
# Project created by QtCreator 2014-10-17T11:05:29
#
#-------------------------------------------------

TARGET  = $$qtLibraryTarget(structured2dgridnaysgridexporter)
QT       += widgets xml
TEMPLATE = lib
CONFIG += plugin

include( ../../../paths.pri )

# iricGuicore library

win32 {
	CONFIG(debug, debug|release) {
		LIBS += -L"../../../libs/guicore/debug"
	} else {
		LIBS += -L"../../../libs/guicore/release"
	}
}
unix {
	LIBS += -L"../../../libs/guicore"
}
LIBS += -liricGuicore

# VTK

LIBS += \
	-lvtkCommonCore-6.1

# Input
HEADERS += structured2dgridnaysgridexporter.h
SOURCES += structured2dgridnaysgridexporter.cpp
TRANSLATIONS += languages/structured2dgridnaysgridexporter_es_ES.ts \
                languages/structured2dgridnaysgridexporter_fr_FR.ts \
                languages/structured2dgridnaysgridexporter_id_ID.ts \
                languages/structured2dgridnaysgridexporter_ja_JP.ts \
                languages/structured2dgridnaysgridexporter_ko_KR.ts \
                languages/structured2dgridnaysgridexporter_ru_RU.ts \
                languages/structured2dgridnaysgridexporter_th_TH.ts \
                languages/structured2dgridnaysgridexporter_vi_VN.ts \
                languages/structured2dgridnaysgridexporter_zh_CN.ts

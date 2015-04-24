#-------------------------------------------------
#
# Project created by QtCreator 2014-10-17T11:05:29
#
#-------------------------------------------------

TARGET  = $$qtLibraryTarget(structured2dgridvtkexporter)
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

# iricMisc library

win32 {
	CONFIG(debug, debug|release) {
		LIBS += -L"../../../libs/misc/debug"
	} else {
		LIBS += -L"../../../libs/misc/release"
	}
}
unix {
	LIBS += -L"../../../libs/misc"
}
LIBS += -liricMisc

# VTK

LIBS += \
	-lvtkCommonCore-6.1 \
	-lvtkCommonDataModel-6.1 \
	-lvtkIOCore-6.1 \
	-lvtkIOLegacy-6.1

# Input
HEADERS += structured2dgridvtkexporter.h
SOURCES += structured2dgridvtkexporter.cpp
TRANSLATIONS += languages/structured2dgridvtkexporter_es_ES.ts \
                languages/structured2dgridvtkexporter_fr_FR.ts \
                languages/structured2dgridvtkexporter_id_ID.ts \
                languages/structured2dgridvtkexporter_ja_JP.ts \
                languages/structured2dgridvtkexporter_ko_KR.ts \
                languages/structured2dgridvtkexporter_ru_RU.ts \
                languages/structured2dgridvtkexporter_th_TH.ts \
                languages/structured2dgridvtkexporter_vi_VN.ts \
                languages/structured2dgridvtkexporter_zh_CN.ts

TARGET  = $$qtLibraryTarget(gridlandxmlexporter)
QT       += widgets xml
TEMPLATE = lib
CONFIG += plugin

include( ../../../paths.pri )

# iricGuibase library

win32 {
	CONFIG(debug, debug|release) {
		LIBS += -L"../../../libs/guibase/debug"
	} else {
		LIBS += -L"../../../libs/guibase/release"
	}
}
unix {
	LIBS += -L"../../../libs/guibase"
}
LIBS += -liricGuibase

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
  -lvtkCommonDataModel-6.1

# Input
HEADERS += gridlandxmlexporter.h
SOURCES += gridlandxmlexporter.cpp
TRANSLATIONS += languages/gridlandxmlexporter_es_ES.ts \
                languages/gridlandxmlexporter_fr_FR.ts \
                languages/gridlandxmlexporter_id_ID.ts \
                languages/gridlandxmlexporter_ja_JP.ts \
                languages/gridlandxmlexporter_ko_KR.ts \
                languages/gridlandxmlexporter_ru_RU.ts \
                languages/gridlandxmlexporter_th_TH.ts \
                languages/gridlandxmlexporter_vi_VN.ts \
                languages/gridlandxmlexporter_zh_CN.ts

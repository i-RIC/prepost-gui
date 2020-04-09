TARGET  = $$qtLibraryTarget(structured2dgridgdalimporter)
TEMPLATE = lib
CONFIG += plugin
QT       += widgets xml

include( ../../../paths.pri )

# iricGuicore library

CONFIG(debug, debug|release) {
	LIBS += -L"../../../libs/guicore/debug"
} else {
	LIBS += -L"../../../libs/guicore/release"
}
LIBS += -liricGuicore

# iricMisc library

CONFIG(debug, debug|release) {
	LIBS += -L"../../../libs/misc/debug"
} else {
	LIBS += -L"../../../libs/misc/release"
}
LIBS += -liricMisc

#gdal

win32 {
        LIBS += -lgdal_i
}
unix {
        LIBS += -lgdal
}

# VTK

LIBS += \
	-lvtkCommonCore-6.1

# Input
HEADERS += structured2dgridgdalimporter.h
SOURCES += structured2dgridgdalimporter.cpp
TRANSLATIONS += languages/structured2dgridgdalimporter_es_ES.ts \
                languages/structured2dgridgdalimporter_fr_FR.ts \
                languages/structured2dgridgdalimporter_id_ID.ts \
                languages/structured2dgridgdalimporter_ja_JP.ts \
                languages/structured2dgridgdalimporter_ko_KR.ts \
                languages/structured2dgridgdalimporter_ru_RU.ts \
                languages/structured2dgridgdalimporter_th_TH.ts \
                languages/structured2dgridgdalimporter_vi_VN.ts \
                languages/structured2dgridgdalimporter_zh_CN.ts

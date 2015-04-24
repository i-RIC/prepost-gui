#-------------------------------------------------
#
# Project created by QtCreator 2014-10-17T11:05:29
#
#-------------------------------------------------

TARGET  = $$qtLibraryTarget(unstructured2dgridtriangleimporter)
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

# VTK

LIBS += \
	-lvtkCommonCore-6.1 \
	-lvtkCommonDataModel-6.1

# Input
HEADERS += unstructured2dgridtriangleimporter.h
SOURCES += unstructured2dgridtriangleimporter.cpp
TRANSLATIONS += languages/unstructured2dgridtriangleimporter_es_ES.ts \
                languages/unstructured2dgridtriangleimporter_fr_FR.ts \
                languages/unstructured2dgridtriangleimporter_id_ID.ts \
                languages/unstructured2dgridtriangleimporter_ja_JP.ts \
                languages/unstructured2dgridtriangleimporter_ko_KR.ts \
                languages/unstructured2dgridtriangleimporter_ru_RU.ts \
                languages/unstructured2dgridtriangleimporter_th_TH.ts \
                languages/unstructured2dgridtriangleimporter_vi_VN.ts \
                languages/unstructured2dgridtriangleimporter_zh_CN.ts

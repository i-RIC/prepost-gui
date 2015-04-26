TARGET = iricGraph2dscattered
TEMPLATE = lib
INCLUDEPATH += ../..

DEFINES += GRAPH2DSCATTERED_LIBRARY

include( ../../../paths.pri )

QT += widgets xml

######################
# Internal libraries #
######################

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

# iricPostbase

win32 {
	CONFIG(debug, debug|release) {
		LIBS += -L"../../postbase/debug"
	} else {
		LIBS += -L"../../postbase/release"
	}
}
unix {
	LIBS += -L"../../postbase"
}
LIBS += -liricPostbase

# iricGraph2d

win32 {
	CONFIG(debug, debug|release) {
		LIBS += -L"../graph2d/debug"
	} else {
		LIBS += -L"../graph2d/release"
	}
}
unix {
	LIBS += -L"../graph2d"
}
LIBS += -liricGraph2d

######################
# External libraries #
######################

# Qwt

win32 {
	CONFIG(debug, debug|release) {
		LIBS += -lqwtd
	} else {
		LIBS += -lqwt
		DEFINES += QT_NO_DEBUG_OUTPUT
		DEFINES += QT_NO_WARNING_OUTPUT
	}
}
unix {
	LIBS += -lqwt
	DEFINES += QT_NO_DEBUG_OUTPUT
	DEFINES += QT_NO_WARNING_OUTPUT
}

# VTK

LIBS += \
	-lvtkCommonCore-6.1 \
	-lvtkCommonDataModel-6.1 \
	-lvtkCommonExecutionModel-6.1 \
	-lvtkFiltersExtraction-6.1

# cgnslib
win32 {
	LIBS += -lcgnsdll
}
unix {
	LIBS += -lcgns
}

# Input
HEADERS += graph2dscattered_global.h \
           graph2dscatteredaxissettingdialog.h \
           graph2dscatteredwindow.h \
           graph2dscatteredwindowactionmanager.h \
           graph2dscatteredwindowcontrolwidget.h \
           graph2dscatteredwindowdataitem.h \
           graph2dscatteredwindowdatamodel.h \
           graph2dscatteredwindowdatasourcedialog.h \
           graph2dscatteredwindowdrawsettingdialog.h \
           graph2dscatteredwindowobjectbrowser.h \
           graph2dscatteredwindowobjectbrowserview.h \
           graph2dscatteredwindowprojectdataitem.h \
           graph2dscatteredwindowresultsetting.h \
           graph2dscatteredwindowsettingwidget.h \
           graph2dscatteredwindowview.h \
           datamodel/graph2dscatteredwindowgridresultdataitem.h \
           datamodel/graph2dscatteredwindowresultdataitem.h \
           datamodel/graph2dscatteredwindowresultgroupdataitem.h \
           datamodel/graph2dscatteredwindowrootdataitem.h
FORMS += graph2dscatteredaxissettingdialog.ui \
         graph2dscatteredwindow.ui \
         graph2dscatteredwindowcontrolwidget.ui \
         graph2dscatteredwindowdatasourcedialog.ui \
         graph2dscatteredwindowdrawsettingdialog.ui \
         graph2dscatteredwindowsettingwidget.ui
SOURCES += graph2dscatteredaxissettingdialog.cpp \
           graph2dscatteredwindow.cpp \
           graph2dscatteredwindowactionmanager.cpp \
           graph2dscatteredwindowcontrolwidget.cpp \
           graph2dscatteredwindowdataitem.cpp \
           graph2dscatteredwindowdatamodel.cpp \
           graph2dscatteredwindowdatasourcedialog.cpp \
           graph2dscatteredwindowdrawsettingdialog.cpp \
           graph2dscatteredwindowobjectbrowser.cpp \
           graph2dscatteredwindowobjectbrowserview.cpp \
           graph2dscatteredwindowprojectdataitem.cpp \
           graph2dscatteredwindowresultsetting.cpp \
           graph2dscatteredwindowsettingwidget.cpp \
           graph2dscatteredwindowview.cpp \
           datamodel/graph2dscatteredwindowgridresultdataitem.cpp \
           datamodel/graph2dscatteredwindowresultdataitem.cpp \
           datamodel/graph2dscatteredwindowresultgroupdataitem.cpp \
           datamodel/graph2dscatteredwindowrootdataitem.cpp
TRANSLATIONS += languages/iricGraph2dscattered_es_ES.ts \
                languages/iricGraph2dscattered_fr_FR.ts \
                languages/iricGraph2dscattered_id_ID.ts \
                languages/iricGraph2dscattered_ja_JP.ts \
                languages/iricGraph2dscattered_ko_KR.ts \
                languages/iricGraph2dscattered_ru_RU.ts \
                languages/iricGraph2dscattered_th_TH.ts \
                languages/iricGraph2dscattered_vi_VN.ts \
                languages/iricGraph2dscattered_zh_CN.ts

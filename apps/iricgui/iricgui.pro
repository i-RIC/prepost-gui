######################################################################
# Automatically generated by qmake (2.01a) ? 11 5 22:15:18 2014
######################################################################

TEMPLATE = app
TARGET = iRIC
CONFIG += qt
CONFIG += debug_and_release

include( ../../paths.pri )

QT += xml \
	opengl \
	network

UI_DIR = ./ui
RC_FILE = iricgui.rc

######################
# Internal libraries #
######################

#iricMisc library

win32 {
	CONFIG(debug, debug|release) {
		LIBS += -L"../../libs/misc/debug"
	} else {
		LIBS += -L"../../libs/misc/release"
	}
}
unix {
	LIBS += -L"../../libs/misc"
}
LIBS += -liricMisc

#iricGuibase library

win32 {
	CONFIG(debug, debug|release) {
		LIBS += -L"../../libs/guibase/debug"
	} else {
		LIBS += -L"../../libs/guibase/release"
	}
}
unix {
	LIBS += -L"../../libs/guibase"
}
LIBS += -liricGuibase

#iricGuicore library

win32 {
	CONFIG(debug, debug|release) {
		LIBS += -L"../../libs/guicore/debug"
	} else {
		LIBS += -L"../../libs/guicore/release"
	}
}
unix {
	LIBS += -L"../../libs/guicore"
}
LIBS += -liricGuicore

#iricSolverconsole library

win32 {
	CONFIG(debug, debug|release) {
		LIBS += -L"../../libs/solverconsole/debug"
	} else {
		LIBS += -L"../../libs/solverconsole/release"
	}
}
unix {
	LIBS += -L"../../libs/solverconsole"
}
LIBS += -liricSolverconsole

#iricPre library

win32 {
	CONFIG(debug, debug|release) {
		LIBS += -L"../../libs/pre/debug"
	} else {
		LIBS += -L"../../libs/pre/release"
	}
}
unix {
	LIBS += -L"../../libs/pre"
}
LIBS += -liricPre

#iricPost2d library

win32 {
	CONFIG(debug, debug|release) {
		LIBS += -L"../../libs/post/post2d/debug"
	} else {
		LIBS += -L"../../libs/post/post2d/release"
	}
}
unix {
	LIBS += -L"../../libs/post/post2d"
}
LIBS += -liricPost2d

#iricPost2dbirdeye library

win32 {
	CONFIG(debug, debug|release) {
		LIBS += -L"../../libs/post/post2dbirdeye/debug"
	} else {
		LIBS += -L"../../libs/post/post2dbirdeye/release"
	}
}
unix {
	LIBS += -L"../../libs/post/post2dbirdeye"
}
LIBS += -liricPost2dbirdeye

#iricPost3d library

win32 {
	CONFIG(debug, debug|release) {
		LIBS += -L"../../libs/post/post3d/debug"
	} else {
		LIBS += -L"../../libs/post/post3d/release"
	}
}
unix {
	LIBS += -L"../../libs/post/post3d"
}
LIBS += -liricPost3d

#iricGraph2d library

win32 {
	CONFIG(debug, debug|release) {
		LIBS += -L"../../libs/post/graph2d/debug"
	} else {
		LIBS += -L"../../libs/post/graph2d/release"
	}
}
unix {
	LIBS += -L"../../libs/post/graph2d"
}
LIBS += -liricGraph2d

#iricGraph2dhybrid library

win32 {
	CONFIG(debug, debug|release) {
		LIBS += -L"../../libs/post/graph2dhybrid/debug"
	} else {
		LIBS += -L"../../libs/post/graph2dhybrid/release"
	}
}
unix {
	LIBS += -L"../../libs/post/graph2dhybrid"
}
LIBS += -liricGraph2dhybrid

#iricGraph2dscattered library

win32 {
	CONFIG(debug, debug|release) {
		LIBS += -L"../../libs/post/graph2dscattered/debug"
	} else {
		LIBS += -L"../../libs/post/graph2dscattered/release"
	}
}
unix {
	LIBS += -L"../../libs/post/graph2dscattered"
}
LIBS += -liricGraph2dscattered

#iricGdRiversurvey library

win32 {
	CONFIG(debug, debug|release) {
		LIBS += -L"../../libs/geodata/riversurvey/debug"
	} else {
		LIBS += -L"../../libs/geodata/riversurvey/release"
	}
}
unix {
	LIBS += -L"../../libs/geodata/riversurvey"
}
LIBS += -liricGdRiversurvey

#iricGccexternalprogram library

win32 {
	CONFIG(debug, debug|release) {
		LIBS += -L"../../libs/gridcreatingcondition/externalprogram/debug"
	} else {
		LIBS += -L"../../libs/gridcreatingcondition/externalprogram/release"
	}
}
unix {
	LIBS += -L"../../libs/gridcreatingcondition/externalprogram"
}
LIBS += -liricGccExternalprogram

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
	-lvtkInteractionStyle-6.1 \
	-lvtkIOExport-6.1 \
	-lvtkRenderingFreeType-6.1 \
	-lvtkRenderingFreeTypeOpenGL-6.1 \
	-lvtkRenderingOpenGL-6.1

INCLUDEPATH += ../../libs

# Input
HEADERS += continuoussnapshot/continuoussnapshotfilepropertypage.h \
           continuoussnapshot/continuoussnapshotgoogleearthsettingpage.h \
           continuoussnapshot/continuoussnapshotmoviepropertypage.h \
           continuoussnapshot/continuoussnapshottimestepsettingpage.h \
           continuoussnapshot/continuoussnapshotwindowselectionpage.h \
           continuoussnapshot/continuoussnapshotwizard.h \
           factory/postprocessorwindowfactory.h \
           googlemapimport/googlemapimageimporter.h \
           googlemapimport/googlemapimageimportsettingdialog.h \
           main/iricmainwindow.h \
           main/iricmergablecommands.h \
           misc/animationcontroller.h \
           misc/installertool.h \
           misc/iricmainwindowaboutdialog.h \
           misc/iricmainwindowactionmanager.h \
           misc/iricmainwindowmiscdialogmanager.h \
           misc/iricmetadata.h \
           misc/mousehelpdialog.h \
           misc/networksetting.h \
           misc/newprojectsolverselectingdialog.h \
           misc/projecttypeselectdialog.h \
           pref/preferencedialog.h \
           pref/preferencepage.h \
           pref/preferencepagegeneral.h \
           pref/preferencepagegraphicsdefault.h \
           pref/preferencepagegridcheck.h \
           pref/preferencepagepost2d.h \
           pref/preferencepagepost3d.h \
           pref/preferencepagepre.h \
           pref/preferencepageproxy.h \
           pref/preferencepagesnapshot.h \
           projectproperty/projectpropertybasicinfowidget.h \
           projectproperty/projectpropertydialog.h \
           solverdef/solverdefinitionabstractdialog.h \
           solverdef/solverdefinitionabstractwidget.h \
           solverdef/solverdefinitionlist.h \
           solverdef/solverdefinitionlistdialog.h \
           solverdef/solverdefinitiontranslationchecker.h \
           solverdef/solverdefinitiontranslationupdatewizard.h \
           startpage/startpagedialog.h \
           startpage/startpagehelptabwidget.h \
           startpage/startpagelabel.h \
           startpage/startpagelaunchtabwidget.h \
           startpage/startpagelink.h \
           startpage/startpagelinks.h \
           startpage/startpagerecentproject.h \
           startpage/startpagerecentprojectlist.h \
           startpage/startpagerecentsolver.h \
           startpage/startpagerecentsolverlist.h \
           verification/verificationgraphdialog.h \
           verification/verificationsettingdialog.h
FORMS += continuoussnapshot/continuoussnapshotfilepropertypage.ui \
         continuoussnapshot/continuoussnapshotgoogleearthsettingpage.ui \
         continuoussnapshot/continuoussnapshotmoviepropertypage.ui \
         continuoussnapshot/continuoussnapshottimestepsettingpage.ui \
         continuoussnapshot/continuoussnapshotwindowselectionpage.ui \
         googlemapimport/googlemapimageimportsettingdialog.ui \
         misc/iricmainwindowaboutdialog.ui \
         misc/mousehelpdialog.ui \
         misc/newprojectsolverselectingdialog.ui \
         misc/projecttypeselectdialog.ui \
         pref/preferencedialog.ui \
         pref/preferencepagegeneral.ui \
         pref/preferencepagegraphicsdefault.ui \
         pref/preferencepagegridcheck.ui \
         pref/preferencepagepost2d.ui \
         pref/preferencepagepost3d.ui \
         pref/preferencepagepre.ui \
         pref/preferencepageproxy.ui \
         pref/preferencepagesnapshot.ui \
         projectproperty/projectpropertybasicinfowidget.ui \
         projectproperty/projectpropertydialog.ui \
         solverdef/solverdefinitionabstractdialog.ui \
         solverdef/solverdefinitionabstractwidget.ui \
         solverdef/solverdefinitionlistdialog.ui \
         startpage/startpagedialog.ui \
         startpage/startpagelaunchtabwidget.ui \
         verification/verificationgraphdialog.ui \
         verification/verificationsettingdialog.ui
SOURCES += continuoussnapshot/continuoussnapshotfilepropertypage.cpp \
           continuoussnapshot/continuoussnapshotgoogleearthsettingpage.cpp \
           continuoussnapshot/continuoussnapshotmoviepropertypage.cpp \
           continuoussnapshot/continuoussnapshottimestepsettingpage.cpp \
           continuoussnapshot/continuoussnapshotwindowselectionpage.cpp \
           continuoussnapshot/continuoussnapshotwizard.cpp \
           factory/postprocessorwindowfactory.cpp \
           googlemapimport/googlemapimageimporter.cpp \
           googlemapimport/googlemapimageimportsettingdialog.cpp \
           main/iricmainwindow.cpp \
           main/main.cpp \
           misc/animationcontroller.cpp \
           misc/installertool.cpp \
           misc/iricmainwindowaboutdialog.cpp \
           misc/iricmainwindowactionmanager.cpp \
           misc/iricmainwindowmiscdialogmanager.cpp \
           misc/iricmetadata.cpp \
           misc/mousehelpdialog.cpp \
           misc/networksetting.cpp \
           misc/newprojectsolverselectingdialog.cpp \
           misc/projecttypeselectdialog.cpp \
           pref/preferencedialog.cpp \
           pref/preferencepagegeneral.cpp \
           pref/preferencepagegraphicsdefault.cpp \
           pref/preferencepagegridcheck.cpp \
           pref/preferencepagepost2d.cpp \
           pref/preferencepagepost3d.cpp \
           pref/preferencepagepre.cpp \
           pref/preferencepageproxy.cpp \
           pref/preferencepagesnapshot.cpp \
           projectproperty/projectpropertybasicinfowidget.cpp \
           projectproperty/projectpropertydialog.cpp \
           solverdef/solverdefinitionabstractdialog.cpp \
           solverdef/solverdefinitionabstractwidget.cpp \
           solverdef/solverdefinitionlist.cpp \
           solverdef/solverdefinitionlistdialog.cpp \
           solverdef/solverdefinitiontranslationchecker.cpp \
           solverdef/solverdefinitiontranslationupdatewizard.cpp \
           startpage/startpagedialog.cpp \
           startpage/startpagehelptabwidget.cpp \
           startpage/startpagelabel.cpp \
           startpage/startpagelaunchtabwidget.cpp \
           startpage/startpagelink.cpp \
           startpage/startpagelinks.cpp \
           startpage/startpagerecentproject.cpp \
           startpage/startpagerecentprojectlist.cpp \
           startpage/startpagerecentsolver.cpp \
           startpage/startpagerecentsolverlist.cpp \
           verification/verificationgraphdialog.cpp \
           verification/verificationsettingdialog.cpp
RESOURCES += iricgui.qrc
TRANSLATIONS += languages/iRIC_es_ES.ts \
                languages/iRIC_fr_FR.ts \
                languages/iRIC_id_ID.ts \
                languages/iRIC_ja_JP.ts \
                languages/iRIC_ko_KR.ts \
                languages/iRIC_ru_RU.ts \
                languages/iRIC_th_TH.ts \
                languages/iRIC_vi_VN.ts \
                languages/iRIC_zh_CN.ts

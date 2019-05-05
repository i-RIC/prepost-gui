TARGET = iricGui
TEMPLATE = lib
CONFIG += qt

DEFINES += GUI_LIBRARY

include( ../../paths.pri )

QT += xml \
	opengl \
	network

UI_DIR = ./ui

######################
# Internal libraries #
######################

# iricCs

win32 {
	CONFIG(debug, debug|release) {
		LIBS += -L"../cs/debug"
	} else {
		LIBS += -L"../cs/release"
	}
}
unix {
	LIBS += -L"../cs"
}
LIBS += -liricCs

#iricMisc library

win32 {
	CONFIG(debug, debug|release) {
		LIBS += -L"../misc/debug"
	} else {
		LIBS += -L"../misc/release"
	}
}
unix {
	LIBS += -L"../misc"
}
LIBS += -liricMisc

#iricGuibase library

win32 {
	CONFIG(debug, debug|release) {
		LIBS += -L"../guibase/debug"
	} else {
		LIBS += -L"../guibase/release"
	}
}
unix {
	LIBS += -L"../guibase"
}
LIBS += -liricGuibase

#iricTmsloader library

win32 {
	CONFIG(debug, debug|release) {
		LIBS += -L"../tmsloader/debug"
	} else {
		LIBS += -L"../tmsloader/release"
	}
}
unix {
	LIBS += -L"../tmsloader"
}
LIBS += -liricTmsloader

#iricGuicore library

win32 {
	CONFIG(debug, debug|release) {
		LIBS += -L"../guicore/debug"
	} else {
		LIBS += -L"../guicore/release"
	}
}
unix {
	LIBS += -L"../guicore"
}
LIBS += -liricGuicore

#iricSolverconsole library

win32 {
	CONFIG(debug, debug|release) {
		LIBS += -L"../solverconsole/debug"
	} else {
		LIBS += -L"../solverconsole/release"
	}
}
unix {
	LIBS += -L"../solverconsole"
}
LIBS += -liricSolverconsole

#iricPre library

win32 {
	CONFIG(debug, debug|release) {
		LIBS += -L"../pre/debug"
	} else {
		LIBS += -L"../pre/release"
	}
}
unix {
	LIBS += -L"../pre"
}
LIBS += -liricPre

#iricPost2d library

win32 {
	CONFIG(debug, debug|release) {
		LIBS += -L"../post/post2d/debug"
	} else {
		LIBS += -L"../post/post2d/release"
	}
}
unix {
	LIBS += -L"../post/post2d"
}
LIBS += -liricPost2d

#iricPost2dbirdeye library

win32 {
	CONFIG(debug, debug|release) {
		LIBS += -L"../post/post2dbirdeye/debug"
	} else {
		LIBS += -L"../post/post2dbirdeye/release"
	}
}
unix {
	LIBS += -L"../post/post2dbirdeye"
}
LIBS += -liricPost2dbirdeye

#iricPost3d library

win32 {
	CONFIG(debug, debug|release) {
		LIBS += -L"../post/post3d/debug"
	} else {
		LIBS += -L"../post/post3d/release"
	}
}
unix {
	LIBS += -L"../post/post3d"
}
LIBS += -liricPost3d

#iricGraph2d library

win32 {
	CONFIG(debug, debug|release) {
		LIBS += -L"../post/graph2d/debug"
	} else {
		LIBS += -L"../post/graph2d/release"
	}
}
unix {
	LIBS += -L"../post/graph2d"
}
LIBS += -liricGraph2d

#iricGraph2dhybrid library

win32 {
	CONFIG(debug, debug|release) {
		LIBS += -L"../post/graph2dhybrid/debug"
	} else {
		LIBS += -L"../post/graph2dhybrid/release"
	}
}
unix {
	LIBS += -L"../post/graph2dhybrid"
}
LIBS += -liricGraph2dhybrid

#iricGraph2dscattered library

win32 {
	CONFIG(debug, debug|release) {
		LIBS += -L"../post/graph2dscattered/debug"
	} else {
		LIBS += -L"../post/graph2dscattered/release"
	}
}
unix {
	LIBS += -L"../post/graph2dscattered"
}
LIBS += -liricGraph2dscattered

#iricGdRiversurvey library

win32 {
	CONFIG(debug, debug|release) {
		LIBS += -L"../geodata/riversurvey/debug"
	} else {
		LIBS += -L"../geodata/riversurvey/release"
	}
}
unix {
	LIBS += -L"../geodata/riversurvey"
}
LIBS += -liricGdRiversurvey

#iricGdPointmap library

win32 {
	CONFIG(debug, debug|release) {
		LIBS += -L"../geodata/pointmap/debug"
	} else {
		LIBS += -L"../geodata/pointmap/release"
	}
}
unix {
	LIBS += -L"../geodata/pointmap"
}
LIBS += -liricGdPointmap

#iricGccexternalprogram library

win32 {
	CONFIG(debug, debug|release) {
		LIBS += -L"../gridcreatingcondition/externalprogram/debug"
	} else {
		LIBS += -L"../gridcreatingcondition/externalprogram/release"
	}
}
unix {
	LIBS += -L"../gridcreatingcondition/externalprogram"
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

# Post-Build Event
win32 {
	QMAKE_POST_LINK += copy $(TargetPath) $(SolutionDir)\\libdlls\\$(Configuration)
}

# Input
HEADERS += gui_api.h \
           continuoussnapshot/continuoussnapshotconfirmpage.h \
           continuoussnapshot/continuoussnapshotfilepropertypage.h \
           continuoussnapshot/continuoussnapshotgoogleearthsettingpage.h \
           continuoussnapshot/continuoussnapshotintroductionpage.h \
           continuoussnapshot/continuoussnapshotmoviepropertypage.h \
           continuoussnapshot/continuoussnapshottimestepsettingpage.h \
           continuoussnapshot/continuoussnapshotwindowselectionpage.h \
           continuoussnapshot/continuoussnapshotwizard.h \
           factory/postprocessorwindowfactory.h \
           main/iricmainwindow.h \
           main/iricmergablecommands.h \
           misc/animationcontroller.h \
           misc/flushrequester.h \
           misc/installertool.h \
           misc/iricmainwindowaboutdialog.h \
           misc/iricmainwindowactionmanager.h \
           misc/iricmainwindowmiscdialogmanager.h \
           misc/mousehelpdialog.h \
           misc/newprojectsolverselectingdialog.h \
           misc/periodicalupdatechecker.h \
           misc/projecttypeselectdialog.h \
           misc/wrongsettingexception.h \
           pref/preferencedialog.h \
           pref/preferencepage.h \
           pref/preferencepagebackgroundgrid.h \
           pref/preferencepagegeneral.h \
           pref/preferencepagegraphicsdefault.h \
           pref/preferencepagegridcheck.h \
           pref/preferencepageproxy.h \
           pref/preferencepageriversurvey.h \
           pref/preferencepagetms.h \
           pref/preferencepagewebcsvtile.h \
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
           verification/verificationsettingdialog.h \
           main/private/iricmainwindow_calculatedresultmanager.h \
           main/private/iricmainwindow_snapshotsaver.h \
           misc/private/cancelhandler.h \
           pref/private/preferencepageriversurveyfractionadddialog.h \
           pref/private/preferencepagetmsadddialog.h \
           pref/private/preferencepagewebcsvtileadddialog.h \
           startpage/private/startpagedialog_impl.h \
           startpage/private/startpagelabel_impl.h
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
         pref/preferencepagebackgroundgrid.ui \
         pref/preferencepagegeneral.ui \
         pref/preferencepagegraphicsdefault.ui \
         pref/preferencepagegridcheck.ui \
         pref/preferencepageproxy.ui \
         pref/preferencepageriversurvey.ui \
         pref/preferencepagetms.ui \
         pref/preferencepagewebcsvtile.ui \
         projectproperty/projectpropertybasicinfowidget.ui \
         projectproperty/projectpropertydialog.ui \
         solverdef/solverdefinitionabstractdialog.ui \
         solverdef/solverdefinitionabstractwidget.ui \
         solverdef/solverdefinitionlistdialog.ui \
         startpage/startpagedialog.ui \
         startpage/startpagelaunchtabwidget.ui \
         verification/verificationgraphdialog.ui \
         verification/verificationsettingdialog.ui \
         pref/private/preferencepageriversurveyfractionadddialog.ui \
         pref/private/preferencepagetmsadddialog.ui \
         pref/private/preferencepagewebcsvtileadddialog.ui
SOURCES += continuoussnapshot/continuoussnapshotconfirmpage.cpp \
           continuoussnapshot/continuoussnapshotfilepropertypage.cpp \
           continuoussnapshot/continuoussnapshotgoogleearthsettingpage.cpp \
           continuoussnapshot/continuoussnapshotintroductionpage.cpp \
           continuoussnapshot/continuoussnapshotmoviepropertypage.cpp \
           continuoussnapshot/continuoussnapshottimestepsettingpage.cpp \
           continuoussnapshot/continuoussnapshotwindowselectionpage.cpp \
           continuoussnapshot/continuoussnapshotwizard.cpp \
           factory/postprocessorwindowfactory.cpp \
           main/iricmainwindow.cpp \
           misc/animationcontroller.cpp \
           misc/flushrequester.cpp \
           misc/installertool.cpp \
           misc/iricmainwindowaboutdialog.cpp \
           misc/iricmainwindowactionmanager.cpp \
           misc/iricmainwindowmiscdialogmanager.cpp \
           misc/mousehelpdialog.cpp \
           misc/newprojectsolverselectingdialog.cpp \
           misc/periodicalupdatechecker.cpp \
           misc/projecttypeselectdialog.cpp \
           pref/preferencedialog.cpp \
           pref/preferencepage.cpp \
           pref/preferencepagebackgroundgrid.cpp \
           pref/preferencepagegeneral.cpp \
           pref/preferencepagegraphicsdefault.cpp \
           pref/preferencepagegridcheck.cpp \
           pref/preferencepageproxy.cpp \
           pref/preferencepageriversurvey.cpp \
           pref/preferencepagetms.cpp \
           pref/preferencepagewebcsvtile.cpp \
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
           verification/verificationsettingdialog.cpp \
           main/private/iricmainwindow_calculatedresultmanager.cpp \
           main/private/iricmainwindow_snapshotsaver.cpp \
           misc/private/cancelhandler.cpp \
           pref/private/preferencepageriversurveyfractionadddialog.cpp \
           pref/private/preferencepagetmsadddialog.cpp \
           pref/private/preferencepagewebcsvtileadddialog.cpp
RESOURCES += gui.qrc
TRANSLATIONS += languages/iricGui_es_ES.ts \
                languages/iricGui_fr_FR.ts \
                languages/iricGui_id_ID.ts \
                languages/iricGui_ja_JP.ts \
                languages/iricGui_ko_KR.ts \
                languages/iricGui_ru_RU.ts \
                languages/iricGui_th_TH.ts \
                languages/iricGui_vi_VN.ts \
                languages/iricGui_zh_CN.ts

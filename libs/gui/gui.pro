TARGET = iricGui
TEMPLATE = lib
CONFIG += qt

DEFINES += GUI_LIBRARY

include( ../../paths.pri )

QT += xml \
	opengl \
	network

UI_DIR = ./ui

# Use Precompiled headers (PCH)
PRECOMPILED_HEADER = gui_pch.h

######################
# Internal libraries #
######################

# iricCs

unix {
	LIBS += -L"../cs"
}
LIBS += -liricCs

#iricMisc library

unix {
	LIBS += -L"../misc"
}
LIBS += -liricMisc

#iricGuibase library

unix {
	LIBS += -L"../guibase"
}
LIBS += -liricGuibase

#iricTmsloader library

unix {
	LIBS += -L"../tmsloader"
}
LIBS += -liricTmsloader

#iricGuicore library

unix {
	LIBS += -L"../guicore"
}
LIBS += -liricGuicore

#iricSolverconsole library

unix {
	LIBS += -L"../solverconsole"
}
LIBS += -liricSolverconsole

#iricPre library

unix {
	LIBS += -L"../pre"
}
LIBS += -liricPre

#iricPost2d library

unix {
	LIBS += -L"../post/post2d"
}
LIBS += -liricPost2d

#iricPost2dbirdeye library

unix {
	LIBS += -L"../post/post2dbirdeye"
}
LIBS += -liricPost2dbirdeye

#iricPost3d library

unix {
	LIBS += -L"../post/post3d"
}
LIBS += -liricPost3d

#iricGraph2d library

unix {
	LIBS += -L"../post/graph2d"
}
LIBS += -liricGraph2d

#iricGraph2dhybrid library

unix {
	LIBS += -L"../post/graph2dhybrid"
}
LIBS += -liricGraph2dhybrid

#iricGraph2dscattered library

unix {
	LIBS += -L"../post/graph2dscattered"
}
LIBS += -liricGraph2dscattered

#iricGraph2dVerification library

unix {
	LIBS += -L"../post/graph2dverification"
}
LIBS += -liricGraph2dVerification

#iricPostCrosssection library

unix {
	LIBS += -L"../post/crosssection"
}
LIBS += -liricPostCrosssection

#iricGdRiversurvey library

unix {
	LIBS += -L"../geodata/riversurvey"
}
LIBS += -liricGdRiversurvey

#iricGdPointmap library

unix {
	LIBS += -L"../geodata/pointmap"
}
LIBS += -liricGdPointmap

#iricGccexternalprogram library

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
	-lvtkCommonCore-$${VTK_MAJ_MIN} \
	-lvtkCommonDataModel-$${VTK_MAJ_MIN} \
	-lvtkInteractionStyle-$${VTK_MAJ_MIN} \
	-lvtkIOExport-$${VTK_MAJ_MIN} \
	-lvtkRenderingCore-$${VTK_MAJ_MIN} \
	-lvtkRenderingFreeType-$${VTK_MAJ_MIN} \
	-lvtkRenderingFreeTypeOpenGL-$${VTK_MAJ_MIN} \
	-lvtkRenderingOpenGL-$${VTK_MAJ_MIN}

##equals(VTK_MAJOR_VERSION, 8):equals(VTK_MINOR_VERSION, 2) {
##	LIBS += \
##		-lvtkRenderingFreeTypeOpenGL-$${VTK_MAJ_MIN} \
##		-lvtkRenderingOpenGL-$${VTK_MAJ_MIN}
##}

equals(VTK_MAJOR_VERSION, 8):equals(VTK_MINOR_VERSION, 2) {
	LIBS -= \
		-lvtkRenderingFreeTypeOpenGL-$${VTK_MAJ_MIN} \
		-lvtkRenderingOpenGL-$${VTK_MAJ_MIN}
}

INCLUDEPATH += ../../libs

win32 {
	DESTDIR = $(SolutionDir)/libdlls/$(Configuration)
	LIBS += -L$(SolutionDir)/libdlls/$(Configuration)
}

# Input
HEADERS += gui_api.h \
           continuoussnapshot/continuoussnapshotsetting.h \
           continuoussnapshot/continuoussnapshotwizard.h \
           factory/postprocessorwindowfactory.h \
           main/iricmainwindow.h \
           main/iricmergablecommands.h \
           misc/animationcontroller.h \
           misc/copycamerasettingdialog.h \
           misc/flushrequester.h \
           misc/installertool.h \
           misc/iricmainwindowaboutdialog.h \
           misc/iricmainwindowactionmanager.h \
           misc/iricmainwindowmiscdialogmanager.h \
           misc/mousehelpdialog.h \
           misc/newprojectsolverselectingdialog.h \
           misc/periodicalupdatechecker.h \
           misc/projecttypeselectdialog.h \
           misc/recentprojectsmanager.h \
           misc/recentsolversmanager.h \
           misc/subwindowsaligndialog.h \
           misc/windowaligner.h \
           misc/wrongsettingexception.h \
           pref/preferencedialog.h \
           pref/preferencepage.h \
           pref/preferencepagebackgroundgrid.h \
           pref/preferencepagefontdefault.h \
           pref/preferencepagegeneral.h \
           pref/preferencepagegraphicsdefault.h \
           pref/preferencepagegridcheck.h \
           pref/preferencepageproxy.h \
           pref/preferencepageriversurvey.h \
           pref/preferencepagetextformat.h \
           pref/preferencepagetms.h \
           pref/preferencepagewebcsvtile.h \
           pref/preferencepagewindowsize.h \
           pref/private/preferencepageriversurvey_fractionadddialog.h \
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
           continuoussnapshot/private/continuoussnapshotwizard_confirmpage.h \
           continuoussnapshot/private/continuoussnapshotwizard_filepropertypage.h \
           continuoussnapshot/private/continuoussnapshotwizard_googleearthsettingpage.h \
           continuoussnapshot/private/continuoussnapshotwizard_introductionpage.h \
           continuoussnapshot/private/continuoussnapshotwizard_moviepropertypage.h \
           continuoussnapshot/private/continuoussnapshotwizard_timestepsettingpage.h \
           continuoussnapshot/private/continuoussnapshotwizard_windowselectionpage.h \
           main/private/iricmainwindow_calculatedresultmanager.h \
           main/private/iricmainwindow_modelessdialogmodechanger.h \
           main/private/iricmainwindow_snapshotsaver.h \
           misc/private/cancelhandler.h \
           pref/private/preferencepagetmsadddialog.h \
           pref/private/preferencepagewebcsvtileadddialog.h \
           startpage/private/startpagedialog_impl.h \
           startpage/private/startpagelabel_impl.h
FORMS += googlemapimport/googlemapimageimportsettingdialog.ui \
         misc/copycamerasettingdialog.ui \
         misc/iricmainwindowaboutdialog.ui \
         misc/mousehelpdialog.ui \
         misc/newprojectsolverselectingdialog.ui \
         misc/projecttypeselectdialog.ui \
         misc/subwindowsaligndialog.ui \
         pref/preferencedialog.ui \
         pref/preferencepagebackgroundgrid.ui \
         pref/preferencepagefontdefault.ui \
         pref/preferencepagegeneral.ui \
         pref/preferencepagegraphicsdefault.ui \
         pref/preferencepagegridcheck.ui \
         pref/preferencepageproxy.ui \
         pref/preferencepageriversurvey.ui \
         pref/preferencepagetextformat.ui \
         pref/preferencepagetms.ui \
         pref/preferencepagewebcsvtile.ui \
         pref/preferencepagewindowsize.ui \
         pref/private/preferencepageriversurvey_fractionadddialog.ui \
         projectproperty/projectpropertybasicinfowidget.ui \
         projectproperty/projectpropertydialog.ui \
         solverdef/solverdefinitionabstractdialog.ui \
         solverdef/solverdefinitionabstractwidget.ui \
         solverdef/solverdefinitionlistdialog.ui \
         startpage/startpagedialog.ui \
         startpage/startpagelaunchtabwidget.ui \
         continuoussnapshot/private/continuoussnapshotwizard_filepropertypage.ui \
         continuoussnapshot/private/continuoussnapshotwizard_googleearthsettingpage.ui \
         continuoussnapshot/private/continuoussnapshotwizard_moviepropertypage.ui \
         continuoussnapshot/private/continuoussnapshotwizard_timestepsettingpage.ui \
         continuoussnapshot/private/continuoussnapshotwizard_windowselectionpage.ui \
         pref/private/preferencepagetmsadddialog.ui \
         pref/private/preferencepagewebcsvtileadddialog.ui
SOURCES += continuoussnapshot/continuoussnapshotsetting.cpp \
           continuoussnapshot/continuoussnapshotwizard.cpp \
           factory/postprocessorwindowfactory.cpp \
           main/iricmainwindow.cpp \
           misc/animationcontroller.cpp \
           misc/copycamerasettingdialog.cpp \
           misc/flushrequester.cpp \
           misc/installertool.cpp \
           misc/iricmainwindowaboutdialog.cpp \
           misc/iricmainwindowactionmanager.cpp \
           misc/iricmainwindowmiscdialogmanager.cpp \
           misc/mousehelpdialog.cpp \
           misc/newprojectsolverselectingdialog.cpp \
           misc/periodicalupdatechecker.cpp \
           misc/projecttypeselectdialog.cpp \
           misc/recentprojectsmanager.cpp \
           misc/recentsolversmanager.cpp \
           misc/subwindowsaligndialog.cpp \
           misc/windowaligner.cpp \
           pref/preferencedialog.cpp \
           pref/preferencepage.cpp \
           pref/preferencepagebackgroundgrid.cpp \
           pref/preferencepagefontdefault.cpp \
           pref/preferencepagegeneral.cpp \
           pref/preferencepagegraphicsdefault.cpp \
           pref/preferencepagegridcheck.cpp \
           pref/preferencepageproxy.cpp \
           pref/preferencepageriversurvey.cpp \
           pref/preferencepagetextformat.cpp \
           pref/preferencepagetms.cpp \
           pref/preferencepagewebcsvtile.cpp \
           pref/preferencepagewindowsize.cpp \
           pref/private/preferencepageriversurvey_fractionadddialog.cpp \
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
           continuoussnapshot/private/continuoussnapshotwizard_confirmpage.cpp \
           continuoussnapshot/private/continuoussnapshotwizard_filepropertypage.cpp \
           continuoussnapshot/private/continuoussnapshotwizard_googleearthsettingpage.cpp \
           continuoussnapshot/private/continuoussnapshotwizard_introductionpage.cpp \
           continuoussnapshot/private/continuoussnapshotwizard_moviepropertypage.cpp \
           continuoussnapshot/private/continuoussnapshotwizard_timestepsettingpage.cpp \
           continuoussnapshot/private/continuoussnapshotwizard_windowselectionpage.cpp \
           main/private/iricmainwindow_calculatedresultmanager.cpp \
           main/private/iricmainwindow_modelessdialogmodechanger.cpp \
           main/private/iricmainwindow_snapshotsaver.cpp \
           misc/private/cancelhandler.cpp \
           pref/private/preferencepagetmsadddialog.cpp \
           pref/private/preferencepagewebcsvtileadddialog.cpp
RESOURCES += gui.qrc
TRANSLATIONS += languages/iricGui_ar_EG.ts \
                languages/iricGui_bg_BG.ts \
                languages/iricGui_bs_BA.ts \
                languages/iricGui_ca_ES.ts \
                languages/iricGui_cs_CZ.ts \
                languages/iricGui_da_DK.ts \
                languages/iricGui_de_DE.ts \
                languages/iricGui_el_GR.ts \
                languages/iricGui_es_ES.ts \
                languages/iricGui_et_EE.ts \
                languages/iricGui_eu_ES.ts \
                languages/iricGui_fi_FI.ts \
                languages/iricGui_fr_FR.ts \
                languages/iricGui_gl_ES.ts \
                languages/iricGui_hi_IN.ts \
                languages/iricGui_hu_HU.ts \
                languages/iricGui_id_ID.ts \
                languages/iricGui_is_IS.ts \
                languages/iricGui_it_IT.ts \
                languages/iricGui_ja_JP.ts \
                languages/iricGui_ko_KR.ts \
                languages/iricGui_ky_KG.ts \
                languages/iricGui_lt_LT.ts \
                languages/iricGui_lv_LV.ts \
                languages/iricGui_nb_NO.ts \
                languages/iricGui_nl_NL.ts \
                languages/iricGui_pl_PL.ts \
                languages/iricGui_pt_BR.ts \
                languages/iricGui_pt_PT.ts \
                languages/iricGui_ro_RO.ts \
                languages/iricGui_ru_RU.ts \
                languages/iricGui_sl_SI.ts \
                languages/iricGui_sv_SE.ts \
                languages/iricGui_th_TH.ts \
                languages/iricGui_tr_TR.ts \
                languages/iricGui_uk_UA.ts \
                languages/iricGui_vi_VN.ts \
                languages/iricGui_zh_CN.ts \
                languages/iricGui_zh_TW.ts

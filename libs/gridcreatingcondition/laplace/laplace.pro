TARGET = iricGccLaplace
TEMPLATE = lib
INCLUDEPATH += ../..

DEFINES += GCC_LAPLACE_LIBRARY

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

# iricGeoio

win32 {
	CONFIG(debug, debug|release) {
		LIBS += -L"../../geoio/debug"
	} else {
		LIBS += -L"../../geoio/release"
	}
}
unix {
	LIBS += -L"../../geoio"
}
LIBS += -liricGeoio

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

# iricGdRiversurvey

win32 {
	CONFIG(debug, debug|release) {
		LIBS += -L"../../geodata/riversurvey/debug"
	} else {
		LIBS += -L"../../geodata/riversurvey/release"
	}
}
unix {
	LIBS += -L"../../geodata/riversurvey"
}
LIBS += -liricGdRiversurvey

######################
# External libraries #
######################

# VTK

LIBS += \
	-lvtkCommonComputationalGeometry-6.1 \
	-lvtkCommonCore-6.1 \
	-lvtkCommonDataModel-6.1 \
	-lvtkRenderingCore-6.1 \
	-lvtkRenderingLabel-6.1

# Post-Build Event
win32 {
	QMAKE_POST_LINK += IF NOT EXIST $(SolutionDir)\\libdlls\\$(Configuration) mkdir $(SolutionDir)\\libdlls\\$(Configuration) && copy $(TargetPath) $(SolutionDir)\\libdlls\\$(Configuration)
}

# Input
HEADERS += gcc_laplace_global.h \
           gridcreatingconditioncreatorlaplace.h \
           gridcreatingconditionlaplace.h \
           gridcreatingconditionlaplacebuildbanklinesdialog.h \
           gridcreatingconditionlaplacectrlpointsdialog.h \
           gridcreatingconditionlaplacedeploysettingdialog.h \
           gridcreatingconditionlaplacedivisionsettingdialog.h \
           gridcreatingconditionlaplacesubregiondeploysettingdialog.h \
           gridcreatingconditionlaplacewholeregiondivisionsettingdialog.h \
           private/gridcreatingconditionlaplace_banklinesbuilder.h \
           private/gridcreatingconditionlaplace_buildsubgridpointsthread.h \
           private/gridcreatingconditionlaplace_centerlinecoordinateseditor.h \
           private/gridcreatingconditionlaplace_centerlinefinishdefiningcommand.h \
           private/gridcreatingconditionlaplace_centerlinestartdefinitioncommand.h \
           private/gridcreatingconditionlaplace_centerlineupdatelabelsandsplinecommand.h \
           private/gridcreatingconditionlaplace_commonratiooptimizethread.h \
           private/gridcreatingconditionlaplace_ctrlpointmovecommand.h \
           private/gridcreatingconditionlaplace_deploysettingcommand.h \
           private/gridcreatingconditionlaplace_divisionsettingcommand.h \
           private/gridcreatingconditionlaplace_edgecoordinateseditor.h \
           private/gridcreatingconditionlaplace_edgeeditcoordinatescommand.h \
           private/gridcreatingconditionlaplace_edgesetinterpolationmodecommand.h \
           private/gridcreatingconditionlaplace_impl.h \
           private/gridcreatingconditionlaplace_newedgestartdefinitioncommand.h \
           private/gridcreatingconditionlaplace_polylineaddvertexcommand.h \
           private/gridcreatingconditionlaplace_polylinedefinenewvertexcommand.h \
           private/gridcreatingconditionlaplace_polylineeditcoordinatescommand.h \
           private/gridcreatingconditionlaplace_polylinemovevertexcommand.h \
           private/gridcreatingconditionlaplace_polylineremovevertexcommand.h \
           private/gridcreatingconditionlaplace_subregiondeploysettingcommand.h \
           private/gridcreatingconditionlaplace_updateedgelinesforedgeselectioncommand.h \
           private/gridcreatingconditionlaplace_wholeregiondivisionsettingcommand.h
FORMS += gridcreatingconditionlaplacebuildbanklinesdialog.ui \
         gridcreatingconditionlaplacectrlpointsdialog.ui \
         gridcreatingconditionlaplacedeploysettingdialog.ui \
         gridcreatingconditionlaplacedivisionsettingdialog.ui \
         gridcreatingconditionlaplacesubregiondeploysettingdialog.ui \
         gridcreatingconditionlaplacewholeregiondivisionsettingdialog.ui
SOURCES += gridcreatingconditioncreatorlaplace.cpp \
           gridcreatingconditionlaplace.cpp \
           gridcreatingconditionlaplacebuildbanklinesdialog.cpp \
           gridcreatingconditionlaplacectrlpointsdialog.cpp \
           gridcreatingconditionlaplacedeploysettingdialog.cpp \
           gridcreatingconditionlaplacedivisionsettingdialog.cpp \
           gridcreatingconditionlaplacesubregiondeploysettingdialog.cpp \
           gridcreatingconditionlaplacewholeregiondivisionsettingdialog.cpp \
           private/gridcreatingconditionlaplace_banklinesbuilder.cpp \
           private/gridcreatingconditionlaplace_buildsubgridpointsthread.cpp \
           private/gridcreatingconditionlaplace_centerlinecoordinateseditor.cpp \
           private/gridcreatingconditionlaplace_centerlinefinishdefiningcommand.cpp \
           private/gridcreatingconditionlaplace_centerlinestartdefinitioncommand.cpp \
           private/gridcreatingconditionlaplace_centerlineupdatelabelsandsplinecommand.cpp \
           private/gridcreatingconditionlaplace_commonratiooptimizethread.cpp \
           private/gridcreatingconditionlaplace_ctrlpointmovecommand.cpp \
           private/gridcreatingconditionlaplace_deploysettingcommand.cpp \
           private/gridcreatingconditionlaplace_divisionsettingcommand.cpp \
           private/gridcreatingconditionlaplace_edgecoordinateseditor.cpp \
           private/gridcreatingconditionlaplace_edgeeditcoordinatescommand.cpp \
           private/gridcreatingconditionlaplace_edgesetinterpolationmodecommand.cpp \
           private/gridcreatingconditionlaplace_impl.cpp \
           private/gridcreatingconditionlaplace_newedgestartdefinitioncommand.cpp \
           private/gridcreatingconditionlaplace_polylineaddvertexcommand.cpp \
           private/gridcreatingconditionlaplace_polylinedefinenewvertexcommand.cpp \
           private/gridcreatingconditionlaplace_polylineeditcoordinatescommand.cpp \
           private/gridcreatingconditionlaplace_polylinemovevertexcommand.cpp \
           private/gridcreatingconditionlaplace_polylineremovevertexcommand.cpp \
           private/gridcreatingconditionlaplace_subregiondeploysettingcommand.cpp \
           private/gridcreatingconditionlaplace_updateedgelinesforedgeselectioncommand.cpp \
           private/gridcreatingconditionlaplace_wholeregiondivisionsettingcommand.cpp
RESOURCES += laplace.qrc
TRANSLATIONS += languages/iricGccLaplace_ar_EG.ts \
                languages/iricGccLaplace_bg_BG.ts \
                languages/iricGccLaplace_bs_BA.ts \
                languages/iricGccLaplace_ca_ES.ts \
                languages/iricGccLaplace_cs_CZ.ts \
                languages/iricGccLaplace_da_DK.ts \
                languages/iricGccLaplace_de_DE.ts \
                languages/iricGccLaplace_el_GR.ts \
                languages/iricGccLaplace_es_ES.ts \
                languages/iricGccLaplace_et_EE.ts \
                languages/iricGccLaplace_eu_ES.ts \
                languages/iricGccLaplace_fi_FI.ts \
                languages/iricGccLaplace_fr_FR.ts \
                languages/iricGccLaplace_gl_ES.ts \
                languages/iricGccLaplace_hi_IN.ts \
                languages/iricGccLaplace_hu_HU.ts \
                languages/iricGccLaplace_id_ID.ts \
                languages/iricGccLaplace_is_IS.ts \
                languages/iricGccLaplace_it_IT.ts \
                languages/iricGccLaplace_ja_JP.ts \
                languages/iricGccLaplace_ko_KR.ts \
                languages/iricGccLaplace_ky_KG.ts \
                languages/iricGccLaplace_lt_LT.ts \
                languages/iricGccLaplace_lv_LV.ts \
                languages/iricGccLaplace_nb_NO.ts \
                languages/iricGccLaplace_nl_NL.ts \
                languages/iricGccLaplace_pl_PL.ts \
                languages/iricGccLaplace_pt_BR.ts \
                languages/iricGccLaplace_pt_PT.ts \
                languages/iricGccLaplace_ro_RO.ts \
                languages/iricGccLaplace_ru_RU.ts \
                languages/iricGccLaplace_sl_SI.ts \
                languages/iricGccLaplace_sv_SE.ts \
                languages/iricGccLaplace_th_TH.ts \
                languages/iricGccLaplace_tr_TR.ts \
                languages/iricGccLaplace_uk_UA.ts \
                languages/iricGccLaplace_vi_VN.ts \
                languages/iricGccLaplace_zh_CN.ts \
                languages/iricGccLaplace_zh_TW.ts

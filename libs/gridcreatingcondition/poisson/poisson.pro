TARGET = iricGccPoisson
TEMPLATE = lib
INCLUDEPATH += ../..

DEFINES += GCC_POISSON_LIBRARY

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
	DESTDIR = $(SolutionDir)\\libdlls\\$(Configuration)
	LIBS += -L$(SolutionDir)\\libdlls\\$(Configuration)
}

# Input
HEADERS += gcc_poisson_global.h \
           gridcreatingconditioncreatorpoisson.h \
           gridcreatingconditionpoisson.h \
           gridcreatingconditionpoissonbuildbanklinesdialog.h \
           gridcreatingconditionpoissonctrlpointsdialog.h \
           gridcreatingconditionpoissongridgeneratedialog.h \
           poissonsolver.h \
           springsolver.h \
           springsolverthread.h \
           private/gridcreatingconditionpoisson_addvertexcommand.h \
           private/gridcreatingconditionpoisson_coordinateseditor.h \
           private/gridcreatingconditionpoisson_definenewpointcommand.h \
           private/gridcreatingconditionpoisson_editcoordinatescommand.h \
           private/gridcreatingconditionpoisson_finishdefiningcommand.h \
           private/gridcreatingconditionpoisson_impl.h \
           private/gridcreatingconditionpoisson_movecommand.h \
           private/gridcreatingconditionpoisson_movevertexcommand.h \
           private/gridcreatingconditionpoisson_removevertexcommand.h \
           private/gridcreatingconditionpoisson_updatelabelscommand.h \
           springsolver/edge.h \
           springsolver/grid.h
FORMS += gridcreatingconditionpoissonbuildbanklinesdialog.ui \
         gridcreatingconditionpoissonctrlpointsdialog.ui \
         gridcreatingconditionpoissongridgeneratedialog.ui
SOURCES += gridcreatingconditioncreatorpoisson.cpp \
           gridcreatingconditionpoisson.cpp \
           gridcreatingconditionpoissonbuildbanklinesdialog.cpp \
           gridcreatingconditionpoissonctrlpointsdialog.cpp \
           gridcreatingconditionpoissongridgeneratedialog.cpp \
           poissonsolver.cpp \
           springsolver.cpp \
           springsolverthread.cpp \
           private/gridcreatingconditionpoisson_addvertexcommand.cpp \
           private/gridcreatingconditionpoisson_coordinateseditor.cpp \
           private/gridcreatingconditionpoisson_definenewpointcommand.cpp \
           private/gridcreatingconditionpoisson_editcoordinatescommand.cpp \
           private/gridcreatingconditionpoisson_finishdefiningcommand.cpp \
           private/gridcreatingconditionpoisson_movecommand.cpp \
           private/gridcreatingconditionpoisson_movevertexcommand.cpp \
           private/gridcreatingconditionpoisson_removevertexcommand.cpp \
           private/gridcreatingconditionpoisson_updatelabelscommand.cpp \
           springsolver/edge.cpp \
           springsolver/grid.cpp
RESOURCES += poisson.qrc
TRANSLATIONS += languages/iricGccPoisson_ar_EG.ts \
                languages/iricGccPoisson_bg_BG.ts \
                languages/iricGccPoisson_bs_BA.ts \
                languages/iricGccPoisson_ca_ES.ts \
                languages/iricGccPoisson_cs_CZ.ts \
                languages/iricGccPoisson_da_DK.ts \
                languages/iricGccPoisson_de_DE.ts \
                languages/iricGccPoisson_el_GR.ts \
                languages/iricGccPoisson_es_ES.ts \
                languages/iricGccPoisson_et_EE.ts \
                languages/iricGccPoisson_eu_ES.ts \
                languages/iricGccPoisson_fi_FI.ts \
                languages/iricGccPoisson_fr_FR.ts \
                languages/iricGccPoisson_gl_ES.ts \
                languages/iricGccPoisson_hi_IN.ts \
                languages/iricGccPoisson_hu_HU.ts \
                languages/iricGccPoisson_id_ID.ts \
                languages/iricGccPoisson_is_IS.ts \
                languages/iricGccPoisson_it_IT.ts \
                languages/iricGccPoisson_ja_JP.ts \
                languages/iricGccPoisson_ko_KR.ts \
                languages/iricGccPoisson_ky_KG.ts \
                languages/iricGccPoisson_lt_LT.ts \
                languages/iricGccPoisson_lv_LV.ts \
                languages/iricGccPoisson_nb_NO.ts \
                languages/iricGccPoisson_nl_NL.ts \
                languages/iricGccPoisson_pl_PL.ts \
                languages/iricGccPoisson_pt_BR.ts \
                languages/iricGccPoisson_pt_PT.ts \
                languages/iricGccPoisson_ro_RO.ts \
                languages/iricGccPoisson_ru_RU.ts \
                languages/iricGccPoisson_sl_SI.ts \
                languages/iricGccPoisson_sv_SE.ts \
                languages/iricGccPoisson_th_TH.ts \
                languages/iricGccPoisson_tr_TR.ts \
                languages/iricGccPoisson_uk_UA.ts \
                languages/iricGccPoisson_vi_VN.ts \
                languages/iricGccPoisson_zh_CN.ts \
                languages/iricGccPoisson_zh_TW.ts

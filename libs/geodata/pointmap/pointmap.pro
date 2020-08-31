######################################################################
# Automatically generated by qmake (2.01a) ? 10 23 18:49:25 2014
######################################################################

TARGET = iricGdPointmap
TEMPLATE = lib
INCLUDEPATH += ../..

DEFINES += GD_POINTMAP_LIBRARY

DEFINES += ANSI_DECLARATORS

include( ../../../paths.pri )

QT += network widgets xml

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

# iricCs

win32 {
	CONFIG(debug, debug|release) {
		LIBS += -L"../../cs/debug"
	} else {
		LIBS += -L"../../cs/release"
	}
}
unix {
	LIBS += -L"../../cs"
}
LIBS += -liricCs

# iricTriangle

win32 {
	CONFIG(debug, debug|release) {
		LIBS += -L"../../triangle/debug"
	} else {
		LIBS += -L"../../triangle/release"
	}
}
unix {
	LIBS += -L"../../triangle"
}
LIBS += -liricTriangle

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

######################
# External libraries #
######################

# VTK

LIBS += \
	-lvtkCommonComputationalGeometry-6.1 \
	-lvtkCommonCore-6.1 \
	-lvtkCommonDataModel-6.1 \
	-lvtkCommonExecutionModel-6.1 \
	-lvtkCommonTransforms-6.1 \
	-lvtkFiltersCore-6.1 \
	-lvtkFiltersGeneral-6.1 \
	-lvtkIOCore-6.1 \
	-lvtkIOExport-6.1 \
	-lvtkIOGeometry-6.1 \
	-lvtkIOLegacy-6.1 \
	-lvtkRenderingCore-6.1 \
	-lvtkRenderingLabel-6.1 \
	-lvtkRenderingLOD-6.1

# iriclib

LIBS += -liriclib

# GDAL

win32 {
	LIBS += -lgdal_i
}
unix {
	LIBS += -lgdal
}

# Post-Build Event
win32 {
	QMAKE_POST_LINK += IF NOT EXIST $(SolutionDir)\\libdlls\\$(Configuration) mkdir $(SolutionDir)\\libdlls\\$(Configuration) && copy $(TargetPath) $(SolutionDir)\\libdlls\\$(Configuration)
}

# Input
HEADERS += gd_pointmap_global.h \
           geodatapointmap.h \
           geodatapointmapaddpointdialog.h \
           geodatapointmapbreakline.h \
           geodatapointmapcellmappert.h \
           geodatapointmapdelptsgreaterthandialog.h \
           geodatapointmapdelptslessthandialog.h \
           geodatapointmapeditptsdialog.h \
           geodatapointmapinterpolatepoints.h \
           geodatapointmaplandxmlimporter.h \
           geodatapointmaplandxmlexporter.h \
           geodatapointmapmappingmode.h \
           geodatapointmapnodemappert.h \
           geodatapointmaprealcreator.h \
           geodatapointmaprealexporter.h \
           geodatapointmaprealimporter.h \
           geodatapointmaprealimporterfilterdialog.h \
           geodatapointmaprepresentationdialog.h \
           geodatapointmapstlexporter.h \
           geodatapointmapstlimporter.h \
           geodatapointmapt.h \
           geodatapointmaptemplatemappingsetting.h \
           geodatapointmaptemplatenodemappert.h \
           geodatapointmapvtkexporter.h \
           geodatapointmapwebimporter.h \
           geodatapointmapwebimporterregionselectdialog.h \
           geodatapointmapwebimportersetting.h \
           geodatapointmapwebimportersettingmanager.h \
           geodatapointmapwebimporterzoomleveldialog.h \
           private/geodatapointmap_addinterpolatepointscommand.h \
           private/geodatapointmap_addpointscommand.h \
           private/geodatapointmap_breaklineaddcommand.h \
           private/geodatapointmap_breaklineaddpointcommand.h \
           private/geodatapointmap_breaklinecanceldefinitioncommand.h \
           private/geodatapointmap_breaklinefinishdefinitioncommand.h \
           private/geodatapointmap_deletepointscommand.h \
           private/geodatapointmap_editpointscommand.h \
           private/geodatapointmap_editsinglepointcommand.h \
           private/geodatapointmap_interpolatelineaddpointcommand.h \
           private/geodatapointmap_removetrianglescommand.h \
           private/geodatapointmap_triangleswithlongedgeremover.h \
           private/geodatapointmap_triangleswithlongedgeremover_dialog.h \
           private/geodatapointmapcellmappert_detail.h \
           private/geodatapointmapnodemappert_detail.h \
           private/geodatapointmapt_detail.h \
           private/geodatapointmaptemplatenodemappert_detail.h \
           private/geodatapointmapwebimportersetting_impl.h
FORMS += geodatapointmapaddpointdialog.ui \
         geodatapointmapdelptsgreaterthandialog.ui \
         geodatapointmapdelptslessthandialog.ui \
         geodatapointmapeditptsdialog.ui \
         geodatapointmapinterpolatepoints.ui \
         geodatapointmaprealimporterfilterdialog.ui \
         geodatapointmaprepresentationdialog.ui \
         geodatapointmapwebimporterregionselectdialog.ui \
         geodatapointmapwebimporterzoomleveldialog.ui \
         private/geodatapointmap_triangleswithlongedgeremover_dialog.ui
SOURCES += geodatapointmap.cpp \
           geodatapointmapaddpointdialog.cpp \
           geodatapointmapbreakline.cpp \
           geodatapointmapdelptsgreaterthandialog.cpp \
           geodatapointmapdelptslessthandialog.cpp \
           geodatapointmapeditptsdialog.cpp \
           geodatapointmapinterpolatepoints.cpp \
           geodatapointmaplandxmlimporter.cpp \
           geodatapointmaplandxmlexporter.cpp \
           geodatapointmapmappingmode.cpp \
           geodatapointmaprealcreator.cpp \
           geodatapointmaprealexporter.cpp \
           geodatapointmaprealimporter.cpp \
           geodatapointmaprealimporterfilterdialog.cpp \
           geodatapointmaprepresentationdialog.cpp \
           geodatapointmapstlexporter.cpp \
           geodatapointmapstlimporter.cpp \
           geodatapointmaptemplatemappingsetting.cpp \
           geodatapointmapvtkexporter.cpp \
           geodatapointmapwebimporter.cpp \
           geodatapointmapwebimporterregionselectdialog.cpp \
           geodatapointmapwebimportersetting.cpp \
           geodatapointmapwebimportersettingmanager.cpp \
           geodatapointmapwebimporterzoomleveldialog.cpp \
           private/geodatapointmap_addinterpolatepointscommand.cpp \
           private/geodatapointmap_addpointscommand.cpp \
           private/geodatapointmap_breaklineaddcommand.cpp \
           private/geodatapointmap_breaklineaddpointcommand.cpp \
           private/geodatapointmap_breaklinecanceldefinitioncommand.cpp \
           private/geodatapointmap_breaklinefinishdefinitioncommand.cpp \
           private/geodatapointmap_deletepointscommand.cpp \
           private/geodatapointmap_editpointscommand.cpp \
           private/geodatapointmap_editsinglepointcommand.cpp \
           private/geodatapointmap_interpolatelineaddpointcommand.cpp \
           private/geodatapointmap_removetrianglescommand.cpp \
           private/geodatapointmap_triangleswithlongedgeremover.cpp \
           private/geodatapointmap_triangleswithlongedgeremover_dialog.cpp
RESOURCES += pointmap.qrc
TRANSLATIONS += languages/iricGdPointmap_ar_EG.ts \
                languages/iricGdPointmap_bg_BG.ts \
                languages/iricGdPointmap_bs_BA.ts \
                languages/iricGdPointmap_ca_ES.ts \
                languages/iricGdPointmap_cs_CZ.ts \
                languages/iricGdPointmap_da_DK.ts \
                languages/iricGdPointmap_de_DE.ts \
                languages/iricGdPointmap_el_GR.ts \
                languages/iricGdPointmap_es_ES.ts \
                languages/iricGdPointmap_et_EE.ts \
                languages/iricGdPointmap_eu_ES.ts \
                languages/iricGdPointmap_fi_FI.ts \
                languages/iricGdPointmap_fr_FR.ts \
                languages/iricGdPointmap_gl_ES.ts \
                languages/iricGdPointmap_hi_IN.ts \
                languages/iricGdPointmap_hu_HU.ts \
                languages/iricGdPointmap_id_ID.ts \
                languages/iricGdPointmap_is_IS.ts \
                languages/iricGdPointmap_it_IT.ts \
                languages/iricGdPointmap_ja_JP.ts \
                languages/iricGdPointmap_ko_KR.ts \
                languages/iricGdPointmap_ky_KG.ts \
                languages/iricGdPointmap_lt_LT.ts \
                languages/iricGdPointmap_lv_LV.ts \
                languages/iricGdPointmap_nb_NO.ts \
                languages/iricGdPointmap_nl_NL.ts \
                languages/iricGdPointmap_pl_PL.ts \
                languages/iricGdPointmap_pt_BR.ts \
                languages/iricGdPointmap_pt_PT.ts \
                languages/iricGdPointmap_ro_RO.ts \
                languages/iricGdPointmap_ru_RU.ts \
                languages/iricGdPointmap_sl_SI.ts \
                languages/iricGdPointmap_sv_SE.ts \
                languages/iricGdPointmap_th_TH.ts \
                languages/iricGdPointmap_tr_TR.ts \
                languages/iricGdPointmap_uk_UA.ts \
                languages/iricGdPointmap_vi_VN.ts \
                languages/iricGdPointmap_zh_CN.ts \
                languages/iricGdPointmap_zh_TW.ts

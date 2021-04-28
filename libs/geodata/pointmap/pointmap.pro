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

unix {
	LIBS += -L"../../misc"
}
LIBS += -liricMisc

# iricCs

unix {
	LIBS += -L"../../cs"
}
LIBS += -liricCs

# iricTriangle

unix {
	LIBS += -L"../../triangle"
}
LIBS += -liricTriangle

# iricGuibase

unix {
	LIBS += -L"../../guibase"
}
LIBS += -liricGuibase

# iricGuicore

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

# geos

LIBS += -lgeos

# iriclib

LIBS += -liriclib

# GDAL

win32 {
	LIBS += -lgdal_i
}
unix {
	LIBS += -lgdal
}

win32 {
	DESTDIR = $(SolutionDir)/libdlls/$(Configuration)
	LIBS += -L$(SolutionDir)/libdlls/$(Configuration)
}

# Input
HEADERS += gd_pointmap_global.h \
           geodatapointmap.h \
           geodatapointmapbreakline.h \
           geodatapointmapcellmappert.h \
           geodatapointmapdelptsgreaterthandialog.h \
           geodatapointmapdelptslessthandialog.h \
           geodatapointmapeditptsdialog.h \
           geodatapointmapinterpolatepointsdialog.h \
           geodatapointmaplandxmlexporter.h \
           geodatapointmaplandxmlimporter.h \
           geodatapointmapmappingmode.h \
           geodatapointmapmergesettingdialog.h \
           geodatapointmapnodemappert.h \
           geodatapointmaprealbuilder.h \
           geodatapointmaprealcreator.h \
           geodatapointmaprealexporter.h \
           geodatapointmaprealimporter.h \
           geodatapointmaprealimporterfilterdialog.h \
           geodatapointmaprealtextimporter.h \
           geodatapointmaprepresentationdialog.h \
           geodatapointmapstlexporter.h \
           geodatapointmapstlimporter.h \
           geodatapointmapt.h \
           geodatapointmaptemplatemappingsetting.h \
           geodatapointmaptemplatenodemappert.h \
           geodatapointmaptincreator.h \
           geodatapointmapvtkexporter.h \
           geodatapointmapwebimporter.h \
           geodatapointmapwebimporterregionselectdialog.h \
           geodatapointmapwebimportersetting.h \
           geodatapointmapwebimportersettingmanager.h \
           geodatapointmapwebimporterzoomleveldialog.h \
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
           private/geodatapointmaprealbuilder_impl.h \
           private/geodatapointmaprealtextimporter_lineparser.h \
           private/geodatapointmaprealtextimporter_settingdialog.h \
           private/geodatapointmaprealtextimporter_values.h \
           private/geodatapointmapt_detail.h \
           private/geodatapointmaptemplatenodemappert_detail.h \
           private/geodatapointmapwebimportersetting_impl.h
FORMS += geodatapointmapdelptsgreaterthandialog.ui \
         geodatapointmapdelptslessthandialog.ui \
         geodatapointmapeditptsdialog.ui \
         geodatapointmapinterpolatepointsdialog.ui \
         geodatapointmapmergesettingdialog.ui \
         geodatapointmaprealimporterfilterdialog.ui \
         geodatapointmaprepresentationdialog.ui \
         geodatapointmapwebimporterregionselectdialog.ui \
         geodatapointmapwebimporterzoomleveldialog.ui \
         private/geodatapointmap_triangleswithlongedgeremover_dialog.ui \
         private/geodatapointmaprealtextimporter_settingdialog.ui
SOURCES += geodatapointmap.cpp \
           geodatapointmapbreakline.cpp \
           geodatapointmapdelptsgreaterthandialog.cpp \
           geodatapointmapdelptslessthandialog.cpp \
           geodatapointmapeditptsdialog.cpp \
           geodatapointmapinterpolatepointsdialog.cpp \
           geodatapointmaplandxmlexporter.cpp \
           geodatapointmaplandxmlimporter.cpp \
           geodatapointmapmappingmode.cpp \
           geodatapointmapmergesettingdialog.cpp \
           geodatapointmaprealbuilder.cpp \
           geodatapointmaprealcreator.cpp \
           geodatapointmaprealexporter.cpp \
           geodatapointmaprealimporter.cpp \
           geodatapointmaprealimporterfilterdialog.cpp \
           geodatapointmaprealtextimporter.cpp \
           geodatapointmaprepresentationdialog.cpp \
           geodatapointmapstlexporter.cpp \
           geodatapointmapstlimporter.cpp \
           geodatapointmaptemplatemappingsetting.cpp \
           geodatapointmaptincreator.cpp \
           geodatapointmapvtkexporter.cpp \
           geodatapointmapwebimporter.cpp \
           geodatapointmapwebimporterregionselectdialog.cpp \
           geodatapointmapwebimportersetting.cpp \
           geodatapointmapwebimportersettingmanager.cpp \
           geodatapointmapwebimporterzoomleveldialog.cpp \
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
           private/geodatapointmap_triangleswithlongedgeremover_dialog.cpp \
           private/geodatapointmaprealtextimporter_lineparser.cpp \
           private/geodatapointmaprealtextimporter_settingdialog.cpp
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

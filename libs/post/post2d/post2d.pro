TARGET = iricPost2d
TEMPLATE = lib
INCLUDEPATH += ../..

DEFINES += POST2D_LIBRARY

include( ../../../paths.pri )

QT += widgets xml

# Use Precompiled headers (PCH)
PRECOMPILED_HEADER = post2d_pch.h

######################
# Internal libraries #
######################

# iricAxis2d

unix {
	LIBS += -L"../../dataitem/axis2d"
}
LIBS += -liricAxis2d

# iricDistancemeasure

unix {
	LIBS += -L"../../distancemeasure/axis2d"
}
LIBS += -liricDistancemeasure

# iricMeasureddata

unix {
	LIBS += -L"../../measureddata/axis2d"
}
LIBS += -liricMeasureddata

# iricCs

unix {
	LIBS += -L"../../cs"
}
LIBS += -liricCs

# iricMisc

unix {
	LIBS += -L"../../misc"
}
LIBS += -liricMisc

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

# iricPostbase

unix {
	LIBS += -L"../../postbase"
}
LIBS += -liricPostbase

######################
# External libraries #
######################

# Shapelib

win32{
	LIBS += -lshapelib_i
}

#gdal

win32 {
	LIBS += -lgdal_i
}
unix {
	LIBS += -lgdal
}

#geos

LIBS += -lgeos

# VTK

LIBS += \
	-lvtkCommonCore-$${VTK_MAJ_MIN} \
	-lvtkCommonDataModel-$${VTK_MAJ_MIN} \
	-lvtkCommonExecutionModel-$${VTK_MAJ_MIN} \
	-lvtkCommonMath-$${VTK_MAJ_MIN} \
	-lvtkCommonMisc-$${VTK_MAJ_MIN} \
	-lvtkCommonTransforms-$${VTK_MAJ_MIN} \
	-lvtkFiltersCore-$${VTK_MAJ_MIN} \
	-lvtkFiltersGeometry-$${VTK_MAJ_MIN} \
	-lvtkFiltersGeneral-$${VTK_MAJ_MIN} \
	-lvtkFiltersExtraction-$${VTK_MAJ_MIN} \
	-lvtkFiltersFlowPaths-$${VTK_MAJ_MIN} \
	-lvtkFiltersSources-$${VTK_MAJ_MIN} \
	-lvtkFiltersTexture-$${VTK_MAJ_MIN} \
	-lvtkGUISupportQt-$${VTK_MAJ_MIN} \
	-lvtkInteractionWidgets-$${VTK_MAJ_MIN} \
	-lvtkIOImage-$${VTK_MAJ_MIN} \
	-lvtkIOCore-$${VTK_MAJ_MIN} \
	-lvtkIOLegacy-$${VTK_MAJ_MIN} \
	-lvtkRenderingAnnotation-$${VTK_MAJ_MIN} \
	-lvtkRenderingCore-$${VTK_MAJ_MIN} \
	-lvtkRenderingFreeType-$${VTK_MAJ_MIN} \
	-lvtkRenderingLabel-$${VTK_MAJ_MIN} \
	-lvtkRenderingLOD-$${VTK_MAJ_MIN}

win32 {
	DESTDIR = $(SolutionDir)/libdlls/$(Configuration)
	LIBS += -L$(SolutionDir)/libdlls/$(Configuration)
}

# Input
HEADERS += post2d_global.h \
           datamodel/private/post2dwindowcellscalargroupdataitem_impl.h \
           datamodel/private/post2dwindowcellscalargroupdataitem_setting.h \
           datamodel/private/post2dwindowcellscalargroupdataitem_settingeditwidget.h \
           datamodel/private/post2dwindowgridshapedataitem_settingeditwidget.h \
           datamodel/private/post2dwindownodescalargroupdataitem_impl.h \
           datamodel/private/post2dwindownodescalargroupdataitem_setting.h \
           datamodel/private/post2dwindownodescalargroupdataitem_settingeditwidget.h \
           datamodel/private/post2dwindowparticlesbasescalargroupdataitem_settingeditwidget.h \
           datamodel/private/post2dwindowparticlesbasevectorgroupdataitem_settingeditwidget.h \
           post2dgridregionselectdialog.h \
           post2dobjectbrowser.h \
           post2dobjectbrowserview.h \
           post2dpropertybrowser.h \
           post2dwindow.h \
           post2dwindowactionmanager.h \
           post2dwindowdataitem.h \
           post2dwindowdatamodel.h \
           post2dwindowgraphicsview.h \
           post2dwindowprojectdataitem.h \
           scalarbari.h \
           datamodel/post2dwindowbackgroundimagedataitem.h \
           datamodel/post2dwindowbackgroundimagesdataitem.h \
           datamodel/post2dwindowcellflagdataitem.h \
           datamodel/post2dwindowcellflaggroupdataitem.h \
           datamodel/post2dwindowcellflagsettingdialog.h \
           datamodel/post2dwindowcellscalargroupdataitem.h \
           datamodel/post2dwindowcellscalargrouptopdataitem.h \
           datamodel/post2dwindowedgeiscalargrouptopdataitem.h \
           datamodel/post2dwindowedgejscalargrouptopdataitem.h \
           datamodel/post2dwindowgeodatadataitem.h \
           datamodel/post2dwindowgeodatagroupdataitem.h \
           datamodel/post2dwindowgeodatatopdataitem.h \
           datamodel/post2dwindowgraphdataitem.h \
           datamodel/post2dwindowgraphgroupdataitem.h \
           datamodel/post2dwindowgraphsetting.h \
           datamodel/post2dwindowgraphsettingdialog.h \
           datamodel/post2dwindowgridshapedataitem.h \
           datamodel/post2dwindowgridtypedataitem.h \
           datamodel/post2dwindowmeasureddatatopdataitem.h \
           datamodel/post2dwindownodescalargroupdataitem.h \
           datamodel/post2dwindownodescalargrouptopdataitem.h \
           datamodel/post2dwindownodevectorarrowdataitem.h \
           datamodel/post2dwindownodevectorarrowgroupdataitem.h \
           datamodel/post2dwindownodevectorarrowgroupstructureddataitem.h \
           datamodel/post2dwindownodevectorarrowgroupunstructureddataitem.h \
           datamodel/post2dwindownodevectorparticledataitem.h \
           datamodel/post2dwindownodevectorparticlegroupdataitem.h \
           datamodel/post2dwindownodevectorparticlegroupstructureddataitem.h \
           datamodel/post2dwindownodevectorparticlegroupunstructureddataitem.h \
           datamodel/post2dwindownodevectorstreamlinedataitem.h \
           datamodel/post2dwindownodevectorstreamlinegroupdataitem.h \
           datamodel/post2dwindownodevectorstreamlinegroupstructureddataitem.h \
           datamodel/post2dwindownodevectorstreamlinegroupunstructureddataitem.h \
           datamodel/post2dwindowparticlegrouprootdataitem.h \
           datamodel/post2dwindowparticlegrouptopdataitem.h \
           datamodel/post2dwindowparticlesbasescalardataitem.h \
           datamodel/post2dwindowparticlesbasescalargroupdataitem.h \
           datamodel/post2dwindowparticlesbasetopdataitem.h \
           datamodel/post2dwindowparticlesbasevectordataitem.h \
           datamodel/post2dwindowparticlesbasevectorgroupdataitem.h \
           datamodel/post2dwindowparticlestopdataitem.h \
           datamodel/post2dwindowpointscalargrouptopdataitemi.h \
           datamodel/post2dwindowpolydatagroupdataitem.h \
           datamodel/post2dwindowpolydatatopdataitem.h \
           datamodel/post2dwindowpolydatavaluedataitem.h \
           datamodel/post2dwindowrootdataitem.h \
           datamodel/post2dwindowscalargrouptopdataitem.h \
           datamodel/post2dwindowzonedataitem.h \
           datamodel/private/post2dwindowcellflaggroupdataitem_setsettingcommand.h \
           datamodel/private/post2dwindowgeodatagroupdataitem_scalarstocolorseditdialog.h \
           datamodel/private/post2dwindowgraphgroupdataitem_impl.h \
           datamodel/private/post2dwindowgraphgroupdataitem_setsettingcommand.h \
           datamodel/private/post2dwindowgraphsettingcustomregiondialog.h \
           datamodel/private/post2dwindowgraphsettingdialog_impl.h \
           datamodel/private/post2dwindowgridtypedataitem_applycolormapsettingcommand.h \
           datamodel/private/post2dwindowgridtypedataitem_applycolormapsettingdialog.h \
           datamodel/private/post2dwindowgridtypedataitem_precolormapsettingupdatehandler.h \
           datamodel/private/post2dwindowgridtypedataitem_toolbarwidgetcontroller.h \
           datamodel/private/post2dwindownodescalargroupdataitem_shapeexporter.h \
           datamodel/private/post2dwindownodevectorarrowgroupdataitem_updateactorsettingcommand.h \
           datamodel/private/post2dwindownodevectorarrowgroupstructureddataitem_propertydialog.h \
           datamodel/private/post2dwindownodevectorarrowgroupstructureddataitem_propertydialog_additionalwidgets.h \
           datamodel/private/post2dwindownodevectorarrowgroupunstructureddataitem_propertydialog.h \
           datamodel/private/post2dwindownodevectorarrowgroupunstructureddataitem_propertydialog_additionalwidgets.h \
           datamodel/private/post2dwindownodevectorparticlegroupstructureddataitem_impl.h \
           datamodel/private/post2dwindownodevectorparticlegroupstructureddataitem_setting.h \
           datamodel/private/post2dwindownodevectorparticlegroupstructureddataitem_settingeditwidget.h \
           datamodel/private/post2dwindownodevectorparticlegroupstructureddataitem_settingeditwidget_startpositioneditwidget.h \
           datamodel/private/post2dwindownodevectorparticlegroupunstructureddataitem_impl.h \
           datamodel/private/post2dwindownodevectorparticlegroupunstructureddataitem_setting.h \
           datamodel/private/post2dwindownodevectorparticlegroupunstructureddataitem_settingeditwidget.h \
           datamodel/private/post2dwindownodevectorparticlegroupunstructureddataitem_settingeditwidget_startpositioneditwidget.h \
           datamodel/private/post2dwindownodevectorstreamlinegroupstructureddataitem_impl.h \
           datamodel/private/post2dwindownodevectorstreamlinegroupstructureddataitem_setting.h \
           datamodel/private/post2dwindownodevectorstreamlinegroupstructureddataitem_settingeditwidget.h \
           datamodel/private/post2dwindownodevectorstreamlinegroupstructureddataitem_settingeditwidget_startpositioneditwidget.h \
           datamodel/private/post2dwindownodevectorstreamlinegroupunstructureddataitem_impl.h \
           datamodel/private/post2dwindownodevectorstreamlinegroupunstructureddataitem_setting.h \
           datamodel/private/post2dwindownodevectorstreamlinegroupunstructureddataitem_settingeditwidget.h \
           datamodel/private/post2dwindownodevectorstreamlinegroupunstructureddataitem_settingeditwidget_startpositioneditwidget.h \
           datamodel/private/post2dwindowparticlesbasescalargroupdataitem_toolbarwidget.h \
           datamodel/private/post2dwindowparticlesbasescalargroupdataitem_toolbarwidgetcontroller.h \
           datamodel/private/post2dwindowparticlesbasevectorgroupdataitem_updatesettingcommand.h \
           datamodel/private/post2dwindowpolydatagroupdataitem_propertydialog.h \
           datamodel/private/post2dwindowpolydatagroupdataitem_updateactorsettingscommand.h
FORMS += post2dgridregionselectdialog.ui \
         datamodel/post2dwindowcellflagsettingdialog.ui \
         datamodel/post2dwindowgraphsettingdialog.ui \
         datamodel/private/post2dwindowcellscalargroupdataitem_settingeditwidget.ui \
         datamodel/private/post2dwindowgeodatagroupdataitem_scalarstocolorseditdialog.ui \
         datamodel/private/post2dwindowgraphsettingcustomregiondialog.ui \
         datamodel/private/post2dwindowgridshapedataitem_settingeditwidget.ui \
         datamodel/private/post2dwindownodescalargroupdataitem_settingeditwidget.ui \
         datamodel/private/post2dwindownodevectorarrowgroupstructureddataitem_propertydialog.ui \
         datamodel/private/post2dwindownodevectorarrowgroupstructureddataitem_propertydialog_additionalwidgets.ui \
         datamodel/private/post2dwindownodevectorarrowgroupunstructureddataitem_propertydialog.ui \
         datamodel/private/post2dwindownodevectorarrowgroupunstructureddataitem_propertydialog_additionalwidgets.ui \
         datamodel/private/post2dwindownodevectorparticlegroupstructureddataitem_settingeditwidget.ui \
         datamodel/private/post2dwindownodevectorparticlegroupstructureddataitem_settingeditwidget_startpositioneditwidget.ui \
         datamodel/private/post2dwindownodevectorparticlegroupunstructureddataitem_settingeditwidget.ui \
         datamodel/private/post2dwindownodevectorparticlegroupunstructureddataitem_settingeditwidget_startpositioneditwidget.ui \
         datamodel/private/post2dwindownodevectorstreamlinegroupstructureddataitem_settingeditwidget.ui \
         datamodel/private/post2dwindownodevectorstreamlinegroupstructureddataitem_settingeditwidget_startpositioneditwidget.ui \
         datamodel/private/post2dwindownodevectorstreamlinegroupunstructureddataitem_settingeditwidget.ui \
         datamodel/private/post2dwindownodevectorstreamlinegroupunstructureddataitem_settingeditwidget_startpositioneditwidget.ui \
         datamodel/private/post2dwindowparticlesbasescalargroupdataitem_settingeditwidget.ui \
         datamodel/private/post2dwindowparticlesbasescalargroupdataitem_toolbarwidget.ui \
         datamodel/private/post2dwindowparticlesbasevectorgroupdataitem_settingeditwidget.ui \
         datamodel/private/post2dwindowpolydatagroupdataitem_propertydialog.ui
SOURCES += post2dgridregionselectdialog.cpp \
           datamodel/private/post2dwindowcellscalargroupdataitem_impl.cpp \
           datamodel/private/post2dwindowcellscalargroupdataitem_setting.cpp \
           datamodel/private/post2dwindowcellscalargroupdataitem_settingeditwidget.cpp \
           datamodel/private/post2dwindowgridshapedataitem_settingeditwidget.cpp \
           datamodel/private/post2dwindownodescalargroupdataitem_impl.cpp \
           datamodel/private/post2dwindownodescalargroupdataitem_setting.cpp \
           datamodel/private/post2dwindownodescalargroupdataitem_settingeditwidget.cpp \
           datamodel/private/post2dwindowparticlesbasescalargroupdataitem_settingeditwidget.cpp \
           datamodel/private/post2dwindowparticlesbasevectorgroupdataitem_settingeditwidget.cpp \
           post2dobjectbrowser.cpp \
           post2dobjectbrowserview.cpp \
           post2dpropertybrowser.cpp \
           post2dwindow.cpp \
           post2dwindowactionmanager.cpp \
           post2dwindowdataitem.cpp \
           post2dwindowdatamodel.cpp \
           post2dwindowgraphicsview.cpp \
           post2dwindowprojectdataitem.cpp \
           datamodel/post2dwindowbackgroundimagedataitem.cpp \
           datamodel/post2dwindowbackgroundimagesdataitem.cpp \
           datamodel/post2dwindowcellflagdataitem.cpp \
           datamodel/post2dwindowcellflaggroupdataitem.cpp \
           datamodel/post2dwindowcellflagsettingdialog.cpp \
           datamodel/post2dwindowcellscalargroupdataitem.cpp \
           datamodel/post2dwindowcellscalargrouptopdataitem.cpp \
           datamodel/post2dwindowedgeiscalargrouptopdataitem.cpp \
           datamodel/post2dwindowedgejscalargrouptopdataitem.cpp \
           datamodel/post2dwindowgeodatadataitem.cpp \
           datamodel/post2dwindowgeodatagroupdataitem.cpp \
           datamodel/post2dwindowgeodatatopdataitem.cpp \
           datamodel/post2dwindowgraphdataitem.cpp \
           datamodel/post2dwindowgraphgroupdataitem.cpp \
           datamodel/post2dwindowgraphsetting.cpp \
           datamodel/post2dwindowgraphsettingdialog.cpp \
           datamodel/post2dwindowgridshapedataitem.cpp \
           datamodel/post2dwindowgridtypedataitem.cpp \
           datamodel/post2dwindowmeasureddatatopdataitem.cpp \
           datamodel/post2dwindownodescalargroupdataitem.cpp \
           datamodel/post2dwindownodescalargrouptopdataitem.cpp \
           datamodel/post2dwindownodevectorarrowdataitem.cpp \
           datamodel/post2dwindownodevectorarrowgroupdataitem.cpp \
           datamodel/post2dwindownodevectorarrowgroupstructureddataitem.cpp \
           datamodel/post2dwindownodevectorarrowgroupunstructureddataitem.cpp \
           datamodel/post2dwindownodevectorparticledataitem.cpp \
           datamodel/post2dwindownodevectorparticlegroupdataitem.cpp \
           datamodel/post2dwindownodevectorparticlegroupstructureddataitem.cpp \
           datamodel/post2dwindownodevectorparticlegroupunstructureddataitem.cpp \
           datamodel/post2dwindownodevectorstreamlinedataitem.cpp \
           datamodel/post2dwindownodevectorstreamlinegroupdataitem.cpp \
           datamodel/post2dwindownodevectorstreamlinegroupstructureddataitem.cpp \
           datamodel/post2dwindownodevectorstreamlinegroupunstructureddataitem.cpp \
           datamodel/post2dwindowparticlegrouprootdataitem.cpp \
           datamodel/post2dwindowparticlegrouptopdataitem.cpp \
           datamodel/post2dwindowparticlesbasescalardataitem.cpp \
           datamodel/post2dwindowparticlesbasescalargroupdataitem.cpp \
           datamodel/post2dwindowparticlesbasetopdataitem.cpp \
           datamodel/post2dwindowparticlesbasevectordataitem.cpp \
           datamodel/post2dwindowparticlesbasevectorgroupdataitem.cpp \
           datamodel/post2dwindowparticlestopdataitem.cpp \
           datamodel/post2dwindowpolydatagroupdataitem.cpp \
           datamodel/post2dwindowpolydatatopdataitem.cpp \
           datamodel/post2dwindowpolydatavaluedataitem.cpp \
           datamodel/post2dwindowrootdataitem.cpp \
           datamodel/post2dwindowscalargrouptopdataitem.cpp \
           datamodel/post2dwindowzonedataitem.cpp \
           datamodel/private/post2dwindowcellflaggroupdataitem_setsettingcommand.cpp \
           datamodel/private/post2dwindowgeodatagroupdataitem_scalarstocolorseditdialog.cpp \
           datamodel/private/post2dwindowgraphgroupdataitem_setsettingcommand.cpp \
           datamodel/private/post2dwindowgraphsettingcustomregiondialog.cpp \
           datamodel/private/post2dwindowgridtypedataitem_applycolormapsettingcommand.cpp \
           datamodel/private/post2dwindowgridtypedataitem_applycolormapsettingdialog.cpp \
           datamodel/private/post2dwindowgridtypedataitem_precolormapsettingupdatehandler.cpp \
           datamodel/private/post2dwindowgridtypedataitem_toolbarwidgetcontroller.cpp \
           datamodel/private/post2dwindownodescalargroupdataitem_shapeexporter.cpp \
           datamodel/private/post2dwindownodevectorarrowgroupdataitem_updateactorsettingcommand.cpp \
           datamodel/private/post2dwindownodevectorarrowgroupstructureddataitem_propertydialog.cpp \
           datamodel/private/post2dwindownodevectorarrowgroupstructureddataitem_propertydialog_additionalwidgets.cpp \
           datamodel/private/post2dwindownodevectorarrowgroupunstructureddataitem_propertydialog.cpp \
           datamodel/private/post2dwindownodevectorarrowgroupunstructureddataitem_propertydialog_additionalwidgets.cpp \
           datamodel/private/post2dwindownodevectorparticlegroupstructureddataitem_impl.cpp \
           datamodel/private/post2dwindownodevectorparticlegroupstructureddataitem_setting.cpp \
           datamodel/private/post2dwindownodevectorparticlegroupstructureddataitem_settingeditwidget.cpp \
           datamodel/private/post2dwindownodevectorparticlegroupstructureddataitem_settingeditwidget_startpositioneditwidget.cpp \
           datamodel/private/post2dwindownodevectorparticlegroupunstructureddataitem_impl.cpp \
           datamodel/private/post2dwindownodevectorparticlegroupunstructureddataitem_setting.cpp \
           datamodel/private/post2dwindownodevectorparticlegroupunstructureddataitem_settingeditwidget.cpp \
           datamodel/private/post2dwindownodevectorparticlegroupunstructureddataitem_settingeditwidget_startpositioneditwidget.cpp \
           datamodel/private/post2dwindownodevectorstreamlinegroupstructureddataitem_impl.cpp \
           datamodel/private/post2dwindownodevectorstreamlinegroupstructureddataitem_setting.cpp \
           datamodel/private/post2dwindownodevectorstreamlinegroupstructureddataitem_settingeditwidget.cpp \
           datamodel/private/post2dwindownodevectorstreamlinegroupstructureddataitem_settingeditwidget_startpositioneditwidget.cpp \
           datamodel/private/post2dwindownodevectorstreamlinegroupunstructureddataitem_impl.cpp \
           datamodel/private/post2dwindownodevectorstreamlinegroupunstructureddataitem_setting.cpp \
           datamodel/private/post2dwindownodevectorstreamlinegroupunstructureddataitem_settingeditwidget.cpp \
           datamodel/private/post2dwindownodevectorstreamlinegroupunstructureddataitem_settingeditwidget_startpositioneditwidget.cpp \
           datamodel/private/post2dwindowparticlesbasescalargroupdataitem_toolbarwidget.cpp \
           datamodel/private/post2dwindowparticlesbasescalargroupdataitem_toolbarwidgetcontroller.cpp \
           datamodel/private/post2dwindowparticlesbasevectorgroupdataitem_updatesettingcommand.cpp \
           datamodel/private/post2dwindowpolydatagroupdataitem_propertydialog.cpp \
           datamodel/private/post2dwindowpolydatagroupdataitem_updateactorsettingscommand.cpp
RESOURCES += post2d.qrc
TRANSLATIONS += languages/iricPost2d_ar_EG.ts \
                languages/iricPost2d_bg_BG.ts \
                languages/iricPost2d_bs_BA.ts \
                languages/iricPost2d_ca_ES.ts \
                languages/iricPost2d_cs_CZ.ts \
                languages/iricPost2d_da_DK.ts \
                languages/iricPost2d_de_DE.ts \
                languages/iricPost2d_el_GR.ts \
                languages/iricPost2d_es_ES.ts \
                languages/iricPost2d_et_EE.ts \
                languages/iricPost2d_eu_ES.ts \
                languages/iricPost2d_fi_FI.ts \
                languages/iricPost2d_fr_FR.ts \
                languages/iricPost2d_gl_ES.ts \
                languages/iricPost2d_hi_IN.ts \
                languages/iricPost2d_hu_HU.ts \
                languages/iricPost2d_id_ID.ts \
                languages/iricPost2d_is_IS.ts \
                languages/iricPost2d_it_IT.ts \
                languages/iricPost2d_ja_JP.ts \
                languages/iricPost2d_ko_KR.ts \
                languages/iricPost2d_ky_KG.ts \
                languages/iricPost2d_lt_LT.ts \
                languages/iricPost2d_lv_LV.ts \
                languages/iricPost2d_nb_NO.ts \
                languages/iricPost2d_nl_NL.ts \
                languages/iricPost2d_pl_PL.ts \
                languages/iricPost2d_pt_BR.ts \
                languages/iricPost2d_pt_PT.ts \
                languages/iricPost2d_ro_RO.ts \
                languages/iricPost2d_ru_RU.ts \
                languages/iricPost2d_sl_SI.ts \
                languages/iricPost2d_sv_SE.ts \
                languages/iricPost2d_th_TH.ts \
                languages/iricPost2d_tr_TR.ts \
                languages/iricPost2d_uk_UA.ts \
                languages/iricPost2d_vi_VN.ts \
                languages/iricPost2d_zh_CN.ts \
                languages/iricPost2d_zh_TW.ts

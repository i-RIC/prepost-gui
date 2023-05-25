TARGET = iricPost3d
TEMPLATE = lib
INCLUDEPATH += ../..

DEFINES += POST3D_LIBRARY

include( ../../../paths.pri )

QT += widgets xml

# Use Precompiled headers (PCH)
PRECOMPILED_HEADER = post3d_pch.h

######################
# Internal libraries #
######################

# iricAxis3d

unix {
	LIBS += -L"../../dataitem/axis3d"
}
LIBS += -liricAxis3d

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
HEADERS += post3d_global.h \
           datamodel/private/post3dwindowcontourgroupdataitem_impl.h \
           datamodel/private/post3dwindowcontourgroupdataitem_setting.h \
           datamodel/private/post3dwindowcontourgroupdataitem_settingeditwidget.h \
           post3dobjectbrowser.h \
           post3dobjectbrowserview.h \
           post3dsamplingratesettingcontainer.h \
           post3dwindow.h \
           post3dwindowactionmanager.h \
           post3dwindowcellrangelistsettingeditwidget.h \
           post3dwindowcellrangesettingcontainer.h \
           post3dwindowcellrangesettingeditwidget.h \
           post3dwindowdataitem.h \
           post3dwindowdatamodel.h \
           post3dwindowfacelistsettingeditwidget.h \
           post3dwindowfacesettingcontainer.h \
           post3dwindowfacesettingeditwidget.h \
           post3dwindowgraphicsview.h \
           post3dwindowprojectdataitem.h \
           datamodel/post3dwindowcellcontourdataitem.h \
           datamodel/post3dwindowcellcontourgroupdataitem.h \
           datamodel/post3dwindowcellcontourgrouptopdataitem.h \
           datamodel/post3dwindowcontourdataitem.h \
           datamodel/post3dwindowcontourgroupdataitem.h \
           datamodel/post3dwindowcontourgrouptopdataitem.h \
           datamodel/post3dwindowfacedataitem.h \
           datamodel/post3dwindowfacesettingwidget.h \
           datamodel/post3dwindowgridshapedataitem.h \
           datamodel/post3dwindowgridtypedataitem.h \
           datamodel/post3dwindowisosurfacesettingdialog.h \
           datamodel/post3dwindownodescalargroupdataitem.h \
           datamodel/post3dwindownodescalargrouptopdataitem.h \
           datamodel/post3dwindownodevectorarrowdataitem.h \
           datamodel/post3dwindownodevectorarrowgroupdataitem.h \
           datamodel/post3dwindownodevectorarrowtopdataitem.h \
           datamodel/post3dwindownodevectorparticledataitem.h \
           datamodel/post3dwindownodevectorparticlegroupdataitem.h \
           datamodel/post3dwindownodevectorparticlegroupstructureddataitem.h \
           datamodel/post3dwindownodevectorstreamlinedataitem.h \
           datamodel/post3dwindownodevectorstreamlinegroupdataitem.h \
           datamodel/post3dwindownodevectorstreamlinegroupstructureddataitem.h \
           datamodel/post3dwindowparticlegrouprootdataitem.h \
           datamodel/post3dwindowparticlegrouptopdataitem.h \
           datamodel/post3dwindowparticlesbasescalardataitem.h \
           datamodel/post3dwindowparticlesbasescalargroupdataitem.h \
           datamodel/post3dwindowparticlesbasetopdataitem.h \
           datamodel/post3dwindowparticlesbasevectordataitem.h \
           datamodel/post3dwindowparticlesbasevectorgroupdataitem.h \
           datamodel/post3dwindowparticlestopdataitem.h \
           datamodel/post3dwindowrootdataitem.h \
           datamodel/post3dwindowsurfacedataitem.h \
           datamodel/post3dwindowsurfacegroupdataitem.h \
           datamodel/post3dwindowzonedataitem.h \
           datamodel/private/post3dwindowcellcontourgroupdataitem_propertydialog.h \
           datamodel/private/post3dwindowcellcontourgroupdataitem_setrangescommand.h \
           datamodel/private/post3dwindowcellcontourgroupdataitem_updateactorsettingcommand.h \
           datamodel/private/post3dwindowcontourgroupdataitem_setfacescommand.h \
           datamodel/private/post3dwindowgridshapedataitem_propertydialog.h \
           datamodel/private/post3dwindowgridshapedataitem_updateactorsettingscommand.h \
           datamodel/private/post3dwindownodescalargroupdataitem_setsettingcommand.h \
           datamodel/private/post3dwindownodevectorarrowgroupdataitem_propertydialog.h \
           datamodel/private/post3dwindownodevectorarrowgroupdataitem_setfacesettingscommand.h \
           datamodel/private/post3dwindownodevectorarrowgroupdataitem_updateactorsettingcommand.h \
           datamodel/private/post3dwindownodevectorparticlegroupstructureddataitem_impl.h \
           datamodel/private/post3dwindownodevectorparticlegroupstructureddataitem_setting.h \
           datamodel/private/post3dwindownodevectorparticlegroupstructureddataitem_settingeditwidget.h \
           datamodel/private/post3dwindownodevectorparticlegroupstructureddataitem_settingeditwidget_startpositioneditwidget.h \
           datamodel/private/post3dwindownodevectorstreamlinegroupstructureddataitem_impl.h \
           datamodel/private/post3dwindownodevectorstreamlinegroupstructureddataitem_setting.h \
           datamodel/private/post3dwindownodevectorstreamlinegroupstructureddataitem_settingeditwidget.h \
           datamodel/private/post3dwindownodevectorstreamlinegroupstructureddataitem_settingeditwidget_startpositioneditwidget.h \
           datamodel/private/post3dwindowparticlesbasescalargroupdataitem_propertydialog.h \
           datamodel/private/post3dwindowparticlesbasescalargroupdataitem_toolbarwidget.h \
           datamodel/private/post3dwindowparticlesbasescalargroupdataitem_toolbarwidgetcontroller.h \
           datamodel/private/post3dwindowparticlesbasescalargroupdataitem_updateactorsettingcommand.h \
           datamodel/private/post3dwindowparticlesbasevectorgroupdataitem_propertydialog.h \
           datamodel/private/post3dwindowparticlesbasevectorgroupdataitem_updatesettingcommand.h
FORMS += post3dwindowcellrangelistsettingeditwidget.ui \
         datamodel/private/post3dwindowcontourgroupdataitem_settingeditwidget.ui \
         post3dwindowcellrangesettingeditwidget.ui \
         post3dwindowfacelistsettingeditwidget.ui \
         post3dwindowfacesettingeditwidget.ui \
         datamodel/post3dwindowfacesettingwidget.ui \
         datamodel/post3dwindowisosurfacesettingdialog.ui \
         datamodel/private/post3dwindowcellcontourgroupdataitem_propertydialog.ui \
         datamodel/private/post3dwindowgridshapedataitem_propertydialog.ui \
         datamodel/private/post3dwindownodevectorarrowgroupdataitem_propertydialog.ui \
         datamodel/private/post3dwindownodevectorparticlegroupstructureddataitem_settingeditwidget.ui \
         datamodel/private/post3dwindownodevectorparticlegroupstructureddataitem_settingeditwidget_startpositioneditwidget.ui \
         datamodel/private/post3dwindownodevectorstreamlinegroupstructureddataitem_settingeditwidget.ui \
         datamodel/private/post3dwindownodevectorstreamlinegroupstructureddataitem_settingeditwidget_startpositioneditwidget.ui \
         datamodel/private/post3dwindowparticlesbasescalargroupdataitem_propertydialog.ui \
         datamodel/private/post3dwindowparticlesbasescalargroupdataitem_toolbarwidget.ui \
         datamodel/private/post3dwindowparticlesbasevectorgroupdataitem_propertydialog.ui
SOURCES += post3dobjectbrowser.cpp \
           datamodel/private/post3dwindowcontourgroupdataitem_impl.cpp \
           datamodel/private/post3dwindowcontourgroupdataitem_setting.cpp \
           datamodel/private/post3dwindowcontourgroupdataitem_settingeditwidget.cpp \
           post3dobjectbrowserview.cpp \
           post3dsamplingratesettingcontainer.cpp \
           post3dwindow.cpp \
           post3dwindowactionmanager.cpp \
           post3dwindowcellrangelistsettingeditwidget.cpp \
           post3dwindowcellrangesettingcontainer.cpp \
           post3dwindowcellrangesettingeditwidget.cpp \
           post3dwindowdataitem.cpp \
           post3dwindowdatamodel.cpp \
           post3dwindowfacelistsettingeditwidget.cpp \
           post3dwindowfacesettingcontainer.cpp \
           post3dwindowfacesettingeditwidget.cpp \
           post3dwindowgraphicsview.cpp \
           post3dwindowprojectdataitem.cpp \
           datamodel/post3dwindowcellcontourdataitem.cpp \
           datamodel/post3dwindowcellcontourgroupdataitem.cpp \
           datamodel/post3dwindowcellcontourgrouptopdataitem.cpp \
           datamodel/post3dwindowcontourdataitem.cpp \
           datamodel/post3dwindowcontourgroupdataitem.cpp \
           datamodel/post3dwindowcontourgrouptopdataitem.cpp \
           datamodel/post3dwindowfacedataitem.cpp \
           datamodel/post3dwindowfacesettingwidget.cpp \
           datamodel/post3dwindowgridshapedataitem.cpp \
           datamodel/post3dwindowgridtypedataitem.cpp \
           datamodel/post3dwindowisosurfacesettingdialog.cpp \
           datamodel/post3dwindownodescalargroupdataitem.cpp \
           datamodel/post3dwindownodescalargrouptopdataitem.cpp \
           datamodel/post3dwindownodevectorarrowdataitem.cpp \
           datamodel/post3dwindownodevectorarrowgroupdataitem.cpp \
           datamodel/post3dwindownodevectorarrowtopdataitem.cpp \
           datamodel/post3dwindownodevectorparticledataitem.cpp \
           datamodel/post3dwindownodevectorparticlegroupdataitem.cpp \
           datamodel/post3dwindownodevectorparticlegroupstructureddataitem.cpp \
           datamodel/post3dwindownodevectorstreamlinedataitem.cpp \
           datamodel/post3dwindownodevectorstreamlinegroupdataitem.cpp \
           datamodel/post3dwindownodevectorstreamlinegroupstructureddataitem.cpp \
           datamodel/post3dwindowparticlegrouprootdataitem.cpp \
           datamodel/post3dwindowparticlegrouptopdataitem.cpp \
           datamodel/post3dwindowparticlesbasescalardataitem.cpp \
           datamodel/post3dwindowparticlesbasescalargroupdataitem.cpp \
           datamodel/post3dwindowparticlesbasetopdataitem.cpp \
           datamodel/post3dwindowparticlesbasevectordataitem.cpp \
           datamodel/post3dwindowparticlesbasevectorgroupdataitem.cpp \
           datamodel/post3dwindowparticlestopdataitem.cpp \
           datamodel/post3dwindowrootdataitem.cpp \
           datamodel/post3dwindowsurfacedataitem.cpp \
           datamodel/post3dwindowsurfacegroupdataitem.cpp \
           datamodel/post3dwindowzonedataitem.cpp \
           datamodel/private/post3dwindowcellcontourgroupdataitem_propertydialog.cpp \
           datamodel/private/post3dwindowcellcontourgroupdataitem_setrangescommand.cpp \
           datamodel/private/post3dwindowcellcontourgroupdataitem_updateactorsettingcommand.cpp \
           datamodel/private/post3dwindowcontourgroupdataitem_setfacescommand.cpp \
           datamodel/private/post3dwindowgridshapedataitem_propertydialog.cpp \
           datamodel/private/post3dwindowgridshapedataitem_updateactorsettingscommand.cpp \
           datamodel/private/post3dwindownodescalargroupdataitem_setsettingcommand.cpp \
           datamodel/private/post3dwindownodevectorarrowgroupdataitem_propertydialog.cpp \
           datamodel/private/post3dwindownodevectorarrowgroupdataitem_setfacesettingscommand.cpp \
           datamodel/private/post3dwindownodevectorarrowgroupdataitem_updateactorsettingcommand.cpp \
           datamodel/private/post3dwindownodevectorparticlegroupstructureddataitem_impl.cpp \
           datamodel/private/post3dwindownodevectorparticlegroupstructureddataitem_setting.cpp \
           datamodel/private/post3dwindownodevectorparticlegroupstructureddataitem_settingeditwidget.cpp \
           datamodel/private/post3dwindownodevectorparticlegroupstructureddataitem_settingeditwidget_startpositioneditwidget.cpp \
           datamodel/private/post3dwindownodevectorstreamlinegroupstructureddataitem_impl.cpp \
           datamodel/private/post3dwindownodevectorstreamlinegroupstructureddataitem_setting.cpp \
           datamodel/private/post3dwindownodevectorstreamlinegroupstructureddataitem_settingeditwidget.cpp \
           datamodel/private/post3dwindownodevectorstreamlinegroupstructureddataitem_settingeditwidget_startpositioneditwidget.cpp \
           datamodel/private/post3dwindowparticlesbasescalargroupdataitem_propertydialog.cpp \
           datamodel/private/post3dwindowparticlesbasescalargroupdataitem_toolbarwidget.cpp \
           datamodel/private/post3dwindowparticlesbasescalargroupdataitem_toolbarwidgetcontroller.cpp \
           datamodel/private/post3dwindowparticlesbasescalargroupdataitem_updateactorsettingcommand.cpp \
           datamodel/private/post3dwindowparticlesbasevectorgroupdataitem_propertydialog.cpp \
           datamodel/private/post3dwindowparticlesbasevectorgroupdataitem_updatesettingcommand.cpp
RESOURCES += post3d.qrc
TRANSLATIONS += languages/iricPost3d_ar_EG.ts \
                languages/iricPost3d_bg_BG.ts \
                languages/iricPost3d_bs_BA.ts \
                languages/iricPost3d_ca_ES.ts \
                languages/iricPost3d_cs_CZ.ts \
                languages/iricPost3d_da_DK.ts \
                languages/iricPost3d_de_DE.ts \
                languages/iricPost3d_el_GR.ts \
                languages/iricPost3d_es_ES.ts \
                languages/iricPost3d_et_EE.ts \
                languages/iricPost3d_eu_ES.ts \
                languages/iricPost3d_fi_FI.ts \
                languages/iricPost3d_fr_FR.ts \
                languages/iricPost3d_gl_ES.ts \
                languages/iricPost3d_hi_IN.ts \
                languages/iricPost3d_hu_HU.ts \
                languages/iricPost3d_id_ID.ts \
                languages/iricPost3d_is_IS.ts \
                languages/iricPost3d_it_IT.ts \
                languages/iricPost3d_ja_JP.ts \
                languages/iricPost3d_ko_KR.ts \
                languages/iricPost3d_ky_KG.ts \
                languages/iricPost3d_lt_LT.ts \
                languages/iricPost3d_lv_LV.ts \
                languages/iricPost3d_nb_NO.ts \
                languages/iricPost3d_nl_NL.ts \
                languages/iricPost3d_pl_PL.ts \
                languages/iricPost3d_pt_BR.ts \
                languages/iricPost3d_pt_PT.ts \
                languages/iricPost3d_ro_RO.ts \
                languages/iricPost3d_ru_RU.ts \
                languages/iricPost3d_sl_SI.ts \
                languages/iricPost3d_sv_SE.ts \
                languages/iricPost3d_th_TH.ts \
                languages/iricPost3d_tr_TR.ts \
                languages/iricPost3d_uk_UA.ts \
                languages/iricPost3d_vi_VN.ts \
                languages/iricPost3d_zh_CN.ts \
                languages/iricPost3d_zh_TW.ts

TARGET = iricGdPointmap
TEMPLATE = lib
INCLUDEPATH += ../..

DEFINES += GD_POINTMAP_LIBRARY

DEFINES += ANSI_DECLARATORS

include( ../../../paths.pri )

QT += network widgets xml

# Use Precompiled headers (PCH)
PRECOMPILED_HEADER = pointmap_pch.h

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

# iricGdPolygonGroup

unix {
        LIBS += -L"../polygongroup"
}
LIBS += -liricGdPolygonGroup

# iricGdPolygon

unix {
        LIBS += -L"../polygon"
}
LIBS += -liricGdPolygon

# iricGdPolydataGroup

unix {
        LIBS += -L"../polydatagroup"
}
LIBS += -liricGdPolyDataGroup

# iricGdPolydata

unix {
        LIBS += -L"../polydata"
}
LIBS += -liricGdPolydata

######################
# External libraries #
######################

# VTK

LIBS += \
	-lvtkCommonComputationalGeometry-$${VTK_MAJ_MIN} \
	-lvtkCommonCore-$${VTK_MAJ_MIN} \
	-lvtkCommonDataModel-$${VTK_MAJ_MIN} \
	-lvtkCommonExecutionModel-$${VTK_MAJ_MIN} \
	-lvtkCommonTransforms-$${VTK_MAJ_MIN} \
	-lvtkFiltersCore-$${VTK_MAJ_MIN} \
	-lvtkFiltersGeneral-$${VTK_MAJ_MIN} \
	-lvtkIOCore-$${VTK_MAJ_MIN} \
	-lvtkIOExport-$${VTK_MAJ_MIN} \
	-lvtkIOGeometry-$${VTK_MAJ_MIN} \
	-lvtkIOLegacy-$${VTK_MAJ_MIN} \
	-lvtkRenderingCore-$${VTK_MAJ_MIN} \
	-lvtkRenderingLabel-$${VTK_MAJ_MIN} \
	-lvtkRenderingLOD-$${VTK_MAJ_MIN}

# geos

LIBS += -lgeos

# iriclib

win32 {
    CONFIG(debug, debug|release) {
        LIBS += -liriclibd
    } else {
        LIBS += -liriclib
    }
}

unix {
    LIBS += -liriclib
}

# poco

win32 {
    CONFIG(debug, debug|release) {
        LIBS += -lPocoFoundationd
    } else {
        LIBS += -lPocoFoundation
    }
}

unix {
    LIBS += -lPocoFoundation
}

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
           geodatapointmapcellmapper.h \
           geodatapointmaplandxmlexporter.h \
           geodatapointmaplandxmlimporter.h \
           geodatapointmapmappingmode.h \
           geodatapointmapmergesettingdialog.h \
           geodatapointmapnodemapper.h \
           geodatapointmapproxy.h \
           geodatapointmaprealbuilder.h \
           geodatapointmaprealcreator.h \
           geodatapointmaprealexporter.h \
           geodatapointmaprealtextimporter.h \
           geodatapointmapriter3dimporter.h \
           geodatapointmapriter3dloader.h \
           geodatapointmapriter3dprojectwatcher.h \
           geodatapointmapstlexporter.h \
           geodatapointmapstlimporter.h \
           geodatapointmaptemplatemappingsetting.h \
           geodatapointmaptemplatenodemapper.h \
           geodatapointmaptincreator.h \
           geodatapointmapvtkexporter.h \
           geodatapointmapwebimporter.h \
           geodatapointmapwebimporterregionselectdialog.h \
           geodatapointmapwebimportersetting.h \
           geodatapointmapwebimportersettingmanager.h \
           geodatapointmapwebimporterzoomleveldialog.h \
           private/geodatapointmap_displaysetting.h \
           private/geodatapointmap_impl.h \
           private/geodatapointmap_mappingsetting.h \
           private/geodatapointmap_mappingsettingdialog.h \
           private/geodatapointmap_mappingsettingdialog_templatedetaildialog.h \
           private/geodatapointmap_modifycommand.h \
           private/geodatapointmap_pointsmanager.h \
           private/geodatapointmap_pointsmanager_actions.h \
           private/geodatapointmap_pointsmanager_addpointcontroller.h \
           private/geodatapointmap_pointsmanager_addpointcontroller_setexistingpointcommand.h \
           private/geodatapointmap_pointsmanager_addpointscommand.h \
           private/geodatapointmap_pointsmanager_deletepointscommand.h \
           private/geodatapointmap_pointsmanager_editpointscommand.h \
           private/geodatapointmap_pointsmanager_editpointsdialog.h \
           private/geodatapointmap_pointsmanager_editsinglepointcommand.h \
           private/geodatapointmap_pointsmanager_filterselectedpointsdialog.h \
           private/geodatapointmap_pointsmanager_interpolatepointcontroller.h \
           private/geodatapointmap_pointsmanager_interpolatepointcontroller_addpointcommand.h \
           private/geodatapointmap_pointsmanager_interpolatepointcontroller_settingdialog.h \
           private/geodatapointmap_pointsmanager_selectpointboundingboxcontroller.h \
           private/geodatapointmap_pointsmanager_selectpointpolygoncontroller.h \
           private/geodatapointmap_polygonscellmapper.h \
           private/geodatapointmap_polygonsmanager.h \
           private/geodatapointmap_polygonsmanager_attributeformappingcondition.h \
           private/geodatapointmap_polygonsmanager_attributeformappingcondition_editwidget.h \
           private/geodatapointmap_polygonsmanager_attributeformappingcondition_stringconverter.h \
           private/geodatapointmap_polygonsmanager_geodatadataitem.h \
           private/geodatapointmap_polygonsmanager_geodatagroupdataitem.h \
           private/geodatapointmap_polygonsmanager_geodatatopdataitem.h \
           private/geodatapointmap_polygonsmanager_gridtypedataitem.h \
           private/geodatapointmap_polygonsmanager_mappingsetting.h \
           private/geodatapointmap_polygonsmanager_rootdataitem.h \
           private/geodatapointmap_polygonsnodemapper.h \
           private/geodatapointmap_riter3dsetting.h \
           private/geodatapointmap_templatecellmapper.h \
           private/geodatapointmap_templatenodemapper.h \
           private/geodatapointmap_tincellmapper.h \
           private/geodatapointmap_tinmanager.h \
           private/geodatapointmap_tinmanager_actions.h \
           private/geodatapointmap_tinmanager_addbreaklinecontroller.h \
           private/geodatapointmap_tinmanager_addbreaklinecontroller_addpointcommand.h \
           private/geodatapointmap_tinmanager_breakline.h \
           private/geodatapointmap_tinmanager_deletebreaklinecontroller.h \
           private/geodatapointmap_tinmanager_impl.h \
           private/geodatapointmap_tinmanager_rebuildtinfrompointscommand.h \
           private/geodatapointmap_tinmanager_removetrianglescommand.h \
           private/geodatapointmap_tinmanager_removetrianglessetting.h \
           private/geodatapointmap_tinmanager_removetrianglessettingdialog.h \
           private/geodatapointmap_tinmanager_removetriangleswithlongedgecontroller.h \
           private/geodatapointmap_tinmanager_tinbuilder.h \
           private/geodatapointmap_tinmanager_triangleswithlongedgeremover.h \
           private/geodatapointmap_tinnodemapper.h \
           private/geodatapointmapproxy_displaysetting.h \
           private/geodatapointmapproxy_displaysettingwidget.h \
           private/geodatapointmapproxy_impl.h \
           private/geodatapointmaprealbuilder_impl.h \
           private/geodatapointmaprealtextimporter_lineparser.h \
           private/geodatapointmaprealtextimporter_settingdialog.h \
           private/geodatapointmaprealtextimporter_values.h \
           private/geodatapointmapriter3dloader_tarheaderblock.h \
           private/geodatapointmapwebimportersetting_impl.h \
           public/geodatapointmap_displaysettingwidget.h
FORMS += geodatapointmapmergesettingdialog.ui \
         geodatapointmapwebimporterregionselectdialog.ui \
         geodatapointmapwebimporterzoomleveldialog.ui \
         private/geodatapointmap_mappingsettingdialog.ui \
         private/geodatapointmap_mappingsettingdialog_templatedetaildialog.ui \
         private/geodatapointmap_pointsmanager_editpointsdialog.ui \
         private/geodatapointmap_pointsmanager_filterselectedpointsdialog.ui \
         private/geodatapointmap_pointsmanager_interpolatepointcontroller_settingdialog.ui \
         private/geodatapointmap_polygonsmanager_attributeformappingcondition_editwidget.ui \
         private/geodatapointmap_tinmanager_removetrianglessettingdialog.ui \
         private/geodatapointmapproxy_displaysettingwidget.ui \
         private/geodatapointmaprealtextimporter_settingdialog.ui \
         public/geodatapointmap_displaysettingwidget.ui
SOURCES += geodatapointmap.cpp \
           geodatapointmapcellmapper.cpp \
           geodatapointmaplandxmlexporter.cpp \
           geodatapointmaplandxmlimporter.cpp \
           geodatapointmapmappingmode.cpp \
           geodatapointmapmergesettingdialog.cpp \
           geodatapointmapnodemapper.cpp \
           geodatapointmapproxy.cpp \
           geodatapointmaprealbuilder.cpp \
           geodatapointmaprealcreator.cpp \
           geodatapointmaprealexporter.cpp \
           geodatapointmaprealtextimporter.cpp \
           geodatapointmapriter3dimporter.cpp \
           geodatapointmapriter3dloader.cpp \
           geodatapointmapriter3dprojectwatcher.cpp \
           geodatapointmapstlexporter.cpp \
           geodatapointmapstlimporter.cpp \
           geodatapointmaptemplatemappingsetting.cpp \
           geodatapointmaptemplatenodemapper.cpp \
           geodatapointmaptincreator.cpp \
           geodatapointmapvtkexporter.cpp \
           geodatapointmapwebimporter.cpp \
           geodatapointmapwebimporterregionselectdialog.cpp \
           geodatapointmapwebimportersetting.cpp \
           geodatapointmapwebimportersettingmanager.cpp \
           geodatapointmapwebimporterzoomleveldialog.cpp \
           private/geodatapointmap_displaysetting.cpp \
           private/geodatapointmap_impl.cpp \
           private/geodatapointmap_mappingsetting.cpp \
           private/geodatapointmap_mappingsettingdialog.cpp \
           private/geodatapointmap_mappingsettingdialog_templatedetaildialog.cpp \
           private/geodatapointmap_modifycommand.cpp \
           private/geodatapointmap_pointsmanager.cpp \
           private/geodatapointmap_pointsmanager_actions.cpp \
           private/geodatapointmap_pointsmanager_addpointcontroller.cpp \
           private/geodatapointmap_pointsmanager_addpointcontroller_setexistingpointcommand.cpp \
           private/geodatapointmap_pointsmanager_addpointscommand.cpp \
           private/geodatapointmap_pointsmanager_deletepointscommand.cpp \
           private/geodatapointmap_pointsmanager_editpointscommand.cpp \
           private/geodatapointmap_pointsmanager_editpointsdialog.cpp \
           private/geodatapointmap_pointsmanager_editsinglepointcommand.cpp \
           private/geodatapointmap_pointsmanager_filterselectedpointsdialog.cpp \
           private/geodatapointmap_pointsmanager_interpolatepointcontroller.cpp \
           private/geodatapointmap_pointsmanager_interpolatepointcontroller_addpointcommand.cpp \
           private/geodatapointmap_pointsmanager_interpolatepointcontroller_settingdialog.cpp \
           private/geodatapointmap_pointsmanager_selectpointboundingboxcontroller.cpp \
           private/geodatapointmap_pointsmanager_selectpointpolygoncontroller.cpp \
           private/geodatapointmap_polygonscellmapper.cpp \
           private/geodatapointmap_polygonsmanager.cpp \
           private/geodatapointmap_polygonsmanager_attributeformappingcondition.cpp \
           private/geodatapointmap_polygonsmanager_attributeformappingcondition_editwidget.cpp \
           private/geodatapointmap_polygonsmanager_attributeformappingcondition_stringconverter.cpp \
           private/geodatapointmap_polygonsmanager_geodatadataitem.cpp \
           private/geodatapointmap_polygonsmanager_geodatagroupdataitem.cpp \
           private/geodatapointmap_polygonsmanager_geodatatopdataitem.cpp \
           private/geodatapointmap_polygonsmanager_gridtypedataitem.cpp \
           private/geodatapointmap_polygonsmanager_mappingsetting.cpp \
           private/geodatapointmap_polygonsmanager_rootdataitem.cpp \
           private/geodatapointmap_polygonsnodemapper.cpp \
           private/geodatapointmap_riter3dsetting.cpp \
           private/geodatapointmap_templatecellmapper.cpp \
           private/geodatapointmap_templatenodemapper.cpp \
           private/geodatapointmap_tincellmapper.cpp \
           private/geodatapointmap_tinmanager.cpp \
           private/geodatapointmap_tinmanager_actions.cpp \
           private/geodatapointmap_tinmanager_addbreaklinecontroller.cpp \
           private/geodatapointmap_tinmanager_addbreaklinecontroller_addpointcommand.cpp \
           private/geodatapointmap_tinmanager_breakline.cpp \
           private/geodatapointmap_tinmanager_deletebreaklinecontroller.cpp \
           private/geodatapointmap_tinmanager_impl.cpp \
           private/geodatapointmap_tinmanager_rebuildtinfrompointscommand.cpp \
           private/geodatapointmap_tinmanager_removetrianglescommand.cpp \
           private/geodatapointmap_tinmanager_removetrianglessetting.cpp \
           private/geodatapointmap_tinmanager_removetrianglessettingdialog.cpp \
           private/geodatapointmap_tinmanager_removetriangleswithlongedgecontroller.cpp \
           private/geodatapointmap_tinmanager_tinbuilder.cpp \
           private/geodatapointmap_tinmanager_triangleswithlongedgeremover.cpp \
           private/geodatapointmap_tinnodemapper.cpp \
           private/geodatapointmapproxy_displaysetting.cpp \
           private/geodatapointmapproxy_displaysettingwidget.cpp \
           private/geodatapointmapproxy_impl.cpp \
           private/geodatapointmaprealtextimporter_lineparser.cpp \
           private/geodatapointmaprealtextimporter_settingdialog.cpp \
           private/geodatapointmapriter3dloader_tarheaderblock.cpp \
           public/geodatapointmap_displaysettingwidget.cpp
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

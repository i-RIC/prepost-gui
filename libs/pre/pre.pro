######################################################################
# Automatically generated by qmake (2.01a) ? 10 24 00:36:17 2014
######################################################################

TARGET = iricPre
TEMPLATE = lib


DEFINES += PRE_LIBRARY

include( ../../paths.pri )

QT += widgets xml

# Use Precompiled headers (PCH)
PRECOMPILED_HEADER = pre_pch.h

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
	LIBS += -L"../measureddata/axis2d"
}
LIBS += -liricMeasureddata

# iricMisc

unix {
	LIBS += -L"../misc"
}
LIBS += -liricMisc

# iricGuibase

unix {
	LIBS += -L"../guibase"
}
LIBS += -liricGuibase

# iricGuicore

unix {
	LIBS += -L"../guicore"
}

LIBS += -liricGuicore

# iricGdRiversurvey

unix {
	LIBS += -L"../geodata/riversurvey"
}
LIBS += -liricGdRiversurvey

# iricGdPoint

unix {
        LIBS += -L"../geodata/point"
}
LIBS += -liricGdPoint

# iricGdPointGroup

unix {
        LIBS += -L"../geodata/pointgroup"
}
LIBS += -liricGdPointGroup

# iricGdPointmap

unix {
	LIBS += -L"../geodata/pointmap"
}
LIBS += -liricGdPointmap

# iricGdPolyData

unix {
        LIBS += -L"../geodata/polydata"
}
LIBS += -liricGdPolyData

# iricGdPolygon

unix {
	LIBS += -L"../geodata/polygon"
}
LIBS += -liricGdPolygon

# iricGdPolygonGroup

unix {
        LIBS += -L"../geodata/polygongroup"
}
LIBS += -liricGdPolygonGroup

# iricGdPolyline

unix {
	LIBS += -L"../geodata/polyline"
}
LIBS += -liricGdPolyline

# iricGdPolylineGroup

unix {
        LIBS += -L"../geodata/polylinegroup"
}
LIBS += -liricGdPolylineGroup

# iricGdNetcdf
unix {
	LIBS += -L"../geodata/netcdf"
}
LIBS += -liricGdNetcdf

# iricGccCenterandwidth

unix {
	LIBS += -L"../gridcreatingcondition/centerandwidth"
}
LIBS += -liricGccCenterandwidth

# iricGccCompoundchannel

unix {
	LIBS += -L"../gridcreatingcondition/compoundchannel"
}
LIBS += -liricGccCompoundchannel

# iricGccExternalprogram

unix {
	LIBS += -L"../gridcreatingcondition/externalprogram"
}
LIBS += -liricGccExternalprogram

# iricGccGridcombine

unix {
	LIBS += -L"../gridcreatingcondition/gridcombine"
}
LIBS += -liricGccGridcombine

# iricGccGridgenerator

unix {
	LIBS += -L"../gridcreatingcondition/gridgenerator"
}
LIBS += -liricGccGridgenerator

# iricGccLaplace

unix {
	LIBS += -L"../gridcreatingcondition/laplace"
}
LIBS += -liricGccLaplace

# iricGccRectangularregion

unix {
	LIBS += -L"../gridcreatingcondition/rectangularregion"
}
LIBS += -liricGccRectangularregion

# iricGccRectangularregionlonlat

unix {
	LIBS += -L"../gridcreatingcondition/rectangularregionlonlat"
}
LIBS += -liricGccRectangularregionlonlat

# iricGccRiversurvey

unix {
	LIBS += -L"../gridcreatingcondition/riversurvey"
}
LIBS += -liricGccRiversurvey

# iricGccRiversurvey15d

unix {
	LIBS += -L"../gridcreatingcondition/riversurvey15d"
}
LIBS += -liricGccRiversurvey15d

# iricGccTriangle

unix {
	LIBS += -L"../gridcreatingcondition/triangle"
}
LIBS += -liricGccTriangle

# iricGccPoisson

unix {
	LIBS += -L"../gridcreatingcondition/poisson"
}
LIBS += -liricGccPoisson

# iricHdRiversurveywaterelevation

unix {
	LIBS += -L"../hydraulicdata/riversurveywaterelevation"
}
LIBS += -liricHdRiversurveywaterelevation

######################
# External libraries #
######################

# VTK

LIBS += \
	-lvtkCommonCore-$${VTK_MAJ_MIN} \
	-lvtkCommonDataModel-$${VTK_MAJ_MIN} \
	-lvtkCommonExecutionModel-$${VTK_MAJ_MIN} \
	-lvtkCommonMisc-$${VTK_MAJ_MIN} \
	-lvtkFiltersCore-$${VTK_MAJ_MIN} \
	-lvtkFiltersGeometry-$${VTK_MAJ_MIN} \
	-lvtkFiltersGeneral-$${VTK_MAJ_MIN} \
	-lvtkFiltersExtraction-$${VTK_MAJ_MIN} \
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

# shapelib

win32 {
	LIBS += -lshapelib_i
}

win32 {
	DESTDIR = $(SolutionDir)/libdlls/$(Configuration)
	LIBS += -L$(SolutionDir)/libdlls/$(Configuration)
}

# Input
HEADERS += pre_global.h \
           preobjectbrowser.h \
           preobjectbrowserview.h \
           preprocessordatamodel.h \
           preprocessorgraphicsview.h \
           preprocessorwindow.h \
           preprocessorwindowactionmanager.h \
           preprocessorwindowprojectdataitem.h \
           prepropertybrowser.h \
           bc/boundaryconditiondialog.h \
           datamodel/attributebrowserhelper.h \
           datamodel/graphicssupport.h \
           datamodel/preprocessorbackgroundimagedataitem.h \
           datamodel/preprocessorbackgroundimagesdataitem.h \
           datamodel/preprocessorbcdataitem.h \
           datamodel/preprocessorbcgroupdataitem.h \
           datamodel/preprocessorbcgroupsettingdialog.h \
           datamodel/preprocessorbcsettingdataitem.h \
           datamodel/preprocessorbcsettinggroupdataitem.h \
           datamodel/preprocessorcustommappingsetting.h \
           datamodel/preprocessorgeodatacomplexgroupdataitem.h \
           datamodel/preprocessorgeodatadataitem.h \
           datamodel/preprocessorgeodatagroupdataitem.h \
           datamodel/preprocessorgeodatatopdataitem.h \
           datamodel/preprocessorgeodatavariationdataitem.h \
           datamodel/preprocessorgridandgridcreatingconditiondataitem.h \
           datamodel/preprocessorgridattributecelldataitem.h \
           datamodel/preprocessorgridattributecelldisplaysettingdialog.h \
           datamodel/preprocessorgridattributecellgroupdataitem.h \
           datamodel/preprocessorgridattributecustommappingdialog.h \
           datamodel/preprocessorgridattributemappingsettingdataitem.h \
           datamodel/preprocessorgridattributemappingsettingtopdataitem.h \
           datamodel/preprocessorgridattributemappingsettingtopeditdialog.h \
           datamodel/preprocessorgridattributenodedataitem.h \
           datamodel/preprocessorgridattributenodegroupdataitem.h \
           datamodel/preprocessorgridattributetemplatemappingsettingdialog.h \
           datamodel/preprocessorgridcreatingconditionalgorithmselectdialog.h \
           datamodel/preprocessorgridcreatingconditiondataitem.h \
           datamodel/preprocessorgriddataitem.h \
           datamodel/preprocessorgridshapedataitem.h \
           datamodel/preprocessorgridtypedataitem.h \
           datamodel/preprocessorhydraulicdatadataitem.h \
           datamodel/preprocessorhydraulicdatagroupdataitem.h \
           datamodel/preprocessorhydraulicdatatopdataitem.h \
           datamodel/preprocessorinputconditiondataitem.h \
           datamodel/preprocessormeasureddatatopdataitem.h \
           datamodel/preprocessornormal15dgridwithcrosssectiondataitem.h \
           datamodel/preprocessornormal15dgridwithcrosssectionshapedataitem.h \
           datamodel/preprocessorrootdataitem.h \
           datamodel/preprocessorstructured2dgriddataitem.h \
           datamodel/preprocessorstructured2dgridshapedataitem.h \
           datamodel/preprocessorunstructured2dgriddataitem.h \
           datamodel/preprocessorunstructured2dgridshapedataitem.h \
           factory/geodatafactorysetup.h \
           factory/gridcreatingconditionfactory.h \
           factory/gridexporterfactory.h \
           factory/gridimporterfactory.h \
           factory/hydraulicdatafactory.h \
           factory/preprocessorgriddataitemfactory.h \
           gridexporter/cgnsgridexporter.h \
           gridimporter/cgnsgridimporter.h \
           gridimporter/projectgridimporter.h \
           gridimporter/structured15dgridwithcrosssectioncgnsimporter.h \
           gridimporter/structured15dgridwithcrosssectionprojectimporter.h \
           gridimporter/structured2dgridcgnsimporter.h \
           gridimporter/structured2dgridprojectimporter.h \
           gridimporter/unstructured2dgridcgnsimporter.h \
           gridimporter/unstructured2dgridprojectimporter.h \
           misc/gridattributegeneratorlauncher.h \
           misc/preprocessorgridattributemappingmode.h \
           misc/preprocessorlegendboxeditdialog.h \
           datamodel/gridshape/preprocessorgridshapedeltadialog.h \
           datamodel/gridshape/preprocessorgridshapenewpositiondialog.h \
           datamodel/private/preprocessorbcdataitem_impl.h \
           datamodel/private/preprocessorbcgroupdataitem_setsettingcommand.h \
           datamodel/private/preprocessorgeodatagroupdataitem_colormapsettingeditdialog.h \
           datamodel/private/preprocessorgeodatagroupdataitem_variationsettingdialog.h \
           datamodel/private/preprocessorgridattributecelldataitem_propertydialog.h \
           datamodel/private/preprocessorgridattributecellgroupdataitem_modifyopacityandupdateactorsettingscommand.h \
           datamodel/private/preprocessorgridattributenodedataitem_propertydialog.h \
           datamodel/private/preprocessorgridattributenodegroupdataitem_modifyopacityandupdateactorsettingscommand.h \
           datamodel/private/preprocessorgridcreatingconditiondataitem_impl.h \
           datamodel/private/preprocessorgriddataitem_impl.h \
           datamodel/private/preprocessorgridtypedataitem_applycolormapsettingandrendercommand.h \
           datamodel/private/preprocessorgridtypedataitem_applycolormapsettingdialog.h \
           subwindow/gridbirdeyewindow/gridbirdeyewindow.h \
           subwindow/gridbirdeyewindow/gridbirdeyewindowactionmanager.h \
           subwindow/gridbirdeyewindow/gridbirdeyewindowdatamodel.h \
           subwindow/gridbirdeyewindow/gridbirdeyewindowgraphicsview.h \
           subwindow/gridbirdeyewindow/gridbirdeyewindowgriddataitem.h \
           subwindow/gridbirdeyewindow/gridbirdeyewindowsetting.h \
           subwindow/gridcrosssectionwindow/preprocessorgridcrosssectionwindow.h \
           subwindow/gridcrosssectionwindow/preprocessorgridcrosssectionwindowdelegate.h \
           subwindow/gridcrosssectionwindow/preprocessorgridcrosssectionwindowgraphicsview.h \
           subwindow/gridcrosssectionwindow/preprocessorgridcrosssectionwindowprojectdataitem.h \
           subwindow/gridcrosssectionwindow/preprocessorgridcrosssectionwindowtableview.h \
           subwindow/gridcrosssectionwindow2/preprocessorgridcrosssectionwindow2.h \
           subwindow/gridcrosssectionwindow2/private/preprocessorgridcrosssectionwindow2_csvexportcontroller.h \
           subwindow/gridcrosssectionwindow2/private/preprocessorgridcrosssectionwindow2_displaysettingcontainer.h \
           subwindow/gridcrosssectionwindow2/private/preprocessorgridcrosssectionwindow2_displaysettingdialog.h \
           subwindow/gridcrosssectionwindow2/private/preprocessorgridcrosssectionwindow2_exportsettingdialog.h \
           subwindow/gridcrosssectionwindow2/private/preprocessorgridcrosssectionwindow2_graphicsview_colormapcontroller.h \
           subwindow/gridcrosssectionwindow2/private/preprocessorgridcrosssectionwindow2_graphicsview_drawregioninformation.h \
           subwindow/gridcrosssectionwindow2/private/preprocessorgridcrosssectionwindow2_graphicsview_elevationchartcontroller.h \
           subwindow/gridcrosssectionwindow2/private/preprocessorgridcrosssectionwindow2_graphicsview_independentchartcontroller.h \
           subwindow/gridcrosssectionwindow2/private/preprocessorgridcrosssectionwindow2_snapshotsavecontroller.h \
           subwindow/gridcrosssectionwindow2/private/preprocessorgridcrosssectionwindow2_updategraphicsviewcommand.h \
           subwindow/structured15dgridwithcrosssectioncrosssectionwindow/structured15dgridwithcrosssectioncrosssectionaltitudemovedialog.h \
           subwindow/structured15dgridwithcrosssectioncrosssectionwindow/structured15dgridwithcrosssectioncrosssectionwindow.h \
           subwindow/structured15dgridwithcrosssectioncrosssectionwindow/structured15dgridwithcrosssectioncrosssectionwindowdelegate.h \
           subwindow/structured15dgridwithcrosssectioncrosssectionwindow/structured15dgridwithcrosssectioncrosssectionwindowgraphicsview.h \
           subwindow/structured15dgridwithcrosssectioncrosssectionwindow/structured15dgridwithcrosssectioncrosssectionwindowprojectdataitem.h \
           subwindow/structured15dgridwithcrosssectioncrosssectionwindow/structured15dgridwithcrosssectioncrosssectionwindowtableview.h \
           subwindow/gridbirdeyewindow/private/gridbirdeyewindowgriddataitem_editdisplaysettingdialog.h \
           subwindow/gridbirdeyewindow/private/gridbirdeyewindowgriddataitem_updateactorsettingcommand.h \
           subwindow/gridbirdeyewindow/private/gridbirdeyewindowgriddataitem_updatedisplaysettingcommand.h \
           subwindow/gridcrosssectionwindow2/private/preprocessorgridcrosssectionwindow2_controller.h \
           subwindow/gridcrosssectionwindow2/private/preprocessorgridcrosssectionwindow2_displaysettingtablecontroller.h \
           subwindow/gridcrosssectionwindow2/private/preprocessorgridcrosssectionwindow2_displaysettingtablecontroller_delegate.h \
           subwindow/gridcrosssectionwindow2/private/preprocessorgridcrosssectionwindow2_edittablecontroller.h \
           subwindow/gridcrosssectionwindow2/private/preprocessorgridcrosssectionwindow2_edittablecontroller_delegate.h \
           subwindow/gridcrosssectionwindow2/private/preprocessorgridcrosssectionwindow2_gridattributedisplaysettingcontainer.h \
           subwindow/gridcrosssectionwindow2/private/preprocessorgridcrosssectionwindow2_gridattributedisplaysettingeditdialog.h \
           subwindow/gridcrosssectionwindow2/private/preprocessorgridcrosssectionwindow2_impl.h \
           subwindow/gridcrosssectionwindow2/private/preprocessorgridcrosssectionwindow2_updatecommand.h \
           subwindow/gridcrosssectionwindow2/public/preprocessorgridcrosssectionwindow2_graphicsview.h
FORMS += bc/boundaryconditiondialog.ui \
         datamodel/preprocessorbcgroupsettingdialog.ui \
         datamodel/preprocessorgridattributecelldisplaysettingdialog.ui \
         datamodel/preprocessorgridattributecustommappingdialog.ui \
         datamodel/preprocessorgridattributemappingsettingtopeditdialog.ui \
         datamodel/preprocessorgridattributetemplatemappingsettingdialog.ui \
         datamodel/preprocessorgridcreatingconditionalgorithmselectdialog.ui \
         misc/preprocessorlegendboxeditdialog.ui \
         datamodel/gridshape/preprocessorgridshapedeltadialog.ui \
         datamodel/gridshape/preprocessorgridshapenewpositiondialog.ui \
         datamodel/private/preprocessorgeodatagroupdataitem_colormapsettingeditdialog.ui \
         datamodel/private/preprocessorgeodatagroupdataitem_variationsettingdialog.ui \
         datamodel/private/preprocessorgridattributecelldataitem_propertydialog.ui \
         datamodel/private/preprocessorgridattributenodedataitem_propertydialog.ui \
         subwindow/gridcrosssectionwindow/preprocessorgridcrosssectionwindow.ui \
         subwindow/gridcrosssectionwindow2/preprocessorgridcrosssectionwindow2.ui \
         subwindow/gridcrosssectionwindow2/private/preprocessorgridcrosssectionwindow2_displaysettingdialog.ui \
         subwindow/gridcrosssectionwindow2/private/preprocessorgridcrosssectionwindow2_exportsettingdialog.ui \
         subwindow/structured15dgridwithcrosssectioncrosssectionwindow/structured15dgridwithcrosssectioncrosssectionaltitudemovedialog.ui \
         subwindow/structured15dgridwithcrosssectioncrosssectionwindow/structured15dgridwithcrosssectioncrosssectionwindow.ui \
         subwindow/gridbirdeyewindow/private/gridbirdeyewindowgriddataitem_editdisplaysettingdialog.ui \
         subwindow/gridcrosssectionwindow2/private/preprocessorgridcrosssectionwindow2_controller.ui \
         subwindow/gridcrosssectionwindow2/private/preprocessorgridcrosssectionwindow2_gridattributedisplaysettingeditdialog.ui
SOURCES += preobjectbrowser.cpp \
           preobjectbrowserview.cpp \
           preprocessordatamodel.cpp \
           preprocessorgraphicsview.cpp \
           preprocessorwindow.cpp \
           preprocessorwindowactionmanager.cpp \
           preprocessorwindowprojectdataitem.cpp \
           prepropertybrowser.cpp \
           bc/boundaryconditiondialog.cpp \
           datamodel/attributebrowserhelper.cpp \
           datamodel/graphicssupport.cpp \
           datamodel/preprocessorbackgroundimagedataitem.cpp \
           datamodel/preprocessorbackgroundimagesdataitem.cpp \
           datamodel/preprocessorbcdataitem.cpp \
           datamodel/preprocessorbcgroupdataitem.cpp \
           datamodel/preprocessorbcgroupsettingdialog.cpp \
           datamodel/preprocessorbcsettingdataitem.cpp \
           datamodel/preprocessorbcsettinggroupdataitem.cpp \
           datamodel/preprocessorcustommappingsetting.cpp \
           datamodel/preprocessorgeodatacomplexgroupdataitem.cpp \
           datamodel/preprocessorgeodatadataitem.cpp \
           datamodel/preprocessorgeodatagroupdataitem.cpp \
           datamodel/preprocessorgeodatatopdataitem.cpp \
           datamodel/preprocessorgeodatavariationdataitem.cpp \
           datamodel/preprocessorgridandgridcreatingconditiondataitem.cpp \
           datamodel/preprocessorgridattributecelldataitem.cpp \
           datamodel/preprocessorgridattributecelldisplaysettingdialog.cpp \
           datamodel/preprocessorgridattributecellgroupdataitem.cpp \
           datamodel/preprocessorgridattributecustommappingdialog.cpp \
           datamodel/preprocessorgridattributemappingsettingdataitem.cpp \
           datamodel/preprocessorgridattributemappingsettingtopdataitem.cpp \
           datamodel/preprocessorgridattributemappingsettingtopeditdialog.cpp \
           datamodel/preprocessorgridattributenodedataitem.cpp \
           datamodel/preprocessorgridattributenodegroupdataitem.cpp \
           datamodel/preprocessorgridattributetemplatemappingsettingdialog.cpp \
           datamodel/preprocessorgridcreatingconditionalgorithmselectdialog.cpp \
           datamodel/preprocessorgridcreatingconditiondataitem.cpp \
           datamodel/preprocessorgriddataitem.cpp \
           datamodel/preprocessorgridshapedataitem.cpp \
           datamodel/preprocessorgridtypedataitem.cpp \
           datamodel/preprocessorhydraulicdatadataitem.cpp \
           datamodel/preprocessorhydraulicdatagroupdataitem.cpp \
           datamodel/preprocessorhydraulicdatatopdataitem.cpp \
           datamodel/preprocessorinputconditiondataitem.cpp \
           datamodel/preprocessormeasureddatatopdataitem.cpp \
           datamodel/preprocessornormal15dgridwithcrosssectiondataitem.cpp \
           datamodel/preprocessornormal15dgridwithcrosssectionshapedataitem.cpp \
           datamodel/preprocessorrootdataitem.cpp \
           datamodel/preprocessorstructured2dgriddataitem.cpp \
           datamodel/preprocessorstructured2dgridshapedataitem.cpp \
           datamodel/preprocessorunstructured2dgriddataitem.cpp \
           datamodel/preprocessorunstructured2dgridshapedataitem.cpp \
           factory/geodatafactorysetup.cpp \
           factory/gridcreatingconditionfactory.cpp \
           factory/gridexporterfactory.cpp \
           factory/gridimporterfactory.cpp \
           factory/hydraulicdatafactory.cpp \
           factory/preprocessorgriddataitemfactory.cpp \
           gridexporter/cgnsgridexporter.cpp \
           gridimporter/cgnsgridimporter.cpp \
           gridimporter/projectgridimporter.cpp \
           gridimporter/structured15dgridwithcrosssectioncgnsimporter.cpp \
           gridimporter/structured15dgridwithcrosssectionprojectimporter.cpp \
           gridimporter/structured2dgridcgnsimporter.cpp \
           gridimporter/structured2dgridprojectimporter.cpp \
           gridimporter/unstructured2dgridcgnsimporter.cpp \
           gridimporter/unstructured2dgridprojectimporter.cpp \
           misc/gridattributegeneratorlauncher.cpp \
           misc/preprocessorgridattributemappingmode.cpp \
           misc/preprocessorlegendboxeditdialog.cpp \
           datamodel/gridshape/preprocessorgridshapedeltadialog.cpp \
           datamodel/gridshape/preprocessorgridshapenewpositiondialog.cpp \
           datamodel/private/preprocessorbcgroupdataitem_setsettingcommand.cpp \
           datamodel/private/preprocessorgeodatagroupdataitem_colormapsettingeditdialog.cpp \
           datamodel/private/preprocessorgeodatagroupdataitem_variationsettingdialog.cpp \
           datamodel/private/preprocessorgridattributecelldataitem_propertydialog.cpp \
           datamodel/private/preprocessorgridattributecellgroupdataitem_modifyopacityandupdateactorsettingscommand.cpp \
           datamodel/private/preprocessorgridattributenodedataitem_propertydialog.cpp \
           datamodel/private/preprocessorgridattributenodegroupdataitem_modifyopacityandupdateactorsettingscommand.cpp \
           datamodel/private/preprocessorgridtypedataitem_applycolormapsettingandrendercommand.cpp \
           datamodel/private/preprocessorgridtypedataitem_applycolormapsettingdialog.cpp \
           subwindow/gridbirdeyewindow/gridbirdeyewindow.cpp \
           subwindow/gridbirdeyewindow/gridbirdeyewindowactionmanager.cpp \
           subwindow/gridbirdeyewindow/gridbirdeyewindowdatamodel.cpp \
           subwindow/gridbirdeyewindow/gridbirdeyewindowgraphicsview.cpp \
           subwindow/gridbirdeyewindow/gridbirdeyewindowgriddataitem.cpp \
           subwindow/gridbirdeyewindow/gridbirdeyewindowsetting.cpp \
           subwindow/gridcrosssectionwindow/preprocessorgridcrosssectionwindow.cpp \
           subwindow/gridcrosssectionwindow/preprocessorgridcrosssectionwindowdelegate.cpp \
           subwindow/gridcrosssectionwindow/preprocessorgridcrosssectionwindowgraphicsview.cpp \
           subwindow/gridcrosssectionwindow/preprocessorgridcrosssectionwindowprojectdataitem.cpp \
           subwindow/gridcrosssectionwindow/preprocessorgridcrosssectionwindowtableview.cpp \
           subwindow/gridcrosssectionwindow2/preprocessorgridcrosssectionwindow2.cpp \
           subwindow/gridcrosssectionwindow2/private/preprocessorgridcrosssectionwindow2_csvexportcontroller.cpp \
           subwindow/gridcrosssectionwindow2/private/preprocessorgridcrosssectionwindow2_displaysettingcontainer.cpp \
           subwindow/gridcrosssectionwindow2/private/preprocessorgridcrosssectionwindow2_displaysettingdialog.cpp \
           subwindow/gridcrosssectionwindow2/private/preprocessorgridcrosssectionwindow2_exportsettingdialog.cpp \
           subwindow/gridcrosssectionwindow2/private/preprocessorgridcrosssectionwindow2_graphicsview_colormapcontroller.cpp \
           subwindow/gridcrosssectionwindow2/private/preprocessorgridcrosssectionwindow2_graphicsview_drawregioninformation.cpp \
           subwindow/gridcrosssectionwindow2/private/preprocessorgridcrosssectionwindow2_graphicsview_elevationchartcontroller.cpp \
           subwindow/gridcrosssectionwindow2/private/preprocessorgridcrosssectionwindow2_graphicsview_independentchartcontroller.cpp \
           subwindow/gridcrosssectionwindow2/private/preprocessorgridcrosssectionwindow2_snapshotsavecontroller.cpp \
           subwindow/gridcrosssectionwindow2/private/preprocessorgridcrosssectionwindow2_updategraphicsviewcommand.cpp \
           subwindow/structured15dgridwithcrosssectioncrosssectionwindow/structured15dgridwithcrosssectioncrosssectionaltitudemovedialog.cpp \
           subwindow/structured15dgridwithcrosssectioncrosssectionwindow/structured15dgridwithcrosssectioncrosssectionwindow.cpp \
           subwindow/structured15dgridwithcrosssectioncrosssectionwindow/structured15dgridwithcrosssectioncrosssectionwindowdelegate.cpp \
           subwindow/structured15dgridwithcrosssectioncrosssectionwindow/structured15dgridwithcrosssectioncrosssectionwindowgraphicsview.cpp \
           subwindow/structured15dgridwithcrosssectioncrosssectionwindow/structured15dgridwithcrosssectioncrosssectionwindowprojectdataitem.cpp \
           subwindow/structured15dgridwithcrosssectioncrosssectionwindow/structured15dgridwithcrosssectioncrosssectionwindowtableview.cpp \
           subwindow/gridbirdeyewindow/private/gridbirdeyewindowgriddataitem_editdisplaysettingdialog.cpp \
           subwindow/gridbirdeyewindow/private/gridbirdeyewindowgriddataitem_updateactorsettingcommand.cpp \
           subwindow/gridbirdeyewindow/private/gridbirdeyewindowgriddataitem_updatedisplaysettingcommand.cpp \
           subwindow/gridcrosssectionwindow2/private/preprocessorgridcrosssectionwindow2_controller.cpp \
           subwindow/gridcrosssectionwindow2/private/preprocessorgridcrosssectionwindow2_displaysettingtablecontroller.cpp \
           subwindow/gridcrosssectionwindow2/private/preprocessorgridcrosssectionwindow2_displaysettingtablecontroller_delegate.cpp \
           subwindow/gridcrosssectionwindow2/private/preprocessorgridcrosssectionwindow2_edittablecontroller.cpp \
           subwindow/gridcrosssectionwindow2/private/preprocessorgridcrosssectionwindow2_edittablecontroller_delegate.cpp \
           subwindow/gridcrosssectionwindow2/private/preprocessorgridcrosssectionwindow2_gridattributedisplaysettingcontainer.cpp \
           subwindow/gridcrosssectionwindow2/private/preprocessorgridcrosssectionwindow2_gridattributedisplaysettingeditdialog.cpp \
           subwindow/gridcrosssectionwindow2/private/preprocessorgridcrosssectionwindow2_impl.cpp \
           subwindow/gridcrosssectionwindow2/private/preprocessorgridcrosssectionwindow2_updatecommand.cpp \
           subwindow/gridcrosssectionwindow2/public/preprocessorgridcrosssectionwindow2_graphicsview.cpp
RESOURCES += pre.qrc
TRANSLATIONS += languages/iricPre_ar_EG.ts \
                languages/iricPre_bg_BG.ts \
                languages/iricPre_bs_BA.ts \
                languages/iricPre_ca_ES.ts \
                languages/iricPre_cs_CZ.ts \
                languages/iricPre_da_DK.ts \
                languages/iricPre_de_DE.ts \
                languages/iricPre_el_GR.ts \
                languages/iricPre_es_ES.ts \
                languages/iricPre_et_EE.ts \
                languages/iricPre_eu_ES.ts \
                languages/iricPre_fi_FI.ts \
                languages/iricPre_fr_FR.ts \
                languages/iricPre_gl_ES.ts \
                languages/iricPre_hi_IN.ts \
                languages/iricPre_hu_HU.ts \
                languages/iricPre_id_ID.ts \
                languages/iricPre_is_IS.ts \
                languages/iricPre_it_IT.ts \
                languages/iricPre_ja_JP.ts \
                languages/iricPre_ko_KR.ts \
                languages/iricPre_ky_KG.ts \
                languages/iricPre_lt_LT.ts \
                languages/iricPre_lv_LV.ts \
                languages/iricPre_nb_NO.ts \
                languages/iricPre_nl_NL.ts \
                languages/iricPre_pl_PL.ts \
                languages/iricPre_pt_BR.ts \
                languages/iricPre_pt_PT.ts \
                languages/iricPre_ro_RO.ts \
                languages/iricPre_ru_RU.ts \
                languages/iricPre_sl_SI.ts \
                languages/iricPre_sv_SE.ts \
                languages/iricPre_th_TH.ts \
                languages/iricPre_tr_TR.ts \
                languages/iricPre_uk_UA.ts \
                languages/iricPre_vi_VN.ts \
                languages/iricPre_zh_CN.ts \
                languages/iricPre_zh_TW.ts

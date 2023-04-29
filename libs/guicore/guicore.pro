TARGET = iricGuicore
TEMPLATE = lib


DEFINES += GUICORE_LIBRARY
DEFINES += ANSI_DECLARATORS
win32 {
	DEFINES += YAML_CPP_DLL
}

include( ../../paths.pri )

QT += network qml widgets xml

# Use Precompiled headers (PCH)
PRECOMPILED_HEADER = guicore_pch.h

######################
# Internal libraries #
######################

# iricCs

unix {
	LIBS += -L"../cs"
}
LIBS += -liricCs

# iricGuibase

unix{
LIBS += -L"../guibase"
}

LIBS += -liricGuibase

# iricGeoIo

unix{
LIBS += -L"../geoio"
}

LIBS += -liricGeoio

# iricTmsLoader

unix {
	LIBS += -L"../tmsloader"
}
LIBS += -liricTmsLoader

# iricMisc

unix {
	LIBS += -L"../misc"
}
LIBS += -liricMisc

# iricTriangle

unix {
	LIBS += -L"../triangle"
}
LIBS += -liricTriangle

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

# yaml-cpp

win32 {
	LIBS += -lyaml-cpp
}
unix {
	LIBS += -lyaml-cpp
}

# Qwt

CONFIG(debug, debug|release) {
	win32 {
		LIBS += -lqwtd
	}
	unix {
		LIBS += -lqwt
	}
}
else {
	LIBS += -lqwt
	DEFINES += QT_NO_DEBUG_OUTPUT
	DEFINES += QT_NO_WARNING_OUTPUT
}

# VTK

LIBS += \
	-lvtkCommonCore-$${VTK_MAJ_MIN} \
	-lvtkCommonDataModel-$${VTK_MAJ_MIN} \
	-lvtkCommonExecutionModel-$${VTK_MAJ_MIN} \
	-lvtkCommonMisc-$${VTK_MAJ_MIN} \
	-lvtkCommonTransforms-$${VTK_MAJ_MIN} \
	-lvtkFiltersCore-$${VTK_MAJ_MIN} \
	-lvtkFiltersGeometry-$${VTK_MAJ_MIN} \
	-lvtkFiltersGeneral-$${VTK_MAJ_MIN} \
	-lvtkFiltersExtraction-$${VTK_MAJ_MIN} \
	-lvtkFiltersSources-$${VTK_MAJ_MIN} \
	-lvtkFiltersTexture-$${VTK_MAJ_MIN} \
	-lvtkGUISupportQt-$${VTK_MAJ_MIN} \
	-lvtkInteractionStyle-$${VTK_MAJ_MIN} \
	-lvtkInteractionWidgets-$${VTK_MAJ_MIN} \
	-lvtkIOImage-$${VTK_MAJ_MIN} \
	-lvtkIOCore-$${VTK_MAJ_MIN} \
	-lvtkIOLegacy-$${VTK_MAJ_MIN} \
	-lvtkRenderingAnnotation-$${VTK_MAJ_MIN} \
	-lvtkRenderingCore-$${VTK_MAJ_MIN} \
	-lvtkRenderingFreeType-$${VTK_MAJ_MIN} \
	-lvtkRenderingLabel-$${VTK_MAJ_MIN} \
	-lvtkRenderingLOD-$${VTK_MAJ_MIN} \
	-lvtkRenderingOpenGL2-$${VTK_MAJ_MIN} \
	-lvtkRenderingQt-$${VTK_MAJ_MIN}


# hdf5
win32 {
	CONFIG(debug, debug|release) {
		LIBS += -lhdf5_D
	} else {
		LIBS += -lhdf5
	}
}

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

win32 {
	DESTDIR = $(SolutionDir)/libdlls/$(Configuration)
	LIBS += -L$(SolutionDir)/libdlls/$(Configuration)
}

# Input
HEADERS += guicore_global.h \
           arrows/arrowscolorsettingeditsmallwidget.h \
           arrows/arrowscolorsettingeditwidget.h \
           arrows/arrowslegendsettingcontainer.h \
           arrows/arrowslegendsettingeditwidget.h \
           arrows/arrowslengthsettingeditwidget.h \
           arrows/arrowssettingcontainer.h \
           arrows/arrowssettingeditwidget.h \
           arrows/arrowssettingtoolbarwidget.h \
           arrows/arrowsshapesettingeditwidget.h \
           base/additionalmenuwindowi.h \
           base/animationcontrollerinterface.h \
           base/clipboardoperatablewindowinterface.h \
           base/iricmainwindowinterface.h \
           base/propertybrowser.h \
           base/snapshotenabledwindowinterface.h \
           base/windowwithobjectbrowserinterface.h \
           base/windowwithpropertybrowser.h \
           base/windowwithtmsi.h \
           base/windowwithzindexinterface.h \
           bgcolor/backgroundcoloreditinterface.h \
           bgcolor/backgroundcoloreditvtkinterface.h \
           contour/contoursettingcontainer.h \
           contour/contoursettingeditwidget.h \
           datamodel/attributebrowsertargetdataitem.h \
           datamodel/geodataproxydataitem.h \
           datamodel/graphics2dwindowdatamodel.h \
           datamodel/graphics3dwindowdatamodel.h \
           datamodel/graphicswindowdataitem.h \
           datamodel/graphicswindowdataitemstandarditemchangecommand.h \
           datamodel/graphicswindowdataitemupdateactorsettingdialog.h \
           datamodel/graphicswindowdatamodel.h \
           datamodel/graphicswindowdrawcommands.h \
           datamodel/graphicswindowrootdataitem.h \
           datamodel/graphicswindowsimpledatamodel.h \
           datamodel/modifycommandandrenderdialog.h \
           datamodel/propertybrowserattribute.h \
           datamodel/propertybrowserview.h \
           datamodel/vtk2dgraphicsview.h \
           datamodel/vtk3dgraphicsview.h \
           datamodel/vtkgraphicsview.h \
           executer/iricmainwindowexecuterwatcher.h \
           filter/generalfilteringsettingcontainer.h \
           filter/generalfilteringsettingeditwidget.h \
           filter/structured2dfilteringsettingcontainer.h \
           filter/structured2dfilteringsettingeditwidget.h \
           filter/structured3dfilteringsettingcontainer.h \
           filter/structured3dfilteringsettingeditwidget.h \
           filter/structuredgridskiporsubdividesettingcontainer.h \
           filter/structuredgridskiporsubdividesettingeditwidget.h \
           image/imagesettingcontainer.h \
           image/imagesettingwidget.h \
           misc/cgnsutil.h \
           misc/iricmetadata.h \
           misc/mouseboundingbox.h \
           misc/mousepositionwidget.h \
           misc/valuerangecontainer.h \
           misc/valuerangecontainerutil.h \
           misc/vtkpointsetandvaluerangeset.h \
           misc/vtkpointsetandvaluerangesett.h \
           misc/vtkpointsetvaluerangeset.h \
           named/namedgraphicswindowdataitemtool.h \
           named/namedgraphicwindowdataitem.h \
           post/postprocessorwindow.h \
           post/postprocessorwindowfactoryinterface.h \
           post/postprocessorwindowprojectdataitem.h \
           post/postzonedataitem.h \
           post/postzoneselectingdialog.h \
           postcontainer/postabstractsteps.h \
           postcontainer/postbaseiterativedatacontainer.h \
           postcontainer/postbaseiterativeintegerdatacontainer.h \
           postcontainer/postbaseiterativenumericaldatacontainer.h \
           postcontainer/postbaseiterativerealdatacontainer.h \
           postcontainer/postbaseiterativeseriesdatacontainer.h \
           postcontainer/postbaseiterativestringdatacontainer.h \
           postcontainer/postbaseselectingdialog.h \
           postcontainer/postcalculatedresult.h \
           postcontainer/postcalculatedresultargument.h \
           postcontainer/postcalculatedresultargumenteditdialog.h \
           postcontainer/postcalculatedresulteditdialog.h \
           postcontainer/postcalculatedresultlistdialog.h \
           postcontainer/postdatacontainer.h \
           postcontainer/postdataexportdialog.h \
           postcontainer/postexportsetting.h \
           postcontainer/postiterationsteps.h \
           postcontainer/postseriesdatacontainer.h \
           postcontainer/postsolutioninfo.h \
           postcontainer/poststringresult.h \
           postcontainer/poststringresultargument.h \
           postcontainer/poststringresultargumenteditdialog.h \
           postcontainer/poststringresulteditwidget.h \
           postcontainer/posttimesteps.h \
           postcontainer/postzonedatacontainer.h \
           postcontainer/postzonepointseriesdatacontainer.h \
           project/addiblegcptablemodel.h \
           project/backgroundimageinfo.h \
           project/backgroundimageinfodialog.h \
           project/backgroundimageinfogeoreferencedialog.h \
           project/colorsource.h \
           project/gcptablemodel.h \
           project/gcptablerow.h \
           project/georeferenceview.h \
           project/offsetsettingdialog.h \
           project/projectcgnsfile.h \
           project/projectdata.h \
           project/projectdataitem.h \
           project/projectmainfile.h \
           project/projectpostprocessors.h \
           project/projecttimesettingdialog.h \
           project/projectworkspace.h \
           project/workfolderinfo.h \
           region/region2dsettingcontainer.h \
           region/region2dsettingeditwidget.h \
           region/regionrange2dsettingcontainer.h \
           region/regionrange2dsettingeditwidget.h \
           scalarstocolors/colormapenumeratefactory.h \
           scalarstocolors/colormapenumeratelegendsettingcontainer.h \
           scalarstocolors/colormapenumeratelegendsettingeditwidget.h \
           scalarstocolors/colormapenumeratesettingcontainer.h \
           scalarstocolors/colormapenumeratesettingeditwidget.h \
           scalarstocolors/colormapfactory.h \
           scalarstocolors/colormapfactoryi.h \
           scalarstocolors/colormaplegendsettingcontainer.h \
           scalarstocolors/colormaplegendsettingcontaineri.h \
           scalarstocolors/colormaplegendsettingeditwidget.h \
           scalarstocolors/colormapsettingcontainer.h \
           scalarstocolors/colormapsettingcontaineri.h \
           scalarstocolors/colormapsettingcontainerutil.h \
           scalarstocolors/colormapsettingeditdialog.h \
           scalarstocolors/colormapsettingeditwidget.h \
           scalarstocolors/colormapsettingeditwidgeti.h \
           scalarstocolors/colormapsettingeditwidgetwithimportexportbutton.h \
           scalarstocolors/colormapsettingmodifycommand.h \
           scalarstocolors/colormapsettingtoolbarwidget.h \
           scalarstocolors/colormapsettingtoolbarwidgetcontroller.h \
           scalarstocolors/colormapsettingvaluecolorpaircontainer.h \
           scalarstocolors/delegatedcolormapsettingcontainer.h \
           scalarstocolors/delegatedcolormapsettingeditwidget.h \
           scalarstocolors/discretevaluesutil.h \
           solverdef/enumloader.h \
           solverdef/enumloadert.h \
           solverdef/integerenumloader.h \
           solverdef/realenumloader.h \
           solverdef/solverdefinition.h \
           solverdef/solverdefinitionabstract.h \
           solverdef/solverdefinitionboundarycondition.h \
           solverdef/solverdefinitiongridattribute.h \
           solverdef/solverdefinitiongridattributedimension.h \
           solverdef/solverdefinitiongridattributedimensioncreator.h \
           solverdef/solverdefinitiongridattributedimensiont.h \
           solverdef/solverdefinitiongridattributeinteger.h \
           solverdef/solverdefinitiongridattributeintegercell.h \
           solverdef/solverdefinitiongridattributeintegerdimension.h \
           solverdef/solverdefinitiongridattributeintegernode.h \
           solverdef/solverdefinitiongridattributeintegeroptioncell.h \
           solverdef/solverdefinitiongridattributeintegeroptionnode.h \
           solverdef/solverdefinitiongridattributereal.h \
           solverdef/solverdefinitiongridattributerealcell.h \
           solverdef/solverdefinitiongridattributerealdimension.h \
           solverdef/solverdefinitiongridattributerealnode.h \
           solverdef/solverdefinitiongridattributerealoptioncell.h \
           solverdef/solverdefinitiongridattributerealoptionnode.h \
           solverdef/solverdefinitiongridattributet.h \
           solverdef/solverdefinitiongridcomplexattribute.h \
           solverdef/solverdefinitiongridtype.h \
           solverdef/solverdefinitionnode.h \
           solverdef/solverdefinitionoutput.h \
           solverdef/solverdefinitiontranslator.h \
           tmsimage/tmsimagedataitem.h \
           tmsimage/tmsimagegroupdataitem.h \
           tmsimage/tmsimagesetting.h \
           tmsimage/tmsimagesettingmanager.h \
           tmsimage/tmsregionselectwidget.h \
           arrows/private/arrowslegendsettingcontainer_imagebuilder.h \
           arrows/private/arrowssettingeditwidget_modifycommand.h \
           bgcolor/private/backgroundcoloreditinterface_setbackgroundcolorcommand.h \
           datamodel/private/graphicswindowdataitem_modifycommand.h \
           datamodel/private/graphicswindowdataitem_rendercommand.h \
           datamodel/private/graphicswindowdataitem_renderredoonlycommand.h \
           datamodel/private/graphicswindowdataitem_standarditemmodifycommand.h \
           datamodel/private/graphicswindowdataitem_updateactorsettingcommand.h \
           datamodel/private/graphicswindowrootdataitem_movedowncommand.h \
           datamodel/private/graphicswindowrootdataitem_moveupcommand.h \
           datamodel/private/vtkgraphicsview_impl.h \
           image/private/imagesettingcontainer_movecommand.h \
           image/private/imagesettingcontainer_resizecommand.h \
           image/private/imagesettingcontainer_updatecommand.h \
           image/public/imagesettingcontainer_controller.h \
           image/public/imagesettingcontainer_imagebuilder.h \
           misc/private/cgnsutil_detail.h \
           misc/private/vtkpointsetandvaluerangesett_detail.h \
           misc/targeted/targeteditemi.h \
           misc/targeted/targeteditemsettargetcommand.h \
           misc/targeted/targeteditemsettargetcommandtool.h \
           postcontainer/exporter/postzonedatacsvexporter.h \
           postcontainer/exporter/postzonedataexporter.h \
           postcontainer/exporter/postzonedatashapeexporter.h \
           postcontainer/exporter/postzonedatatpoexporter.h \
           postcontainer/exporter/postzonedatavtkexporter.h \
           postcontainer/private/postcalculatedresulteditdialog_tabledelegate.h \
           postcontainer/private/poststringresult_impl.h \
           postcontainer/private/postzonedatacontainer_particlegrouploader.h \
           postcontainer/private/postzonedatacontainer_particleloader.h \
           postcontainer/private/postzonedatacontainer_polydataloader.h \
           pre/base/preprocessordataitem.h \
           pre/base/preprocessordatamodelinterface.h \
           pre/base/preprocessorgeodatacomplexgroupdataiteminterface.h \
           pre/base/preprocessorgeodatadataiteminterface.h \
           pre/base/preprocessorgeodatagroupdataiteminterface.h \
           pre/base/preprocessorgeodatatopdataiteminterface.h \
           pre/base/preprocessorgraphicsviewinterface.h \
           pre/base/preprocessorgridandgridcreatingconditiondataiteminterface.h \
           pre/base/preprocessorgridcreatingconditiondataiteminterface.h \
           pre/base/preprocessorgriddataiteminterface.h \
           pre/base/preprocessorgridtypedataiteminterface.h \
           pre/base/preprocessorhydraulicdatadataiteminterface.h \
           pre/base/preprocessorhydraulicdatagroupdataiteminterface.h \
           pre/base/preprocessorhydraulicdatatopdataiteminterface.h \
           pre/base/preprocessorwindowinterface.h \
           pre/complex/gridcomplexconditiondialog.h \
           pre/complex/gridcomplexconditiongroup.h \
           pre/complex/gridcomplexconditiongroupeditdialog.h \
           pre/complex/gridcomplexconditiongroupeditwidgetcontainer.h \
           pre/complex/gridcomplexconditiongrouprealeditwidget.h \
           pre/complex/gridcomplexconditionwidget.h \
           pre/complex/gridcomplexconditionwidgetcontainer.h \
           pre/geodata/geodata.h \
           pre/geodata/geodatacellmappert.h \
           pre/geodata/geodatacreator.h \
           pre/geodata/geodataexporter.h \
           pre/geodata/geodatafactory.h \
           pre/geodata/geodataimporter.h \
           pre/geodata/geodatamapper.h \
           pre/geodata/geodatamappersettingi.h \
           pre/geodata/geodatamappert.h \
           pre/geodata/geodatanodemappert.h \
           pre/geodata/geodataproxy.h \
           pre/geodata/geodatawebimporter.h \
           pre/geodata/geodatawithsinglemapper.h \
           pre/geodatabackground/geodatabackground.h \
           pre/geodatabackground/geodatabackgroundcellmappert.h \
           pre/geodatabackground/geodatabackgroundcomplex.h \
           pre/geodatabackground/geodatabackgroundcomplexcreator.h \
           pre/geodatabackground/geodatabackgroundcreatort.h \
           pre/geodatabackground/geodatabackgroundeditdialog.h \
           pre/geodatabackground/geodatabackgroundintegercreator.h \
           pre/geodatabackground/geodatabackgroundnodemappert.h \
           pre/geodatabackground/geodatabackgroundrealcreator.h \
           pre/grid/grid.h \
           pre/grid/grid2d.h \
           pre/grid/grid3d.h \
           pre/grid/gridcgnsestimater.h \
           pre/grid/gridexporterinterface.h \
           pre/grid/gridimporterinterface.h \
           pre/grid/gridinternalexporter.h \
           pre/grid/gridinternalimporter.h \
           pre/grid/structured15dgridwithcrosssection.h \
           pre/grid/structured2dgrid.h \
           pre/grid/unstructured2dgrid.h \
           pre/gridcreatingcondition/gridcreatingcondition.h \
           pre/gridcreatingcondition/gridcreatingconditioncreator.h \
           pre/gridcreatingcondition/gridcreatingconditionfactoryi.h \
           pre/gridcreatingcondition/gridcreatingconditionio.h \
           pre/hydraulicdata/hydraulicdata.h \
           pre/hydraulicdata/hydraulicdatacreator.h \
           pre/hydraulicdata/hydraulicdataexporter.h \
           pre/hydraulicdata/hydraulicdataimporter.h \
           pre/hydraulicdata/hydraulicdatamultiexporter.h \
           project/inputcond/inputconditioncontainer.h \
           project/inputcond/inputconditioncontainerfunctional.h \
           project/inputcond/inputconditioncontainerinteger.h \
           project/inputcond/inputconditioncontainerreal.h \
           project/inputcond/inputconditioncontainerset.h \
           project/inputcond/inputconditioncontainerstring.h \
           project/inputcond/inputconditiondependency.h \
           project/inputcond/inputconditiondialog.h \
           project/inputcond/inputconditionpage.h \
           project/inputcond/inputconditionpagecontainer.h \
           project/inputcond/inputconditionpagelist.h \
           project/inputcond/inputconditionwidget.h \
           project/inputcond/inputconditionwidgetfilename.h \
           project/inputcond/inputconditionwidgetset.h \
           project/measured/measureddata.h \
           project/measured/measureddatacsvexporter.h \
           project/measured/measureddatacsvimporter.h \
           project/measured/measureddataexporteri.h \
           project/measured/measureddataimporteri.h \
           project/private/backgroundimageinfo_impl.h \
           project/private/backgroundimageinfo_setactorpropertycommand.h \
           project/private/backgroundimageinfo_setting.h \
           project/private/backgroundimageinfogeoreferencedialog_styleditemdelegate.h \
           project/private/cgnsfileentry_impl.h \
           project/private/colorsource_impl.h \
           project/private/georeferenceview_imageinfo.h \
           project/private/projectdatacopythread.h \
           project/private/projectdataitem_impl.h \
           project/private/projectdatamovethread.h \
           project/private/projectmainfile_impl.h \
           project/private/projectworkspace_impl.h \
           project/private/workfolderinfo_impl.h \
           project/util/gcptablegeoreferenceviewhelper.h \
           project/util/georeferenceview_imageinfo_destinationoperationwaitingselectionhelper.h \
           project/util/georeferenceview_imageinfo_initselectionhelper.h \
           project/util/georeferenceview_imageinfo_pointsinrectangleselectionhelper.h \
           project/util/georeferenceview_imageinfo_selectionhelper.h \
           project/util/georeferenceview_imageinfo_translationselectionhelper.h \
           project/util/georeferenceview_imageinfo_voidselectionhelper.h \
           project/util/georeferenceview_imageinfo_waitingselectionhelper.h \
           project/util/georeferenceviewhelper.h \
           project/util/georeferenceviewselectionhelper.h \
           project/util/georeferenceviewselectionhelpert.h \
           project/util/georeferenceviewselectionhelpert_detail.h \
           project/util/pointsgeoreferenceviewhelper.h \
           project/util/polygongeoreferenceviewhelper.h \
           project/util/rectgeoreferenceviewhelper.h \
           project/util/selectionhelper.h \
           project/util/selectionhelpert.h \
           project/util/selectionhelpert_detail.h \
           project/util/selectionhelpert_impl.h \
           project/util/viewhelpert.h \
           project/util/viewhelpert_detail.h \
           scalarstocolors/private/colormapenumeratelegendsettingcontainer_imagebuilder.h \
           scalarstocolors/private/colormapenumeratesettingeditwidget_colortablecontroller.h \
           scalarstocolors/private/colormapenumeratesettingeditwidget_colortablecontroller_delegate.h \
           scalarstocolors/private/colormapenumeratesettingeditwidget_importdialog.h \
           scalarstocolors/private/colormapenumeratesettingeditwidget_importdialog_setting.h \
           scalarstocolors/private/colormaplegendsettingcontainer_imagebuilder.h \
           scalarstocolors/private/colormapsettingeditwidget_colortablecontroller.h \
           scalarstocolors/private/colormapsettingeditwidget_colortablecontroller_delegate.h \
           scalarstocolors/private/colormapsettingeditwidget_importdialog.h \
           scalarstocolors/private/colormapsettingeditwidget_importdialog_setting.h \
           scalarstocolors/private/colormapsettingeditwidget_switchtodiscretedialog.h \
           scalarstocolors/private/colormapsettingeditwidget_switchtodiscretedialog_delegate.h \
           solverdef/private/enumloadert_detail.h \
           solverdef/private/solverdefinition_impl.h \
           solverdef/private/solverdefinitionabstract_impl.h \
           solverdef/private/solverdefinitionboundarycondition_impl.h \
           solverdef/private/solverdefinitiongridattribute_impl.h \
           solverdef/private/solverdefinitiongridattributedimension_impl.h \
           solverdef/private/solverdefinitiongridattributedimensiont_detail.h \
           solverdef/private/solverdefinitiongridattributet_detail.h \
           solverdef/private/solverdefinitiongridcomplexattribute_impl.h \
           solverdef/private/solverdefinitiongridtype_impl.h \
           solverdef/private/solverdefinitionoutput_impl.h \
           solverdef/private/solverdefinitiontranslator_impl.h \
           tmsimage/private/tmsimagegroupdataitem_impl.h \
           tmsimage/private/tmsimagesetting_impl.h \
           tmsimage/private/tmsregionselectwidget_impl.h \
           post/post2d/base/post2dwindowgeodatadataiteminterface.h \
           post/post2d/base/post2dwindowgridtypedataiteminterface.h \
           pre/complex/private/gridcomplexconditiongroup_impl.h \
           pre/complex/private/gridcomplexconditionwidget_impl.h \
           pre/geodata/private/geodata_propertydialog.h \
           pre/geodata/private/geodata_updateactorsettingcommand.h \
           pre/geodata/private/geodatacellmappert_detail.h \
           pre/geodata/private/geodatacreator_impl.h \
           pre/geodata/private/geodataexporter_impl.h \
           pre/geodata/private/geodataimporter_impl.h \
           pre/geodata/private/geodatamapper_impl.h \
           pre/geodata/private/geodatamappert_detail.h \
           pre/geodata/private/geodatanodemappert_detail.h \
           pre/geodata/private/geodataproxy_propertydialog.h \
           pre/geodata/private/geodataproxy_updateactorsettingcommand.h \
           pre/geodatabackground/private/geodatabackground_editvaluecommand.h \
           pre/geodatabackground/private/geodatabackgroundcellmappert_detail.h \
           pre/geodatabackground/private/geodatabackgroundcreatort_detail.h \
           pre/geodatabackground/private/geodatabackgroundnodemappert_detail.h \
           pre/grid/private/grid_impl.h \
           pre/grid/structured15dgrid/structured15dgridwithcrosssectioncrosssection.h \
           pre/gridcond/base/gridattributebaseobject.h \
           pre/gridcond/base/gridattributecellcontainert.h \
           pre/gridcond/base/gridattributecontainer.h \
           pre/gridcond/base/gridattributecontainert.h \
           pre/gridcond/base/gridattributedimensioncontainer.h \
           pre/gridcond/base/gridattributedimensioncontainert.h \
           pre/gridcond/base/gridattributedimensionintegercontainer.h \
           pre/gridcond/base/gridattributedimensionrealcontainer.h \
           pre/gridcond/base/gridattributedimensionscontainer.h \
           pre/gridcond/base/gridattributedimensionselectwidget.h \
           pre/gridcond/base/gridattributeeditcommand.h \
           pre/gridcond/base/gridattributeeditdialog.h \
           pre/gridcond/base/gridattributeeditnameandvaluedialog.h \
           pre/gridcond/base/gridattributeeditvariationcommand.h \
           pre/gridcond/base/gridattributeeditwidget.h \
           pre/gridcond/base/gridattributeeditwidgetcontainer.h \
           pre/gridcond/base/gridattributeeditwidgett.h \
           pre/gridcond/base/gridattributenodecontainert.h \
           pre/gridcond/base/gridattributestringconverter.h \
           pre/gridcond/base/gridattributevariationeditdialog.h \
           pre/gridcond/base/gridattributevariationeditwidget.h \
           pre/gridcond/base/gridattributevariationeditwidgetcontainer.h \
           pre/gridcond/base/gridattributevariationeditwidgett.h \
           pre/gridcond/complex/gridcomplexattributecontainer.h \
           pre/gridcond/complex/gridcomplexattributeeditwidget.h \
           pre/gridcond/container/gridattributeintegercellcontainer.h \
           pre/gridcond/container/gridattributeintegernodecontainer.h \
           pre/gridcond/container/gridattributerealcellcontainer.h \
           pre/gridcond/container/gridattributerealnodecontainer.h \
           pre/gridcond/dimensionselectwidget/gridattributedimensioncomboboxselectwidget.h \
           pre/gridcond/dimensionselectwidget/gridattributedimensiontimesliderselectwidget.h \
           pre/gridcond/editwidget/gridattributeintegereditwidget.h \
           pre/gridcond/editwidget/gridattributeintegeroptioneditwidget.h \
           pre/gridcond/editwidget/gridattributeintegervariationeditwidget.h \
           pre/gridcond/editwidget/gridattributerealeditwidget.h \
           pre/gridcond/editwidget/gridattributerealoptioneditwidget.h \
           pre/gridcond/editwidget/gridattributerealvariationeditwidget.h \
           pre/gridcond/stringconverter/gridattributestringconverterenumerate.h \
           pre/gridcond/stringconverter/gridattributestringconverterinteger.h \
           pre/gridcond/stringconverter/gridattributestringconverterreal.h \
           pre/hydraulicdata/private/hydraulicdata_impl.h \
           pre/hydraulicdata/private/hydraulicdatacreator_impl.h \
           pre/hydraulicdata/private/hydraulicdataexporter_impl.h \
           pre/hydraulicdata/private/hydraulicdataimporter_impl.h \
           pre/hydraulicdata/private/hydraulicdatamultiexporter_impl.h \
           project/inputcond/private/inputconditioncgnsfile.h \
           project/inputcond/private/inputconditioncgnsfileselectdialog.h \
           project/inputcond/private/inputconditioncontainer_impl.h \
           project/inputcond/private/inputconditioncontainerfunctional_impl.h \
           project/inputcond/private/inputconditioncontainerinteger_impl.h \
           project/inputcond/private/inputconditioncontainerreal_impl.h \
           project/inputcond/private/inputconditioncontainerstring_impl.h \
           project/inputcond/private/inputconditiondependencychecksubcaptions.h \
           project/inputcond/private/inputconditiondependencychecksubenumerations.h \
           project/inputcond/private/inputconditiondependencychecksubimages.h \
           project/inputcond/private/inputconditiondependencyconditionalways.h \
           project/inputcond/private/inputconditiondependencyconditionand.h \
           project/inputcond/private/inputconditiondependencyconditionisequal.h \
           project/inputcond/private/inputconditiondependencyconditionisgreaterequal.h \
           project/inputcond/private/inputconditiondependencyconditionisgreaterthan.h \
           project/inputcond/private/inputconditiondependencyconditionislessequal.h \
           project/inputcond/private/inputconditiondependencyconditionislessthan.h \
           project/inputcond/private/inputconditiondependencyconditionnot.h \
           project/inputcond/private/inputconditiondependencyconditionor.h \
           project/inputcond/private/inputconditiondependencysetsubcaptionaction.h \
           project/inputcond/private/inputconditiondependencysetsubenumerationsaction.h \
           project/inputcond/private/inputconditiondependencysetsubimageaction.h \
           project/inputcond/private/inputconditionwidgetcgnsbaseiterativeintegerresult.h \
           project/inputcond/private/inputconditionwidgetcgnsbaseiterativerealresult.h \
           project/inputcond/private/inputconditionwidgetcgnsfile.h \
           project/inputcond/private/inputconditionwidgetcgnsgridcellintegerresult.h \
           project/inputcond/private/inputconditionwidgetcgnsgridcellrealresult.h \
           project/inputcond/private/inputconditionwidgetcgnsgridedgeiintegerresult.h \
           project/inputcond/private/inputconditionwidgetcgnsgridedgeirealresult.h \
           project/inputcond/private/inputconditionwidgetcgnsgridedgejintegerresult.h \
           project/inputcond/private/inputconditionwidgetcgnsgridedgejrealresult.h \
           project/inputcond/private/inputconditionwidgetcgnsgridnodeintegerresult.h \
           project/inputcond/private/inputconditionwidgetcgnsgridnoderealresult.h \
           project/inputcond/private/inputconditionwidgetcgnsresult.h \
           project/inputcond/private/inputconditionwidgetcheckbox.h \
           project/inputcond/private/inputconditionwidgetfoldername.h \
           project/inputcond/private/inputconditionwidgetfunctional.h \
           project/inputcond/private/inputconditionwidgetfunctionaldelegate.h \
           project/inputcond/private/inputconditionwidgetfunctionaldialog.h \
           project/inputcond/private/inputconditionwidgetfunctionaldownloaddialog.h \
           project/inputcond/private/inputconditionwidgetimage.h \
           project/inputcond/private/inputconditionwidgetinteger.h \
           project/inputcond/private/inputconditionwidgetintegeroption.h \
           project/inputcond/private/inputconditionwidgetlabel.h \
           project/inputcond/private/inputconditionwidgetreal.h \
           project/inputcond/private/inputconditionwidgetrealoption.h \
           project/inputcond/private/inputconditionwidgetstring.h \
           project/inputcond/private/inputconditionwidgettooltip.h \
           project/measured/private/measureddata_impl.h \
           pre/gridcond/base/private/gridattributecellcontainert_detail.h \
           pre/gridcond/base/private/gridattributecontainert_detail.h \
           pre/gridcond/base/private/gridattributedimensioncontainer_impl.h \
           pre/gridcond/base/private/gridattributedimensioncontainert_detail.h \
           pre/gridcond/base/private/gridattributedimensionscontainer_impl.h \
           pre/gridcond/base/private/gridattributeeditwidget_impl.h \
           pre/gridcond/base/private/gridattributeeditwidgett_detail.h \
           pre/gridcond/base/private/gridattributenodecontainert_detail.h \
           pre/gridcond/base/private/gridattributevariationeditwidgett_detail.h
FORMS += arrows/arrowscolorsettingeditsmallwidget.ui \
         arrows/arrowscolorsettingeditwidget.ui \
         arrows/arrowslegendsettingeditwidget.ui \
         arrows/arrowslengthsettingeditwidget.ui \
         arrows/arrowssettingeditwidget.ui \
         arrows/arrowssettingtoolbarwidget.ui \
         arrows/arrowsshapesettingeditwidget.ui \
         contour/contoursettingeditwidget.ui \
         datamodel/propertybrowserview.ui \
         filter/generalfilteringsettingeditwidget.ui \
         filter/structured2dfilteringsettingeditwidget.ui \
         filter/structured3dfilteringsettingeditwidget.ui \
         filter/structuredgridskiporsubdividesettingeditwidget.ui \
         image/imagesettingwidget.ui \
         misc/mousepositionwidget.ui \
         post/postzoneselectingdialog.ui \
         postcontainer/postbaseselectingdialog.ui \
         postcontainer/postcalculatedresultargumenteditdialog.ui \
         postcontainer/postcalculatedresulteditdialog.ui \
         postcontainer/postcalculatedresultlistdialog.ui \
         postcontainer/postdataexportdialog.ui \
         postcontainer/poststringresultargumenteditdialog.ui \
         postcontainer/poststringresulteditwidget.ui \
         project/backgroundimageinfodialog.ui \
         project/backgroundimageinfogeoreferencedialog.ui \
         project/offsetsettingdialog.ui \
         project/projecttimesettingdialog.ui \
         region/region2dsettingeditwidget.ui \
         region/regionrange2dsettingeditwidget.ui \
         scalarstocolors/colormapenumeratelegendsettingeditwidget.ui \
         scalarstocolors/colormapenumeratesettingeditwidget.ui \
         scalarstocolors/colormaplegendsettingeditwidget.ui \
         scalarstocolors/colormapsettingeditdialog.ui \
         scalarstocolors/colormapsettingeditwidget.ui \
         scalarstocolors/colormapsettingeditwidgetwithimportexportbutton.ui \
         scalarstocolors/colormapsettingtoolbarwidget.ui \
         scalarstocolors/delegatedcolormapsettingeditwidget.ui \
         pre/complex/gridcomplexconditiondialog.ui \
         pre/complex/gridcomplexconditiongroupeditdialog.ui \
         pre/complex/gridcomplexconditiongrouprealeditwidget.ui \
         pre/complex/gridcomplexconditionwidget.ui \
         pre/geodatabackground/geodatabackgroundeditdialog.ui \
         project/inputcond/inputconditiondialog.ui \
         scalarstocolors/private/colormapenumeratesettingeditwidget_importdialog.ui \
         scalarstocolors/private/colormapsettingeditwidget_importdialog.ui \
         scalarstocolors/private/colormapsettingeditwidget_switchtodiscretedialog.ui \
         pre/gridcond/base/gridattributeeditdialog.ui \
         pre/gridcond/base/gridattributeeditnameandvaluedialog.ui \
         pre/gridcond/base/gridattributevariationeditdialog.ui \
         pre/gridcond/dimensionselectwidget/gridattributedimensioncomboboxselectwidget.ui \
         project/inputcond/private/inputconditioncgnsfileselectdialog.ui \
         project/inputcond/private/inputconditionwidgetcgnsfile.ui \
         project/inputcond/private/inputconditionwidgetfunctionaldialog.ui \
         project/inputcond/private/inputconditionwidgetfunctionaldownloaddialog.ui
SOURCES += arrows/arrowscolorsettingeditsmallwidget.cpp \
           arrows/arrowscolorsettingeditwidget.cpp \
           arrows/arrowslegendsettingcontainer.cpp \
           arrows/arrowslegendsettingeditwidget.cpp \
           arrows/arrowslengthsettingeditwidget.cpp \
           arrows/arrowssettingcontainer.cpp \
           arrows/arrowssettingeditwidget.cpp \
           arrows/arrowssettingtoolbarwidget.cpp \
           arrows/arrowsshapesettingeditwidget.cpp \
           base/iricmainwindowinterface.cpp \
           base/propertybrowser.cpp \
           base/snapshotenabledwindowinterface.cpp \
           base/windowwithzindexinterface.cpp \
           bgcolor/backgroundcoloreditinterface.cpp \
           bgcolor/backgroundcoloreditvtkinterface.cpp \
           contour/contoursettingcontainer.cpp \
           contour/contoursettingeditwidget.cpp \
           datamodel/attributebrowsertargetdataitem.cpp \
           datamodel/geodataproxydataitem.cpp \
           datamodel/graphics2dwindowdatamodel.cpp \
           datamodel/graphics3dwindowdatamodel.cpp \
           datamodel/graphicswindowdataitem.cpp \
           datamodel/graphicswindowdataitemstandarditemchangecommand.cpp \
           datamodel/graphicswindowdataitemupdateactorsettingdialog.cpp \
           datamodel/graphicswindowdatamodel.cpp \
           datamodel/graphicswindowdrawcommands.cpp \
           datamodel/graphicswindowrootdataitem.cpp \
           datamodel/graphicswindowsimpledatamodel.cpp \
           datamodel/modifycommandandrenderdialog.cpp \
           datamodel/propertybrowserattribute.cpp \
           datamodel/propertybrowserview.cpp \
           datamodel/vtk2dgraphicsview.cpp \
           datamodel/vtk3dgraphicsview.cpp \
           datamodel/vtkgraphicsview.cpp \
           executer/iricmainwindowexecuterwatcher.cpp \
           filter/generalfilteringsettingcontainer.cpp \
           filter/generalfilteringsettingeditwidget.cpp \
           filter/structured2dfilteringsettingcontainer.cpp \
           filter/structured2dfilteringsettingeditwidget.cpp \
           filter/structured3dfilteringsettingcontainer.cpp \
           filter/structured3dfilteringsettingeditwidget.cpp \
           filter/structuredgridskiporsubdividesettingcontainer.cpp \
           filter/structuredgridskiporsubdividesettingeditwidget.cpp \
           image/imagesettingcontainer.cpp \
           image/imagesettingwidget.cpp \
           misc/cgnsutil.cpp \
           misc/iricmetadata.cpp \
           misc/mouseboundingbox.cpp \
           misc/mousepositionwidget.cpp \
           misc/valuerangecontainer.cpp \
           misc/valuerangecontainerutil.cpp \
           misc/vtkpointsetandvaluerangeset.cpp \
           misc/vtkpointsetvaluerangeset.cpp \
           named/namedgraphicswindowdataitemtool.cpp \
           named/namedgraphicwindowdataitem.cpp \
           post/postprocessorwindow.cpp \
           post/postprocessorwindowfactoryinterface.cpp \
           post/postprocessorwindowprojectdataitem.cpp \
           post/postzonedataitem.cpp \
           post/postzoneselectingdialog.cpp \
           postcontainer/postabstractsteps.cpp \
           postcontainer/postbaseiterativedatacontainer.cpp \
           postcontainer/postbaseiterativeintegerdatacontainer.cpp \
           postcontainer/postbaseiterativenumericaldatacontainer.cpp \
           postcontainer/postbaseiterativerealdatacontainer.cpp \
           postcontainer/postbaseiterativeseriesdatacontainer.cpp \
           postcontainer/postbaseiterativestringdatacontainer.cpp \
           postcontainer/postbaseselectingdialog.cpp \
           postcontainer/postcalculatedresult.cpp \
           postcontainer/postcalculatedresultargument.cpp \
           postcontainer/postcalculatedresultargumenteditdialog.cpp \
           postcontainer/postcalculatedresulteditdialog.cpp \
           postcontainer/postcalculatedresultlistdialog.cpp \
           postcontainer/postdatacontainer.cpp \
           postcontainer/postdataexportdialog.cpp \
           postcontainer/postexportsetting.cpp \
           postcontainer/postiterationsteps.cpp \
           postcontainer/postseriesdatacontainer.cpp \
           postcontainer/postsolutioninfo.cpp \
           postcontainer/poststringresult.cpp \
           postcontainer/poststringresultargument.cpp \
           postcontainer/poststringresultargumenteditdialog.cpp \
           postcontainer/poststringresulteditwidget.cpp \
           postcontainer/posttimesteps.cpp \
           postcontainer/postzonedatacontainer.cpp \
           postcontainer/postzonepointseriesdatacontainer.cpp \
           project/addiblegcptablemodel.cpp \
           project/backgroundimageinfo.cpp \
           project/backgroundimageinfodialog.cpp \
           project/backgroundimageinfogeoreferencedialog.cpp \
           project/colorsource.cpp \
           project/gcptablemodel.cpp \
           project/georeferenceview.cpp \
           project/offsetsettingdialog.cpp \
           project/projectcgnsfile.cpp \
           project/projectdata.cpp \
           project/projectdataitem.cpp \
           project/projectmainfile.cpp \
           project/projectpostprocessors.cpp \
           project/projecttimesettingdialog.cpp \
           project/projectworkspace.cpp \
           project/workfolderinfo.cpp \
           region/region2dsettingcontainer.cpp \
           region/region2dsettingeditwidget.cpp \
           region/regionrange2dsettingcontainer.cpp \
           region/regionrange2dsettingeditwidget.cpp \
           scalarstocolors/colormapenumeratefactory.cpp \
           scalarstocolors/colormapenumeratelegendsettingcontainer.cpp \
           scalarstocolors/colormapenumeratelegendsettingeditwidget.cpp \
           scalarstocolors/colormapenumeratesettingcontainer.cpp \
           scalarstocolors/colormapenumeratesettingeditwidget.cpp \
           scalarstocolors/colormapfactory.cpp \
           scalarstocolors/colormaplegendsettingcontainer.cpp \
           scalarstocolors/colormaplegendsettingeditwidget.cpp \
           scalarstocolors/colormapsettingcontainer.cpp \
           scalarstocolors/colormapsettingcontaineri.cpp \
           scalarstocolors/colormapsettingcontainerutil.cpp \
           scalarstocolors/colormapsettingeditdialog.cpp \
           scalarstocolors/colormapsettingeditwidget.cpp \
           scalarstocolors/colormapsettingeditwidgeti.cpp \
           scalarstocolors/colormapsettingeditwidgetwithimportexportbutton.cpp \
           scalarstocolors/colormapsettingmodifycommand.cpp \
           scalarstocolors/colormapsettingtoolbarwidget.cpp \
           scalarstocolors/colormapsettingtoolbarwidgetcontroller.cpp \
           scalarstocolors/colormapsettingvaluecolorpaircontainer.cpp \
           scalarstocolors/delegatedcolormapsettingcontainer.cpp \
           scalarstocolors/delegatedcolormapsettingeditwidget.cpp \
           scalarstocolors/discretevaluesutil.cpp \
           solverdef/integerenumloader.cpp \
           solverdef/realenumloader.cpp \
           solverdef/solverdefinition.cpp \
           solverdef/solverdefinitionabstract.cpp \
           solverdef/solverdefinitionboundarycondition.cpp \
           solverdef/solverdefinitiongridattribute.cpp \
           solverdef/solverdefinitiongridattributedimension.cpp \
           solverdef/solverdefinitiongridattributedimensioncreator.cpp \
           solverdef/solverdefinitiongridattributeinteger.cpp \
           solverdef/solverdefinitiongridattributeintegercell.cpp \
           solverdef/solverdefinitiongridattributeintegerdimension.cpp \
           solverdef/solverdefinitiongridattributeintegernode.cpp \
           solverdef/solverdefinitiongridattributeintegeroptioncell.cpp \
           solverdef/solverdefinitiongridattributeintegeroptionnode.cpp \
           solverdef/solverdefinitiongridattributereal.cpp \
           solverdef/solverdefinitiongridattributerealcell.cpp \
           solverdef/solverdefinitiongridattributerealdimension.cpp \
           solverdef/solverdefinitiongridattributerealnode.cpp \
           solverdef/solverdefinitiongridattributerealoptioncell.cpp \
           solverdef/solverdefinitiongridattributerealoptionnode.cpp \
           solverdef/solverdefinitiongridcomplexattribute.cpp \
           solverdef/solverdefinitiongridtype.cpp \
           solverdef/solverdefinitionnode.cpp \
           solverdef/solverdefinitionoutput.cpp \
           solverdef/solverdefinitiontranslator.cpp \
           tmsimage/tmsimagedataitem.cpp \
           tmsimage/tmsimagegroupdataitem.cpp \
           tmsimage/tmsimagesetting.cpp \
           tmsimage/tmsimagesettingmanager.cpp \
           tmsimage/tmsregionselectwidget.cpp \
           arrows/private/arrowslegendsettingcontainer_imagebuilder.cpp \
           arrows/private/arrowssettingeditwidget_modifycommand.cpp \
           bgcolor/private/backgroundcoloreditinterface_setbackgroundcolorcommand.cpp \
           datamodel/private/graphicswindowdataitem_modifycommand.cpp \
           datamodel/private/graphicswindowdataitem_rendercommand.cpp \
           datamodel/private/graphicswindowdataitem_renderredoonlycommand.cpp \
           datamodel/private/graphicswindowdataitem_standarditemmodifycommand.cpp \
           datamodel/private/graphicswindowdataitem_updateactorsettingcommand.cpp \
           datamodel/private/graphicswindowrootdataitem_movedowncommand.cpp \
           datamodel/private/graphicswindowrootdataitem_moveupcommand.cpp \
           image/private/imagesettingcontainer_movecommand.cpp \
           image/private/imagesettingcontainer_resizecommand.cpp \
           image/private/imagesettingcontainer_updatecommand.cpp \
           image/public/imagesettingcontainer_controller.cpp \
           image/public/imagesettingcontainer_imagebuilder.cpp \
           misc/targeted/targeteditemsettargetcommand.cpp \
           misc/targeted/targeteditemsettargetcommandtool.cpp \
           postcontainer/exporter/postzonedatacsvexporter.cpp \
           postcontainer/exporter/postzonedatashapeexporter.cpp \
           postcontainer/exporter/postzonedatatpoexporter.cpp \
           postcontainer/exporter/postzonedatavtkexporter.cpp \
           postcontainer/private/postcalculatedresulteditdialog_tabledelegate.cpp \
           postcontainer/private/postzonedatacontainer_particlegrouploader.cpp \
           postcontainer/private/postzonedatacontainer_particleloader.cpp \
           postcontainer/private/postzonedatacontainer_polydataloader.cpp \
           pre/base/preprocessordataitem.cpp \
           pre/base/preprocessordatamodelinterface.cpp \
           pre/base/preprocessorgeodatatopdataiteminterface.cpp \
           pre/base/preprocessorgraphicsviewinterface.cpp \
           pre/base/preprocessorgridandgridcreatingconditiondataiteminterface.cpp \
           pre/base/preprocessorgridcreatingconditiondataiteminterface.cpp \
           pre/base/preprocessorhydraulicdatadataiteminterface.cpp \
           pre/base/preprocessorhydraulicdatagroupdataiteminterface.cpp \
           pre/base/preprocessorhydraulicdatatopdataiteminterface.cpp \
           pre/base/preprocessorwindowinterface.cpp \
           pre/complex/gridcomplexconditiondialog.cpp \
           pre/complex/gridcomplexconditiongroup.cpp \
           pre/complex/gridcomplexconditiongroupeditdialog.cpp \
           pre/complex/gridcomplexconditiongroupeditwidgetcontainer.cpp \
           pre/complex/gridcomplexconditiongrouprealeditwidget.cpp \
           pre/complex/gridcomplexconditionwidget.cpp \
           pre/complex/gridcomplexconditionwidgetcontainer.cpp \
           pre/geodata/geodata.cpp \
           pre/geodata/geodatacreator.cpp \
           pre/geodata/geodataexporter.cpp \
           pre/geodata/geodatafactory.cpp \
           pre/geodata/geodataimporter.cpp \
           pre/geodata/geodatamapper.cpp \
           pre/geodata/geodataproxy.cpp \
           pre/geodata/geodatawebimporter.cpp \
           pre/geodata/geodatawithsinglemapper.cpp \
           pre/geodatabackground/geodatabackground.cpp \
           pre/geodatabackground/geodatabackgroundcomplex.cpp \
           pre/geodatabackground/geodatabackgroundcomplexcreator.cpp \
           pre/geodatabackground/geodatabackgroundeditdialog.cpp \
           pre/geodatabackground/geodatabackgroundintegercreator.cpp \
           pre/geodatabackground/geodatabackgroundrealcreator.cpp \
           pre/grid/grid.cpp \
           pre/grid/grid2d.cpp \
           pre/grid/grid3d.cpp \
           pre/grid/gridcgnsestimater.cpp \
           pre/grid/gridinternalexporter.cpp \
           pre/grid/gridinternalimporter.cpp \
           pre/grid/structured15dgridwithcrosssection.cpp \
           pre/grid/structured2dgrid.cpp \
           pre/grid/unstructured2dgrid.cpp \
           pre/gridcreatingcondition/gridcreatingcondition.cpp \
           pre/gridcreatingcondition/gridcreatingconditioncreator.cpp \
           pre/gridcreatingcondition/gridcreatingconditionio.cpp \
           pre/hydraulicdata/hydraulicdata.cpp \
           pre/hydraulicdata/hydraulicdatacreator.cpp \
           pre/hydraulicdata/hydraulicdataexporter.cpp \
           pre/hydraulicdata/hydraulicdataimporter.cpp \
           pre/hydraulicdata/hydraulicdatamultiexporter.cpp \
           project/inputcond/inputconditioncontainer.cpp \
           project/inputcond/inputconditioncontainerfunctional.cpp \
           project/inputcond/inputconditioncontainerinteger.cpp \
           project/inputcond/inputconditioncontainerreal.cpp \
           project/inputcond/inputconditioncontainerset.cpp \
           project/inputcond/inputconditioncontainerstring.cpp \
           project/inputcond/inputconditiondependency.cpp \
           project/inputcond/inputconditiondialog.cpp \
           project/inputcond/inputconditionpage.cpp \
           project/inputcond/inputconditionpagecontainer.cpp \
           project/inputcond/inputconditionpagelist.cpp \
           project/inputcond/inputconditionwidget.cpp \
           project/inputcond/inputconditionwidgetfilename.cpp \
           project/inputcond/inputconditionwidgetset.cpp \
           project/measured/measureddata.cpp \
           project/measured/measureddatacsvexporter.cpp \
           project/measured/measureddatacsvimporter.cpp \
           project/private/backgroundimageinfo_impl.cpp \
           project/private/backgroundimageinfo_setactorpropertycommand.cpp \
           project/private/backgroundimageinfo_setting.cpp \
           project/private/backgroundimageinfogeoreferencedialog_styleditemdelegate.cpp \
           project/private/georeferenceview_imageinfo.cpp \
           project/private/projectdatacopythread.cpp \
           project/private/projectdatamovethread.cpp \
           project/util/gcptablegeoreferenceviewhelper.cpp \
           project/util/georeferenceview_imageinfo_destinationoperationwaitingselectionhelper.cpp \
           project/util/georeferenceview_imageinfo_initselectionhelper.cpp \
           project/util/georeferenceview_imageinfo_pointsinrectangleselectionhelper.cpp \
           project/util/georeferenceview_imageinfo_selectionhelper.cpp \
           project/util/georeferenceview_imageinfo_translationselectionhelper.cpp \
           project/util/georeferenceview_imageinfo_voidselectionhelper.cpp \
           project/util/georeferenceview_imageinfo_waitingselectionhelper.cpp \
           project/util/georeferenceviewhelper.cpp \
           project/util/georeferenceviewselectionhelper.cpp \
           project/util/pointsgeoreferenceviewhelper.cpp \
           project/util/polygongeoreferenceviewhelper.cpp \
           project/util/rectgeoreferenceviewhelper.cpp \
           project/util/selectionhelper.cpp \
           scalarstocolors/private/colormapenumeratelegendsettingcontainer_imagebuilder.cpp \
           scalarstocolors/private/colormapenumeratesettingeditwidget_colortablecontroller.cpp \
           scalarstocolors/private/colormapenumeratesettingeditwidget_colortablecontroller_delegate.cpp \
           scalarstocolors/private/colormapenumeratesettingeditwidget_importdialog.cpp \
           scalarstocolors/private/colormapenumeratesettingeditwidget_importdialog_setting.cpp \
           scalarstocolors/private/colormaplegendsettingcontainer_imagebuilder.cpp \
           scalarstocolors/private/colormapsettingeditwidget_colortablecontroller.cpp \
           scalarstocolors/private/colormapsettingeditwidget_colortablecontroller_delegate.cpp \
           scalarstocolors/private/colormapsettingeditwidget_importdialog.cpp \
           scalarstocolors/private/colormapsettingeditwidget_importdialog_setting.cpp \
           scalarstocolors/private/colormapsettingeditwidget_switchtodiscretedialog.cpp \
           scalarstocolors/private/colormapsettingeditwidget_switchtodiscretedialog_delegate.cpp \
           post/post2d/base/post2dwindowgridtypedataiteminterface.cpp \
           pre/geodata/private/geodata_propertydialog.cpp \
           pre/geodata/private/geodata_updateactorsettingcommand.cpp \
           pre/geodata/private/geodataproxy_propertydialog.cpp \
           pre/geodata/private/geodataproxy_updateactorsettingcommand.cpp \
           pre/geodatabackground/private/geodatabackground_editvaluecommand.cpp \
           pre/grid/structured15dgrid/structured15dgridwithcrosssectioncrosssection.cpp \
           pre/gridcond/base/gridattributebaseobject.cpp \
           pre/gridcond/base/gridattributecontainer.cpp \
           pre/gridcond/base/gridattributedimensioncontainer.cpp \
           pre/gridcond/base/gridattributedimensionintegercontainer.cpp \
           pre/gridcond/base/gridattributedimensionrealcontainer.cpp \
           pre/gridcond/base/gridattributedimensionscontainer.cpp \
           pre/gridcond/base/gridattributedimensionselectwidget.cpp \
           pre/gridcond/base/gridattributeeditcommand.cpp \
           pre/gridcond/base/gridattributeeditdialog.cpp \
           pre/gridcond/base/gridattributeeditnameandvaluedialog.cpp \
           pre/gridcond/base/gridattributeeditvariationcommand.cpp \
           pre/gridcond/base/gridattributeeditwidget.cpp \
           pre/gridcond/base/gridattributestringconverter.cpp \
           pre/gridcond/base/gridattributevariationeditdialog.cpp \
           pre/gridcond/base/gridattributevariationeditwidget.cpp \
           pre/gridcond/base/gridattributevariationeditwidgetcontainer.cpp \
           pre/gridcond/complex/gridcomplexattributecontainer.cpp \
           pre/gridcond/complex/gridcomplexattributeeditwidget.cpp \
           pre/gridcond/dimensionselectwidget/gridattributedimensioncomboboxselectwidget.cpp \
           pre/gridcond/dimensionselectwidget/gridattributedimensiontimesliderselectwidget.cpp \
           pre/gridcond/editwidget/gridattributeintegereditwidget.cpp \
           pre/gridcond/editwidget/gridattributeintegeroptioneditwidget.cpp \
           pre/gridcond/editwidget/gridattributeintegervariationeditwidget.cpp \
           pre/gridcond/editwidget/gridattributerealeditwidget.cpp \
           pre/gridcond/editwidget/gridattributerealoptioneditwidget.cpp \
           pre/gridcond/editwidget/gridattributerealvariationeditwidget.cpp \
           pre/gridcond/stringconverter/gridattributestringconverterenumerate.cpp \
           pre/gridcond/stringconverter/gridattributestringconverterinteger.cpp \
           pre/gridcond/stringconverter/gridattributestringconverterreal.cpp \
           project/inputcond/private/inputconditioncgnsfile.cpp \
           project/inputcond/private/inputconditioncgnsfileselectdialog.cpp \
           project/inputcond/private/inputconditiondependencychecksubcaptions.cpp \
           project/inputcond/private/inputconditiondependencychecksubenumerations.cpp \
           project/inputcond/private/inputconditiondependencychecksubimages.cpp \
           project/inputcond/private/inputconditiondependencyconditionalways.cpp \
           project/inputcond/private/inputconditiondependencyconditionand.cpp \
           project/inputcond/private/inputconditiondependencyconditionisequal.cpp \
           project/inputcond/private/inputconditiondependencyconditionisgreaterequal.cpp \
           project/inputcond/private/inputconditiondependencyconditionisgreaterthan.cpp \
           project/inputcond/private/inputconditiondependencyconditionislessequal.cpp \
           project/inputcond/private/inputconditiondependencyconditionislessthan.cpp \
           project/inputcond/private/inputconditiondependencyconditionnot.cpp \
           project/inputcond/private/inputconditiondependencyconditionor.cpp \
           project/inputcond/private/inputconditiondependencysetsubcaptionaction.cpp \
           project/inputcond/private/inputconditiondependencysetsubenumerationsaction.cpp \
           project/inputcond/private/inputconditiondependencysetsubimageaction.cpp \
           project/inputcond/private/inputconditionwidgetcgnsbaseiterativeintegerresult.cpp \
           project/inputcond/private/inputconditionwidgetcgnsbaseiterativerealresult.cpp \
           project/inputcond/private/inputconditionwidgetcgnsfile.cpp \
           project/inputcond/private/inputconditionwidgetcgnsgridcellintegerresult.cpp \
           project/inputcond/private/inputconditionwidgetcgnsgridcellrealresult.cpp \
           project/inputcond/private/inputconditionwidgetcgnsgridedgeiintegerresult.cpp \
           project/inputcond/private/inputconditionwidgetcgnsgridedgeirealresult.cpp \
           project/inputcond/private/inputconditionwidgetcgnsgridedgejintegerresult.cpp \
           project/inputcond/private/inputconditionwidgetcgnsgridedgejrealresult.cpp \
           project/inputcond/private/inputconditionwidgetcgnsgridnodeintegerresult.cpp \
           project/inputcond/private/inputconditionwidgetcgnsgridnoderealresult.cpp \
           project/inputcond/private/inputconditionwidgetcgnsresult.cpp \
           project/inputcond/private/inputconditionwidgetcheckbox.cpp \
           project/inputcond/private/inputconditionwidgetfoldername.cpp \
           project/inputcond/private/inputconditionwidgetfunctional.cpp \
           project/inputcond/private/inputconditionwidgetfunctionaldelegate.cpp \
           project/inputcond/private/inputconditionwidgetfunctionaldialog.cpp \
           project/inputcond/private/inputconditionwidgetfunctionaldownloaddialog.cpp \
           project/inputcond/private/inputconditionwidgetimage.cpp \
           project/inputcond/private/inputconditionwidgetinteger.cpp \
           project/inputcond/private/inputconditionwidgetintegeroption.cpp \
           project/inputcond/private/inputconditionwidgetlabel.cpp \
           project/inputcond/private/inputconditionwidgetreal.cpp \
           project/inputcond/private/inputconditionwidgetrealoption.cpp \
           project/inputcond/private/inputconditionwidgetstring.cpp \
           project/inputcond/private/inputconditionwidgettooltip.cpp
RESOURCES += guicore.qrc
TRANSLATIONS += languages/iricGuicore_ar_EG.ts \
                languages/iricGuicore_bg_BG.ts \
                languages/iricGuicore_bs_BA.ts \
                languages/iricGuicore_ca_ES.ts \
                languages/iricGuicore_cs_CZ.ts \
                languages/iricGuicore_da_DK.ts \
                languages/iricGuicore_de_DE.ts \
                languages/iricGuicore_el_GR.ts \
                languages/iricGuicore_es_ES.ts \
                languages/iricGuicore_et_EE.ts \
                languages/iricGuicore_eu_ES.ts \
                languages/iricGuicore_fi_FI.ts \
                languages/iricGuicore_fr_FR.ts \
                languages/iricGuicore_gl_ES.ts \
                languages/iricGuicore_hi_IN.ts \
                languages/iricGuicore_hu_HU.ts \
                languages/iricGuicore_id_ID.ts \
                languages/iricGuicore_is_IS.ts \
                languages/iricGuicore_it_IT.ts \
                languages/iricGuicore_ja_JP.ts \
                languages/iricGuicore_ko_KR.ts \
                languages/iricGuicore_ky_KG.ts \
                languages/iricGuicore_lt_LT.ts \
                languages/iricGuicore_lv_LV.ts \
                languages/iricGuicore_nb_NO.ts \
                languages/iricGuicore_nl_NL.ts \
                languages/iricGuicore_pl_PL.ts \
                languages/iricGuicore_pt_BR.ts \
                languages/iricGuicore_pt_PT.ts \
                languages/iricGuicore_ro_RO.ts \
                languages/iricGuicore_ru_RU.ts \
                languages/iricGuicore_sl_SI.ts \
                languages/iricGuicore_sv_SE.ts \
                languages/iricGuicore_th_TH.ts \
                languages/iricGuicore_tr_TR.ts \
                languages/iricGuicore_uk_UA.ts \
                languages/iricGuicore_vi_VN.ts \
                languages/iricGuicore_zh_CN.ts \
                languages/iricGuicore_zh_TW.ts

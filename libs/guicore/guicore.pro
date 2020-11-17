TARGET = iricGuicore
TEMPLATE = lib


DEFINES += GUICORE_LIBRARY
DEFINES += ANSI_DECLARATORS
win32 {
	DEFINES += YAML_CPP_DLL
}

include( ../../paths.pri )

QT += network qml widgets xml

######################
# Internal libraries #
######################

# iricCs

unix {
	LIBS += -L"../cs"
}
LIBS += -liricCs

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

# iricGuibase

unix{
LIBS += -L"../guibase"
}

LIBS += -liricGuibase

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
	-lvtkCommonCore-6.1 \
	-lvtkCommonDataModel-6.1 \
	-lvtkCommonExecutionModel-6.1 \
	-lvtkCommonMisc-6.1 \
	-lvtkFiltersCore-6.1 \
	-lvtkFiltersGeometry-6.1 \
	-lvtkFiltersGeneral-6.1 \
	-lvtkFiltersExtraction-6.1 \
	-lvtkFiltersSources-6.1 \
	-lvtkFiltersTexture-6.1 \
	-lvtkGUISupportQt-6.1 \
	-lvtkInteractionStyle-6.1 \
	-lvtkInteractionWidgets-6.1 \
	-lvtkIOImage-6.1 \
	-lvtkIOCore-6.1 \
	-lvtkIOLegacy-6.1 \
	-lvtkRenderingAnnotation-6.1 \
	-lvtkRenderingCore-6.1 \
	-lvtkRenderingFreeType-6.1 \
	-lvtkRenderingLabel-6.1 \
	-lvtkRenderingLOD-6.1 \
	-lvtkRenderingQt-6.1


# hdf5
win32 {
	CONFIG(debug, debug|release) {
		LIBS += -lhdf5_D
	} else {
		LIBS += -lhdf5
	}
}

# cgnslib

win32{
LIBS += -lcgnsdll
}
unix{
LIBS += -lcgns
}

# iriclib

LIBS += -liriclib

win32 {
	DESTDIR = $(SolutionDir)/libdlls/$(Configuration)
	LIBS += -L$(SolutionDir)/libdlls/$(Configuration)
}

# Input
HEADERS += guicore_global.h \
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
           datamodel/attributebrowsertargetdataitem.h \
           datamodel/geodataproxydataitem.h \
           datamodel/graphics2dwindowdatamodel.h \
           datamodel/graphics3dwindowdatamodel.h \
           datamodel/graphicswindowdataitem.h \
           datamodel/graphicswindowdataitemstandarditemchangecommand.h \
           datamodel/graphicswindowdatamodel.h \
           datamodel/graphicswindowdrawcommands.h \
           datamodel/graphicswindowrootdataitem.h \
           datamodel/graphicswindowsimpledatamodel.h \
           datamodel/propertybrowserattribute.h \
           datamodel/propertybrowserview.h \
           datamodel/vtk2dgraphicsview.h \
           datamodel/vtk3dgraphicsview.h \
           datamodel/vtkgraphicsview.h \
           executer/iricmainwindowexecuterwatcher.h \
           misc/cgnsfileopener.h \
           misc/cgnslinkfollower.h \
           misc/cgnsutil.h \
           misc/iricmetadata.h \
           misc/mouseboundingbox.h \
           misc/mousepositionwidget.h \
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
           project/cgnsfileentry.h \
           project/cgnsfilelist.h \
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
           scalarstocolors/colortransferfunctioncontainer.h \
           scalarstocolors/colortransferfunctioneditdialog.h \
           scalarstocolors/colortransferfunctioneditwidget.h \
           scalarstocolors/lookuptablecontainer.h \
           scalarstocolors/lookuptableeditwidget.h \
           scalarstocolors/scalarstocolorscontainer.h \
           scalarstocolors/scalarstocolorscontainerutil.h \
           scalarstocolors/scalarstocolorseditdialog.h \
           scalarstocolors/scalarstocolorseditwidget.h \
           scalarstocolors/scalarstocolorseditwidgetcontainer.h \
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
           solverdef/solverdefinitiontranslator.h \
           tmsimage/tmsimagedataitem.h \
           tmsimage/tmsimagegroupdataitem.h \
           tmsimage/tmsimagesetting.h \
           tmsimage/tmsimagesettingmanager.h \
           tmsimage/tmsregionselectwidget.h \
           bgcolor/private/backgroundcoloreditinterface_setbackgroundcolorcommand.h \
           datamodel/private/graphicswindowdataitem_modifycommand.h \
           datamodel/private/graphicswindowdataitem_rendercommand.h \
           datamodel/private/graphicswindowdataitem_renderredoonlycommand.h \
           datamodel/private/graphicswindowdataitem_standarditemmodifycommand.h \
           datamodel/private/graphicswindowrootdataitem_movedowncommand.h \
           datamodel/private/graphicswindowrootdataitem_moveupcommand.h \
           datamodel/private/vtkgraphicsview_impl.h \
           misc/private/cgnsfileopener_impl.h \
           misc/private/cgnslinkfollower_impl.h \
           misc/private/cgnsutil_detail.h \
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
           pre/geodata/geodataimporter.h \
           pre/geodata/geodatamapper.h \
           pre/geodata/geodatamappersettingi.h \
           pre/geodata/geodatamappert.h \
           pre/geodata/geodatanodemappert.h \
           pre/geodata/geodataproxy.h \
           pre/geodata/geodatawebimporter.h \
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
           pre/gridcond/gridcellattributepropertydialog.h \
           pre/gridcond/gridnodeattributepropertydialog.h \
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
           project/private/backgroundimageinfo_setactorpropertycommand.h \
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
           solverdef/private/solverdefinitiontranslator_impl.h \
           tmsimage/private/tmsimagegroupdataitem_impl.h \
           tmsimage/private/tmsimagesetting_impl.h \
           tmsimage/private/tmsregionselectwidget_impl.h \
           pre/complex/private/gridcomplexconditiongroup_impl.h \
           pre/complex/private/gridcomplexconditionwidget_impl.h \
           pre/geodata/private/geodatacellmappert_detail.h \
           pre/geodata/private/geodatacreator_impl.h \
           pre/geodata/private/geodataexporter_impl.h \
           pre/geodata/private/geodataimporter_impl.h \
           pre/geodata/private/geodatamapper_impl.h \
           pre/geodata/private/geodatamappert_detail.h \
           pre/geodata/private/geodatanodemappert_detail.h \
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
           pre/hydraulicdata/private/hydraulicdata_impl.h \
           pre/hydraulicdata/private/hydraulicdatacreator_impl.h \
           pre/hydraulicdata/private/hydraulicdataexporter_impl.h \
           pre/hydraulicdata/private/hydraulicdataimporter_impl.h \
           pre/hydraulicdata/private/hydraulicdatamultiexporter_impl.h \
           project/inputcond/private/inputconditioncontainer_impl.h \
           project/inputcond/private/inputconditioncontainerfunctional_impl.h \
           project/inputcond/private/inputconditioncontainerinteger_impl.h \
           project/inputcond/private/inputconditioncontainerreal_impl.h \
           project/inputcond/private/inputconditioncontainerstring_impl.h \
           project/inputcond/private/inputconditiondependencychecksubcaptions.h \
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
           project/inputcond/private/inputconditiondependencysetsubimageaction.h \
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
FORMS += datamodel/propertybrowserview.ui \
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
         scalarstocolors/colortransferfunctioneditwidget.ui \
         scalarstocolors/lookuptableeditwidget.ui \
         scalarstocolors/scalarstocolorseditdialog.ui \
         pre/complex/gridcomplexconditiondialog.ui \
         pre/complex/gridcomplexconditiongroupeditdialog.ui \
         pre/complex/gridcomplexconditiongrouprealeditwidget.ui \
         pre/complex/gridcomplexconditionwidget.ui \
         pre/geodatabackground/geodatabackgroundeditdialog.ui \
         pre/gridcond/gridcellattributepropertydialog.ui \
         pre/gridcond/gridnodeattributepropertydialog.ui \
         project/inputcond/inputconditiondialog.ui \
         pre/gridcond/base/gridattributeeditdialog.ui \
         pre/gridcond/base/gridattributeeditnameandvaluedialog.ui \
         pre/gridcond/base/gridattributevariationeditdialog.ui \
         pre/gridcond/dimensionselectwidget/gridattributedimensioncomboboxselectwidget.ui \
         project/inputcond/private/inputconditionwidgetfunctionaldialog.ui \
         project/inputcond/private/inputconditionwidgetfunctionaldownloaddialog.ui
SOURCES += base/iricmainwindowinterface.cpp \
           base/propertybrowser.cpp \
           base/snapshotenabledwindowinterface.cpp \
           base/windowwithzindexinterface.cpp \
           bgcolor/backgroundcoloreditinterface.cpp \
           bgcolor/backgroundcoloreditvtkinterface.cpp \
           datamodel/attributebrowsertargetdataitem.cpp \
           datamodel/geodataproxydataitem.cpp \
           datamodel/graphics2dwindowdatamodel.cpp \
           datamodel/graphics3dwindowdatamodel.cpp \
           datamodel/graphicswindowdataitem.cpp \
           datamodel/graphicswindowdataitemstandarditemchangecommand.cpp \
           datamodel/graphicswindowdatamodel.cpp \
           datamodel/graphicswindowdrawcommands.cpp \
           datamodel/graphicswindowrootdataitem.cpp \
           datamodel/graphicswindowsimpledatamodel.cpp \
           datamodel/propertybrowserattribute.cpp \
           datamodel/propertybrowserview.cpp \
           datamodel/vtk2dgraphicsview.cpp \
           datamodel/vtk3dgraphicsview.cpp \
           datamodel/vtkgraphicsview.cpp \
           executer/iricmainwindowexecuterwatcher.cpp \
           misc/cgnsfileopener.cpp \
           misc/cgnslinkfollower.cpp \
           misc/cgnsutil.cpp \
           misc/iricmetadata.cpp \
           misc/mouseboundingbox.cpp \
           misc/mousepositionwidget.cpp \
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
           project/cgnsfileentry.cpp \
           project/cgnsfilelist.cpp \
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
           scalarstocolors/colortransferfunctioncontainer.cpp \
           scalarstocolors/colortransferfunctioneditdialog.cpp \
           scalarstocolors/colortransferfunctioneditwidget.cpp \
           scalarstocolors/lookuptablecontainer.cpp \
           scalarstocolors/lookuptableeditwidget.cpp \
           scalarstocolors/scalarstocolorscontainer.cpp \
           scalarstocolors/scalarstocolorscontainerutil.cpp \
           scalarstocolors/scalarstocolorseditdialog.cpp \
           scalarstocolors/scalarstocolorseditwidget.cpp \
           scalarstocolors/scalarstocolorseditwidgetcontainer.cpp \
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
           solverdef/solverdefinitiontranslator.cpp \
           tmsimage/tmsimagedataitem.cpp \
           tmsimage/tmsimagegroupdataitem.cpp \
           tmsimage/tmsimagesetting.cpp \
           tmsimage/tmsimagesettingmanager.cpp \
           tmsimage/tmsregionselectwidget.cpp \
           bgcolor/private/backgroundcoloreditinterface_setbackgroundcolorcommand.cpp \
           datamodel/private/graphicswindowdataitem_modifycommand.cpp \
           datamodel/private/graphicswindowdataitem_rendercommand.cpp \
           datamodel/private/graphicswindowdataitem_renderredoonlycommand.cpp \
           datamodel/private/graphicswindowdataitem_standarditemmodifycommand.cpp \
           datamodel/private/graphicswindowrootdataitem_movedowncommand.cpp \
           datamodel/private/graphicswindowrootdataitem_moveupcommand.cpp \
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
           pre/geodata/geodataimporter.cpp \
           pre/geodata/geodatamapper.cpp \
           pre/geodata/geodataproxy.cpp \
           pre/geodata/geodatawebimporter.cpp \
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
           pre/gridcond/gridcellattributepropertydialog.cpp \
           pre/gridcond/gridnodeattributepropertydialog.cpp \
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
           project/private/backgroundimageinfo_setactorpropertycommand.cpp \
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
           project/inputcond/private/inputconditiondependencychecksubcaptions.cpp \
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
           project/inputcond/private/inputconditiondependencysetsubimageaction.cpp \
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

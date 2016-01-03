######################################################################
# Automatically generated by qmake (2.01a) ? 10 24 00:36:17 2014
######################################################################

TARGET = iricGuicore
TEMPLATE = lib


DEFINES += GUICORE_LIBRARY
DEFINES += ANSI_DECLARATORS
DEFINES += YAML_CPP_DLL

include( ../../paths.pri )

QT += network widgets xml

######################
# Internal libraries #
######################

# iricMisc

win32 {
	CONFIG(debug, debug|release) {
		LIBS += -L"../misc/debug"
	} else {
		LIBS += -L"../misc/release"
	}
}
unix {
	LIBS += -L"../misc"
}
LIBS += -liricMisc

# iricTriangle

win32 {
	CONFIG(debug, debug|release) {
		LIBS += -L"../triangle/debug"
	} else {
		LIBS += -L"../triangle/release"
	}
}
unix {
	LIBS += -L"../triangle"
}
LIBS += -liricTriangle

# iricGuibase


win32{
CONFIG(debug, debug|release) {
	LIBS += -L"../guibase/debug"
} else {
	LIBS += -L"../guibase/release"
}
}
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
	LIBS += -lgdal
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

# cgnslib

win32{
LIBS += -lcgnsdll
}
unix{
LIBS += -lcgns
}

# iriclib

LIBS += -liriclib

# Input
HEADERS += guicore_global.h \
           axis2d/axis2ddataitem.h \
           axis3d/axis3ddataitem.h \
           base/additionalmenuwindowi.h \
           base/animationcontrollerinterface.h \
           base/clipboardoperatablewindowinterface.h \
           base/iricmainwindowinterface.h \
           base/propertybrowser.h \
           base/snapshotenabledwindowinterface.h \
           base/windowwithobjectbrowserinterface.h \
           base/windowwithpropertybrowser.h \
           base/windowwithzindexinterface.h \
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
           distancemeasure/distancemeasurecopypropertydialog.h \
           distancemeasure/distancemeasuredataitem.h \
           distancemeasure/distancemeasuregroupdataitem.h \
           measureddata/measureddatafiledataitem.h \
           measureddata/measureddatapointdataitem.h \
           measureddata/measureddatapointgroupdataitem.h \
           measureddata/measureddatapointsettingdialog.h \
           measureddata/measureddatavectordataitem.h \
           measureddata/measureddatavectorgroupdataitem.h \
           measureddata/measureddatavectorsettingdialog.h \
           misc/mouseboundingbox.h \
           misc/mousepositionwidget.h \
           misc/qundocommandhelper.h \
           named/namedgraphicswindowdataitemtool.h \
           named/namedgraphicwindowdataitem.h \
           post/postprocessorwindow.h \
           post/postprocessorwindowfactoryinterface.h \
           post/postprocessorwindowprojectdataitem.h \
           post/postzoneselectingdialog.h \
           postcontainer/postabstractsteps.h \
           postcontainer/postbaseiterativeseriesdatacontainer.h \
           postcontainer/postbaseselectingdialog.h \
           postcontainer/postdatacontainer.h \
           postcontainer/postdataexportdialog.h \
           postcontainer/postdummy3dzonedatacontainer.h \
           postcontainer/postexportsetting.h \
           postcontainer/postiterationsteps.h \
           postcontainer/postseriesdatacontainer.h \
           postcontainer/postsolutioninfo.h \
           postcontainer/posttimesteps.h \
           postcontainer/postzonedatacontainer.h \
           postcontainer/postzonepointseriesdatacontainer.h \
           project/backgroundimageinfo.h \
           project/backgroundimageinfodialog.h \
           project/cgnsfilelist.h \
           project/colorsource.h \
           project/measureddata.h \
           project/offsetsettingdialog.h \
           project/projectcgnsfile.h \
           project/projectdata.h \
           project/projectdataitem.h \
           project/projectmainfile.h \
           project/projectpostprocessors.h \
           project/projectworkspace.h \
           project/workfolderinfo.h \
           scalarstocolors/colortransferfunctioncontainer.h \
           scalarstocolors/colortransferfunctioneditdialog.h \
           scalarstocolors/colortransferfunctioneditwidget.h \
           scalarstocolors/lookuptablecontainer.h \
           scalarstocolors/lookuptableeditwidget.h \
           scalarstocolors/scalarstocolorscontainer.h \
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
           distancemeasure/private/distancemeasuregroupdataitem_impl.h \
           misc/targeted/targeteditemi.h \
           misc/targeted/targeteditemsettargetcommand.h \
           misc/targeted/targeteditemsettargetcommandtool.h \
           postcontainer/exporter/postzonedatacsvexporter.h \
           postcontainer/exporter/postzonedataexporter.h \
           postcontainer/exporter/postzonedatashapeexporter.h \
           postcontainer/exporter/postzonedatavtkexporter.h \
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
           pre/base/preprocessorwindowinterface.h \
           pre/complex/gridcomplexconditiondialog.h \
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
           pre/hydraulicdata/hydraulicdataimporter.h \
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
           project/inputcond/inputconditionwidgetfoldername.h \
           project/inputcond/inputconditionwidgetfunctional.h \
           project/inputcond/inputconditionwidgetinteger.h \
           project/inputcond/inputconditionwidgetintegeroption.h \
           project/inputcond/inputconditionwidgetreal.h \
           project/inputcond/inputconditionwidgetrealoption.h \
           project/inputcond/inputconditionwidgetset.h \
           project/inputcond/inputconditionwidgetstring.h \
           project/private/projectdataitem_impl.h \
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
           pre/geodata/private/geodatacellmappert_detail.h \
           pre/geodata/private/geodatamappert_detail.h \
           pre/geodata/private/geodatanodemappert_detail.h \
           pre/geodatabackground/private/geodatabackgroundcellmappert_detail.h \
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
           pre/gridcond/base/gridattributeeditdialog.h \
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
           project/inputcond/functional/inputconditionwidgetfunctionaldelegate.h \
           project/inputcond/functional/inputconditionwidgetfunctionaldialog.h \
           pre/gridcond/base/private/gridattributecellcontainert_detail.h \
           pre/gridcond/base/private/gridattributecontainert_detail.h \
           pre/gridcond/base/private/gridattributedimensioncontainert_detail.h \
           pre/gridcond/base/private/gridattributeeditwidgett_detail.h \
           pre/gridcond/base/private/gridattributenodecontainert_detail.h
FORMS += datamodel/propertybrowserview.ui \
         distancemeasure/distancemeasurecopypropertydialog.ui \
         measureddata/measureddatapointsettingdialog.ui \
         measureddata/measureddatavectorsettingdialog.ui \
         misc/mousepositionwidget.ui \
         post/postzoneselectingdialog.ui \
         postcontainer/postbaseselectingdialog.ui \
         postcontainer/postdataexportdialog.ui \
         project/backgroundimageinfodialog.ui \
         project/offsetsettingdialog.ui \
         scalarstocolors/colortransferfunctioneditwidget.ui \
         scalarstocolors/lookuptableeditwidget.ui \
         scalarstocolors/scalarstocolorseditdialog.ui \
         pre/complex/gridcomplexconditiondialog.ui \
         pre/complex/gridcomplexconditionwidget.ui \
         pre/geodatabackground/geodatabackgroundeditdialog.ui \
         pre/gridcond/gridcellattributepropertydialog.ui \
         pre/gridcond/gridnodeattributepropertydialog.ui \
         project/inputcond/inputconditiondialog.ui \
         pre/gridcond/base/gridattributeeditdialog.ui \
         pre/gridcond/base/gridattributevariationeditdialog.ui \
         pre/gridcond/dimensionselectwidget/gridattributedimensioncomboboxselectwidget.ui \
         project/inputcond/functional/inputconditionwidgetfunctionaldialog.ui
SOURCES += axis2d/axis2ddataitem.cpp \
           axis3d/axis3ddataitem.cpp \
           base/iricmainwindowinterface.cpp \
           base/propertybrowser.cpp \
           base/snapshotenabledwindowinterface.cpp \
           base/windowwithzindexinterface.cpp \
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
           distancemeasure/distancemeasurecopypropertydialog.cpp \
           distancemeasure/distancemeasuredataitem.cpp \
           distancemeasure/distancemeasuregroupdataitem.cpp \
           measureddata/measureddatafiledataitem.cpp \
           measureddata/measureddatapointdataitem.cpp \
           measureddata/measureddatapointgroupdataitem.cpp \
           measureddata/measureddatapointsettingdialog.cpp \
           measureddata/measureddatavectordataitem.cpp \
           measureddata/measureddatavectorgroupdataitem.cpp \
           measureddata/measureddatavectorsettingdialog.cpp \
           misc/mouseboundingbox.cpp \
           misc/mousepositionwidget.cpp \
           misc/qundocommandhelper.cpp \
           named/namedgraphicswindowdataitemtool.cpp \
           named/namedgraphicwindowdataitem.cpp \
           post/postprocessorwindow.cpp \
           post/postprocessorwindowfactoryinterface.cpp \
           post/postprocessorwindowprojectdataitem.cpp \
           post/postzoneselectingdialog.cpp \
           postcontainer/postabstractsteps.cpp \
           postcontainer/postbaseiterativeseriesdatacontainer.cpp \
           postcontainer/postbaseselectingdialog.cpp \
           postcontainer/postdatacontainer.cpp \
           postcontainer/postdataexportdialog.cpp \
           postcontainer/postdummy3dzonedatacontainer.cpp \
           postcontainer/postexportsetting.cpp \
           postcontainer/postiterationsteps.cpp \
           postcontainer/postseriesdatacontainer.cpp \
           postcontainer/postsolutioninfo.cpp \
           postcontainer/posttimesteps.cpp \
           postcontainer/postzonedatacontainer.cpp \
           postcontainer/postzonepointseriesdatacontainer.cpp \
           project/backgroundimageinfo.cpp \
           project/backgroundimageinfodialog.cpp \
           project/cgnsfilelist.cpp \
           project/colorsource.cpp \
           project/measureddata.cpp \
           project/offsetsettingdialog.cpp \
           project/projectcgnsfile.cpp \
           project/projectdata.cpp \
           project/projectdataitem.cpp \
           project/projectmainfile.cpp \
           project/projectpostprocessors.cpp \
           project/projectworkspace.cpp \
           project/workfolderinfo.cpp \
           scalarstocolors/colortransferfunctioncontainer.cpp \
           scalarstocolors/colortransferfunctioneditdialog.cpp \
           scalarstocolors/colortransferfunctioneditwidget.cpp \
           scalarstocolors/lookuptablecontainer.cpp \
           scalarstocolors/lookuptableeditwidget.cpp \
           scalarstocolors/scalarstocolorscontainer.cpp \
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
           misc/targeted/targeteditemsettargetcommand.cpp \
           misc/targeted/targeteditemsettargetcommandtool.cpp \
           postcontainer/exporter/postzonedatacsvexporter.cpp \
           postcontainer/exporter/postzonedatashapeexporter.cpp \
           postcontainer/exporter/postzonedatavtkexporter.cpp \
           pre/base/preprocessordataitem.cpp \
           pre/base/preprocessordatamodelinterface.cpp \
           pre/base/preprocessorgeodatatopdataiteminterface.cpp \
           pre/base/preprocessorgraphicsviewinterface.cpp \
           pre/base/preprocessorgridandgridcreatingconditiondataiteminterface.cpp \
           pre/base/preprocessorgridcreatingconditiondataiteminterface.cpp \
           pre/base/preprocessorwindowinterface.cpp \
           pre/complex/gridcomplexconditiondialog.cpp \
           pre/complex/gridcomplexconditionwidget.cpp \
           pre/complex/gridcomplexconditionwidgetcontainer.cpp \
           pre/geodata/geodata.cpp \
           pre/geodata/geodatacreator.cpp \
           pre/geodata/geodataexporter.cpp \
           pre/geodata/geodataimporter.cpp \
           pre/geodata/geodatamapper.cpp \
           pre/geodata/geodataproxy.cpp \
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
           project/inputcond/inputconditionwidgetfoldername.cpp \
           project/inputcond/inputconditionwidgetfunctional.cpp \
           project/inputcond/inputconditionwidgetinteger.cpp \
           project/inputcond/inputconditionwidgetintegeroption.cpp \
           project/inputcond/inputconditionwidgetreal.cpp \
           project/inputcond/inputconditionwidgetrealoption.cpp \
           project/inputcond/inputconditionwidgetset.cpp \
           project/inputcond/inputconditionwidgetstring.cpp \
           pre/grid/structured15dgrid/structured15dgridwithcrosssectioncrosssection.cpp \
           pre/gridcond/base/gridattributebaseobject.cpp \
           pre/gridcond/base/gridattributecontainer.cpp \
           pre/gridcond/base/gridattributedimensioncontainer.cpp \
           pre/gridcond/base/gridattributedimensionintegercontainer.cpp \
           pre/gridcond/base/gridattributedimensionrealcontainer.cpp \
           pre/gridcond/base/gridattributedimensionscontainer.cpp \
           pre/gridcond/base/gridattributedimensionselectwidget.cpp \
           pre/gridcond/base/gridattributeeditdialog.cpp \
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
           project/inputcond/functional/inputconditionwidgetfunctionaldelegate.cpp \
           project/inputcond/functional/inputconditionwidgetfunctionaldialog.cpp
RESOURCES += guicore.qrc
TRANSLATIONS += languages/iricGuicore_es_ES.ts \
                languages/iricGuicore_fr_FR.ts \
                languages/iricGuicore_id_ID.ts \
                languages/iricGuicore_ja_JP.ts \
                languages/iricGuicore_ko_KR.ts \
                languages/iricGuicore_ru_RU.ts \
                languages/iricGuicore_th_TH.ts \
                languages/iricGuicore_vi_VN.ts \
                languages/iricGuicore_zh_CN.ts

#include "../../base/iricmainwindowi.h"
#include "../../datamodel/graphicswindowdataitem.h"
#include "../../datamodel/graphicswindowdatamodel.h"
#include "../../datamodel/vtkgraphicsview.h"
#include "../../project/projectdata.h"
#include "../base/preprocessorgraphicsviewi.h"
#include "../base/preprocessorgridtypedataitemi.h"
#include "../base/preprocessorgeodatadataitemi.h"
#include "../base/preprocessorgeodatagroupdataitemi.h"
#include "../base/preprocessorgeodatatopdataitemi.h"
#include "../base/preprocessorwindowi.h"
#include "geodata.h"
#include "geodatacreator.h"

#include <guibase/objectbrowserview.h>
#include <guicore/pre/base/preprocessordatamodeli.h>
#include <guicore/pre/base/preprocessorhydraulicdatatopdataitemi.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/stringtool.h>
#include <misc/zdepthrange.h>

#include <QList>
#include <QMenu>
#include <QStandardItem>
#include <QXmlStreamWriter>

GeoData::Setting::Setting() :
	CompositeContainer ({&name, &caption, &mapped}),
	name {"name"},
	caption {"caption"},
	mapped {"mapped", false}
{}

GeoData::GeoData(ProjectDataItem* d, GeoDataCreator* creator, SolverDefinitionGridAttribute* condition) :
	ProjectDataItem {d},
	m_creator {creator},
	m_gridAttribute {condition}
{
	m_menu = new QMenu(projectData()->mainWindow());
	m_editNameAction = new QAction(GeoData::tr("Edit &Name..."), this);
	connect(m_editNameAction, SIGNAL(triggered()), this, SLOT(editName()));
}
GeoData::~GeoData()
{
	delete m_menu;
}

QString GeoData::name() const
{
	return m_setting.name;
}
void GeoData::setName(const QString& name)
{
	m_setting.name = name;
	updateFilename();
}

const QString& GeoData::typeName() const
{
	return m_creator->typeName();
}

QString GeoData::caption() const
{
	return m_setting.caption;
}

void GeoData::setCaption(const QString& cap)
{
	m_setting.caption = cap;
}

SolverDefinitionGridAttribute* GeoData::gridAttribute() const
{
	return m_gridAttribute;
}

GeoDataCreator* GeoData::creator() const
{
	return m_creator;
}

std::vector<GeoDataImporter*> GeoData::importers() const
{
	if (m_creator == nullptr) {
		std::vector<GeoDataImporter*> l;
		return l;
	}
	return m_creator->importers();
}

std::vector<GeoDataExporter*> GeoData::exporters() const
{
	if (m_creator == nullptr) {
		std::vector<GeoDataExporter*> l;
		return l;
	}
	return m_creator->exporters();
}

void GeoData::setupDataItem()
{
	PreProcessorGeoDataDataItemI* item = dynamic_cast<PreProcessorGeoDataDataItemI*>(parent());
	QString fname = name();
	fname.append(".dat");
	item->setFilename(fname);
	item->standardItem()->setText(caption());
	item->standardItem()->setEditable(true);
	item->standardItem()->setCheckable(true);
	item->standardItem()->setCheckState(Qt::Checked);

	item->copyStandardItem();
}

void GeoData::setupActors()
{}

void GeoData::setupMenu()
{}

void GeoData::showInitialDialog()
{}

bool GeoData::addToolBarButtons(QToolBar*)
{
	return false;
}

void GeoData::applyColorMapSetting()
{}

QMenu* GeoData::menu() const
{
	return m_menu;
}

void GeoData::handleStandardItemChange()
{}

void GeoData::handleStandardItemDoubleClicked()
{}

void GeoData::informSelection(PreProcessorGraphicsViewI*)
{}

void GeoData::informDeselection(PreProcessorGraphicsViewI*)
{}

void GeoData::viewOperationEnded(PreProcessorGraphicsViewI*)
{}

void GeoData::keyPressEvent(QKeyEvent*, PreProcessorGraphicsViewI*)
{}

void GeoData::keyReleaseEvent(QKeyEvent*, PreProcessorGraphicsViewI*)
{}

void GeoData::mouseDoubleClickEvent(QMouseEvent*, PreProcessorGraphicsViewI*)
{}

void GeoData::mouseMoveEvent(QMouseEvent*, PreProcessorGraphicsViewI* )
{}

void GeoData::mousePressEvent(QMouseEvent*, PreProcessorGraphicsViewI*)
{}

void GeoData::mouseReleaseEvent(QMouseEvent*, PreProcessorGraphicsViewI*)
{}

void GeoData::addCustomMenuItems(QMenu*)
{}

void GeoData::updateZDepthRangeItemCount(ZDepthRange& range)
{
	range.setItemCount(0);
}

void GeoData::assignActorZValues(const ZDepthRange&)
{}

bool GeoData::getValueRange(double*, double*)
{
	return false;
}

void GeoData::showPropertyDialog()
{
	showPropertyDialogModal();
}

QDialog* GeoData::propertyDialog(QWidget*)
{
	return nullptr;
}

void GeoData::handlePropertyDialogAccepted(QDialog*)
{}

ColorMapSettingContainerI* GeoData::colorMapSettingContainer() const
{
	return geoDataDataItem()->colorMapSettingContainer();
}

void GeoData::update2Ds()
{}

bool GeoData::isVisible() const
{
	auto item = geoDataDataItem();
	return item->isAncientChecked() && item->standardItem()->checkState() == Qt::Checked;
}

bool GeoData::isMapped() const
{
	return m_setting.mapped;
}

void GeoData::setMapped(bool mapped)
{
	m_setting.mapped = mapped;
}

GeoDataProxy* GeoData::getProxy()
{
	return nullptr;
}

void GeoData::viewOperationEndedGlobal(PreProcessorGraphicsViewI*)
{}

void GeoData::applyOffset(double x, double y)
{
	doApplyOffset(x, y);
}

void GeoData::handleDimensionCurrentIndexChange(int, int)
{}

void GeoData::handleDimensionValuesChange(const std::vector<QVariant>&, const std::vector<QVariant>&)
{}

PreProcessorWindowI* GeoData::preProcessorWindow()
{
	return dynamic_cast<PreProcessorWindowI*>(geoDataDataItem()->mainWindow());
}


PreProcessorGeoDataTopDataItemI* GeoData::geoDataTopDataItem() const
{
	return dynamic_cast<PreProcessorGeoDataTopDataItemI*>(geoDataGroupDataItem()->parent());
}

PreProcessorGeoDataGroupDataItemI* GeoData::geoDataGroupDataItem() const
{
	return dynamic_cast<PreProcessorGeoDataGroupDataItemI*>(geoDataDataItem()->parent());
}

PreProcessorGeoDataDataItemI* GeoData::geoDataDataItem() const
{
	return dynamic_cast<PreProcessorGeoDataDataItemI*>(parent());
}

PreProcessorGridTypeDataItemI* GeoData::gridTypeDataItem() const
{
	return dynamic_cast<PreProcessorGridTypeDataItemI*>(geoDataTopDataItem()->parent());
}

PreProcessorHydraulicDataTopDataItemI* GeoData::hydraulicDataTopDataItem() const
{
	auto gridTypeName = geoDataTopDataItem()->gridType()->name();
	return dataModel()->hydraulicDataTopDataItem(gridTypeName);
}

PreProcessorHydraulicDataGroupDataItemI* GeoData::hydraulicDataGroupDataItem(const std::string& name) const
{
	auto topDataItem = hydraulicDataTopDataItem();
	if (topDataItem == nullptr) {return nullptr;}

	return topDataItem->groupDataItem(name);
}

void GeoData::pushCommand(QUndoCommand* com)
{
	geoDataDataItem()->pushCommand(com);
}

void GeoData::pushRenderCommand(QUndoCommand* com)
{
	geoDataDataItem()->pushRenderCommand(com, geoDataDataItem());
}

void GeoData::pushRenderRedoOnlyCommand(QUndoCommand* com)
{
	geoDataDataItem()->pushRenderRedoOnlyCommand(com, geoDataDataItem());
}

void GeoData::showPropertyDialogModal()
{
	QDialog* propDialog = propertyDialog(preProcessorWindow());
	if (propDialog == nullptr) {return;}
	int result = propDialog->exec();
	if (result == QDialog::Accepted) {
		handlePropertyDialogAccepted(propDialog);
	}
	delete propDialog;
}

void GeoData::showPropertyDialogModeless()
{
	QDialog* propDialog = propertyDialog(preProcessorWindow());
	if (propDialog == nullptr) {return;}
	propDialog->setAttribute(Qt::WA_DeleteOnClose);
	connect(propDialog, &QObject::destroyed, iricMainWindow(), &iRICMainWindowI::exitModelessDialogMode);

	iricMainWindow()->enterModelessDialogMode();

	propDialog->show();
}

PreProcessorGraphicsViewI* GeoData::graphicsView()
{
	return dynamic_cast<PreProcessorGraphicsViewI*>(preProcessorWindow()->centralWidget());
}

vtkRenderer* GeoData::renderer()
{
	return geoDataDataItem()->renderer();
}

void GeoData::renderGraphicsView()
{
	geoDataDataItem()->renderGraphicsView();
}

vtkActorCollection* GeoData::actorCollection()
{
	return geoDataDataItem()->actorCollection();
}

vtkActor2DCollection* GeoData::actor2DCollection()
{
	return geoDataDataItem()->actor2DCollection();
}

void GeoData::updateActorSetting()
{}

void GeoData::editName()
{
	ObjectBrowserView* view = dataModel()->objectBrowserView();
	view->edit(geoDataDataItem()->standardItem()->index());
}

QAction* GeoData::deleteAction()
{
	return dataModel()->objectBrowserView()->deleteAction();
}

MouseBoundingBox* GeoData::mouseBoundingBox()
{
	return geoDataDataItem()->mouseBoundingBox();
}

PreProcessorDataModelI* GeoData::dataModel() const
{
	return geoDataDataItem()->dataModel();
}

void GeoData::updateVisibility()
{
	geoDataDataItem()->updateVisibility();
}

void GeoData::updateVisibilityWithoutRendering()
{
	geoDataDataItem()->updateVisibilityWithoutRendering();
}

void GeoData::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_setting.load(node);
	updateFilename();
}

void GeoData::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	m_setting.save(writer);
}

iRICLib::H5CgnsGeographicDataGroup::Type GeoData::iRICLibType() const
{
	return iRICLib::H5CgnsGeographicDataGroup::Type::Unknown;
}

GridAttributeDimensionsContainer* GeoData::dimensions() const
{
	auto item = geoDataGroupDataItem();
	if (item == nullptr) {return nullptr;}
	return item->dimensions();
}

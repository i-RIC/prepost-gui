#include "../../base/iricmainwindowinterface.h"
#include "../../datamodel/graphicswindowdataitem.h"
#include "../../datamodel/graphicswindowdatamodel.h"
#include "../../datamodel/vtkgraphicsview.h"
#include "../../project/projectdata.h"
#include "../base/preprocessorgraphicsviewinterface.h"
#include "../base/preprocessorgridtypedataiteminterface.h"
#include "../base/preprocessorgeodatadataiteminterface.h"
#include "../base/preprocessorgeodatagroupdataiteminterface.h"
#include "../base/preprocessorgeodatatopdataiteminterface.h"
#include "../base/preprocessorwindowinterface.h"
#include "geodata.h"
#include "geodatacreator.h"

#include <guibase/objectbrowserview.h>
#include <guicore/pre/base/preprocessordatamodelinterface.h>
#include <guicore/pre/base/preprocessorhydraulicdatatopdataiteminterface.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/stringtool.h>
#include <misc/zdepthrange.h>

#include <QList>
#include <QMenu>
#include <QStandardItem>
#include <QXmlStreamWriter>

#include <cgnslib.h>

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
	mapperFunc = &GeoData::nodeMappers;
	if (condition != nullptr && condition->position() == SolverDefinitionGridAttribute::CellCenter) {
		mapperFunc = &GeoData::cellMappers;
	}
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

void GeoData::setPosition(SolverDefinitionGridAttribute::Position pos)
{
	if (pos == SolverDefinitionGridAttribute::Node) {
		mapperFunc = &GeoData::nodeMappers;
	} else {
		mapperFunc = &GeoData::cellMappers;
	}
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

GeoDataMapper* GeoData::mapper() const
{
	return m_mapper;
}

void GeoData::setMapper(GeoDataMapper* m)
{
	m_mapper = m;
}

std::vector<GeoDataMapper*> GeoData::mappers() const
{
	return (this->*mapperFunc)();
}

void GeoData::setDefaultMapper()
{
	auto tmpmappers = mappers();
	if (tmpmappers.size() == 0) {m_mapper = nullptr;}
	m_mapper = tmpmappers[0];
}

std::vector<GeoDataMapper*> GeoData::nodeMappers() const
{
	if (m_creator == nullptr) {
		std::vector<GeoDataMapper*> l;
		return l;
	}
	return m_creator->nodeMappers();
}

std::vector<GeoDataMapper*> GeoData::cellMappers() const
{
	if (m_creator == nullptr) {
		std::vector<GeoDataMapper*> l;
		return l;
	}
	return m_creator->cellMappers();
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
	PreProcessorGeoDataDataItemInterface* item = dynamic_cast<PreProcessorGeoDataDataItemInterface*>(parent());
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

QMenu* GeoData::menu() const
{
	return m_menu;
}

void GeoData::handleStandardItemChange()
{}

void GeoData::handleStandardItemDoubleClicked()
{}

void GeoData::informSelection(PreProcessorGraphicsViewInterface*)
{}

void GeoData::informDeselection(PreProcessorGraphicsViewInterface*)
{}

void GeoData::viewOperationEnded(PreProcessorGraphicsViewInterface*)
{}

void GeoData::keyPressEvent(QKeyEvent*, PreProcessorGraphicsViewInterface*)
{}

void GeoData::keyReleaseEvent(QKeyEvent*, PreProcessorGraphicsViewInterface*)
{}

void GeoData::mouseDoubleClickEvent(QMouseEvent*, PreProcessorGraphicsViewInterface*)
{}

void GeoData::mouseMoveEvent(QMouseEvent*, PreProcessorGraphicsViewInterface* )
{}

void GeoData::mousePressEvent(QMouseEvent*, PreProcessorGraphicsViewInterface*)
{}

void GeoData::mouseReleaseEvent(QMouseEvent*, PreProcessorGraphicsViewInterface*)
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

QDialog* GeoData::propertyDialog(QWidget*)
{
	return nullptr;
}

void GeoData::handlePropertyDialogAccepted(QDialog*)
{}

ScalarsToColorsContainer* GeoData::scalarsToColorsContainer()
{
	ProjectDataItem* groupdi1 = parent()->parent();
	PreProcessorGeoDataGroupDataItemInterface* groupdi2 = dynamic_cast<PreProcessorGeoDataGroupDataItemInterface*>(groupdi1);
	if (groupdi2 == nullptr) {return nullptr;}
	PreProcessorGridTypeDataItemInterface* typedi = dynamic_cast<PreProcessorGridTypeDataItemInterface*>(groupdi1->parent()->parent());
	ScalarsToColorsContainer* stc = typedi->scalarsToColors(groupdi2->condition()->name());
	return stc;
}

void GeoData::update2Ds()
{}

bool GeoData::isVisible() const
{
	PreProcessorGeoDataDataItemInterface* item = dynamic_cast<PreProcessorGeoDataDataItemInterface*>(parent());
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

void GeoData::saveToCgnsFile(const int)
{
	QString filename = relativeFilename();
	cgsize_t strlen = filename.length() + 1;
	int type = iRICLibType();
	cgsize_t valuelen = 1;
	cg_array_write("filename", Character, 1, &strlen, iRIC::toStr(filename).c_str());
	cg_array_write("type", Integer, 1, &valuelen, &type);
}

void GeoData::viewOperationEndedGlobal(PreProcessorGraphicsViewInterface*)
{}

void GeoData::applyOffset(double x, double y)
{
	doApplyOffset(x, y);
}

bool GeoData::requestCoordinateSystem() const
{
	return false;
}

void GeoData::handleDimensionCurrentIndexChange(int, int)
{}

void GeoData::handleDimensionValuesChange(const std::vector<QVariant>&, const std::vector<QVariant>&)
{}

PreProcessorWindowInterface* GeoData::preProcessorWindow()
{
	return dynamic_cast<PreProcessorWindowInterface*>(geoDataDataItem()->mainWindow());
}


PreProcessorGeoDataTopDataItemInterface* GeoData::geoDataTopDataItem() const
{
	return dynamic_cast<PreProcessorGeoDataTopDataItemInterface*>(geoDataGroupDataItem()->parent());
}

PreProcessorGeoDataGroupDataItemInterface* GeoData::geoDataGroupDataItem() const
{
	return dynamic_cast<PreProcessorGeoDataGroupDataItemInterface*>(geoDataDataItem()->parent());
}

PreProcessorGeoDataDataItemInterface* GeoData::geoDataDataItem() const
{
	return dynamic_cast<PreProcessorGeoDataDataItemInterface*>(parent());
}

PreProcessorHydraulicDataTopDataItemInterface* GeoData::hydraulicDataTopDataItem() const
{
	auto gridTypeName = geoDataTopDataItem()->gridType()->name();
	return dataModel()->hydraulicDataTopDataItem(gridTypeName);
}

PreProcessorHydraulicDataGroupDataItemInterface* GeoData::hydraulicDataGroupDataItem(const std::string& name) const
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

PreProcessorGraphicsViewInterface* GeoData::graphicsView()
{
	return dynamic_cast<PreProcessorGraphicsViewInterface*>(preProcessorWindow()->centralWidget());
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

void GeoData::editName()
{
	ObjectBrowserView* view = dataModel()->objectBrowserView();
	view->edit(geoDataDataItem()->standardItem()->index());
}

QAction* GeoData::deleteAction()
{
	PreProcessorGeoDataDataItemInterface* item = dynamic_cast<PreProcessorGeoDataDataItemInterface*>(parent());
	return item->dataModel()->objectBrowserView()->deleteAction();
}

MouseBoundingBox* GeoData::mouseBoundingBox()
{
	PreProcessorGeoDataDataItemInterface* r = dynamic_cast<PreProcessorGeoDataDataItemInterface*>(parent());
	return r->mouseBoundingBox();
}

PreProcessorDataModelInterface* GeoData::dataModel() const
{
	PreProcessorGeoDataDataItemInterface* item = dynamic_cast<PreProcessorGeoDataDataItemInterface*>(parent());
	return item->dataModel();
}

void GeoData::updateVisibility()
{
	PreProcessorGeoDataDataItemInterface* item = dynamic_cast<PreProcessorGeoDataDataItemInterface*>(parent());
	item->updateVisibility();
}

void GeoData::updateVisibilityWithoutRendering()
{
	PreProcessorGeoDataDataItemInterface* item = dynamic_cast<PreProcessorGeoDataDataItemInterface*>(parent());
	item->updateVisibilityWithoutRendering();
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

int GeoData::iRICLibType() const
{
	return IRIC_GEO_UNKNOWN;
}

GridAttributeDimensionsContainer* GeoData::dimensions() const
{
	PreProcessorGeoDataGroupDataItemInterface* item = dynamic_cast<PreProcessorGeoDataGroupDataItemInterface*>(parent()->parent());
	if (item == nullptr) {return nullptr;}
	return item->dimensions();
}

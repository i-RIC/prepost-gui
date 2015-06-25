#include "../../base/iricmainwindowinterface.h"
#include "../../datamodel/graphicswindowdataitem.h"
#include "../../datamodel/graphicswindowdatamodel.h"
#include "../../datamodel/vtkgraphicsview.h"
#include "../../project/projectdata.h"
#include "../base/preprocessorgraphicsviewinterface.h"
#include "../base/preprocessorgridtypedataiteminterface.h"
#include "../base/preprocessorgeodatadataiteminterface.h"
#include "../base/preprocessorgeodatadataiteminterface.h"
#include "../base/preprocessorgeodatagroupdataiteminterface.h"
#include "geodata.h"
#include "geodatacreator.h"

#include <guibase/objectbrowserview.h>
#include <misc/stringtool.h>
#include <misc/zdepthrange.h>

#include <QList>
#include <QMenu>
#include <QStandardItem>
#include <QXmlStreamWriter>

#include <cgnslib.h>

GeoData::GeoData(ProjectDataItem* d, GeoDataCreator* creator, SolverDefinitionGridAttribute* condition)
	: ProjectDataItem(d)
{
	m_creator = creator;
	m_gridRelatedCondition = condition;
	m_name = "";
	m_caption = "";

	m_menu = new QMenu(projectData()->mainWindow());
	m_editNameAction = new QAction(tr("Edit &Name..."), this);
	connect(m_editNameAction, SIGNAL(triggered()), this, SLOT(editName()));
	mapperFunc = &GeoData::nodeMappers;
	if (condition != nullptr && condition->position() == SolverDefinitionGridAttribute::CellCenter) {
		mapperFunc = &GeoData::cellMappers;
	}
	m_mapped = false;
}
GeoData::~GeoData()
{
	delete m_menu;
}

const QString& GeoData::typeName() const
{
	return m_creator->typeName();
}

QList<GeoDataMapper*> GeoData::nodeMappers() const
{
	if (m_creator == nullptr) {
		QList<GeoDataMapper*> l;
		return l;
	}
	return m_creator->nodeMappers();
}

QList<GeoDataMapper*> GeoData::cellMappers() const
{
	if (m_creator == nullptr) {
		QList<GeoDataMapper*> l;
		return l;
	}
	return m_creator->cellMappers();
}

QList<GeoDataImporter*> GeoData::importers()
{
	if (m_creator == nullptr) {
		QList<GeoDataImporter*> l;
		return l;
	}
	return m_creator->importers();
}

QList<GeoDataExporter*> GeoData::exporters()
{
	if (m_creator == nullptr) {
		QList<GeoDataExporter*> l;
		return l;
	}
	return m_creator->exporters();
}

const QIcon GeoData::icon() const
{
	if (m_creator == nullptr) {return QIcon();}
	return m_creator->icon();
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

PreProcessorWindowInterface* GeoData::preProcessorWindow()
{
	PreProcessorGeoDataDataItemInterface* item = dynamic_cast<PreProcessorGeoDataDataItemInterface*>(parent());
	return dynamic_cast<PreProcessorWindowInterface*>(item->mainWindow());
}

PreProcessorGraphicsViewInterface* GeoData::graphicsView()
{
	return dynamic_cast<PreProcessorGraphicsViewInterface*>(preProcessorWindow()->centralWidget());
}

vtkRenderer* GeoData::renderer()
{
	PreProcessorGeoDataDataItemInterface* item = dynamic_cast<PreProcessorGeoDataDataItemInterface*>(parent());
	return item->renderer();
}

void GeoData::renderGraphicsView()
{
	PreProcessorGeoDataDataItemInterface* item = dynamic_cast<PreProcessorGeoDataDataItemInterface*>(parent());
	item->renderGraphicsView();
}

vtkActorCollection* GeoData::actorCollection()
{
	PreProcessorGeoDataDataItemInterface* item = dynamic_cast<PreProcessorGeoDataDataItemInterface*>(parent());
	return item->m_actorCollection;
}

vtkActor2DCollection* GeoData::actor2DCollection()
{
	PreProcessorGeoDataDataItemInterface* item = dynamic_cast<PreProcessorGeoDataDataItemInterface*>(parent());
	return item->m_actor2DCollection;
}

void GeoData::setDefaultMapper()
{
	QList<GeoDataMapper*> tmpmappers = mappers();
	if (tmpmappers.count() == 0) {m_mapper = nullptr;}
	m_mapper = *(tmpmappers.begin());
}

void GeoData::editName()
{
	PreProcessorGeoDataDataItemInterface* item = dynamic_cast<PreProcessorGeoDataDataItemInterface*>(parent());
	ObjectBrowserView* view = dataModel()->objectBrowserView();
	view->edit(item->m_standardItem->index());
}

void GeoData::loadName(const QDomNode& node)
{
	setName(node.toElement().attribute("name"));
}

void GeoData::saveName(QXmlStreamWriter& writer)
{
	writer.writeAttribute("name", m_name);
}

void GeoData::updateZDepthRangeItemCount(ZDepthRange& range)
{
	range.setItemCount(0);
}

ScalarsToColorsContainer* GeoData::scalarsToColorsContainer()
{
	ProjectDataItem* groupdi1 = parent()->parent();
	PreProcessorGeoDataGroupDataItemInterface* groupdi2 = dynamic_cast<PreProcessorGeoDataGroupDataItemInterface*>(groupdi1);
	if (groupdi2 == nullptr) {return nullptr;}
	PreProcessorGridTypeDataItemInterface* typedi = dynamic_cast<PreProcessorGridTypeDataItemInterface*>(groupdi1->parent()->parent());
	ScalarsToColorsContainer* stc = typedi->scalarsToColors(groupdi2->condition()->name());
	return stc;
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

PreProcessorDataModelInterface* GeoData::dataModel()
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

bool GeoData::isVisible()
{
	PreProcessorGeoDataDataItemInterface* item = dynamic_cast<PreProcessorGeoDataDataItemInterface*>(parent());
	return item->isAncientChecked() && item->standardItem()->checkState() == Qt::Checked;
}

void GeoData::doLoadFromProjectMainFile(const QDomNode& node)
{
	int mapped = node.toElement().attribute("mapped", "1").toInt();
	m_mapped = static_cast<bool>(mapped);
	loadName(node);
	m_caption = node.toElement().attribute("caption");
}

void GeoData::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeAttribute("mapped", QString::number(static_cast<int>(m_mapped)));
	saveName(writer);
	writer.writeAttribute("caption", m_caption);
}

void GeoData::saveToCgnsFile(const int /*fn*/)
{
	QString filename = relativeFilename();
	cgsize_t strlen = filename.length() + 1;
	int type = iRICLibType();
	cgsize_t valuelen = 1;
	cg_array_write("filename", Character, 1, &strlen, iRIC::toStr(filename).c_str());
	cg_array_write("type", Integer, 1, &valuelen, &type);
}

GridAttributeDimensionsContainer* GeoData::dimensions() const
{
	PreProcessorGeoDataGroupDataItemInterface* item = dynamic_cast<PreProcessorGeoDataGroupDataItemInterface*>(parent()->parent());
	if (item == nullptr) {return nullptr;}
	return item->dimensions();
}

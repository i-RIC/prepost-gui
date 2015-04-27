#include "rawdata.h"
#include "rawdatacreator.h"
#include "../../datamodel/vtkgraphicsview.h"
#include "../../datamodel/graphicswindowdataitem.h"
#include "../../datamodel/graphicswindowdatamodel.h"
#include "../../base/iricmainwindowinterface.h"
#include "../../project/projectdata.h"
#include "../base/preprocessorrawdatadataiteminterface.h"
#include "../base/preprocessorrawdatagroupdataiteminterface.h"
#include "../base/preprocessorgridtypedataiteminterface.h"
#include "../base/preprocessorrawdatadataiteminterface.h"
#include "../base/preprocessorgraphicsviewinterface.h"

#include <guibase/objectbrowserview.h>
#include <misc/stringtool.h>
#include <misc/zdepthrange.h>
#include <QMenu>
#include <QList>
#include <QStandardItem>
#include <QXmlStreamWriter>

#include <cgnslib.h>

RawData::RawData(ProjectDataItem *d, RawDataCreator* creator, SolverDefinitionGridRelatedCondition* condition)
	: ProjectDataItem(d)
{
	m_creator = creator;
	m_gridRelatedCondition = condition;
	m_name = "";
	m_caption = "";

	m_menu = new QMenu(projectData()->mainWindow());
	m_editNameAction = new QAction(tr("Edit &Name..."), this);
	connect(m_editNameAction, SIGNAL(triggered()), this, SLOT(editName()));
	mapperFunc = &RawData::nodeMappers;
	if (condition != nullptr && condition->position() == SolverDefinitionGridRelatedCondition::CellCenter){
		mapperFunc = &RawData::cellMappers;
	}
	m_mapped = false;
}
RawData::~RawData()
{
	delete m_menu;
}

const QString& RawData::typeName() const
{
	return m_creator->typeName();
}

QList<RawDataMapper*> RawData::nodeMappers()
{
	if (m_creator == nullptr){
		QList<RawDataMapper*> l;
		return l;
	}
	return m_creator->nodeMappers();
}

QList<RawDataMapper*> RawData::cellMappers()
{
	if (m_creator == nullptr){
		QList<RawDataMapper*> l;
		return l;
	}
	return m_creator->cellMappers();
}

QList<RawDataImporter*> RawData::importers()
{
	if (m_creator == nullptr){
		QList<RawDataImporter*> l;
		return l;
	}
	return m_creator->importers();
}

QList<RawDataExporter*> RawData::exporters()
{
	if (m_creator == nullptr){
		QList<RawDataExporter*> l;
		return l;
	}
	return m_creator->exporters();
}

const QIcon RawData::icon() const
{
	if (m_creator == nullptr){return QIcon();}
	return m_creator->icon();
}

void RawData::setupDataItem()
{
	PreProcessorRawdataDataItemInterface* item = dynamic_cast<PreProcessorRawdataDataItemInterface*>(parent());
	QString fname = name();
	fname.append(".dat");
	item->setFilename(fname);
	item->standardItem()->setText(caption());
	item->standardItem()->setEditable(true);
	item->standardItem()->setCheckable(true);
	item->standardItem()->setCheckState(Qt::Checked);

	item->copyStandardItem();
}

PreProcessorWindowInterface* RawData::preProcessorWindow()
{
	PreProcessorRawdataDataItemInterface* item = dynamic_cast<PreProcessorRawdataDataItemInterface*>(parent());
	return dynamic_cast<PreProcessorWindowInterface*>(item->mainWindow());
}

PreProcessorGraphicsViewInterface* RawData::graphicsView()
{
	return dynamic_cast<PreProcessorGraphicsViewInterface*>(preProcessorWindow()->centralWidget());
}

vtkRenderer* RawData::renderer()
{
	PreProcessorRawdataDataItemInterface* item = dynamic_cast<PreProcessorRawdataDataItemInterface*>(parent());
	return item->renderer();
}

void RawData::renderGraphicsView()
{
	PreProcessorRawdataDataItemInterface* item = dynamic_cast<PreProcessorRawdataDataItemInterface*>(parent());
	item->renderGraphicsView();
}

vtkActorCollection* RawData::actorCollection()
{
	PreProcessorRawdataDataItemInterface* item = dynamic_cast<PreProcessorRawdataDataItemInterface*>(parent());
	return item->m_actorCollection;
}

vtkActor2DCollection* RawData::actor2DCollection()
{
	PreProcessorRawdataDataItemInterface* item = dynamic_cast<PreProcessorRawdataDataItemInterface*>(parent());
	return item->m_actor2DCollection;
}

void RawData::setDefaultMapper()
{
	QList<RawDataMapper*> tmpmappers = mappers();
	if (tmpmappers.count() == 0){m_mapper = nullptr;}
	m_mapper = *(tmpmappers.begin());
}

void RawData::editName()
{
	PreProcessorRawdataDataItemInterface* item = dynamic_cast<PreProcessorRawdataDataItemInterface*>(parent());
	ObjectBrowserView* view = dataModel()->objectBrowserView();
	view->edit(item->m_standardItem->index());
}

void RawData::loadName(const QDomNode& node)
{
	setName(node.toElement().attribute("name"));
}

void RawData::saveName(QXmlStreamWriter& writer)
{
	writer.writeAttribute("name", m_name);
}

void RawData::updateZDepthRangeItemCount(ZDepthRange& range)
{
	range.setItemCount(0);
}

ScalarsToColorsContainer* RawData::scalarsToColorsContainer()
{
	ProjectDataItem* groupdi1 = parent()->parent();
	PreProcessorRawDataGroupDataItemInterface* groupdi2 = dynamic_cast<PreProcessorRawDataGroupDataItemInterface*>(groupdi1);
	if (groupdi2 == nullptr){return nullptr;}
	PreProcessorGridTypeDataItemInterface* typedi = dynamic_cast<PreProcessorGridTypeDataItemInterface*>(groupdi1->parent()->parent());
	ScalarsToColorsContainer* stc = typedi->scalarsToColors(groupdi2->condition()->name());
	return stc;
}

QAction* RawData::deleteAction()
{
	PreProcessorRawdataDataItemInterface* item = dynamic_cast<PreProcessorRawdataDataItemInterface*>(parent());
	return item->dataModel()->objectBrowserView()->deleteAction();
}

MouseBoundingBox* RawData::mouseBoundingBox()
{
	PreProcessorRawdataDataItemInterface* r = dynamic_cast<PreProcessorRawdataDataItemInterface*>(parent());
	return r->mouseBoundingBox();
}

PreProcessorDataModelInterface* RawData::dataModel()
{
	PreProcessorRawdataDataItemInterface* item = dynamic_cast<PreProcessorRawdataDataItemInterface*>(parent());
	return item->dataModel();
}

void RawData::updateVisibility()
{
	PreProcessorRawdataDataItemInterface* item = dynamic_cast<PreProcessorRawdataDataItemInterface*>(parent());
	item->updateVisibility();
}

void RawData::updateVisibilityWithoutRendering()
{
	PreProcessorRawdataDataItemInterface* item = dynamic_cast<PreProcessorRawdataDataItemInterface*>(parent());
	item->updateVisibilityWithoutRendering();
}

bool RawData::isVisible()
{
	PreProcessorRawdataDataItemInterface* item = dynamic_cast<PreProcessorRawdataDataItemInterface*>(parent());
	return item->isAncientChecked() && item->standardItem()->checkState() == Qt::Checked;
}

void RawData::doLoadFromProjectMainFile(const QDomNode& node)
{
	int mapped = node.toElement().attribute("mapped", "1").toInt();
	m_mapped = static_cast<bool>(mapped);
	loadName(node);
	m_caption = node.toElement().attribute("caption");
}

void RawData::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeAttribute("mapped", QString::number(static_cast<int>(m_mapped)));
	saveName(writer);
	writer.writeAttribute("caption", m_caption);
}

void RawData::saveToCgnsFile(const int /*fn*/)
{
	QString filename = relativeFilename();
	cgsize_t strlen = filename.length() + 1;
	int type = iRICLibType();
	cgsize_t valuelen = 1;
	cg_array_write("filename", Character, 1, &strlen, iRIC::toStr(filename).c_str());
	cg_array_write("type", Integer, 1, &valuelen, &type);
}

GridRelatedConditionDimensionsContainer* RawData::dimensions() const
{
	PreProcessorRawDataGroupDataItemInterface* item = dynamic_cast<PreProcessorRawDataGroupDataItemInterface*>(parent()->parent());
	if (item == nullptr){return nullptr;}
	return item->dimensions();
}

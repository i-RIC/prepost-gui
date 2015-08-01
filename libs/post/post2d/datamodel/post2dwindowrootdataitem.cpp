#include "../post2dobjectbrowser.h"
#include "../post2dwindow.h"
#include "../post2dwindowdatamodel.h"
#include "../post2dwindowdatamodel.h"
#include "../post2dwindowgraphicsview.h"
#include "post2dwindowbackgroundimagesdataitem.h"
#include "post2dwindowgridtypedataitem.h"
#include "post2dwindowmeasureddatatopdataitem.h"
#include "post2dwindowrootdataitem.h"

#include <guibase/objectbrowserview.h>
#include <guicore/axis2d/axis2ddataitem.h>
#include <guicore/datamodel/attributebrowsertargetdataitem.h>
#include <guicore/distancemeasure/distancemeasuregroupdataitem.h>
#include <guicore/postcontainer/postsolutioninfo.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/project/projectdata.h>
#include <guicore/solverdef/solverdefinition.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/xmlsupport.h>
#include <postbase/time/posttimedataitem.h>
#include <postbase/title/posttitledataitem.h>

#include <QStandardItemModel>
#include <QTime>
#include <QXmlStreamWriter>

#include <vtkRenderWindow.h>

Post2dWindowRootDataItem::Post2dWindowRootDataItem(Post2dWindow* window, ProjectDataItem* parent) :
	GraphicsWindowRootDataItem {window, parent}
{
	SolverDefinition* def = projectData()->solverDefinition();
	PostSolutionInfo* post = dynamic_cast<Post2dWindowDataModel*>(dataModel())->postSolutionInfo();

	const QList<SolverDefinitionGridType*>& types = def->gridTypes();
	// build grid type data items.
	QList<Post2dWindowGridTypeDataItem*> gtitems;
	for (auto it = types.begin(); it != types.end(); ++it) {
		Post2dWindowGridTypeDataItem* item = new Post2dWindowGridTypeDataItem(*it, this);
		m_gridTypeDataItems.append(item);
		gtitems.append(item);
	}
	// create grid type data item for dummy grid type if needed.
	const QList<PostZoneDataContainer*>& conts = post->zoneContainers2D();
	bool needDummy = false;
	for (int i = 0; i < conts.count(); ++i) {
		PostZoneDataContainer* c = conts.at(i);
		needDummy = needDummy || (c->gridType() == def->dummyGridType());
	}
	if (needDummy) {
		Post2dWindowGridTypeDataItem* item = new Post2dWindowGridTypeDataItem(def->dummyGridType(), this);
		m_gridTypeDataItems.append(item);
		gtitems.append(item);
	}
	// Measured values data item node.
	m_measuredDataTopDataItem = new Post2dWindowMeasuredDataTopDataItem(this);
	m_childItems.append(m_measuredDataTopDataItem);

	for (int j = 0; j < gtitems.count(); ++j) {
		m_childItems.append(gtitems.at(j));
	}
	// Background images data item node.
	m_backgroundImagesDataItem = new Post2dWindowBackgroundImagesDataItem(this);
	m_childItems.append(m_backgroundImagesDataItem);

	m_titleDataItem = new PostTitleDataItem(this);
	m_childItems.append(m_titleDataItem);

	m_timeDataItem = new PostTimeDataItem(this);
	m_childItems.append(m_timeDataItem);

	m_axesDataItem = new Axis2dDataItem(this);
	m_childItems.append(m_axesDataItem);

	m_distanceMeasureGroupDataItem = new DistanceMeasureGroupDataItem(this);
	m_childItems.push_front(m_distanceMeasureGroupDataItem);

	m_attributeBrowserTargetDataItem = new AttributeBrowserTargetDataItem(this);
	m_childItems.push_front(m_attributeBrowserTargetDataItem);

	updateZDepthRangeItemCount();
	// update item map initially.
	updateItemMap();

	connect(post, SIGNAL(zoneList2DUpdated()), this, SLOT(updateZoneList()));
	connect(post, SIGNAL(currentStepUpdated()), this, SLOT(update()));
}

Post2dWindowRootDataItem::~Post2dWindowRootDataItem()
{
	delete m_measuredDataTopDataItem;
	delete m_backgroundImagesDataItem;

	for (auto it = m_gridTypeDataItems.begin(); it != m_gridTypeDataItems.end(); ++it) {
		delete *it;
	}
	delete m_titleDataItem;
	delete m_timeDataItem;
	delete m_axesDataItem;
	delete m_distanceMeasureGroupDataItem;
	delete m_attributeBrowserTargetDataItem;
}

void Post2dWindowRootDataItem::setupStandardModel(QStandardItemModel* model)
{
	model->clear();

	// add gridtypes.
	for (auto it = m_gridTypeDataItems.begin(); it != m_gridTypeDataItems.end(); ++it) {
		model->appendRow((*it)->standardItem());
	}
	// add measured data item row.
	model->appendRow(m_measuredDataTopDataItem->standardItem());
	// add background item row.
	model->appendRow(m_backgroundImagesDataItem->standardItem());
	// add title item row.
	model->appendRow(m_titleDataItem->standardItem());
	// add time item row.
	model->appendRow(m_timeDataItem->standardItem());
	// add axes item row.
	model->appendRow(m_axesDataItem->standardItem());
	// add distance measure
	model->appendRow(m_distanceMeasureGroupDataItem->standardItem());

	emit standardModelSetuped();
}

void Post2dWindowRootDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	for (auto it = m_gridTypeDataItems.begin(); it != m_gridTypeDataItems.end(); ++it) {
		QDomNode c = node.firstChild();
		while (! c.isNull()) {
			if (c.nodeName() == "GridType" && c.toElement().attribute("name") == (*it)->name()) {
				(*it)->loadFromProjectMainFile(c);
			}
			c = c.nextSibling();
		}
	}
	SolverDefinition* def = projectData()->solverDefinition();
	const SolverDefinitionGridType* firstType = *(def->gridTypes().begin());
	if (def->gridTypes().count() == 1 && !(firstType->multiple())) {
		// Current solver support only one grid type, and it does not allow multiple grids to input.
		// The only, and hidden gridtype node should be checked always.
		Post2dWindowGridTypeDataItem* gtItem = *(m_gridTypeDataItems.begin());
		gtItem->standardItem()->setCheckState(Qt::Checked);
	}
	QDomNode titleNode = iRIC::getChildNode(node, "Title");
	if (! titleNode.isNull()) {m_titleDataItem->loadFromProjectMainFile(titleNode);}
	QDomNode timeNode = iRIC::getChildNode(node, "Time");
	if (! timeNode.isNull()) {m_timeDataItem->loadFromProjectMainFile(timeNode);}
	QDomNode dmNode = iRIC::getChildNode(node, "DistanceMeasures");
	if (! dmNode.isNull()) {m_distanceMeasureGroupDataItem->loadFromProjectMainFile(dmNode);}
	updateItemMap();
	updateZDepthRange();
}
void Post2dWindowRootDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
//	writer.writeStartElement("BackgroundImages");
//	m_backgroundImagesDataItem->saveToProjectMainFile(writer);
//	writer.writeEndElement();

	for (auto it = m_gridTypeDataItems.begin(); it != m_gridTypeDataItems.end(); ++it) {
		writer.writeStartElement("GridType");
		(*it)->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
	writer.writeStartElement("Title");
	m_titleDataItem->saveToProjectMainFile(writer);
	writer.writeEndElement();

	writer.writeStartElement("Time");
	m_timeDataItem->saveToProjectMainFile(writer);
	writer.writeEndElement();

	writer.writeStartElement("DistanceMeasures");
	m_distanceMeasureGroupDataItem->saveToProjectMainFile(writer);
	writer.writeEndElement();
}

Post2dWindowGridTypeDataItem* Post2dWindowRootDataItem::gridTypeDataItem(const QString& name)
{
	for (auto it = m_gridTypeDataItems.begin(); it != m_gridTypeDataItems.end(); ++it) {
		if ((*it)->name() == name) {return *it;}
	}
	return 0;
}

void Post2dWindowRootDataItem::updateZoneList()
{
	// When zone list is updated, post2d window used to update tree structure.
	// But, because of this, post2d window settings are all discarded every time
	// when user starts solver execution. so, it is removed now.
	/*
		dataModel()->itemModel()->blockSignals(true);
		for (auto it = m_gridTypeDataItems.begin(); it != m_gridTypeDataItems.end(); ++it)
		{
			(*it)->setupZoneDataItems();
		}
		dataModel()->itemModel()->blockSignals(false);
		dataModel()->graphicsView()->setActiveDataItem(this);
		updateItemMap();
		updateZDepthRangeItemCount();
		assignActorZValues(m_zDepthRange);
		dynamic_cast<VTK2DGraphicsView*>(dataModel()->graphicsView())->ResetCameraClippingRange();
		dataModel()->objectBrowserView()->expandAll();
		dataModel()->fit();
	*/
}

void Post2dWindowRootDataItem::update()
{
	QTime time;
	time.start();
	for (auto it = m_gridTypeDataItems.begin(); it != m_gridTypeDataItems.end(); ++it) {
		(*it)->update();
	}
	qDebug("Whole update elapsed time:%d", time.elapsed());
	time.restart();
	m_timeDataItem->update();
	qDebug("Time update elapsed time:%d", time.elapsed());
	time.restart();
	renderGraphicsView();
	qDebug("Rendering:%d", time.elapsed());
}

Post2dWindowZoneDataItem* Post2dWindowRootDataItem::zoneDataItem(const QString& name)
{
	for (auto it = m_gridTypeDataItems.begin(); it != m_gridTypeDataItems.end(); ++it) {
		Post2dWindowGridTypeDataItem* gtItem = *it;
		Post2dWindowZoneDataItem* i = gtItem->zoneData(name);
		if (i != 0) {return i;}
	}
	return 0;
}

#include "../post2dbirdeyeobjectbrowser.h"
#include "../post2dbirdeyewindow.h"
#include "../post2dbirdeyewindowdatamodel.h"
#include "../post2dbirdeyewindowgraphicsview.h"
#include "post2dbirdeyewindowgridtypedataitem.h"
#include "post2dbirdeyewindowrootdataitem.h"

#include <dataitem/axis3d/axis3ddataitem.h>
#include <guibase/objectbrowserview.h>
#include <guicore/postcontainer/postsolutioninfo.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/project/projectdata.h>
#include <guicore/solverdef/solverdefinition.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/xmlsupport.h>
#include <postbase/time/posttimedataitem.h>
#include <postbase/title/posttitledataitem.h>

#include <QDomNode>
#include <QTime>
#include <QXmlStreamWriter>

#include <vtkRenderWindow.h>

Post2dBirdEyeWindowRootDataItem::Post2dBirdEyeWindowRootDataItem(Post2dBirdEyeWindow* window, ProjectDataItem* parent) :
	GraphicsWindowRootDataItem {window, parent}
{
	SolverDefinition* def = projectData()->solverDefinition();
	PostSolutionInfo* post = dynamic_cast<Post2dBirdEyeWindowDataModel*>(dataModel())->postSolutionInfo();

	const QList<SolverDefinitionGridType*>& types = def->gridTypes();
	// build grid type data items.
	for (SolverDefinitionGridType* type : types) {
		Post2dBirdEyeWindowGridTypeDataItem* item = new Post2dBirdEyeWindowGridTypeDataItem(type, this);
		m_gridTypeDataItems.append(item);
		m_childItems.push_back(item);
	}
	// create grid type data item for dummy grid type if needed.
	bool needDummy = false;
	for (PostZoneDataContainer* c : post->zoneContainers2D()) {
		needDummy = needDummy || (c->gridType() == def->dummyGridType());
	}
	if (needDummy) {
		Post2dBirdEyeWindowGridTypeDataItem* item = new Post2dBirdEyeWindowGridTypeDataItem(def->dummyGridType(), this);
		m_gridTypeDataItems.append(item);
		m_childItems.push_back(item);
	}

	m_titleDataItem = new PostTitleDataItem(this);
	m_childItems.push_back(m_titleDataItem);

	m_timeDataItem = new PostTimeDataItem(this);
	m_childItems.push_back(m_timeDataItem);

	m_axesDataItem = new Axis3dDataItem(this);
	m_childItems.push_back(m_axesDataItem);

	updateZDepthRangeItemCount();
	// update item map initially.
	updateItemMap();

	connect(post, SIGNAL(zoneList2DUpdated()), this, SLOT(updateZoneList()));
	connect(post, SIGNAL(currentStepUpdated()), this, SLOT(update()));
}

Post2dBirdEyeWindowRootDataItem::~Post2dBirdEyeWindowRootDataItem()
{
	for (Post2dBirdEyeWindowGridTypeDataItem* item : m_gridTypeDataItems) {
		delete item;
	}
	delete m_titleDataItem;
	delete m_timeDataItem;
	delete m_axesDataItem;
}

void Post2dBirdEyeWindowRootDataItem::setupStandardModel(QStandardItemModel* model)
{
	model->clear();

	// add gridtypes.
	for (Post2dBirdEyeWindowGridTypeDataItem* item : m_gridTypeDataItems) {
		model->appendRow(item->standardItem());
	}

	model->appendRow(m_titleDataItem->standardItem());
	model->appendRow(m_timeDataItem->standardItem());
	model->appendRow(m_axesDataItem->standardItem());
}

void Post2dBirdEyeWindowRootDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	for (Post2dBirdEyeWindowGridTypeDataItem* item : m_gridTypeDataItems) {
		QDomNode c = node.firstChild();
		while (! c.isNull()) {
			if (c.nodeName() == "GridType" && c.toElement().attribute("name") == item->name().c_str()) {
				item->loadFromProjectMainFile(c);
			}
			c = c.nextSibling();
		}
	}
	SolverDefinition* def = projectData()->solverDefinition();
	const SolverDefinitionGridType* firstType = *(def->gridTypes().begin());
	if (def->gridTypes().count() == 1 && !(firstType->multiple())) {
		// Current solver support only one grid type, and it does not allow multiple grids to input.
		// The only, and hidden gridtype node should be checked always.
		// Post2dBirdEyeWindowGridTypeDataItem* gtItem = *(m_gridTypeDataItems.begin());
		// gtItem->standardItem()->setCheckState(Qt::Checked);
	}
	QDomNode titleNode = iRIC::getChildNode(node, "Title");
	if (! titleNode.isNull()) {m_titleDataItem->loadFromProjectMainFile(titleNode);}
	QDomNode timeNode = iRIC::getChildNode(node, "Time");
	if (! timeNode.isNull()) {m_timeDataItem->loadFromProjectMainFile(timeNode);}
	updateItemMap();
}
void Post2dBirdEyeWindowRootDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	for (Post2dBirdEyeWindowGridTypeDataItem* item : m_gridTypeDataItems) {
		writer.writeStartElement("GridType");
		item->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
	writer.writeStartElement("Title");
	m_titleDataItem->saveToProjectMainFile(writer);
	writer.writeEndElement();

	writer.writeStartElement("Time");
	m_timeDataItem->saveToProjectMainFile(writer);
	writer.writeEndElement();
}

const QList<Post2dBirdEyeWindowGridTypeDataItem*>& Post2dBirdEyeWindowRootDataItem::gridTypeDataItems() const
{
	return m_gridTypeDataItems;
}

Post2dBirdEyeWindowGridTypeDataItem* Post2dBirdEyeWindowRootDataItem::gridTypeDataItem(const std::string& name) const
{
	for (auto item : m_gridTypeDataItems) {
		if (item->name() == name) {return item;}
	}
	return 0;
}

void Post2dBirdEyeWindowRootDataItem::updateZoneList()
{
	// When zone list is updated, post2d bird's-eye window used to update tree structure.
	// But, because of this, post2d window settings are all discarded every time
	// when user starts solver execution. so, it is removed now.
	/*
	dataModel()->itemModel()->blockSignals(true);
	for (Post2dBirdEyeWindowGridTypeDataItem* item : m_gridTypeDataItems) {
		item->setupZoneDataItems();
	}
	dataModel()->itemModel()->blockSignals(false);
	dataModel()->graphicsView()->setActiveDataItem(this);
	updateItemMap();
	dataModel()->objectBrowserView()->expandAll();
	dataModel()->graphicsView()->cameraFit();
	*/
}

void Post2dBirdEyeWindowRootDataItem::update()
{
	QTime time;
	time.start();
	for (auto item : m_gridTypeDataItems) {
		item->update();
	}
	qDebug("Whole update elapsed time:%d", time.elapsed());
	time.restart();
	m_timeDataItem->update();
	qDebug("Time update elapsed time:%d", time.elapsed());
	time.restart();
	renderGraphicsView();
	qDebug("Rendering:%d", time.elapsed());
}

Post2dBirdEyeWindowZoneDataItem* Post2dBirdEyeWindowRootDataItem::zoneDataItem(const std::string& name) const
{
	for (auto gtItem : m_gridTypeDataItems) {
		Post2dBirdEyeWindowZoneDataItem* i = gtItem->zoneData(name);
		if (i != 0) {return i;}
	}
	return nullptr;
}

PostTitleDataItem* Post2dBirdEyeWindowRootDataItem::titleDataItem() const
{
	return m_titleDataItem;
}

PostTimeDataItem* Post2dBirdEyeWindowRootDataItem::timeDataItem() const
{
	return m_timeDataItem;
}

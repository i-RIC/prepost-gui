#include "../post3dobjectbrowser.h"
#include "../post3dwindow.h"
#include "../post3dwindowdatamodel.h"
#include "../post3dwindowdatamodel.h"
#include "../post3dwindowgraphicsview.h"
#include "post3dwindowgridtypedataitem.h"
#include "post3dwindowrootdataitem.h"

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
#include <QXmlStreamWriter>

#include <vtkRenderWindow.h>

Post3dWindowRootDataItem::Post3dWindowRootDataItem(Post3dWindow* window, ProjectDataItem* parent) :
	GraphicsWindowRootDataItem(window, parent)
{
	SolverDefinition* def = projectData()->solverDefinition();
	PostSolutionInfo* post = dynamic_cast<Post3dWindowDataModel*>(dataModel())->postSolutionInfo();

	const auto& types = def->gridTypes();
	// build grid type data items.
	const auto& conts = post->zoneContainers3D();
	for (auto type : types) {
		bool zoneExists = false;
		for (auto c : conts) {
			zoneExists = zoneExists || c->gridType() == type;
		}
		if (! zoneExists) {continue;}

		auto item = new Post3dWindowGridTypeDataItem(type, this);
		m_gridTypeDataItems.append(item);
		m_childItems.push_back(item);
	}
	// create grid type data item for dummy grid type if needed.
	bool needDummy = false;
	for (auto c : conts) {
		needDummy = needDummy || (c->gridType() == def->dummyGridType());
	}
	if (needDummy) {
		auto item = new Post3dWindowGridTypeDataItem(def->dummyGridType(), this);
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

	connect(post, SIGNAL(zoneList3DUpdated()), this, SLOT(updateZoneList()));
	connect(post, SIGNAL(currentStepUpdated()), this, SLOT(update()));
}

Post3dWindowRootDataItem::~Post3dWindowRootDataItem()
{
	for (auto it = m_gridTypeDataItems.begin(); it != m_gridTypeDataItems.end(); ++it) {
		delete *it;
	}
	delete m_titleDataItem;
	delete m_timeDataItem;
	delete m_axesDataItem;
}

void Post3dWindowRootDataItem::setupStandardModel(QStandardItemModel* model)
{
	model->clear();

	// add gridtypes.
	for (auto it = m_gridTypeDataItems.begin(); it != m_gridTypeDataItems.end(); ++it) {
		model->appendRow((*it)->standardItem());
	}
	// add title item row.
	model->appendRow(m_titleDataItem->standardItem());
	// add time item row.
	model->appendRow(m_timeDataItem->standardItem());
	// add axes item row
	model->appendRow(m_axesDataItem->standardItem());
}

const QList<Post3dWindowGridTypeDataItem*>& Post3dWindowRootDataItem::gridTypeDataItems() const
{
	return m_gridTypeDataItems;
}

Post3dWindowGridTypeDataItem* Post3dWindowRootDataItem::gridTypeDataItem(const std::string& name) const
{
	for (auto it = m_gridTypeDataItems.begin(); it != m_gridTypeDataItems.end(); ++it) {
		if ((*it)->name() == name) {return *it;}
	}
	return nullptr;
}

Post3dWindowZoneDataItem* Post3dWindowRootDataItem::zoneDataItem(const std::string& name)
{
	for (auto it = m_gridTypeDataItems.begin(); it != m_gridTypeDataItems.end(); ++it) {
		Post3dWindowGridTypeDataItem* gtItem = *it;
		Post3dWindowZoneDataItem* i = gtItem->zoneData(name);
		if (i != nullptr) {return i;}
	}
	return nullptr;
}

PostTitleDataItem* Post3dWindowRootDataItem::titleDataItem() const
{
	return m_titleDataItem;
}

PostTimeDataItem* Post3dWindowRootDataItem::timeDataItem() const
{
	return m_timeDataItem;
}

void Post3dWindowRootDataItem::updateZoneList()
{
	// When zone list is updated, post3d window used to update tree structure.
	// But, because of this, post3d window settings are all discarded every time
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
		//+++++
		//assignActorZValues(m_zDepthRange);
		//+++++
		dataModel()->objectBrowserView()->expandAll();
		dataModel()->fit();
	*/
}

void Post3dWindowRootDataItem::update()
{
	for (auto it = m_gridTypeDataItems.begin(); it != m_gridTypeDataItems.end(); ++it) {
		(*it)->update();
	}
	m_timeDataItem->update();
	renderGraphicsView();
}

void Post3dWindowRootDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	for (auto it = m_gridTypeDataItems.begin(); it != m_gridTypeDataItems.end(); ++it) {
		QDomNode c = node.firstChild();
		while (! c.isNull()) {
			if (c.nodeName() == "GridType" && c.toElement().attribute("name") == (*it)->name().c_str()) {
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
		// Post3dWindowGridTypeDataItem* gtItem = *(m_gridTypeDataItems.begin());
		// gtItem->standardItem()->setCheckState(Qt::Checked);
	}
	QDomNode titleNode = iRIC::getChildNode(node, "Title");
	if (! titleNode.isNull()) {m_titleDataItem->loadFromProjectMainFile(titleNode);}
	QDomNode timeNode = iRIC::getChildNode(node, "Time");
	if (! timeNode.isNull()) {m_timeDataItem->loadFromProjectMainFile(timeNode);}
	QDomNode axesNode = iRIC::getChildNode(node, "Axes");
	if (! axesNode.isNull()) {m_axesDataItem->loadFromProjectMainFile(axesNode);}

	updateItemMap();
}
void Post3dWindowRootDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
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

	writer.writeStartElement("Axes");
	m_axesDataItem->saveToProjectMainFile(writer);
	writer.writeEndElement();
}


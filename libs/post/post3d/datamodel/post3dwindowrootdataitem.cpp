#include "../post3dobjectbrowser.h"
#include "../post3dwindow.h"
#include "../post3dwindowdatamodel.h"
#include "../post3dwindowdatamodel.h"
#include "../post3dwindowgraphicsview.h"
#include "post3dwindowgridtypedataitem.h"
#include "post3dwindowrootdataitem.h"

#include <guibase/objectbrowserview.h>
#include <guicore/axis3d/axis3ddataitem.h>
#include <guicore/postcontainer/postsolutioninfo.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/project/projectdata.h>
#include <guicore/solverdef/solverdefinition.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/xmlsupport.h>
#include <postbase/time/posttimedataitem.h>
#include <postbase/title/posttitledataitem.h>

#include <QStandardItemModel>
#include <QXmlStreamWriter>

#include <vtkRenderWindow.h>

Post3dWindowRootDataItem::Post3dWindowRootDataItem(Post3dWindow* window, ProjectDataItem* parent)
	: GraphicsWindowRootDataItem(window, parent)
{
	// Background images data item node.
//	m_backgroundImagesDataItem = new Post3dWindowBackgroundImagesDataItem(this);
//	m_childItems.append(m_backgroundImagesDataItem);

	SolverDefinition* def = projectData()->solverDefinition();
	PostSolutionInfo* post = dynamic_cast<Post3dWindowDataModel*>(dataModel())->postSolutionInfo();

	const QList<SolverDefinitionGridType*>& types = def->gridTypes();
	// build grid type data items.
	QList<SolverDefinitionGridType*>::const_iterator it;
	for (it = types.begin(); it != types.end(); ++it){
		Post3dWindowGridTypeDataItem* item = new Post3dWindowGridTypeDataItem(*it, this);
		m_gridTypeDataItems.append(item);
		m_childItems.append(item);
	}
	// create grid type data item for dummy grid type if needed.
	const QList<PostZoneDataContainer*>& conts = post->zoneContainers3D();
	bool needDummy = false;
	for (int i = 0; i < conts.count(); ++i){
		PostZoneDataContainer* c = conts.at(i);
		needDummy = needDummy || (c->gridType() == def->dummyGridType());
	}
	if (needDummy){
		Post3dWindowGridTypeDataItem* item = new Post3dWindowGridTypeDataItem(def->dummyGridType(), this);
		m_gridTypeDataItems.append(item);
		m_childItems.append(item);
	}

	m_titleDataItem = new PostTitleDataItem(this);
	m_childItems.append(m_titleDataItem);

	m_timeDataItem = new PostTimeDataItem(this);
	m_childItems.append(m_timeDataItem);

	m_axesDataItem = new Axis3dDataItem(this);
	m_childItems.append(m_axesDataItem);

	updateZDepthRangeItemCount();
	// update item map initially.
	updateItemMap();

	connect(post, SIGNAL(zoneList3DUpdated()), this, SLOT(updateZoneList()));
	connect(post, SIGNAL(currentStepUpdated()), this, SLOT(update()));
}

Post3dWindowRootDataItem::~Post3dWindowRootDataItem()
{
	QList<Post3dWindowGridTypeDataItem*>::iterator it;
	for(it = m_gridTypeDataItems.begin(); it != m_gridTypeDataItems.end(); ++it){
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
	QList<Post3dWindowGridTypeDataItem*>::iterator it;
	for (it = m_gridTypeDataItems.begin(); it != m_gridTypeDataItems.end(); ++it){
		model->appendRow((*it)->standardItem());
	}
	// add title item row.
	model->appendRow(m_titleDataItem->standardItem());
	// add time item row.
	model->appendRow(m_timeDataItem->standardItem());
	// add axes item row
	model->appendRow(m_axesDataItem->standardItem());
}

void Post3dWindowRootDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	QList<Post3dWindowGridTypeDataItem*>::iterator it;
	for (it = m_gridTypeDataItems.begin(); it != m_gridTypeDataItems.end(); ++it){
		QDomNode c = node.firstChild();
		while (! c.isNull()){
			if (c.nodeName() == "GridType" && c.toElement().attribute("name") == (*it)->name()){
				(*it)->loadFromProjectMainFile(c);
			}
			c = c.nextSibling();
		}
	}
	SolverDefinition* def = projectData()->solverDefinition();
	const SolverDefinitionGridType* firstType = *(def->gridTypes().begin());
	if (def->gridTypes().count() == 1 && ! (firstType->multiple())){
		// Current solver support only one grid type, and it does not allow multiple grids to input.
		// The only, and hidden gridtype node should be checked always.
		Post3dWindowGridTypeDataItem* gtItem = *(m_gridTypeDataItems.begin());
		gtItem->standardItem()->setCheckState(Qt::Checked);
	}
	QDomNode titleNode = iRIC::getChildNode(node, "Title");
	if (! titleNode.isNull()){m_titleDataItem->loadFromProjectMainFile(titleNode);}
	QDomNode timeNode = iRIC::getChildNode(node, "Time");
	if (! timeNode.isNull()){m_timeDataItem->loadFromProjectMainFile(timeNode);}
	updateItemMap();
}
void Post3dWindowRootDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	QList<Post3dWindowGridTypeDataItem*>::iterator it;
	for (it = m_gridTypeDataItems.begin(); it != m_gridTypeDataItems.end(); ++it){
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
}

Post3dWindowGridTypeDataItem* Post3dWindowRootDataItem::gridTypeDataItem(const QString& name)
{
	QList<Post3dWindowGridTypeDataItem*>::iterator it;
	for (it = m_gridTypeDataItems.begin(); it != m_gridTypeDataItems.end(); ++it)
	{
		if ((*it)->name() == name){return *it;}
	}
	return 0;
}

void Post3dWindowRootDataItem::updateZoneList()
{
	// When zone list is updated, post3d window used to update tree structure.
	// But, because of this, post3d window settings are all discarded every time
	// when user starts solver execution. so, it is removed now.
/*
	dataModel()->itemModel()->blockSignals(true);
	QList<Post3dWindowGridTypeDataItem*>::iterator it;
	for (it = m_gridTypeDataItems.begin(); it != m_gridTypeDataItems.end(); ++it)
	{
		(*it)->setupZoneDataItems();
	}
	dataModel()->itemModel()->blockSignals(false);
	dataModel()->graphicsView()->setActiveDataItem(this);
	updateItemMap();
	updateZDepthRangeItemCount();
	//+++++
	//assignActionZValues(m_zDepthRange);
	//+++++
	dataModel()->objectBrowserView()->expandAll();
	dataModel()->fit();
*/
}

void Post3dWindowRootDataItem::update()
{
	QList<Post3dWindowGridTypeDataItem*>::iterator it;
	for (it = m_gridTypeDataItems.begin(); it != m_gridTypeDataItems.end(); ++it)
	{
		(*it)->update();
	}
	m_timeDataItem->update();
	renderGraphicsView();
}

Post3dWindowZoneDataItem* Post3dWindowRootDataItem::zoneDataItem(const QString& name)
{
	QList<Post3dWindowGridTypeDataItem*>::iterator it;
	for (it = m_gridTypeDataItems.begin(); it != m_gridTypeDataItems.end(); ++it)
	{
		Post3dWindowGridTypeDataItem* gtItem = *it;
		Post3dWindowZoneDataItem* i = gtItem->zoneData(name);
		if (i != 0){return i;}
	}
	return 0;
}

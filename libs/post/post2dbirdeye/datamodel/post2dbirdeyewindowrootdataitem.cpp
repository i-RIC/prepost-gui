#include "../post2dbirdeyeobjectbrowser.h"
#include "../post2dbirdeyewindow.h"
#include "../post2dbirdeyewindowdatamodel.h"
#include "../post2dbirdeyewindowdatamodel.h"
#include "../post2dbirdeyewindowgraphicsview.h"
#include "post2dbirdeyewindowaxesdataitem.h"
#include "post2dbirdeyewindowgridtypedataitem.h"
#include "post2dbirdeyewindowrootdataitem.h"

#include <guibase/objectbrowserview.h>
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

Post2dBirdEyeWindowRootDataItem::Post2dBirdEyeWindowRootDataItem(Post2dBirdEyeWindow* window, ProjectDataItem* parent)
	: GraphicsWindowRootDataItem(window, parent)
{
	SolverDefinition* def = projectData()->solverDefinition();
	PostSolutionInfo* post = dynamic_cast<Post2dBirdEyeWindowDataModel*>(dataModel())->postSolutionInfo();

	const QList<SolverDefinitionGridType*>& types = def->gridTypes();
	// build grid type data items.
	QList<SolverDefinitionGridType*>::const_iterator it;
	for (it = types.begin(); it != types.end(); ++it){
		Post2dBirdEyeWindowGridTypeDataItem* item = new Post2dBirdEyeWindowGridTypeDataItem(*it, this);
		m_gridTypeDataItems.append(item);
		m_childItems.append(item);
	}
	// create grid type data item for dummy grid type if needed.
	const QList<PostZoneDataContainer*>& conts = post->zoneContainers2D();
	bool needDummy = false;
	for (int i = 0; i < conts.count(); ++i){
		PostZoneDataContainer* c = conts.at(i);
		needDummy = needDummy || (c->gridType() == def->dummyGridType());
	}
	if (needDummy){
		Post2dBirdEyeWindowGridTypeDataItem* item = new Post2dBirdEyeWindowGridTypeDataItem(def->dummyGridType(), this);
		m_gridTypeDataItems.append(item);
		m_childItems.append(item);
	}

	m_titleDataItem = new PostTitleDataItem(this);
	m_childItems.append(m_titleDataItem);

	m_timeDataItem = new PostTimeDataItem(this);
	m_childItems.append(m_timeDataItem);

	m_axesDataItem = new Post2dBirdEyeWindowAxesDataItem(this);
	m_childItems.append(m_axesDataItem);

	updateZDepthRangeItemCount();
	// update item map initially.
	updateItemMap();

	connect(post, SIGNAL(zoneList2DUpdated()), this, SLOT(updateZoneList()));
	connect(post, SIGNAL(currentStepUpdated()), this, SLOT(update()));
}

Post2dBirdEyeWindowRootDataItem::~Post2dBirdEyeWindowRootDataItem()
{
	QList<Post2dBirdEyeWindowGridTypeDataItem*>::iterator it;
	for(it = m_gridTypeDataItems.begin(); it != m_gridTypeDataItems.end(); ++it){
		delete *it;
	}
	delete m_titleDataItem;
	delete m_timeDataItem;
	delete m_axesDataItem;
}

void Post2dBirdEyeWindowRootDataItem::setupStandardModel(QStandardItemModel* model)
{
	model->clear();

	// add gridtypes.
	QList<Post2dBirdEyeWindowGridTypeDataItem*>::iterator it;
	for (it = m_gridTypeDataItems.begin(); it != m_gridTypeDataItems.end(); ++it){
		model->appendRow((*it)->standardItem());
	}
	// add title item row.
	model->appendRow(m_titleDataItem->standardItem());
	// add time item row.
	model->appendRow(m_timeDataItem->standardItem());
	// add axes item row.
	model->appendRow(m_axesDataItem->standardItem());

	emit standardModelSetuped();
}

void Post2dBirdEyeWindowRootDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	QList<Post2dBirdEyeWindowGridTypeDataItem*>::iterator it;
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
		Post2dBirdEyeWindowGridTypeDataItem* gtItem = *(m_gridTypeDataItems.begin());
		gtItem->standardItem()->setCheckState(Qt::Checked);
	}
	QDomNode titleNode = iRIC::getChildNode(node, "Title");
	if (! titleNode.isNull()){m_titleDataItem->loadFromProjectMainFile(titleNode);}
	QDomNode timeNode = iRIC::getChildNode(node, "Time");
	if (! timeNode.isNull()){m_timeDataItem->loadFromProjectMainFile(timeNode);}
	updateItemMap();
}
void Post2dBirdEyeWindowRootDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	QList<Post2dBirdEyeWindowGridTypeDataItem*>::iterator it;
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

Post2dBirdEyeWindowGridTypeDataItem* Post2dBirdEyeWindowRootDataItem::gridTypeDataItem(const QString& name)
{
	QList<Post2dBirdEyeWindowGridTypeDataItem*>::iterator it;
	for (it = m_gridTypeDataItems.begin(); it != m_gridTypeDataItems.end(); ++it)
	{
		if ((*it)->name() == name){return *it;}
	}
	return 0;
}

void Post2dBirdEyeWindowRootDataItem::updateZoneList()
{
	dataModel()->itemModel()->blockSignals(true);
	QList<Post2dBirdEyeWindowGridTypeDataItem*>::iterator it;
	for (it = m_gridTypeDataItems.begin(); it != m_gridTypeDataItems.end(); ++it)
	{
		(*it)->setupZoneDataItems();
	}
	dataModel()->itemModel()->blockSignals(false);
	dataModel()->graphicsView()->setActiveDataItem(this);
	updateItemMap();
//	updateZDepthRangeItemCount();
//	assignActionZValues(m_zDepthRange);
//	dynamic_cast<VTK3DGraphicsView*>(dataModel()->graphicsView())->ResetCameraClippingRange();
	dataModel()->objectBrowserView()->expandAll();
	dataModel()->fit();
}

void Post2dBirdEyeWindowRootDataItem::update()
{
	QTime time;
	time.start();
	QList<Post2dBirdEyeWindowGridTypeDataItem*>::iterator it;
	for (it = m_gridTypeDataItems.begin(); it != m_gridTypeDataItems.end(); ++it)
	{
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

Post2dBirdEyeWindowZoneDataItem* Post2dBirdEyeWindowRootDataItem::zoneDataItem(const QString& name)
{
	QList<Post2dBirdEyeWindowGridTypeDataItem*>::iterator it;
	for (it = m_gridTypeDataItems.begin(); it != m_gridTypeDataItems.end(); ++it)
	{
		Post2dBirdEyeWindowGridTypeDataItem* gtItem = *it;
		Post2dBirdEyeWindowZoneDataItem* i = gtItem->zoneData(name);
		if (i != 0){return i;}
	}
	return 0;
}

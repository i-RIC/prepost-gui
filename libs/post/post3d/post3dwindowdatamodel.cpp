#include "datamodel/post3dwindowcontourgrouptopdataitem.h"
#include "datamodel/post3dwindowgridshapedataitem.h"
#include "datamodel/post3dwindowgridtypedataitem.h"
#include "datamodel/post3dwindownodescalargroupdataitem.h"
#include "datamodel/post3dwindownodevectorarrowtopdataitem.h"
#include "datamodel/post3dwindownodevectorparticlegroupdataitem.h"
#include "datamodel/post3dwindownodevectorstreamlinegroupdataitem.h"
#include "datamodel/post3dwindowrootdataitem.h"
#include "datamodel/post3dwindowsurfacegroupdataitem.h"
#include "datamodel/post3dwindowzonedataitem.h"
#include "post3dwindow.h"
#include "post3dwindowdatamodel.h"
#include "post3dwindowgraphicsview.h"

#include <guibase/objectbrowserview.h>
#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/post/postzoneselectingdialog.h>
#include <guicore/postcontainer/postsolutioninfo.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <misc/xmlsupport.h>
#include <postbase/time/posttimedataitem.h>
#include <postbase/title/posttitledataitem.h>

#include <QGraphicsItemGroup>
#include <QInputDialog>
#include <QStandardItem>
#include <QStandardItemModel>

#include <vtkRenderWindow.h>

#include <cmath>

#define _USE_MATH_DEFINES

Post3dWindowDataModel::Post3dWindowDataModel(Post3dWindow* w, ProjectDataItem* parent) :
	Graphics3DWindowDataModel {w, parent}
{
	init();
}

Post3dWindowDataModel::~Post3dWindowDataModel()
{
	m_objectBrowserView->blockSignals(true);
	delete m_rootDataItem;
	m_objectBrowserView->blockSignals(false);
}

void Post3dWindowDataModel::init()
{
	m_zScale = 1;
	// Discard the old data.
	// We do not need the next line, because the m_rootDataItem
	// is deleted automatically when the last projectdata is deleted.
	// if (m_rootDataItem != nullptr){delete m_rootDataItem;}
	// build new data.
	m_graphicsView = new Post3dWindowGraphicsView(dynamic_cast<QWidget*>(parent()));

	// setup the basic itemModel structure.
	Post3dWindowRootDataItem* root = new Post3dWindowRootDataItem(dynamic_cast<Post3dWindow*>(mainWindow()), this);
	m_rootDataItem = root;
	root->setupStandardModel(m_itemModel);
	connect(m_itemModel, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(handleObjectBrowserChange(QStandardItem*)));

	m_graphicsView->setActiveDataItem(root);
	m_graphicsView->setModel(this);
	m_graphicsView->cameraFit();
}

Post3dWindowGraphicsView* Post3dWindowDataModel::graphicsView() const
{
	return dynamic_cast<Post3dWindowGraphicsView*>(m_graphicsView);
}

PostSolutionInfo* Post3dWindowDataModel::postSolutionInfo()
{
	return projectData()->mainfile()->postSolutionInfo();
}

void Post3dWindowDataModel::gridShapeSetting()
{
	Post3dWindowZoneDataItem* zItem = getZoneDataItem();
	if (zItem == nullptr) {return;}
	Post3dWindowGridShapeDataItem* item = zItem->gridShapeDataItem();
	if (item == nullptr) {return;}
	item->showPropertyDialog();
}

void Post3dWindowDataModel::surfaceSetting()
{
	Post3dWindowZoneDataItem* zItem = getZoneDataItem();
	if (zItem == nullptr) {return;}
//	Post3dWindowSurfaceGroupDataItem* item = zItem->
//	item->showPropertyDialog();
}

void Post3dWindowDataModel::addContour()
{
	Post3dWindowZoneDataItem* item = getZoneDataItem();
	if (item == nullptr) {return;}
	Post3dWindowContourGroupTopDataItem* citem = item->contourGroupTopItem();
	citem->showAddDialog();
}

void Post3dWindowDataModel::addIsosurface()
{
	Post3dWindowZoneDataItem* item = getZoneDataItem();
	if (item == nullptr) {return;}
	Post3dWindowNodeScalarGroupTopDataItem* citem = item->scalarGroupDataItem();
	citem->showAddDialog();
}

void Post3dWindowDataModel::contourSetting()
{
	addContour();
}

void Post3dWindowDataModel::arrowSetting()
{
	Post3dWindowZoneDataItem* item = getZoneDataItem();
	if (item == nullptr) {return;}
	Post3dWindowNodeVectorArrowTopDataItem* aitem = item->arrowTopDataItem();
	aitem->showAddDialog();
}

void Post3dWindowDataModel::streamlineSetting()
{
	Post3dWindowZoneDataItem* item = getZoneDataItem();
	if (item == nullptr) {return;}
	Post3dWindowNodeVectorStreamlineGroupDataItem* sitem = item->streamlineGroupDataItem();
	sitem->showPropertyDialog();
}

void Post3dWindowDataModel::particleSetting()
{
	Post3dWindowZoneDataItem* item = getZoneDataItem();
	if (item == nullptr) {return;}
	Post3dWindowNodeVectorParticleGroupDataItem* pitem = item->particleGroupDataItem();
	pitem->showPropertyDialog();
}

void Post3dWindowDataModel::isosurfaceSetting()
{
	addIsosurface();
}

Post3dWindowZoneDataItem* Post3dWindowDataModel::getZoneDataItem()
{
	GraphicsWindowDataItem* item = m_selectedItem;
	while (item != nullptr) {
		Post3dWindowZoneDataItem* zitem = dynamic_cast<Post3dWindowZoneDataItem*>(item);
		// zone item found!
		if (zitem != nullptr) {return zitem;}
		// try parent item.
		item = dynamic_cast<GraphicsWindowDataItem*>(item->parent());
		if (dynamic_cast<GraphicsWindowRootDataItem*>(item) != nullptr) {
			// reached to root node.
			break;
		}
	}
	// no appropriate zonedatatem found.
	PostSolutionInfo* info = postSolutionInfo();
	QList<PostZoneDataContainer*> containers = info->zoneContainers3D();
	Post3dWindowZoneDataItem* zitem;
	if (containers.count() == 0) {return nullptr;}
	if (containers.count() > 1) {
		PostZoneSelectingDialog dialog(mainWindow());
		dialog.setContainers(containers);
		int ret = dialog.exec();
		if (ret != QDialog::Accepted) {return nullptr;}
		std::string gridType = dialog.gridTypeName();
		std::string zone = dialog.zoneName();
		Post3dWindowRootDataItem* root = dynamic_cast<Post3dWindowRootDataItem*>(m_rootDataItem);
		Post3dWindowGridTypeDataItem* gt = root->gridTypeDataItem(gridType);
		zitem = gt->zoneData(zone);
		return zitem;
	} else {
		Post3dWindowRootDataItem* root = dynamic_cast<Post3dWindowRootDataItem*>(m_rootDataItem);
		QList<Post3dWindowGridTypeDataItem*> list = root->gridTypeDataItems();
		Post3dWindowGridTypeDataItem* gt = list.at(0);
		QList<Post3dWindowZoneDataItem*> zonelist = gt->zoneDatas();
		zitem = zonelist.at(0);
		return zitem;
	}
}

void Post3dWindowDataModel::titleSetting()
{
	Post3dWindowRootDataItem* r = dynamic_cast<Post3dWindowRootDataItem*>(m_rootDataItem);
	r->titleDataItem()->showPropertyDialog();
}

void Post3dWindowDataModel::timeSetting()
{
	Post3dWindowRootDataItem* r = dynamic_cast<Post3dWindowRootDataItem*>(m_rootDataItem);
	r->timeDataItem()->showPropertyDialog();
}

void Post3dWindowDataModel::editZScale()
{
	bool ok;
	double newZscale = QInputDialog::getDouble(mainWindow(), tr("Z-direction Scale"), tr("Input new Z-direction scale."), m_zScale, 1E-6, 1E6, 3, &ok);
	if (! ok) {return;}
	m_zScale = newZscale;
	m_rootDataItem->updateZScale(newZscale);
	m_graphicsView->cameraFit();
}

void Post3dWindowDataModel::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_zScale =  iRIC::getDoubleAttribute(node, "ZScale", 1);
	Post3dWindowRootDataItem* ritem = dynamic_cast<Post3dWindowRootDataItem*>(m_rootDataItem);
	ritem->updateZScale(m_zScale);
	GraphicsWindowDataModel::doLoadFromProjectMainFile(node);
}

void Post3dWindowDataModel::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	iRIC::setDoubleAttribute(writer, "ZScale", m_zScale);
	GraphicsWindowDataModel::doSaveToProjectMainFile(writer);
}

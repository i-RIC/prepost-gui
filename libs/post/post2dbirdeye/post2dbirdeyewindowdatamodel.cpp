#include "datamodel/post2dbirdeyewindowgridshapedataitem.h"
#include "datamodel/post2dbirdeyewindowgridtypedataitem.h"
#include "datamodel/post2dbirdeyewindownodescalargroupdataitem.h"
#include "datamodel/post2dbirdeyewindownodescalargrouptopdataitem.h"
#include "datamodel/post2dbirdeyewindowrootdataitem.h"
#include "datamodel/post2dbirdeyewindowzonedataitem.h"
#include "post2dbirdeyewindow.h"
#include "post2dbirdeyewindowdatamodel.h"
#include "post2dbirdeyewindowgraphicsview.h"

#include <guibase/objectbrowserview.h>
#include <guicore/post/postzoneselectingdialog.h>
#include <guicore/postcontainer/postsolutioninfo.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <misc/xmlsupport.h>
#include <postbase/time/posttimedataitem.h>
#include <postbase/title/posttitledataitem.h>

#include <QGraphicsItemGroup>
#include <QInputDialog>
#include <QMessageBox>
#include <QStandardItem>
#include <QStandardItemModel>

#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

#include <cmath>

#define _USE_MATH_DEFINES

Post2dBirdEyeWindowDataModel::Post2dBirdEyeWindowDataModel(Post2dBirdEyeWindow* w, ProjectDataItem* parent)
	: Graphics3DWindowDataModel(w, parent)
{
	init();
}

Post2dBirdEyeWindowDataModel::~Post2dBirdEyeWindowDataModel()
{
	m_objectBrowserView->blockSignals(true);
	delete m_rootDataItem;
	m_objectBrowserView->blockSignals(false);
}

void Post2dBirdEyeWindowDataModel::init()
{
	// Discard the old data.
	// We do not need the next line, because the m_rootDataItem
	// is deleted automatically when the last projectdata is deleted.
	// if (m_rootDataItem != nullptr){delete m_rootDataItem;}
	// build new data.
	m_graphicsView = new Post2dBirdEyeWindowGraphicsView(dynamic_cast<QWidget*>(parent()));
	connect(m_graphicsView, SIGNAL(worldPositionChangedForStatusBar(QPointF)), mainWindow(), SIGNAL(worldPositionChangedForStatusBar(QPointF)));

	// setup the basic itemModel structure.
	Post2dBirdEyeWindowRootDataItem* root = new Post2dBirdEyeWindowRootDataItem(dynamic_cast<Post2dBirdEyeWindow*>(mainWindow()), this);
	m_rootDataItem = root;
	root->setupStandardModel(m_itemModel);
	connect(m_itemModel, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(handleObjectBrowserChange(QStandardItem*)));

	m_graphicsView->setActiveDataItem(root);
	m_graphicsView->setModel(this);

	m_zScale = 1;
	m_graphicsView->cameraFit();
}

Post2dBirdEyeWindowGraphicsView* Post2dBirdEyeWindowDataModel::graphicsView() const
{
	return dynamic_cast<Post2dBirdEyeWindowGraphicsView*>(m_graphicsView);
}

PostSolutionInfo* Post2dBirdEyeWindowDataModel::postSolutionInfo()
{
	return projectData()->mainfile()->postSolutionInfo();
}

void Post2dBirdEyeWindowDataModel::gridShapeSetting()
{
	Post2dBirdEyeWindowZoneDataItem* zItem = getZoneDataItem();
	if (zItem == nullptr) {return;}
	Post2dBirdEyeWindowGridShapeDataItem* item = zItem->gridShapeDataItem();
	if (item == nullptr) {return;}
	item->showPropertyDialog();
}

void Post2dBirdEyeWindowDataModel::addContour()
{
	Post2dBirdEyeWindowZoneDataItem* zItem = getZoneDataItem();
	if (zItem == nullptr) {return;}
	Post2dBirdEyeWindowNodeScalarGroupTopDataItem* item = zItem->scalarGroupTopDataItem();
	if (item == nullptr) {
		QMessageBox::warning(mainWindow(),tr("Warning"), tr("Contour setting is not available, because this result does not contain scalar values."));
		return;
	}
	item->showAddDialog();
}

void Post2dBirdEyeWindowDataModel::contourSetting()
{
	addContour();
}

void Post2dBirdEyeWindowDataModel::addCellScalar()
{
	// todo fix this
	/*
	Post2dBirdEyeWindowZoneDataItem* zItem = getZoneDataItem();
	if (zItem == nullptr) { return; }
	Post2dBirdEyeWindowCellScalarGroupTopDataItem* item = zItem->cellScalarGroupTopDataItem();
	if (item == nullptr) {
		QMessageBox::warning(mainWindow(), tr("Warning"), tr("Contour setting is not available, because this result does not contain cell scalar values."));
		return;
	}
	item->showAddDialog();
	*/
}

void Post2dBirdEyeWindowDataModel::cellScalarSetting()
{
	addCellScalar();
}

Post2dBirdEyeWindowZoneDataItem* Post2dBirdEyeWindowDataModel::getZoneDataItem()
{
	GraphicsWindowDataItem* item = m_selectedItem;
	while (item != nullptr) {
		Post2dBirdEyeWindowZoneDataItem* zitem = dynamic_cast<Post2dBirdEyeWindowZoneDataItem*>(item);
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
	QList<PostZoneDataContainer*> containers = info->zoneContainers2D();
	Post2dBirdEyeWindowZoneDataItem* zitem;
	if (containers.count() == 0) {
		// No zone container exists.
		return nullptr;
	} else if (containers.count() > 1) {
		// Multiple zone containers found.
		PostZoneSelectingDialog dialog(mainWindow());
		dialog.setContainers(containers);
		int ret = dialog.exec();
		if (ret != QDialog::Accepted) {return nullptr;}
		std::string gridType = dialog.gridTypeName();
		std::string zone = dialog.zoneName();
		Post2dBirdEyeWindowRootDataItem* root = dynamic_cast<Post2dBirdEyeWindowRootDataItem*>(m_rootDataItem);
		Post2dBirdEyeWindowGridTypeDataItem* gt = root->gridTypeDataItem(gridType);
		return gt->zoneData(zone);
	} else {
		Post2dBirdEyeWindowRootDataItem* root = dynamic_cast<Post2dBirdEyeWindowRootDataItem*>(m_rootDataItem);
		QList<Post2dBirdEyeWindowGridTypeDataItem*> list = root->gridTypeDataItems();
		Post2dBirdEyeWindowGridTypeDataItem* gt = list.at(0);
		return gt->zoneDatas().at(0);
	}
}

void Post2dBirdEyeWindowDataModel::createCameraConnections()
{

}

void Post2dBirdEyeWindowDataModel::titleSetting()
{
	Post2dBirdEyeWindowRootDataItem* r = dynamic_cast<Post2dBirdEyeWindowRootDataItem*>(m_rootDataItem);
	r->titleDataItem()->showPropertyDialog();
}

void Post2dBirdEyeWindowDataModel::timeSetting()
{
	Post2dBirdEyeWindowRootDataItem* r = dynamic_cast<Post2dBirdEyeWindowRootDataItem*>(m_rootDataItem);
	r->timeDataItem()->showPropertyDialog();
}

void Post2dBirdEyeWindowDataModel::editZScale()
{
	bool ok;
	double newZscale = QInputDialog::getDouble(mainWindow(), tr("Z-direction Scale"), tr("Input new Z-direction scale."), m_zScale, 1E-6, 1E6, 3, &ok);
	if (! ok) {return;}
	m_zScale = newZscale;
	m_rootDataItem->updateZScale(newZscale);
	m_graphicsView->cameraFit();
}

void Post2dBirdEyeWindowDataModel::doLoadFromProjectMainFile(const QDomNode& node)
{
	GraphicsWindowDataModel::doLoadFromProjectMainFile(node);
	m_zScale = iRIC::getDoubleAttribute(node, "ZScale", 1);
	m_rootDataItem->updateZScale(m_zScale);
}

void Post2dBirdEyeWindowDataModel::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	iRIC::setDoubleAttribute(writer, "ZScale", m_zScale);
	GraphicsWindowDataModel::doSaveToProjectMainFile(writer);
}

#include "datamodel/post2dwindowcalculationresultdataitem.h"
#include "datamodel/post2dwindowinputgriddataitem.h"
#include "datamodel/post2dwindowgeodatatopdataitem.h"
#include "datamodel/post2dwindowgraphgroupdataitem.h"
#include "datamodel/post2dwindowgridshapedataitem.h"
#include "datamodel/post2dwindowgridtypedataitem.h"
#include "datamodel/post2dwindowmeasureddatatopdataitem.h"
#include "datamodel/post2dwindowcellscalargroupdataitem.h"
#include "datamodel/post2dwindowcellscalargrouptopdataitem.h"
#include "datamodel/post2dwindownodescalargroupdataitem.h"
#include "datamodel/post2dwindownodescalargrouptopdataitem.h"
#include "datamodel/post2dwindownodevectorarrowgrouptopdataitem.h"
#include "datamodel/post2dwindownodevectorparticlegroupdataitem.h"
#include "datamodel/post2dwindownodevectorstreamlinegroupdataitem.h"
#include "datamodel/post2dwindowpolydatagroupdataitem.h"
#include "datamodel/post2dwindowrootdataitem.h"
#include "datamodel/post2dwindowzonedataitem.h"
#include "post2dwindow.h"
#include "post2dwindowdatamodel.h"
#include "post2dwindowgraphicsview.h"

#include <dataitem/measureddata/measureddatafiledataitem.h>
#include <dataitem/measureddata/measureddatapointgroupdataitem.h>
#include <dataitem/measureddata/measureddatavectorgroupdataitem.h>
#include <dataitem/measureddata/measureddatavectorgrouptopdataitem.h>
#include <guibase/widget/itemselectingdialog.h>
#include <guibase/objectbrowserview.h>
#include <guicore/post/postzoneselectingdialog.h>
#include <guicore/postcontainer/postsolutioninfo.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <guicore/tmsimage/tmsimagegroupdataitem.h>
#include <postbase/time/posttimedataitem.h>
#include <postbase/title/posttitledataitem.h>
#include <misc/xmlsupport.h>

#include <QGraphicsItemGroup>
#include <QInputDialog>
#include <QMessageBox>
#include <QStandardItem>
#include <QStandardItemModel>

#include <cmath>

#define _USE_MATH_DEFINES



Post2dWindowDataModel::Post2dWindowDataModel(Post2dWindow* w, ProjectDataItem* parent) :
	Graphics2DWindowDataModel(w, parent),
	m_zScale {1}
{
	m_objectBrowserView = nullptr;
	init();
}

Post2dWindowDataModel::~Post2dWindowDataModel()
{
	m_objectBrowserView->blockSignals(true);
	delete m_rootDataItem;
	m_objectBrowserView->blockSignals(false);
}

void Post2dWindowDataModel::init()
{
	// Discard the old data.
	// We do not need the next line, because the m_rootDataItem
	// is deleted automatically when the last projectdata is deleted.
	// if (m_rootDataItem != nullptr){delete m_rootDataItem;}
	// build new data.
	m_graphicsView = new Post2dWindowGraphicsView(dynamic_cast<QWidget*>(parent()));
	m_graphicsView->setModel(this);
	connect(m_graphicsView, SIGNAL(worldPositionChangedForStatusBar(QPointF)), mainWindow(), SIGNAL(worldPositionChangedForStatusBar(QPointF)));

	// setup the basic itemModel structure.
	auto root = new Post2dWindowRootDataItem(dynamic_cast<Post2dWindow*>(mainWindow()), this);
	m_rootDataItem = root;
	root->setZDepthRange(m_dataRange);
	root->setupStandardModel(m_itemModel);
	connect(m_itemModel, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(handleObjectBrowserChange(QStandardItem*)));

	m_graphicsView->setActiveDataItem(root);
	m_graphicsView->cameraFit();
}

void Post2dWindowDataModel::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_zScale = iRIC::getDoubleAttribute(node, "ZScale", 1);
	m_rootDataItem->updateZScale(m_zScale);
	GraphicsWindowDataModel::doLoadFromProjectMainFile(node);
}

void Post2dWindowDataModel::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	iRIC::setDoubleAttribute(writer, "ZScale", m_zScale);
	GraphicsWindowDataModel::doSaveToProjectMainFile(writer);
}

Post2dWindowRootDataItem* Post2dWindowDataModel::rootDataItem() const
{
	return dynamic_cast<Post2dWindowRootDataItem*> (m_rootDataItem);
}

Post2dWindowGraphicsView* Post2dWindowDataModel::graphicsView() const
{
	return dynamic_cast<Post2dWindowGraphicsView*>(m_graphicsView);
}

PostSolutionInfo* Post2dWindowDataModel::postSolutionInfo()
{
	return projectData()->mainfile()->postSolutionInfo();
}

void Post2dWindowDataModel::editZScale()
{
	bool ok;
	double newZscale = QInputDialog::getDouble(mainWindow(), tr("Z-direction Scale"), tr("Input new Z-direction scale."), m_zScale, 1E-6, 1E6, 3, &ok);
	if (! ok) {return;}
	m_zScale = newZscale;
	m_rootDataItem->updateZScale(newZscale);
	m_graphicsView->cameraFit();
}

double Post2dWindowDataModel::zScale() const
{
	return m_zScale;
}

void Post2dWindowDataModel::updateTmsList()
{
	rootDataItem()->tmsGroupDataItem()->rebuildChildItems();
}

void Post2dWindowDataModel::gridShapeSetting()
{
	auto zItem = getZoneDataItem();
	if (zItem == nullptr) {return;}

	auto item = zItem->resultDataItem()->gridShapeDataItem();
	if (item == nullptr) {return;}

	item->showPropertyDialog();
}

void Post2dWindowDataModel::addContour()
{
	auto zItem = getZoneDataItem();
	if (zItem == nullptr) {return;}
	auto item = zItem->resultDataItem()->nodeScalarGroupTopDataItem();
	if (item == nullptr) {
		QMessageBox::warning(mainWindow(),tr("Warning"), tr("Contour setting is not available, because this result does not contain scalar values."));
		return;
	}
	item->showAddDialog();
}

void Post2dWindowDataModel::contourSetting()
{
	addContour();
}

void Post2dWindowDataModel::vectorSetting()
{
	auto zItem = getZoneDataItem();
	if (zItem == nullptr) {return;}
	auto item = zItem->resultDataItem()->arrowGroupDataItem();
	if (item == nullptr) {
		QMessageBox::warning(mainWindow(),tr("Warning"), tr("Arrow setting is not available, because this result does not contain vector values."));
		return;
	}
	item->showAddDialog();
}

void Post2dWindowDataModel::streamlineSetting()
{
	auto zItem = getZoneDataItem();
	if (zItem == nullptr) {return;}
	auto item = zItem->resultDataItem()->streamlineDataItem();
	if (item == nullptr) {
		QMessageBox::warning(mainWindow(),tr("Warning"), tr("Streamline setting is not available, because this result does not contain vector values."));
		return;
	}
	item->showPropertyDialog();
}

void Post2dWindowDataModel::particleSetting()
{
	auto zItem = getZoneDataItem();
	if (zItem == nullptr) {return;}
	auto item = zItem->resultDataItem()->particleGroupDataItem();
	if (item == nullptr) {
		QMessageBox::warning(mainWindow(),tr("Warning"), tr("Particle setting is not available, because this result does not contain vector values."));
		return;
	}
	item->showPropertyDialog();
}

void Post2dWindowDataModel::polygonSetting()
{
	auto item = getPolyDataGroupItem();
	if (item == nullptr) {return;}

	item->showPropertyDialog();
}

void Post2dWindowDataModel::addCellScalar()
{
	auto zItem = getZoneDataItem();
	if (zItem == nullptr) { return; }
	auto item = zItem->resultDataItem()->cellScalarGroupTopDataItem();
	if (item == nullptr) {
		QMessageBox::warning(mainWindow(), tr("Warning"), tr("Contour setting is not available, because this result does not contain cell scalar values."));
		return;
	}
	item->showAddDialog();
}

void Post2dWindowDataModel::cellScalarSetting()
{
	addCellScalar();
}

Post2dWindowZoneDataItem* Post2dWindowDataModel::getZoneDataItem()
{
	GraphicsWindowDataItem* item = m_selectedItem;
	while (item != nullptr) {
		Post2dWindowZoneDataItem* zitem = dynamic_cast<Post2dWindowZoneDataItem*>(item);
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
		Post2dWindowRootDataItem* root = dynamic_cast<Post2dWindowRootDataItem*>(m_rootDataItem);
		Post2dWindowGridTypeDataItem* gt = root->gridTypeDataItem(gridType);
		return gt->zoneData(zone);
	} else {
		Post2dWindowRootDataItem* root = dynamic_cast<Post2dWindowRootDataItem*>(m_rootDataItem);
		QList<Post2dWindowGridTypeDataItem*> list = root->gridTypeDataItems();
		Post2dWindowGridTypeDataItem* gt = list.at(0);
		return gt->zoneDatas().at(0);
	}
}

Post2dWindowPolyDataGroupDataItem* Post2dWindowDataModel::getPolyDataGroupItem()
{
	GraphicsWindowDataItem* item = m_selectedItem;
	while (item != nullptr) {
		auto mitem = dynamic_cast<Post2dWindowPolyDataGroupDataItem*> (item);
		if (mitem != nullptr) {return mitem;}
		item = dynamic_cast<GraphicsWindowDataItem*> (item->parent());
	}
	return nullptr;
}

MeasuredDataFileDataItem* Post2dWindowDataModel::getMeasuredDataFileDataItem()
{
	GraphicsWindowDataItem* item = m_selectedItem;
	while (item != nullptr) {
		MeasuredDataFileDataItem* mitem = dynamic_cast<MeasuredDataFileDataItem*>(item);
		if (mitem != nullptr) {return mitem;}
		// try parent item.
		item = dynamic_cast<GraphicsWindowDataItem*>(item->parent());
		if (dynamic_cast<GraphicsWindowRootDataItem*>(item) != nullptr) {
			// reached to root node.
			break;
		}
	}
	Post2dWindowRootDataItem* root = dynamic_cast<Post2dWindowRootDataItem*>(m_rootDataItem);
	Post2dWindowMeasuredDataTopDataItem* mdItem = root->measuredDataTopDataItem();
	if (mdItem->childItems().size() == 1) {
		return dynamic_cast<MeasuredDataFileDataItem*>(mdItem->childItems().at(0));
	}
	return nullptr;
}

void Post2dWindowDataModel::createCameraConnections()
{

}

void Post2dWindowDataModel::titleSetting()
{
	Post2dWindowRootDataItem* r = dynamic_cast<Post2dWindowRootDataItem*>(m_rootDataItem);
	r->titleDataItem()->showPropertyDialog();
}

void Post2dWindowDataModel::timeSetting()
{
	Post2dWindowRootDataItem* r = dynamic_cast<Post2dWindowRootDataItem*>(m_rootDataItem);
	r->timeDataItem()->showPropertyDialog();
}

void Post2dWindowDataModel::measuredDataPointSetting()
{
	MeasuredDataFileDataItem* fItem = getMeasuredDataFileDataItem();
	if (fItem == nullptr) {return;}
	fItem->pointGroupDataItem()->showPropertyDialog();
}

void Post2dWindowDataModel::measuredDataVectorSetting()
{
	MeasuredDataFileDataItem* fItem = getMeasuredDataFileDataItem();
	if (fItem == nullptr) {return;}
	fItem->vectorGroupDataItem()->showPropertyDialog();
}

void Post2dWindowDataModel::applyOffset(double x, double y)
{
	m_rootDataItem->applyOffset(x, y);
	m_rootDataItem->renderGraphicsView();
}

void Post2dWindowDataModel::initCollapseState()
{
	auto v = objectBrowserView();
	for (auto gtItem : rootDataItem()->gridTypeDataItems()) {
		auto geoDataItem = gtItem->geoDataItem();
		if (geoDataItem != nullptr) {
			v->collapse(geoDataItem->standardItem()->index());
		}

		for (auto zitem : gtItem->zoneDatas()) {
			auto inputGridDataItem = zitem->inputGridDataItem();
			if (inputGridDataItem != nullptr) {
				v->collapse(inputGridDataItem->standardItem()->index());
			}

			auto graphDataItem = zitem->resultDataItem()->graphGroupDataItem();
			if (graphDataItem != nullptr) {
				v->collapse(graphDataItem->standardItem()->index());
			}
		}
	}
}

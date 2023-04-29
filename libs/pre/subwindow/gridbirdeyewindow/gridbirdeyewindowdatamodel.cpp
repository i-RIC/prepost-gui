#include "../../datamodel/preprocessorgridtypedataitem.h"
#include "gridbirdeyewindow.h"
#include "gridbirdeyewindowdatamodel.h"
#include "gridbirdeyewindowgraphicsview.h"
#include "gridbirdeyewindowgriddataitem.h"
#include "datamodel/gridbirdeyewindowrootdataitem.h"
#include "datamodel/gridbirdeyewindowzonedataitem.h"

#include <misc/xmlsupport.h>

GridBirdEyeWindowDataModel::GridBirdEyeWindowDataModel(GridBirdEyeWindow* w, ProjectDataItem* parent) :
	Graphics3DWindowDataModel(w, parent),
	m_zScale {1}
{
	init(w);
}

GridBirdEyeWindowDataModel::~GridBirdEyeWindowDataModel()
{
	delete m_rootDataItem;
}

GridBirdEyeWindowGraphicsView* GridBirdEyeWindowDataModel::graphicsView() const
{
	return dynamic_cast<GridBirdEyeWindowGraphicsView*>(m_graphicsView);
}

void GridBirdEyeWindowDataModel::editZScale()
{
	bool ok;
	double newZscale = QInputDialog::getDouble(mainWindow(), tr("Z-direction Scale"), tr("Input new Z-direction scale."), m_zScale, 1E-6, 1E6, 3, &ok);
	if (! ok) {return;}
	m_zScale = newZscale;
	m_rootDataItem->updateZScale(newZscale);
	m_graphicsView->cameraFit();
}

void GridBirdEyeWindowDataModel::editDisplaySetting()
{
	// m_gridDataItem->editDisplaySetting();
}

void GridBirdEyeWindowDataModel::updateGrid()
{
	zoneDataItem()->update();
}

void GridBirdEyeWindowDataModel::init(GridBirdEyeWindow* w)
{
	m_graphicsView = new GridBirdEyeWindowGraphicsView(w);

	auto root = new GridBirdEyeWindowRootDataItem(w, this);
	m_rootDataItem = root;
	root->setupStandardModel(m_itemModel);
	connect(m_itemModel, &QStandardItemModel::itemChanged, this, &GridBirdEyeWindowDataModel::handleObjectBrowserChange);

	m_graphicsView->setActiveDataItem(root);
	m_graphicsView->setModel(this);
}

void GridBirdEyeWindowDataModel::doLoadFromProjectMainFile(const QDomNode& node)
{
	GraphicsWindowDataModel::doLoadFromProjectMainFile(node);
	m_zScale = iRIC::getDoubleAttribute(node, "ZScale", 1);
	m_rootDataItem->updateZScale(m_zScale);
}

void GridBirdEyeWindowDataModel::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	iRIC::setDoubleAttribute(writer, "ZScale", m_zScale);
	m_rootDataItem->saveToProjectMainFile(writer);
}

PreProcessorGridTypeDataItem* GridBirdEyeWindowDataModel::gridTypeDataItem() const
{
	return dynamic_cast<PreProcessorGridTypeDataItem*> (gridDataItem()->parent()->parent());
}

PreProcessorGridDataItem* GridBirdEyeWindowDataModel::gridDataItem() const
{
	return dynamic_cast<PreProcessorGridDataItem*> (parent()->parent());
}

GridBirdEyeWindowZoneDataItem* GridBirdEyeWindowDataModel::zoneDataItem() const
{
	auto r = dynamic_cast<GridBirdEyeWindowRootDataItem*>(m_rootDataItem);
	return r->zoneDataItem();
}

void GridBirdEyeWindowDataModel::handleResize(QResizeEvent* event)
{
	m_rootDataItem->handleResize(event, m_graphicsView);
}

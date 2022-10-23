#include "../../datamodel/preprocessorgridtypedataitem.h"
#include "gridbirdeyewindow.h"
#include "gridbirdeyewindowdatamodel.h"
#include "gridbirdeyewindowgraphicsview.h"
#include "gridbirdeyewindowgriddataitem.h"

GridBirdEyeWindowDataModel::GridBirdEyeWindowDataModel(GridBirdEyeWindow* w, ProjectDataItem* parent) :
	GraphicsWindowSimpleDataModel(w, parent),
	m_gridDataItem {nullptr}
{
	m_graphicsView->setModel(this);
	m_gridDataItem = new GridBirdEyeWindowGridDataItem(this);
	m_graphicsView->setActiveDataItem(m_gridDataItem);
}

GridBirdEyeWindowDataModel::~GridBirdEyeWindowDataModel()
{
	delete m_gridDataItem;
}

GridBirdEyeWindowGraphicsView* GridBirdEyeWindowDataModel::graphicsView() const
{
	return dynamic_cast<GridBirdEyeWindowGraphicsView*>(m_graphicsView);
}

void GridBirdEyeWindowDataModel::editZScale()
{
	m_gridDataItem->editZScale();
}

void GridBirdEyeWindowDataModel::editDisplaySetting()
{
	m_gridDataItem->editDisplaySetting();
}

void GridBirdEyeWindowDataModel::updateGrid()
{
	m_gridDataItem->updateGrid();
}

void GridBirdEyeWindowDataModel::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_gridDataItem->loadFromProjectMainFile(node);
}

void GridBirdEyeWindowDataModel::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	m_gridDataItem->saveToProjectMainFile(writer);
}

PreProcessorGridTypeDataItem* GridBirdEyeWindowDataModel::gridTypeDataItem() const
{
	return dynamic_cast<PreProcessorGridTypeDataItem*> (gridDataItem()->parent()->parent());
}

PreProcessorGridDataItem* GridBirdEyeWindowDataModel::gridDataItem() const
{
	return dynamic_cast<PreProcessorGridDataItem*> (parent());
}

void GridBirdEyeWindowDataModel::handleResize()
{
	m_gridDataItem->handleResize(m_graphicsView);
}

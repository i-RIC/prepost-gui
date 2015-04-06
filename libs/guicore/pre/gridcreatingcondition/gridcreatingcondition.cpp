#include "gridcreatingcondition.h"
#include "gridcreatingconditioncreator.h"
#include "../base/preprocessordataitem.h"
#include "../base/preprocessorgridcreatingconditiondataiteminterface.h"
#include "../base/preprocessorwindowinterface.h"
#include "../base/preprocessorgraphicsviewinterface.h"
//#include "pre/preprocessorwindow.h"
//#include "pre/preprocessorgraphicsview.h"

#include <QMenu>

GridCreatingCondition::GridCreatingCondition(ProjectDataItem* parent, GridCreatingConditionCreator* creator)
	: ProjectDataItem("gridcreatingcondition.dat", parent)
{
	m_creator = creator;
	m_conditionDataItem = dynamic_cast<PreProcessorGridCreatingConditionDataItemInterface*>(parent);
	m_menu = new QMenu(tr("&Grid Creating Condition"), 0);
}

GridCreatingCondition::~GridCreatingCondition()
{
	delete m_menu;
}

const QString& GridCreatingCondition::name() const
{
	return m_creator->name();
}

PreProcessorWindowInterface* GridCreatingCondition::preProcessorWindow() const
{
	return dynamic_cast<PreProcessorDataItem*>(parent())->preProcessorWindow();
}

PreProcessorGraphicsViewInterface* GridCreatingCondition::graphicsView()
{
	return dynamic_cast<PreProcessorGraphicsViewInterface*>(preProcessorWindow()->centralWidget());
}

vtkRenderer* GridCreatingCondition::renderer()
{
	PreProcessorGridCreatingConditionDataItemInterface* item = dynamic_cast<PreProcessorGridCreatingConditionDataItemInterface*>(parent());
	return item->renderer();
}

vtkActorCollection* GridCreatingCondition::actorCollection()
{
	PreProcessorGridCreatingConditionDataItemInterface* item = dynamic_cast<PreProcessorGridCreatingConditionDataItemInterface*>(parent());
	return item->m_actorCollection;
}

vtkActor2DCollection* GridCreatingCondition::actor2DCollection()
{
	PreProcessorGridCreatingConditionDataItemInterface* item = dynamic_cast<PreProcessorGridCreatingConditionDataItemInterface*>(parent());
	return item->m_actor2DCollection;
}

void GridCreatingCondition::updateZDepthRangeItemCount(ZDepthRange& range)
{
	range.setItemCount(0);
}

PreProcessorDataModelInterface* GridCreatingCondition::dataModel()
{
	PreProcessorGridCreatingConditionDataItemInterface* r = dynamic_cast<PreProcessorGridCreatingConditionDataItemInterface*>(parent());
	return r->dataModel();
}

void GridCreatingCondition::renderGraphicsView()
{
	PreProcessorGridCreatingConditionDataItemInterface* item = dynamic_cast<PreProcessorGridCreatingConditionDataItemInterface*>(parent());
	item->renderGraphicsView();
}

void GridCreatingCondition::updateVisibility()
{
	PreProcessorGridCreatingConditionDataItemInterface* item = dynamic_cast<PreProcessorGridCreatingConditionDataItemInterface*>(parent());
	item->updateVisibility();
}

void GridCreatingCondition::updateVisibilityWithoutRendering()
{
	PreProcessorGridCreatingConditionDataItemInterface* item = dynamic_cast<PreProcessorGridCreatingConditionDataItemInterface*>(parent());
	item->updateVisibilityWithoutRendering();
}


ProjectData* GridCreatingCondition::projectData()
{
	return m_conditionDataItem->projectData();
}

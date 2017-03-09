#include "../base/preprocessordataitem.h"
#include "../base/preprocessorgraphicsviewinterface.h"
#include "../base/preprocessorgridcreatingconditiondataiteminterface.h"
#include "../base/preprocessorwindowinterface.h"
#include "gridcreatingcondition.h"
#include "gridcreatingconditioncreator.h"

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

GridCreatingConditionCreator* GridCreatingCondition::creator() const
{
	return m_creator;
}

const QString& GridCreatingCondition::name() const
{
	return m_creator->name();
}

QMenu* GridCreatingCondition::menu() const
{
	return m_menu;
}

PreProcessorWindowInterface* GridCreatingCondition::preProcessorWindow() const
{
	return dynamic_cast<PreProcessorDataItem*>(parent())->preProcessorWindow();
}

PreProcessorGraphicsViewInterface* GridCreatingCondition::graphicsView() const
{
	return dynamic_cast<PreProcessorGraphicsViewInterface*>(preProcessorWindow()->centralWidget());
}

vtkRenderer* GridCreatingCondition::renderer()
{
	return gccDataItem()->renderer();
}

vtkActorCollection* GridCreatingCondition::actorCollection()
{
	return gccDataItem()->actorCollection();
}

vtkActor2DCollection* GridCreatingCondition::actor2DCollection()
{
	return gccDataItem()->actor2DCollection();
}

void GridCreatingCondition::updateZDepthRangeItemCount(ZDepthRange& range)
{
	range.setItemCount(0);
}

PreProcessorGridCreatingConditionDataItemInterface* GridCreatingCondition::gccDataItem() const
{
	return dynamic_cast<PreProcessorGridCreatingConditionDataItemInterface*>(parent());
}

PreProcessorDataModelInterface* GridCreatingCondition::dataModel() const
{
	return gccDataItem()->dataModel();
}

void GridCreatingCondition::renderGraphicsView()
{
	gccDataItem()->renderGraphicsView();
}

void GridCreatingCondition::pushCommand(QUndoCommand* com)
{
	gccDataItem()->pushCommand(com);
}

void GridCreatingCondition::pushRenderCommand(QUndoCommand* com)
{
	gccDataItem()->pushRenderCommand(com, gccDataItem());
}

void GridCreatingCondition::updateVisibility()
{
	gccDataItem()->updateVisibility();
}

void GridCreatingCondition::updateVisibilityWithoutRendering()
{
	gccDataItem()->updateVisibilityWithoutRendering();
}


ProjectData* GridCreatingCondition::projectData()
{
	return m_conditionDataItem->projectData();
}

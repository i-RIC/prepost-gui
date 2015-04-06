#include "gridcreatingconditioncreatortriangle.h"
#include "gridcreatingconditiontriangle.h"
#include <guibase/irictoolbar.h>
#include <QAction>
#include <QIcon>

GridCreatingConditionCreatorTriangle::GridCreatingConditionCreatorTriangle()
	: GridCreatingConditionCreator()
{
	m_name = "triangle";
	m_caption = tr("Create grid from polygon shape");
	m_description = tr(
		"Please define a polygon as the region to create grid."
		"You can additionally define holes and refinement areas as restrictions."
	    );
}

GridCreatingCondition* GridCreatingConditionCreatorTriangle::create(ProjectDataItem* parent)
{
	return new GridCreatingConditionTriangle(parent, this);
}

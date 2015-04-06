#include "gridcreatingconditioncreatorcenterandwidth.h"
#include "gridcreatingconditioncenterandwidth.h"
#include <guibase/irictoolbar.h>
#include <QAction>
#include <QIcon>

GridCreatingConditionCreatorCenterAndWidth::GridCreatingConditionCreatorCenterAndWidth()
	: GridCreatingConditionCreator()
{
	m_name = "center_and_width";
	m_caption = tr("Create grid from polygonal line and width");
	m_description = tr(
		"First, please define polygonal line, by mouse-clicking. "
		"The polygonal line is used as the center line of the grid. "
		"Then, you define the width, division number of grid in I direction and "
		"J direction."
	    );
}

GridCreatingCondition* GridCreatingConditionCreatorCenterAndWidth::create(ProjectDataItem* parent)
{
	return new GridCreatingConditionCenterAndWidth(parent, this);
}

#include "gridcreatingconditioncreatorgridcombine.h"
#include "gridcreatingconditiongridcombine.h"

GridCreatingConditionCreatorGridCombine::GridCreatingConditionCreatorGridCombine()
	: GridCreatingConditionCreator()
{
	m_name = "grid_combine";
	m_caption = tr("Create grid by combining two grids");
	m_description = tr(
		"Grid with shape of river meeting point can be created "
		"using this algorithm. Please create or import two grids "
		"first. Selecting the main channel grid and sub channel grid, "
		"and specifying some parameters, you can create a "
		"river meeting point grid."
	    );
}

GridCreatingCondition* GridCreatingConditionCreatorGridCombine::create(ProjectDataItem* parent)
{
	return new GridCreatingConditionGridCombine(parent, this);
}

#include "gridcreatingconditioncreatorrectangularregion.h"
#include "gridcreatingconditionrectangularregion.h"

GridCreatingConditionCreatorRectangularRegion::GridCreatingConditionCreatorRectangularRegion() :
	GridCreatingConditionCreator()
{
	m_name = "rectangular_region";
	m_caption = tr("Create grid by dividing rectangular region");
	m_description = tr(
		"Specify a rectangular region where to create a grid, "
		"and the cell width. The cell width is used for both "
		"X-direction and Y-direction.");
}

GridCreatingConditionCreatorRectangularRegion::~GridCreatingConditionCreatorRectangularRegion()
{}

SolverDefinitionGridType::GridType GridCreatingConditionCreatorRectangularRegion::gridType() const
{
	return SolverDefinitionGridType::gtStructured2DGrid;
}

GridCreatingCondition* GridCreatingConditionCreatorRectangularRegion::create(ProjectDataItem* parent)
{
	return new GridCreatingConditionRectangularRegion(parent, this);
}

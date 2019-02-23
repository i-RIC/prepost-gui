#include "gridcreatingconditioncreatorlaplace.h"
#include "gridcreatingconditionlaplace.h"

GridCreatingConditionCreatorLaplace::GridCreatingConditionCreatorLaplace() :
	GridCreatingConditionCreator()
{
	m_name = "laplace";
	m_caption = tr("Create grid shape solving Laplace equation");
	m_description = tr(
		"You can create smooth grid shape solving laplace equation.");
}

GridCreatingConditionCreatorLaplace::~GridCreatingConditionCreatorLaplace()
{}

SolverDefinitionGridType::GridType GridCreatingConditionCreatorLaplace::gridType() const
{
	return SolverDefinitionGridType::gtStructured2DGrid;
}

GridCreatingCondition* GridCreatingConditionCreatorLaplace::create(ProjectDataItem* parent)
{
	return new GridCreatingConditionLaplace(parent, this);
}

#include "gridcreatingconditioncreatorpoisson.h"
#include "gridcreatingconditionpoisson.h"

GridCreatingConditionCreatorPoisson::GridCreatingConditionCreatorPoisson() :
	GridCreatingConditionCreator()
{
	m_name = "poisson";
	m_caption = tr("Create grid shape solving Poisson equation");
	m_description = tr(
		"You can create smooth grid shape solving poisson equation.");
}

GridCreatingConditionCreatorPoisson::~GridCreatingConditionCreatorPoisson()
{}

SolverDefinitionGridType::GridType GridCreatingConditionCreatorPoisson::gridType() const
{
	return SolverDefinitionGridType::gtStructured2DGrid;
}

GridCreatingCondition* GridCreatingConditionCreatorPoisson::create(ProjectDataItem* parent)
{
	return new GridCreatingConditionPoisson(parent, this);
}

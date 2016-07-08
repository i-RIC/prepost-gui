#include "gridcreatingconditioncompoundchannel.h"
#include "gridcreatingconditioncreatorcompoundchannel.h"

#include <QAction>
#include <QIcon>

GridCreatingConditionCreatorCompoundChannel::GridCreatingConditionCreatorCompoundChannel() :
	GridCreatingConditionCreator()
{
	m_name = "compoundchannel";
	m_caption = tr("Create compound channel grid");
	m_description = tr(
		"Please define a polygon as the region to create grid, and another polygon as "
		"the low water channel region. "
		"Then, please add a polygonal line as river center.\n"
		"The low water channel region should be included in grid creation region, and"
		"River center line should be included in the low water channel region.");
}

GridCreatingConditionCreatorCompoundChannel::~GridCreatingConditionCreatorCompoundChannel()
{}

SolverDefinitionGridType::GridType GridCreatingConditionCreatorCompoundChannel::gridType() const
{
	return SolverDefinitionGridType::gtStructured2DGrid;
}

GridCreatingCondition* GridCreatingConditionCreatorCompoundChannel::create(ProjectDataItem* parent)
{
	return new GridCreatingConditionCompoundChannel(parent, this);
}

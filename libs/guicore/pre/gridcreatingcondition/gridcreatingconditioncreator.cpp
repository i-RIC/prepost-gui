#include "gridcreatingcondition.h"
#include "gridcreatingconditioncreator.h"

GridCreatingConditionCreator::GridCreatingConditionCreator()
	: QObject(0)
{}

GridCreatingCondition* GridCreatingConditionCreator::restore(const QDomNode& node, ProjectDataItem* parent)
{
	GridCreatingCondition* ret = create(parent);
	ret->loadFromProjectMainFile(node);
	return ret;
}

bool GridCreatingConditionCreator::checkAvailability(PreProcessorGridCreatingConditionDataItemInterface* ditem) const
{
	return true;
}

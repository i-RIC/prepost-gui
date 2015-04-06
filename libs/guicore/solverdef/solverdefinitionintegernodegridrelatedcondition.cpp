#include "solverdefinitionintegernodegridrelatedcondition.h"
#include "../pre/gridcond/container/gridrelatedconditionintegernodecontainer.h"
#include "../pre/gridcond/editwidget/gridrelatedconditionintegereditwidget.h"
#include "../pre/gridcond/editwidget/gridrelatedconditionintegervariationeditwidget.h"

GridRelatedConditionContainer* SolverDefinitionIntegerNodeGridRelatedCondition::buildContainer(Grid* grid)
{
	return new GridRelatedConditionIntegerNodeContainer(grid, this);
}

GridRelatedConditionEditWidget* SolverDefinitionIntegerNodeGridRelatedCondition::editWidget(QWidget* parent)
{
	return new GridRelatedConditionIntegerEditWidget(parent, this);
}

GridRelatedConditionVariationEditWidget* SolverDefinitionIntegerNodeGridRelatedCondition::variationEditWidget(QWidget* parent)
{
	return new GridRelatedConditionIntegerVariationEditWidget(parent, this);
}

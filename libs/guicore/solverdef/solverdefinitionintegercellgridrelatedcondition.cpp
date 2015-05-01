#include "../pre/gridcond/container/gridrelatedconditionintegercellcontainer.h"
#include "../pre/gridcond/editwidget/gridrelatedconditionintegereditwidget.h"
#include "../pre/gridcond/editwidget/gridrelatedconditionintegervariationeditwidget.h"
#include "solverdefinitionintegercellgridrelatedcondition.h"

GridRelatedConditionContainer* SolverDefinitionIntegerCellGridRelatedCondition::buildContainer(Grid* grid)
{
	return new GridRelatedConditionIntegerCellContainer(grid, this);
}

GridRelatedConditionEditWidget* SolverDefinitionIntegerCellGridRelatedCondition::editWidget(QWidget* parent)
{
	return new GridRelatedConditionIntegerEditWidget(parent, this);
}

GridRelatedConditionVariationEditWidget* SolverDefinitionIntegerCellGridRelatedCondition::variationEditWidget(QWidget* parent)
{
	return new GridRelatedConditionIntegerVariationEditWidget(parent, this);
}

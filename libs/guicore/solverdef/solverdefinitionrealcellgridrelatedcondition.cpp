#include "solverdefinitionrealcellgridrelatedcondition.h"
#include "../pre/gridcond/container/gridrelatedconditionrealcellcontainer.h"
#include "../pre/gridcond/editwidget/gridrelatedconditionrealeditwidget.h"
#include "../pre/gridcond/editwidget/gridrelatedconditionrealvariationeditwidget.h"

GridRelatedConditionContainer* SolverDefinitionRealCellGridRelatedCondition::buildContainer(Grid* grid)
{
	return new GridRelatedConditionRealCellContainer(grid, this);
}

GridRelatedConditionEditWidget* SolverDefinitionRealCellGridRelatedCondition::editWidget(QWidget* parent)
{
	return new GridRelatedConditionRealEditWidget(parent, this);
}

GridRelatedConditionVariationEditWidget* SolverDefinitionRealCellGridRelatedCondition::variationEditWidget(QWidget* parent)
{
	return new GridRelatedConditionRealVariationEditWidget(parent, this);
}

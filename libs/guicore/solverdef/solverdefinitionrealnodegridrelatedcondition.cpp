#include "solverdefinitionrealnodegridrelatedcondition.h"
#include "../pre/gridcond/container/gridrelatedconditionrealnodecontainer.h"
#include "../pre/gridcond/editwidget/gridrelatedconditionrealeditwidget.h"
#include "../pre/gridcond/editwidget/gridrelatedconditionrealvariationeditwidget.h"

GridRelatedConditionContainer* SolverDefinitionRealNodeGridRelatedCondition::buildContainer(Grid* grid)
{
	return new GridRelatedConditionRealNodeContainer(grid, this);
}

GridRelatedConditionEditWidget* SolverDefinitionRealNodeGridRelatedCondition::editWidget(QWidget* parent)
{
	return new GridRelatedConditionRealEditWidget(parent, this);
}

GridRelatedConditionVariationEditWidget* SolverDefinitionRealNodeGridRelatedCondition::variationEditWidget(QWidget* parent)
{
	return new GridRelatedConditionRealVariationEditWidget(parent, this);
}

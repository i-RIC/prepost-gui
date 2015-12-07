#include "../pre/gridcond/container/gridattributeintegernodecontainer.h"
#include "../pre/gridcond/editwidget/gridattributeintegereditwidget.h"
#include "../pre/gridcond/editwidget/gridattributeintegervariationeditwidget.h"
#include "solverdefinitiongridattributeintegernode.h"

GridAttributeEditWidget* SolverDefinitionGridAttributeIntegerNode::editWidget(QWidget* parent)
{
	return new GridAttributeIntegerEditWidget(parent, this);
}

GridAttributeVariationEditWidget* SolverDefinitionGridAttributeIntegerNode::variationEditWidget(QWidget* parent)
{
	return new GridAttributeIntegerVariationEditWidget(parent, this);
}

GridAttributeContainer* SolverDefinitionGridAttributeIntegerNode::buildContainer(Grid* grid)
{
	return new GridAttributeIntegerNodeContainer(grid, this);
}

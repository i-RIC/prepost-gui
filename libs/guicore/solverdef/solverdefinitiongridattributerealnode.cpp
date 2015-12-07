#include "../pre/gridcond/container/gridattributerealnodecontainer.h"
#include "../pre/gridcond/editwidget/gridattributerealeditwidget.h"
#include "../pre/gridcond/editwidget/gridattributerealvariationeditwidget.h"
#include "solverdefinitiongridattributerealnode.h"

GridAttributeEditWidget* SolverDefinitionGridAttributeRealNode::editWidget(QWidget* parent)
{
	return new GridAttributeRealEditWidget(parent, this);
}

GridAttributeVariationEditWidget* SolverDefinitionGridAttributeRealNode::variationEditWidget(QWidget* parent)
{
	return new GridAttributeRealVariationEditWidget(parent, this);
}

GridAttributeContainer* SolverDefinitionGridAttributeRealNode::buildContainer(Grid* grid)
{
	return new GridAttributeRealNodeContainer(grid, this);
}

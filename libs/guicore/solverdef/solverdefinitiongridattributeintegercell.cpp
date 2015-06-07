#include "../pre/gridcond/container/gridattributeintegercellcontainer.h"
#include "../pre/gridcond/editwidget/gridattributeintegereditwidget.h"
#include "../pre/gridcond/editwidget/gridattributeintegervariationeditwidget.h"
#include "solverdefinitiongridattributeintegercell.h"

GridAttributeContainer* SolverDefinitionGridAttributeIntegerCell::buildContainer(Grid* grid)
{
	return new GridAttributeIntegerCellContainer(grid, this);
}

GridAttributeEditWidget* SolverDefinitionGridAttributeIntegerCell::editWidget(QWidget* parent)
{
	return new GridAttributeIntegerEditWidget(parent, this);
}

GridAttributeVariationEditWidget* SolverDefinitionGridAttributeIntegerCell::variationEditWidget(QWidget* parent)
{
	return new GridAttributeIntegerVariationEditWidget(parent, this);
}

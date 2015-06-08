#include "../pre/gridcond/container/gridattributerealcellcontainer.h"
#include "../pre/gridcond/editwidget/gridattributerealeditwidget.h"
#include "../pre/gridcond/editwidget/gridattributerealvariationeditwidget.h"
#include "solverdefinitiongridattributerealcell.h"

GridAttributeContainer* SolverDefinitionGridAttributeRealCell::buildContainer(Grid* grid)
{
	return new GridAttributeRealCellContainer(grid, this);
}

GridAttributeEditWidget* SolverDefinitionGridAttributeRealCell::editWidget(QWidget* parent)
{
	return new GridAttributeRealEditWidget(parent, this);
}

GridAttributeVariationEditWidget* SolverDefinitionGridAttributeRealCell::variationEditWidget(QWidget* parent)
{
	return new GridAttributeRealVariationEditWidget(parent, this);
}

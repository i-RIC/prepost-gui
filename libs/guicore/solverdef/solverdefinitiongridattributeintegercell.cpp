#include "../pre/gridcond/container/gridattributeintegercellcontainer.h"
#include "../pre/gridcond/editwidget/gridattributeintegereditwidget.h"
#include "../pre/gridcond/editwidget/gridattributeintegervariationeditwidget.h"
#include "../scalarstocolors/colormapfactory.h"
#include "solverdefinitiongridattributeintegercell.h"

SolverDefinitionGridAttributeIntegerCell::SolverDefinitionGridAttributeIntegerCell(const QDomElement& elem, SolverDefinition* solverDef, int order) :
	SolverDefinitionGridAttributeInteger{ elem, solverDef, CellCenter, false, order }
{
	setColorMapFactory(new ColorMapFactory());
}

GridAttributeEditWidget* SolverDefinitionGridAttributeIntegerCell::editWidget(QWidget* parent)
{
	return new GridAttributeIntegerEditWidget(parent, this);
}

GridAttributeVariationEditWidget* SolverDefinitionGridAttributeIntegerCell::variationEditWidget(QWidget* parent)
{
	return new GridAttributeIntegerVariationEditWidget(parent, this);
}

SolverDefinitionGridAttributeIntegerCell::SolverDefinitionGridAttributeIntegerCell(const QDomElement& elem, SolverDefinition* solverDef, bool isOption, int order) :
	SolverDefinitionGridAttributeInteger{elem, solverDef, CellCenter, isOption, order }
{}

GridAttributeContainer* SolverDefinitionGridAttributeIntegerCell::buildContainer(Grid* grid)
{
	return new GridAttributeIntegerCellContainer(grid, this);
}

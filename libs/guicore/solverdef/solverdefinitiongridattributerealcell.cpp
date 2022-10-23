#include "../pre/gridcond/container/gridattributerealcellcontainer.h"
#include "../pre/gridcond/editwidget/gridattributerealeditwidget.h"
#include "../pre/gridcond/editwidget/gridattributerealvariationeditwidget.h"
#include "../scalarstocolors/colormapfactory.h"
#include "solverdefinitiongridattributerealcell.h"

SolverDefinitionGridAttributeRealCell::SolverDefinitionGridAttributeRealCell(const QDomElement& elem, SolverDefinition *solverDef, int order) :
	SolverDefinitionGridAttributeReal {elem, solverDef, CellCenter, false, order}
{
	setColorMapFactory(new ColorMapFactory());
}

GridAttributeEditWidget* SolverDefinitionGridAttributeRealCell::editWidget(QWidget* parent)
{
	return new GridAttributeRealEditWidget(parent, this);
}

GridAttributeVariationEditWidget* SolverDefinitionGridAttributeRealCell::variationEditWidget(QWidget* parent)
{
	return new GridAttributeRealVariationEditWidget(parent, this);
}

SolverDefinitionGridAttributeRealCell::SolverDefinitionGridAttributeRealCell(const QDomElement& elem, SolverDefinition *solverDef, bool isOption, int order) :
	SolverDefinitionGridAttributeReal {elem, solverDef, CellCenter, isOption, order}
{}

GridAttributeContainer* SolverDefinitionGridAttributeRealCell::buildContainer(Grid* grid)
{
	return new GridAttributeRealCellContainer(grid, this);
}

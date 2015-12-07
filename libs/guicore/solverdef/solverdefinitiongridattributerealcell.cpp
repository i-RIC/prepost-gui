#include "../pre/gridcond/container/gridattributerealcellcontainer.h"
#include "../pre/gridcond/editwidget/gridattributerealeditwidget.h"
#include "../pre/gridcond/editwidget/gridattributerealvariationeditwidget.h"
#include "solverdefinitiongridattributerealcell.h"

SolverDefinitionGridAttributeRealCell::SolverDefinitionGridAttributeRealCell(const QDomElement& elem, const SolverDefinitionTranslator& translator, int order) :
	SolverDefinitionGridAttributeReal {elem, translator, CellCenter, false, order}
{}

GridAttributeEditWidget* SolverDefinitionGridAttributeRealCell::editWidget(QWidget* parent)
{
	return new GridAttributeRealEditWidget(parent, this);
}

GridAttributeVariationEditWidget* SolverDefinitionGridAttributeRealCell::variationEditWidget(QWidget* parent)
{
	return new GridAttributeRealVariationEditWidget(parent, this);
}

SolverDefinitionGridAttributeRealCell::SolverDefinitionGridAttributeRealCell(const QDomElement& elem, const SolverDefinitionTranslator& translator, bool isOption, int order) :
	SolverDefinitionGridAttributeReal {elem, translator, CellCenter, isOption, order}
{}

GridAttributeContainer* SolverDefinitionGridAttributeRealCell::buildContainer(Grid* grid)
{
	return new GridAttributeRealCellContainer(grid, this);
}

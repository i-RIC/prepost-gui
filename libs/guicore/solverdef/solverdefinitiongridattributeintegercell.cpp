#include "../pre/gridcond/container/gridattributeintegercellcontainer.h"
#include "../pre/gridcond/editwidget/gridattributeintegereditwidget.h"
#include "../pre/gridcond/editwidget/gridattributeintegervariationeditwidget.h"
#include "solverdefinitiongridattributeintegercell.h"

SolverDefinitionGridAttributeIntegerCell::SolverDefinitionGridAttributeIntegerCell(const QDomElement& elem, const SolverDefinitionTranslator& translator, int order) :
	SolverDefinitionGridAttributeInteger{ elem, translator, CellCenter, false, order }
{}

GridAttributeEditWidget* SolverDefinitionGridAttributeIntegerCell::editWidget(QWidget* parent)
{
	return new GridAttributeIntegerEditWidget(parent, this);
}

GridAttributeVariationEditWidget* SolverDefinitionGridAttributeIntegerCell::variationEditWidget(QWidget* parent)
{
	return new GridAttributeIntegerVariationEditWidget(parent, this);
}

SolverDefinitionGridAttributeIntegerCell::SolverDefinitionGridAttributeIntegerCell(const QDomElement& elem, const SolverDefinitionTranslator& translator, bool isOption, int order) :
	SolverDefinitionGridAttributeInteger{elem, translator, CellCenter, isOption, order }
{}

GridAttributeContainer* SolverDefinitionGridAttributeIntegerCell::buildContainer(Grid* grid)
{
	return new GridAttributeIntegerCellContainer(grid, this);
}

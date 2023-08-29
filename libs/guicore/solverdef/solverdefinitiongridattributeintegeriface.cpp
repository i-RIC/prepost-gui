#include "../pre/gridcond/container/gridattributeintegerifacecontainer.h"
#include "../pre/gridcond/editwidget/gridattributeintegereditwidget.h"
#include "../pre/gridcond/editwidget/gridattributeintegervariationeditwidget.h"
#include "../scalarstocolors/colormapfactory.h"
#include "solverdefinitiongridattributeintegeriface.h"

SolverDefinitionGridAttributeIntegerIFace::SolverDefinitionGridAttributeIntegerIFace(const QDomElement& elem, SolverDefinition* solverDef, int order) :
	SolverDefinitionGridAttributeInteger{ elem, solverDef, CellCenter, false, order }
{
	setColorMapFactory(new ColorMapFactory());
}

GridAttributeEditWidget* SolverDefinitionGridAttributeIntegerIFace::editWidget(QWidget* parent)
{
	return new GridAttributeIntegerEditWidget(parent, this);
}

GridAttributeVariationEditWidget* SolverDefinitionGridAttributeIntegerIFace::variationEditWidget(QWidget* parent)
{
	return new GridAttributeIntegerVariationEditWidget(parent, this);
}

SolverDefinitionGridAttributeIntegerIFace::SolverDefinitionGridAttributeIntegerIFace(const QDomElement& elem, SolverDefinition* solverDef, bool isOption, int order) :
	SolverDefinitionGridAttributeInteger{elem, solverDef, CellCenter, isOption, order }
{}

GridAttributeContainer* SolverDefinitionGridAttributeIntegerIFace::buildContainer(Grid* grid)
{
	return new GridAttributeIntegerIFaceContainer(grid, this);
}

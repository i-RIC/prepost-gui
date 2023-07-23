#include "../pre/gridcond/container/gridattributeintegerjfacecontainer.h"
#include "../pre/gridcond/editwidget/gridattributeintegereditwidget.h"
#include "../pre/gridcond/editwidget/gridattributeintegervariationeditwidget.h"
#include "../scalarstocolors/colormapfactory.h"
#include "solverdefinitiongridattributeintegerjface.h"

SolverDefinitionGridAttributeIntegerJFace::SolverDefinitionGridAttributeIntegerJFace(const QDomElement& elem, SolverDefinition* solverDef, int order) :
	SolverDefinitionGridAttributeInteger{ elem, solverDef, CellCenter, false, order }
{
	setColorMapFactory(new ColorMapFactory());
}

GridAttributeEditWidget* SolverDefinitionGridAttributeIntegerJFace::editWidget(QWidget* parent)
{
	return new GridAttributeIntegerEditWidget(parent, this);
}

GridAttributeVariationEditWidget* SolverDefinitionGridAttributeIntegerJFace::variationEditWidget(QWidget* parent)
{
	return new GridAttributeIntegerVariationEditWidget(parent, this);
}

SolverDefinitionGridAttributeIntegerJFace::SolverDefinitionGridAttributeIntegerJFace(const QDomElement& elem, SolverDefinition* solverDef, bool isOption, int order) :
	SolverDefinitionGridAttributeInteger{elem, solverDef, CellCenter, isOption, order }
{}

GridAttributeContainer* SolverDefinitionGridAttributeIntegerJFace::buildContainer(Grid* grid)
{
	return new GridAttributeIntegerJFaceContainer(grid, this);
}

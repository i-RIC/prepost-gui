#include "../pre/gridcond/container/gridattributerealifacecontainer.h"
#include "../pre/gridcond/editwidget/gridattributerealeditwidget.h"
#include "../pre/gridcond/editwidget/gridattributerealvariationeditwidget.h"
#include "../scalarstocolors/colormapfactory.h"
#include "solverdefinitiongridattributerealiface.h"

SolverDefinitionGridAttributeRealIFace::SolverDefinitionGridAttributeRealIFace(const QDomElement& elem, SolverDefinition *solverDef, int order) :
	SolverDefinitionGridAttributeReal {elem, solverDef, CellCenter, false, order}
{
	setColorMapFactory(new ColorMapFactory());
}

GridAttributeEditWidget* SolverDefinitionGridAttributeRealIFace::editWidget(QWidget* parent)
{
	return new GridAttributeRealEditWidget(parent, this);
}

GridAttributeVariationEditWidget* SolverDefinitionGridAttributeRealIFace::variationEditWidget(QWidget* parent)
{
	return new GridAttributeRealVariationEditWidget(parent, this);
}

SolverDefinitionGridAttributeRealIFace::SolverDefinitionGridAttributeRealIFace(const QDomElement& elem, SolverDefinition *solverDef, bool isOption, int order) :
	SolverDefinitionGridAttributeReal {elem, solverDef, CellCenter, isOption, order}
{}

GridAttributeContainer* SolverDefinitionGridAttributeRealIFace::buildContainer(Grid* grid)
{
	return new GridAttributeRealIFaceContainer(grid, this);
}

#include "../pre/gridcond/container/gridattributerealjfacecontainer.h"
#include "../pre/gridcond/editwidget/gridattributerealeditwidget.h"
#include "../pre/gridcond/editwidget/gridattributerealvariationeditwidget.h"
#include "../scalarstocolors/colormapfactory.h"
#include "solverdefinitiongridattributerealjface.h"

SolverDefinitionGridAttributeRealJFace::SolverDefinitionGridAttributeRealJFace(const QDomElement& elem, SolverDefinition *solverDef, int order) :
	SolverDefinitionGridAttributeReal {elem, solverDef, CellCenter, false, order}
{
	setColorMapFactory(new ColorMapFactory());
}

GridAttributeEditWidget* SolverDefinitionGridAttributeRealJFace::editWidget(QWidget* parent)
{
	return new GridAttributeRealEditWidget(parent, this);
}

GridAttributeVariationEditWidget* SolverDefinitionGridAttributeRealJFace::variationEditWidget(QWidget* parent)
{
	return new GridAttributeRealVariationEditWidget(parent, this);
}

SolverDefinitionGridAttributeRealJFace::SolverDefinitionGridAttributeRealJFace(const QDomElement& elem, SolverDefinition *solverDef, bool isOption, int order) :
	SolverDefinitionGridAttributeReal {elem, solverDef, CellCenter, isOption, order}
{}

GridAttributeContainer* SolverDefinitionGridAttributeRealJFace::buildContainer(Grid* grid)
{
	return new GridAttributeRealJFaceContainer(grid, this);
}

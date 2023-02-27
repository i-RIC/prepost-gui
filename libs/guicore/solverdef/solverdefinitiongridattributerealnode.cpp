#include "../pre/gridcond/container/gridattributerealnodecontainer.h"
#include "../pre/gridcond/editwidget/gridattributerealeditwidget.h"
#include "../pre/gridcond/editwidget/gridattributerealvariationeditwidget.h"
#include "../scalarstocolors/colormapfactory.h"
#include "solverdefinitiongridattributerealnode.h"

SolverDefinitionGridAttributeRealNode::SolverDefinitionGridAttributeRealNode(const QDomElement& elem, SolverDefinition* solverDef, int order) :
	SolverDefinitionGridAttributeReal {elem, solverDef, Node, false, order}
{
	setColorMapFactory(new ColorMapFactory());
}

GridAttributeEditWidget* SolverDefinitionGridAttributeRealNode::editWidget(QWidget* parent)
{
	return new GridAttributeRealEditWidget(parent, this);
}

GridAttributeVariationEditWidget* SolverDefinitionGridAttributeRealNode::variationEditWidget(QWidget* parent)
{
	return new GridAttributeRealVariationEditWidget(parent, this);
}

SolverDefinitionGridAttributeRealNode::SolverDefinitionGridAttributeRealNode(const QDomElement& elem, SolverDefinition* solverDef, bool isOption, int order) :
	SolverDefinitionGridAttributeReal {elem, solverDef, Node, isOption, order}
{}

GridAttributeContainer* SolverDefinitionGridAttributeRealNode::buildContainer(Grid* grid)
{
	return new GridAttributeRealNodeContainer(grid, this);
}

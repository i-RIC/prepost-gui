#include "../pre/gridcond/container/gridattributerealnodecontainer.h"
#include "../pre/gridcond/editwidget/gridattributerealeditwidget.h"
#include "../pre/gridcond/editwidget/gridattributerealvariationeditwidget.h"
#include "solverdefinitiongridattributerealnode.h"

SolverDefinitionGridAttributeRealNode::SolverDefinitionGridAttributeRealNode(const QDomElement& elem, const SolverDefinitionTranslator& translator, int order) :
	SolverDefinitionGridAttributeReal {elem, translator, Node, false, order}
{}

GridAttributeEditWidget* SolverDefinitionGridAttributeRealNode::editWidget(QWidget* parent)
{
	return new GridAttributeRealEditWidget(parent, this);
}

GridAttributeVariationEditWidget* SolverDefinitionGridAttributeRealNode::variationEditWidget(QWidget* parent)
{
	return new GridAttributeRealVariationEditWidget(parent, this);
}

SolverDefinitionGridAttributeRealNode::SolverDefinitionGridAttributeRealNode(const QDomElement& elem, const SolverDefinitionTranslator& translator, bool isOption, int order) :
	SolverDefinitionGridAttributeReal {elem, translator, Node, isOption, order}
{}

GridAttributeContainer* SolverDefinitionGridAttributeRealNode::buildContainer(Grid* grid)
{
	return new GridAttributeRealNodeContainer(grid, this);
}

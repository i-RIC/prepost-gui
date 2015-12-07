#include "../pre/gridcond/container/gridattributeintegernodecontainer.h"
#include "../pre/gridcond/editwidget/gridattributeintegereditwidget.h"
#include "../pre/gridcond/editwidget/gridattributeintegervariationeditwidget.h"
#include "solverdefinitiongridattributeintegernode.h"

SolverDefinitionGridAttributeIntegerNode::SolverDefinitionGridAttributeIntegerNode(const QDomElement& elem, const SolverDefinitionTranslator& translator, int order) :
	SolverDefinitionGridAttributeInteger {elem, translator, Node, false, order}
{}

GridAttributeEditWidget* SolverDefinitionGridAttributeIntegerNode::editWidget(QWidget* parent)
{
	return new GridAttributeIntegerEditWidget(parent, this);
}

GridAttributeVariationEditWidget* SolverDefinitionGridAttributeIntegerNode::variationEditWidget(QWidget* parent)
{
	return new GridAttributeIntegerVariationEditWidget(parent, this);
}

SolverDefinitionGridAttributeIntegerNode::SolverDefinitionGridAttributeIntegerNode(const QDomElement& elem, const SolverDefinitionTranslator& translator, bool isOption, int order) :
	SolverDefinitionGridAttributeInteger {elem, translator, Node, isOption, order}
{}

GridAttributeContainer* SolverDefinitionGridAttributeIntegerNode::buildContainer(Grid* grid)
{
	return new GridAttributeIntegerNodeContainer(grid, this);
}

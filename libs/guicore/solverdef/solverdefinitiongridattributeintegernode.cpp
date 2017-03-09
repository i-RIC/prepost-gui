#include "../pre/gridcond/container/gridattributeintegernodecontainer.h"
#include "../pre/gridcond/editwidget/gridattributeintegereditwidget.h"
#include "../pre/gridcond/editwidget/gridattributeintegervariationeditwidget.h"
#include "solverdefinitiongridattributeintegernode.h"

SolverDefinitionGridAttributeIntegerNode::SolverDefinitionGridAttributeIntegerNode(const QDomElement& elem, SolverDefinition* solverDef, int order) :
	SolverDefinitionGridAttributeInteger {elem, solverDef, Node, false, order}
{}

GridAttributeEditWidget* SolverDefinitionGridAttributeIntegerNode::editWidget(QWidget* parent)
{
	return new GridAttributeIntegerEditWidget(parent, this);
}

GridAttributeVariationEditWidget* SolverDefinitionGridAttributeIntegerNode::variationEditWidget(QWidget* parent)
{
	return new GridAttributeIntegerVariationEditWidget(parent, this);
}

SolverDefinitionGridAttributeIntegerNode::SolverDefinitionGridAttributeIntegerNode(const QDomElement& elem, SolverDefinition* solverDef, bool isOption, int order) :
	SolverDefinitionGridAttributeInteger {elem, solverDef, Node, isOption, order}
{}

GridAttributeContainer* SolverDefinitionGridAttributeIntegerNode::buildContainer(Grid* grid)
{
	return new GridAttributeIntegerNodeContainer(grid, this);
}

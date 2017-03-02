#include "solverdefinitiongridattributeintegerdimension.h"
#include "../pre/gridcond/base/gridattributedimensionintegercontainer.h"
#include <QVariant>

SolverDefinitionGridAttributeIntegerDimension::SolverDefinitionGridAttributeIntegerDimension(const QDomElement& elem, SolverDefinition* solverDef, SolverDefinitionGridAttribute* cond) :
	SolverDefinitionGridAttributeDimensionT<int> (elem, solverDef, cond)
{}

int SolverDefinitionGridAttributeIntegerDimension::fromVariant(const QVariant& v) const
{
	return v.toInt();
}

GridAttributeDimensionContainer* SolverDefinitionGridAttributeIntegerDimension::buildContainer()
{
	return new GridAttributeDimensionIntegerContainer(this);
}

#include "solverdefinitiongridattributerealdimension.h"
#include "../pre/gridcond/base/gridattributedimensionrealcontainer.h"
#include <QVariant>

SolverDefinitionGridAttributeRealDimension::SolverDefinitionGridAttributeRealDimension(const QDomElement& elem, SolverDefinition* solverDef, SolverDefinitionGridAttribute* cond) :
	SolverDefinitionGridAttributeDimensionT<double> (elem, solverDef, cond)
{}

double SolverDefinitionGridAttributeRealDimension::fromVariant(const QVariant& v) const
{
	return v.toDouble();
}

GridAttributeDimensionContainer* SolverDefinitionGridAttributeRealDimension::buildContainer()
{
	return new GridAttributeDimensionRealContainer(this);
}

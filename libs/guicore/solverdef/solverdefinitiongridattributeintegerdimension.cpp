#include "solverdefinitiongridattributeintegerdimension.h"
#include "../pre/gridcond/base/gridattributedimensionintegercontainer.h"
#include <QVariant>

SolverDefinitionGridAttributeIntegerDimension::SolverDefinitionGridAttributeIntegerDimension(const QDomElement& elem, const SolverDefinitionTranslator& translator, SolverDefinitionGridAttribute* cond) :
	SolverDefinitionGridAttributeDimensionT<int> (elem, translator, cond)
{}

int SolverDefinitionGridAttributeIntegerDimension::fromVariant(const QVariant& v) const
{
	return v.toInt();
}

GridAttributeDimensionContainer* SolverDefinitionGridAttributeIntegerDimension::buildContainer()
{
	return new GridAttributeDimensionIntegerContainer(this);
}

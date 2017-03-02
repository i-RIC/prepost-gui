#ifndef SOLVERDEFINITIONGRIDATTRIBUTET_DETAIL_H
#define SOLVERDEFINITIONGRIDATTRIBUTET_DETAIL_H

#include "../solverdefinitiongridattributet.h"

template <class V>
SolverDefinitionGridAttributeT<V>::SolverDefinitionGridAttributeT(const QDomElement& node, SolverDefinition* solverDef, Position pos, bool isOption, int order) :
	SolverDefinitionGridAttribute {node, solverDef, pos, isOption, order}
{}

template <class V>
V SolverDefinitionGridAttributeT<V>::defaultValue() const
{
	return fromVariant(variantDefaultValue());
}

#endif // SOLVERDEFINITIONGRIDATTRIBUTET_DETAIL_H

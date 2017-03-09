#ifndef SOLVERDEFINITIONGRIDATTRIBUTEDIMENSIONT_DETAIL_H
#define SOLVERDEFINITIONGRIDATTRIBUTEDIMENSIONT_DETAIL_H

#include "../solverdefinitiongridattributedimensiont.h"

template <class V>
SolverDefinitionGridAttributeDimensionT<V>::SolverDefinitionGridAttributeDimensionT(const QDomElement& elem, SolverDefinition* solverDef, SolverDefinitionGridAttribute* att) :
	SolverDefinitionGridAttributeDimension(elem, solverDef, att)
{}

template <class V>
SolverDefinitionGridAttributeDimensionT<V>::~SolverDefinitionGridAttributeDimensionT()
{}

#endif // SOLVERDEFINITIONGRIDATTRIBUTEDIMENSIONT_DETAIL_H

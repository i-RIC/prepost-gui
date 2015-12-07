#ifndef SOLVERDEFINITIONGRIDATTRIBUTEDIMENSIONT_DETAIL_H
#define SOLVERDEFINITIONGRIDATTRIBUTEDIMENSIONT_DETAIL_H

#include "../solverdefinitiongridattributedimensiont.h"

template <class V>
SolverDefinitionGridAttributeDimensionT<V>::SolverDefinitionGridAttributeDimensionT(const QDomElement& elem, const SolverDefinitionTranslator& translator, SolverDefinitionGridAttribute* att) :
	SolverDefinitionGridAttributeDimension(elem, translator, att)
{}

template <class V>
SolverDefinitionGridAttributeDimensionT<V>::~SolverDefinitionGridAttributeDimensionT()
{}

#endif // SOLVERDEFINITIONGRIDATTRIBUTEDIMENSIONT_DETAIL_H

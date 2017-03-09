#ifndef SOLVERDEFINITIONGRIDATTRIBUTEDIMENSIONT_H
#define SOLVERDEFINITIONGRIDATTRIBUTEDIMENSIONT_H

#include "solverdefinitiongridattributedimension.h"

class QVariant;

template <class V>
class SolverDefinitionGridAttributeDimensionT : public SolverDefinitionGridAttributeDimension
{
public:
	SolverDefinitionGridAttributeDimensionT(const QDomElement& elem, SolverDefinition* solverDef, SolverDefinitionGridAttribute* cond);
	virtual ~SolverDefinitionGridAttributeDimensionT();

	virtual V fromVariant(const QVariant& v) const = 0;
};

#include "private/solverdefinitiongridattributedimensiont_detail.h"

#endif // SOLVERDEFINITIONGRIDATTRIBUTEDIMENSIONT_H

#ifndef SOLVERDEFINITIONGRIDATTRIBUTET_H
#define SOLVERDEFINITIONGRIDATTRIBUTET_H

#include "../guicore_global.h"
#include "solverdefinitiongridattribute.h"
#include <QVariant>

template <class V>
class SolverDefinitionGridAttributeT : public SolverDefinitionGridAttribute
{
public:
	SolverDefinitionGridAttributeT(const QDomElement& node, SolverDefinition* solverDef, Position pos, bool isOption, int order);

	V defaultValue() const;

	virtual V fromVariant(const QVariant& v) const = 0;
};

#include "private/solverdefinitiongridattributet_detail.h"

#endif // SOLVERDEFINITIONGRIDATTRIBUTET_H

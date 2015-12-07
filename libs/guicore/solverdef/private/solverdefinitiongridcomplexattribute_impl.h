#ifndef SOLVERDEFINITIONGRIDCOMPLEXATTRIBUTE_IMPL_H
#define SOLVERDEFINITIONGRIDCOMPLEXATTRIBUTE_IMPL_H

#include "../solverdefinitiongridcomplexattribute.h"

#include <QDomElement>

class SolverDefinitionGridComplexAttribute::Impl
{
public:
	Impl(const QDomElement& elem, SolverDefinitionGridComplexAttribute* parent);
	void load(const QDomElement& elem, SolverDefinitionGridComplexAttribute* parent);

	QDomElement m_element;
};

#endif // SOLVERDEFINITIONGRIDCOMPLEXATTRIBUTE_IMPL_H

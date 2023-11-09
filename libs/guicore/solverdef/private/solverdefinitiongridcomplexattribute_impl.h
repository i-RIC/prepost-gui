#ifndef SOLVERDEFINITIONGRIDCOMPLEXATTRIBUTE_IMPL_H
#define SOLVERDEFINITIONGRIDCOMPLEXATTRIBUTE_IMPL_H

#include "../solverdefinitiongridcomplexattribute.h"

#include <QDomElement>

class SolverDefinitionGridComplexAttribute::Impl
{
public:
	Impl(const QDomElement& elem);
	void load(const QDomElement& elem);

	QDomElement m_element;
	bool m_isGrouped;
};

#endif // SOLVERDEFINITIONGRIDCOMPLEXATTRIBUTE_IMPL_H

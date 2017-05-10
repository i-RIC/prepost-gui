#ifndef SOLVERDEFINITIONGRIDATTRIBUTEDIMENSION_IMPL_H
#define SOLVERDEFINITIONGRIDATTRIBUTEDIMENSION_IMPL_H

#include "../solverdefinitiongridattributedimension.h"

#include <QString>

class SolverDefinitionGridAttribute;

class SolverDefinitionGridAttributeDimension::Impl
{
public:
	Impl(const QDomElement& node, SolverDefinition* solverDef, SolverDefinitionGridAttribute* att);
	void load(const QDomElement& node, SolverDefinition* solverDef);

	std::string m_name;
	QString m_caption;
	std::string m_englishCaption;

	SolverDefinitionGridAttribute* m_attribute;
};

#endif // SOLVERDEFINITIONGRIDATTRIBUTEDIMENSION_IMPL_H

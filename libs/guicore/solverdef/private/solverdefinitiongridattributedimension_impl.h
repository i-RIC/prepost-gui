#ifndef SOLVERDEFINITIONGRIDATTRIBUTEDIMENSION_IMPL_H
#define SOLVERDEFINITIONGRIDATTRIBUTEDIMENSION_IMPL_H

#include "../solverdefinitiongridattributedimension.h"

#include <QString>

class SolverDefinitionGridAttribute;

class SolverDefinitionGridAttributeDimension::Impl
{
public:
	Impl(const QDomElement& node, const SolverDefinitionTranslator& translator, SolverDefinitionGridAttribute* att);
	void load(const QDomElement& node, const SolverDefinitionTranslator& translator);

	std::string m_name;
	QString m_caption;
	std::string m_englishCaption;

	SolverDefinitionGridAttribute* m_attribute;
};

#endif // SOLVERDEFINITIONGRIDATTRIBUTEDIMENSION_IMPL_H

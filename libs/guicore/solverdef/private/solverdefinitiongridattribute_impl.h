#ifndef SOLVERDEFINITIONGRIDATTRIBUTE_IMPL_H
#define SOLVERDEFINITIONGRIDATTRIBUTE_IMPL_H

#include "../solverdefinitiongridattribute.h"

#include <QList>
#include <QString>
#include <QVariant>

class SolverDefinitionGridAttribute::Impl
{
public:
	Impl(const QDomElement& elem, const SolverDefinitionTranslator& translator, SolverDefinitionGridAttribute* parent);
	~Impl();

	void load(const QDomElement& elem, const SolverDefinitionTranslator& translator);

	std::string m_name;
	QString m_caption;
	std::string m_englishCaption;
	bool m_isOption {false};
	QVariant m_variantDefaultValue;
	QVariant m_variantMaximumValue;
	QVariant m_variantMinimumValue;
	Position m_position {Node};
	int m_order {0};
	QList<SolverDefinitionGridAttributeDimension*> m_dimensions;

	SolverDefinitionGridAttribute* m_parent;
};

#endif // SOLVERDEFINITIONGRIDATTRIBUTE_IMPL_H

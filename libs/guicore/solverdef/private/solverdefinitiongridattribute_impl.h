#ifndef SOLVERDEFINITIONGRIDATTRIBUTE_IMPL_H
#define SOLVERDEFINITIONGRIDATTRIBUTE_IMPL_H

#include "../solverdefinitiongridattribute.h"

#include <QString>
#include <QVariant>

#include <vector>

class SolverDefinitionGridAttribute::Impl
{
public:
	Impl(const QDomElement& elem, SolverDefinition* solverDef, SolverDefinitionGridAttribute* parent);
	~Impl();

	void load(const QDomElement& elem, SolverDefinition* solverDef);

	std::string m_name;
	QString m_caption;
	std::string m_englishCaption;
	bool m_isOption {false};
	QVariant m_variantDefaultValue;
	QVariant m_variantMaximumValue;
	QVariant m_variantMinimumValue;
	Position m_position {Node};
	QString m_mapping;
	QString m_mappingArgs;
	int m_order {0};
	bool m_isReferenceInformation;
	std::vector<SolverDefinitionGridAttributeDimension*> m_dimensions;
	SolverDefinition* m_solverDefinition;

	SolverDefinitionGridAttribute* m_parent;
};

#endif // SOLVERDEFINITIONGRIDATTRIBUTE_IMPL_H

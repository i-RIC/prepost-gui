#ifndef SOLVERDEFINITIONBOUNDARYCONDITION_IMPL_H
#define SOLVERDEFINITIONBOUNDARYCONDITION_IMPL_H

#include "../solverdefinitionboundarycondition.h"

#include <QDomElement>
#include <QString>

#include <string>

class SolverDefinition;

class SolverDefinitionBoundaryCondition::Impl
{
public:
	Impl(const QDomElement& node, const SolverDefinitionTranslator& translator);
	void load(const QDomElement& node, const SolverDefinitionTranslator& translator);

	std::string m_name;
	QString m_caption;
	std::string m_englishCaption;
	Position m_position;
	QDomElement m_element;
	SolverDefinition* m_definition;
};

#endif // SOLVERDEFINITIONBOUNDARYCONDITION_IMPL_H

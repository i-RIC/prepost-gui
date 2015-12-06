#include "solverdefinitionboundarycondition.h"
#include "solverdefinitiontranslator.h"
#include "solverdefinition.h"

#include <misc/stringtool.h>

class SolverDefinitionBoundaryCondition::Impl
{
public:
	Impl(const QDomElement& node, const SolverDefinitionTranslator& translator);
	void load(const QDomElement& node, const SolverDefinitionTranslator& translator);

	std::string m_name;
	QString m_caption;
	std::string  m_englishCaption;
	Position m_position;
	QDomElement m_element;
	SolverDefinition* m_definition;
};

SolverDefinitionBoundaryCondition::Impl::Impl(const QDomElement& node, const SolverDefinitionTranslator& translator)
{
	load(node, translator);
}

void SolverDefinitionBoundaryCondition::Impl::load(const QDomElement& node, const SolverDefinitionTranslator& translator)
{
	m_name = iRIC::toStr(node.attribute("name", "default"));
	m_englishCaption = iRIC::toStr(node.attribute("caption"));
	m_caption = translator.translate(node.attribute("caption")), QObject::tr("Default");

	m_element = node;

	QString pos = node.attribute("position").toLower();
	if (pos == "node") {
		m_position = pNode;
	} else if (pos == "cell") {
		m_position = pCell;
	} else if (pos == "edge") {
		m_position = pEdge;
	}
}

SolverDefinitionBoundaryCondition::SolverDefinitionBoundaryCondition(QDomElement node, const SolverDefinitionTranslator& translator) :
	SolverDefinitionNode {node, translator},
	m_impl {new Impl {node, translator}}
{}

SolverDefinitionBoundaryCondition::~SolverDefinitionBoundaryCondition()
{
	delete m_impl;
}

const std::string& SolverDefinitionBoundaryCondition::name() const
{
	return m_impl->m_name;
}

const QString& SolverDefinitionBoundaryCondition::caption() const
{
	return m_impl->m_caption;
}

const std::string& SolverDefinitionBoundaryCondition::englishCaption() const
{
	return m_impl->m_englishCaption;
}

void SolverDefinitionBoundaryCondition::setCaption(const QString& caption)
{
	m_impl->m_caption = caption;
}

const QDomElement& SolverDefinitionBoundaryCondition::element() const
{
	return m_impl->m_element;
}

SolverDefinitionBoundaryCondition::Position SolverDefinitionBoundaryCondition::position() const
{
	return m_impl->m_position;
}

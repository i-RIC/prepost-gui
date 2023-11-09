#include "solverdefinitionboundarycondition.h"
#include "solverdefinitiontranslator.h"
#include "solverdefinition.h"
#include "private/solverdefinitionboundarycondition_impl.h"

#include <misc/stringtool.h>

SolverDefinitionBoundaryCondition::Impl::Impl(const QDomElement& node, SolverDefinition* solverDef) :
	m_definition {solverDef}
{
	load(node, solverDef->buildTranslator());
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

SolverDefinitionBoundaryCondition::SolverDefinitionBoundaryCondition(QDomElement node, SolverDefinition* solverDef) :
	SolverDefinitionNode {node, solverDef},
	impl {new Impl {node, solverDef}}
{}

SolverDefinitionBoundaryCondition::~SolverDefinitionBoundaryCondition()
{}

const std::string& SolverDefinitionBoundaryCondition::name() const
{
	return impl->m_name;
}

const QString& SolverDefinitionBoundaryCondition::caption() const
{
	return impl->m_caption;
}

const std::string& SolverDefinitionBoundaryCondition::englishCaption() const
{
	return impl->m_englishCaption;
}

void SolverDefinitionBoundaryCondition::setCaption(const QString& caption)
{
	impl->m_caption = caption;
}

const QDomElement& SolverDefinitionBoundaryCondition::element() const
{
	return impl->m_element;
}

SolverDefinitionBoundaryCondition::Position SolverDefinitionBoundaryCondition::position() const
{
	return impl->m_position;
}

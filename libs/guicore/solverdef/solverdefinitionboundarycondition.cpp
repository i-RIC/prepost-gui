#include "solverdefinitionboundarycondition.h"

/*
PreProcessorBCDataItem* SolverDefinitionBoundaryCondition::createConditionDataItem(SolverDefinition* def, PreProcessorDataItem* parent)
{
	return new PreProcessorBCDataItem(def, this, parent);
}
*/

void SolverDefinitionBoundaryCondition::load(const QDomElement& node, const SolverDefinitionTranslator& translator)
{
	m_name = node.attribute("name", "default");
	m_englishCaption = node.attribute("caption");
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

#include "solverdefinitionnode.h"

SolverDefinitionNode::SolverDefinitionNode() :
	m_solverDefinition {nullptr}
{}

SolverDefinitionNode::SolverDefinitionNode(const QDomElement&, SolverDefinition* solverDef) :
	m_solverDefinition {solverDef}
{}

SolverDefinitionNode::~SolverDefinitionNode()
{}

SolverDefinition* SolverDefinitionNode::solverDefinition() const
{
	return m_solverDefinition;
}

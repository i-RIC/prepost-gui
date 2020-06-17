#ifndef SOLVERDEFINITIONNODE_H
#define SOLVERDEFINITIONNODE_H

#include "../guicore_global.h"

class SolverDefinition;
class QDomElement;

/// This class represents an element node inside solver definition file,
/// like calculation condition node grid condition node etc.
class GUICOREDLL_EXPORT SolverDefinitionNode
{
public:
	SolverDefinitionNode();
	SolverDefinitionNode(const QDomElement& elem, SolverDefinition* solverDef);
	virtual ~SolverDefinitionNode();

	SolverDefinition* solverDefinition() const;

private:
	SolverDefinition* m_solverDefinition;
};

#endif // SOLVERDEFINITIONNODE_H

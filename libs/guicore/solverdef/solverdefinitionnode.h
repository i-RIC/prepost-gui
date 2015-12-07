#ifndef SOLVERDEFINITIONNODE_H
#define SOLVERDEFINITIONNODE_H

#include "../guicore_global.h"

class SolverDefinitionTranslator;
class QDomElement;

/// This class represents an element node inside solver definition file,
/// like calculation condition node grid condition node etc.
class GUICOREDLL_EXPORT SolverDefinitionNode
{
public:
	SolverDefinitionNode();
	SolverDefinitionNode(const QDomElement& elem, const SolverDefinitionTranslator& translator);
	virtual ~SolverDefinitionNode();
};

#endif // SOLVERDEFINITIONNODE_H

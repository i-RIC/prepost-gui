#ifndef SOLVERDEFINITIONNODE_H
#define SOLVERDEFINITIONNODE_H

#include "../guicore_global.h"
#include "solverdefinitiontranslator.h"

#include <QDomElement>

/// This class represents an element node inside solver definition file,
/// like calculation condition node grid condition node etc.
class GUICOREDLL_EXPORT SolverDefinitionNode
{

public:
	/// constructor
	SolverDefinitionNode() {}
	/// constructor
	SolverDefinitionNode(QDomElement /*node*/, const SolverDefinitionTranslator& /*translator*/) {}
};

#endif // SOLVERDEFINITIONABSTRACTNODE_H

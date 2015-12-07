#ifndef SOLVERDEFINITIONGRIDATTRIBUTEDIMENSIONCREATOR_H
#define SOLVERDEFINITIONGRIDATTRIBUTEDIMENSIONCREATOR_H

#include "../guicore_global.h"

class QDomElement;
class SolverDefinitionTranslator;
class SolverDefinitionGridAttribute;
class SolverDefinitionGridAttributeDimension;

class GUICOREDLL_EXPORT SolverDefinitionGridAttributeDimensionCreator
{
public:
	static SolverDefinitionGridAttributeDimension* create(const QDomElement& elem, const SolverDefinitionTranslator& translator, SolverDefinitionGridAttribute* cond);

private:
	SolverDefinitionGridAttributeDimensionCreator();
	~SolverDefinitionGridAttributeDimensionCreator();
};

#endif // SOLVERDEFINITIONGRIDATTRIBUTEDIMENSIONCREATOR_H

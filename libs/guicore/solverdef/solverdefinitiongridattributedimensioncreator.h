#ifndef SOLVERDEFINITIONGRIDRELATEDCONDITIONDIMENSIONCREATOR_H
#define SOLVERDEFINITIONGRIDRELATEDCONDITIONDIMENSIONCREATOR_H

#include "../guicore_global.h"

class QDomElement;
class SolverDefinitionTranslator;
class SolverDefinitionGridAttribute;
class SolverDefinitionGridAttributeDimension;

class GUICOREDLL_EXPORT SolverDefinitionGridAttributeDimensionCreator
{
public:
	~SolverDefinitionGridAttributeDimensionCreator();
	static SolverDefinitionGridAttributeDimension* create(QDomElement elem, const SolverDefinitionTranslator& translator, SolverDefinitionGridAttribute* cond);

private:
	SolverDefinitionGridAttributeDimensionCreator();
};

#endif // SOLVERDEFINITIONGRIDRELATEDCONDITIONDIMENSIONCREATOR_H

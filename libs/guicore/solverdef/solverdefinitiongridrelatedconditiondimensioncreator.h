#ifndef SOLVERDEFINITIONGRIDRELATEDCONDITIONDIMENSIONCREATOR_H
#define SOLVERDEFINITIONGRIDRELATEDCONDITIONDIMENSIONCREATOR_H

#include "../guicore_global.h"

class QDomElement;
class SolverDefinitionTranslator;
class SolverDefinitionGridRelatedCondition;
class SolverDefinitionGridRelatedConditionDimension;

class GUICOREDLL_EXPORT SolverDefinitionGridRelatedConditionDimensionCreator
{
public:
	~SolverDefinitionGridRelatedConditionDimensionCreator();
	static SolverDefinitionGridRelatedConditionDimension* create(QDomElement elem, const SolverDefinitionTranslator& translator, SolverDefinitionGridRelatedCondition* cond);

private:
	SolverDefinitionGridRelatedConditionDimensionCreator();
};

#endif // SOLVERDEFINITIONGRIDRELATEDCONDITIONDIMENSIONCREATOR_H

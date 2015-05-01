#include "solverdefinitiongridrelatedconditiondimensioncreator.h"
#include "solverdefinitiongridrelatedconditiondimensiont.h"

SolverDefinitionGridRelatedConditionDimensionCreator::SolverDefinitionGridRelatedConditionDimensionCreator()
{

}

SolverDefinitionGridRelatedConditionDimensionCreator::~SolverDefinitionGridRelatedConditionDimensionCreator()
{

}

SolverDefinitionGridRelatedConditionDimension* SolverDefinitionGridRelatedConditionDimensionCreator::create(QDomElement elem, const SolverDefinitionTranslator& translator, SolverDefinitionGridRelatedCondition* cond)
{
	if (elem.attribute("valueType") == "integer") {
		return new SolverDefinitionGridRelatedConditionIntegerDimension(elem, translator, cond);
	} else if (elem.attribute("valueType") == "real") {
		return new SolverDefinitionGridRelatedConditionRealDimension(elem, translator, cond);
	}
	return 0;
}

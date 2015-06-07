#include "solverdefinitiongridattributedimensioncreator.h"
#include "solverdefinitiongridattributedimensiont.h"

SolverDefinitionGridAttributeDimensionCreator::SolverDefinitionGridAttributeDimensionCreator()
{

}

SolverDefinitionGridAttributeDimensionCreator::~SolverDefinitionGridAttributeDimensionCreator()
{

}

SolverDefinitionGridAttributeDimension* SolverDefinitionGridAttributeDimensionCreator::create(QDomElement elem, const SolverDefinitionTranslator& translator, SolverDefinitionGridAttribute* cond)
{
	if (elem.attribute("valueType") == "integer") {
		return new SolverDefinitionGridAttributeIntegerDimension(elem, translator, cond);
	} else if (elem.attribute("valueType") == "real") {
		return new SolverDefinitionGridAttributeRealDimension(elem, translator, cond);
	}
	return nullptr;
}

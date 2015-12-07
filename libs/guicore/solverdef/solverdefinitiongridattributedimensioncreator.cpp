#include "solverdefinitiongridattributedimensioncreator.h"
#include "solverdefinitiongridattributedimensiont.h"

#include <QDomElement>

SolverDefinitionGridAttributeDimension* SolverDefinitionGridAttributeDimensionCreator::create(const QDomElement& elem, const SolverDefinitionTranslator& translator, SolverDefinitionGridAttribute* cond)
{
	QString valueType = elem.attribute("valueType");
	if (valueType == "integer") {
		return new SolverDefinitionGridAttributeIntegerDimension(elem, translator, cond);
	} else if (valueType == "real") {
		return new SolverDefinitionGridAttributeRealDimension(elem, translator, cond);
	}
	return nullptr;
}

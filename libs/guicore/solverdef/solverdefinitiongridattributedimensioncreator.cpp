#include "solverdefinitiongridattributedimensioncreator.h"
#include "solverdefinitiongridattributeintegerdimension.h"
#include "solverdefinitiongridattributerealdimension.h"

#include <QDomElement>

SolverDefinitionGridAttributeDimension* SolverDefinitionGridAttributeDimensionCreator::create(const QDomElement& elem, SolverDefinition* solverDef, SolverDefinitionGridAttribute* cond)
{
	QString valueType = elem.attribute("valueType");
	if (valueType == "integer") {
		return new SolverDefinitionGridAttributeIntegerDimension(elem, solverDef, cond);
	} else if (valueType == "real") {
		return new SolverDefinitionGridAttributeRealDimension(elem, solverDef, cond);
	}
	return nullptr;
}

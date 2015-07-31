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
	/// Create dimension instance
	static SolverDefinitionGridAttributeDimension* create(const QDomElement& elem, const SolverDefinitionTranslator& translator, SolverDefinitionGridAttribute* cond);

private:
	/// Constructor
	SolverDefinitionGridAttributeDimensionCreator();
	/// Destructor
	~SolverDefinitionGridAttributeDimensionCreator();
};

#endif // SOLVERDEFINITIONGRIDRELATEDCONDITIONDIMENSIONCREATOR_H

#ifndef SOLVERDEFINITIONGRIDATTRIBUTEREALDIMENSION_H
#define SOLVERDEFINITIONGRIDATTRIBUTEREALDIMENSION_H

#include "../guicore_global.h"
#include "solverdefinitiongridattributedimensiont.h"

class GUICOREDLL_EXPORT SolverDefinitionGridAttributeRealDimension :
		public SolverDefinitionGridAttributeDimensionT<double>
{
public:
	SolverDefinitionGridAttributeRealDimension(const QDomElement& elem, const SolverDefinitionTranslator& translator, SolverDefinitionGridAttribute* cond);

	double fromVariant(const QVariant& v) const override;
	GridAttributeDimensionContainer* buildContainer() override;
};

#endif // SOLVERDEFINITIONGRIDATTRIBUTEREALDIMENSION_H

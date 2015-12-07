#ifndef SOLVERDEFINITIONGRIDATTRIBUTEINTEGERDIMENSION_H
#define SOLVERDEFINITIONGRIDATTRIBUTEINTEGERDIMENSION_H

#include "../guicore_global.h"
#include "solverdefinitiongridattributedimensiont.h"

class GUICOREDLL_EXPORT SolverDefinitionGridAttributeIntegerDimension :
		public SolverDefinitionGridAttributeDimensionT<int>
{
public:
	SolverDefinitionGridAttributeIntegerDimension(const QDomElement& elem, const SolverDefinitionTranslator& translator, SolverDefinitionGridAttribute* cond);

	int fromVariant(const QVariant& v) const override;
	GridAttributeDimensionContainer* buildContainer() override;
};

#endif // SOLVERDEFINITIONGRIDATTRIBUTEINTEGERDIMENSION_H

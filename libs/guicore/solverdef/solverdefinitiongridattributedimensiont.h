#ifndef SOLVERDEFINITIONGRIDATTRIBUTEDIMENSIONT_H
#define SOLVERDEFINITIONGRIDATTRIBUTEDIMENSIONT_H

#include "solverdefinitiongridattributedimension.h"
#include "../pre/gridcond/base/gridattributedimensioncontainert.h"

template <class V>
class SolverDefinitionGridAttributeDimensionT : public SolverDefinitionGridAttributeDimension
{

public:
	SolverDefinitionGridAttributeDimensionT(const QDomElement& elem, const SolverDefinitionTranslator& translator, SolverDefinitionGridAttribute* cond) :
		SolverDefinitionGridAttributeDimension {elem, translator, cond}
	{}
	virtual V fromVariant(const QVariant& v) const = 0;
};

class GUICOREDLL_EXPORT SolverDefinitionGridAttributeIntegerDimension :
		public SolverDefinitionGridAttributeDimensionT<int>
{

public:
	SolverDefinitionGridAttributeIntegerDimension(const QDomElement& elem, const SolverDefinitionTranslator& translator, SolverDefinitionGridAttribute* cond) :
		SolverDefinitionGridAttributeDimensionT<int> (elem, translator, cond)
	{}
	int fromVariant(const QVariant& v) const override {
		return v.toInt();
	}
	GridAttributeDimensionContainer* buildContainer() override {
		return new GridAttributeDimensionIntegerContainer(this);
	}
};

class GUICOREDLL_EXPORT SolverDefinitionGridAttributeRealDimension :
		public SolverDefinitionGridAttributeDimensionT<double>
{

public:
	SolverDefinitionGridAttributeRealDimension(const QDomElement& elem, const SolverDefinitionTranslator& translator, SolverDefinitionGridAttribute* cond) :
		SolverDefinitionGridAttributeDimensionT<double> (elem, translator, cond)
	{}
	double fromVariant(const QVariant& v) const override {
		return v.toDouble();
	}
	GridAttributeDimensionContainer* buildContainer() override {
		return new GridAttributeDimensionRealContainer(this);
	}
};

#endif // SOLVERDEFINITIONGRIDATTRIBUTEDIMENSIONT_H

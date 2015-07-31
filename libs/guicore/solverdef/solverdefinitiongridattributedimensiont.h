#ifndef SOLVERDEFINITIONGRIDRELATEDCONDITIONDIMENSIONT_H
#define SOLVERDEFINITIONGRIDRELATEDCONDITIONDIMENSIONT_H

#include "solverdefinitiongridattributedimension.h"
#include "../pre/gridcond/base/gridattributedimensioncontainert.h"

template <class V>
class SolverDefinitionGridAttributeDimensionT : public SolverDefinitionGridAttributeDimension
{

public:
	SolverDefinitionGridAttributeDimensionT(QDomElement node, const SolverDefinitionTranslator& translator, SolverDefinitionGridAttribute* cond) :
		SolverDefinitionGridAttributeDimension {node, translator, cond}
	{}
	virtual V fromVariant(const QVariant& v) const = 0;
};

class GUICOREDLL_EXPORT SolverDefinitionGridAttributeIntegerDimension :
		public SolverDefinitionGridAttributeDimensionT<int>
{

public:
	SolverDefinitionGridAttributeIntegerDimension(QDomElement node, const SolverDefinitionTranslator& translator, SolverDefinitionGridAttribute* cond) :
		SolverDefinitionGridAttributeDimensionT<int> {node, translator, cond}
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
	SolverDefinitionGridAttributeRealDimension(QDomElement node, const SolverDefinitionTranslator& translator, SolverDefinitionGridAttribute* cond) :
		SolverDefinitionGridAttributeDimensionT<double> {node, translator, cond}
	{}
	double fromVariant(const QVariant& v) const override {
		return v.toDouble();
	}
	GridAttributeDimensionContainer* buildContainer() override {
		return new GridAttributeDimensionRealContainer(this);
	}
};

#endif // SOLVERDEFINITIONGRIDRELATEDCONDITIONDIMENSIONT_H

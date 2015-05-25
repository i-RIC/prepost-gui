#ifndef SOLVERDEFINITIONGRIDRELATEDCONDITIONDIMENSIONT_H
#define SOLVERDEFINITIONGRIDRELATEDCONDITIONDIMENSIONT_H

#include "../guicore_global.h"
#include "solverdefinitiongridrelatedconditiondimension.h"
#include "../pre/gridcond/base/gridrelatedconditiondimensioncontainert.h"

template <class V>
class SolverDefinitionGridRelatedConditionDimensionT : public SolverDefinitionGridRelatedConditionDimension
{
public:
	SolverDefinitionGridRelatedConditionDimensionT(QDomElement node, const SolverDefinitionTranslator& translator, SolverDefinitionGridRelatedCondition* cond)
		: SolverDefinitionGridRelatedConditionDimension(node, translator, cond)
	{}
	virtual V fromVariant(const QVariant& v) const = 0;
};

class GUICOREDLL_EXPORT SolverDefinitionGridRelatedConditionIntegerDimension
	: public SolverDefinitionGridRelatedConditionDimensionT<int>
{
public:
	SolverDefinitionGridRelatedConditionIntegerDimension(QDomElement node, const SolverDefinitionTranslator& translator, SolverDefinitionGridRelatedCondition* cond)
		: SolverDefinitionGridRelatedConditionDimensionT<int>(node, translator, cond)
	{}
	int fromVariant(const QVariant& v) const override {
		return v.toInt();
	}
	GridRelatedConditionDimensionContainer* buildContainer() override {
		return new GridRelatedConditionDimensionIntegerContainer(this);
	}
};

class GUICOREDLL_EXPORT SolverDefinitionGridRelatedConditionRealDimension
	: public SolverDefinitionGridRelatedConditionDimensionT<double>
{
public:
	SolverDefinitionGridRelatedConditionRealDimension(QDomElement node, const SolverDefinitionTranslator& translator, SolverDefinitionGridRelatedCondition* cond)
		: SolverDefinitionGridRelatedConditionDimensionT<double>(node, translator, cond)
	{}
	double fromVariant(const QVariant& v) const override {
		return v.toDouble();
	}
	GridRelatedConditionDimensionContainer* buildContainer() override {
		return new GridRelatedConditionDimensionRealContainer(this);
	}
};

#endif // SOLVERDEFINITIONGRIDRELATEDCONDITIONDIMENSIONT_H

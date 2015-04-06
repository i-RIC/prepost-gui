#ifndef SOLVERDEFINITIONGRIDRELATEDCONDITIONT_H
#define SOLVERDEFINITIONGRIDRELATEDCONDITIONT_H

#include "../guicore_global.h"
#include "solverdefinitiongridrelatedcondition.h"
#include <QVariant>

template <class V>
class SolverDefinitionGridRelatedConditionT : public SolverDefinitionGridRelatedCondition
{
public:
	SolverDefinitionGridRelatedConditionT(QDomElement node, const SolverDefinitionTranslator& translator)
		: SolverDefinitionGridRelatedCondition(node, translator)
	{}
	virtual V fromVariant(const QVariant& v) const = 0;
	V defaultValue() const {
		return fromVariant(variantDefaultValue());
	}
};

class GUICOREDLL_EXPORT SolverDefinitionGridRelatedIntegerCondition : public SolverDefinitionGridRelatedConditionT<int>
{
public:
	SolverDefinitionGridRelatedIntegerCondition(QDomElement node, const SolverDefinitionTranslator& translator)
		: SolverDefinitionGridRelatedConditionT<int>(node, translator)
	{}
	int fromVariant(const QVariant& v) const {
		return v.toInt();
	}
	RawData* buildBackgroundRawData(ProjectDataItem* parent);
};

class GUICOREDLL_EXPORT SolverDefinitionGridRelatedRealCondition : public SolverDefinitionGridRelatedConditionT<double>
{
public:
	SolverDefinitionGridRelatedRealCondition(QDomElement node, const SolverDefinitionTranslator& translator)
		: SolverDefinitionGridRelatedConditionT<double>(node, translator)
	{}
	double fromVariant(const QVariant& v) const {
		return v.toDouble();
	}
	RawData* buildBackgroundRawData(ProjectDataItem* parent);
};

#endif // SOLVERDEFINITIONGRIDRELATEDCONDITIONT_H

#ifndef SOLVERDEFINITIONGRIDRELATEDCONDITIONT_H
#define SOLVERDEFINITIONGRIDRELATEDCONDITIONT_H

#include "../guicore_global.h"
#include "solverdefinitiongridattribute.h"
#include <QVariant>

template <class V>
class SolverDefinitionGridAttributeT : public SolverDefinitionGridAttribute
{
public:
	SolverDefinitionGridAttributeT(QDomElement node, const SolverDefinitionTranslator& translator)
		: SolverDefinitionGridAttribute(node, translator)
	{}
	virtual V fromVariant(const QVariant& v) const = 0;
	V defaultValue() const {
		return fromVariant(variantDefaultValue());
	}
};

class GUICOREDLL_EXPORT SolverDefinitionGridAttributeInteger : public SolverDefinitionGridAttributeT<int>
{
public:
	SolverDefinitionGridAttributeInteger(QDomElement node, const SolverDefinitionTranslator& translator)
		: SolverDefinitionGridAttributeT<int>(node, translator)
	{}
	int fromVariant(const QVariant& v) const override {
		return v.toInt();
	}
	RawData* buildBackgroundRawData(ProjectDataItem* parent) override;
};

class GUICOREDLL_EXPORT SolverDefinitionGridAttributeReal : public SolverDefinitionGridAttributeT<double>
{
public:
	SolverDefinitionGridAttributeReal(QDomElement node, const SolverDefinitionTranslator& translator)
		: SolverDefinitionGridAttributeT<double>(node, translator)
	{}
	double fromVariant(const QVariant& v) const override {
		return v.toDouble();
	}
	RawData* buildBackgroundRawData(ProjectDataItem* parent) override;
};

#endif // SOLVERDEFINITIONGRIDRELATEDCONDITIONT_H

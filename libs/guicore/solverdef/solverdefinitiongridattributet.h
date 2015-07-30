#ifndef SOLVERDEFINITIONGRIDRELATEDCONDITIONT_H
#define SOLVERDEFINITIONGRIDRELATEDCONDITIONT_H

#include "../guicore_global.h"
#include "solverdefinitiongridattribute.h"
#include <QVariant>

template <class V>
class SolverDefinitionGridAttributeT : public SolverDefinitionGridAttribute
{

public:
	SolverDefinitionGridAttributeT(QDomElement node, const SolverDefinitionTranslator& translator, Position pos, bool isOption) :
		SolverDefinitionGridAttribute {node, translator, pos, isOption}
	{}
	virtual V fromVariant(const QVariant& v) const = 0;
	V defaultValue() const {
		return fromVariant(variantDefaultValue());
	}
};

class GUICOREDLL_EXPORT SolverDefinitionGridAttributeInteger : public SolverDefinitionGridAttributeT<int>
{

public:
	SolverDefinitionGridAttributeInteger(QDomElement node, const SolverDefinitionTranslator& translator, Position pos, bool isOption) :
		SolverDefinitionGridAttributeT<int> {node, translator, pos, isOption}
	{}
	int fromVariant(const QVariant& v) const override {
		return v.toInt();
	}
	GeoData* buildBackgroundGeoData(ProjectDataItem* parent) override;
};

class GUICOREDLL_EXPORT SolverDefinitionGridAttributeReal : public SolverDefinitionGridAttributeT<double>
{

public:
	SolverDefinitionGridAttributeReal(QDomElement node, const SolverDefinitionTranslator& translator, Position pos, bool isOption) :
		SolverDefinitionGridAttributeT<double> {node, translator, pos, isOption}
	{}
	double fromVariant(const QVariant& v) const override {
		return v.toDouble();
	}
	GeoData* buildBackgroundGeoData(ProjectDataItem* parent) override;
};

#endif // SOLVERDEFINITIONGRIDRELATEDCONDITIONT_H

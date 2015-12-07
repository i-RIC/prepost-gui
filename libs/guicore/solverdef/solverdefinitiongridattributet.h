#ifndef SOLVERDEFINITIONGRIDATTRIBUTET_H
#define SOLVERDEFINITIONGRIDATTRIBUTET_H

#include "../guicore_global.h"
#include "solverdefinitiongridattribute.h"
#include <QVariant>

template <class V>
class SolverDefinitionGridAttributeT : public SolverDefinitionGridAttribute
{
public:
	SolverDefinitionGridAttributeT(const QDomElement& node, const SolverDefinitionTranslator& translator, Position pos, bool isOption, int order) :
		SolverDefinitionGridAttribute {node, translator, pos, isOption, order}
	{}
	virtual V fromVariant(const QVariant& v) const = 0;
	V defaultValue() const {
		return fromVariant(variantDefaultValue());
	}
};

class GUICOREDLL_EXPORT SolverDefinitionGridAttributeInteger : public SolverDefinitionGridAttributeT<int>
{
public:
	SolverDefinitionGridAttributeInteger(const QDomElement& elem, const SolverDefinitionTranslator& translator, Position pos, bool isOption, int order) :
		SolverDefinitionGridAttributeT<int> (elem, translator, pos, isOption, order)
	{}
	int fromVariant(const QVariant& v) const override {
		return v.toInt();
	}
	GeoData* buildBackgroundGeoData(ProjectDataItem* parent) override;
};

class GUICOREDLL_EXPORT SolverDefinitionGridAttributeReal : public SolverDefinitionGridAttributeT<double>
{
public:
	SolverDefinitionGridAttributeReal(const QDomElement& elem, const SolverDefinitionTranslator& translator, Position pos, bool isOption, int order) :
		SolverDefinitionGridAttributeT<double> (elem, translator, pos, isOption, order)
	{}
	double fromVariant(const QVariant& v) const override {
		return v.toDouble();
	}
	GeoData* buildBackgroundGeoData(ProjectDataItem* parent) override;
};

#endif // SOLVERDEFINITIONGRIDATTRIBUTET_H

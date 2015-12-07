#ifndef SOLVERDEFINITIONGRIDATTRIBUTEREAL_H
#define SOLVERDEFINITIONGRIDATTRIBUTEREAL_H

#include "../guicore_global.h"

#include "solverdefinitiongridattributet.h"

class GUICOREDLL_EXPORT SolverDefinitionGridAttributeReal : public SolverDefinitionGridAttributeT<double>
{
public:
	SolverDefinitionGridAttributeReal(const QDomElement& elem, const SolverDefinitionTranslator& translator, Position pos, bool isOption, int order);

	double fromVariant(const QVariant& v) const override;
	GeoData* buildBackgroundGeoData(ProjectDataItem* parent) override;
};

#endif // SOLVERDEFINITIONGRIDATTRIBUTEREAL_H

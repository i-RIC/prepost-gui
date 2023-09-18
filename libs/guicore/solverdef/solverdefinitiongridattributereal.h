#ifndef SOLVERDEFINITIONGRIDATTRIBUTEREAL_H
#define SOLVERDEFINITIONGRIDATTRIBUTEREAL_H

#include "../guicore_global.h"

#include "solverdefinitiongridattributet.h"

class GUICOREDLL_EXPORT SolverDefinitionGridAttributeReal : public SolverDefinitionGridAttributeT<double>
{
public:
	SolverDefinitionGridAttributeReal(const QDomElement& elem, SolverDefinition* solverDef, Position pos, bool isOption, int order);

	GridAttributeEditWidget* editWidget(QWidget* parent) override;
	GridAttributeVariationEditWidget* variationEditWidget(QWidget* parent) override;
	GridAttributeContainer* buildContainer(v4InputGrid* grid) override;
	GridAttributeStringConverter* stringConverter() const override;
	double fromVariant(const QVariant& v) const override;
	GeoData* buildBackgroundGeoData(ProjectDataItem* parent) override;
};

#endif // SOLVERDEFINITIONGRIDATTRIBUTEREAL_H

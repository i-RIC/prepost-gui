#ifndef SOLVERDEFINITIONGRIDATTRIBUTESTRING_H
#define SOLVERDEFINITIONGRIDATTRIBUTESTRING_H

#include "../guicore_global.h"

#include "solverdefinitiongridattributet.h"

#include <string>

class SolverDefinitionGridAttributeString : public SolverDefinitionGridAttributeT<std::string> {
public:
	SolverDefinitionGridAttributeString();
	SolverDefinitionGridAttributeString(const QDomElement& elem, SolverDefinition* solverDef, Position pos, bool isOption, int order);

	GridAttributeEditWidget* editWidget(QWidget* parent) override;
	GridAttributeVariationEditWidget* variationEditWidget(QWidget* parent) override;
	GridAttributeContainer* buildContainer(v4InputGrid* grid) override;
	GridAttributeStringConverter* stringConverter() const override;
	std::string fromVariant(const QVariant& v) const override;
	GeoData* buildBackgroundGeoData(ProjectDataItem* parent) override;
};

#endif // SOLVERDEFINITIONGRIDATTRIBUTESTRING_H

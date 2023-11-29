#ifndef SOLVERDEFINITIONGRIDATTRIBUTEINTEGER_H
#define SOLVERDEFINITIONGRIDATTRIBUTEINTEGER_H

#include "../guicore_global.h"

#include "solverdefinitiongridattributet.h"

class GUICOREDLL_EXPORT SolverDefinitionGridAttributeInteger : public SolverDefinitionGridAttributeT<int>
{
public:
	SolverDefinitionGridAttributeInteger();
	SolverDefinitionGridAttributeInteger(const QDomElement& elem, SolverDefinition* solverDef, Position pos, bool isOption, int order);

	GridAttributeStringConverter* stringConverter() const override;
	int fromVariant(const QVariant& v) const override;
	GeoData* buildBackgroundGeoData(ProjectDataItem* parent) override;
};

#endif // SOLVERDEFINITIONGRIDATTRIBUTEINTEGER_H

#ifndef GEODATAPOINTMAP_POLYGONSMANAGER_ATTRIBUTEFORMAPPINGCONDITION_H
#define GEODATAPOINTMAP_POLYGONSMANAGER_ATTRIBUTEFORMAPPINGCONDITION_H

#include "geodatapointmap_polygonsmanager.h"

#include <guicore/solverdef/solverdefinitiongridattribute.h>

class GeoDataPointmap::PolygonsManager::AttributeForMappingCondition : public SolverDefinitionGridAttribute
{
public:
	AttributeForMappingCondition();

	GridAttributeStringConverter* stringConverter() const override;
	GridAttributeEditWidget* editWidget(QWidget* parent) override;
	GridAttributeVariationEditWidget* variationEditWidget(QWidget* parent) override;
	GeoData* buildBackgroundGeoData(ProjectDataItem* parent) override;

private:
	GridAttributeContainer* buildContainer(Grid* grid) override;

	class StringConverter;
	class EditWidget;
};

#endif // GEODATAPOINTMAP_POLYGONSMANAGER_ATTRIBUTEFORMAPPINGCONDITION_H

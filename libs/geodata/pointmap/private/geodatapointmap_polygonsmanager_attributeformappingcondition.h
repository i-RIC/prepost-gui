#ifndef GEODATAPOINTMAP_POLYGONSMANAGER_ATTRIBUTEFORMAPPINGCONDITION_H
#define GEODATAPOINTMAP_POLYGONSMANAGER_ATTRIBUTEFORMAPPINGCONDITION_H

#include "geodatapointmap_polygonsmanager.h"

#include <guicore/solverdef/solverdefinitiongridattributeinteger.h>

class GeoDataPointmap::PolygonsManager::AttributeForMappingCondition : public SolverDefinitionGridAttributeInteger
{
public:
	AttributeForMappingCondition();

	GridAttributeStringConverter* stringConverter() const override;
	GridAttributeEditWidget* editWidget(QWidget* parent) override;
	GridAttributeVariationEditWidget* variationEditWidget(QWidget* parent) override;
	QVariant colorMapValue(const QVariant& v) const override;
	GeoData* buildBackgroundGeoData(ProjectDataItem* parent) override;

private:
	GridAttributeContainer* buildContainer(Grid* grid) override;
	void setupColorMapSettingContainer(ColorMapSettingContainerI* c) const override;

	class EditWidget;
	class StringConverter;
};

#endif // GEODATAPOINTMAP_POLYGONSMANAGER_ATTRIBUTEFORMAPPINGCONDITION_H

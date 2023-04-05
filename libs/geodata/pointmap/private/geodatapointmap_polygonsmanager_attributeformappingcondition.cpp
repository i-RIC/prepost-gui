#include "geodatapointmap_polygonsmanager_attributeformappingcondition.h"
#include "geodatapointmap_polygonsmanager_attributeformappingcondition_editwidget.h"
#include "geodatapointmap_polygonsmanager_attributeformappingcondition_stringconverter.h"

GeoDataPointmap::PolygonsManager::AttributeForMappingCondition::AttributeForMappingCondition() :
	SolverDefinitionGridAttribute {}
{}

GridAttributeStringConverter* GeoDataPointmap::PolygonsManager::AttributeForMappingCondition::stringConverter() const
{
	return new StringConverter();
}

GridAttributeEditWidget* GeoDataPointmap::PolygonsManager::AttributeForMappingCondition::editWidget(QWidget* parent)
{
	return new EditWidget(parent, this);
}

GridAttributeVariationEditWidget* GeoDataPointmap::PolygonsManager::AttributeForMappingCondition::variationEditWidget(QWidget* /*parent*/)
{
	return nullptr;
}

GeoData* GeoDataPointmap::PolygonsManager::AttributeForMappingCondition::buildBackgroundGeoData(ProjectDataItem* /*parent*/)
{
	return nullptr;
}

GridAttributeContainer* GeoDataPointmap::PolygonsManager::AttributeForMappingCondition::buildContainer(Grid* /*grid*/)
{
	return nullptr;
}

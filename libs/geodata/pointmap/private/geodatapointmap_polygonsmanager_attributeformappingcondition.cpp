#include "geodatapointmap_polygonsmanager_attributeformappingcondition.h"
#include "geodatapointmap_polygonsmanager_attributeformappingcondition_editwidget.h"
#include "geodatapointmap_polygonsmanager_attributeformappingcondition_stringconverter.h"
#include "geodatapointmap_polygonsmanager_mappingsetting.h"

#include <guicore/scalarstocolors/colormapenumeratefactory.h>
#include <guicore/scalarstocolors/colormapenumeratesettingcontainer.h>
#include <guicore/solverdef/integerenumloader.h>

GeoDataPointmap::PolygonsManager::AttributeForMappingCondition::AttributeForMappingCondition() :
	SolverDefinitionGridAttributeInteger {}
{
	setColorMapFactory(new ColorMapEnumerateFactory());
	setCaption(PolygonsManager::tr("Mapping Setting"));
}

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

QVariant GeoDataPointmap::PolygonsManager::AttributeForMappingCondition::colorMapValue(const QVariant& v) const
{
	MappingSetting setting;
	setting.fromVariant(v);
	return static_cast<int> (setting.type);
}

GeoData* GeoDataPointmap::PolygonsManager::AttributeForMappingCondition::buildBackgroundGeoData(ProjectDataItem* /*parent*/)
{
	return nullptr;
}

GridAttributeContainer* GeoDataPointmap::PolygonsManager::AttributeForMappingCondition::buildContainer(Grid* /*grid*/)
{
	return nullptr;
}

void GeoDataPointmap::PolygonsManager::AttributeForMappingCondition::setupColorMapSettingContainer(ColorMapSettingContainerI* c) const
{
	auto cm = dynamic_cast<ColorMapEnumerateSettingContainer*> (c);

	IntegerEnumLoader intEnum;
	intEnum.addEnumeration(static_cast<int> (MappingSetting::Type::TIN), "TIN", PolygonsManager::tr("TIN"));
	intEnum.addEnumeration(static_cast<int> (MappingSetting::Type::Maximum), "Maximum", PolygonsManager::tr("Maximum"));
	intEnum.addEnumeration(static_cast<int> (MappingSetting::Type::Minimum), "Minimum", PolygonsManager::tr("Minimum"));
	intEnum.addEnumeration(static_cast<int> (MappingSetting::Type::WeightedAverage), "Weighted Average", PolygonsManager::tr("Weighted Average"));

	cm->setup(&intEnum);
}

#include "geodatapointmap_mappingsetting.h"

GeoDataPointmap::MappingSetting::MappingSetting() :
	CompositeContainer({
										 &mappingMode,
										 &templateAutoMode, &templateStreamWiseLength, &templateCrossStreamLength,
										 &templateNumberOfExpansions, &templateWeightExponent}),
	mappingMode {"mappingMode", MappingMode::TIN},
	templateAutoMode {"templateAutoMode", true},
	templateStreamWiseLength {"templateStreamWiseLength", 4},
	templateCrossStreamLength {"templateCrossStreamLength", 1},
	templateNumberOfExpansions {"templateNumberOfExpansions", 3},
	templateWeightExponent {"templateWeightExponent", 1}
{}

GeoDataPointmap::MappingSetting::MappingSetting(const MappingSetting& setting) :
	MappingSetting {}
{
	copyValue(setting);
}

GeoDataPointmap::MappingSetting& GeoDataPointmap::MappingSetting::operator=(const MappingSetting& setting)
{
	copyValue(setting);
	return *this;
}

XmlAttributeContainer& GeoDataPointmap::MappingSetting::operator=(const XmlAttributeContainer& setting)
{
	return operator=(dynamic_cast<const MappingSetting&> (setting));
}

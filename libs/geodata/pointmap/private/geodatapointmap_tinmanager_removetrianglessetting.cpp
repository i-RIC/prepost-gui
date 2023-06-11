#include "geodatapointmap_tinmanager_removetrianglessetting.h"

GeoDataPointmap::TINManager::RemoveTrianglesSetting::RemoveTrianglesSetting() :
	CompositeContainer({&enabled, &thresholdLength}),
	enabled {"enabled", false},
	thresholdLength {"thresholdLength", 0}
{
	addPrefix("removeTriangles");
}

GeoDataPointmap::TINManager::RemoveTrianglesSetting::RemoveTrianglesSetting(const RemoveTrianglesSetting& s) :
	RemoveTrianglesSetting {}
{
	copyValue(s);
}

GeoDataPointmap::TINManager::RemoveTrianglesSetting& GeoDataPointmap::TINManager::RemoveTrianglesSetting::operator=(const RemoveTrianglesSetting& c)
{
	copyValue(c);
	return *this;
}

XmlAttributeContainer& GeoDataPointmap::TINManager::RemoveTrianglesSetting::operator=(const XmlAttributeContainer& c)
{
	return operator=(dynamic_cast<const RemoveTrianglesSetting&> (c));
}

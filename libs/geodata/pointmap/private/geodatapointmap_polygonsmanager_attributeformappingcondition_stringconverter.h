#ifndef GEODATAPOINTMAP_POLYGONSMANAGER_ATTRIBUTEFORMAPPINGCONDITION_STRINGCONVERTER_H
#define GEODATAPOINTMAP_POLYGONSMANAGER_ATTRIBUTEFORMAPPINGCONDITION_STRINGCONVERTER_H

#include <guicore/pre/gridcond/base/gridattributestringconverter.h>

#include "geodatapointmap_polygonsmanager_attributeformappingcondition.h"

class GeoDataPointmap::PolygonsManager::AttributeForMappingCondition::StringConverter : public GridAttributeStringConverter
{
public:
	StringConverter();

	QString convert(const QVariant& value) override;
};

#endif // GEODATAPOINTMAP_POLYGONSMANAGER_ATTRIBUTEFORMAPPINGCONDITION_STRINGCONVERTER_H

#ifndef GEODATAPOINTMAP_MAPPINGSETTING_H
#define GEODATAPOINTMAP_MAPPINGSETTING_H

#include "../geodatapointmap.h"

#include <misc/boolcontainer.h>
#include <misc/compositecontainer.h>
#include <misc/doublecontainer.h>
#include <misc/enumcontainert.h>
#include <misc/intcontainer.h>

class GeoDataPointmap::MappingSetting : public CompositeContainer
{
public:
	enum class MappingMode {
		TIN,
		Template,
		Polygons,
	};

	MappingSetting();
	MappingSetting(const MappingSetting& setting);

	MappingSetting& operator=(const MappingSetting& setting);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& setting);

	EnumContainerT<MappingMode> mappingMode;

	BoolContainer templateAutoMode;
	DoubleContainer templateStreamWiseLength;
	DoubleContainer templateCrossStreamLength;
	IntContainer templateNumberOfExpansions;
	DoubleContainer templateWeightExponent;
};

#endif // GEODATAPOINTMAP_MAPPINGSETTING_H

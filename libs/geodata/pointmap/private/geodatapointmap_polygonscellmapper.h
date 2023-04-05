#ifndef GEODATAPOINTMAP_POLYGONSCELLMAPPER_H
#define GEODATAPOINTMAP_POLYGONSCELLMAPPER_H

#include "../geodatapointmap.h"

#include <guicore/pre/geodata/geodatacellmappert.h>

class GeoDataPointmap::PolygonsCellMapper : public GeoDataCellMapperT<double, vtkDoubleArray>
{
public:
	PolygonsCellMapper(QObject* parent);

	GeoDataMapperSettingI* initialize(bool* boolMap) override;
	void map(bool* boolMap, GeoDataMapperSettingI* s) override;
	void terminate(GeoDataMapperSettingI* s) override;
};

#endif // GEODATAPOINTMAP_POLYGONSCELLMAPPER_H

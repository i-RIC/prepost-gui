#ifndef GEODATAPOINTMAP_POLYGONSNODEMAPPER_H
#define GEODATAPOINTMAP_POLYGONSNODEMAPPER_H

#include "../geodatapointmap.h"

#include <guicore/pre/geodata/geodatanodemappert.h>

class GeoDataPointmap::PolygonsNodeMapper : public GeoDataNodeMapperT<double, vtkDoubleArray>
{
public:
	PolygonsNodeMapper(QObject* parent);

	GeoDataMapperSettingI* initialize(bool* boolMap) override;
	void map(bool* boolMap, GeoDataMapperSettingI* s) override;
	void terminate(GeoDataMapperSettingI* s) override;
};

#endif // GEODATAPOINTMAP_POLYGONSNODEMAPPER_H

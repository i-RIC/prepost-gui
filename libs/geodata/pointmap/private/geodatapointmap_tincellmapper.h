#ifndef GEODATAPOINTMAP_TINCELLMAPPER_H
#define GEODATAPOINTMAP_TINCELLMAPPER_H

#include "../geodatapointmap.h"

#include <guicore/pre/geodata/geodatacellmappert.h>

class GeoDataPointmap::TinCellMapper : public GeoDataCellMapperT<double, vtkDoubleArray>
{
public:
	TinCellMapper(QObject* parent);

	GeoDataMapperSettingI* initialize(bool* boolMap) override;
	void map(bool* boolMap, GeoDataMapperSettingI* s) override;
	void terminate(GeoDataMapperSettingI* s) override;
};

#endif // GEODATAPOINTMAP_TINCELLMAPPER_H

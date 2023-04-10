#ifndef GEODATAPOINTMAP_TINNODEMAPPER_H
#define GEODATAPOINTMAP_TINNODEMAPPER_H

#include "../geodatapointmap.h"

#include <guicore/pre/geodata/geodatanodemappert.h>

class GeoDataPointmap::TinNodeMapper : public GeoDataNodeMapperT<double, vtkDoubleArray>
{
public:
	TinNodeMapper(QObject* parent);

	GeoDataMapperSettingI* initialize(bool* boolMap) override;
	void map(bool* boolMap, GeoDataMapperSettingI* s) override;
	void terminate(GeoDataMapperSettingI* s) override;
};

#endif // GEODATAPOINTMAP_TINNODEMAPPER_H

#ifndef GEODATAPOINTMAP_TEMPLATECELLMAPPER_H
#define GEODATAPOINTMAP_TEMPLATECELLMAPPER_H

#include "../geodatapointmap.h"

#include <guicore/pre/geodata/geodatacellmappert.h>

class GeoDataPointmap::TemplateCellMapper : public GeoDataCellMapperT<double, vtkDoubleArray>
{
public:
	TemplateCellMapper(QObject* parent);

	GeoDataMapperSettingI* initialize(bool* boolMap) override;
	void map(bool* boolMap, GeoDataMapperSettingI* s) override;
	void terminate(GeoDataMapperSettingI* s) override;
};

#endif // GEODATAPOINTMAP_TEMPLATECELLMAPPER_H

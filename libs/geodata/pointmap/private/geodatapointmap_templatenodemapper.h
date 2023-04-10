#ifndef GEODATAPOINTMAP_TEMPLATENODEMAPPER_H
#define GEODATAPOINTMAP_TEMPLATENODEMAPPER_H

#include "../geodatapointmap.h"

#include <guicore/pre/geodata/geodatanodemappert.h>

class GeoDataPointmap::TemplateNodeMapper : public GeoDataNodeMapperT<double, vtkDoubleArray>
{
public:
	TemplateNodeMapper(QObject* parent);

	GeoDataMapperSettingI* initialize(bool* boolMap) override;
	void map(bool* boolMap, GeoDataMapperSettingI* s) override;
	void terminate(GeoDataMapperSettingI* s) override;
};

#endif // GEODATAPOINTMAP_TEMPLATENODEMAPPER_H

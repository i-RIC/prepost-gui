#ifndef GEODATANETCDFCELLMAPPERT_H
#define GEODATANETCDFCELLMAPPERT_H

#include "geodatanetcdft.h"

#include <guicore/pre/geodata/geodatamappersettingi.h>
#include <guicore/pre/geodata/geodatacellmappert.h>
#include <misc/doublemappingsetting.h>

#include <QList>

class GeoDataNetcdfCellMapperSetting : public GeoDataMapperSettingI
{
public:
	GeoDataNetcdfCellMapperSetting() : GeoDataMapperSettingI() {}
	~GeoDataNetcdfCellMapperSetting() {}
	QList<DoubleMappingSetting> settings;
};

template <class V, class DA>
class GeoDataNetcdfCellMapperT : public GeoDataCellMapperT<V, DA>
{
public:
	GeoDataNetcdfCellMapperT(GeoDataCreator* parent);

	GeoDataMapperSettingI* initialize(bool* boolMap) override;
	void map(bool* boolMap, GeoDataMapperSettingI* s) override;
	void terminate(GeoDataMapperSettingI* s) override;
};

#include "private/geodatanetcdfcellmappert_detail.h"

#endif // GEODATANETCDFCELLMAPPERT_H

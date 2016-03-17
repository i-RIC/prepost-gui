#ifndef GEODATAPOINTMAPNODEMAPPERT_H
#define GEODATAPOINTMAPNODEMAPPERT_H

#include "geodatapointmapt.h"

#include <guicore/pre/geodata/geodatamappersettingi.h>
#include <guicore/pre/geodata/geodatanodemappert.h>
#include <misc/doublemappingsetting.h>

#include <QList>

class GeoDataPointmapNodeMapperSetting : public GeoDataMapperSettingI
{
public:
	GeoDataPointmapNodeMapperSetting() : GeoDataMapperSettingI() {}
	~GeoDataPointmapNodeMapperSetting() {}
	QList<DoubleMappingSetting> settings;
};

template <class V, class DA>
class GeoDataPointmapNodeMapperT : public GeoDataNodeMapperT<V, DA>
{
public:
	GeoDataPointmapNodeMapperT(GeoDataCreator* parent);

	GeoDataMapperSettingI* initialize(bool* boolMap) override;
	void map(bool* boolMap, GeoDataMapperSettingI* s) override;
	void terminate(GeoDataMapperSettingI* s) override;
};

#include "private/geodatapointmapnodemappert_detail.h"

#endif // GEODATAPOINTMAPNODEMAPPERT_H

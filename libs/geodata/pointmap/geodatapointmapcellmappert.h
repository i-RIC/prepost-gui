#ifndef GEODATAPOINTMAPCELLMAPPERT_H
#define GEODATAPOINTMAPCELLMAPPERT_H

#include "geodatapointmapt.h"

#include <guicore/pre/geodata/geodatacellmappert.h>
#include <misc/doublemappingsetting.h>

#include <QList>

#include <vtkCell.h>
#include <vtkSmartPointer.h>

class GeoDataPointmapCellMapperSetting : public GeoDataMapperSettingI
{
public:
	GeoDataPointmapCellMapperSetting() : GeoDataMapperSettingI() {}
	~GeoDataPointmapCellMapperSetting() {}
	QList<DoubleMappingSetting> settings;
};

template <class V, class DA>
class GeoDataPointmapCellMapperT : public GeoDataCellMapperT<V, DA>
{
public:
	GeoDataPointmapCellMapperT(GeoDataCreator* parent);

	GeoDataMapperSettingI* initialize(bool* boolMap) override;
	void map(bool* boolMap, GeoDataMapperSettingI* s) override;
	void terminate(GeoDataMapperSettingI* s) override;
};

#include "private/geodatapointmapcellmappert_detail.h"

#endif // GEODATAPOINTMAPCELLMAPPERT_H

#ifndef GEODATAPOINTGROUPCELLMAPPERT_DETAIL_H
#define GEODATAPOINTGROUPCELLMAPPERT_DETAIL_H

#include "../geodatapointgroupcellmappert.h"

#include <guibase/vtktool/vtkpointsutil.h>

template <class V, class DA>
GeoDataPointGroupCellMapperT<V, DA>::GeoDataPointGroupCellMapperT(GeoDataCreator* parent) :
	GeoDataCellMapperT<V, DA> ("PointGroup cell mapper", parent)
{}

template <class V, class DA>
GeoDataMapperSettingI* GeoDataPointGroupCellMapperT<V, DA>::initialize(bool* /*boolMap*/)
{
	auto s = new GeoDataPointGroupNodeMapperSetting();
	return s;
}

template <class V, class DA>
void GeoDataPointGroupCellMapperT<V, DA>::map(bool* /*boolMap*/, GeoDataMapperSettingI* /*s*/)
{}

template <class V, class DA>
void GeoDataPointGroupCellMapperT<V, DA>::terminate(GeoDataMapperSettingI* setting)
{
	delete setting;
}

#endif // GEODATAPOINTGROUPCELLMAPPERT_DETAIL_H

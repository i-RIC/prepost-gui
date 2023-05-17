#ifndef GEODATABACKGROUNDCELLMAPPERT_DETAIL_H
#define GEODATABACKGROUNDCELLMAPPERT_DETAIL_H

#include "../geodatabackgroundcellmappert.h"

template <class V, class DA>
GeoDataBackgroundCellMapperT<V, DA>::GeoDataBackgroundCellMapperT(GeoDataCreator* parent) :
	GeoDataCellMapperT<V, DA>("Background cell mapper", parent)
{}

template <class V, class DA>
GeoDataMapperSettingI* GeoDataBackgroundCellMapperT<V, DA>::initialize(bool* boolMap)
{
	GeoDataBackgroundCellMapperSetting* s = new GeoDataBackgroundCellMapperSetting();
	unsigned int count = GeoDataMapperT<V>::container()->dataCount();
	for (unsigned int i = 0; i < count; ++i) {
		if (! *(boolMap + i)) {
			// not mapped yet.
			// So, map. ALWAYS.
			s->ranges.append(i);
			*(boolMap + i) = true;
		}
	}
	return s;
}

template <class V, class DA>
void GeoDataBackgroundCellMapperT<V, DA>::map(bool* boolMap, GeoDataMapperSettingI* s)
{
	auto s2 = dynamic_cast<GeoDataBackgroundCellMapperSetting*>(s);
	DA* da = dynamic_cast<DA*>(GeoDataMapperT<V>::container()->dataArray());
	GeoDataBackground* background = dynamic_cast<GeoDataBackground* >(GeoDataMapper::geoData());
	QVariant variantValue = background->variantValue();
	V value = GeoDataMapperT<V>::fromVariant(variantValue);
	const auto& ranges = s2->ranges.ranges();
	for (const auto& r : ranges) {
		for (unsigned int j = r.from; j <= r.to; ++j) {
			if (*(boolMap + j) == false) {
				da->SetValue(static_cast<vtkIdType>(j), value);
				*(boolMap + j) = true;
			}
		}
	}
	da->Modified();
}

template <class V, class DA>
void GeoDataBackgroundCellMapperT<V, DA>::terminate(GeoDataMapperSettingI* s)
{
	delete s;
}

#endif // GEODATABACKGROUNDCELLMAPPERT_DETAIL_H

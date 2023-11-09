#ifndef GEODATABACKGROUONDNODEMAPPERT_DETAIL_H
#define GEODATABACKGROUONDNODEMAPPERT_DETAIL_H

#include "../geodatabackgroundnodemappert.h"

class GeoDataBackgroundNodeMapperSetting : public GeoDataMapperSettingI
{
public:
	GeoDataBackgroundNodeMapperSetting() : GeoDataMapperSettingI() {}
	~GeoDataBackgroundNodeMapperSetting() {}

	IntegerRangeContainer ranges;
};

template <class V, class DA>
GeoDataBackgroundNodeMapperT<V, DA>::GeoDataBackgroundNodeMapperT(GeoDataCreator* parent) :
	GeoDataNodeMapperT<V, DA> ("Background node mapper", parent)
{}

template <class V, class DA>
GeoDataMapperSettingI* GeoDataBackgroundNodeMapperT<V, DA>::initialize(bool* boolMap)
{
	auto s = new GeoDataBackgroundNodeMapperSetting();
	unsigned int count = GeoDataMapperT<V, DA>::container()->dataCount();
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
void GeoDataBackgroundNodeMapperT<V, DA>::map(bool* boolMap, GeoDataMapperSettingI* s)
{
	auto s2 = dynamic_cast<GeoDataBackgroundNodeMapperSetting*>(s);
	auto da = GeoDataMapperT<V, DA>::container()->dataArray();
	auto background = dynamic_cast<GeoDataBackground* >(GeoDataMapper::geoData());
	QVariant variantValue = background->variantValue();
	V value = GeoDataMapperT<V, DA>::fromVariant(variantValue);
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
void GeoDataBackgroundNodeMapperT<V, DA>::terminate(GeoDataMapperSettingI* s)
{
	delete s;
}

#endif // GEODATABACKGROUONDNODEMAPPERT_DETAIL_H

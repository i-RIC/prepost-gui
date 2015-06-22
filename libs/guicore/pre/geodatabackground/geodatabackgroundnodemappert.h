#ifndef GEODATABACKGROUNDNODEMAPPERT_H
#define GEODATABACKGROUNDNODEMAPPERT_H

#include "../geodata/geodatanodemappert.h"
#include "geodatabackground.h"
#include <misc/integerrangecontainer.h>

class GeoDataBackgroundNodeMapperSetting : public GeoDataMapperSetting
{

public:
	GeoDataBackgroundNodeMapperSetting() : GeoDataMapperSetting() {}
	virtual ~GeoDataBackgroundNodeMapperSetting() {}
	IntegerRangeContainer ranges;
};

template <class V, class DA>
class GeoDataBackgroundNodeMapperT : public GeoDataNodeMapperT<V, DA>
{

public:
	GeoDataBackgroundNodeMapperT(GeoDataCreator* parent)
		: GeoDataNodeMapperT<V, DA>(parent) {
		GeoDataNodeMapperT<V, DA>::m_caption = "Background node mapper";
	}
	GeoDataMapperSetting* initialize(bool* boolMap) override {
		GeoDataBackgroundNodeMapperSetting* s = new GeoDataBackgroundNodeMapperSetting();
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
	void map(bool* boolMap, GeoDataMapperSetting* s) override {
		GeoDataBackgroundNodeMapperSetting* s2 =
			dynamic_cast<GeoDataBackgroundNodeMapperSetting*>(s);
		DA* da = dynamic_cast<DA*>(GeoDataMapperT<V>::container()->dataArray());
		GeoDataBackground* background = dynamic_cast<GeoDataBackground* >(GeoDataMapper::m_geodata);
		QVariant variantValue = background->variantValue();
		V value = GeoDataMapperT<V>::fromVariant(variantValue);
		const QList<IntegerRangeContainer::Range>& ranges = s2->ranges.ranges();
		for (int i = 0; i < ranges.size(); ++i) {
			const IntegerRangeContainer::Range& r = ranges.at(i);
			for (unsigned int j = r.from; j <= r.to; ++j) {
				if (*(boolMap + j) == false) {
					da->SetValue(static_cast<vtkIdType>(j), value);
					*(boolMap + j) = true;
				}
			}
		}
		da->Modified();
	}
	void terminate(GeoDataMapperSetting* s) override {
		delete s;
	}
};

#endif // GEODATABACKGROUNDNODEMAPPERT_H

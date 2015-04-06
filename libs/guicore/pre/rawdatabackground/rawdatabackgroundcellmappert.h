#ifndef RAWDATABACKGROUNDCELLMAPPERT_H
#define RAWDATABACKGROUNDCELLMAPPERT_H

#include "../rawdata/rawdatacellmappert.h"
#include "rawdatabackground.h"
#include <misc/integerrangecontainer.h>

class RawDataBackgroundCellMapperSetting : public RawDataMapperSetting
{
public:
	RawDataBackgroundCellMapperSetting() : RawDataMapperSetting(){}
	~RawDataBackgroundCellMapperSetting(){}
	IntegerRangeContainer ranges;
};

template <class V, class DA>
class RawDataBackgroundCellMapperT : public RawDataCellMapperT<V, DA>
{
public:
	RawDataBackgroundCellMapperT(RawDataCreator* parent)
		: RawDataCellMapperT<V, DA>(parent)
	{
		RawDataCellMapperT<V, DA>::m_caption = "Background cell mapper";
	}
	RawDataMapperSetting* initialize(bool* boolMap)
	{
		RawDataBackgroundCellMapperSetting* s = new RawDataBackgroundCellMapperSetting();
		unsigned int count = RawDataMapperT<V>::container()->dataCount();
		for (unsigned int i = 0; i < count; ++i){
			if (! *(boolMap + i)){
				// not mapped yet.
				// So, map. ALWAYS.
				s->ranges.append(i);
				*(boolMap + i) = true;
			}
		}
		return s;
	}

	void map(bool* boolMap, RawDataMapperSetting* s)
	{
		RawDataBackgroundCellMapperSetting* s2 =
				dynamic_cast<RawDataBackgroundCellMapperSetting*> (s);
		DA* da = dynamic_cast<DA*>(RawDataMapperT<V>::container()->dataArray());
		RawDataBackground* background = dynamic_cast<RawDataBackground* >(RawDataMapper::m_rawdata);
		QVariant variantValue = background->variantValue();
		V value = RawDataMapperT<V>::fromVariant(variantValue);
		const QList<IntegerRangeContainer::Range>& ranges = s2->ranges.ranges();
		for (int i = 0; i < ranges.size(); ++i){
			const IntegerRangeContainer::Range& r = ranges.at(i);
			for (unsigned int j = r.from; j <= r.to; ++j){
				if (*(boolMap + j) == false){
					da->SetValue(static_cast<vtkIdType>(j), value);
					*(boolMap + j) = true;
				}
			}
		}
		da->Modified();
	}
	void terminate(RawDataMapperSetting* s)
	{
		delete s;
	}
};

#endif // RAWDATABACKGROUNDCELLMAPPERT_H

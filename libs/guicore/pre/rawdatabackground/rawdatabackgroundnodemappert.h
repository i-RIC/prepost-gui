#ifndef RAWDATABACKGROUNDNODEMAPPERT_H
#define RAWDATABACKGROUNDNODEMAPPERT_H

#include "../rawdata/rawdatanodemappert.h"
#include "rawdatabackground.h"
#include <misc/integerrangecontainer.h>

class RawDataBackgroundNodeMapperSetting : public RawDataMapperSetting
{
public:
	RawDataBackgroundNodeMapperSetting() : RawDataMapperSetting(){}
	virtual ~RawDataBackgroundNodeMapperSetting(){}
	IntegerRangeContainer ranges;
};

template <class V, class DA>
class RawDataBackgroundNodeMapperT : public RawDataNodeMapperT<V, DA>
{
public:
	RawDataBackgroundNodeMapperT(RawDataCreator* parent)
		: RawDataNodeMapperT<V, DA>(parent)
	{
		RawDataNodeMapperT<V, DA>::m_caption = "Background node mapper";
	}
	RawDataMapperSetting* initialize(bool* boolMap)
	{
		RawDataBackgroundNodeMapperSetting* s = new RawDataBackgroundNodeMapperSetting();
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
		RawDataBackgroundNodeMapperSetting* s2 =
				dynamic_cast<RawDataBackgroundNodeMapperSetting*> (s);
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

#endif // RAWDATABACKGROUNDNODEMAPPERT_H

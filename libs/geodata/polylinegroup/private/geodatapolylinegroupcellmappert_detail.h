#ifndef GEODATAPOLYLINEGROUPCELLMAPPERT_DETAIL_H
#define GEODATAPOLYLINEGROUPCELLMAPPERT_DETAIL_H

#include "../geodatapolylinegroup.h"
#include "../geodatapolylinegroupcellmappert.h"
#include "../geodatapolylinegrouppolyline.h"

#include <guibase/vtktool/vtkpointsutil.h>

template <class V, class DA>
GeoDataPolyLineGroupCellMapperT<V, DA>::GeoDataPolyLineGroupCellMapperT(GeoDataCreator* parent) :
	GeoDataCellMapperT<V, DA> ("PolyLineGroup cell mapper", parent)
{}

template <class V, class DA>
GeoDataMapperSettingI* GeoDataPolyLineGroupCellMapperT<V, DA>::initialize(bool* boolMap)
{
	auto s = new GeoDataPolyLineGroupCellMapperSetting();
	unsigned int count = GeoDataMapperT<V>::container()->dataCount();
	auto polylineGroup = dynamic_cast<GeoDataPolyLineGroup*> (GeoDataMapper::geoData());
	polylineGroup->mergeEditTargetData();
	polylineGroup->updateOrder();

	for (unsigned int i = 0; i < count; ++i) {
		if (*(boolMap + i)) {
			s->ranges.add(nullptr);
			continue;
		}
		// not mapped yet.
		vtkCell* cell = GeoDataMapper::grid()->vtkGrid()->GetCell(i);
		auto polygon = vtkPointsUtil::getPolygon(cell);
		double xmin, xmax, ymin, ymax;
		vtkPointsUtil::getBoundingBox(cell, &xmin, &xmax, &ymin, &ymax);
		auto lines = polylineGroup->polyLinesInBoundingBox(xmin, xmax, ymin, ymax);

		std::map<unsigned int, GeoDataPolyLineGroupPolyLine*> linesMap;
		for (GeoDataPolyLineGroupPolyLine* l : lines) {
			if (l->intersectWithPolygon(polygon)) {
				linesMap.insert({l->order(), l});
			}
		}
		if (linesMap.size() > 0) {
			s->ranges.add(linesMap.begin()->second);
			*(boolMap + i) = true;
		} else {
			s->ranges.add(nullptr);
		}
	}
	return s;
}

template <class V, class DA>
void GeoDataPolyLineGroupCellMapperT<V, DA>::map(bool* boolMap, GeoDataMapperSettingI* s)
{
	auto setting = dynamic_cast<GeoDataPolyLineGroupCellMapperSetting*> (s);
	DA* da = dynamic_cast<DA*> (GeoDataMapperT<V>::container()->dataArray());
	const auto& lines = setting->ranges.data();
	const auto& maxIndices = setting->ranges.maxIndices();
	int rangeId = 0;
	vtkIdType idx = 0;
	while (rangeId < maxIndices.size()) {
		GeoDataPolyLineGroupPolyLine* line = lines.at(rangeId);
		vtkIdType maxIdx = maxIndices.at(rangeId);
		if (line == nullptr) {
			idx = maxIdx + 1;
		} else {
			V value = GeoDataMapperT<V>::fromVariant(line->value());
			while (idx <= maxIdx) {
				if (*(boolMap + idx) == false) {
					da->SetValue(idx, value);
					*(boolMap + idx) = true;
				}
				++ idx;
			}
		}
		++ rangeId;
	}
}

template <class V, class DA>
void GeoDataPolyLineGroupCellMapperT<V, DA>::terminate(GeoDataMapperSettingI* setting)
{
	delete setting;
}

#endif // GEODATAPOLYLINEGROUPCELLMAPPERT_DETAIL_H

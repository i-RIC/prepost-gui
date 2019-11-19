#ifndef GEODATAPOLYGONGROUPCELLMAPPERT_DETAIL_H
#define GEODATAPOLYGONGROUPCELLMAPPERT_DETAIL_H

#include "../geodatapolygongroupcellmappert.h"

#include <guibase/vtktool/vtkpointsutil.h>

template <class V, class DA>
GeoDataPolygonGroupCellMapperT<V, DA>::GeoDataPolygonGroupCellMapperT(GeoDataCreator* parent) :
	GeoDataCellMapperT<V, DA> ("PolygonGroup cell mapper", parent)
{}

template <class V, class DA>
GeoDataMapperSettingI* GeoDataPolygonGroupCellMapperT<V, DA>::initialize(bool* boolMap)
{
	auto s = new GeoDataPolygonGroupNodeMapperSetting();
	unsigned int count = GeoDataMapperT<V>::container()->dataCount();
	auto polygonGroup = dynamic_cast<GeoDataPolygonGroup*> (GeoDataMapper::geoData());
	polygonGroup->mergeEditTargetPolygon();
	for (unsigned int i = 0; i < count; ++i) {
		if (*(boolMap + i)) {
			s->ranges.add(nullptr);
			continue;
		}
		// not mapped yet.
		vtkCell* cell = GeoDataMapper::grid()->vtkGrid()->GetCell(i);
		QPointF point = vtkPointsUtil::getCenter(cell);
		auto pols = polygonGroup->polygonsInBoundingBox(point.x() - TINY_DOUBLE, point.x() + TINY_DOUBLE, point.y() - TINY_DOUBLE, point.y() + TINY_DOUBLE);

		GeoDataPolygonGroupPolygon* mappedPolygon = nullptr;
		for (GeoDataPolygonGroupPolygon* p : pols) {
			if (p->isInside(QPointF(point.x(), point.y()))) {
				mappedPolygon = p;
				break;
			}
		}
		s->ranges.add(mappedPolygon);
	}
	return s;
}

template <class V, class DA>
void GeoDataPolygonGroupCellMapperT<V, DA>::map(bool* boolMap, GeoDataMapperSettingI* s)
{
	auto setting = dynamic_cast<GeoDataPolygonGroupNodeMapperSetting*> (s);
	DA* da = dynamic_cast<DA*> (GeoDataMapperT<V>::container()->dataArray());
	const auto& polys = setting->ranges.polygons();
	const auto& maxIndices = setting->ranges.maxIndices();
	int rangeId = 0;
	vtkIdType idx = 0;
	while (rangeId < maxIndices.size()) {
		GeoDataPolygonGroupPolygon* pol = polys.at(rangeId);
		vtkIdType maxIdx = maxIndices.at(rangeId);
		if (pol == nullptr) {
			idx = maxIdx + 1;
		} else {
			V value = GeoDataMapperT<V>::fromVariant(pol->value());
			while (idx <= maxIdx) {
				da->SetValue(idx, value);
				*(boolMap + idx) = true;
				++ idx;
			}
		}
		++ rangeId;
	}
}

template <class V, class DA>

void GeoDataPolygonGroupCellMapperT<V, DA>::terminate(GeoDataMapperSettingI* setting)
{
	delete setting;
}

#endif // GEODATAPOLYGONGROUPCELLMAPPERT_DETAIL_H

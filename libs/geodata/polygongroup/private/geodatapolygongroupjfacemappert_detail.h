#ifndef GEODATAPOLYGONGROUPJFACEMAPPERT_DETAIL_H
#define GEODATAPOLYGONGROUPJFACEMAPPERT_DETAIL_H

#include "../geodatapolygongroupjfacemappert.h"
#include "../geodatapolygongrouppolygon.h"

#include <guibase/vtkpointsetextended/vtkpolydataextended2d.h>
#include <guibase/vtktool/vtkpointsutil.h>
#include <guicore/grid/v4structured2dgrid.h>
#include <guicore/pre/grid/v4inputgrid.h>
#include <misc/integerrangecontainer.h>

template <class V, class DA>
GeoDataPolygonGroupJFaceMapperT<V, DA>::GeoDataPolygonGroupJFaceMapperT(GeoDataCreator* parent) :
	GeoDataJFaceMapperT<V, DA> ("PolygonGroup jface mapper", parent)
{}

template <class V, class DA>
GeoDataMapperSettingI* GeoDataPolygonGroupJFaceMapperT<V, DA>::initialize(bool* boolMap)
{
	auto s = new GeoDataPolygonGroupJFaceMapperSetting();
	unsigned int count = GeoDataMapperT<V, DA>::container()->dataCount();
	auto polygonGroup = dynamic_cast<GeoDataPolygonGroup*> (GeoDataMapper::geoData());
	polygonGroup->mergeEditTargetData();
	polygonGroup->updateOrder();

	vtkPointSet* jfaceGrid = dynamic_cast<v4Structured2dGrid*> (GeoDataMapper::grid()->grid())->vtkJEdgeData()->data();

	for (unsigned int i = 0; i < count; ++i) {
		if (*(boolMap + i)) {
			s->ranges.add(nullptr);
			continue;
		}
		// not mapped yet.
		auto point = vtkPointsUtil::getCenter(jfaceGrid->GetCell(i));
		auto pols = polygonGroup->polygonsInBoundingBox(point.x() - TINY_DOUBLE, point.x() + TINY_DOUBLE, point.y() - TINY_DOUBLE, point.y() + TINY_DOUBLE);

		std::map<unsigned int, GeoDataPolygonGroupPolygon*> polsMap;
		for (auto p : pols) {
			if (p->isInside(QPointF(point.x(), point.y()))) {
				polsMap.insert({p->order(), p});
			}
		}
		if (polsMap.size() > 0) {
			s->ranges.add(polsMap.begin()->second);
			*(boolMap + i) = true;
		} else {
			s->ranges.add(nullptr);
		}
	}
	return s;
}

template <class V, class DA>
void GeoDataPolygonGroupJFaceMapperT<V, DA>::map(bool* boolMap, GeoDataMapperSettingI* s)
{
	auto setting = dynamic_cast<GeoDataPolygonGroupJFaceMapperSetting*> (s);
	DA* da = dynamic_cast<DA*> (GeoDataMapperT<V, DA>::container()->dataArray());
	const auto& polys = setting->ranges.data();
	const auto& maxIndices = setting->ranges.maxIndices();
	int rangeId = 0;
	vtkIdType idx = 0;
	while (rangeId < maxIndices.size()) {
		GeoDataPolygonGroupPolygon* pol = polys.at(rangeId);
		vtkIdType maxIdx = maxIndices.at(rangeId);
		if (pol == nullptr) {
			idx = maxIdx + 1;
		} else {
			V value = GeoDataMapperT<V, DA>::fromVariant(pol->value());
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

void GeoDataPolygonGroupJFaceMapperT<V, DA>::terminate(GeoDataMapperSettingI* setting)
{
	delete setting;
}
#endif // GEODATAPOLYGONGROUPJFACEMAPPERT_DETAIL_H

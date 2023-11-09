#ifndef GEODATAPOLYLINEGROUPIFACEMAPPERT_DETAIL_H
#define GEODATAPOLYLINEGROUPIFACEMAPPERT_DETAIL_H

#include "../geodatapolylinegroup.h"
#include "../geodatapolylinegroupifacemappert.h"
#include "../geodatapolylinegrouppolyline.h"

#include <guibase/vtkpointsetextended/vtkpolydataextended2d.h>
#include <guibase/vtktool/vtkpointsutil.h>
#include <guicore/grid/v4structured2dgrid.h>
#include <guicore/pre/geodata/geodatamappersettingi.h>
#include <guicore/pre/grid/v4inputgrid.h>
#include <misc/rangecontainert.h>

class GeoDataPolyLineGroupIFaceMapperSetting : public GeoDataMapperSettingI
{
public:
	GeoDataPolyLineGroupIFaceMapperSetting() : GeoDataMapperSettingI() {}
	~GeoDataPolyLineGroupIFaceMapperSetting() override {}

	RangeContainerT<GeoDataPolyLineGroupPolyLine*> ranges;
};

template <class V, class DA>
GeoDataPolyLineGroupIFaceMapperT<V, DA>::GeoDataPolyLineGroupIFaceMapperT(GeoDataCreator* parent) :
	GeoDataIFaceMapperT<V, DA> ("PolyLineGroup iface mapper", parent)
{}

template <class V, class DA>
GeoDataMapperSettingI* GeoDataPolyLineGroupIFaceMapperT<V, DA>::initialize(bool* boolMap)
{
	auto s = new GeoDataPolyLineGroupIFaceMapperSetting();
	unsigned int count = GeoDataMapperT<V, DA>::container()->dataCount();
	auto polylineGroup = dynamic_cast<GeoDataPolyLineGroup*> (GeoDataMapper::geoData());
	polylineGroup->mergeEditTargetData();
	polylineGroup->updateOrder();

	auto grid = dynamic_cast<v4Structured2dGrid*> (GeoDataMapper::grid()->grid());
	vtkPointSet* ifaceGrid = grid->vtkIEdgeData()->data();
	QPointF p1, p2;
	std::vector<QPointF> points;
	double xmin, xmax, ymin, ymax;

	for (unsigned int i = 0; i < count; ++i) {
		if (*(boolMap + i)) {continue;}

		// not mapped yet.
		auto cell = ifaceGrid->GetCell(i);
		vtkPointsUtil::getBoundingBox(cell, &xmin, &xmax, &ymin, &ymax);
		auto lines = polylineGroup->polyLinesInBoundingBox(xmin, xmax, ymin, ymax);

		std::map<unsigned int, GeoDataPolyLineGroupPolyLine*> linesMap;
		for (auto l : lines) {
			p1 = grid->point2d(cell->GetPointId(0));
			p2 = grid->point2d(cell->GetPointId(1));
			QPointF intersection;
			double r;
			if (l->intersectWithLine(p1, p2, &intersection, &r)) {
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
void GeoDataPolyLineGroupIFaceMapperT<V, DA>::map(bool* boolMap, GeoDataMapperSettingI* s)
{
	auto setting = dynamic_cast<GeoDataPolyLineGroupIFaceMapperSetting*> (s);
	DA* da = dynamic_cast<DA*> (GeoDataMapperT<V, DA>::container()->dataArray());
	const auto& lines = setting->ranges.data();
	const auto& maxIndices = setting->ranges.maxIndices();
	int rangeId = 0;
	vtkIdType idx = 0;
	while (rangeId < maxIndices.size()) {
		auto line = lines.at(rangeId);
		vtkIdType maxIdx = maxIndices.at(rangeId);
		if (line == nullptr) {
			idx = maxIdx + 1;
		} else {
			V value = GeoDataMapperT<V, DA>::fromVariant(line->value());
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
void GeoDataPolyLineGroupIFaceMapperT<V, DA>::terminate(GeoDataMapperSettingI* setting)
{
	delete setting;
}

#endif // GEODATAPOLYLINEGROUPIFACEMAPPERT_DETAIL_H

#ifndef GEODATAPOLYGONCELLMAPPERT_DETAIL_H
#define GEODATAPOLYGONCELLMAPPERT_DETAIL_H

#include "../geodatapolygoncellmappert.h"
#include "../geodatapolygoncellmappersetting.h"

#include <guibase/vtktool/vtkpointsutil.h>

#include <vtkCell.h>

template <class V, class DA>
GeoDataPolygonCellMapperT<V, DA>::GeoDataPolygonCellMapperT(GeoDataCreator* parent) :
	GeoDataCellMapperT<V, DA> ("Polygon cell mapper", parent)
{}

template <class V, class DA>
GeoDataMapperSettingI* GeoDataPolygonCellMapperT<V, DA>::initialize(bool* boolMap)
{
	GeoDataPolygonCellMapperSetting* s = new GeoDataPolygonCellMapperSetting();
	unsigned int count = GeoDataMapperT<V>::container()->dataCount();
	GeoDataPolygon* polygon = dynamic_cast<GeoDataPolygon* >(GeoDataMapper::geoData());
	vtkPointSet* pointSet = polygon->polyData();
	double bounds[6];
	pointSet->GetBounds(bounds);
	double weights[3];
	for (unsigned int i = 0; i < count; ++i) {
		if (! *(boolMap + i)) {
			// not mapped yet.
			// get grid cell.
			vtkCell* cell = GeoDataMapper::grid()->vtkGrid()->GetCell(i);
			double point[3];
			QPointF center = vtkPointsUtil::getCenter(cell);
			point[0] = center.x();
			point[1] = center.y();
			point[2] = 0;
			// investigate whether the point is inside the polygon.
			bool in;
			// first use bounds for checking.
			if (point[0] < bounds[0] || // x < xmin
					point[0] > bounds[1] || // x > xmax
					point[1] < bounds[2] || // y < ymin
					point[1] > bounds[3]) { // y > ymax
				// not in the polygon.
				in = false;
			} else {
				vtkIdType cellid;
				double pcoords[3];
				int subid;
				cellid = pointSet->FindCell(point, 0, 0, 1e-4, subid, pcoords, weights);
				in = (cellid >= 0);
				if (! in) {
					// Not found, but if the grid is ugly, sometimes FindCell()
					// fails, even there is a cell that contains point.
					for (cellid = 0; ! in && cellid < pointSet->GetNumberOfCells(); ++cellid) {
						vtkCell* tmpcell = pointSet->GetCell(cellid);
						double dist2;
						double closestPoint[3];
						int ret = tmpcell->EvaluatePosition(point, &(closestPoint[0]), subid, pcoords, dist2, weights);
						in = (ret == 1);
					}
				}
			}
			if (in) {
				s->ranges.append(i);
				*(boolMap + i) = true;
			}
		}
	}
	return s;
}

template <class V, class DA>
void GeoDataPolygonCellMapperT<V, DA>::map(bool* boolMap, GeoDataMapperSettingI* s)
{
	GeoDataPolygonCellMapperSetting* setting = dynamic_cast<GeoDataPolygonCellMapperSetting*>(s);
	DA* da = dynamic_cast<DA*>(GeoDataMapperT<V>::container()->dataArray());
	GeoDataPolygon* polygon = dynamic_cast<GeoDataPolygon* >(GeoDataMapperT<V>::geoData());
	V value = GeoDataMapperT<V>::fromVariant(polygon->variantValue());
	const QList<IntegerRangeContainer::Range>& ranges = setting->ranges.ranges();
	for (const IntegerRangeContainer::Range& r : ranges) {
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
void GeoDataPolygonCellMapperT<V, DA>::terminate(GeoDataMapperSettingI* s)
{
	delete s;
}

#endif // GEODATAPOLYGONCELLMAPPERT_DETAIL_H

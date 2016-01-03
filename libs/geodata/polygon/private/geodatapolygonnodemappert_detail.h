#ifndef GEODATAPOLYGONNODEMAPPERT_DETAIL_H
#define GEODATAPOLYGONNODEMAPPERT_DETAIL_H

#include "../geodatapolygonnodemappert.h"

template <class V, class DA>
GeoDataPolygonNodeMapperT<V, DA>::GeoDataPolygonNodeMapperT(GeoDataCreator* parent) :
	GeoDataNodeMapperT<V, DA> ("Polygon node mapper", parent)
{}

template <class V, class DA>
GeoDataMapperSettingI* GeoDataPolygonNodeMapperT<V, DA>::initialize(bool* boolMap)
{
	GeoDataPolygonNodeMapperSetting* s = new GeoDataPolygonNodeMapperSetting();
	unsigned int count = GeoDataMapperT<V>::container()->dataCount();
	GeoDataPolygon* polygon = dynamic_cast<GeoDataPolygon* >(GeoDataMapper::geoData());
	vtkUnstructuredGrid* tmpgrid = polygon->grid();
	double bounds[6];
	tmpgrid->GetBounds(bounds);
	double weights[3];
	for (unsigned int i = 0; i < count; ++i) {
		if (! *(boolMap + i)) {
			// not mapped yet.
			double point[3];
			GeoDataMapper::grid()->vtkGrid()->GetPoint(i, point);
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
				cellid = tmpgrid->FindCell(point, 0, 0, 1e-4, subid, pcoords, weights);
				in = (cellid >= 0);
				if (! in) {
					// Not found, but if the grid is ugly, sometimes FindCell()
					// fails, even there is a cell that contains point.
					for (cellid = 0; ! in && cellid < tmpgrid->GetNumberOfCells(); ++cellid) {
						vtkCell* tmpcell = tmpgrid->GetCell(cellid);
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
void GeoDataPolygonNodeMapperT<V, DA>::map(bool* boolMap, GeoDataMapperSettingI* s)
{
	GeoDataPolygonNodeMapperSetting* setting = dynamic_cast<GeoDataPolygonNodeMapperSetting*>(s);
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

void GeoDataPolygonNodeMapperT<V, DA>::terminate(GeoDataMapperSettingI* setting)
{
	delete setting;
}

#endif // GEODATAPOLYGONNODEMAPPERT_DETAIL_H

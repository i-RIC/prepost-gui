#ifndef GEODATAPOLYGONNODEMAPPERT_H
#define GEODATAPOLYGONNODEMAPPERT_H

#include <guicore/pre/geodata/geodatanodemappert.h>
#include "geodatapolygon.h"
#include <misc/integerrangecontainer.h>
#include <vtkCell.h>
#include <vtkDataSetTriangleFilter.h>
#include <vtkSmartPointer.h>

class GeoDataPolygonNodeMapperSetting : public GeoDataMapperSetting
{

public:
	GeoDataPolygonNodeMapperSetting() : GeoDataMapperSetting() {}
	virtual ~GeoDataPolygonNodeMapperSetting() {}
	IntegerRangeContainer ranges;
};

template <class V, class DA>
class GeoDataPolygonNodeMapperT : public GeoDataNodeMapperT<V, DA>
{

public:
	GeoDataPolygonNodeMapperT(GeoDataCreator* parent) :
		GeoDataNodeMapperT<V, DA> {parent}
	{
		GeoDataNodeMapperT<V, DA>::m_caption = "Polygon node mapper";
	}
	GeoDataMapperSetting* initialize(bool* boolMap) override {
		GeoDataPolygonNodeMapperSetting* s = new GeoDataPolygonNodeMapperSetting();
		unsigned int count = GeoDataMapperT<V>::container()->dataCount();
		GeoDataPolygon* polygon = dynamic_cast<GeoDataPolygon* >(GeoDataMapper::m_geodata);
		vtkUnstructuredGrid* tmpgrid = polygon->grid();
		double bounds[6];
		tmpgrid->GetBounds(bounds);
		double weights[3];
		for (unsigned int i = 0; i < count; ++i) {
			if (! *(boolMap + i)) {
				// not mapped yet.
				double point[3];
				GeoDataMapper::m_grid->vtkGrid()->GetPoint(i, point);
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

	void map(bool* boolMap, GeoDataMapperSetting* s) override {
		GeoDataPolygonNodeMapperSetting* setting = dynamic_cast<GeoDataPolygonNodeMapperSetting*>(s);
		DA* da = dynamic_cast<DA*>(GeoDataMapperT<V>::container()->dataArray());
		GeoDataPolygon* polygon = dynamic_cast<GeoDataPolygon* >(GeoDataMapperT<V>::m_geodata);
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
	void terminate(GeoDataMapperSetting* setting) override {
		delete setting;
	}
};

#endif // GEODATAPOLYGONNODEMAPPERT_H

#ifndef GEODATAPOLYGONCELLMAPPERT_H
#define GEODATAPOLYGONCELLMAPPERT_H

#include <guicore/pre/geodata/geodatacellmappert.h>
#include "geodatapolygon.h"
#include <vtkCell.h>
#include <vtkDataSetTriangleFilter.h>
#include <vtkSmartPointer.h>
#include <misc/integerrangecontainer.h>

class GeoDataPolygonCellMapperSetting : public GeoDataMapperSetting
{

public:
	GeoDataPolygonCellMapperSetting() : GeoDataMapperSetting() {}
	virtual ~GeoDataPolygonCellMapperSetting() {}
	IntegerRangeContainer ranges;
};

template <class V, class DA>
class GeoDataPolygonCellMapperT : public GeoDataCellMapperT<V, DA>
{

public:
	GeoDataPolygonCellMapperT(GeoDataCreator* parent) :
		GeoDataCellMapperT<V, DA> (parent)
	{
		GeoDataCellMapperT<V, DA>::m_caption = "Polygon cell mapper";
	}
	GeoDataMapperSetting* initialize(bool* boolMap) override {
		GeoDataPolygonCellMapperSetting* s = new GeoDataPolygonCellMapperSetting();
		unsigned int count = GeoDataMapperT<V>::container()->dataCount();
		GeoDataPolygon* polygon = dynamic_cast<GeoDataPolygon* >(GeoDataMapper::m_geodata);
		// first, triangulate the polygon.
		vtkUnstructuredGrid* tmpgrid = polygon->grid();
		double bounds[6];
		tmpgrid->GetBounds(bounds);
		double weights[3];
		for (unsigned int i = 0; i < count; ++i) {
			if (! *(boolMap + i)) {
				// not mapped yet.
				// get grid cell.
				vtkCell* cell = GeoDataMapper::m_grid->vtkGrid()->GetCell(i);
				vtkIdList* ids = cell->GetPointIds();
				bool cellIsInPolygon = true;
				int pointCount = ids->GetNumberOfIds();
				for (int pid = 0; cellIsInPolygon && pid < pointCount; ++pid) {
					int pindex = cell->GetPointId(pid);
					double point[3];
					GeoDataMapper::m_grid->vtkGrid()->GetPoint(pindex, point);
					// investigate whether the point is inside the polygon.
					// first use bounds for checking.
					if (point[0] < bounds[0] || // x < xmin
							point[0] > bounds[1] || // x > xmax
							point[1] < bounds[2] || // y < ymin
							point[1] > bounds[3]) { // y > ymax
						// this point is outside of polygon.
						cellIsInPolygon = false;
					} else {
						vtkIdType cellid;
						double pcoords[3];
						int subid;
						cellid = tmpgrid->FindCell(point, 0, 0, 1e-4, subid, pcoords, weights);
						bool found = (cellid >= 0);
						if (! found) {
							// Not found, but if the grid is ugly, sometimes FindCell()
							// fails, even there is a cell that contains point.
							for (cellid = 0; ! found && cellid < tmpgrid->GetNumberOfCells(); ++cellid) {
								vtkCell* tmpcell = tmpgrid->GetCell(cellid);
								double dist2;
								double closestPoint[3];
								int ret = tmpcell->EvaluatePosition(point, &(closestPoint[0]), subid, pcoords, dist2, weights);
								found = (ret == 1);
							}
						}
						cellIsInPolygon = cellIsInPolygon && found;
					}
				}
				if (cellIsInPolygon) {
					s->ranges.append(i);
					*(boolMap + i) = true;
				}
			}
		}
		return s;
	}

	void map(bool* boolMap, GeoDataMapperSetting* s) override {
		GeoDataPolygonCellMapperSetting* setting = dynamic_cast<GeoDataPolygonCellMapperSetting*>(s);
		DA* da = dynamic_cast<DA*>(GeoDataMapperT<V>::container()->dataArray());
		GeoDataPolygon* polygon = dynamic_cast<GeoDataPolygon* >(GeoDataMapperT<V>::m_geodata);
		V value = GeoDataMapperT<V>::fromVariant(polygon->variantValue());
		const QList<IntegerRangeContainer::Range>& ranges = setting->ranges.ranges();
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

#endif // GEODATAPOLYGONCELLMAPPERT_H

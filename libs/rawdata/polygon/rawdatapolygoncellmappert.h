#ifndef RAWDATAPOLYGONCELLMAPPERT_H
#define RAWDATAPOLYGONCELLMAPPERT_H

#include <guicore/pre/rawdata/rawdatacellmappert.h>
#include "rawdatapolygon.h"
#include <vtkCell.h>
#include <vtkDataSetTriangleFilter.h>
#include <vtkSmartPointer.h>
#include <misc/integerrangecontainer.h>

class RawDataPolygonCellMapperSetting : public RawDataMapperSetting
{
public:
	RawDataPolygonCellMapperSetting() : RawDataMapperSetting(){}
	virtual ~RawDataPolygonCellMapperSetting(){}
	IntegerRangeContainer ranges;
};

template <class V, class DA>
class RawDataPolygonCellMapperT : public RawDataCellMapperT<V, DA>
{
public:
	RawDataPolygonCellMapperT(RawDataCreator* parent)
		: RawDataCellMapperT<V, DA>(parent)
	{
		RawDataCellMapperT<V, DA>::m_caption = "Polygon cell mapper";
	}
	RawDataMapperSetting* initialize(bool* boolMap)
	{
		RawDataPolygonCellMapperSetting* s = new RawDataPolygonCellMapperSetting();
		unsigned int count = RawDataMapperT<V>::container()->dataCount();
		RawDataPolygon* polygon = dynamic_cast<RawDataPolygon* >(RawDataMapper::m_rawdata);
		// first, triangulate the polygon.
		vtkUnstructuredGrid* tmpgrid = polygon->grid();
		double bounds[6];
		tmpgrid->GetBounds(bounds);
		double weights[3];
		for (unsigned int i = 0; i < count; ++i){
			if (! *(boolMap + i)){
				// not mapped yet.
				// get grid cell.
				vtkCell* cell = RawDataMapper::m_grid->vtkGrid()->GetCell(i);
				vtkIdList* ids = cell->GetPointIds();
				bool cellIsInPolygon = true;
				int pointCount = ids->GetNumberOfIds();
				for (int pid = 0; cellIsInPolygon && pid < pointCount; ++pid){
					int pindex = cell->GetPointId(pid);
					double point[3];
					RawDataMapper::m_grid->vtkGrid()->GetPoint(pindex, point);
					// investigate whether the point is inside the polygon.
					// first use bounds for checking.
					if (point[0] < bounds[0] || // x < xmin
						point[0] > bounds[1] || // x > xmax
						point[1] < bounds[2] || // y < ymin
						point[1] > bounds[3])   // y > ymax
					{
						// this point is outside of polygon.
						cellIsInPolygon = false;
					} else {
						vtkIdType cellid;
						double pcoords[3];
						int subid;
						cellid = tmpgrid->FindCell(point, 0, 0, 1e-4, subid, pcoords, weights);
						bool found = (cellid >= 0);
						if (! found){
							// Not found, but if the grid is ugly, sometimes FindCell()
							// fails, even there is a cell that contains point.
							for (cellid = 0; ! found && cellid < tmpgrid->GetNumberOfCells(); ++cellid){
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
				if (cellIsInPolygon){
					s->ranges.append(i);
					*(boolMap + i) = true;
				}
			}
		}
		return s;
	}

	void map(bool* boolMap, RawDataMapperSetting* s)
	{
		RawDataPolygonCellMapperSetting* setting = dynamic_cast<RawDataPolygonCellMapperSetting*> (s);
		DA* da = dynamic_cast<DA*>(RawDataMapperT<V>::container()->dataArray());
		RawDataPolygon* polygon = dynamic_cast<RawDataPolygon* >(RawDataMapperT<V>::m_rawdata);
		V value = RawDataMapperT<V>::fromVariant(polygon->variantValue());
		const QList<IntegerRangeContainer::Range>& ranges = setting->ranges.ranges();
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

#endif // RAWDATAPOLYGONCELLMAPPERT_H

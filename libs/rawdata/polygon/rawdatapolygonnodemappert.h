#ifndef RAWDATAPOLYGONNODEMAPPERT_H
#define RAWDATAPOLYGONNODEMAPPERT_H

#include <guicore/pre/rawdata/rawdatanodemappert.h>
#include "rawdatapolygon.h"
#include <misc/integerrangecontainer.h>
#include <vtkCell.h>
#include <vtkDataSetTriangleFilter.h>
#include <vtkSmartPointer.h>

class RawDataPolygonNodeMapperSetting : public RawDataMapperSetting
{
public:
	RawDataPolygonNodeMapperSetting() : RawDataMapperSetting(){}
	virtual ~RawDataPolygonNodeMapperSetting(){}
	IntegerRangeContainer ranges;
};

template <class V, class DA>
class RawDataPolygonNodeMapperT : public RawDataNodeMapperT<V, DA>
{
public:
	RawDataPolygonNodeMapperT(RawDataCreator* parent)
		: RawDataNodeMapperT<V, DA>(parent)
	{
		RawDataNodeMapperT<V, DA>::m_caption = "Polygon node mapper";
	}
	RawDataMapperSetting* initialize(bool* boolMap)
	{
		RawDataPolygonNodeMapperSetting* s = new RawDataPolygonNodeMapperSetting();
		unsigned int count = RawDataMapperT<V>::container()->dataCount();
		RawDataPolygon* polygon = dynamic_cast<RawDataPolygon* >(RawDataMapper::m_rawdata);
		vtkUnstructuredGrid* tmpgrid = polygon->grid();
		double bounds[6];
		tmpgrid->GetBounds(bounds);
		double weights[3];
		for (unsigned int i = 0; i < count; ++i){
			if (! *(boolMap + i)){
				// not mapped yet.
				double point[3];
				RawDataMapper::m_grid->vtkGrid()->GetPoint(i, point);
				// investigate whether the point is inside the polygon.
				bool in;
				// first use bounds for checking.
				if (point[0] < bounds[0] || // x < xmin
					point[0] > bounds[1] || // x > xmax
					point[1] < bounds[2] || // y < ymin
					point[1] > bounds[3])   // y > ymax
				{
					// not in the polygon.
					in = false;
				} else {
					vtkIdType cellid;
					double pcoords[3];
					int subid;
					cellid = tmpgrid->FindCell(point, 0, 0, 1e-4, subid, pcoords, weights);
					in = (cellid >= 0);
					if (! in){
						// Not found, but if the grid is ugly, sometimes FindCell()
						// fails, even there is a cell that contains point.
						for (cellid = 0; ! in && cellid < tmpgrid->GetNumberOfCells(); ++cellid){
							vtkCell* tmpcell = tmpgrid->GetCell(cellid);
							double dist2;
							double closestPoint[3];
							int ret = tmpcell->EvaluatePosition(point, &(closestPoint[0]), subid, pcoords, dist2, weights);
							in = (ret == 1);
						}
					}

				}
				if (in){
					s->ranges.append(i);
					*(boolMap + i) = true;
				}
			}
		}
		return s;
	}

	void map(bool* boolMap, RawDataMapperSetting* s)
	{
		RawDataPolygonNodeMapperSetting* setting = dynamic_cast<RawDataPolygonNodeMapperSetting*> (s);
		DA* da = dynamic_cast<DA*>(RawDataMapperT<V>::container()->dataArray());
		RawDataPolygon* polygon = dynamic_cast<RawDataPolygon* >(RawDataMapperT<V>::m_rawdata);
		V value = RawDataMapperT<V>::fromVariant(polygon->variantValue());
		const QList<IntegerRangeContainer::Range>& ranges = setting->ranges.ranges();
		for (const IntegerRangeContainer::Range& r : ranges){
			for (unsigned int j = r.from; j <= r.to; ++j){
				if (*(boolMap + j) == false){
					da->SetValue(static_cast<vtkIdType>(j), value);
					*(boolMap + j) = true;
				}
			}
		}
		da->Modified();
	}
	void terminate(RawDataMapperSetting* setting)
	{
		delete setting;
	}
};

#endif // RAWDATAPOLYGONNODEMAPPERT_H

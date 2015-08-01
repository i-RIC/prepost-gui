#ifndef GEODATANETCDFCELLMAPPERT_H
#define GEODATANETCDFCELLMAPPERT_H

#include <guicore/pre/geodata/geodatacellmappert.h>
#include "geodatanetcdft.h"
#include <misc/doublemappingsetting.h>
#include <vtkCell.h>
#include <vtkSmartPointer.h>
#include <QSet>

class GeoDataNetcdfCellMapperSetting : public GeoDataMapperSetting
{

public:
	GeoDataNetcdfCellMapperSetting() : GeoDataMapperSetting() {}
	~GeoDataNetcdfCellMapperSetting() {}
	QList<DoubleMappingSetting> settings;
};

template <class V, class DA>
class GeoDataNetcdfCellMapperT : public GeoDataCellMapperT<V, DA>
{

public:
	GeoDataNetcdfCellMapperT(GeoDataCreator* parent) :
		GeoDataCellMapperT<V, DA> {parent}
	{
		GeoDataCellMapperT<V, DA>::m_caption = "Raster data cell mapper";
	}
	GeoDataMapperSetting* initialize(bool* boolMap) override {
		GeoDataNetcdfCellMapperSetting* s = new GeoDataNetcdfCellMapperSetting();
		unsigned int count = GeoDataCellMapperT<V, DA>::container()->dataCount();
		GeoDataNetcdfT<V, DA>* netcdf = dynamic_cast<GeoDataNetcdfT<V, DA>* >(GeoDataMapper::m_geodata);
		vtkStructuredGrid* tmpgrid = netcdf->grid();
		for (unsigned int i = 0; i < count; ++i) {
			if (*(boolMap + i)) {continue;}

			// not mapped yet.
			double point[3];
			double pointCenter[3];
			pointCenter[0] = pointCenter[1] = pointCenter[2] = 0;
			vtkCell* cell = GeoDataCellMapperT<V, DA>::m_grid->vtkGrid()->GetCell(i);
			for (int j = 0; j < cell->GetNumberOfPoints(); ++j) {
				GeoDataCellMapperT<V, DA>::m_grid->vtkGrid()->GetPoint(cell->GetPointId(j), point);
				for (int k = 0; k < 3; ++k) {
					pointCenter[k] += point[k];
				}
			}
			for (int k = 0; k < 3; ++k) {
				pointCenter[k] /= cell->GetNumberOfPoints();
			}
			// investigate whether the point is inside one of the cells.
			vtkIdType cellid;
			double pcoords[4];
			double weights[4];
			int subid;
			cellid = tmpgrid->FindCell(pointCenter, 0, 0, 1e-4, subid, pcoords, weights);
			if (cellid >= 0) {
				DoubleMappingSetting setting;
				setting.target = i;
				setting.indices.append(cellid);
				s->settings.append(setting);
			}
		}
		return s;
	}

	void map(bool* boolMap, GeoDataMapperSetting* s) override {
		GeoDataNetcdfCellMapperSetting* s2 =
			dynamic_cast<GeoDataNetcdfCellMapperSetting*>(s);
		DA* da = GeoDataCellMapperT<V, DA>::container()->dataArray();
		GeoDataNetcdfT<V, DA>* netcdf = dynamic_cast<GeoDataNetcdfT<V, DA>* >(GeoDataMapper::m_geodata);
		DA* vals = netcdf->vtkValues();
		V missingValue = netcdf->missingValue();
		for (int i = 0; i < s2->settings.size(); ++i) {
			const DoubleMappingSetting& setting = s2->settings.at(i);
			if (*(boolMap + setting.target) == false) {
				V value = vals->GetValue(setting.indices.at(0));
				if (value != missingValue) {
					da->SetValue(static_cast<vtkIdType>(setting.target), value);
					*(boolMap + setting.target) = true;
				}
			}
		}
		da->Modified();
	}

	void terminate(GeoDataMapperSetting* s) override {
		delete s;
	}
};

#endif // GEODATANETCDFCELLMAPPERT_H

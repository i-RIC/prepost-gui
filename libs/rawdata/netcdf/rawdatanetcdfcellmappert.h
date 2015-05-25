#ifndef RAWDATANETCDFCELLMAPPERT_H
#define RAWDATANETCDFCELLMAPPERT_H

#include <guicore/pre/rawdata/rawdatacellmappert.h>
#include "rawdatanetcdft.h"
#include <misc/doublemappingsetting.h>
#include <vtkCell.h>
#include <vtkSmartPointer.h>
#include <QSet>

class RawDataNetcdfCellMapperSetting : public RawDataMapperSetting
{
public:
	RawDataNetcdfCellMapperSetting() : RawDataMapperSetting() {}
	~RawDataNetcdfCellMapperSetting() {}
	QList<DoubleMappingSetting> settings;
};

template <class V, class DA>
class RawDataNetcdfCellMapperT : public RawDataCellMapperT<V, DA>
{
public:
	RawDataNetcdfCellMapperT(RawDataCreator* parent)
		: RawDataCellMapperT<V, DA>(parent) {
		RawDataCellMapperT<V, DA>::m_caption = "Raster data cell mapper";
	}
	RawDataMapperSetting* initialize(bool* boolMap) override {
		RawDataNetcdfCellMapperSetting* s = new RawDataNetcdfCellMapperSetting();
		unsigned int count = RawDataCellMapperT<V, DA>::container()->dataCount();
		RawDataNetcdfT<V, DA>* netcdf = dynamic_cast<RawDataNetcdfT<V, DA>* >(RawDataMapper::m_rawdata);
		vtkStructuredGrid* tmpgrid = netcdf->grid();
		for (unsigned int i = 0; i < count; ++i) {
			if (*(boolMap + i)) {continue;}

			// not mapped yet.
			double point[3];
			double pointCenter[3];
			pointCenter[0] = pointCenter[1] = pointCenter[2] = 0;
			vtkCell* cell = RawDataCellMapperT<V, DA>::m_grid->vtkGrid()->GetCell(i);
			for (int j = 0; j < cell->GetNumberOfPoints(); ++j) {
				RawDataCellMapperT<V, DA>::m_grid->vtkGrid()->GetPoint(cell->GetPointId(j), point);
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

	void map(bool* boolMap, RawDataMapperSetting* s) override {
		RawDataNetcdfCellMapperSetting* s2 =
			dynamic_cast<RawDataNetcdfCellMapperSetting*>(s);
		DA* da = RawDataCellMapperT<V, DA>::container()->dataArray();
		RawDataNetcdfT<V, DA>* netcdf = dynamic_cast<RawDataNetcdfT<V, DA>* >(RawDataMapper::m_rawdata);
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

	void terminate(RawDataMapperSetting* s) override {
		delete s;
	}
};

#endif // RAWDATANETCDFCELLMAPPERT_H


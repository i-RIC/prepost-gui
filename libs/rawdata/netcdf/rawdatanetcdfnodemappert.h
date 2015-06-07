#ifndef RAWDATANETCDFNODEMAPPERT_H
#define RAWDATANETCDFNODEMAPPERT_H

#include <guicore/pre/rawdata/rawdatanodemappert.h>
#include "rawdatanetcdft.h"
#include <guicore/solverdef/enumloader.h>
#include <misc/doublemappingsetting.h>
#include <vtkCell.h>
#include <vtkSmartPointer.h>
#include <QSet>

class RawDataNetcdfNodeMapperSetting : public RawDataMapperSetting
{

public:
	RawDataNetcdfNodeMapperSetting() : RawDataMapperSetting() {}
	~RawDataNetcdfNodeMapperSetting() {}
	QList<DoubleMappingSetting> settings;
};

template <class V, class DA>
class RawDataNetcdfNodeMapperT : public RawDataNodeMapperT<V, DA>
{

public:
	RawDataNetcdfNodeMapperT(RawDataCreator* parent)
		: RawDataNodeMapperT<V, DA>(parent) {
		RawDataNodeMapperT<V, DA>::m_caption = "Raster data node mapper";
	}
	RawDataMapperSetting* initialize(bool* boolMap) override {
		RawDataNetcdfNodeMapperSetting* s = new RawDataNetcdfNodeMapperSetting();
		unsigned int count = RawDataNodeMapperT<V, DA>::container()->dataCount();
		RawDataNetcdfT<V, DA>* netcdf = dynamic_cast<RawDataNetcdfT<V, DA>* >(RawDataMapper::m_rawdata);
		vtkStructuredGrid* tmpgrid = netcdf->grid();
		for (unsigned int i = 0; i < count; ++i) {
			if (*(boolMap + i)) {continue;}

			// not mapped yet.
			double point[3];
			RawDataMapper::m_grid->vtkGrid()->GetPoint(i, point);
			// investigate whether the point is inside one of the cells.
			vtkIdType cellid;
			double pcoords[4];
			double weights[4];
			int subid;
			cellid = tmpgrid->FindCell(point, 0, 0, 1e-4, subid, pcoords, weights);
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
		RawDataNetcdfNodeMapperSetting* s2 =
			dynamic_cast<RawDataNetcdfNodeMapperSetting*>(s);
		DA* da = RawDataNodeMapperT<V, DA>::container()->dataArray();
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

#endif // RAWDATANETCDFNODEMAPPERT_H

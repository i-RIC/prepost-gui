#ifndef GEODATANETCDFNODEMAPPERT_H
#define GEODATANETCDFNODEMAPPERT_H

#include <guicore/pre/geodata/geodatanodemappert.h>
#include "geodatanetcdft.h"
#include <guicore/solverdef/enumloader.h>
#include <misc/doublemappingsetting.h>
#include <vtkCell.h>
#include <vtkSmartPointer.h>
#include <QSet>

class GeoDataNetcdfNodeMapperSetting : public GeoDataMapperSetting
{

public:
	GeoDataNetcdfNodeMapperSetting() : GeoDataMapperSetting() {}
	~GeoDataNetcdfNodeMapperSetting() {}
	QList<DoubleMappingSetting> settings;
};

template <class V, class DA>
class GeoDataNetcdfNodeMapperT : public GeoDataNodeMapperT<V, DA>
{

public:
	GeoDataNetcdfNodeMapperT(GeoDataCreator* parent) :
		GeoDataNodeMapperT<V, DA> (parent)
	{
		GeoDataNodeMapperT<V, DA>::m_caption = "Raster data node mapper";
	}
	GeoDataMapperSetting* initialize(bool* boolMap) override {
		GeoDataNetcdfNodeMapperSetting* s = new GeoDataNetcdfNodeMapperSetting();
		unsigned int count = GeoDataNodeMapperT<V, DA>::container()->dataCount();
		GeoDataNetcdfT<V, DA>* netcdf = dynamic_cast<GeoDataNetcdfT<V, DA>* >(GeoDataMapper::m_geodata);
		vtkStructuredGrid* tmpgrid = netcdf->grid();
		for (unsigned int i = 0; i < count; ++i) {
			if (*(boolMap + i)) {continue;}

			// not mapped yet.
			double point[3];
			GeoDataMapper::m_grid->vtkGrid()->GetPoint(i, point);
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

	void map(bool* boolMap, GeoDataMapperSetting* s) override {
		GeoDataNetcdfNodeMapperSetting* s2 =
			dynamic_cast<GeoDataNetcdfNodeMapperSetting*>(s);
		DA* da = GeoDataNodeMapperT<V, DA>::container()->dataArray();
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

#endif // GEODATANETCDFNODEMAPPERT_H

#ifndef GEODATANETCDFCELLMAPPERT_DETAIL_H
#define GEODATANETCDFCELLMAPPERT_DETAIL_H

#include "../geodatanetcdfcellmappert.h"

#include <guicore/pre/geodata/geodatamappersettingi.h>
#include <misc/doublemappingsetting.h>

#include <vtkCell.h>

#include <vector>

class GeoDataNetcdfCellMapperSetting : public GeoDataMapperSettingI
{
public:
	std::vector<DoubleMappingSetting> settings;
};

template <class V, class DA>
GeoDataNetcdfCellMapperT<V, DA>::GeoDataNetcdfCellMapperT(GeoDataCreator* parent) :
	GeoDataCellMapperT<V, DA> ("Raster data cell mapper", parent)
{}

template <class V, class DA>
GeoDataMapperSettingI* GeoDataNetcdfCellMapperT<V, DA>::initialize(bool* boolMap)
{
	GeoDataNetcdfCellMapperSetting* s = new GeoDataNetcdfCellMapperSetting();
	unsigned int count = GeoDataCellMapperT<V, DA>::container()->dataCount();
	s->settings.reserve(count);
	GeoDataNetcdfT<V, DA>* netcdf = dynamic_cast<GeoDataNetcdfT<V, DA>* >(GeoDataMapper::geoData());
	vtkStructuredGrid* tmpgrid = netcdf->grid();
	for (unsigned int i = 0; i < count; ++i) {
		if (*(boolMap + i)) {continue;}

		// not mapped yet.
		double point[3];
		double pointCenter[3];
		pointCenter[0] = pointCenter[1] = pointCenter[2] = 0;
		vtkCell* cell = GeoDataCellMapperT<V, DA>::grid()->vtkGrid()->GetCell(i);
		for (int j = 0; j < cell->GetNumberOfPoints(); ++j) {
			GeoDataCellMapperT<V, DA>::grid()->vtkGrid()->GetPoint(cell->GetPointId(j), point);
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
			s->settings.push_back(setting);
		}
	}
	return s;
}

template <class V, class DA>
void GeoDataNetcdfCellMapperT<V, DA>::map(bool* boolMap, GeoDataMapperSettingI* s)
{
	GeoDataNetcdfCellMapperSetting* s2 =
		dynamic_cast<GeoDataNetcdfCellMapperSetting*>(s);
	DA* da = GeoDataCellMapperT<V, DA>::container()->dataArray();
	GeoDataNetcdfT<V, DA>* netcdf = dynamic_cast<GeoDataNetcdfT<V, DA>* >(GeoDataMapper::geoData());
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

template <class V, class DA>
void GeoDataNetcdfCellMapperT<V, DA>::terminate(GeoDataMapperSettingI* s)
{
	delete s;
}

#endif // GEODATANETCDFCELLMAPPERT_DETAIL_H

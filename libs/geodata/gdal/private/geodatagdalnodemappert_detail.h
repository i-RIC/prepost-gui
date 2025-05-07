#ifndef GEODATAGDALNODEMAPPERT_DETAIL_H
#define GEODATAGDALNODEMAPPERT_DETAIL_H

#include "../geodatagdalnodemappert.h"

#include <guibase/vtkpointsetextended/vtkpointsetextended.h>
#include <guicore/grid/v4grid.h>
#include <guicore/pre/grid/v4inputgrid.h>

#include <vector>

class GeoDataGdalNodeMapperSetting : public GeoDataMapperSettingI
{
public:
	std::vector<DoubleMappingSetting> settings;
};

template <class V, class DA>
GeoDataGdalNodeMapperT<V, DA>::GeoDataGdalNodeMapperT(GeoDataCreator* parent) :
	GeoDataNodeMapperT<V, DA> ("Raster data node mapper", parent)
{}

template <class V, class DA>
GeoDataMapperSettingI* GeoDataGdalNodeMapperT<V, DA>::initialize(bool* boolMap)
{
	GeoDataGdalNodeMapperSetting* s = new GeoDataGdalNodeMapperSetting();
	unsigned int count = GeoDataNodeMapperT<V, DA>::container()->dataCount();
	GeoDataGdalT<V, DA>* gdal = dynamic_cast<GeoDataGdalT<V, DA>* >(GeoDataMapper::geoData());
	vtkStructuredGrid* tmpgrid = gdal->grid();

	vtkPointSet* vtkGrid = GeoDataMapper::grid()->grid()->vtkData()->data();
	for (unsigned int i = 0; i < count; ++i) {
		if (*(boolMap + i)) {continue;}

		// not mapped yet.
		double point[3];
		vtkGrid->GetPoint(i, point);
		// investigate whether the point is inside one of the cells.
		vtkIdType cellid;
		double pcoords[4];
		double weights[4];
		int subid;
		cellid = tmpgrid->FindCell(point, 0, 0, 1e-4, subid, pcoords, weights);
		if (cellid >= 0) {
			DoubleMappingSetting setting;
			setting.target = i;
			setting.indices.push_back(cellid);
			s->settings.push_back(setting);
		}
	}
	return s;
}

template <class V, class DA>
void GeoDataGdalNodeMapperT<V, DA>::map(bool* boolMap, GeoDataMapperSettingI* s)
{
	GeoDataGdalNodeMapperSetting* s2 =
		dynamic_cast<GeoDataGdalNodeMapperSetting*>(s);
	DA* da = GeoDataNodeMapperT<V, DA>::container()->dataArray();
	GeoDataGdalT<V, DA>* gdal = dynamic_cast<GeoDataGdalT<V, DA>* >(GeoDataMapper::geoData());
	DA* vals = gdal->vtkValues();
	V missingValue = gdal->missingValue();
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
void GeoDataGdalNodeMapperT<V, DA>::terminate(GeoDataMapperSettingI* s)
{
	delete s;
}

#endif // GEODATAGDALNODEMAPPERT_DETAIL_H

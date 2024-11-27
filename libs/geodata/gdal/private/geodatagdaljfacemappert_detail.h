#ifndef GEODATANETCDFJFACEMAPPERT_DETAIL_H
#define GEODATANETCDFJFACEMAPPERT_DETAIL_H

#include "../geodatanetcdfjfacemappert.h"

#include <guibase/vtkpointsetextended/vtkpolydataextended2d.h>
#include <guibase/vtktool/vtkpointsutil.h>
#include <guicore/grid/v4structured2dgrid.h>
#include <guicore/pre/geodata/geodatamappersettingi.h>
#include <guicore/pre/grid/v4inputgrid.h>
#include <misc/doublemappingsetting.h>

#include <vector>

class GeoDataNetcdfJFaceMapperSetting : public GeoDataMapperSettingI
{
public:
	std::vector<DoubleMappingSetting> settings;
};

template <class V, class DA>
GeoDataNetcdfJFaceMapperT<V, DA>::GeoDataNetcdfJFaceMapperT(GeoDataCreator* parent) :
	GeoDataJFaceMapperT<V, DA> ("Raster data node mapper", parent)
{}

template <class V, class DA>
GeoDataMapperSettingI* GeoDataNetcdfJFaceMapperT<V, DA>::initialize(bool* boolMap)
{
	GeoDataNetcdfJFaceMapperSetting* s = new GeoDataNetcdfJFaceMapperSetting();
	unsigned int count = GeoDataJFaceMapperT<V, DA>::container()->dataCount();
	GeoDataNetcdfT<V, DA>* netcdf = dynamic_cast<GeoDataNetcdfT<V, DA>* >(GeoDataMapper::geoData());
	vtkStructuredGrid* tmpgrid = netcdf->grid();

	vtkPointSet* jfaceGrid = dynamic_cast<v4Structured2dGrid*> (GeoDataMapper::grid()->grid())->vtkJEdgeData()->data();
	double edgeCenter[3];

	for (unsigned int i = 0; i < count; ++i) {
		if (*(boolMap + i)) {continue;}

		// not mapped yet.
		auto point = vtkPointsUtil::getCenter(jfaceGrid->GetCell(i));
		edgeCenter[0] = point.x(); edgeCenter[1] = point.y(); edgeCenter[2] = 0;

		// investigate whether the point is inside one of the cells.
		vtkIdType cellid;
		double pcoords[4];
		double weights[4];
		int subid;
		cellid = tmpgrid->FindCell(edgeCenter, 0, 0, 1e-4, subid, pcoords, weights);
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
void GeoDataNetcdfJFaceMapperT<V, DA>::map(bool* boolMap, GeoDataMapperSettingI* s)
{
	GeoDataNetcdfJFaceMapperSetting* s2 =
		dynamic_cast<GeoDataNetcdfJFaceMapperSetting*>(s);
	DA* da = GeoDataJFaceMapperT<V, DA>::container()->dataArray();
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
void GeoDataNetcdfJFaceMapperT<V, DA>::terminate(GeoDataMapperSettingI* s)
{
	delete s;
}

#endif // GEODATANETCDFJFACEMAPPERT_DETAIL_H

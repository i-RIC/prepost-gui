#ifndef GEODATAGDALCELLMAPPERT_DETAIL_H
#define GEODATAGDALCELLMAPPERT_DETAIL_H

#include "../geodatagdalcellmappert.h"

#include <guibase/vtkpointsetextended/vtkpointsetextended.h>
#include <guibase/vtktool/vtkpointsutil.h>
#include <guicore/grid/v4grid.h>
#include <guicore/pre/geodata/geodatamappersettingi.h>
#include <guicore/pre/grid/v4inputgrid.h>
#include <misc/doublemappingsetting.h>

#include <vector>

class GeoDataGdalCellMapperSetting : public GeoDataMapperSettingI
{
public:
	std::vector<DoubleMappingSetting> settings;
};

template <class V, class DA>
GeoDataGdalCellMapperT<V, DA>::GeoDataGdalCellMapperT(GeoDataCreator* parent) :
	GeoDataCellMapperT<V, DA> ("Raster data cell mapper", parent)
{}

template <class V, class DA>
GeoDataMapperSettingI* GeoDataGdalCellMapperT<V, DA>::initialize(bool* boolMap)
{
	GeoDataGdalCellMapperSetting* s = new GeoDataGdalCellMapperSetting();
	unsigned int count = GeoDataCellMapperT<V, DA>::container()->dataCount();
	s->settings.reserve(count);
	GeoDataGdalT<V, DA>* gdal = dynamic_cast<GeoDataGdalT<V, DA>* >(GeoDataMapper::geoData());
	vtkStructuredGrid* tmpgrid = gdal->grid();

	vtkPointSet* vtkGrid = GeoDataMapper::grid()->grid()->vtkData()->data();
	double cellCenter[3];
	for (unsigned int i = 0; i < count; ++i) {
		if (*(boolMap + i)) {continue;}

		vtkCell* cell = vtkGrid->GetCell(i);
		QPointF point = vtkPointsUtil::getCenter(cell);
		cellCenter[0] = point.x(); cellCenter[1] = point.y(); cellCenter[2] = 0;
		// investigate whether the point is inside one of the cells.
		vtkIdType cellid;
		double pcoords[4];
		double weights[4];
		int subid;
		cellid = tmpgrid->FindCell(cellCenter, 0, 0, 1e-4, subid, pcoords, weights);
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
void GeoDataGdalCellMapperT<V, DA>::map(bool* boolMap, GeoDataMapperSettingI* s)
{
	GeoDataGdalCellMapperSetting* s2 =
		dynamic_cast<GeoDataGdalCellMapperSetting*>(s);
	DA* da = GeoDataCellMapperT<V, DA>::container()->dataArray();
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
void GeoDataGdalCellMapperT<V, DA>::terminate(GeoDataMapperSettingI* s)
{
	delete s;
}

#endif // GEODATAGDALCELLMAPPERT_DETAIL_H

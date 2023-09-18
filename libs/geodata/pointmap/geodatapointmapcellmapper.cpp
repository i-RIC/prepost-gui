#include "geodatapointmapcellmapper.h"

#include <guibase/vtkpointsetextended/vtkpointsetextended.h>
#include <guicore/grid/v4grid.h>
#include <guicore/pre/grid/v4inputgrid.h>

namespace {

DoubleMappingSetting setupCellSetting(unsigned int target, vtkCell* cell, double* weights)
{
	DoubleMappingSetting setting;
	setting.target = target;
	for (vtkIdType i = 0; i < cell->GetNumberOfPoints(); ++i) {
		vtkIdType vid = cell->GetPointId(i);
		setting.indices.push_back(vid);
		setting.weights.push_back(*(weights + i));
	}
	return setting;
}

bool isCenterInsideBounds(double point[3], double bounds[6])
{
	for (int i = 0; i < 3; ++i) {
		if (point[i] < bounds[i * 2]) {return false;}
		if (point[i] > bounds[i * 2 + 1]) {return false;}
	}
	return true;
}

} // namespace

GeoDataPointmapCellMapper::GeoDataPointmapCellMapper(GeoDataCreator* parent) :
	GeoDataCellMapperT<double, vtkDoubleArray>("Pointmap cell mapper", parent)
{}

GeoDataMapperSettingI* GeoDataPointmapCellMapper::initialize(bool* boolMap)
{
	GeoDataPointmapCellMapperSetting* s = new GeoDataPointmapCellMapperSetting();
	auto pointmap = dynamic_cast<GeoDataPointmap*>(geoData());
	pointmap->rebuildTinFromPointsIfNeeded();

	unsigned int count = container()->dataCount();
	vtkPolyData* tin = pointmap->tin();
	double* weights = new double[3];
	if (container()->gridAttribute()->isOption()) {
		// @todo not implemented yet.

	} else {
		double bounds[6];
		tin->GetBounds(bounds);

		vtkPointSet* vtkGrid = GeoDataMapper::grid()->grid()->vtkData()->data();
		for (unsigned int i = 0; i < count; ++i) {
			if (*(boolMap + i)) {continue;}

			// not mapped yet.
			double point[3];
			double pointCenter[3];
			pointCenter[0] = pointCenter[1] = pointCenter[2] = 0;
			vtkCell* cell = vtkGrid->GetCell(i);
			for (int j = 0; j < cell->GetNumberOfPoints(); ++j) {
				vtkGrid->GetPoint(cell->GetPointId(j), point);
				for (int k = 0; k < 3; ++k) {
					pointCenter[k] += point[k];
				}
			}
			for (int k = 0; k < 3; ++k) {
				pointCenter[k] /= cell->GetNumberOfPoints();
			}
			if (! isCenterInsideBounds(pointCenter, bounds)) {continue;}

			vtkCell* c = pointmap->findTinCell(pointCenter[0], pointCenter[1], weights);
			if (c != nullptr) {
				s->settings.push_back(setupCellSetting(i, c, weights));
			}
		}
	}
	delete[] weights;
	return s;
}

void GeoDataPointmapCellMapper::map(bool* boolMap, GeoDataMapperSettingI* s)
{
	GeoDataPointmapCellMapperSetting* s2 =
		dynamic_cast<GeoDataPointmapCellMapperSetting*>(s);
	vtkDoubleArray* da = container()->dataArray();
	auto pointmap = dynamic_cast<GeoDataPointmap*>(GeoDataMapper::geoData());
	vtkDoubleArray* vals = pointmap->tinValues();
	for (int i = 0; i < s2->settings.size(); ++i) {
		const DoubleMappingSetting& setting = s2->settings.at(i);
		if (*(boolMap + setting.target) == false) {
			double value = 0;
			for (int j = 0; j < setting.indices.size(); ++j) {
				value += setting.weights.at(j) * vals->GetValue(setting.indices.at(j));
			}
			da->SetValue(static_cast<vtkIdType>(setting.target), value);
			*(boolMap + setting.target) = true;
		}
	}
	da->Modified();
}

void GeoDataPointmapCellMapper::terminate(GeoDataMapperSettingI* s)
{
	delete s;
}

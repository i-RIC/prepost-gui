#include "geodatapointmap_tinnodemapper.h"

#include <guicore/pre/geodata/geodatamappersettingi.h>
#include <misc/doublemappingsetting.h>

namespace {

class Setting : public GeoDataMapperSettingI
{
public:
	std::vector<DoubleMappingSetting> settings;
};

DoubleMappingSetting setupSetting(unsigned int target, vtkCell* cell, double* weights)
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

bool isNodeInsideBounds(double point[3], double bounds[6])
{
	for (int i = 0; i < 3; ++i) {
		if (point[i] < bounds[i * 2]) {return false;}
		if (point[i] > bounds[i * 2 + 1]) {return false;}
	}
	return true;
}

} // namespace

GeoDataPointmap::TinNodeMapper::TinNodeMapper(QObject* parent) :
	GeoDataNodeMapperT<double, vtkDoubleArray> {"Pointmap node mapper with TIN", parent}
{}

GeoDataMapperSettingI* GeoDataPointmap::TinNodeMapper::initialize(bool* boolMap)
{
	auto s = new ::Setting();
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

		for (unsigned int i = 0; i < count; ++i) {
			if (*(boolMap + i)) {continue;}

			// not mapped yet.
			double point[3];
			GeoDataMapper::grid()->vtkGrid()->GetPoint(i, point);
			if (!isNodeInsideBounds(point, bounds)) {continue;}

			vtkCell* cell = pointmap->findTinCell(point[0], point[1], weights);
			if (cell != nullptr) {
				s->settings.push_back(setupSetting(i, cell, weights));
			}
		}
	}
	delete[] weights;
	return s;
}

void GeoDataPointmap::TinNodeMapper::map(bool* boolMap, GeoDataMapperSettingI* s)
{
	auto s2 = dynamic_cast<::Setting*>(s);
	auto da = container()->dataArray();
	auto pointmap = dynamic_cast<GeoDataPointmap*>(geoData());
	auto vals = pointmap->tinValues();
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

void GeoDataPointmap::TinNodeMapper::terminate(GeoDataMapperSettingI* s)
{
	delete s;
}
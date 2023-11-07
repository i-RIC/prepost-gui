#include "geodatariversurvey.h"
#include "geodatariversurveynodemapper.h"

#include <guicore/pre/geodata/geodatamappersettingi.h>
#include <misc/doublemappingsetting.h>

#include <vtkCell.h>
#include <vtkSmartPointer.h>

#include <set>

namespace {

void expandRegion(double* bounds)
{
	double rate = 3;

	double xWidth = *(bounds + 1) - *(bounds + 0);
	double yWidth = *(bounds + 3) - *(bounds + 2);
	double zWidth = *(bounds + 5) - *(bounds + 4);

	double xCenter = (*(bounds + 0) + *(bounds + 1)) * 0.5;
	double yCenter = (*(bounds + 2) + *(bounds + 3)) * 0.5;
	double zCenter = (*(bounds + 4) + *(bounds + 5)) * 0.5;

	*(bounds + 0) = xCenter - xWidth * 0.5 * rate;
	*(bounds + 1) = xCenter + xWidth * 0.5 * rate;
	*(bounds + 2) = yCenter - yWidth * 0.5 * rate;
	*(bounds + 3) = yCenter + yWidth * 0.5 * rate;
	*(bounds + 4) = zCenter - zWidth * 0.5 * rate;
	*(bounds + 5) = zCenter + zWidth * 0.5 * rate;
}

} // namespace

class GeoDataRiverSurveyNodeMapperSetting : public GeoDataMapperSettingI
{
public:
	GeoDataRiverSurveyNodeMapperSetting() : GeoDataMapperSettingI() {}
	~GeoDataRiverSurveyNodeMapperSetting() {}

	QList<DoubleMappingSetting> settings;
};

GeoDataRiverSurveyNodeMapper::GeoDataRiverSurveyNodeMapper(GeoDataCreator* parent) :
	GeoDataNodeMapperT<double, vtkDoubleArray>("Cross-section data node mapper", parent)
{}

GeoDataMapperSettingI* GeoDataRiverSurveyNodeMapper::initialize(bool* boolMap)
{
	GeoDataRiverSurveyNodeMapperSetting* s = new GeoDataRiverSurveyNodeMapperSetting();

	unsigned int count = GeoDataNodeMapperT<double, vtkDoubleArray>::container()->dataCount();
	GeoDataRiverSurvey* rs = dynamic_cast<GeoDataRiverSurvey*>(GeoDataMapper::geoData());
	double point[3];
	double weights[4];
	auto grid = GeoDataMapper::grid()->vtkGrid();
	for (unsigned int i = 0; i < count; ++i) {
		if (! *(boolMap + i)) {
			// not mapped yet.
			grid->GetPoint(i, point);

			// investigate whether the point is inside one of the cells.
			auto cell = rs->findBackgroundGridCell(point[0], point[1], weights);
			if (cell != nullptr) {
				DoubleMappingSetting setting;
				setting.target = i;
				for (vtkIdType j = 0; j < cell->GetNumberOfPoints(); ++j) {
					vtkIdType vid = cell->GetPointId(j);
					setting.indices.push_back(vid);
					setting.weights.push_back(weights[j]);
				}
				s->settings.append(setting);
				*(boolMap + i) = true;
			}
		}
	}
	return s;
}

void GeoDataRiverSurveyNodeMapper::map(bool* boolMap, GeoDataMapperSettingI* s)
{
	GeoDataRiverSurveyNodeMapperSetting* s2 =
		dynamic_cast<GeoDataRiverSurveyNodeMapperSetting*>(s);
	vtkDoubleArray* da = GeoDataNodeMapperT<double, vtkDoubleArray>::container()->dataArray();
	auto rs = dynamic_cast<GeoDataRiverSurvey*>(GeoDataNodeMapperT<double, vtkDoubleArray>::geoData());
	auto grid = rs->backgroundGrid();
	auto vals = vtkDoubleArray::SafeDownCast(grid->GetPointData()->GetArray("Data"));
	for (const auto& setting : s2->settings) {
		if (*(boolMap + setting.target) == false) {
			double value = 0;
			for (int j = 0; j < setting.indices.size(); ++j) {
				value += setting.weights.at(j) * vals->GetValue(setting.indices.at(j));
			}
			da->SetValue(setting.target, value);
			*(boolMap + setting.target) = true;
		}
	}
	da->Modified();
}

void GeoDataRiverSurveyNodeMapper::terminate(GeoDataMapperSettingI* s)
{
	delete s;
}

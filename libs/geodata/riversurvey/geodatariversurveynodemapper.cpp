#include "geodatariversurveynodemapper.h"

#include <guicore/pre/geodata/geodatamappersettingi.h>

class GeoDataRiverSurveyNodeMapperSetting : public GeoDataMapperSettingI
{
public:
	GeoDataRiverSurveyNodeMapperSetting() : GeoDataMapperSettingI() {}
	~GeoDataRiverSurveyNodeMapperSetting() {}

	QList<DoubleMappingSetting> settings;
};

GeoDataRiverSurveyNodeMapper::GeoDataRiverSurveyNodeMapper(GeoDataCreator* parent) :
	GeoDataNodeMapperT<double, vtkDoubleArray>("River survey node mapper", parent)
{}

GeoDataMapperSettingI* GeoDataRiverSurveyNodeMapper::initialize(bool* boolMap)
{
	GeoDataRiverSurveyNodeMapperSetting* s = new GeoDataRiverSurveyNodeMapperSetting();

	unsigned int count = GeoDataNodeMapperT<double, vtkDoubleArray>::container()->dataCount();
	GeoDataRiverSurvey* rs = dynamic_cast<GeoDataRiverSurvey*>(GeoDataMapper::geoData());
	vtkStructuredGrid* grid = rs->backgroundGrid();
	double weights[4];
	for (unsigned int i = 0; i < count; ++i) {
		if (! *(boolMap + i)) {
			// not mapped yet.
			double point[3];
			GeoDataMapper::grid()->vtkGrid()->GetPoint(i, point);
			// investigate whether the point is inside one of the cells.
			vtkIdType cellid;
			double pcoords[3];
			int subid;
			cellid = grid->FindCell(point, 0, 0, 1e-4, subid, pcoords, weights);
			if (cellid >= 0) {
				// calculate the value, using the value and weights.
				vtkCell* cell = grid->GetCell(cellid);
				DoubleMappingSetting setting;
				setting.target = i;
				for (vtkIdType j = 0; j < cell->GetNumberOfPoints(); ++j) {
					vtkIdType vid = cell->GetPointId(j);
					setting.indices.append(vid);
					setting.weights.append(weights[j]);
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
	GeoDataRiverSurvey* rs = dynamic_cast<GeoDataRiverSurvey*>(GeoDataNodeMapperT<double, vtkDoubleArray>::geoData());
	vtkStructuredGrid* grid = rs->backgroundGrid();
	vtkDoubleArray* vals = vtkDoubleArray::SafeDownCast(grid->GetPointData()->GetArray("Data"));
	for (const DoubleMappingSetting& setting : s2->settings) {
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

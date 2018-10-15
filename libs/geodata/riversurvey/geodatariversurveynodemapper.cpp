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
			} else {
				// wider region search.
				// first, get the region that the cells that uses this point occupies.
				vtkSmartPointer<vtkIdList> cells = vtkSmartPointer<vtkIdList>::New();
				GeoDataMapper::grid()->vtkGrid()->GetPointCells(i, cells);
				double bounds[6];
				for (vtkIdType j = 0; j < cells->GetNumberOfIds(); ++j) {
					vtkIdType cellid = cells->GetId(j);
					double tmpbounds[6];
					GeoDataMapper::grid()->vtkGrid()->GetCellBounds(cellid, tmpbounds);
					for (int k = 0; k < 3; ++k) {
						if (j == 0 || tmpbounds[k * 2] < bounds[k * 2]) {
							bounds[k * 2] = tmpbounds[k * 2];
						}
						if (j == 0 || tmpbounds[k * 2 + 1] > bounds[k * 2 + 1]) {
							bounds[k * 2 + 1] = tmpbounds[k * 2 + 1];
						}
					}
				}
				expandRegion(&(bounds[0]));
				// try all cells that has points within bounds.
				std::set<vtkIdType> cellsToTry;
				for (vtkIdType pId = 0; pId < grid->GetNumberOfPoints(); ++pId) {
					double tmppoint[3];
					grid->GetPoint(pId, tmppoint);
					if (tmppoint[0] < bounds[0]) { continue; }
					if (tmppoint[0] > bounds[1]) { continue; }
					if (tmppoint[1] < bounds[2]) { continue; }
					if (tmppoint[1] > bounds[3]) { continue; }
					vtkSmartPointer<vtkIdList> tmpCells = vtkSmartPointer<vtkIdList>::New();
					grid->GetPointCells(pId, tmpCells);
					for (vtkIdType j = 0; j < tmpCells->GetNumberOfIds(); ++j) {
						vtkIdType cellid = tmpCells->GetId(j);
						if (cellsToTry.find(cellid) != cellsToTry.end()) { continue; }
						cellsToTry.insert(cellid);
					}
				}
				// try to find the cell using cellsToTry.
				for (vtkIdType probeCellid : cellsToTry) {
					double pcoords[3];
					int subid;
					vtkCell* probeCell = grid->GetCell(probeCellid);
					vtkIdType cellid = grid->FindCell(point, probeCell, 0, 1e-4, subid, pcoords, weights);
					if (cellid >= 0) {
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
						break;
					}
				}
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

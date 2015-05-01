#include "rawdatariversurveynodemapper.h"

class RawDataRiverSurveyNodeMapperSetting : public RawDataMapperSetting
{
public:
	RawDataRiverSurveyNodeMapperSetting() : RawDataMapperSetting() {}
	~RawDataRiverSurveyNodeMapperSetting() {}
	QList<DoubleMappingSetting> settings;
};

RawDataMapperSetting* RawDataRiverSurveyNodeMapper::initialize(bool* boolMap)
{
	RawDataRiverSurveyNodeMapperSetting* s = new RawDataRiverSurveyNodeMapperSetting();

	unsigned int count = RawDataNodeMapperT<double, vtkDoubleArray>::container()->dataCount();
	RawDataRiverSurvey* rs = dynamic_cast<RawDataRiverSurvey*>(RawDataMapper::m_rawdata);
	vtkStructuredGrid* grid = rs->backgroundGrid();
	double* weights = new double[4];
	for (unsigned int i = 0; i < count; ++i) {
		if (! *(boolMap + i)) {
			// not mapped yet.
			double point[3];
			m_grid->vtkGrid()->GetPoint(i, point);
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
					setting.weights.append(*(weights + j));
				}
				s->settings.append(setting);
				*(boolMap + i) = true;
			}
		}
	}
	delete weights;
	return s;
}

void RawDataRiverSurveyNodeMapper::map(bool* boolMap, RawDataMapperSetting* s)
{
	RawDataRiverSurveyNodeMapperSetting* s2 =
		dynamic_cast<RawDataRiverSurveyNodeMapperSetting*>(s);
	vtkDoubleArray* da = RawDataNodeMapperT<double, vtkDoubleArray>::container()->dataArray();
	RawDataRiverSurvey* rs = dynamic_cast<RawDataRiverSurvey*>(RawDataNodeMapperT<double, vtkDoubleArray>::m_rawdata);
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

void RawDataRiverSurveyNodeMapper::terminate(RawDataMapperSetting* s)
{
	delete s;
}

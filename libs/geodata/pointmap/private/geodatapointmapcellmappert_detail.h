#ifndef GEODATAPOINTMAPCELLMAPPERT_DETAIL_H
#define GEODATAPOINTMAPCELLMAPPERT_DETAIL_H

#include "../geodatapointmapcellmappert.h"

#include <QSet>

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

template <class V, class DA>
GeoDataPointmapCellMapperT<V, DA>::GeoDataPointmapCellMapperT(GeoDataCreator* parent) :
	GeoDataCellMapperT<V, DA>("Pointmap cell mapper", parent)
{}

template <class V, class DA>
GeoDataMapperSettingI* GeoDataPointmapCellMapperT<V, DA>::initialize(bool* boolMap)
{
	GeoDataPointmapCellMapperSetting* s = new GeoDataPointmapCellMapperSetting();
	unsigned int count = GeoDataCellMapperT<V, DA>::container()->dataCount();
	GeoDataPointMapT<V, DA>* pointmap = dynamic_cast<GeoDataPointMapT<V, DA>* >(GeoDataCellMapperT<V, DA>::geoData());
	if (pointmap->needRemeshing()) {
		pointmap->remeshTINS(true);
	}
	vtkPolyData* tmpgrid = pointmap->delaunayedPolyData();
	double* weights = new double[3];
	if (GeoDataCellMapperT<V, DA>::container()->gridAttribute()->isOption()) {
		// @todo not implemented yet.

	} else {
		double bounds[6];
		tmpgrid->GetBounds(bounds);

		for (unsigned int i = 0; i < count; ++i) {
			if (*(boolMap + i)) {continue;}

			// not mapped yet.
			double point[3];
			double pointCenter[3];
			pointCenter[0] = pointCenter[1] = pointCenter[2] = 0;
			vtkCell* cell = GeoDataCellMapperT<V, DA>::grid()->vtkGrid()->GetCell(i);
			for (int j = 0; j < cell->GetNumberOfPoints(); ++j) {
				GeoDataMapper::grid()->vtkGrid()->GetPoint(cell->GetPointId(j), point);
				for (int k = 0; k < 3; ++k) {
					pointCenter[k] += point[k];
				}
			}
			for (int k = 0; k < 3; ++k) {
				pointCenter[k] /= cell->GetNumberOfPoints();
			}
			if (! isCenterInsideBounds(pointCenter, bounds)) {continue;}

			vtkCell* c = pointmap->findCell(pointCenter[0], pointCenter[1], weights);
			if (c != nullptr) {
				s->settings.push_back(setupCellSetting(i, c, weights));
			}
		}
	}
	delete weights;
	return s;
}

template <class V, class DA>
void GeoDataPointmapCellMapperT<V, DA>::map(bool* boolMap, GeoDataMapperSettingI* s)
{
	GeoDataPointmapCellMapperSetting* s2 =
		dynamic_cast<GeoDataPointmapCellMapperSetting*>(s);
	DA* da = GeoDataCellMapperT<V, DA>::container()->dataArray();
	GeoDataPointMapT<V, DA>* pointmap = dynamic_cast<GeoDataPointMapT<V, DA>* >(GeoDataMapper::geoData());
	DA* vals = pointmap->vtkValues();
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

template <class V, class DA>
void GeoDataPointmapCellMapperT<V, DA>::terminate(GeoDataMapperSettingI* s)
{
	delete s;
}

#endif // GEODATAPOINTMAPCELLMAPPERT_DETAIL_H

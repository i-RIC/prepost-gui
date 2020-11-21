#ifndef GEODATAPOINTMAPNODEMAPPERT_DETAIL_H
#define GEODATAPOINTMAPNODEMAPPERT_DETAIL_H

#include "../geodatapointmapnodemappert.h"

#include <QSet>

namespace {

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

template <class V, class DA>
GeoDataPointmapNodeMapperT<V, DA>::GeoDataPointmapNodeMapperT(GeoDataCreator* parent) :
	GeoDataNodeMapperT<V, DA>("Pointmap node mapper", parent)
{}

template <class V, class DA>
GeoDataMapperSettingI* GeoDataPointmapNodeMapperT<V, DA>::initialize(bool* boolMap)
{
	GeoDataPointmapNodeMapperSetting* s = new GeoDataPointmapNodeMapperSetting();
	unsigned int count = GeoDataNodeMapperT<V, DA>::container()->dataCount();
	GeoDataPointMapT<V, DA>* pointmap = dynamic_cast<GeoDataPointMapT<V, DA>* >(GeoDataNodeMapperT<V, DA>::geoData());
	if (pointmap->needRemeshing()) {
		pointmap->remeshTINS(true);
	}
	vtkPolyData* tmpgrid = pointmap->delaunayedPolyData();
	double* weights = new double[3];
	if (GeoDataNodeMapperT<V, DA>::container()->gridAttribute()->isOption()) {
		// @todo not implemented yet.


	} else {
		double bounds[6];
		tmpgrid->GetBounds(bounds);

		for (unsigned int i = 0; i < count; ++i) {
			if (*(boolMap + i)) {continue;}

			// not mapped yet.
			double point[3];
			GeoDataMapper::grid()->vtkGrid()->GetPoint(i, point);
			if (!isNodeInsideBounds(point, bounds)) { continue; }

			vtkCell* cell = pointmap->findCell(point[0], point[1], weights);
			if (cell != nullptr) {
				s->settings.push_back(setupSetting(i, cell, weights));
			}
		}
	}
	delete weights;
	return s;
}

template <class V, class DA>
void GeoDataPointmapNodeMapperT<V, DA>::map(bool* boolMap, GeoDataMapperSettingI* s)
{
	GeoDataPointmapNodeMapperSetting* s2 = dynamic_cast<GeoDataPointmapNodeMapperSetting*>(s);
	DA* da = GeoDataNodeMapperT<V, DA>::container()->dataArray();
	GeoDataPointMapT<V, DA>* pointmap = dynamic_cast<GeoDataPointMapT<V, DA>* >(GeoDataNodeMapperT<V, DA>::geoData());
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
void GeoDataPointmapNodeMapperT<V, DA>::terminate(GeoDataMapperSettingI* s)
{
	delete s;
}

#endif // GEODATAPOINTMAPNODEMAPPERT_DETAIL_H

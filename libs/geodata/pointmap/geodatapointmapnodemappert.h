#ifndef GEODATAPOINTMAPNODEMAPPERT_H
#define GEODATAPOINTMAPNODEMAPPERT_H

#include "geodatapointmapt.h"

#include <guicore/pre/geodata/geodatanodemappert.h>
#include <misc/doublemappingsetting.h>

#include <QSet>

#include <vtkCell.h>
#include <vtkSmartPointer.h>

class GeoDataPointmapNodeMapperSetting : public GeoDataMapperSetting
{

public:
	GeoDataPointmapNodeMapperSetting() : GeoDataMapperSetting() {}
	~GeoDataPointmapNodeMapperSetting() {}
	QList<DoubleMappingSetting> settings;
};

template <class V, class DA>
class GeoDataPointmapNodeMapperT : public GeoDataNodeMapperT<V, DA>
{

public:
	GeoDataPointmapNodeMapperT(GeoDataCreator* parent)
		: GeoDataNodeMapperT<V, DA>(parent) {
		GeoDataNodeMapperT<V, DA>::m_caption = "Pointmap node mapper";
	}
	GeoDataMapperSetting* initialize(bool* boolMap) override {
		GeoDataPointmapNodeMapperSetting* s = new GeoDataPointmapNodeMapperSetting();
		unsigned int count = GeoDataNodeMapperT<V, DA>::container()->dataCount();
		GeoDataPointMapT<V, DA>* pointmap = dynamic_cast<GeoDataPointMapT<V, DA>* >(GeoDataNodeMapperT<V, DA>::m_geodata);
		if (pointmap->needRemeshing()) {
			pointmap->remeshTINS(true);
		}
		vtkPolyData* tmpgrid = pointmap->delaunayedPolyData();
		double* weights = new double[3];
		if (GeoDataNodeMapperT<V, DA>::container()->gridAttribute()->isOption()) {
			// @todo not implemented yet.


		} else {
			for (unsigned int i = 0; i < count; ++i) {
				if (*(boolMap + i)) {continue;}

				// not mapped yet.
				double point[3];
				GeoDataMapper::m_grid->vtkGrid()->GetPoint(i, point);
				// investigate whether the point is inside one of the cells.
				vtkIdType cellid;
				double pcoords[3];
				int subid;
				cellid = tmpgrid->FindCell(point, 0, 0, 1e-4, subid, pcoords, weights);
				if (cellid >= 0) {
					DoubleMappingSetting setting;
					setting.target = i;
					vtkCell* cell = tmpgrid->GetCell(cellid);
					for (vtkIdType j = 0; j < cell->GetNumberOfPoints(); ++j) {
						vtkIdType vid = cell->GetPointId(j);
						setting.indices.append(vid);
						setting.weights.append(*(weights + j));
					}
					s->settings.append(setting);
					*(boolMap + i) = true;
				} else {
					// wider region search.
					// first, get the region that the cells that uses this point occupies.
					vtkSmartPointer<vtkIdList> cells = vtkSmartPointer<vtkIdList>::New();
					GeoDataNodeMapperT<V, DA>::m_grid->vtkGrid()->GetPointCells(i, cells);
					double bounds[6];
					for (vtkIdType j = 0; j < cells->GetNumberOfIds(); ++j) {
						vtkIdType cellid = cells->GetId(j);
						double tmpbounds[6];
						GeoDataNodeMapperT<V, DA>::m_grid->vtkGrid()->GetCellBounds(cellid, tmpbounds);
						for (int k = 0; k < 3; ++k) {
							if (j == 0 || tmpbounds[k * 2] < bounds[k * 2]) {
								bounds[k * 2] = tmpbounds[k * 2];
							}
							if (j == 0 || tmpbounds[k * 2 + 1] > bounds[k * 2 + 1]) {
								bounds[k * 2 + 1] = tmpbounds[k * 2 + 1];
							}
						}
					}
					// try all cells that has points within bounds.
					QSet<vtkIdType> cellsToTry;
					QSet<vtkIdType> triedPoints;
					QSet<vtkIdType> cPoints;
					vtkIdType nearestpoint = tmpgrid->FindPoint(point);
					if (nearestpoint < 0) {
						// no point found.
						continue;
					}
					cPoints.insert(nearestpoint);
					while (cPoints.count() > 0) {
						QSet<vtkIdType> nextCPoints;
						foreach(vtkIdType point, cPoints) {
							if (triedPoints.contains(point)) {continue;}
							vtkSmartPointer<vtkIdList> tmpCells = vtkSmartPointer<vtkIdList>::New();
							tmpgrid->GetPointCells(point, tmpCells);

							for (vtkIdType j = 0; j < tmpCells->GetNumberOfIds(); ++j) {
								vtkIdType cellid = tmpCells->GetId(j);
								if (cellsToTry.contains(cellid)) {continue;}
								cellsToTry.insert(cellid);
								vtkSmartPointer<vtkIdList> tmpPoints = vtkSmartPointer<vtkIdList>::New();
								tmpgrid->GetCellPoints(cellid, tmpPoints);
								for (vtkIdType k = 0; k < tmpPoints->GetNumberOfIds(); ++k) {
									vtkIdType pointid = tmpPoints->GetId(k);
									if (triedPoints.contains(pointid)) {continue;}
									double tmppoint[3];
									tmpgrid->GetPoint(pointid, tmppoint);
									if (tmppoint[0] < bounds[0]) {continue;}
									if (tmppoint[0] > bounds[1]) {continue;}
									if (tmppoint[1] < bounds[2]) {continue;}
									if (tmppoint[1] > bounds[3]) {continue;}
									nextCPoints.insert(pointid);
								}
							}
							triedPoints.insert(point);
						}
						cPoints = nextCPoints;
					}

					// try to find the cell using cellsToTry.
					bool found = false;
					foreach(vtkIdType probeCellid, cellsToTry) {
						if (found) {continue;}

						double pcoords[3];
						int subid;
						vtkCell* probeCell = tmpgrid->GetCell(probeCellid);
						vtkIdType cellid = tmpgrid->FindCell(point, probeCell, 0, 1e-4, subid, pcoords, weights);
						if (cellid >= 0) {
							vtkCell* cell = tmpgrid->GetCell(cellid);
							DoubleMappingSetting setting;
							setting.target = i;
							for (vtkIdType j = 0; j < cell->GetNumberOfPoints(); ++j) {
								vtkIdType vid = cell->GetPointId(j);
								setting.indices.append(vid);
								setting.weights.append(*(weights + j));
							}
							s->settings.append(setting);
							*(boolMap + i) = true;
							found = true;
						}
					}
				}
			}
		}
		delete weights;
		return s;
	}

	void map(bool* boolMap, GeoDataMapperSetting* s) override {
		GeoDataPointmapNodeMapperSetting* s2 = dynamic_cast<GeoDataPointmapNodeMapperSetting*>(s);
		DA* da = GeoDataNodeMapperT<V, DA>::container()->dataArray();
		GeoDataPointMapT<V, DA>* pointmap = dynamic_cast<GeoDataPointMapT<V, DA>* >(GeoDataNodeMapperT<V, DA>::m_geodata);
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

	void terminate(GeoDataMapperSetting* s) override {
		delete s;
	}
};

#endif // GEODATAPOINTMAPNODEMAPPERT_H

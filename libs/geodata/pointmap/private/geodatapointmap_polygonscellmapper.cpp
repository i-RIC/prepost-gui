#include "geodatapointmap_polygonscellmapper.h"

#include <guibase/vtkpointsetextended/vtkpointsetextended.h>
#include <guicore/grid/v4grid.h>
#include <guicore/pre/grid/v4inputgrid.h>

GeoDataPointmap::PolygonsCellMapper::PolygonsCellMapper(QObject* parent) :
	GeoDataCellMapperT<double, vtkDoubleArray>("Pointmap cell mapper with Polygons", parent)
{}

GeoDataMapperSettingI* GeoDataPointmap::PolygonsCellMapper::initialize(bool* /*boolMap*/)
{
	return nullptr;
}

void GeoDataPointmap::PolygonsCellMapper::map(bool* boolMap, GeoDataMapperSettingI* /*s*/)
{
	auto pointmap = dynamic_cast<GeoDataPointmap*>(geoData());
	auto da = container()->dataArray();
	pointmap->rebuildTinFromPointsIfNeeded();

	unsigned int count = container()->dataCount();
	if (container()->gridAttribute()->isOption()) {
		// @todo not implemented yet.

	} else {
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
			double value;
			bool ok = pointmap->mapWithPolygons(pointCenter[0], pointCenter[1], &value);
			if (ok) {
				da->SetValue(i, value);
				*(boolMap + i) = true;
			}
		}
	}
}

void GeoDataPointmap::PolygonsCellMapper::terminate(GeoDataMapperSettingI* /*s*/)
{}

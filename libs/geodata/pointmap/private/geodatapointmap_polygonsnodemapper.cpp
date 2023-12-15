#include "geodatapointmap_impl.h"
#include "geodatapointmap_polygonsmanager.h"
#include "geodatapointmap_polygonsnodemapper.h"

#include <guibase/vtkpointsetextended/vtkpointsetextended.h>
#include <guicore/grid/v4grid.h>
#include <guicore/pre/grid/v4inputgrid.h>

GeoDataPointmap::PolygonsNodeMapper::PolygonsNodeMapper(QObject* parent) :
	GeoDataNodeMapperT<double, vtkDoubleArray>("Pointmap node mapper with Polygons", parent)
{}

GeoDataMapperSettingI* GeoDataPointmap::PolygonsNodeMapper::initialize(bool* /*boolMap*/)
{
	return nullptr;
}

void GeoDataPointmap::PolygonsNodeMapper::map(bool* boolMap, GeoDataMapperSettingI* /*s*/)
{
	auto pointmap = dynamic_cast<GeoDataPointmap*>(geoData());
	pointmap->impl->m_polygonsManager.updatePolygonOrder();

	auto da = container()->dataArray();
	pointmap->rebuildTinFromPointsIfNeeded();
	pointmap->mergeEditTargetPolygon();

	unsigned int count = container()->dataCount();
	if (container()->gridAttribute()->isOption()) {
		// @todo not implemented yet.

	} else {
		vtkPointSet* vtkGrid = GeoDataMapper::grid()->grid()->vtkData()->data();
		for (unsigned int i = 0; i < count; ++i) {
			if (*(boolMap + i)) {continue;}

			// not mapped yet.
			double point[3];
			vtkGrid->GetPoint(i, point);
			double value;
			bool ok = pointmap->mapWithPolygons(point[0], point[1], &value);
			if (ok) {
				da->SetValue(i, value);
				*(boolMap + i) = true;
			}
		}
	}
	da->Modified();
}

void GeoDataPointmap::PolygonsNodeMapper::terminate(GeoDataMapperSettingI* /*s*/)
{}

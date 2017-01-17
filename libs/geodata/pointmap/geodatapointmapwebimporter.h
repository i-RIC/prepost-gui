#ifndef GEODATAPOINTMAPWEBIMPORTER_H
#define GEODATAPOINTMAPWEBIMPORTER_H

#include "gd_pointmap_global.h"
#include <guicore/pre/geodata/geodatawebimporter.h>

class CoordinateSystem;

class GD_POINTMAP_EXPORT GeoDataPointmapWebImporter : public GeoDataWebImporter
{
	Q_OBJECT

public:
	GeoDataPointmapWebImporter(GeoDataCreator* creator);

	bool isCompatibleWith(SolverDefinitionGridAttribute* condition) const override;

	bool importData(GeoData* data, int index, QWidget* w) override;

private:
	bool doInit(int* count, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemInterface* item, QWidget* w) override;

	CoordinateSystem* m_coordinateSystem;
};

#endif // GEODATAPOINTMAPWEBIMPORTER_H

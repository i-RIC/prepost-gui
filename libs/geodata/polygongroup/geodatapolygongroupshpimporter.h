#ifndef GEODATAPOLYGONGROUPSHPIMPORTER_H
#define GEODATAPOLYGONGROUPSHPIMPORTER_H

#include <geodata/polydatagroup/geodatapolydatagroupshpimporter.h>

class GeoDataPolygonGroupShpImporter : public GeoDataPolyDataGroupShpImporter
{
	Q_OBJECT

public:
	GeoDataPolygonGroupShpImporter(GeoDataCreator* creator);

	const QStringList fileDialogFilters() override;
	bool importData(GeoData* data, int index, QWidget* w) override;
};

#endif // GEODATAPOLYGONGROUPSHPIMPORTER_H

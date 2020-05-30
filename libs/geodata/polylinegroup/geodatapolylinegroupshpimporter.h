#ifndef GEODATAPOLYLINEGROUPSHPIMPORTER_H
#define GEODATAPOLYLINEGROUPSHPIMPORTER_H

#include <geodata/polydatagroup/geodatapolydatagroupshpimporter.h>

class GeoDataPolyLineGroupShpImporter : public GeoDataPolyDataGroupShpImporter
{
	Q_OBJECT

public:
	GeoDataPolyLineGroupShpImporter(GeoDataCreator* creator);

	const QStringList fileDialogFilters() override;
	bool importData(GeoData* data, int index, QWidget* w) override;
};

#endif // GEODATAPOLYLINEGROUPSHPIMPORTER_H

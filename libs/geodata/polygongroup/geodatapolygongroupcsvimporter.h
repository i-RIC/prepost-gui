#ifndef GEODATAPOLYGONGROUPCSVIMPORTER_H
#define GEODATAPOLYGONGROUPCSVIMPORTER_H

#include <guicore/pre/geodata/geodataimporter.h>

class GeoDataPolygonGroupCsvImporter : public GeoDataImporter
{
public:
	GeoDataPolygonGroupCsvImporter(GeoDataCreator* creator);

	const QStringList fileDialogFilters() override;
	const QStringList acceptableExtensions() override;

	bool importData(GeoData* data, int index, QWidget* w) override;
};

#endif // GEODATAPOLYGONGROUPCSVIMPORTER_H

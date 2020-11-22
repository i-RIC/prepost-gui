#ifndef GEODATAPOLYLINEGROUPCSVIMPORTER_H
#define GEODATAPOLYLINEGROUPCSVIMPORTER_H

#include <guicore/pre/geodata/geodataimporter.h>

class GeoDataPolyLineGroupCsvImporter : public GeoDataImporter
{
public:
	GeoDataPolyLineGroupCsvImporter(GeoDataCreator* creator);

	const QStringList fileDialogFilters() override;
	const QStringList acceptableExtensions() override;

	bool importData(GeoData* data, int index, QWidget* w) override;
};

#endif // GEODATAPOLYLINEGROUPCSVIMPORTER_H

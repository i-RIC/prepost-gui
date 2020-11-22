#ifndef GEODATAPOINTGROUPCSVIMPORTER_H
#define GEODATAPOINTGROUPCSVIMPORTER_H

#include <guicore/pre/geodata/geodataimporter.h>

#include <QVariant>

class GeoDataPointGroupCsvImporter : public GeoDataImporter
{
	Q_OBJECT

public:
	GeoDataPointGroupCsvImporter(GeoDataCreator* creator);

	const QStringList fileDialogFilters() override;
	const QStringList acceptableExtensions() override;

	bool importData(GeoData* data, int index, QWidget* w) override;
};


#endif // GEODATAPOINTGROUPCSVIMPORTER_H

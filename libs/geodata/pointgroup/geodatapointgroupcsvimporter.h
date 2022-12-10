#ifndef GEODATAPOINTGROUPCSVIMPORTER_H
#define GEODATAPOINTGROUPCSVIMPORTER_H

#include <guicore/pre/geodata/geodataimporter.h>

#include <QVariant>

class CoordinateSystemConverter;

class GeoDataPointGroupCsvImporter : public GeoDataImporter
{
	Q_OBJECT

public:
	GeoDataPointGroupCsvImporter(GeoDataCreator* creator);
	~GeoDataPointGroupCsvImporter();

	const QStringList fileDialogFilters() override;
	const QStringList acceptableExtensions() override;

	bool importData(GeoData* data, int index, QWidget* w) override;

private:
	bool doInit(const QString& filename, const QString& selectedFilter, int* count, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemInterface* item, QWidget* w) override;

	CoordinateSystemConverter* m_converter;
};


#endif // GEODATAPOINTGROUPCSVIMPORTER_H

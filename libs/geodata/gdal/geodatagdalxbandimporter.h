#ifndef GEODATAGDALXBANDIMPORTER_H
#define GEODATAGDALXBANDIMPORTER_H

#include "gd_gdal_global.h"
#include <guicore/pre/geodata/geodataimporter.h>

class GD_GDAL_EXPORT GeoDataGdalXbandImporter : public GeoDataImporter
{
	Q_OBJECT

public:
	GeoDataGdalXbandImporter(GeoDataCreator* creator);
	~GeoDataGdalXbandImporter();

	const QStringList fileDialogFilters() override;
	const QStringList acceptableExtensions() override;
	bool importData(GeoData* data, int index, QWidget* w) override;

private:
	bool doInit(const QString& filename, const QString& /*selectedFilter*/, int* count, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemI* item, QWidget* w) override;

	QString m_dirName;
	QStringList m_fileNames;
	PreProcessorGeoDataGroupDataItemI* m_groupDataItem;
};

#endif // GEODATAGDALXBANDIMPORTER_H

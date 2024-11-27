#ifndef GEODATANETCDFXBANDIMPORTER_H
#define GEODATANETCDFXBANDIMPORTER_H

#include "gd_netcdf_global.h"
#include <guicore/pre/geodata/geodataimporter.h>

class GD_NETCDF_EXPORT GeoDataNetcdfXbandImporter : public GeoDataImporter
{
	Q_OBJECT

public:
	GeoDataNetcdfXbandImporter(GeoDataCreator* creator);
	~GeoDataNetcdfXbandImporter();

	const QStringList fileDialogFilters() override;
	const QStringList acceptableExtensions() override;
	bool importData(GeoData* data, int index, QWidget* w) override;

private:
	bool doInit(const QString& filename, const QString& /*selectedFilter*/, int* count, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemI* item, QWidget* w) override;

	QString m_dirName;
	QStringList m_fileNames;
	PreProcessorGeoDataGroupDataItemI* m_groupDataItem;
};

#endif // GEODATANETCDFXBANDIMPORTER_H

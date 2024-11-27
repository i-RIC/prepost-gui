#ifndef GEODATAGDALIMPORTER_H
#define GEODATAGDALIMPORTER_H

#include "gd_gdal_global.h"
#include "geodatagdal.h"
#include <guicore/pre/geodata/geodataimporter.h>

class GD_GDAL_EXPORT GeoDataGdalImporter : public GeoDataImporter
{
	Q_OBJECT

public:
	GeoDataGdalImporter(GeoDataCreator* creator);
	virtual ~GeoDataGdalImporter();

	const QStringList fileDialogFilters() override;
	const QStringList acceptableExtensions() override;
	bool importData(GeoData* data, int index, QWidget* w) override;

protected:
	bool doInit(const QString& filename, const QString& selectedFilter, int* count, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemI* item, QWidget* w) override;
	static int ncGetVariableAsDouble(int ncid, int varid, size_t len, double* buffer);
	static int ncGetVariableAsQVariant(int ncid, int varid, size_t len, std::vector<QVariant>& list);
	static std::vector<QVariant> convertTimeValues(QString units, const std::vector<QVariant>& values, QWidget* parent, bool* canceled);

private:
	virtual int importValues(int ncid_in, int icid_out, int varIdOut, int xDimId, int yDimId, int lonDimId, int latDimId, const std::vector<int>& dimIds, GeoDataGdal* dat) = 0;

protected:
	std::vector<QString> m_dims;
	QString m_valueVariable;

	GeoDataGdal::CoordinateSystemType m_csType;

	int m_xDimId;
	int m_yDimId;
	int m_lonDimId;
	int m_latDimId;

	int m_lonVarId;
	int m_latVarId;

	PreProcessorGeoDataGroupDataItemI* m_groupDataItem;
};

#endif // GEODATAGDALIMPORTER_H

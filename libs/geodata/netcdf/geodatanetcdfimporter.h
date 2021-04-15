#ifndef GEODATANETCDFIMPORTER_H
#define GEODATANETCDFIMPORTER_H

#include "gd_netcdf_global.h"
#include "geodatanetcdf.h"
#include <guicore/pre/geodata/geodataimporter.h>

class GD_NETCDF_EXPORT GeoDataNetcdfImporter : public GeoDataImporter
{
	Q_OBJECT

public:
	GeoDataNetcdfImporter(GeoDataCreator* creator);
	virtual ~GeoDataNetcdfImporter();

	const QStringList fileDialogFilters() override;
	const QStringList acceptableExtensions() override;
	bool importData(GeoData* data, int index, QWidget* w) override;

protected:
	bool doInit(const QString& filename, const QString& selectedFilter, int* count, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemInterface* item, QWidget* w) override;
	static int ncGetVariableAsDouble(int ncid, int varid, size_t len, double* buffer);
	static int ncGetVariableAsQVariant(int ncid, int varid, size_t len, std::vector<QVariant>& list);
	static std::vector<QVariant> convertTimeValues(QString units, const std::vector<QVariant>& values, QWidget* parent, bool* canceled);

private:
	virtual int importValues(int ncid_in, int icid_out, int varIdOut, int xDimId, int yDimId, int lonDimId, int latDimId, const std::vector<int>& dimIds, GeoDataNetcdf* dat) = 0;

protected:
	std::vector<QString> m_dims;
	QString m_valueVariable;

	GeoDataNetcdf::CoordinateSystemType m_csType;

	int m_xDimId;
	int m_yDimId;
	int m_lonDimId;
	int m_latDimId;

	int m_lonVarId;
	int m_latVarId;

	PreProcessorGeoDataGroupDataItemInterface* m_groupDataItem;
};

#endif // GEODATANETCDFIMPORTER_H

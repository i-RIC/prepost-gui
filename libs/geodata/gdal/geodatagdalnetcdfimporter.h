#ifndef GEODATAGDALNETCDFIMPORTER_H
#define GEODATAGDALNETCDFIMPORTER_H

#include "gd_gdal_global.h"
#include "geodatagdal.h"
#include <guicore/pre/geodata/geodataimporter.h>

class GD_GDAL_EXPORT GeoDataGdalNetcdfImporter : public GeoDataImporter
{
	Q_OBJECT

public:
	GeoDataGdalNetcdfImporter(GeoDataCreator* creator);
	virtual ~GeoDataGdalNetcdfImporter();

	const QStringList fileDialogFilters() override;
	const QStringList acceptableExtensions() override;
	bool importData(GeoData* data, int index, QWidget* w) override;

protected:
	bool doInit(int* count, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemI* item, QWidget* w) override;
	static int ncGetVariableAsDouble(int ncid, int varid, size_t len, double* buffer);
	static int ncGetVariableAsQVariant(int ncid, int varid, size_t len, std::vector<QVariant>& list);
	static std::vector<QVariant> convertTimeValues(QString units, const std::vector<QVariant>& values, QWidget* parent, bool* canceled);

private:
	virtual int importValues(int ncid_in, int icid_out, int varIdOut, int xDimId, int yDimId, const std::vector<int>& dimIds, GeoDataGdal* dat) = 0;

protected:
	std::vector<QString> m_dims;
	QString m_valueVariable;

	int m_xDimId;
	int m_yDimId;
	int m_xVarId;
	int m_yVarId;

	PreProcessorGeoDataGroupDataItemI* m_groupDataItem;
};

#endif // GEODATAGDALNETCDFIMPORTER_H

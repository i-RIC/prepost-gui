#ifndef RAWDATANETCDFIMPORTER_H
#define RAWDATANETCDFIMPORTER_H

#include "rd_netcdf_global.h"
#include "rawdatanetcdf.h"
#include <guicore/pre/rawdata/rawdataimporter.h>

class RD_NETCDF_EXPORT RawDataNetcdfImporter : public RawDataImporter
{
	Q_OBJECT
public:
	RawDataNetcdfImporter(RawDataCreator* creator) : RawDataImporter(creator){}
	virtual ~RawDataNetcdfImporter(){}
	const QStringList fileDialogFilters();
	const QStringList acceptableExtensions();
	bool importData(RawData* data, int index, QWidget* w);

protected:
	bool doInit(const QString& filename, const QString& /*selectedFilter*/, int* count, SolverDefinitionGridRelatedCondition* condition, PreProcessorRawDataGroupDataItemInterface* item, QWidget* w);
	static int ncGetVariableAsDouble(int ncid, int varid, size_t len, double* buffer);
	static int ncGetVariableAsQVariant(int ncid, int varid, size_t len, QList<QVariant>& list);
	static QList<QVariant> convertTimeValues(QString units, QList<QVariant>& values, QWidget *parent, bool* canceled);

	virtual int importValues(int ncid_in, int icid_out, int varIdOut, int xDimId, int yDimId, int lonDimId, int latDimId, const QList<int> dimIds, RawDataNetcdf* dat) = 0;

	QList<QString> m_dims;
	QString m_valueVariable;

	RawDataNetcdf::CoordinateSystemType m_csType;

	int m_xDimId;
	int m_yDimId;
	int m_lonDimId;
	int m_latDimId;

	PreProcessorRawDataGroupDataItemInterface* m_groupDataItem;
};

#endif // RAWDATANETCDFIMPORTER_H

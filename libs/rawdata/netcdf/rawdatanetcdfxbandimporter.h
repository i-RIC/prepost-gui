#ifndef RAWDATANETCDFXBANDIMPORTER_H
#define RAWDATANETCDFXBANDIMPORTER_H

#include "rd_netcdf_global.h"
#include <guicore/pre/rawdata/rawdataimporter.h>

class RD_NETCDF_EXPORT RawDataNetcdfXbandImporter : public RawDataImporter
{
	Q_OBJECT
public:
	RawDataNetcdfXbandImporter(RawDataCreator* creator) : RawDataImporter(creator) {}
	virtual ~RawDataNetcdfXbandImporter() {}
	const QStringList fileDialogFilters();
	const QStringList acceptableExtensions() override;
	bool importData(RawData* data, int index, QWidget* w);

protected:
	bool doInit(const QString& filename, const QString& /*selectedFilter*/, int* count, SolverDefinitionGridRelatedCondition* condition, PreProcessorRawDataGroupDataItemInterface* item, QWidget* w);
	QString m_tmpFileName;

	PreProcessorRawDataGroupDataItemInterface* m_groupDataItem;
};

#endif // RAWDATANETCDFXBANDIMPORTER_H

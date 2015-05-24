#ifndef RAWDATANETCDFGDALIMPORTER_H
#define RAWDATANETCDFGDALIMPORTER_H

#include "rd_netcdf_global.h"
#include <guicore/pre/rawdata/rawdataimporter.h>

class CoordinateSystem;
class GDALDataset;
class GDALRasterBand;
class OGRSpatialReference;
class RawDataNetcdf;

class RD_NETCDF_EXPORT RawDataNetcdfGdalImporter : public RawDataImporter
{
public:
	Q_OBJECT
public:
	RawDataNetcdfGdalImporter(RawDataCreator* creator) : RawDataImporter(creator) {}
	virtual ~RawDataNetcdfGdalImporter() {}
	const QStringList fileDialogFilters();
	const QStringList acceptableExtensions() override;
	bool importData(RawData* data, int index, QWidget* w);

protected:
	bool doInit(const QString& filename, const QString& /*selectedFilter*/, int* count, SolverDefinitionGridRelatedCondition* condition, PreProcessorRawDataGroupDataItemInterface* item, QWidget* w);
	virtual int outputValues(int ncid, int varId, GDALRasterBand* band, RawDataNetcdf* data) = 0;
	GDALDataset* m_dataset;
	OGRSpatialReference* m_sr;
	double m_transform[6];
	CoordinateSystem* m_coordinateSystem;
	int m_count;
};

#endif // RAWDATANETCDFGDALIMPORTER_H

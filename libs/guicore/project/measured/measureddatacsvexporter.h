#ifndef MEASUREDDATACSVEXPORTER_H
#define MEASUREDDATACSVEXPORTER_H

#include "measureddataexporteri.h"

class MeasuredDataCsvExporter : public MeasuredDataExporterI
{
public:
	void exportData(const QString& filename, const MeasuredData& data) const override;
};

#endif // MEASUREDDATACSVEXPORTER_H

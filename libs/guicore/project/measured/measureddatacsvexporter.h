#ifndef MEASUREDDATACSVEXPORTER_H
#define MEASUREDDATACSVEXPORTER_H

#include "../../guicore_global.h"

#include "measureddataexporteri.h"

class GUICOREDLL_EXPORT MeasuredDataCsvExporter : public MeasuredDataExporterI
{
public:
	void exportData(const QString& filename, const QPointF& offset, const MeasuredData& data) const override;
};

#endif // MEASUREDDATACSVEXPORTER_H

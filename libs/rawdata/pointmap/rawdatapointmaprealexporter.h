#ifndef RAWDATAPOINTMAPREALEXPORTER_H
#define RAWDATAPOINTMAPREALEXPORTER_H

#include <guicore/pre/rawdata/rawdataexporter.h>

class RawDataPointmapRealExporter : public RawDataExporter
{
	Q_OBJECT
public:
	RawDataPointmapRealExporter(RawDataCreator* creator);
	bool doEmport(RawData* data, const QString& filename, const QString& selectedFilter, QWidget* w, ProjectData* pd);
	const QStringList fileDialogFilters();
};

#endif // RAWDATAPOINTMAPREALEXPORTER_H
